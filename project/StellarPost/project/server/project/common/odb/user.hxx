/*
    实现 user 微服务对数据库相关操作的框架代码
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
    namespace user
    {
#pragma db object table("user")
        class User
        {
        public:
            User() {}
            // 用户名新增用户 -- 用户 ID、昵称、密码
            User(const std::string &uid, const std::string &nickname, const std::string &password)
                : _user_id{uid}, _nickname{nickname}, _password{password}
            {
            }
            // 手机号新增用户 -- 用户 ID、 手机号
            User(const std::string &uid, const std::string &phone)
                : _user_id{uid}, _phone{phone}, _nickname(uid)
            {
            }

            // userID
            auto UserID() -> std::string
            {
                return _user_id;
            }
            auto UserID(const std::string &uid) -> void
            {
                _user_id = uid;
            }

            // 昵称
            auto NickName() -> std::string
            {
                if (_nickname)
                {
                    return *_nickname;
                }
                else
                {
                    return std::string();
                }
            }
            auto NickName(const std::string &name) -> void
            {
                _nickname = name;
            }

            // 签名
            auto Description() -> std::string
            {
                if (_description)
                {
                    return *_description;
                }
                else
                {
                    return std::string();
                }
            }
            auto Description(const std::string &desc) -> void
            {
                _description = desc;
            }

            // 密码
            auto Password() -> std::string
            {
                if (_password)
                {
                    return *_password;
                }
                else
                {
                    return std::string();
                }
            }
            auto Password(const std::string &password) -> void
            {
                _password = password;
            }

            // 手机号
            auto Phone() -> std::string
            {
                if (_phone)
                {
                    return *_phone;
                }
                else
                {
                    return std::string();
                }
            }
            auto Phone(const std::string &phone) -> void
            {
                _phone = phone;
            }

            // 头像文件 ID
            auto AvatarID() -> std::string
            {
                if (_avatar_id)
                {
                    return *_avatar_id;
                }
                else
                {
                    return std::string();
                }
            }
            auto AvatarID(const std::string &id) -> void
            {
                _avatar_id = id;
            }

        private:
            friend class odb::access;

        private:
// 主键 ID
#pragma db id auto // 使用 auto 让 ODB 自动生成 ID
            unsigned long _id;
// 用户 ID
#pragma db type("varchar(64)") index unique
            std::string _user_id;
// 用户昵称
#pragma db type("varchar(64)") index unique
            odb::nullable<std::string> _nickname;    // 不一定存在
                                                     // 用户签名
            odb::nullable<std::string> _description; // 不一定存在
// 登陆密码
#pragma db type("varchar(64)")
            odb::nullable<std::string> _password; // 不一定存在
// 绑定手机号
#pragma db type("varchar(64)") index unique
            odb::nullable<std::string> _phone; // 不一定存在
// 用户头像文件 ID
#pragma db type("varchar(64)")
            odb::nullable<std::string> _avatar_id; // 不一定存在
        };

        // odb -d mysql --std c++17 --generate-query --generate-schema --profile boost/date-time user.hxx
    }
}