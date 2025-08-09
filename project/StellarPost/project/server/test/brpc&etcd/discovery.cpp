// 测试通过

// C++
#include <print>
#include <memory>
#include <thread>
#include <chrono>
#include <functional>
// test etcd & brpc
#include "../../../project/common/etcd/etcd.hpp"
#include "../../../project/common/brpc/brpc.hpp"
#include "brpc.pb.h"
#include "../../../project/common/log/logger.hpp"
// gflags
#include <gflags/gflags.h>

DEFINE_string(ectdHost, "127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(callService, "/service/echo", "服务监控根目录");

auto main(int argc, char *argv[]) -> int
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 构建 rpc 信道管理对象
    stellar_post::Brpc::ChannelManager::managerPtr ptr = std::make_shared<stellar_post::Brpc::ChannelManager>();
    ptr->Declared(FLAGS_callService);

    auto putCb = std::bind(&stellar_post::Brpc::ChannelManager::Online, ptr.get(), std::placeholders::_1, std::placeholders::_2);
    auto delCb = std::bind(&stellar_post::Brpc::ChannelManager::Offline, ptr.get(), std::placeholders::_1, std::placeholders::_2);

    // 构建服务注册发现对象
    stellar_post::Etcd::Discovery::discoveryPtr discovery = std::make_shared<stellar_post::Etcd::Discovery>(FLAGS_ectdHost,
                                                                                                          FLAGS_baseService,
                                                                                                          putCb, delCb);

    while (true)
    {
        // 通过 rpc 信道管理对象获取提供 echo 服务的信道
        auto channel = ptr->Get(FLAGS_callService);
        if (!channel)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // 每秒发起一个请求
            continue;
        }
        auto chan = channel.value();
        // 发起 echo 的 rpc 调用
        brpc::EchoService_Stub stub(chan.get());
        brpc::EchoRequest request;
        brpc::EchoResponse *response = new brpc::EchoResponse(); // 创建响应对象, 用于接收服务器响应
        request.set_message("Hello World!");                     // 设置请求消息内容
        brpc::Controller *controller = new brpc::Controller();   // 创建 rpc 控制器 / 上下文, 用于控制 rpc 调用
        // 同步
        stub.Echo(controller, &request, response, nullptr); // 同步调用 Echo 方法, 异步请求最后一个 Closure 需要设置
        if (controller->Failed())                           // 检查 rpc 调用是否失败
        {
            std::cerr << "RPC 调用失败: " << controller->ErrorText() << std::endl;
            delete controller; // 删除控制器对象, 释放资源
            delete response;   // 删除响应对象, 释放资源

            std::this_thread::sleep_for(std::chrono::seconds(1)); // 每秒发起一个请求
            continue;
        }
        else
        {
            std::println("收到响应: {}", response->message());

            delete controller; // 删除控制器对象, 释放资源
            delete response;   // 删除响应对象, 释放资源

            break;
        }
    }

    return 0;
}