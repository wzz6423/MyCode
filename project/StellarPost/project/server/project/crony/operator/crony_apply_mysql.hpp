/*
    实现 crony_apply 微服务对 MySQL 数据库相关操作的代码
*/

#pragma once

// C
#include <cstring>
#include <cstddef>
// C++
#include <string>
#include <memory>
#include <unordered_set>
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
#include "../odb/crony_apply.hxx"
#include "crony_apply-odb.hxx"

namespace stellar_post
{
    namespace crony
    {
        class CronyApplyTable
        {
        public:
            using catPtr = std::shared_ptr<CronyApplyTable>;

        public:
            CronyApplyTable(const std::shared_ptr<odb::core::database> &db)
                : _db(db)
            {
            }

            // 新增好友申请事件
            auto Add(const std::shared_ptr<CronyApply> &apply) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务
                    // std::unique_ptr<odb::mysql::transaction_impl> t(db.begin()); // 手动开启事务

                    // 新增事务
                    _db->persist(apply.get());

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "{} add {} and {} crony apply event error: {}",
                            apply->EventId(), apply->ApplicantId(), apply->ReceiverId(), e.what());
                    return false;
                }
            }

            // 删除好友申请事件
            auto Remove(const std::string &applicantId, const std::string &receiverId) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 移除事务
                    using query = odb::query<CronyApply>;
                    using result = odb::result<CronyApply>;
                    _db->erase_query<CronyApply>(query::applicant_id == applicantId &&
                                                 query::receiver_id == receiverId);

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "remove {} and {} crony apply event error: {}",
                            applicantId, receiverId, e.what());
                    return false;
                }
            }

            // 判断申请好友关系事件是否存在
            auto Exists(const std::string &applicantId, const std::string &receiverId) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 查询事务
                    using query = odb::query<CronyApply>;
                    using result = odb::result<CronyApply>;
                    result r(_db->query<CronyApply>(query::applicant_id == applicantId &&
                                                    query::receiver_id == receiverId));
                    bool flag = r.empty();

                    // 提交事务
                    trans.commit();

                    if (flag)
                    {
                        return false;
                    }
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "check {} and {} apply event error: {}",
                            applicantId, receiverId, e.what());
                    return false;
                }
            }

            // 获取用户待处理好友申请列表及申请者用户 ID 列表
            auto ApplyUserList(const std::string &receiverId) const -> std::unordered_set<std::string>
            {
                std::unordered_set<std::string> applicantIds;
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 查询事务
                    using query = odb::query<CronyApply>;
                    using result = odb::result<CronyApply>;
                    result r(_db->query<CronyApply>(query::receiver_id == receiverId));
                    for (auto &applyUser : r)
                    {
                        applicantIds.insert(std::move(applyUser.ApplicantId()));
                    }

                    // 提交事务
                    trans.commit();
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "get {} apply user ids error: {}",
                            receiverId, e.what());
                }
                return applicantIds;
            }

        private:
            std::shared_ptr<odb::core::database> _db;
        };
    }
}