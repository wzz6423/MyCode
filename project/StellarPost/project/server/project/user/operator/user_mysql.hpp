/*
    实现 user 微服务对 MySQL 数据库相关操作的代码
*/

#pragma once

// C
#include <cstring>
#include <cstddef>
// C++
#include <string>
#include <vector>
#include <sstream>
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
#include "user.hxx"
#include "user-odb.hxx"

namespace stellar_post
{
    namespace user
    {
        // 用户注册 -- 用户信息新增
        // 用户登录 -- 通过昵称获取用户信息
        // 验证码获取
        // 手机号注册 -- 用户信息新增
        // 手机号登录 -- 通过手机号获取用户信息
        // 获取(多个)用户信息 -- 通过(多个)用户 ID 获取(多个)用户信息
        // 用户信息修改
        class UserTable
        {
        public:
            using userTablePtr = std::shared_ptr<UserTable>;

        public:
            UserTable() = delete;
            UserTable(const std::shared_ptr<odb::core::database> &db)
                : _db{db}
            {
            }

            // 插入
            auto Insert(const std::shared_ptr<User> &user) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务
                    // std::unique_ptr<odb::mysql::transaction_impl> t(db.begin()); // 手动开启事务

                    // 插入事务
                    _db->persist(*user);

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "insert user {} data error {}", user->NickName(), e.what());
                    return false;
                }
            }

            // 更新(先查询后更新, 此处的形参 user 一定是查询出来进行修改后的而不是重新构造的)
            auto Update(const std::shared_ptr<User> &user) const -> bool
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 更新事务
                    _db->update(*user);

                    // 提交事务
                    trans.commit();
                    return true;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "update user {} data error {}", user->NickName(), e.what());
                    return false;
                }
            }

            // 查看
            // 昵称
            auto SelectNickName(const std::string &nickName) const -> std::shared_ptr<User>
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 查询事务
                    using query = odb::query<User>;
                    using result = odb::result<User>;
                    std::shared_ptr<User> res(_db->query_one<User>(query::nickname == nickName));

                    // 提交事务
                    trans.commit();
                    return res;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "select user {} data by nickname error {}", nickName, e.what());
                    return nullptr;
                }
            }
            // 手机号
            auto SelectPhone(const std::string &phone) const -> std::shared_ptr<User>
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 查询事务
                    using query = odb::query<User>;
                    using result = odb::result<User>;
                    std::shared_ptr<User> res(_db->query_one<User>(query::phone == phone));

                    // 提交事务
                    trans.commit();
                    return res;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "select user {} data by phone error {}", phone, e.what());
                    return nullptr;
                }
            }
            // 用户 ID
            auto SelectID(const std::string &id) const -> std::shared_ptr<User>
            {
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 查询事务
                    using query = odb::query<User>;
                    using result = odb::result<User>;
                    std::shared_ptr<User> res(_db->query_one<User>(query::user_id == id));

                    // 提交事务
                    trans.commit();
                    return res;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "select user {} data by user_id error {}", id, e.what());
                    return nullptr;
                }
            }
            // 多个用户 ID
            auto SelectIDs(const std::vector<std::string> &ids) const -> std::vector<std::shared_ptr<User>>
            {
                // select * from user where user_id in ('id1', 'id2', ...)
                if (ids.empty())
                {
                    return {};
                }
                std::vector<std::shared_ptr<User>> res;
                try
                {
                    // 获取事务操作对象, 开启事务
                    odb::transaction trans(_db->begin()); // 自动开启事务

                    // 查询事务
                    using query = odb::query<User>;
                    using result = odb::result<User>;

                    std::stringstream cond;
                    cond << "user_id in(";
                    for (const auto &id : ids)
                    {
                        cond << "'" << id << "',";
                    }
                    std::string sql = cond.str();
                    sql.pop_back(); // 删除最后多余的 ,
                    sql += ")";

                    result r(_db->query<User>(sql));
                    for (auto &user : r)
                    {
                        res.push_back(std::make_shared<User>(user));
                    }

                    // 提交事务
                    trans.commit();
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "select user data by ids error {}", e.what());
                }
                return res;
            }

        private:
            std::shared_ptr<odb::core::database> _db;
        };
    }
}