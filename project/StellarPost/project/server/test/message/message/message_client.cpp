// 测试通过

// message_client 测试客户端实现

// C++
#include <print>
#include <iostream>
#include <thread>
#include <functional>
#include <memory>
// messageServer 模块
#include "message_server.hpp"
// gflags
#include <gflags/gflags.h>
// gtest
#include <gtest/gtest.h>

// etcd brpc
DEFINE_string(registryHost, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(messageService, "/service/message_service/", "当前实例名称");

stellar_post::Brpc::ChannelManager::managerPtr _message_channels;

auto Range(const std::string &ssid,
           const boost::posix_time::ptime &begin,
           const boost::posix_time::ptime &end) -> void
{
    auto channel = _message_channels->Get(FLAGS_messageService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::message::MsgStorageService_Stub stub((*channel).get());
    stellar_post::message::GetHistoryMsgReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id("user1");
    req.set_chat_session_id(ssid);
    req.set_start_time(boost::posix_time::to_time_t(begin));
    req.set_over_time(boost::posix_time::to_time_t(end));
    stellar_post::message::GetHistoryMsgRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetHistoryMsg(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
    for (const auto &msg : rsp.msg_list())
    {
        std::println("{}-{}-{}-{}-{}-{}-{}", msg.message_id(), msg.chat_session_id(),
                     boost::posix_time::to_simple_string(boost::posix_time::from_time_t(msg.timestamp())),
                     msg.sender().user_id(), msg.sender().nickname(), msg.sender().avatar());
        switch (msg.message().message_type())
        {
        case stellar_post::base::MessageType::STRING:
            std::println("{}", msg.message().string_message().content());
            break;
        case stellar_post::base::MessageType::IMAGE:
            std::println("{}", msg.message().image_message().image_content());
            break;
        case stellar_post::base::MessageType::FILE:
            std::println("{}-{}", msg.message().file_message().file_contents(), msg.message().file_message().file_name());
            break;
        case stellar_post::base::MessageType::SPEECH:
            std::println("{}", msg.message().speech_message().file_contents());
            break;
        default:
            std::println("type error!");
            break;
        }
    }
}

auto Recent(const std::string &ssid, int count) -> void
{
    auto channel = _message_channels->Get(FLAGS_messageService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::message::MsgStorageService_Stub stub((*channel).get());
    stellar_post::message::GetRecentMsgReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id("user1");
    req.set_chat_session_id(ssid);
    req.set_msg_count(count);
    stellar_post::message::GetRecentMsgRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetRecentMsg(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
    for (const auto &msg : rsp.msg_list())
    {
        std::println("{}-{}-{}-{}-{}-{}-{}", msg.message_id(), msg.chat_session_id(),
                     boost::posix_time::to_simple_string(boost::posix_time::from_time_t(msg.timestamp())),
                     msg.sender().user_id(), msg.sender().nickname(), msg.sender().avatar());
        switch (msg.message().message_type())
        {
        case stellar_post::base::MessageType::STRING:
            std::println("{}", msg.message().string_message().content());
            break;
        case stellar_post::base::MessageType::IMAGE:
            std::println("{}", msg.message().image_message().image_content());
            break;
        case stellar_post::base::MessageType::FILE:
            std::println("{}-{}", msg.message().file_message().file_contents(), msg.message().file_message().file_name());
            break;
        case stellar_post::base::MessageType::SPEECH:
            std::println("{}", msg.message().speech_message().file_contents());
            break;
        default:
            std::println("type error!");
            break;
        }
    }
}

auto Search(const std::string &ssid, const std::string &keyword) -> void
{
    auto channel = _message_channels->Get(FLAGS_messageService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::message::MsgStorageService_Stub stub((*channel).get());
    stellar_post::message::MsgSearchReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id("user1");
    req.set_chat_session_id(ssid);
    req.set_search_key(keyword);
    stellar_post::message::MsgSearchRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.MsgSearch(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
    for (const auto &msg : rsp.msg_list())
    {
        std::println("{}-{}-{}-{}-{}-{}-{}", msg.message_id(), msg.chat_session_id(),
                     boost::posix_time::to_simple_string(boost::posix_time::from_time_t(msg.timestamp())),
                     msg.sender().user_id(), msg.sender().nickname(), msg.sender().avatar());
        switch (msg.message().message_type())
        {
        case stellar_post::base::MessageType::STRING:
            std::println("{}", msg.message().string_message().content());
            break;
        case stellar_post::base::MessageType::IMAGE:
            std::println("{}", msg.message().image_message().image_content());
            break;
        case stellar_post::base::MessageType::FILE:
            std::println("{}-{}", msg.message().file_message().file_contents(), msg.message().file_message().file_name());
            break;
        case stellar_post::base::MessageType::SPEECH:
            std::println("{}", msg.message().speech_message().file_contents());
            break;
        default:
            std::println("type error!");
            break;
        }
    }
}

auto main(int argc, char *argv[]) -> int
{
    testing::InitGoogleTest(&argc, argv);
    // 1.进行服务发现(参数解析) -- 发现 message server 服务器节点地址信息并实例化通信信道
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 构建 rpc 信道管理对象
    _message_channels = std::make_shared<stellar_post::Brpc::ChannelManager>();
    _message_channels->Declared(FLAGS_messageService);

    auto putCb = std::bind(&stellar_post::Brpc::ChannelManager::Online, _message_channels.get(), std::placeholders::_1, std::placeholders::_2);
    auto delCb = std::bind(&stellar_post::Brpc::ChannelManager::Offline, _message_channels.get(), std::placeholders::_1, std::placeholders::_2);

    // 构建服务注册发现对象
    stellar_post::Etcd::Discovery::discoveryPtr discovery = std::make_shared<stellar_post::Etcd::Discovery>(FLAGS_registryHost,
                                                                                                            FLAGS_baseService,
                                                                                                            putCb, delCb);

    boost::posix_time::ptime begin(boost::posix_time::time_from_string("2023-01-01 00:00:00")); // 根据实际情况改
    boost::posix_time::ptime end(boost::posix_time::time_from_string("2023-01-02 00:00:00"));
    Range("ssid1", begin, end);
    std::println("------------------------------------------------------------------------------------------");
    Recent("ssid1", 2);
    std::println("------------------------------------------------------------------------------------------");
    Search("ssid1", "World");

    return RUN_ALL_TESTS(); // 执行所有测试用例
}