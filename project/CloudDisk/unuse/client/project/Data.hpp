#pragma once

// C++
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
// Other
#include "util.hpp"

namespace CloudBackUp 
{
	namespace Data
	{
		class DataManager
		{
		public:
			DataManager(const std::string& backupFile)
				:_backupfile(backupFile)
			{
				InitLoad();
			}

			// 读取备份信息
			bool InitLoad()
			{
				// 从文件中读取所有数据
				std::string body;
				Util::FileUtil fu(_backupfile);
				fu.GetContent(&body);

				// 数据解析添加到表中
				std::string sep = "\n";
				std::vector<std::string> lines;
				Splict(body, sep, &lines);
				for (auto& line : lines)
				{
					std::string sep = " ";
					std::vector<std::string> kv;
					Splict(line, sep, &kv);
					if (kv.size() == 2)
					{
						_data[kv[0]] = kv[1];
					}
				}

				return true;
			}

			// 新增备份信息
			bool Insert(const std::string& key, const std::string& value)
			{
				_data[key] = value;
				Storage();
				return true;
			}

			// 修改备份信息
			bool Update(const std::string& key, const std::string& value)
			{
				_data[key] = value;
				Storage();
				return true;
			}

			// 获取备份信息
			bool Get1ByKey(const std::string& key, std::string* value)
			{
				std::unordered_map<std::string, std::string>::iterator it = _data.find(key);
				if (it == _data.end())
				{
					return false;
				}
				else
				{
					*value = it->second;
					return true;
				}
			}

			// 持久化存储
			bool Storage()
			{
				// 获取所有备份信息
				std::stringstream ss;
				for (auto& e : _data)
				{
					// 将所有信息进行指定持久化格式的组织
					ss << e.first << " " << e.second << "\n";
				}

				// 将组织好的信息写入文件, 持久化存储
				Util::FileUtil fu(_backupfile);
				fu.SetContent(ss.str());

				return true;
			}

			~DataManager() = default;

		private:
			// 字符串分割函数
			int Splict(const std::string& str, const std::string& sep, std::vector<std::string>* arr)
			{
				size_t cnt = 0;
				size_t pos = 0;
				size_t index = 0;
				while (true)
				{
					pos = str.find(sep, index);
					if (pos == std::string::npos)
					{
						break;
					}
					else if(pos == index)
					{
						// 如果分隔符在开头, 则跳过
						index = pos + sep.size();
					}
					else 
					{
						std::string tmp = str.substr(index, pos - index);
						arr->push_back(std::move(tmp));
						index = pos + sep.size();
						++cnt;
					}
				}
				if (index < str.size())
				{
					arr->push_back(std::move(str.substr(index)));
					++cnt;
				}

				return cnt;
			}

		private:
			std::string _backupfile; // 备份信息的持久化存储文件
			std::unordered_map<std::string, std::string> _data; // 备份信息的内存存储
		};
	}
}