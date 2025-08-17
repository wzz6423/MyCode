/*
    建立索引相关代码
    正排索引 -- 文档 id 到文档内容、文档 url 的映射
    倒排索引 -- 文档内容分词后, 词到文档 id 的映射
*/

#pragma once

// C++
#include <print>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
// Other
#include "utils.hpp"
#include "parse.hpp"

namespace Index
{
    struct HtmlInfo
    {
    public:
        std::string title;   // 网页标题
        std::string content; // 网页去标签正文
        std::string url;     // 官网 url
        uint64_t htmlId = 0; // 网页 Id
    };

    struct InvertedElem
    {
    public:
        uint64_t htmlId = 0; // 网页 Id
        std::string keyWord; // 关键字
        int weight = 0;      // 权重
    };

    class Index
    {
    public:
        // 单例模式
        static auto GetInstance() -> Index &
        {
            static Index index;
            return index;
        }

        // 根据 htmlId 获取正排索引
        auto ForwardIndex(const uint64_t htmlId) const -> std::optional<std::shared_ptr<HtmlInfo>>
        {
            if (htmlId >= _forwardIndex.size())
            {
                std::println("ForwardIndex failed : htmlId out range: {}", htmlId);
                return std::nullopt;
            }
            return make_optional(_forwardIndex[htmlId]);
        }

        // 根据关键字获取倒排索引拉链
        auto InvertedList(const std::string &keyWord) const -> std::optional<std::vector<std::shared_ptr<InvertedElem>>>
        {
            auto it = _invertedIndex.find(keyWord);
            if (it == _invertedIndex.end())
            {
                std::println("InvertedList failed : keyWord not found: {}", keyWord);
                return std::nullopt;
            }
            return make_optional(it->second);
        }

        // 根据去标签处理完毕的数据构建正排 & 倒排索引 -- data/rawHtml/raw.txt
        auto BuildIndex(const std::string &rawPath = parse::rswPath) -> bool
        {
            // 打开解析好的文件并进行按行读取
            std::ifstream ifs(rawPath, std::ios::in | std::ios::binary);
            if (!ifs.is_open())
            {
                std::println("BuildIndex failed : open file failed: {}", rawPath);
                return false;
            }

            // 按行读取文件内容
            int lineCount = 0;
            int success = 0;
            std::string line;
            while (std::getline(ifs, line))
            {
                ++lineCount;
                // 建立正排索引
                std::optional<std::shared_ptr<HtmlInfo>> info_opt = buildForwardIndex(line);
                if (!info_opt.has_value())
                {
                    std::println("BuildIndex failed : buildForwardIndex failed: {}", line);
                    continue;
                }
                std::shared_ptr<HtmlInfo> info = std::move(info_opt.value());

                // 建立倒排索引
                if (!buildInvertedIndex(info))
                {
                    std::println("BuildIndex failed : buildInvertedIndex failed: {}", line);
                    continue;
                }
                ++success;
            }

            if (success == 0)
            {
                std::println("BuildIndex failed : no success");
                return false;
            }

            if (success != lineCount)
            {
                std::println("BuildIndex error : success not equal lineCount: {} != {}", success, lineCount);
            }

            return true;
        }

    private:
        // 建立正排索引 -- 返回值不是 bool 是因为建立倒排索引需要用到 htmlId
        auto buildForwardIndex(const std::string &line) -> std::optional<std::shared_ptr<HtmlInfo>>
        {
            // 解析 line, 获取 title, content, url 并进行填充
            std::shared_ptr<HtmlInfo> info = std::make_shared<HtmlInfo>();
            info->htmlId = _forwardIndex.size();
            std::optional<std::vector<std::string>> result_opt = util::StringUtil::Split(line, parse::contentSeparator);
            if (!result_opt.has_value())
            {
                std::println("buildForwardIndex failed : split failed: {}", line);
                return std::nullopt;
            }
            std::vector<std::string> result = std::move(result_opt.value());
            if (result.size() != 3)
            {
                std::println("buildForwardIndex failed : result size not 3: {}", line);
                return std::nullopt;
            }
            info->title = std::move(result[0]);
            info->content = std::move(result[1]);
            info->url = std::move(result[2]);

            // 插入信息到正排索引
            _forwardIndex.push_back(info);

            return make_optional(info);
        }

        // 建立倒排索引
        auto buildInvertedIndex(const std::shared_ptr<HtmlInfo> &info) -> bool
        {
            // 分词
            util::JiebaUtil jieba;
            // 定义词频统计结构
            struct wordCount
            {
            public:
                int titleCount = 0;
                int contentCount = 0;
            };
            // 存储词频映射表
            std::unordered_map<std::string, wordCount> wordMap;

            // title
            std::optional<std::shared_ptr<std::vector<std::string>>> titleWords_opt = jieba.CutString(info->content);
            if (!titleWords_opt.has_value())
            {
                std::println("buildInvertedIndex failed : cut title failed: {}", info->title);
                return false;
            }
            std::shared_ptr<std::vector<std::string>> titleWords = std::move(titleWords_opt.value());
            // content
            std::optional<std::shared_ptr<std::vector<std::string>>> contentWords_opt = jieba.CutString(info->content);
            if (!contentWords_opt.has_value())
            {
                std::println("buildInvertedIndex failed : cut content failed: {}", info->content);
                return false;
            }
            std::shared_ptr<std::vector<std::string>> contentWords = std::move(contentWords_opt.value());

            // 词频统计
            // 用户搜索时也要转为小写
            for (std::string word : *titleWords)
            {
                boost::to_lower(word); // 分词转为小写
                wordMap[word].titleCount++;
            }
            for (std::string word : *contentWords)
            {
                boost::to_lower(word); // 分词转为小写
                wordMap[word].contentCount++;
            }

            // 构建倒排拉链节点并保存
            for (const auto &word : wordMap)
            {
                std::shared_ptr<InvertedElem> elem = std::make_shared<InvertedElem>();
                elem->htmlId = info->htmlId;
                elem->keyWord = word.first;
                elem->weight = word.second.titleCount * titleWeight + word.second.contentCount * contentWeight;
                _invertedIndex[word.first].push_back(elem);
            }

            return true;
        }

        // 单例模式
        Index(const Index &) = delete;
        Index &operator=(const Index &) = delete;
        Index(Index &&) = delete;
        Index &operator=(Index &&) = delete;

    private:
        Index() = default;
        ~Index() = default;

    private:
        // 正排索引数据结构为 vector, 下标即为文档 id -- 时间复杂度: O(1)
        std::vector<std::shared_ptr<HtmlInfo>> _forwardIndex; // 正排索引
        // 倒排索引 -- 关键字与一组(倒排拉链) InvertedElem 的映射关系
        std::unordered_map<std::string, std::vector<std::shared_ptr<InvertedElem>>> _invertedIndex; // 倒排索引

        int titleWeight = 10;  // 标题权重
        int contentWeight = 1; // 正文权重
    };
}