/*
    websocket 模块封装
*/

#pragma once

// C++
#include <string>
#include <memory>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <optional>
// websocket
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace stellar_post
{
    namespace gateway
    {
        // 定义服务器 server 类型
        using Server_t = websocketpp::server<websocketpp::config::asio>;
        // 连接的类型: Server_t::connection_ptr

        class Connection
        {
        public:
            using connPtr = std::shared_ptr<Connection>; // 连接对象智能指针

            // 客户端信息
            struct ClientInfo
            {
            public:
                std::string _user_id;    // 用户 ID
                std::string _session_id; // 会话 ID

                // 构造函数
                ClientInfo(const std::string &user_id, const std::string &session_id)
                    : _user_id(user_id), _session_id(session_id)
                {
                }

                // 析构函数
                ~ClientInfo() = default;
            };

        public:
            // 构造函数
            Connection() = default;

            // 析构函数
            ~Connection() = default;

            // 添加新连接
            auto Add(const std::string &user_id,
                     const std::string &session_id,
                     const Server_t::connection_ptr &conn) -> void
            {
                {
                    std::lock_guard<std::mutex> lock(_mtx);
                    _uid_connections[user_id] = conn;
                    _connection_client[conn] = std::make_shared<ClientInfo>(user_id, session_id);
                }
            }

            // 获取连接
            auto Get(const std::string &user_id) -> std::optional<Server_t::connection_ptr>
            {
                {
                    std::lock_guard<std::mutex> lock(_mtx);
                    if (_uid_connections.find(user_id) == _uid_connections.end())
                    {
                        return std::nullopt;
                    }
                    return std::make_optional(_uid_connections[user_id]);
                }
            }

            // 通过连接获取客户端信息
            auto GetClientInfo(const Server_t::connection_ptr &conn) -> std::optional<std::unique_ptr<ClientInfo>>
            {
                {
                    std::lock_guard<std::mutex> lock(_mtx);
                    if (_connection_client.find(conn) == _connection_client.end())
                    {
                        return std::nullopt;
                    }
                    return std::make_optional(std::make_unique<ClientInfo>(*_connection_client[conn]));
                }
            }

            // 删除
            auto Remove(const Server_t::connection_ptr &conn) -> void
            {
                {
                    std::lock_guard<std::mutex> lock(_mtx);
                    auto it = _connection_client.find(conn);
                    if (it == _connection_client.end())
                    {
                        return;
                    }
                    _uid_connections.erase(it->second->_user_id);
                    _connection_client.erase(it);
                }
            }

        private:
            // 线程安全
            std::mutex _mtx;

            // uid-连接
            std::unordered_map<std::string, Server_t::connection_ptr> _uid_connections;

            // 连接-客户端信息
            std::unordered_map<Server_t::connection_ptr, std::shared_ptr<ClientInfo>> _connection_client;
        };
    }
}