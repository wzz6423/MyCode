/*
    实现 crony 微服务对数据库相关操作的框架代码
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
#pragma db object table("crony_relation")
        class CronyRelation
        {
        public:
            CronyRelation() = default;
            CronyRelation(const std::string &user_id, const std::string &peer_id)
                : _user_id(user_id), _peer_id(peer_id)
            {
            }
            ~CronyRelation() {}

            // 用户 ID
            auto UserId() const -> std::string
            {
                return _user_id;
            }
            auto UserId(const std::string &user_id) -> void
            {
                _user_id = user_id;
            }

            // 好友 ID
            auto PeerId() const -> std::string
            {
                return _peer_id;
            }
            auto PeerId(const std::string &peer_id) -> void
            {
                _peer_id = peer_id;
            }

        private:
            friend class odb::access;

        private:
// 主键 ID
#pragma db id auto // 使用 auto 让 ODB 自动生成 ID
            unsigned long _id;
// 用户 ID
#pragma db type("varchar(64)") index
            std::string _user_id;
// 好友 ID
#pragma db type("varchar(64)")
            std::string _peer_id;
        };

        // odb -d mysql --std c++17 --generate-query --generate-schema --profile boost/date-time crony_relation.hxx
    }
}