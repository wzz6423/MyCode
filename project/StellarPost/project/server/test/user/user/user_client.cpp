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

// insert into user values('', "user1", "wangwu", "Hello World", "123456", "123456", "123456")
// insert into user values('', "user2", "zhaoliu", "Hello World", "1234567", "1234567", "1234567")

// 用户注册 -- 单独测试
TEST(UserClientTest, UserRegister)
{
    // 通过 rpc 信道管理对象获取提供 user server 服务的信道
    auto channel = _user_channels->Get(FLAGS_userService).value();
    ASSERT_TRUE(channel != nullptr);

    stellar_post::user::UserService_Stub stub(channel.get());
    stellar_post::user::UserRegisterReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_nickname(userInfo.nickname());
    req.set_password("123456");
    stellar_post::user::UserRegisterRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.UserRegister(cntl.get(), &req, &rsp, nullptr);

    // 检测返回值中是否上传成功
    ASSERT_TRUE(cntl->Failed() == false);
    ASSERT_TRUE(rsp.success());
}

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

// 用户签名设置
TEST(UserClientTest, UserDescription)
{
    // 通过 rpc 信道管理对象获取提供 user server 服务的信道
    auto channel = _user_channels->Get(FLAGS_userService).value();
    ASSERT_TRUE(channel != nullptr);

    stellar_post::user::UserService_Stub stub(channel.get());
    stellar_post::user::SetUserDescriptionReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(userInfo.user_id());
    req.set_session_id(loginSSID);
    req.set_description(userInfo.description());
    stellar_post::user::SetUserDescriptionRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.SetUserDescription(cntl.get(), &req, &rsp, nullptr);

    // 检测返回值中是否上传成功
    ASSERT_TRUE(cntl->Failed() == false);
    ASSERT_TRUE(rsp.success());
}

// 用户昵称设置
TEST(UserClientTest, UserNickname)
{
    // 通过 rpc 信道管理对象获取提供 user server 服务的信道
    auto channel = _user_channels->Get(FLAGS_userService).value();
    ASSERT_TRUE(channel != nullptr);

    stellar_post::user::UserService_Stub stub(channel.get());
    stellar_post::user::SetUserNicknameReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(userInfo.user_id());
    req.set_session_id(loginSSID);
    userInfo.set_nickname("lisi");
    req.set_nickname("lisi");
    stellar_post::user::SetUserNicknameRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.SetUserNickname(cntl.get(), &req, &rsp, nullptr);

    // 检测返回值中是否上传成功
    ASSERT_TRUE(cntl->Failed() == false);
    ASSERT_TRUE(rsp.success());
}

// 用户信息获取
TEST(UserClientTest, GetUserInfo)
{
    // 通过 rpc 信道管理对象获取提供 user server 服务的信道
    auto channel = _user_channels->Get(FLAGS_userService).value();
    ASSERT_TRUE(channel != nullptr);

    stellar_post::user::UserService_Stub stub(channel.get());
    stellar_post::user::GetUserInfoReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(userInfo.user_id());
    req.set_session_id(loginSSID);
    stellar_post::user::GetUserInfoRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetUserInfo(cntl.get(), &req, &rsp, nullptr);

    // 检测返回值中是否上传成功
    ASSERT_TRUE(cntl->Failed() == false);
    ASSERT_TRUE(rsp.success());
    ASSERT_EQ(userInfo.user_id(), rsp.user_info().user_id());
    ASSERT_EQ(userInfo.nickname(), rsp.user_info().nickname());
    ASSERT_EQ(userInfo.description(), rsp.user_info().description());
    ASSERT_EQ(userInfo.avatar(), rsp.user_info().avatar());
    ASSERT_EQ("", rsp.user_info().phone());
}

