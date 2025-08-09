#pragma once

/*
    -lbrpc -lprotobuf -lpthread -lleveldb -lssl -lcrypto
*/

// C++
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <optional>
// brpc
#include <brpc/server.h>
#include <butil/logging.h>
#include <brpc/channel.h>
// Other
#include "../log/logger.hpp"

namespace stellar_post
{
    namespace Brpc
    {
        // 单个服务的信道管理类
        // 建立服务与信道的映射关系
        class Channel
        {
        public:
            using brpcChannelPtr = std::shared_ptr<brpc::Channel>;
            using channelPtr = std::shared_ptr<Channel>;

        public:
            Channel(const std::string &serviceName = "")
                : _serviceName(serviceName),
                  _worker(0)
            {
            }

            // 添加上线服务, 新增信道
            auto Add(const std::string &host) -> bool
            {
                // 构造 channel
                auto newChannel = std::make_shared<brpc::Channel>();
                brpc::ChannelOptions options;
                options.connect_timeout_ms = -1;                    // 设置连接超时时间为 -1 毫秒, 连接超时一直等待
                options.timeout_ms = -1;                            // 设置 rpc 请求超时时间为 -1 毫秒, 请求超时一直等待
                options.max_retry = 3;                              // 设置最大重试次数为 3 次, 超过次数后返回错误
                options.protocol = "baidu_std";                     // 设置序列化协议为 baidu_std, 这是 brpc 默认的协议
                int ret = newChannel->Init(host.c_str(), &options); // 初始化 channel, 连接到服务器地址
                if (ret == -1)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "init channel error! host: {}, service: {}", host, _serviceName);
                    return false;
                }

                // 添加服务到服务集合
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    _channels.push_back(newChannel); // 添加信道到集合
                    _channelMap[host] = newChannel;  // 添加主机地址与信道的映射关系
                }

