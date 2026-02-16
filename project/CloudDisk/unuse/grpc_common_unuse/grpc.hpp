#pragma once

/*
    grpc 要链接的库太多了, 具体的搜索指令看 example 中的 grpc 目录下的 Makefile 和 CMakeLists.txt
    弃用
*/

// C++
#include <algorithm>     // 算法操作
#include <string>        // 字符串处理
#include <vector>        // 动态数组
#include <unordered_set> // 哈希集合
#include <unordered_map> // 哈希映射
#include <memory>        // 智能指针
#include <mutex>         // 互斥锁
#include <optional>      // 可选类型
#include <chrono>        // 时间处理
// gRPC
#include <grpcpp/grpcpp.h>                 // gRPC核心功能
#include <grpcpp/channel.h>                // gRPC通道类
#include <grpcpp/client_context.h>         // gRPC客户端上下文
#include <grpcpp/create_channel.h>         // 创建通道工具
#include <grpcpp/impl/channel_interface.h> // 通道接口
// log
#include "../log/logger.hpp" // 日志模块封装

namespace cloud_disk
{
    /*
    grpc.hpp: gRPC 服务通信管理框架
    提供基于 gRPC 的服务通信管理功能, 支持服务注册、发现、负载均衡和健康监测

    依赖项:
    - gRPC (grpcpp)
    - 线程安全: 使用 std::mutex
    - 日志系统: cloud_disk_Log 命名空间中的 logger

    功能特点:
    1. 服务注册和发现: 管理服务实例的上下线
    2. 负载均衡: 轮询算法分配请求
    3. 自动健康监测: 检测并移除不可用服务
    4. 线程安全: 所有操作均保证线程安全
    */
    namespace Grpc
    {
        /*
        Channel: 单个服务的 gRPC 信道管理类
        管理一个服务的多个服务实例连接, 提供服务实例的增删查功能,
        使用轮询算法进行负载均衡, 并自动监测连接健康状态
        */
        class Channel
        {
        public:
            // 类型别名
            using grpcChannelPtr = std::shared_ptr<grpc::Channel>; // gRPC通道指针
            using channelPtr = std::shared_ptr<Channel>;           // 本类指针

        public:
            /*
            构造函数: serviceName 服务名称标识符
            */
            explicit Channel(const std::string &serviceName = "")
                : _service_name(serviceName), // 初始化服务名称
                  _worker(0)                  // 轮询计数器归零
            {
            }

            /*
            添加服务实例
            host 服务实例地址(格式: ip:port)
            是否添加成功

            1. 创建新的gRPC通道
            2. 注册健康状态监听器
            3. 将新通道添加到管理列表
            */
            auto Add(const std::string &host) -> bool
            {
                grpcChannelPtr newChannel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
                // grpc::InsecureChannelCredentials 是 gRPC C++ 库中提供的一种凭证类型, 用于创建​​不安全的​​通信通道
                // 表示客户端和服务器之间的连接​​没有加密和身份验证
                // 实际生产环境必须加密

                // 添加通道
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    _channels.push_back(newChannel);
                    _channel_map[host] = newChannel;
                    _channel_states[host] = true; // 初始状态设为健康
                }

                // 如果健康检查线程未启动, 启动它
                startHealthCheckThread();

                Log::lg("info", Log::FileName(), Log::Line(),
                        "gRPC channel added: {} -> {}", _service_name, host);
                return true;
            }

            /*
            移除服务实例
            host 服务实例地址
            是否移除成功

            1. 查找对应通道
            2. 从所有管理结构中移除
            3. 释放相关资源
            */
            auto Delete(const std::string &host) -> bool
            {
                std::unique_lock<std::mutex> lock(_mtx);
                auto it = _channel_map.find(host);
                if (it == _channel_map.end())
                {
                    Log::lg("warning", Log::FileName(), Log::Line(),
                            "gRPC channel not found for deletion: {} -> {}", _service_name, host);
                    return false;
                }

                // 移除通道
                grpcChannelPtr channel = it->second;
                // 从轮询列表中移除
                _channels.erase(
                    std::remove(_channels.begin(), _channels.end(), channel),
                    _channels.end());
                _channel_map.erase(it); // 从映射表中移除

                // 从状态跟踪中移除
                _channel_states.erase(host);

                // 更新轮询计数器(防止越界)
                if (_worker >= _channels.size())
                {
                    _worker = 0;
                }

                Log::lg("info", Log::FileName(), Log::Line(),
                        "gRPC channel removed: {} -> {}", _service_name, host);
                return true;
            }

            /*
            获取可用通道(轮询算法)
            可用通道的智能指针

            使用 Round-Robin 算法轮流返回服务实例, 确保请求均匀分布到所有服务实例
            */
            auto Get() -> std::optional<std::shared_ptr<grpc::Channel>>
            {
                std::unique_lock<std::mutex> lock(_mtx);
                if (_channels.empty())
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "No available gRPC channels for service: {}", _service_name);
                    return std::nullopt; // 无可用通道
                }

