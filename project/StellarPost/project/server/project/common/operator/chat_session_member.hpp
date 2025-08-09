/*
    实现 crony 微服务对 MySQL 数据库相关操作的代码
*/

#pragma once

// C
#include <cstddef>
// C++
#include <string>
#include <vector>
#include <memory>
// Other
#include "../etcd/etcd.hpp"  // 服务注册 etcd 模块封装
#include "../brpc/brpc.hpp"  // brpc 模块封装
#include "../log/logger.hpp" // 日志器 spdlog 模块封装
#include "operator.hpp"      // mysql Operator 模块封装
// odb
#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#include <odb/nullable.hxx>
#include <odb/core.hxx>
#include "../odb/chat_session_member.hxx"
#include "chat_session_member-odb.hxx"

namespace stellar_post
{
    namespace crony
    {
        // 向指定会话添加单个成员
        // 向指定会话添加多个成员
        // 从指定会话移除单个成员
        // 通过会话 ID 获取会话的所有成员 ID
        // 删除会话所有成员
        class ChatSessionMemberTable
        {
        public:
            using csmtPtr = std::shared_ptr<ChatSessionMemberTable>;

        public:
            ChatSessionMemberTable(std::shared_ptr<odb::core::database> db)
                : _db(db)
            {
            }
            ~ChatSessionMemberTable() = default;

            // 向指定会话添加单个成员
            auto Add(const std::shared_ptr<ChatSessionMember> &member) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务
                    // std::unique_ptr<odb::mysql::transaction_impl> t(db.begin()); // 手动开启事务

                    // 插入事务
                    _db->persist(*member);

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "insert signal user {} data in session {} error {}",
                            member->UserId(), member->SessionId(), e.what());
                    return false;
                }
            }

            // 向指定会话添加多个成员
            auto Add(const std::vector<std::shared_ptr<ChatSessionMember>> &members) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 插入事务
                    for (const auto &member : members)
                    {
                        _db->persist(*member);
                    }

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "insert mul {} user data in session {} error {}",
                            members.size(), members[0]->SessionId(), e.what());
                    return false;
                }
            }

            // 从指定会话移除单个成员 -- 先查再删
            auto Remove(const std::shared_ptr<ChatSessionMember> &member) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 插入事务
                    using query = odb::query<ChatSessionMember>;
                    using result = odb::result<ChatSessionMember>;
                    _db->erase_query<ChatSessionMember>(query::session_id == member->SessionId() &&
                                                        query::user_id == member->UserId());

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "remove signal user {} data in session {} error {}",
                            member->UserId(), member->SessionId(), e.what());
                    return false;
                }
            }

            // 删除会话所有成员
            auto Clear(const std::string &sessionID) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 插入事务
                    using query = odb::query<ChatSessionMember>;
                    using result = odb::result<ChatSessionMember>;
                    _db->erase_query<ChatSessionMember>(query::session_id == sessionID);

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "clear user data in session {} error {}", sessionID, e.what());
                    return false;
                }
            }

            // 通过会话 ID 获取会话的所有成员 ID
            auto AllSessionMember(const std::string &sessionID) const -> std::optional<std::vector<std::string>>
            {
                std::vector<std::string> ids;
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 查询事务
                    using query = odb::query<ChatSessionMember>;
                    using result = odb::result<ChatSessionMember>;
                    for (auto &member : _db->query<ChatSessionMember>(query::session_id == sessionID))
                    {
                        ids.push_back(member.UserId());
                    }

                    // 提交事务
                    trans.commit();
                    return ids;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "get user data in session {} error {}", sessionID, e.what());
                    return std::nullopt;
                }
            }

        private:
            std::shared_ptr<odb::core::database> _db;
        };
    }
}