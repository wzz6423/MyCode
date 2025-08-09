#pragma once

/*
    -lamqpcpp -lev
*/

// C++
#include <string>
#include <functional>
#include <memory>
#include <thread>
// RabbitMQ
#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
// Other
#include "../log/logger.hpp" // 日志器 spdlog 模块封装

namespace stellar_post
{
    namespace mqclient
    {
        class RabbitMQ
        {
        public:
            using MessageCallBack = std::function<void(const char *, size_t)>;
            using mqClientPtr = std::shared_ptr<RabbitMQ>;

        public:
            RabbitMQ(const std::string &user,
                     const std::string &passwd,
                     const std::string &host)
            {
                _loop = EV_DEFAULT;
                _handler = std::make_unique<AMQP::LibEvHandler>(_loop);
                std::string url = "amqp://" + user + ":" + passwd + "@" + host + "/";
                _connection = std::make_unique<AMQP::TcpConnection>(_handler.get(), AMQP::Address(url));
                _channel = std::make_unique<AMQP::TcpChannel>(_connection.get());

                _loop_thread = std::thread([this]()
                                           { ev_run(_loop, 0); });
            }

            // 声明交换机、队列、绑定
            auto DeclareComponents(const std::string &exchange,
                                   const std::string &queue,
                                   const std::string &routingKey = "default-routing-key",
                                   AMQP::ExchangeType exchangeType = AMQP::ExchangeType::direct) -> void
            {
                // 1.声明交换机
                auto &exchange_deferred = _channel->declareExchange(exchange, exchangeType);
                exchange_deferred.onError([exchange](const char *msg)
                                          { Log::lg("error", Log::FileName(), Log::Line(),
                                                    "声明交换机 {} 失败: {}", exchange, msg); });
                exchange_deferred.onSuccess([exchange]()
                                            { Log::lg("info", Log::FileName(), Log::Line(),
                                                      "声明交换机 {} 成功", exchange); });

                // 2.声明队列
                auto &queue_deferred = _channel->declareQueue(queue);
                queue_deferred.onError([queue](const char *msg)
                                       { Log::lg("error", Log::FileName(), Log::Line(),
                                                 "声明队列 {} 失败: {}", queue, msg); });
                queue_deferred.onSuccess([queue]()
                                         { Log::lg("info", Log::FileName(), Log::Line(),
                                                   "声明队列 {} 成功", queue); });

                // 3.针对交换机和队列进行绑定
                auto &binding_deferred = _channel->bindQueue(exchange, queue, routingKey);
                binding_deferred.onError([&](const char *msg)
                                         { Log::lg("error", Log::FileName(), Log::Line(),
                                                   "绑定交换机 {} 队列 {} 失败: {}", exchange, queue, msg); });
                binding_deferred.onSuccess([&]()
                                           { Log::lg("info", Log::FileName(), Log::Line(),
                                                     "绑定交换机 {} 队列 {} 成功", exchange, queue); });
            }

            // 发布消息
            auto Publish(const std::string &exchange,
                         const std::string &message,
                         const std::string &routingKey = "default-routing-key") -> bool
            {
                if (!_connection->ready())
                {
                    Log::lg("warning", Log::FileName(), Log::Line(), "Connection not ready");
                }

                try
                {
                    bool ret = _channel->publish(exchange, routingKey, message);
                    if (!ret)
                    {
                        Log::lg("error", Log::FileName(), Log::Line(), "{} : {} 发布消息失败", exchange, routingKey);
                    }
                    return ret;
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "发布消息时发生异常: {}", e.what());
                    return false;
                }
            }

            // 消费消息
            auto Consume(const std::string &queue,
                         const MessageCallBack &callback,
                         const std::string &tag = "default-consumer-tag") -> void
            {
                auto &consume_deferred = _channel->consume(queue, tag);
                consume_deferred.onError([&](const char *msg)
                                         { Log::lg("error", Log::FileName(), Log::Line(),
                                                   "{} 订阅队列 {} 失败", queue, msg); });
                consume_deferred.onReceived([&](const AMQP::Message &message,
                                                uint64_t deliveryTag,
                                                bool redelivered)
                                            {
                                                callback(message.body(), message.bodySize());
                                              _channel->ack(deliveryTag); });
            }

            ~RabbitMQ()
            {
                ev_async_init(&_async_watcher, asyncWatcherCallback);
                ev_async_start(_loop, &_async_watcher);
                ev_async_send(_loop, &_async_watcher);
                _loop_thread.join();
            }

        private:
            static auto asyncWatcherCallback(struct ev_loop *loop, ev_async *watcher, int revents) -> void
            {
                // 处理异步事件
                ev_break(loop, EVBREAK_ALL);
            }

        private:
            struct ev_async _async_watcher;
            struct ev_loop *_loop;
            std::unique_ptr<AMQP::LibEvHandler> _handler;
            std::unique_ptr<AMQP::TcpConnection> _connection;
            std::unique_ptr<AMQP::TcpChannel> _channel;
            std::thread _loop_thread;
        };
    }
}