                // 轮询选择下一个通道
                std::shared_ptr<grpc::Channel> channel = _channels[_worker];
                _worker = (_worker + 1) % _channels.size();
                return channel;
            }

            ~Channel()
            {
                // 安全停止健康检查线程
                {
                    std::lock_guard<std::mutex> lock(_health_mtx);
                    _stopHealthCheck = true;
                }
                _healthCV.notify_all();
                if (_health_thread.joinable())
                {
                    _health_thread.join();
                }
            }

        private:
            // 启动健康检查线程
            auto startHealthCheckThread() -> void
            {
                if (_health_thread.joinable())
                {
                    return;
                }

                // 使用lambda表达式捕获this指针而不是拷贝原子变量
                _health_thread = std::thread([this]
                                             {
            while (!_stopHealthCheck)
            {
                {
                    std::unique_lock<std::mutex> lock(_health_mtx);
                    if (_stopHealthCheck) break;
                    
                    // 每隔10秒检查一次
                    _healthCV.wait_for(lock, std::chrono::seconds(10),
                                       [this] { return _stopHealthCheck.load(); });
                    if (_stopHealthCheck) break;
                }
                
                // 执行健康检查
                performHealthCheck();
            } });
            }

            // 执行实际健康检查
            auto performHealthCheck() -> void
            {
                std::unordered_map<std::string, bool> currentStates;
                std::vector<std::string> toRemove;

                // 第一步: 收集当前通道并检查状态
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    for (const auto &[host, channel] : _channel_map) // C++17 结构化绑定
                    {
                        // 获取当前连接状态
                        grpc_connectivity_state state = channel->GetState(true);
                        bool isHealthy = (state == GRPC_CHANNEL_READY || state == GRPC_CHANNEL_IDLE);

                        // 状态变化日志
                        if (isHealthy != _channel_states[host])
                        {
                            const char *stateName = "UNKNOWN";
                            switch (state)
                            {
                            case GRPC_CHANNEL_IDLE:
                                stateName = "IDLE";
                                break;
                            case GRPC_CHANNEL_READY:
                                stateName = "READY";
                                break;
                            case GRPC_CHANNEL_CONNECTING:
                                stateName = "CONNECTING";
                                break;
                            case GRPC_CHANNEL_TRANSIENT_FAILURE:
                                stateName = "TRANSIENT_FAILURE";
                                break;
                            case GRPC_CHANNEL_SHUTDOWN:
                                stateName = "SHUTDOWN";
                                break;
                            default:
                                break;
                            }
                            Log::lg("info", Log::FileName(), Log::Line(),
                                    "gRPC channel state: {} -> {}", host, stateName);
                        }

                        currentStates[host] = isHealthy;

                        // 标记需要移除的通道
                        if (state == GRPC_CHANNEL_TRANSIENT_FAILURE || state == GRPC_CHANNEL_SHUTDOWN)
                        {
                            toRemove.push_back(host);
                        }
                    }

                    // 更新状态映射
                    _channel_states = currentStates;
                }

                // 第二步: 在锁定区域外移除不健康通道
                for (const auto &host : toRemove)
                {
                    Log::lg("warning", Log::FileName(), Log::Line(),
                            "Removing unhealthy gRPC channel: {}", host);
                    Delete(host); // 调用公开方法移除通道
                }
            }

        private:
            std::mutex _mtx;                                                              // 互斥锁, 保证线程安全
            std::string _service_name;                                                    // 管理的服务名称
            std::vector<std::shared_ptr<grpc::Channel>> _channels;                        // 通道列表(用于轮询)
            std::unordered_map<std::string, std::shared_ptr<grpc::Channel>> _channel_map; // 地址到通道的映射
            int32_t _worker;                                                              // 轮询计数器

            // 健康检查相关
            std::thread _health_thread;                // 健康检查线程
            std::mutex _health_mtx;                    // 健康检查专用锁
            std::condition_variable _healthCV;         // 条件变量
            std::atomic<bool> _stopHealthCheck{false}; // 停止标志

            // 通道健康状态追踪
            std::unordered_map<std::string, bool> _channel_states; // host -> isHealthy
        };

        /*
        ChannelManager: gRPC 服务信道全局管理器

        管理系统中所有服务的通信信道, 提供:
        - 服务注册
        - 服务发现
        - 服务上下线处理
        */
        class ChannelManager
        {
        public:
            using managerPtr = std::shared_ptr<ChannelManager>; // 管理器智能指针

        public:
            ChannelManager() = default;

            /*
            声明需要管理的服务
            serviceName 服务名称

            将服务添加到关注列表, 只有声明过的服务才会被管理
            */
            auto Declared(const std::string &serviceName) -> void
            {
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    _care_services.insert(serviceName);
                }
                Log::lg("debug", Log::FileName(), Log::Line(),
                        "Service declared: {}", serviceName);
            }

            /*
            检查服务是否已声明
            serviceName 服务名称
            */
            auto IsDeclared(const std::string &serviceName) -> bool
            {
                std::unique_lock<std::mutex> lock(_mtx);
                return _care_services.find(serviceName) != _care_services.end();
            }

            /*
            获取服务的通信通道
            serviceName 服务名称

            1. 检查服务是否已声明
            2. 查找对应的通道管理器
            3. 使用轮询算法返回通道
            */
            auto Get(const std::string &serviceName) -> std::optional<std::shared_ptr<grpc::Channel>>
            {
                // 检查服务声明状态
                if (!IsDeclared(serviceName))
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Service not declared: {}", serviceName);
                    return std::nullopt;
                }

                std::shared_ptr<Channel> channelManager;
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    auto it = _services.find(serviceName);
                    // 服务未找到或管理器未初始化
                    if (it == _services.end() || it->second == nullptr)
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "No channel manager for service: {}", serviceName);
                        return std::nullopt;
                    }
                    channelManager = it->second;
                }

                return channelManager->Get();
            }

            /*
            处理服务上线事件
            serviceIns 服务实例完整标识符(service/instance)
            host 服务实例地址

            调用场景: 从服务注册中心(etcd)接收到服务上线通知
            1. 提取服务名称
            2. 检查是否声明
            3. 创建或获取通道管理器
            4. 添加新实例
            */
            auto Online(const std::string &serviceIns, const std::string &host) -> bool
            {
                // 从完整标识符提取服务名称
                std::string serviceName = extractServiceName(serviceIns);

                if (!IsDeclared(serviceName))
                {
                    Log::lg("warning", Log::FileName(), Log::Line(),
                            "Undeclared service online: {}", serviceName);
                    return false;
                }

                std::shared_ptr<Channel> channelManager;
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    // 获取或创建通道管理器
                    auto &manager = _services[serviceName];
                    if (!manager)
                    {
                        manager = std::make_shared<Channel>(serviceName);
                        Log::lg("info", Log::FileName(), Log::Line(),
                                "Created channel manager for service: {}", serviceName);
                    }
                    channelManager = manager;
                }

                // 添加服务实例
                if (!channelManager->Add(host))
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "Failed to add gRPC channel: {} -> {}", serviceName, host);
                    return false;
                }

                Log::lg("info", Log::FileName(), Log::Line(),
                        "Service instance online: {} @ {}", serviceIns, host);
                return true;
            }

            /*
            处理服务下线事件
            serviceIns 服务实例标识符
            host 服务实例地址

            调用场景: 从服务注册中心接收到服务下线通知
            */
            bool Offline(const std::string &serviceIns, const std::string &host)
            {
                std::string serviceName = extractServiceName(serviceIns);

                if (!IsDeclared(serviceName))
                {
                    Log::lg("warning", Log::FileName(), Log::Line(),
                            "Undeclared service offline: {}", serviceName);
                    return false;
                }

                std::shared_ptr<Channel> channelManager;
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    auto it = _services.find(serviceName);
                    if (it == _services.end() || !it->second)
                    {
                        Log::lg("warning", Log::FileName(), Log::Line(),
                                "Channel manager not found for: {}", serviceName);
                        return false;
                    }
                    channelManager = it->second;
                }

                bool result = channelManager->Delete(host);
                if (result)
                {
                    Log::lg("info", Log::FileName(), Log::Line(),
                            "Service instance offline: {} @ {}", serviceIns, host);
                }
                return result;
            }

        private:
            /*
            从实例标识符中提取服务名称
            serviceIns 完整实例标识符(格式: service/instance)
            服务名称

            示例:
            输入: "UserService/A123" -> 返回: "UserService"
            */
            auto extractServiceName(const std::string &serviceIns) -> std::string
            {
                // 查找最后一个分隔符
                size_t pos = serviceIns.find_last_of('/');
                // 未找到分隔符则返回原字符串
                return (pos == std::string::npos)
                           ? serviceIns
                           : serviceIns.substr(0, pos);
            }

        private:
            std::mutex _mtx;                                                     // 互斥锁
            std::unordered_set<std::string> _care_services;                      // 关注的服务集合
            std::unordered_map<std::string, std::shared_ptr<Channel>> _services; // 服务管理映射
        };

        /*
        @use_example:

        // 声明关注的服务
        auto channelManager = std::make_shared<Grpc::GrpcChannelManager>();
        channelManager->Declared("UserService");

        // 服务上线
        channelManager->Online("UserService/instance1", "localhost:50051");

        // 获取信道
        auto channel = channelManager->Get("UserService");
        if (channel.has_value()) {
            // 创建gRPC stub并使用channel
            auto stub = UserService::NewStub(*channel);
            grpc::ClientContext context;
            UserRequest request;
            UserResponse response;
            grpc::Status status = stub->GetUser(&context, request, &response);
        }

        // 服务下线
        channelManager->Offline("UserService/instance1", "localhost:50051");

        */
    }
}