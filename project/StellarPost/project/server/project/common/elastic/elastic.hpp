#pragma once

// C++
#include <string>
#include <vector>
#include <memory>
#include <optional>
// es
#include <elasticlient/client.h>
#include <cpr/cpr.h>
// json
#include "../json/json.hpp" // jsoncpp 模块封装
// Other
#include "../log/logger.hpp" // 日志器 spdlog 模块封装

/*
    -lcpr -lelasticlient
*/

namespace stellar_post
{
    namespace Elastic
    {
        // 索引
        class EsIndex
        {
        public:
            EsIndex(const std::string &name,
                    const std::shared_ptr<elasticlient::Client> client, // 注意: 构造 es 客户端, ip:port 最后的 / 相对根目录是必须有的
                    const std::string &type = "_doc")
                : _name(name),
                  _type(type),
                  _client(client)
            {
                Json::Value analysis;
                Json::Value analyzer;
                Json::Value ik;
                Json::Value tokenizer;

                tokenizer["tokenizer"] = "ik_max_word";
                ik["ik"] = tokenizer;
                analyzer["ik"] = ik;
                analysis["analysis"] = analyzer;
                _index["analysis"] = std::move(analysis);
            }

            // 添加字段
            auto Add(const std::string &key,                      // 索引下字段名
                     const std::string &type = "text",            // 字段类型
                     const std::string &analyzer = "ik_max_word", // 分词器类型
                     bool enabled = true)                         // 是否开启分词器
                -> EsIndex &
            {
                Json::Value fields;

                fields["type"] = type;
                fields["analyzer"] = analyzer;
                if (enabled == false)
                {
                    fields["enabled"] = enabled;
                }

                _properties[key] = std::move(fields);
                return *this;
            }

