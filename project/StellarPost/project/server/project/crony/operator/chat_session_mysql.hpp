/*
    实现 chat_session 微服务对 MySQL 数据库相关操作的代码
*/

#pragma once

// C
#include <cstring>
#include <cstddef>
// C++
#include <string>
#include <vector>
#include <memory>
#include <optional>
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
#include "../odb/chat_session.hxx"
#include "../../common/odb/chat_session_member.hxx"
#include "chat_session-odb.hxx"
#include "chat_session_member-odb.hxx"

// 为 ChatType 提供自定义格式化支持
template <>
struct fmt::formatter<stellar_post::crony::ChatType>
{
    // 默认的 parse 函数（不处理格式说明符）
    constexpr auto parse(format_parse_context &ctx)
    {
        return ctx.begin(); // 直接返回迭代器位置，不处理额外格式
    }

    // 核心格式化函数
    template <typename FormatContext>
    auto format(const stellar_post::crony::ChatType type, FormatContext &ctx)
    {
        // 将枚举值转换为可读字符串
        std::string_view str;
        switch (type)
        {
        case stellar_post::crony::ChatType::SINGLE:
            str = "Single";
            break;
        case stellar_post::crony::ChatType::GROUP:
            str = "Group";
            break;
        default:
            str = "[UNKNOWN]";
        }

        // 直接格式化字符串视图
        return fmt::format_to(ctx.out(), "{}", str);
    }
};

namespace stellar_post
{
    namespace crony
    {
        class ChatSessionTable
        {
        public:
            using cstPtr = std::shared_ptr<ChatSessionTable>;

        public:
            ChatSessionTable(const std::shared_ptr<odb::core::database> &db)
                : _db(db)
            {
            }

            // 新增会话 -- 向会话成员表中新增会话成员信息 & 会话表中新增会话信息
            auto Add(const std::shared_ptr<ChatSession> &chatSession) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务
                    // std::unique_ptr<odb::mysql::transaction_impl> t(db.begin()); // 手动开启事务

                    // 新增事务
                    _db->persist(chatSession.get());

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "add {}-{}-{} chat session error: {}",
                            chatSession->ChatSessionId(), chatSession->ChatSessionName(),
                            chatSession->ChatSessionType(), e.what());
                    return false;
                }
            }

            // 删除会话 -- 删除会话成员表中的会话成员信息 & 删除会话表中的会话信息
            auto Remove(const std::string &chatSessionId) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 移除事务
                    using csquery = odb::query<ChatSession>;
                    using csresult = odb::result<ChatSession>;
                    using csmquery = odb::query<ChatSessionMember>;
                    using csmresult = odb::result<ChatSessionMember>;
                    _db->erase_query<ChatSession>(csquery::chat_session_id == chatSessionId);
                    _db->erase_query<ChatSessionMember>(csmquery::session_id == chatSessionId);

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "remove {} chat session error: {}",
                            chatSessionId, e.what());
                    return false;
                }
            }
            // 删除会话 -- 根据单聊会话两个成员进行删除
            auto Remove(const std::string &userId, const std::string &peerId) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 移除事务
                    using query = odb::query<SingleChatSessionView>;
                    using result = odb::result<SingleChatSessionView>;
                    auto ret = _db->query_one<SingleChatSessionView>(query::cs::chat_session_type == ChatType::SINGLE &&
                                                                     query::csm1::user_id == userId &&
                                                                     query::csm2::user_id == peerId);
                    std::string chatSessionId = ret->_chat_session_id;

                    using csquery = odb::query<ChatSession>;
                    using csresult = odb::result<ChatSession>;
                    using csmquery = odb::query<ChatSessionMember>;
                    using csmresult = odb::result<ChatSessionMember>;
                    _db->erase_query<ChatSession>(csquery::chat_session_id == chatSessionId);
                    _db->erase_query<ChatSessionMember>(csmquery::session_id == chatSessionId);

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "remove {}-{} chat session error: {}",
                            userId, peerId, e.what());
                    return false;
                }
            }

            // 通过会话 ID, 获取会话的详细信息
            auto Select(const std::string &chatSessionId) const -> std::optional<std::shared_ptr<ChatSession>>
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 查询事务
                    using query = odb::query<ChatSession>;
                    using result = odb::result<ChatSession>;
                    std::shared_ptr<ChatSession> res(
                        _db->query_one<ChatSession>(query::chat_session_id == chatSessionId));

                    // 提交事务
                    trans.commit();
                    return res;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "select chat session {} data by chat_session_id error {}", chatSessionId, e.what());
                    return std::nullopt;
                }
            }

            // 通过用户 ID 获取所有的好友单聊会话(连接会话成员表 & 用户表)
            // 会话 ID , 会话名称, 好友昵称, 会话类型->单聊类型, 会话头像->好友头像, 好友 ID
            auto SelectSingleChatSession(const std::string &userId) const
                -> std::optional<std::vector<std::shared_ptr<SingleChatSessionView>>>
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务
                    std::vector<std::shared_ptr<SingleChatSessionView>> res;

                    // 查询事务
                    using query = odb::query<SingleChatSessionView>;
                    using result = odb::result<SingleChatSessionView>;
                    result ret(_db->query<SingleChatSessionView>(
                        query::cs::chat_session_type == ChatType::SINGLE &&
                        query::csm1::user_id == userId &&
                        query::csm2::user_id != query::csm1::user_id));
                    for (auto &item : ret)
                    {
                        res.push_back(std::make_shared<SingleChatSessionView>(std::move(item)));
                    }

                    // 提交事务
                    trans.commit();
                    return res;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "select single chat session {} data by user_id error {}", userId, e.what());
                    return std::nullopt;
                }
            }

            // 通过用户 ID 获取所有的群聊会话(连接会话成员表 & 用户表)
            // 会话 ID , 会话名称, 会话类型->群聊类型
            auto SelectGroupChatSession(const std::string &userId) const
                -> std::optional<std::vector<std::shared_ptr<GroupChatSessionView>>>
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务
                    std::vector<std::shared_ptr<GroupChatSessionView>> res;

                    // 查询事务
                    using query = odb::query<GroupChatSessionView>;
                    using result = odb::result<GroupChatSessionView>;
                    result ret(_db->query<GroupChatSessionView>(
                        query::cs::chat_session_type == ChatType::GROUP &&
                        query::csm::user_id == userId));
                    for (auto &item : ret)
                    {
                        res.push_back(std::make_shared<GroupChatSessionView>(std::move(item)));
                    }

                    // 提交事务
                    trans.commit();
                    return res;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "select group chat session {} data by user_id error {}", userId, e.what());
                    return std::nullopt;
                }
            }

        private:
            std::shared_ptr<odb::core::database> _db;
        };
    }
}