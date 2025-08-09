/*
    实现 crony 微服务对数据库相关操作的框架代码
*/

#pragma once

// C
#include <cstddef>
// C++
#include <string>
#include <string_view>
#include <fmt/core.h>
// Other
#include <odb/nullable.hxx>
#include <odb/core.hxx>
#include "chat_session_member.hxx"

namespace stellar_post
{
    namespace crony
    {
        // 会话类型
        enum class ChatType
        {
            SINGLE = 1,
            GROUP = 2
        };

#pragma db object table("chat_session")
        class ChatSession
        {
        public:
            ChatSession() = default;
            ChatSession(const std::string &chat_session_id,
                        const std::string &chat_session_name,
                        const ChatType &chat_session_type)
                : _chat_session_id(chat_session_id),
                  _chat_session_name(chat_session_name),
                  _chat_session_type(chat_session_type)
            {
            }
            ~ChatSession() {}

            // 聊天会话 ID
            auto ChatSessionId() const -> std::string
            {
                return _chat_session_id;
            }
            auto ChatSessionId(const std::string &chat_session_id) -> void
            {
                _chat_session_id = chat_session_id;
            }
            // 聊天会话名称
            auto ChatSessionName() const -> std::string
            {
                return _chat_session_name;
            }
            auto ChatSessionName(const std::string &chat_session_name) -> void
            {
                _chat_session_name = chat_session_name;
            }
            // 聊天会话类型
            auto ChatSessionType() const -> ChatType
            {
                return _chat_session_type;
            }
            auto ChatSessionType(const ChatType &chat_session_type) -> void
            {
                _chat_session_type = chat_session_type;
            }

        private:
            friend class odb::access;

        private:
// 主键 ID
#pragma db id auto // 使用 auto 让 ODB 自动生成 ID
            unsigned long _id;
// 聊天会话 ID
#pragma db type("varchar(64)") index unique
            std::string _chat_session_id;
// 聊天会话名称
#pragma db type("varchar(64)")
            std::string _chat_session_name;
// 聊天会话类型 -- 1: 单聊 2: 群聊
#pragma db type("tinyint")
            ChatType _chat_session_type;
        };

// 此处条件必须是指定条件:
// cs::_chat_session_type==ChatType::SINGLE && csm1::_user_id==userId && csm2::_user_id!=csm1::_user_id
#pragma db view object(ChatSession = cs)                                             \
    object(ChatSessionMember = csm1 : cs::_chat_session_id == csm1::_session_id)     \
        object(ChatSessionMember = csm2 : cs::_chat_session_id == csm2::_session_id) \
            query((?))
        struct SingleChatSessionView
        {
#pragma db column(cs::_chat_session_id)
            std::string _chat_session_id;
#pragma db column(csm2::_user_id)
            std::string _crony_id;
        };

// 此处条件必须是指定条件:
// cs::_chat_session_type==ChatType::GROUP && csm::_user_id==userId
#pragma db view object(ChatSession = cs)                                       \
    object(ChatSessionMember = csm : cs::_chat_session_id == csm::_session_id) \
        query((?))
        struct GroupChatSessionView
        {
#pragma db column(cs::_chat_session_id)
            std::string _chat_session_id;
#pragma db column(cs::_chat_session_name)
            std::string _chat_session_name;
        };

        // odb -d mysql --std c++17 --generate-query --generate-schema --profile boost/date-time chat_session.hxx
    }
}