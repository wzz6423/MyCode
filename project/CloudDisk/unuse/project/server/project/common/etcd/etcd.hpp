#pragma once

/*
    -letcd-cpp-api -lcpprest
*/

// C++
#include <string>
#include <memory>
#include <functional>
#include <format>
#include <type_traits>
// etcd
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>
#include <etcd/Value.hpp>
#include <etcd/Watcher.hpp>
// Other
#include "../log/logger.hpp"

namespace fmt
{
    // for etcd-log
    // 为 etcd::Event::EventType 实现格式化特化
    template <>
    struct formatter<etcd::Event::EventType>
    {
        constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin())
        {
            return ctx.begin(); // 简单实现，不处理格式说明符
        }

        template <typename FormatContext>
        auto format(const etcd::Event::EventType type, FormatContext &ctx) const
            -> decltype(ctx.out())
        {
            using IntegralType = std::underlying_type_t<etcd::Event::EventType>;
            const auto intType = static_cast<IntegralType>(type);

            const char *str = "Unknown";
            switch (intType)
            {
            case static_cast<IntegralType>(etcd::Event::EventType::PUT):
                str = "PUT";
                break;
            case static_cast<IntegralType>(etcd::Event::EventType::DELETE_):
                str = "DELETE";
                break;
            default:
                str = "INVALID";
            }

            return fmt::format_to(ctx.out(), "{}", str);
        }
    };
}

namespace cloud_disk
{
    namespace Etcd
    {
        // 服务注册客户端类
        class Registry
        {
        public:
            using registryPtr = std::shared_ptr<Registry>;

        public:
            Registry(const std::string &host, const char time = 10)
                : _client(std::make_shared<etcd::Client>(host)),
                  _keepAlive(_client->leasekeepalive(time).get()), // _keepAlive 构造依靠 _client, 故声明顺序要注意
                  _leaseId(_keepAlive->Lease())                    // _leaseId 依赖 _keepAlive, 故声明顺序要注意
            {
            }

            auto registry(const std::string &key, const std::string &value) -> bool
            {
                auto response = _client->put(key, value, _leaseId).get();
                if (response.is_ok() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "etcd registry error: {}", response.error_message());
                    return false;
                }
                return true;
            }

            ~Registry()
            {
                if (_keepAlive)
                {
                    _keepAlive->Cancel(); // 停止租约续期
                }
            }

        private:
            std::shared_ptr<etcd::Client> _client = nullptr;
            std::shared_ptr<etcd::KeepAlive> _keepAlive = nullptr;
            uint64_t _leaseId = 0; // 租约 id
        };

        // 服务发现客户端类
        class Discovery
        {
        private:
            using notifyCallback = std::function<void(std::string, std::string)>;

        public:
            using discoveryPtr = std::shared_ptr<Discovery>;

        public:
            Discovery(const std::string &host,
                      const std::string &baseDir = "",
                      const notifyCallback &putCb = nullptr,
                      const notifyCallback &delCb = nullptr)
                : _client(std::make_shared<etcd::Client>(host)),
                  _watcher(std::make_shared<etcd::Watcher>(*_client.get(),
                                                           baseDir,
                                                           std::bind(&Discovery::callBack, this, std::placeholders::_1),
                                                           true)),
                  _baseDir(baseDir),
                  _putCb(putCb),
                  _delCb(delCb)
            {
                // 先进行服务发现, 先获取到当前已有的数据
                auto response = _client->ls(_baseDir).get();
                if (response.is_ok() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to get response: {}", response.error_message());
                }

                // 进行事件监控, 监控数据发生的改变, 调用回调函数处理 -- _watcher 在构造时已经开始监控了
                int size = response.keys().size();
                for (int i = 0; i < size; ++i)
                {
                    if (_putCb)
                    {
                        _putCb(response.key(i), response.value(i).as_string());
                    }
                    else
                    {
                        Log::lg("warning", Log::FileName(), Log::Line(),
                                "No put callback function provided, cannot handle existing data");
                    }
                }
            }

            ~Discovery()
            {
                if (_watcher)
                {
                    _watcher->Cancel(); // 停止事件监控
                }
            }

        private:
            auto callBack(const etcd::Response &response) -> void
            {
                if (response.is_ok() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to get response: {}", response.error_message());
                    return;
                }

                for (const auto &e : response.events())
                {
                    if (e.event_type() == etcd::Event::EventType::PUT)
                    {
                        if (_putCb)
                        {
                            _putCb(e.kv().key(), e.kv().as_string());
                        }

                        Log::lg("debug", Log::FileName(), Log::Line(),
                                "new server: {} : {}", e.kv().key(), e.kv().as_string());
                    }
                    else if (e.event_type() == etcd::Event::EventType::DELETE_)
                    {
                        if (_delCb)
                        {
                            _delCb(e.prev_kv().key(), e.prev_kv().as_string());
                        }

                        Log::lg("debug", Log::FileName(), Log::Line(),
                                "del server: {} : {}", e.prev_kv().key(), e.prev_kv().as_string());
                    }
                    else
                    {
                        Log::lg("warning", Log::FileName(), Log::Line(),
                                "Unknown event type: {}", e.event_type());
                    }
                }
            }

        private:
            std::shared_ptr<etcd::Client> _client = nullptr;
            std::shared_ptr<etcd::Watcher> _watcher = nullptr;
            std::string _baseDir;  // 基础目录
            notifyCallback _putCb; // put 回调
            notifyCallback _delCb; // del 回调
        };
    }
}