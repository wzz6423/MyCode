#pragma once

// C++
#include <string>
#include <vector>
#include <sstream>
// Other
#include "httplib.h"
#include "util.hpp"
#include "Data.hpp"
// system call
#include <Windows.h> // !注意, 这个头文件引入必须放在最后面, 否则会报错!(和 htttplib 库中引入的一些 windows 下
// 网络编程的库头文件中的宏冲突相关)

namespace CloudBackUp
{
	namespace BackUp
	{
		// 服务器地址和端口号
#define SERVER_IP "121.36.105.32"
#define SERVER_PORT 6423

// 最短上传间隔时间
#define MIN_UPLOAD_INTERVAL 9 // 9s

		class BackUp
		{
		public:
			BackUp(const std::string& back_dir, const std::string& backFile)
				: _back_dir(back_dir), _data(new Data::DataManager(backFile))
			{
			}

			~BackUp() = default;

			// 获取文件唯一标识
			std::string GetFileID(const std::string& fileName)
			{
				// file name-file size-last modify time
				Util::FileUtil fu(fileName);
				std::stringstream ss;
				std::optional<int64_t> ofileSize = fu.FileSize();
				if (!ofileSize.has_value())
				{
					std::cerr << "file size error!" << std::endl;
					ss << fu.Filename() << "-" << "unknown" << "-" << fu.LastModifyTime();
					return std::move(ss.str());
				}
				else {
					ss << fu.Filename() << "-" << ofileSize.value() << "-" << fu.LastModifyTime();
					return std::move(ss.str());
				}
			}

			// 判断文件是否需要上传
			bool NeedUpload(const std::string& fileName)
			{
				// 文件新增 / 修改 --> 上传
				// 新增: 是否存在历史备份信息
				// 修改: 有历史备份信息 + 文件唯一标识是否一致
				std::string oldfileID;
				if (_data->Get1ByKey(fileName, &oldfileID) != false)
				{
					// 文件存在历史备份信息
					if (oldfileID == GetFileID(fileName))
					{
						// 文件唯一标识一致，不需要上传
						return false;
					}
				}

				// 文件体积较大, 拷贝较慢, 一直变换、一直上传不合理, 故需要一段时间内不变化才上传 -- 该文件是否正在被使用
				Util::FileUtil fu(fileName);
				if (time(nullptr) - fu.LastModifyTime() < MIN_UPLOAD_INTERVAL)
				{
					// 文件正在被使用, 不需要上传
					return false;
				}

				// 文件唯一标识不一致 / 文件不存在历史备份信息，需要上传
				return true;
			}

			// 文件上传
			bool Upload(const std::string& fileName)
			{
				// 获取文件数据
				Util::FileUtil fu(fileName);
				std::string body;
				fu.GetContent(&body);

				// 搭建 http 客户端上传文件数据
				httplib::Client client(SERVER_IP, SERVER_PORT);
				httplib::MultipartFormData item;
				item.content = std::move(body);
				item.filename = std::move(fu.Filename());
				item.name = "file";
				item.content_type = "application/octet-stream";

				httplib::MultipartFormDataItems items;
				items.push_back(std::move(item));
				auto res = client.Post("/upload", items);
				if (!res || res->status != 200)
				{
					std::cerr << "post error!" << std::endl;
					return false;
				}

				return true;
			}

			bool Run()
			{
				while (true)
				{
					// 遍历获取指定文件夹中所有文件
					Util::FileUtil fu(_back_dir);
					std::vector<std::string> files;
					fu.ScanDirectory(&files);

					// 逐个判断文件是否需要上传
					for (const auto& file : files)
					{
						if (NeedUpload(file) == true)
						{
							// 需要上传则上传文件进行备份
							if (Upload(file))
							{
								// 上传成功则更新历史备份信息
								_data->Insert(file, std::move(GetFileID(file)));
							}
							else
							{
								std::cerr << "upload error!" << std::endl;
							}
						}
					}

					Sleep(3000); // 3s
				}
			}

		private:
			std::string _back_dir; // 备份目录
			Data::DataManager* _data; // 数据管理器
		};
	}
}