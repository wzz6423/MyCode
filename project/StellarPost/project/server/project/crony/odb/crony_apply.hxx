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
#pragma db object table("crony_apply")
        class CronyApply
        {
        public:
            CronyApply() = default;
            CronyApply(const std::string &event_id,
                       const std::string &applicant_id,
                       const std::string &receiver_id)
                : _event_id(event_id),
                  _applicant_id(applicant_id),
                  _receiver_id(receiver_id)
            {
            }
            ~CronyApply() {}

            // 事件 ID
            auto EventId() const -> std::string
            {
                return _event_id;
            }
            auto EventId(const std::string &event_id) -> void
            {
                _event_id = event_id;
            }

            // 申请者 ID
            auto ApplicantId() const -> std::string
            {
                return _applicant_id;
            }
            auto ApplicantId(const std::string &applicant_id) -> void
            {
                _applicant_id = applicant_id;
            }

            // 接收者 ID
            auto ReceiverId() const -> std::string
            {
                return _receiver_id;
            }
            auto ReceiverId(const std::string &receiver_id) -> void
            {
                _receiver_id = receiver_id;
            }

        private:
            friend class odb::access;

        private:
// 主键 ID
#pragma db id auto // 使用 auto 让 ODB 自动生成 ID
            unsigned long _id;
// 事件 ID
#pragma db type("varchar(64)") index unique
            std::string _event_id;
// 申请者 ID
#pragma db type("varchar(64)") index
            std::string _applicant_id;
// 接收者 ID
#pragma db type("varchar(64)") index
            std::string _receiver_id;
        };

        // odb -d mysql --std c++17 --generate-query --generate-schema --profile boost/date-time crony_apply.hxx
    }
}