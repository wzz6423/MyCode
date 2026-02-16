// 测试通过

// C++
#include <print>
#include <thread>
#include <chrono>
#include <memory>
#include <functional>
// Other
#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include "../../../project/common/etcd/etcd.hpp"
#include "../../../project/common/grpc/grpc.hpp"
#include "grpc.pb.h"
#include "grpc.grpc.pb.h"

// 定义命令行参数
DEFINE_string(etcdHost, "127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(callService, "/service/echo", "需要调用的服务路径");

auto main(int argc, char *argv[]) -> int
{
    // 解析命令行参数
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 1. 创建 gRPC 信道管理对象
    std::shared_ptr<cloud_disk::Grpc::ChannelManager> channelManager =
        std::make_shared<cloud_disk::Grpc::ChannelManager>();

    // 声明需要关注的服务
    channelManager->Declared(FLAGS_callService);

    // 2. 构建 etcd 服务发现对象
    // 定义回调函数，当 etcd 中的服务实例发生变化时触发
    auto putCallback = std::bind(&cloud_disk::Grpc::ChannelManager::Online,
                                 channelManager.get(),
                                 std::placeholders::_1,
                                 std::placeholders::_2);
    auto delCallback = std::bind(&cloud_disk::Grpc::ChannelManager::Offline,
                                 channelManager.get(),
                                 std::placeholders::_1,
                                 std::placeholders::_2);

    std::shared_ptr<cloud_disk::Etcd::Discovery> discovery =
        std::make_shared<cloud_disk::Etcd::Discovery>(
            FLAGS_etcdHost,    // etcd 服务器地址
            FLAGS_baseService, // 服务监控根目录
            putCallback,       // 服务实例上线回调
            delCallback        // 服务实例下线回调
        );

    // 3. 定期尝试调用服务
    while (true)
    {
        // 从信道管理器获取服务信道
        auto channel = channelManager->Get(FLAGS_callService);

        if (!channel)
        {
            // 如果没有可用信道，等待 1 秒后重试
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        // 获取信道对象
        auto grpcChannel = channel.value();

        // 4. 创建 gRPC 客户端存根（Stub）
        grpc::EchoService::Stub *stub = grpc::EchoService::NewStub(grpcChannel).get();

        // 构建请求对象
        grpc::EchoRequest request;
        request.set_message("Hello from discovery client!");

        // 创建响应对象和控制器
        grpc::EchoResponse response;
        grpc::ClientContext context;

        // 5. 发起同步 RPC 调用
        grpc::Status status = stub->Echo(&context, request, &response);

        // 检查 RPC 调用是否成功
        if (!status.ok())
        {
            std::cerr << "RPC 调用失败: " << status.error_message() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        else
        {
            // 输出响应结果
            std::println("收到响应: {}", response.message());
            break;
        }
    }

    return 0;
}
