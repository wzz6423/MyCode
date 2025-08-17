/*
    实用工具
*/

#pragma once

// C++
#include <iostream>
#include <fstream>
#include <string>
#include <print>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>
// syscall
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
// boost
#include <boost/algorithm/string.hpp>
// jieba
#include "cppjieba/include/cppjieba/Jieba.hpp"
// json
#include <jsoncpp/json/json.h>

namespace util
{
    // 文件操作
    class FileUtil
    {
    public:
        static auto ReadFile(const std::string &filePath, std::string *out) -> bool
        {
            // v1 - 效率低下, 不断 getline 效率太差了
            // std::ifstream ifs(filePath, std::ios::in);
            // if (!ifs.is_open())
            // {
            //     std::println("open file failed: {}", filePath);
            //     return false;
            // }
            // std::string line;
            // while(std::getline(ifs, line))
            // {
            //     *out += line;
            // }

            // v2 - 优化效率
            // // 以二进制模式打开并直接定位到文件末尾
            // std::ifstream ifs(filePath, std::ios::binary | std::ios::ate);
            // if (!ifs.is_open())
            // {
            //     std::println("open file failed: {}", filePath);
            //     return false;
            // }
            // // 获取文件大小
            // const auto fileSize = ifs.tellg();
            // if (fileSize == -1)
            // {
            //     std::println("get file size failed: {}", filePath);
            //     return false;
            // }
            // // 处理空文件情况
            // if (fileSize == 0)
            // {
            //     out->clear(); // 确保输出为空
            //     return true;
            // }
            // // 回到文件开头
            // ifs.seekg(0);
            // // 重置输出字符串并预分配空间
            // out->clear();
            // out->resize(static_cast<size_t>(fileSize));
            // // 一次性读取整个文件内容
            // if (!ifs.read(out->data(), fileSize))
            // {
            //     std::println("read file failed: {}", filePath);
            //     out->clear(); // 失败时清空输出
            //     return false;
            // }

            // v3 - 最高效读大文件处理方式 -- 使用 mmap 文件内存映射
            int fd = open(filePath.c_str(), O_RDONLY);
            if (fd == -1)
            {
                std::println("open failed");
                return false;
            }

            struct stat sb;
            if (fstat(fd, &sb) == -1)
            {
                std::println("fstat failed");
                return false;
            }

            size_t size = sb.st_size;
            char *addr = static_cast<char *>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
            close(fd); // 文件描述符可立即关闭
            if (addr == MAP_FAILED)
            {
                std::println("mmap failed");
                return false;
            }

            // 直接构造字符串(避免复制)
            *out = std::move(std::string(addr, size));
            // 解除内存映射关系, 释放资源
            munmap(addr, size);

            return true;
        }
    };

    // 字符串切分
    class StringUtil
    {
    public:
        // 字符串切分 -- 使用 boost 库提供的方法
        static auto Split(const std::string &str, const std::string &sep) -> std::optional<std::vector<std::string>>
        {
            std::vector<std::string> result;
            boost::split(result, str, boost::is_any_of(sep), boost::token_compress_on);
            if (result.empty())
            {
                std::println("split failed");
                return std::nullopt;
            }
            return std::move(result);
        }
    };

    // 分词
    class JiebaUtil
    {
    public:
        // JiebaUtil() = default;
        JiebaUtil(const JiebaUtil &) = delete;
        JiebaUtil &operator=(const JiebaUtil &) = delete;
        JiebaUtil(JiebaUtil &&) = delete;
        JiebaUtil &operator=(JiebaUtil &&) = delete;
        ~JiebaUtil() = default;

        static auto GetInstance() -> JiebaUtil &
        {
            static JiebaUtil instance;
            return instance;
        }

        auto Init() -> void
        {
            std::ifstream ifs(stop_word_path);
            if (!ifs.is_open())
            {
                std::println("open stop word file failed");
                return;
            }
            std::string line;
            while (std::getline(ifs, line))
            {
                _stopWords[line] = true;
            }

            ifs.close();
        }

        static auto CutString(const std::string &str) -> std::optional<std::shared_ptr<std::vector<std::string>>>
        {
            return GetInstance().cutStringHelper(str);
        }

        // static auto CutString(const std::string &str) -> std::optional<std::shared_ptr<std::vector<std::string>>>
        // {
        //     std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
        //     _jieba.CutForSearch(str, *result);
        //     if (result->empty())
        //     {
        //         std::println("cut failed");
        //         return std::nullopt;
        //     }
        //     return make_optional(result);
        // }

    private:
        JiebaUtil()
            : _jieba(dict_path, model_path, user_dict_path, idf_path, stop_word_path)
        {
        }

        auto cutStringHelper(const std::string &str) -> std::optional<std::shared_ptr<std::vector<std::string>>>
        {
            std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
            _jieba.CutForSearch(str, *result);
            if (result->empty())
            {
                std::println("cut failed");
                return std::nullopt;
            }

            for (auto it = result->begin(); it != result->end();)
            {
                auto word = _stopWords.find(*it);
                if (word != _stopWords.end())
                {
                    // 删除暂停词
                    it = result->erase(it);
                }
                else
                {
                    ++it;
                }
            }

            return make_optional(result);
        }

    private:
        inline static const std::string dict_path = "./cppjieba/dict/jieba.dict.utf8";
        inline static const std::string model_path = "./cppjieba/dict/hmm_model.utf8";
        inline static const std::string user_dict_path = "./cppjieba/dict/user.dict.utf8";
        inline static const std::string idf_path = "./cppjieba/dict/idf.utf8";
        inline static const std::string stop_word_path = "./cppjieba/dict/stop_words.utf8";

    private:
        // inline static cppjieba::Jieba _jieba = cppjieba::Jieba(dict_path, model_path, user_dict_path, idf_path, stop_word_path);
        cppjieba::Jieba _jieba;
        std::unordered_map<std::string, bool> _stopWords; // 暂停词
    };

    class JsonUtil
    {
    public:
        static auto Serialize(const Json::Value &val, std::string *dst) -> bool
        {
            // 先定义 Json::StreamWriter 工厂类 Json::StreamWriterBuilder
            Json::StreamWriterBuilder swb;
            std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());

            // 通过 Json::StreamWriter 中的 write 接口进行序列化
            std::stringstream ss;
            int ret = sw->write(val, &ss);
            if (ret == 0)
            {
                *dst = ss.str();
                return true;
            }
            else
            {
                std::println("Json::StreamWriter write failed");
                return false;
            }
        }
    };

    // 获取摘要
    class AbstractUtil
    {
    public:
        static auto GetAbstract(const std::string &content, const std::string &keyWord) -> std::string
        {
            // 找到 keyWord 在 content 中首次出现的位置, 然后往前找 50 个字节, 往后找 100 个字节作为摘要
            // 大小写区分的问题
            size_t pos = std::search(content.begin(), content.end(), keyWord.begin(), keyWord.end(),
                                     [](int x, int y)
                                     { return (std::tolower(x) == std::tolower(y)); }) -
                         content.begin();
            if (pos == content.size())
            {
                return "null";
            }

            // 确定切分部分的起始和结束位置
            size_t start = 0;
            size_t end = content.size() - 1;
            if (pos > _prev)
            {
                start = pos - _prev;
            }
            if (pos + _next < content.size())
            {
                end = pos + _next;
            }

            // 返回摘要
            if (start >= end)
            {
                return "null";
            }
            return content.substr(start, end - start) + "...";
        }

    private:
        AbstractUtil() = default;
        ~AbstractUtil() = default;

        inline static const size_t _prev = 50;
        inline static const size_t _next = 100;
    };
}