/*
    一些通用功能的实现：
    1.获取时间戳
    2.获取格式化的时间
    3.通过文件名从某路径下查找并获取完整文件路径
    4.从完整文件路径+文件名形式下截取文件路径
    5.从完整文件路径+文件名形式下截取文件名
    6.检测某路径下某文件的存在性
    7.创建多级目录(不会创建文件，若路径中出现文件名将会被当做目录创建)
    8.创建文件，若路径不存在会自动创建目录
    9.在文件中写入内容，可以选择覆盖写/追加写，支持使用容器

*/

#pragma once

// C++
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <optional>
#include <source_location>
// C
#include <ctime>

namespace Log
{
    namespace Util
    {
        struct Date
        {
        public:
            // 获取时间戳
            static time_t NowTime()
            {
                return time(nullptr);
            }

            // 获取格式化的时间
            static std::string FormatTime(const char *format = "%Y-%m-%d %H:%M:%S")
            {
                time_t now = time(nullptr);
                char fmtTime[64];
                strftime(fmtTime, sizeof(fmtTime) - 1, format, localtime(&now));
                return std::string(fmtTime);
            }

            // 通过已经存在的时间获取格式化的时间
            static std::string FormatExistTime(const time_t now, const char *format = "%Y-%m-%d %H:%M:%S")
            {
                struct tm t;
                localtime_r(&now, &t);
                char fmtTime[32] = {0};
                strftime(fmtTime, sizeof(fmtTime) - 1, format, &t);
                return std::string(fmtTime);
            }
        };

        struct File
        {
        public:
            static std::optional<std::filesystem::path> FilePath(
                const std::string &file_name,
                const std::filesystem::path &dir = std::filesystem::current_path(),
                size_t max_depth = 6)
            {
                try
                {
                    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir))
                    {
                        return std::nullopt;
                    }

                    auto options = std::filesystem::directory_options::skip_permission_denied;

                    for (auto it = std::filesystem::recursive_directory_iterator(dir, options);
                         it != std::filesystem::recursive_directory_iterator(); ++it)
                    {
                        if (it.depth() > max_depth)
                        {
                            it.disable_recursion_pending();
                            continue;
                        }

                        if (it->is_regular_file() && it->path().filename() == file_name)
                        {
                            return it->path(); // 找到文件，立即返回
                        }
                    }
                }
                catch (const std::filesystem::filesystem_error &e)
                {
                    if (e.code() == std::errc::no_such_file_or_directory)
                    {
                        return std::nullopt;
                    }
                }

                return std::nullopt; // 未找到文件
            }

            // 从完整文件路径+文件名形式下截取文件路径
            static std::string ExtractDirectory(const std::string &full_path)
            {
                std::filesystem::path path(full_path);
                return path.parent_path().string(); // 返回目录部分
            }

            // 从完整文件路径+文件名形式下截取文件名
            static std::string ExtractFilename(const std::string &full_path)
            {
                std::filesystem::path path(full_path);
                return path.filename().string(); // 直接获取文件名部分
            }

            // 检测某路径下某文件的存在性
            static bool IsFileExists(const std::string &file_name, const std::filesystem::path &dir = std::filesystem::current_path())
            {
                if (FilePath(file_name, dir) == std::nullopt)
                {
                    return false;
                }
                return true;
            }

            // 创建多级目录(不会创建文件，若路径中出现文件名将会被当做目录创建)
            static bool CreateDirectory(const std::string &file_path)
            {
                if (!std::filesystem::exists(file_path))
                {
                    std::filesystem::create_directories(file_path);
                    return true;
                }

                return false;
            }

            // 创建文件，若路径不存在会自动创建目录
            static bool CreateFile(const std::string &file_path, const std::string &content = "")
            {
                // 获取文件所在目录路径
                std::filesystem::path path(file_path);
                std::filesystem::path dir = path.has_parent_path() ? path.parent_path() : std::filesystem::current_path();

                // 递归创建所有缺失的目录
                if (!dir.empty() && !std::filesystem::exists(dir))
                {
                    if (!std::filesystem::create_directories(dir))
                    {
                        return false; // 目录创建失败
                    }
                }

                // 创建并写入文件
                std::ofstream file(path, std::ios::binary | std::ios::app);
                if (!file.is_open())
                {
                    return false; // 文件打开失败
                }

                if (!content.empty())
                {
                    file << content;
                }

                file.close();
                return true;
            }

            // 使用容器在文件中写入内容, 可以选择覆盖写/追加写
            template <typename Container>
            static bool WriteLines(const std::string &file_path, const Container &lines, bool append = false)
            {
                // 1. 检查并创建父目录
                if (!IsFileExists(ExtractFilename(file_path), ExtractDirectory(file_path)))
                {
                    if (!CreateFile(file_path))
                    {
                        return false; // 文件+目录创建失败
                    }
                }

                // 2. 打开文件（追加或覆盖模式）
                std::ofstream file(file_path, std::ios::binary | (append ? std::ios::app : std::ios::trunc));
                if (!file.is_open())
                {
                    return false; // 文件打开失败
                }

                // 3. 逐行写入内容
                for (const auto &line : lines)
                {
                    file << line << "\n";
                }

                // 4. 检查写入状态并返回
                if (!file.good())
                {
                    file.close();
                    return false;
                }
                else
                {
                    file.close();
                    return true;
                }
            }
        };

        // 将字符串全部转为大写, 便于比较, 以防使用者使用不规范
        std::string ToUpper(const std::string &str)
        {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(),
                           [](unsigned char c)
                           { return std::toupper(c); });
            return std::move(result);
        }

        // 获取当前文件名
        inline std::string GetFileName(const std::source_location &loc = std::source_location::current())
        {
            return loc.file_name();
        }

        // 获取当前行号
        inline size_t GetLine(const std::source_location &loc = std::source_location::current())
        {
            return loc.line();
        }
    }
}