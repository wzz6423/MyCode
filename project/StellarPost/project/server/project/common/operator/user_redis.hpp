/*
    实现 user 微服务对 Redis 数据库相关操作的代码
*/

#pragma once

// C++
#include <memory>
#include <thread>
#include <chrono>
#include <optional>
// redis
#include <sw/redis++/redis.h> // Redis 模块

namespace stellar_post
{
    namespace Operator
    {
        class RedisFactory
        {
        public:
            static auto Create(const std::string &host,
                               const int port,
                               const int db,
                               const bool keepAlive) -> std::shared_ptr<sw::redis::Redis>
            {
                sw::redis::ConnectionOptions options;
                options.host = host;
                options.port = port;
                options.db = db;
                options.keep_alive = keepAlive;
                std::shared_ptr<sw::redis::Redis> res =
                    std::make_shared<sw::redis::Redis>(options);
                return res;
            }
        };
    }

    namespace user
    {
        class Session
        {
        public:
            using sessionPtr = std::shared_ptr<Session>;

        public:
            Session(const std::shared_ptr<sw::redis::Redis> &redis)
                : _redis(redis)
            {
            }

            // 新增(会话 ID -- ssid、 用户 ID -- uid)
            auto Add(const std::string &ssid, const std::string &uid) -> bool
            {
                return _redis->set(ssid, uid);
            }

            // 删除
            auto Remove(const std::string &ssid) -> bool
            {
                return _redis->del(ssid);
            }

            // 获取
            auto Get(const std::string &ssid) -> std::optional<std::string>
            {
                return _redis->get(ssid);
            }

        private:
            std::shared_ptr<sw::redis::Redis> _redis;
        };

        // 登录状态
        class Status
        {
        public:
            using statusPtr = std::shared_ptr<Status>;

        public:
            Status(const std::shared_ptr<sw::redis::Redis> &redis)
                : _redis(redis)
            {
            }

            // 新增(用户 ID -- uid)
            auto Add(const std::string &uid) -> bool
            {
                return _redis->set(uid, "");
            }

            // 删除
            auto Remove(const std::string &uid) -> bool
            {
                return _redis->del(uid);
            }

            // 获取
            auto Exists(const std::string &uid) -> bool
            {
                return _redis->exists(uid);
            }

        private:
            std::shared_ptr<sw::redis::Redis> _redis;
        };

        // 验证码
        class Codes
        {
        public:
            using codesPtr = std::shared_ptr<Codes>;

        public:
            Codes(const std::shared_ptr<sw::redis::Redis> &redis)
                : _redis(redis)
            {
            }

            // 新增(验证码 ID -- cid、 验证码 -- code)
            auto Add(const std::string &cid, const std::string &code,
                        const std::chrono::seconds &time = std::chrono::seconds(300)) -> bool // 缺省过期时间 5 mins
            {
                return _redis->set(cid, code, time);
            }

            // 删除
            auto Remove(const std::string &cid) -> bool
            {
                return _redis->del(cid);
            }

            // 获取
            auto Get(const std::string &cid) -> std::optional<std::string>
            {
                return _redis->get(cid);
            }

        private:
            std::shared_ptr<sw::redis::Redis> _redis;
        };
    }
}