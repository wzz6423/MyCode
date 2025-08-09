// 测试通过

// crony_client 测试客户端实现

// C++
#include <print>
#include <iostream>
#include <thread>
#include <functional>
#include <memory>
// cronyServer 模块
#include "crony_server.hpp"
// gflags
#include <gflags/gflags.h>
// gtest
#include <gtest/gtest.h>

// etcd brpc
DEFINE_string(registryHost, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(cronyService, "/service/crony_service/", "当前实例名称");

stellar_post::Brpc::ChannelManager::managerPtr _crony_channels;

// 数据库造数据进行测试

// 好友申请测试用例
auto ApplyTest(const std::string &uid1, const std::string &uid2) -> void
{
    auto channel = _crony_channels->Get(FLAGS_cronyService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::crony::CronyService_Stub stub((*channel).get());
    stellar_post::crony::CronyAddReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(uid1);
    req.set_respondent_id(uid2);
    stellar_post::crony::CronyAddRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.CronyAdd(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
}

// 好友申请列表获取测试用例
auto ApplyListTest(const std::string &uid) -> void
{
    auto channel = _crony_channels->Get(FLAGS_cronyService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::crony::CronyService_Stub stub((*channel).get());
    stellar_post::crony::GetPendingCronyEventListReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(uid);
    stellar_post::crony::GetPendingCronyEventListRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetPendingCronyEventList(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
    for (auto &event : rsp.event())
    {
        std::println("event: {} {} {}", event.sender().user_id(), event.sender().nickname(), event.sender().avatar());
    }
}

// 好友申请处理测试用例
auto ApplyHandleTest(const std::string &uid1, const std::string &uid2, bool agree) -> void
{
    auto channel = _crony_channels->Get(FLAGS_cronyService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::crony::CronyService_Stub stub((*channel).get());
    stellar_post::crony::CronyAddProcessReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(uid1);
    req.set_apply_user_id(uid2);
    req.set_agree(agree);
    stellar_post::crony::CronyAddProcessRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.CronyAddProcess(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
    if (agree)
    {
        std::println("{}", rsp.new_session_id());
    }
}

// 好友搜索测试用例
auto SearchTest(const std::string &uid, const std::string &keyword) -> void
{
    auto channel = _crony_channels->Get(FLAGS_cronyService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::crony::CronyService_Stub stub((*channel).get());
    stellar_post::crony::CronySearchReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(uid);
    req.set_search_key(keyword);
    stellar_post::crony::CronySearchRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.CronySearch(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
    for (auto &e : rsp.user_info())
    {
        std::println("{} {} {}", e.user_id(), e.nickname(), e.avatar());
    }
}

// 获取好友列表测试用例
auto FriendListTest(const std::string &uid) -> void
{
    auto channel = _crony_channels->Get(FLAGS_cronyService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::crony::CronyService_Stub stub((*channel).get());
    stellar_post::crony::GetCronyListReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(uid);
    stellar_post::crony::GetCronyListRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetCronyList(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
    for (auto &e : rsp.crony_list())
    {
        std::println("{} {} {}", e.user_id(), e.nickname(), e.avatar());
    }
}

// 好友删除测试用例
auto FriendDeleteTest(const std::string &uid1, const std::string &uid2) -> void
{
    auto channel = _crony_channels->Get(FLAGS_cronyService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::crony::CronyService_Stub stub((*channel).get());
    stellar_post::crony::CronyRemoveReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(uid1);
    req.set_peer_id(uid2);
    stellar_post::crony::CronyRemoveRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.CronyRemove(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
}

// 发起群聊测试用例
auto CreateChatSessionTest(const std::string &uid, const std::string &name, std::vector<std::string> &member_ids) -> void
{
    auto channel = _crony_channels->Get(FLAGS_cronyService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::crony::CronyService_Stub stub((*channel).get());
    stellar_post::crony::ChatSessionCreateReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(uid);
    req.set_chat_session_name(name);
    for (auto &member_id : member_ids)
    {
        req.add_member_id_list(member_id);
    }
    stellar_post::crony::ChatSessionCreateRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.ChatSessionCreate(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
    std::println("{} - {}", rsp.chat_session_info().chat_session_id(), rsp.chat_session_info().chat_session_name());
}

// 获取聊天会话列表测试用例
auto ChatSessionMemberTest(const std::string &uid, const std::string &csid) -> void
{
    auto channel = _crony_channels->Get(FLAGS_cronyService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::crony::CronyService_Stub stub((*channel).get());
    stellar_post::crony::GetChatSessionMemberReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(uid);
    req.set_chat_session_id(csid);
    stellar_post::crony::GetChatSessionMemberRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetChatSessionMember(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
    for (auto &e : rsp.member_info_list())
    {
        std::println("{} - {}", e.user_id(), e.nickname());
    }
}

// 发消息测试用例 -- transmite
auto ChatSessionMsgTest(const std::string &uid) -> void
{
    auto channel = _crony_channels->Get(FLAGS_cronyService);
    if (!channel)
    {
        std::println("channel is null");
        return;
    }
    stellar_post::crony::CronyService_Stub stub((*channel).get());
    stellar_post::crony::GetChatSessionListReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(uid);
    stellar_post::crony::GetChatSessionListRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetChatSessionList(cntl.get(), &req, &rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp.success());
    for (auto &e : rsp.chat_session_info_list())
    {
        std::println("{} - {} - {}", e.chat_session_id(), e.chat_session_name(),
                     e.prev_message().message().string_message().content());
    }
}

auto main(int argc, char *argv[]) -> int
{
    testing::InitGoogleTest(&argc, argv);
    // 1.进行服务发现(参数解析) -- 发现 crony server 服务器节点地址信息并实例化通信信道
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 构建 rpc 信道管理对象
    _crony_channels = std::make_shared<stellar_post::Brpc::ChannelManager>();
    _crony_channels->Declared(FLAGS_cronyService);
    _crony_channels->Declared(FLAGS_cronyService);

    auto putCb = std::bind(&stellar_post::Brpc::ChannelManager::Online, _crony_channels.get(), std::placeholders::_1, std::placeholders::_2);
    auto delCb = std::bind(&stellar_post::Brpc::ChannelManager::Offline, _crony_channels.get(), std::placeholders::_1, std::placeholders::_2);

    // 构建服务注册发现对象
    stellar_post::Etcd::Discovery::discoveryPtr discovery = std::make_shared<stellar_post::Etcd::Discovery>(FLAGS_registryHost,
                                                                                                            FLAGS_baseService,
                                                                                                            putCb, delCb);

    return RUN_ALL_TESTS(); // 执行所有测试用例
}