                return true;
            }

            // 删除下线服务, 释放信道
            auto Delte(const std::string &host) -> bool
            {
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    auto it = _channelMap.find(host);
                    if (it != _channelMap.end())
                    {
                        // 找到对应信道, 从集合中删除
                        brpcChannelPtr channel = it->second;
                        // _channels.erase(std::remove(_channels.begin(), _channels.end(), channel), _channels.end());
                        std::erase(_channels, channel);
                        _channelMap.erase(it); // 删除映射关系
                        return true;
                    }
                    else
                    {
                        Log::lg("wrning", Log::FileName(), Log::Line(),
                                "delete channel not found! host: {}, service: {}", host, _serviceName);
                        return false;
                    }
                }
            }

            // 获取工作服务 -- 采用 RR 轮转
            auto Get() -> std::optional<brpcChannelPtr>
            {
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    if (_channels.empty())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "no available channels for service: {}", _serviceName);
                        return std::nullopt; // 没有可用信道
                    }
                    else
                    {
                        // 轮转获取信道
                        brpcChannelPtr channel = _channels[_worker];
                        _worker = (_worker + 1) % _channels.size(); // 更新轮转下标
                        return channel;                             // 返回当前信道
                    }
                }
            }

        private:
            std::mutex _mtx;                                             // 保证线程安全
            std::string _serviceName;                                    // 服务名称
            std::vector<brpcChannelPtr> _channels;                       // 当前服务对应信道集合 -- 便于 RR 轮转
            std::unordered_map<std::string, brpcChannelPtr> _channelMap; // 主机地址与信道的映射关系
            int32_t _worker;                                             // 当前轮转下标计数器 -- 便于 RR 轮转
        };

        // 服务信道管理类
        class ChannelManager
        {
        public:
            using managerPtr = std::shared_ptr<ChannelManager>;

        public:
            ChannelManager()
            {
            }

            // 声明关注上下线的服务, 不关注的不管理
            auto Declared(const std::string &serviceName) -> void
            {
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    _careServices.insert(serviceName); // 添加到关注的服务集合
                }
            }

            auto IsDeclared(const std::string &serviceName) -> bool
            {
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    return _careServices.find(serviceName) != _careServices.end(); // 检查服务是否在关注集合中
                }
            }

            // 通过服务名称获取服务的通信信道
            auto Get(const std::string &serviceName) -> std::optional<Channel::brpcChannelPtr>
            {
                if (IsDeclared(serviceName) == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "service not declared: {}", serviceName);
                    return std::nullopt; // 服务未关注
                }

                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    if (_services.empty())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "no available channels for service: {}", serviceName);
                        return std::nullopt; // 没有可用信道
                    }
                    else
                    {
                        auto it = _services.find(serviceName);
                        if (it != _services.end())
                        {
                            return it->second->Get(); // 返回对应信道
                        }
                        else
                        {
                            Log::lg("error", Log::FileName(), Log::Line(), "service not found: {}", serviceName);
                            return std::nullopt; // 服务未找到
                        }
                    }
                }
            }

            // 用于 etcd 的 _putCb
            // 服务节点上线, 管理服务节点
            auto Online(const std::string &serviceIns, const std::string &host) -> bool
            {
                std::string serviceName = getServiceName(serviceIns);
                if (IsDeclared(serviceName) == false)
                {
                    Log::lg("warning", Log::FileName(), Log::Line(), "service not declared: {}", serviceName);
                    return false; // 服务未关注
                }

                Channel::channelPtr addPtr;
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    // 获取管理对象, 有则添加节点, 没有则创建
                    auto it = _services.find(serviceName);
                    if (it == _services.end())
                    {
                        // 创建新的信道管理对象
                        addPtr = std::make_shared<Channel>(serviceName);
                        _services[serviceName] = addPtr; // 添加到服务映射关系
                    }
                    else
                    {
                        addPtr = it->second; // 获取已存在的信道管理对象
                    }
                }

                if (!addPtr)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "failed to get channel manager for service: {}", serviceName);
                    return false; // 获取信道管理对象失败
                }
                Log::lg("debug", Log::FileName(), Log::Line(), "{} : {} 服务上线", serviceName, host);
                return addPtr->Add(host); // 添加服务节点到信道管理对象
            }

            // 用于 etcd 的 _delCb
            // 服务节点下线, 从服务信道管理中删除指定节点信道
            auto Offline(const std::string &serviceIns, const std::string &host) -> bool
            {
                std::string serviceName = getServiceName(serviceIns);

                if (IsDeclared(serviceName) == false)
                {
                    Log::lg("warning", Log::FileName(), Log::Line(), "service not declared: {}", serviceName);
                    return false; // 服务未关注
                }

                Channel::channelPtr delPtr;
                {
                    std::unique_lock<std::mutex> lock(_mtx);

                    // 先找管理对象
                    auto it = _services.find(serviceName);
                    if (it == _services.end())
                    {
                        Log::lg("warning", Log::FileName(), Log::Line(),
                                "service not found when delete: {}", serviceName);
                        return false; // 服务未找到
                    }

                    delPtr = it->second; // 获取信道管理对象
                    if (!delPtr)
                    {
                        Log::lg("warning", Log::FileName(), Log::Line(),
                                "channel manager not found for service when delete: {}", serviceName);
                        return false; // 信道管理对象未找到
                    }
                }
                Log::lg("debug", Log::FileName(), Log::Line(), "{} : {} 服务下线", serviceName, host);
                return delPtr->Delte(host); // 从信道管理对象中删除指定节点信道
            }

        private:
            std::string getServiceName(const std::string &serviceIns)
            {
                auto pos = serviceIns.find_last_of('/');
                return serviceIns.substr(0, pos);
            }

        private:
            std::mutex _mtx;                                                // 保证线程安全
            std::unordered_set<std::string> _careServices;                  // 关注的服务名称集合
            std::unordered_map<std::string, Channel::channelPtr> _services; // 服务名称与信道管理类的映射关系
        };
    }
}