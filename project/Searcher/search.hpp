/*
    完成 search 搜索相关功能
*/

#pragma once

// C++
#include <print>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
#include <algorithm>
// Other
#include "utils.hpp"
#include "index.hpp"
#include "parse.hpp"

namespace search
{
    struct InvertedElemSingle
    {
    public:
        uint64_t htmlId = 0;               // 网页 Id
        std::vector<std::string> keyWords; // 关键字集合
        int weight = 0;                    // 权重
    };

    class Search
    {
    public:
        Search() = default;
        ~Search() = default;

        // 初始化
        auto Init(const std::string &input = parse::rswPath) -> void
        {
            // 根据 index 对象建立索引
            _index.BuildIndex(input);
        }

        // 搜索 -- query: 搜索关键字 -- return: 搜索结果(json 字符串)
        auto Searcher(const std::string &query) -> std::string
        {
            // 分词
            std::optional<std::shared_ptr<std::vector<std::string>>> words_opt = util::JiebaUtil::CutString(query);
            if (!words_opt.has_value())
            {
                return "";
            }
            std::shared_ptr<std::vector<std::string>> words = std::move(words_opt.value());

            // 触发 -- 根据分词结果找到所有文档的倒排拉链 -- 注意要全部转为小写
            // std::vector<std::shared_ptr<Index::InvertedElem>> invertedLists; // 所有的倒排索引拉链
            std::vector<std::shared_ptr<InvertedElemSingle>> invertedLists; // 去重的所有的倒排索引拉链
            std::unordered_map<uint64_t, std::shared_ptr<InvertedElemSingle>> invertedListsMap;
            for (std::string word : *words)
            {
                boost::to_lower(word);
                std::optional<std::vector<std::shared_ptr<Index::InvertedElem>>> invertedList_opt =
                    _index.InvertedList(word);
                if (!invertedList_opt.has_value())
                {
                    std::println("find InvertedList failed: {}", word);
                    continue;
                }
                std::vector<std::shared_ptr<Index::InvertedElem>> invertedList = std::move(invertedList_opt.value());

                // 存在重复的文档，需要去重
                // invertedLists.insert(invertedLists.end(), invertedList.begin(), invertedList.end());
                for (auto &elem : invertedList)
                {
                    if (invertedListsMap.find(elem->htmlId) == invertedListsMap.end())
                    {
                        invertedListsMap[elem->htmlId] = std::make_shared<InvertedElemSingle>();
                        invertedListsMap[elem->htmlId]->htmlId = elem->htmlId;
                        invertedListsMap[elem->htmlId]->keyWords.push_back(elem->keyWord);
                        invertedListsMap[elem->htmlId]->weight = elem->weight;
                    }
                    else
                    {
                        invertedListsMap[elem->htmlId]->keyWords.push_back(elem->keyWord);
                        invertedListsMap[elem->htmlId]->weight += elem->weight;
                    }
                }
            }
            if (invertedLists.empty())
            {
                return "";
            }
            for (auto &elem : invertedListsMap)
            {
                invertedLists.push_back(elem.second);
            }

            // 合并排序 -- 按照相关性进行降序排序
            // std::sort(invertedLists.begin(), invertedLists.end(),
            //           [](const std::shared_ptr<Index::InvertedElem> &lhs, const std::shared_ptr<Index::InvertedElem> &rhs)
            //           { return lhs->weight > rhs->weight; });
            std::sort(invertedLists.begin(), invertedLists.end(),
                      [](const std::shared_ptr<InvertedElemSingle> &lhs, const std::shared_ptr<InvertedElemSingle> &rhs)
                      { return lhs->weight > rhs->weight; });

            // 构建 json -- 根据排序结果构建 json
            Json::Value root;
            for (auto &elem : invertedLists)
            {
                std::optional<std::shared_ptr<Index::HtmlInfo>> htmlInfo_opt = _index.ForwardIndex(elem->htmlId);
                if (!htmlInfo_opt.has_value())
                {
                    std::println("find ForwardIndex failed: {}", elem->htmlId);
                    continue;
                }
                std::shared_ptr<Index::HtmlInfo> htmlInfo = std::move(htmlInfo_opt.value());

                Json::Value elemJson;
                elemJson["title"] = htmlInfo->title;
                elemJson["url"] = htmlInfo->url;
                elemJson["desc"] = util::AbstractUtil::GetAbstract(htmlInfo->content, elem->keyWords[0]); // 0 号关键字作为摘要

                root.append(std::move(elemJson));
            }

            // 序列化 json
            std::string dst;
            if (!util::JsonUtil::Serialize(root, &dst))
            {
                std::println("Serialize failed");
                return "";
            }
            return dst;
        }

    private:
        inline static Index::Index &_index = Index::Index::GetInstance(); // 系统用来进行查找的索引
    };
}