            // 创建索引
            auto Create(const std::string &indexId = "defaultIndexId") -> bool
            {
                Json::Value mappings;
                mappings["dynamic"] = true;
                mappings["properties"] = _properties;

                _index["mappings"] = std::move(mappings);

                std::string jsonIndex;
                bool ret = json::Serialize(_index, &jsonIndex);
                if (ret == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "index serialize error!");
                    return false;
                }
                else
                {
                    // 要进行异常捕获, 防止进程崩溃
                    try
                    {
                        // 发起请求
                        auto rsp = _client->index(_name, _type, indexId, jsonIndex);
                        if (rsp.status_code < 200 || rsp.status_code >= 300)
                        {
                            Log::lg("error", Log::FileName(), Log::Line(),
                                    "es request error when create index: {} ! status code: {}, status text: {}", _name, rsp.status_code, rsp.status_line);
                            return false;
                        }
                    }
                    catch (std::exception &e)
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "es request error when create index: {} ! {}", _name, e.what());
                        return false;
                    }
                }
                Log::lg("debug", Log::FileName(), Log::Line(), "create index success!");
                return true;
            }

        private:
            std::string _name;                             // 索引名称
            std::string _type;                             // 索引类型
            Json::Value _properties;                       // 每次单独 Add 使用的 Json::Value
            Json::Value _index;                            // 将要构建的索引请求语句组织为 json 格式 --> 组织为字符串
            std::shared_ptr<elasticlient::Client> _client; // es client
        };

        // 新增数据
        class EsInsert
        {
        public:
            EsInsert(const std::string &name,
                     const std::shared_ptr<elasticlient::Client> client, // 注意: 构造 es 客户端, ip:port 最后的 / 相对根目录是必须有的
                     const std::string &type = "_doc")
                : _name(name),
                  _type(type),
                  _client(client)
            {
            }

            // 写函数模板没有 auto 好用
            auto Add(const std::string &key, const auto &val) -> EsInsert &
            {
                _item[key] = val;
                return *this;
            }

            auto Insert(const std::string &id = "") -> bool
            {
                std::string jsonItem;
                bool ret = json::Serialize(_item, &jsonItem);
                if (ret == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "item serialize error!");
                    return false;
                }
                else
                {
                    // 要进行异常捕获, 防止进程崩溃
                    try
                    {
                        // 发起请求
                        auto rsp = _client->index(_name, _type, id, jsonItem);
                        if (rsp.status_code < 200 || rsp.status_code >= 300)
                        {
                            Log::lg("error", Log::FileName(), Log::Line(),
                                    "es request error when add data: {}-{} !", _name, jsonItem);
                            return false;
                        }
                    }
                    catch (std::exception &e)
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "es request error when add data: {}-{} ! {}", _name, jsonItem, e.what());
                        return false;
                    }
                }
                Log::lg("debug", Log::FileName(), Log::Line(), "add data success!");
                return true;
            }

        private:
            std::string _name;                             // 索引名称
            std::string _type;                             // 索引类型
            Json::Value _item;                             // 新增数据的 Json::Value
            std::shared_ptr<elasticlient::Client> _client; // es client
        };

        // 删除
        class EsRemove
        {
        public:
            EsRemove(const std::string &name,
                     const std::shared_ptr<elasticlient::Client> client, // 注意: 构造 es 客户端, ip:port 最后的 / 相对根目录是必须有的
                     const std::string &type = "_doc")
                : _name(name),
                  _type(type),
                  _client(client)
            {
            }

            auto Remove(const std::string &id) -> bool
            {
                // 要进行异常捕获, 防止进程崩溃
                try
                {
                    // 发起请求
                    auto rsp = _client->remove(_name, _type, id);
                    if (rsp.status_code < 200 || rsp.status_code >= 300)
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "es request error when remove data: {}-{} !", _name, id);
                        return false;
                    }
                }
                catch (std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "es request error when remove data: {}-{} ! {}", _name, id, e.what());
                    return false;
                }
                Log::lg("debug", Log::FileName(), Log::Line(), "remove data success!");
                return true;
            }

        private:
            std::string _name;                             // 索引名称
            std::string _type;                             // 索引类型
            std::shared_ptr<elasticlient::Client> _client; // es client
        };

        // 搜索
        class EsSerach
        {
        public:
            EsSerach(const std::string &name,
                     const std::shared_ptr<elasticlient::Client> client, // 注意: 构造 es 客户端, ip:port 最后的 / 相对根目录是必须有的
                     const std::string &type = "_doc")
                : _name(name),
                  _type(type),
                  _client(client)
            {
            }

            // 不匹配哪些值 -- 精确匹配
            auto MustNotTerms(const std::string &key, const std::vector<std::string> &vals) -> EsSerach &
            {
                Json::Value terms;
                Json::Value fields;
                for (const auto &val : vals)
                {
                    fields[key].append(val);
                }
                terms["terms"] = fields;

                _mustNot.append(std::move(terms));
                return *this;
            }

            // 不完全匹配哪些值 -- 分词匹配
            auto AddShouldMatch(const std::string &key, const std::string &val) -> EsSerach &
            {
                Json::Value match;
                Json::Value field;
                field[key] = val;
                match["match"] = std::move(field);

                _should.append(std::move(match));
                return *this;
            }

            // 完全匹配哪些值 -- 不分词匹配
            auto AddMustTerm(const std::string &key, const std::string &val) -> EsSerach &
            {
                Json::Value term;
                Json::Value field;
                field[key] = val;
                term["term"] = std::move(field);

                _must.append(std::move(term));
                return *this;
            }

            // 完全匹配哪些值 -- 分词匹配
            auto AddMustMatch(const std::string &key, const std::string &val) -> EsSerach &
            {
                Json::Value match;
                Json::Value field;
                field[key] = val;
                match["match"] = std::move(field);

                _must.append(std::move(match));
                return *this;
            }

            auto Search() -> std::optional<Json::Value>
            {
                Json::Value cond;
                if (_mustNot.empty() == false)
                {
                    cond["must_not"] = std::move(_mustNot);
                }
                if (_should.empty() == false)
                {
                    cond["should"] = std::move(_should);
                }
                if (_must.empty() == false)
                {
                    cond["must"] = std::move(_must);
                }
                Json::Value query;
                query["bool"] = std::move(cond);
                Json::Value root;
                root["query"] = std::move(query);

                std::string jsonSearch;
                bool ret = json::Serialize(root, &jsonSearch);
                cpr::Response rsp;
                if (ret == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "item serialize error!");
                    return std::nullopt;
                }
                else
                {
                    // 要进行异常捕获, 防止进程崩溃
                    try
                    {
                        // 发起请求
                        rsp = _client->search(_name, _type, jsonSearch);
                        if (rsp.status_code < 200 || rsp.status_code >= 300)
                        {
                            Log::lg("error", Log::FileName(), Log::Line(),
                                    "es request error when search data: {}-{} !", _name, jsonSearch);
                            return std::nullopt;
                        }
                    }
                    catch (std::exception &e)
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "es request error when search data: {}-{} ! {}", _name, jsonSearch, e.what());
                        return std::nullopt;
                    }
                }

                // 反序列化响应正文
                Json::Value jsonResponse;
                ret = json::UnSerialize(rsp.text, &jsonResponse);
                if (ret == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "unserialize {} error!", rsp.text);
                    return std::nullopt;
                }
                Log::lg("debug", Log::FileName(), Log::Line(), "search data success!");
                return jsonResponse["hits"]["hits"];
            }

        private:
            std::string _name;                             // 索引名称
            std::string _type;                             // 索引类型
            std::shared_ptr<elasticlient::Client> _client; // es client
            Json::Value _mustNot;                          // 必须不遵循的条件
            Json::Value _should;                           // 必须遵循的条件(遵循其中之一)
            Json::Value _must;                             // 必须遵循的条件(每一条都遵循)
        };
    }
}