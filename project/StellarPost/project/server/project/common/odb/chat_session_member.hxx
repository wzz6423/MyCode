/*
    聊天会话成员表 -- user && crony
*/

#pragma once

// C
#include <cstddef>
// C++
#include <string>
// Other
#include <odb/nullable.hxx>
#include <odb/core.hxx>

namespace stellar_post
{
    namespace crony
    {
#pragma db object table("chat_session_member")
        class ChatSessionMember
        {
        public:
            ChatSessionMember() = default;
            ChatSessionMember(const ChatSessionMember &) = default;
            ChatSessionMember(ChatSessionMember &&) = default;
            ChatSessionMember &operator=(const ChatSessionMember &) = default;
            ChatSessionMember &operator=(ChatSessionMember &&) = default;
            ~ChatSessionMember() = default;

            ChatSessionMember(const std::string &userID, const std::string &sessionID)
                : _user_id(userID), _session_id(sessionID)
            {
            }

            // 设置 session_id
            auto SetSessionId(const std::string &sessionID) -> void
            {
                _session_id = sessionID;
            }
            // 获取 session_id
            auto SessionId() const -> std::string
            {
                return _session_id;
            }

            // 设置 user_id
            auto SetUserId(const std::string &userID) -> void
            {
                _user_id = userID;
            }
            // 获取 user_id
            auto UserId() const -> std::string
            {
                return _user_id;
            }

        private:
            friend class odb::access;

        private:
            // 主键 ID
#pragma db id auto // 使用 auto 让 ODB 自动生成 ID
            unsigned long _id;

            // 用户 ID
#pragma db type("varchar(64)")
            std::string _user_id;

            // 聊天会话 ID
#pragma db type("varchar(64)") index
            std::string _session_id;
        };

        // odb -d mysql --std c++17 --generate-query --generate-schema --profile boost/date-time chat_session_member.hxx
    }
}