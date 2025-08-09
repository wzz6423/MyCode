/*
    实现 user 微服务对 elasticsearch 相关操作的代码
*/

#pragma once

// C++
#include <string>
#include <vector>
#include <memory>
#include <optional>
// elastic search
#include "operator.hpp"
#include "../elastic/elastic.hpp"
// Other
#include "../log/logger.hpp" // 日志器 spdlog 模块封装
#include "../odb/user.hxx"

namespace stellar_post
{
    namespace user
    {
        class ESUser
        {
        public:
            using esUserPtr = std::shared_ptr<ESUser>;

        public:
            ESUser(const std::shared_ptr<elasticlient::Client> &client)
                : _client(client)
            {
            }

            // 创建索引
            auto CreateIndex() -> bool
            {
                stellar_post::Elastic::EsIndex index("user", _client);
                index.Add("user_id", "keyword", "standard", true);
                index.Add("nickName");
                index.Add("phone", "keyword", "standard", true);
                index.Add("description", "text", "standard", false);
                index.Add("avatar_id", "keyword", "standard", false);
                bool ret = index.Create();
                if (!ret)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "user index create failed!");
                    return false;
                }
                return true;
            }

            // 新增数据
            auto Add(const std::string &uid,
                     const std::string &phone,
                     const std::string &nickName,
                     const std::string &description,
                     const std::string &avatar_id) -> bool
            {
                bool ret = Elastic::EsInsert("user", _client)
                               .Add("user_id", uid)
                               .Add("nickName", nickName)
                               .Add("phone", phone)
                               .Add("description", description)
                               .Add("avatar_id", avatar_id)
                               .Insert(uid);
                if (!ret)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "add or update user data error!");
                    return false;
                }
                return true;
            }

            // 搜索
            auto Search(const std::string &key,
                        const std::vector<std::string> &uidList)
                -> std::vector<std::shared_ptr<User>>
            {
                std::vector<std::shared_ptr<User>> userList;
                Json::Value user;
                std::optional<Json::Value> oUser = Elastic::EsSerach("user", _client)
                                                       .AddShouldMatch("phone.keyword", key)
                                                       .AddShouldMatch("user_id.keyword", key)
                                                       .AddShouldMatch("nickName", key)
                                                       .MustNotTerms("user_id.keyword", uidList)
                                                       .Search();
                if (!oUser)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "search data error!");
                    return userList;
                }
                else
                {
                    user = oUser.value();
                    if (user.isArray() == false)
                    {
                        Log::lg("error", Log::FileName(), Log::Line(), "search data error! it is not an arr!");
                        return userList;
                    }
                }

                for (int i = 0; i < user.size(); ++i)
                {
                    std::shared_ptr<User> u = std::make_shared<User>();
                    u->UserID(user[i]["_source"]["user_id"].asString());
                    u->NickName(user[i]["_source"]["nickName"].asString());
                    u->Phone(user[i]["_source"]["phone"].asString());
                    u->Description(user[i]["_source"]["description"].asString());
                    u->AvatarID(user[i]["_source"]["avatar_id"].asString());
                    userList.push_back(u);
                }
                return userList;
            }

        private:
            std::shared_ptr<elasticlient::Client> _client; // es client
        };
    }
}