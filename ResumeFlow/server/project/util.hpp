#pragma once

// C++
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include <memory>
// C
#include <cstdio>
// system call
#include <sys/stat.h>
// Other
#include <jsoncpp/json/json.h>
#include "./bundle/bundle.h"

namespace CloudBackUp
{
    namespace Util
    {
        // 转换时间实用函数
        // 通过已经存在的时间获取格式化的时间
        static std::string FormatExistTime(const time_t now, const char *format = "%Y-%m-%d %H:%M:%S")
        {
            struct tm t;
            localtime_r(&now, &t);
            char fmtTime[32] = {0};
            strftime(fmtTime, sizeof(fmtTime) - 1, format, &t);
            return std::string(fmtTime);
        }

        // 文件操作实用工具类
        class FileUtil
        {
        public:
            FileUtil(const std::string &filename)
                : _filename(filename)
            {
            }

            // 获取文件大小
            std::optional<int64_t> FileSize()
            {
                struct stat st;
                if (stat(_filename.c_str(), &st) < 0)
                {
                    std::cerr << "get file size failed!" << std::endl;
                    return std::nullopt;
                }
                else
                {
                    return st.st_size;
                }
            }

            // 获取该文件最后一次修改时间
            time_t LastModifyTime()
            {
                struct stat st;
                if (stat(_filename.c_str(), &st) < 0)
                {
                    std::cerr << "get file size failed!" << std::endl;
                    return -1;
                }
                else
                {
                    return st.st_mtime;
                }
            }

            // 获取该文件最后一次访问时间
            time_t LastAccessTime()
            {
                struct stat st;
                if (stat(_filename.c_str(), &st) < 0)
                {
                    std::cerr << "get file size failed!" << std::endl;
                    return -1;
                }
                else
                {
                    return st.st_atime;
                }
            }

            // 获取文件名
            std::string Filename()
            {
                std::filesystem::path path(_filename);
                return std::move(path.filename().string()); // 直接获取文件名部分
            }

            // 获取文件部分数据
            bool GetPosLen(std::string *body, size_t pos, size_t len)
            {
                std::ifstream ifs(_filename.c_str(), std::ios::binary);
                if (ifs.is_open() == false)
                {
                    std::cerr << "read open file failed!" << std::endl;
                    return false;
                }

                int64_t filesize;
                std::optional<int64_t> ofielsize = FileSize();
                if (ofielsize)
                {
                    filesize = ofielsize.value();
                }
                else
                {
                    std::cerr << "get filesize error!" << std::endl;
                    ifs.close();
                    return false;
                }

                if (filesize - pos > len)
                {
                    std::cerr << "request file size is too big!" << std::endl;
                    ifs.close();
                    return false;
                }

                ifs.seekg(pos, std::ios::beg);
                body->resize(len);
                ifs.read(&((*body)[0]), len);
                if (ifs.good() == false)
                {
                    std::cerr << "read file failed!" << std::endl;
                    ifs.close();
                    return false;
                }

                ifs.close();
                return true;
            }

            // 获取文件全部数据
            bool GetContent(std::string *body)
            {
                int64_t filesize;
                std::optional<int64_t> ofielsize = FileSize();
                if (ofielsize)
                {
                    filesize = ofielsize.value();
                }
                else
                {
                    std::cerr << "get filesize error!" << std::endl;
                    return false;
                }

                return GetPosLen(body, 0, filesize);
            }

            // 设置文件数据
            bool SetContent(const std::string &body)
            {
                std::ofstream ofs(_filename, std::ios::binary);
                if (ofs.is_open() == false)
                {
                    std::cerr << "write open file failed!" << std::endl;
                    return false;
                }

                ofs.write(&body[0], body.size());
                if (ofs.good() == false)
                {
                    std::cerr << "write file failed!" << std::endl;
                    ofs.close();
                    return false;
                }

                ofs.close();
                return true;
            }

            // 压缩
            bool Compress(const std::string &packageName)
            {
                // 获取源文件数据
                std::string body;
                if (GetContent(&body) == false)
                {
                    std::cerr << "compress get file content failed!" << std::endl;
                    return false;
                }

                // 对数据进行压缩
                std::string packed = bundle::pack(bundle::LZIP, body);

                // 压缩的数据存放在压缩包文件中
                FileUtil packedFile(packageName);
                if (packedFile.SetContent(packed) == false)
                {
                    std::cerr << "compress write packed file failed!" << std::endl;
                    return false;
                }
                return true;
            }

            // 解压缩
            bool UnCompress(const std::string &filename)
            {
                // 读取出当前压缩包数据
                std::string body;
                if (GetContent(&body) == false)
                {
                    std::cerr << "uncompress get file content failed!" << std::endl;
                    return false;
                }

                // 解压缩
                std::string unpacked = bundle::unpack(body);

                // 写入文件
                FileUtil packedFile(filename);
                if (packedFile.SetContent(unpacked) == false)
                {
                    std::cerr << "uncompress write packed file failed!" << std::endl;
                    return false;
                }
                return true;
            }

            // 判断文件是否存在
            bool Exists()
            {
                return std::filesystem::exists(_filename);
            }

            // 创建目录
            bool CreateDirectory()
            {
                if (Exists())
                {
                    return true;
                }
                return std::filesystem::create_directories(_filename);
            }

            // 浏览 / 遍历 目录
            void ScanDirectory(std::vector<std::string> *arry)
            {
                CreateDirectory();
                for (auto &e : std::filesystem::directory_iterator(_filename))
                {
                    if (!std::filesystem::is_directory(e))
                    {
                        // relative_path : 相对路径 + 文件名
                        arry->push_back(std::move(std::filesystem::path(e).relative_path().string()));
                    }
                }
            }

            // 删
            bool Remove()
            {
                if (Exists() == false)
                {
                    return false;
                }
                remove(_filename.c_str());
                return true;
            }

        private:
            std::string _filename; // 文件路径 + 文件名
        };

        // Json 操作实用工具类
        class JsonUtil
        {
        public:
            // 把 Json 格式数据序列化为 string
            static bool Serialize(const Json::Value &root, std::string *str)
            {
                Json::StreamWriterBuilder swb;
                std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
                std::stringstream ss;
                bool ret = sw->write(root, &ss);
                if (ret != 0)
                {
                    std::cerr << "Json write error!" << std::endl;
                    return false;
                }
                *str = std::move(ss.str());

                return true;
            }

            // 把 string 数据反序列化为 Json
            static bool UnSerialize(const std::string &str, Json::Value *root)
            {
                Json::CharReaderBuilder crb;
                std::unique_ptr<Json::CharReader> cb(crb.newCharReader());
                std::string err;
                bool ret = cb->parse(str.c_str(), str.c_str() + str.size(), root, &err);
                if (ret == -1)
                {
                    std::cerr << "Json parse error!" << std::endl;
                    return false;
                }

                return true;
            }
        };
    }
}