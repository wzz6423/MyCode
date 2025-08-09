// 测试通过

// message_client 测试客户端实现
// 本模块测试为联调 -- file, user, transmite, message

// C++
#include <print>
#include <iostream>
#include <thread>
#include <functional>
#include <memory>
// messageServer 模块
#include "transmite_server.hpp"
// gflags
#include <gflags/gflags.h>
// gtest
#include <gtest/gtest.h>
// Other
#include "utils.hpp"
#include "transmite_server.hpp"

// etcd brpc
DEFINE_string(registryHost, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(transmiteService, "/service/transmite_service/", "当前实例名称");

stellar_post::Brpc::ChannelManager::managerPtr _transmite_channels;

auto stringMsg(const std::string &uid, const std::string &sid, const std::string &msg) -> void
{
    auto channel = _transmite_channels->Get(FLAGS_transmiteService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::base::MsgTransmiteService_Stub stub((*channel).get());
    stellar_post::transmite::NewMessageReq req;
    req.set_request_id(Utils::Uuid());
    req.set_user_id(uid);
    req.set_chat_session_id(sid);
    req.mutable_massage()->set_message_type(stellar_post::base::MessageType::STRING);
    req.mutable_massage()->mutable_string_message()->set_image_content(msg);

    stellar_post::transmite::GetTransmiteTargetRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetTransmiteTarget(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
}

auto imageMsg(const std::string &uid, const std::string &sid, const std::string &msg) -> void
{
    auto channel = _transmite_channels->Get(FLAGS_transmiteService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::base::MsgTransmiteService_Stub stub((*channel).get());
    stellar_post::transmite::NewMessageReq req;
    req.set_request_id(Utils::Uuid());
    req.set_user_id(uid);
    req.set_chat_session_id(sid);
    req.mutable_massage()->set_message_type(stellar_post::base::MessageType::IMAGE);
    req.mutable_massage()->mutable_image_message()->set_content(msg);

    stellar_post::transmite::GetTransmiteTargetRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetTransmiteTarget(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
}

auto speechMsg(const std::string &uid, const std::string &sid, const std::string &msg) -> void
{
    auto channel = _transmite_channels->Get(FLAGS_transmiteService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::base::MsgTransmiteService_Stub stub((*channel).get());
    stellar_post::transmite::NewMessageReq req;
    req.set_request_id(Utils::Uuid());
    req.set_user_id(uid);
    req.set_chat_session_id(sid);
    req.mutable_massage()->set_message_type(stellar_post::base::MessageType::SPEECH);
    req.mutable_massage()->mutable_speech_message()->set_file_contents(msg);

    stellar_post::transmite::GetTransmiteTargetRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetTransmiteTarget(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
}

auto fileMsg(const std::string &uid, const std::string &sid, const std::string &fileName,
             const std::string &fileContent) -> void
{
    auto channel = _transmite_channels->Get(FLAGS_transmiteService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::base::MsgTransmiteService_Stub stub((*channel).get());
    stellar_post::transmite::NewMessageReq req;
    req.set_request_id(Utils::Uuid());
    req.set_user_id(uid);
    req.set_chat_session_id(sid);
    req.mutable_massage()->set_message_type(stellar_post::base::MessageType::FILE);
    req.mutable_massage()->mutable_file_message()->set_file_contents(fileContent);
    req.mutable_massage()->mutable_file_message()->set_file_name(fileName);
    req.mutable_massage()->mutable_file_message()->set_file_size(fileContent.size());

    stellar_post::transmite::GetTransmiteTargetRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetTransmiteTarget(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
}

auto main(int argc, char *argv[]) -> int
{
    testing::InitGoogleTest(&argc, argv);
    // 1.进行服务发现(参数解析) -- 发现 message server 服务器节点地址信息并实例化通信信道
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

    stringMsg("user1", "session1", "Hello World!");
    stringMsg("user2", "session1", "Hello World too!");
    imageMsg("user1", "session1", "image1");
    speechMsg("user1", "session1", "speech1");
    fileMsg("user1", "session1", "file1", "file1 content");

    return RUN_ALL_TESTS(); // 执行所有测试用例
}