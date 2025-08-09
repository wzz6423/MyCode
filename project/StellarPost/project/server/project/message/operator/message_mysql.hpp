/*
    实现 message 微服务对 MySQL 数据库相关操作的代码
*/

#pragma once

// C
#include <cstring>
#include <cstddef>
// C++
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
// Other
#include "../../common/etcd/etcd.hpp"         // 服务注册 etcd 模块封装
#include "../../common/brpc/brpc.hpp"         // brpc 模块封装
#include "../../common/log/logger.hpp"        // 日志器 spdlog 模块封装
#include "../../common/operator/operator.hpp" // mysql Operator 模块封装
// odb
#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#include <odb/nullable.hxx>
#include <odb/core.hxx>
#include "../odb/message.hxx"
#include "message-odb.hxx"

namespace stellar_post
{
    namespace message
    {
        class MessageTable
        {
        public:
            using msgPtr = std::shared_ptr<MessageTable>;

        public:
            MessageTable(const std::shared_ptr<odb::core::database> &db)
                : _db(db)
            {
            }
            ~MessageTable() = default;

            // 新增
            auto Add(const std::shared_ptr<Message> &message) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务
                    // std::unique_ptr<odb::mysql::transaction_impl> t(db.begin()); // 手动开启事务

                    // 插入事务
                    _db->persist(*message);

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "insert {} message {} data error {}",
                            message->ChatSessionId(), message->MessageID(), e.what());
                    return false;
                }
            }

            // 删除
            auto Remove(const std::string &sessionId) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 插入事务
                    using query = odb::query<Message>;
                    using result = odb::result<Message>;
                    _db->erase_query<Message>(query::chat_session_id == sessionId);

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "insert {} message data error {}",
                            sessionId, e.what());
                    return false;
                }
            }

            // 查询
            auto SelectRencent(const std::string &sessionId, int count) const -> std::vector<std::shared_ptr<Message>>
            {
                std::vector<std::shared_ptr<Message>> messages;
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 查询事务
                    using query = odb::query<Message>;
                    using result = odb::result<Message>;
                    // 以 sessionID 为过滤条件, 以时间进行逆序, 通过 limit
                    std::string condition = "session_id='" + sessionId + "'" + "order by generate_time desc limit" + std::to_string(count);
                    result r(_db->query<Message>(condition));
                    for (auto &msg : r)
                    {
                        messages.push_back(std::make_shared<Message>(std::move(msg)));
                    }

                    // 提交事务
                    trans.commit();

                    std::reverse(messages.begin(), messages.end());
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "use count get message data in session {} error {}", sessionId, e.what());
                }
                return messages;
            }
            auto SelectRange(const std::string &sessionId,
                             boost::posix_time::ptime beginTime,
                             boost::posix_time::ptime endTime)
                const -> std::vector<std::shared_ptr<Message>>
            {
                std::vector<std::shared_ptr<Message>> messages;
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 查询事务
                    using query = odb::query<Message>;
                    using result = odb::result<Message>;
                    // 以 sessionID + 指定时间段为过滤条件, 以时间进行逆序
                    for (auto &msg : _db->query<Message>(query::chat_session_id == sessionId &&
                                                         query::generate_time >= beginTime &&
                                                         query::generate_time <= endTime))
                    {
                        messages.push_back(std::make_shared<Message>(std::move(msg)));
                    }

                    // 提交事务
                    trans.commit();

                    std::reverse(messages.begin(), messages.end());
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "use time range get message data in session {} error {}", sessionId, e.what());
                }
                return messages;
            }

        private:
            std::shared_ptr<odb::core::database> _db;
        };
    }
}