/*
    实现 message 微服务对 elasticsearch 相关操作的代码
*/

#pragma once

// C++
#include <memory>
// elastic search
#include "../../common/operator/operator.hpp"
#include "../../common/elastic/elastic.hpp"
// Other
#include "../../common/log/logger.hpp" // 日志器 spdlog 模块封装
#include "../odb/message.hxx"

namespace stellar_post
{
    namespace message
    {
        class ESMessage
        {
        public:
            using esmPtr = std::shared_ptr<ESMessage>;

        public:
            ESMessage(const std::shared_ptr<elasticlient::Client> &client)
                : _client(client)
            {
            }
            ~ESMessage() {}

            // 创建索引
            auto CreateIndex() -> bool
            {
                stellar_post::Elastic::EsIndex index("message", _client);
                index.Add("user_id", "keyword", "standard", false);
                index.Add("chat_session_id", "keyword", "standard", true);
                index.Add("message_id", "keyword", "standard", false);
                index.Add("generate_time", "long", "standard", false);
                index.Add("content");
                bool ret = index.Create();
                if (!ret)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "message index create failed!");
                    return false;
                }
                return true;
            }

            // 新增数据
            auto Add(const std::string &userID,
                     const std::string &messageID,
                     const std::string &chatChatSessionId,
                     const std::string &content,
                     const long &generateTime) -> bool
            {
                bool ret = Elastic::EsInsert("user", _client)
                               .Add("user_id", userID)
                               .Add("message_id", messageID)
                               .Add("generate_time", generateTime)
                               .Add("chat_session_id", chatChatSessionId)
                               .Add("content", content)
                               .Insert(messageID);
                if (!ret)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "add or update message data error!");
                    return false;
                }
                return true;
            }

            // 搜索
            auto Search(const std::string &key,
                        const std::string &chatChatSessionId)
                -> std::vector<std::shared_ptr<Message>>
            {
                std::vector<std::shared_ptr<Message>> messageList;
                Json::Value msg;
                std::optional<Json::Value> oMessage = Elastic::EsSerach("message", _client)
                                                          .AddMustTerm("chat_session_id.keyword", chatChatSessionId)
                                                          .AddMustMatch("content", key)
                                                          .Search();
                if (!oMessage)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "search data error!");
                    return messageList;
                }
                else
                {
                    msg = oMessage.value();
                    if (msg.isArray() == false)
                    {
                        Log::lg("error", Log::FileName(), Log::Line(), "search data error! it is not an arr!");
                        return messageList;
                    }
                }

                for (int i = 0; i < msg.size(); ++i)
                {
                    std::shared_ptr<Message> m = std::make_shared<Message>();
                    m->UserID(msg[i]["_source"]["user_id"].asString());
                    m->ChatSessionId(msg[i]["_source"]["chat_session_id"].asString());
                    m->MessageID(msg[i]["_source"]["message_id"].asString());
                    m->GenerateTime(boost::posix_time::ptime(boost::posix_time::from_time_t(msg[i]["_source"]["generate_time"].asUInt64())));
                    m->Content(msg[i]["_source"]["content"].asString());
                    messageList.emplace_back(m);
                }
                return messageList;
            }

            // 删除数据
            auto Remove(const std::string &messageID) -> bool
            {
                bool ret = Elastic::EsRemove("message", _client)
                               .Remove(messageID);
                if (!ret)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "remove message data error!");
                    return false;
                }
                return true;
            }

        private:
            std::shared_ptr<elasticlient::Client> _client;
        };
    }
}