// 批量用户信息获取
TEST(UserClientTest, GetMulUserInfo)
{
    // 通过 rpc 信道管理对象获取提供 user server 服务的信道
    auto channel = _user_channels->Get(FLAGS_userService).value();
    ASSERT_TRUE(channel != nullptr);

    stellar_post::user::UserService_Stub stub(channel.get());
    stellar_post::user::GetMultiUserInfoReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    // req.add_users_id(""); // 在 sql 中查询 lisi 的 user_id
    req.add_users_id("user1");
    req.add_users_id("user2");
    stellar_post::user::GetMultiUserInfoRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetMultiUserInfo(cntl.get(), &req, &rsp, nullptr);

    // 检测返回值中是否上传成功
    ASSERT_TRUE(cntl->Failed() == false);
    ASSERT_TRUE(rsp.success());
    auto usersMap = rsp.mutable_users_info();
    stellar_post::base::UserInfo fuser = (*usersMap)[""]; // 在 sql 中查询 lisi 的 user_id
    stellar_post::base::UserInfo suser = (*usersMap)["user1"];
    stellar_post::base::UserInfo tuser = (*usersMap)["user2"];
    ASSERT_EQ(fuser.nickname(), "lisi");
    ASSERT_EQ(suser.nickname(), "wangwu");
    ASSERT_EQ(tuser.nickname(), "zhaoliu");
    ASSERT_EQ(fuser.description(), "Hello World!");
    ASSERT_EQ(suser.description(), "Hello World!");
    ASSERT_EQ(tuser.description(), "Hello World!");
    ASSERT_EQ(fuser.phone(), "15345678900");
    ASSERT_EQ(suser.phone(), "123456");
    ASSERT_EQ(tuser.phone(), "1234567");
    // ASSERT_EQ(fuser.user_id(), "");
    ASSERT_EQ(suser.user_id(), "user1");
    ASSERT_EQ(tuser.user_id(), "user2");
    std::println("{}", fuser.avatar());
}

// 获取手机验证码 ID
auto GetCode() -> std::string
{
    // 通过 rpc 信道管理对象获取提供 user server 服务的信道
    auto channel = _user_channels->Get(FLAGS_userService).value();

    stellar_post::user::UserService_Stub stub(channel.get());
    stellar_post::user::PhoneVerifyCodeReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_phone_number(userInfo.phone());
    stellar_post::user::PhoneVerifyCodeRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.GetPhoneVerifyCode(cntl.get(), &req, &rsp, nullptr);
    return rsp.verify_code_id();
}
// 用户手机号注册
TEST(UserClientTest, PhoneRegister)
{
    // 通过 rpc 信道管理对象获取提供 user server 服务的信道
    auto channel = _user_channels->Get(FLAGS_userService).value();
    ASSERT_TRUE(channel != nullptr);

    std::string codeID = GetCode();

    stellar_post::user::UserService_Stub stub(channel.get());
    stellar_post::user::PhoneRegisterReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_phone_number(userInfo.phone());
    req.set_verify_code_id(codeID);
    std::string code;
    std::println("请输入验证码:");
    std::cin >> code;
    req.set_verify_code(code);
    stellar_post::user::PhoneRegisterRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.PhoneRegister(cntl.get(), &req, &rsp, nullptr);

    // 检测返回值中是否上传成功
    ASSERT_TRUE(cntl->Failed() == false);
    ASSERT_TRUE(rsp.success());
}
// 用户手机号登录
TEST(UserClientTest, PhoneLogin)
{
    // 通过 rpc 信道管理对象获取提供 user server 服务的信道
    auto channel = _user_channels->Get(FLAGS_userService).value();
    ASSERT_TRUE(channel != nullptr);

    std::string codeID = GetCode();

    stellar_post::user::UserService_Stub stub(channel.get());
    stellar_post::user::PhoneLoginReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_phone_number(userInfo.phone());
    req.set_verify_code_id(codeID);
    std::string code;
    std::println("请输入验证码:");
    std::cin >> code;
    req.set_verify_code(code);
    stellar_post::user::PhoneLoginRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.PhoneLogin(cntl.get(), &req, &rsp, nullptr);

    // 检测返回值中是否上传成功
    ASSERT_TRUE(cntl->Failed() == false);
    ASSERT_TRUE(rsp.success());
    std::println("手机登录会话 ID: {}", rsp.login_session_id());
}
// 用户手机号设置
TEST(UserClientTest, PhoneSet)
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
    // 通过 rpc 信道管理对象获取提供 user server 服务的信道
    auto channel = _user_channels->Get(FLAGS_userService).value();
    ASSERT_TRUE(channel != nullptr);

    std::string codeID = GetCode();

    std::string code;
    std::println("请输入用户 ID:"); // sql 查询
    std::cin >> code;

    stellar_post::user::UserService_Stub stub(channel.get());
    stellar_post::user::SetUserPhoneNumberReq req;
    req.set_request_id(stellar_post::Utils::Uuid());
    req.set_user_id(code);
    req.set_phone_number("15399999999");
    req.set_phone_verify_code_id(codeID);
    std::println("请输入验证码:");
    std::cin >> code;
    req.set_phone_verify_code(code);
    stellar_post::user::SetUserPhoneNumberRsp rsp;
    std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
    stub.SetUserPhoneNumber(cntl.get(), &req, &rsp, nullptr);

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