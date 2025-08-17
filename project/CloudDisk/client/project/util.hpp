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

namespace CloudBackUp
{
    namespace Util
    {
        // 文件操作实用工具类
        class FileUtil
        {
        public:
            FileUtil(const std::string& filename)
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
            bool GetPosLen(std::string* body, size_t pos, size_t len)
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
            bool GetContent(std::string* body)
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
            bool SetContent(const std::string& body)
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
            void ScanDirectory(std::vector<std::string>* arry)
            {
				CreateDirectory();
                for (auto& e : std::filesystem::directory_iterator(_filename))
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
    }
}