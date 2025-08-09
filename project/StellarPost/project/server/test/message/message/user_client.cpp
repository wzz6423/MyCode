// 测试通过

// user_client 测试客户端实现

// C++
#include <print>
#include <iostream>
#include <thread>
#include <functional>
#include <memory>
// userServer 模块
#include "user_server.hpp"
// gflags
#include <gflags/gflags.h>
// gtest
#include <gtest/gtest.h>

stellar_post::Brpc::ChannelManager::managerPtr _user_channels;
stellar_post::base::UserInfo userInfo;
std::string loginSSID;

// etcd brpc
DEFINE_string(registryHost, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(userService, "/service/user_service/", "当前实例名称");

// 用户登录
TEST(UserClientTest, UserLogin)
{
    // 通过 rpc 信道管理对象获取提供 user server 服务的信道
    auto channel = _user_channels->Get(FLAGS_userService).value();
    ASSERT_TRUE(channel != nullptr);

    stellar_post::user::UserService_Stub stub(channel.get());
    stellar_post::user::UserLoginReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_nickname(userInfo.nickname()); // "lisi": 第二次后需更改 or 改 sql 数据
    req.set_password("123456");
    stellar_post::user::UserLoginRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.UserLogin(cntl.get(), &req, &rsp, nullptr);

    // 检测返回值中是否上传成功
    ASSERT_TRUE(cntl->Failed() == false);
    ASSERT_TRUE(rsp.success());
    loginSSID = rsp.login_session_id();
}

// 用户头像设置
TEST(UserClientTest, UserAvatar)
{
    // 通过 rpc 信道管理对象获取提供 user server 服务的信道
    auto channel = _user_channels->Get(FLAGS_userService).value();
    ASSERT_TRUE(channel != nullptr);

    stellar_post::user::UserService_Stub stub(channel.get());
    stellar_post::user::SetUserAvatarReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(userInfo.user_id());
    req.set_session_id(loginSSID);
    req.set_avatar(userInfo.avatar());
    stellar_post::user::SetUserAvatarRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.SetUserAvatar(cntl.get(), &req, &rsp, nullptr);

    // 检测返回值中是否上传成功
    ASSERT_TRUE(cntl->Failed() == false);
    ASSERT_TRUE(rsp.success());
}

auto main(int argc, char *argv[]) -> int
{
    testing::InitGoogleTest(&argc, argv);
    // 1.进行服务发现(参数解析) -- 发现 user server 服务器节点地址信息并实例化通信信道
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 构建 rpc 信道管理对象
    _user_channels = std::make_shared<stellar_post::Brpc::ChannelManager>();
    _user_channels->Declared(FLAGS_userService);

    auto putCb = std::bind(&stellar_post::Brpc::ChannelManager::Online, _user_channels.get(), std::placeholders::_1, std::placeholders::_2);
    auto delCb = std::bind(&stellar_post::Brpc::ChannelManager::Offline, _user_channels.get(), std::placeholders::_1, std::placeholders::_2);

    // 构建服务注册发现对象
    stellar_post::Etcd::Discovery::discoveryPtr discovery = std::make_shared<stellar_post::Etcd::Discovery>(FLAGS_registryHost,
                                                                                                            FLAGS_baseService,
                                                                                                            putCb, delCb);

    userInfo.set_nickname("zhangsan");
    userInfo.set_description("Hello World!");
    userInfo.set_phone("15345678900");
    userInfo.set_avatar("useravatarData");
    // userInfo.set_user_id("");

    return RUN_ALL_TESTS(); // 执行所有测试用例
}