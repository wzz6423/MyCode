/*
    实现 message 微服务对数据库相关操作的框架代码
*/

#pragma once

// C
#include <cstddef>
// C++
#include <string>
// Other
#include <odb/nullable.hxx>
#include <odb/core.hxx>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>

namespace stellar_post
{
    namespace message
    {
#pragma db object table("message")
        class Message
        {
        public:
            Message() = default;
            Message(const std::string &message_id,
                    const std::string &chat_session_id,
                    const std::string &from_user_id,
                    const unsigned char &message_type,
                    const boost::posix_time::ptime &generate_time)
                : _message_id(message_id),
                  _chat_session_id(chat_session_id),
                  _from_user_id(from_user_id),
                  _message_type(message_type),
                  _generate_time(generate_time)
            {
            }

            // 消息 ID
            auto MessageID() -> std::string
            {
                return _message_id;
            }
            auto MessageID(const std::string &message_id) -> void
            {
                _message_id = message_id;
            }

            // 消息会话 ID
            auto ChatSessionId() -> std::string
            {
                return _chat_session_id;
            }
            auto ChatSessionId(const std::string &session_id) -> void
            {
                _chat_session_id = session_id;
            }

            // 消息发送者 ID
            auto UserID() -> std::string
            {
                return _from_user_id;
            }
            auto UserID(const std::string &user_id) -> void
            {
                _from_user_id = user_id;
            }

            // 消息类型
            auto MessageType() -> unsigned char
            {
                return _message_type;
            }
            auto MessageType(const unsigned char &message_type) -> void
            {
                _message_type = message_type;
            }

            // 消息产生时间
            auto GenerateTime() -> boost::posix_time::ptime
            {
                return _generate_time;
            }
            auto GenerateTime(const boost::posix_time::ptime &generate_time) -> void
            {
                _generate_time = generate_time;
            }

            // 文本消息内容 -- 非文本消息忽略
            auto Content() -> std::string
            {
                if (!_content)
                {
                    return std::string();
                }
                else
                {
                    return *_content;
                }
            }
            auto Content(const std::string &content) -> void
            {
                _content = content;
            }

            // 文件 ID -- 文本消息忽略, 图片/文件/语音均存在该字段
            auto FileID() -> std::string
            {
                if (!_file_id)
                {
                    return std::string();
                }
                else
                {
                    return *_file_id;
                }
            }
            auto FileID(const std::string &file_id) -> void
            {
                _file_id = file_id;
            }

            // 文件名称 -- 文件消息存在, 其它消息忽略
            auto FileName() -> std::string
            {
                if (!_file_name)
                {
                    return std::string();
                }
                else
                {
                    return *_file_name;
                }
            }
            auto FileName(const std::string &file_name) -> void
            {
                _file_name = file_name;
            }

            // 文件大小 -- 文件消息存在, 其它消息忽略
            auto FileSize() -> unsigned long long
            {
                if (!_file_size)
                {
                    return 0;
                }
                else
                {
                    return *_file_size;
                }
            }
            auto FileSize(const unsigned long long &file_size) -> void
            {
                _file_size = file_size;
            }

        private:
            friend class odb::access;

        private:
// 主键 ID
#pragma db id auto // 使用 auto 让 ODB 自动生成 ID
            unsigned long _id;
            // 消息 ID
#pragma db type("varchar(64)") index unique
            std::string _message_id;
            // 消息所属会话 ID
#pragma db type("varchar(64)") index
            std::string _chat_session_id;
#pragma db type("varchar(64)")
            // 消息发送者 ID
            std::string _from_user_id;
            // 消息产生时间
#pragma db type("TIMESTAMP") not_null
            boost::posix_time::ptime _generate_time;
            // 消息类型: 0-文本, 1-图片, 2-文件, 3-语音
            unsigned char _message_type;
            // 文本消息内容 -- 非文本消息忽略
            odb::nullable<std::string> _content;
            // 文件 ID -- 文本消息忽略, 图片/文件/语音均存在该字段
#pragma db type("varchar(64)")
            odb::nullable<std::string> _file_id;
            // 文件名称 -- 文件消息存在, 其它消息忽略
#pragma db type("varchar(127)")
            odb::nullable<std::string> _file_name;
            // 文件大小 -- 文件消息存在, 其它消息忽略
            odb::nullable<unsigned long long> _file_size;
        };

        // odb -d mysql --std c++17 --generate-query --generate-schema --profile boost/date-time message.hxx
    }
}