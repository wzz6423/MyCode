/*
    实现 crony_relation 微服务对 MySQL 数据库相关操作的代码
*/

#pragma once

// C
#include <cstring>
#include <cstddef>
// C++
#include <string>
#include <vector>
#include <memory>
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
#include "../odb/crony_relation.hxx"
#include "crony_relation-odb.hxx"

namespace stellar_post
{
    namespace crony
    {
        class CronyRelationTable
        {
        public:
            using crtPtr = std::shared_ptr<CronyRelationTable>;

        public:
            CronyRelationTable(const std::shared_ptr<odb::core::database> &db)
                : _db(db)
            {
            }

            // 新增好友关系信息
            auto Add(const std::string &user_id, const std::string &peer_id) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务
                    // std::unique_ptr<odb::mysql::transaction_impl> t(db.begin()); // 手动开启事务

                    CronyRelation relation1(user_id, peer_id);
                    CronyRelation relation2(peer_id, user_id);

                    // 新增事务
                    _db->persist(relation1);
                    _db->persist(relation2);

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "add {} and {} relation error: {}",
                            user_id, peer_id, e.what());
                    return false;
                }
            }

            // 移除好友关系信息
            auto Remove(const std::string &userId, const std::string &peerId) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 移除事务
                    using query = odb::query<CronyRelation>;
                    using result = odb::result<CronyRelation>;
                    _db->erase_query<CronyRelation>(query::user_id == userId && query::peer_id == peerId);
                    _db->erase_query<CronyRelation>(query::user_id == peerId && query::peer_id == userId);

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "remove {} and {} relation error: {}",
                            userId, peerId, e.what());
                    return false;
                }
            }

            // 判断好友关系是否存在
            auto Exists(const std::string &userId, const std::string &peerId) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 查询事务
                    using query = odb::query<CronyRelation>;
                    using result = odb::result<CronyRelation>;
                    result r(_db->query<CronyRelation>(query::user_id == userId && query::peer_id == peerId));
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
                            "check {} and {} relation error: {}",
                            userId, peerId, e.what());
                    return false;
                }
            }

            // 获取指定用户的好友 ID
            auto CronyIds(const std::string &userId) const -> std::unordered_set<std::string>
            {
                std::unordered_set<std::string> cronyIds;
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 查询事务
                    using query = odb::query<CronyRelation>;
                    using result = odb::result<CronyRelation>;
                    result r(_db->query<CronyRelation>(query::user_id == userId));
                    for (auto &relation : r)
                    {
                        cronyIds.insert(std::move(relation.PeerId()));
                    }

                    // 提交事务
                    trans.commit();
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "get {} crony ids error: {}",
                            userId, e.what());
                }
                return cronyIds;
            }

        private:
            std::shared_ptr<odb::core::database> _db;
        };
    }
}