// 测试通过

// transmite_client 测试客户端实现

// C++
#include <print>
#include <iostream>
#include <thread>
#include <functional>
#include <memory>
// transmiteServer 模块
#include "transmite_server.hpp"
// gflags
#include <gflags/gflags.h>
// gtest
#include <gtest/gtest.h>

// mysql 中数据直接写 sql 插入进行测试

// etcd brpc
DEFINE_string(registryHost, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(transmiteService, "/service/transmite_service/", "当前实例名称");

stellar_post::Brpc::ChannelManager::managerPtr _transmite_channels;

auto main(int argc, char *argv[]) -> int
{
    testing::InitGoogleTest(&argc, argv);
    // 1.进行服务发现(参数解析) -- 发现 transmite server 服务器节点地址信息并实例化通信信道
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 构建 rpc 信道管理对象
    _transmite_channels = std::make_shared<stellar_post::Brpc::ChannelManager>();
    _transmite_channels->Declared(FLAGS_transmiteService);

    auto putCb = std::bind(&stellar_post::Brpc::ChannelManager::Online, _transmite_channels.get(), std::placeholders::_1, std::placeholders::_2);
    auto delCb = std::bind(&stellar_post::Brpc::ChannelManager::Offline, _transmite_channels.get(), std::placeholders::_1, std::placeholders::_2);

    // 构建服务注册发现对象
    stellar_post::Etcd::Discovery::discoveryPtr discovery = std::make_shared<stellar_post::Etcd::Discovery>(FLAGS_registryHost,
                                                                                                            FLAGS_baseService,
                                                                                                            putCb, delCb);

    auto channel = _transmite_channels->Get(FLAGS_transmiteService);
    if (!channel)
    {
        std::println("channel is null");
        return -1;
    }

    stellar_post::base::MsgTransmiteService_Stub stub((*channel).get());
    stellar_post::transmite::NewMessageReq req;
    req.set_request_id("msg forward test 1");
    req.set_user_id("user1");
    req.set_chat_session_id("aaaaa");
    req.mutable_massage()->set_message_type(stellar_post::base::MessageType::STRING);
    req.mutable_massage()->mutable_string_message()->set_content("Hello World!");

    stellar_post::transmite::GetTransmiteTargetRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetTransmiteTarget(cntl.get(), &req, &rsp, nullptr);
    if (cntl->Failed() || rsp.success() == false)
    {
        std::println("GetUserInfo failed: {}", cntl->ErrorText());
        return -1;
    }
    std::println("msg_id: {}", rsp.message().message_id());
    std::println("chat_session_id: {}", rsp.message().chat_session_id());
    std::println("timestamp: {}", rsp.message().timestamp());
    std::println("sender: {} : {}", rsp.message().sender().user_id(), rsp.message().sender().nickname());
    std::println("message: {}", rsp.message().message().string_message().content());
    for (size_t i = 0; i < rsp.target_id_list_size(); i++)
    {
        std::println("target_id: {}", rsp.target_id_list(i));
    }

    return 0;
}