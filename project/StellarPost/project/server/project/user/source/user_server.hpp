/*
    实现 user 微服务
*/

#pragma once

// C
#include <cctype>
// C++
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <ranges>
#include <string_view>
#include <optional>
#include <utility>
// Other
#include "../../common/etcd/etcd.hpp"             // 服务注册 etcd 模块封装
#include "../../common/brpc/brpc.hpp"             // brpc 模块封装
#include "../../common/log/logger.hpp"            // 日志器 spdlog 模块封装
#include "../../common/utils/utils.hpp"           // 工具函数(生成唯一 uid)模块封装
#include "../../common/daemon/daemon.hpp"         // 守护进程模块封装
#include "../dms/dms.hpp"                         // 阿里短信验证码 SDK 模块封装
#include "../operator/user_mysql.hpp"             // mysql 模块封装
#include "../../common/operator/operator.hpp"     // 客户端构造模块封装
#include "../../common/operator/user_redis.hpp"   // redis 数据管理客户端模块封装
#include "../../common/elastic/elastic.hpp"       // elasticsearch 模块封装
#include "../../common/operator/user_elastic.hpp" // elasticsearch 数据管理客户端模块封装
#include "base.pb.h"                              // protobuf 框架代码
#include "user.pb.h"                              // protobuf 框架代码
#include "file.pb.h"                              // protobuf 框架代码
#include "../../common/odb/user.hxx"              // odb 数据库操作封装
#include "user-odb.hxx"                           // odb 数据库操作封装

namespace stellar_post
{
    namespace user
    {
        class UserServiceImpl : public UserService
        {
        public:
            UserServiceImpl() = delete;
            UserServiceImpl(const std::shared_ptr<elasticlient::Client> &es_client,
                            const std::shared_ptr<odb::core::database> &mysql_client,
                            const std::shared_ptr<sw::redis::Redis> &redis_client,
                            const std::shared_ptr<Brpc::ChannelManager> &channel_manager,
                            const DMS::DMSClient::dmsPtr &dmsClient,
                            const std::string &file_service_name)
                : _es_user(std::make_shared<ESUser>(es_client)),
                  _mysql_user(std::make_shared<UserTable>(mysql_client)),
                  _redis_session(std::make_shared<Session>(redis_client)),
                  _redis_status(std::make_shared<Status>(redis_client)),
                  _redis_codes(std::make_shared<Codes>(redis_client)),
                  _dms_client(dmsClient),
                  _channel_manager(channel_manager),
                  _file_service_name(file_service_name)
            {
                if (!_es_user->CreateIndex())
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(), "es create index error!");
                }
            }

            ~UserServiceImpl() = default;

            // 用户注册
            virtual auto UserRegister(::google::protobuf::RpcController *controller,
                                      const ::stellar_post::user::UserRegisterReq *request,
                                      ::stellar_post::user::UserRegisterRsp *response,
                                      ::google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());
                // 定义错误处理函数, 当出错时被调用
                auto error = [this, response](const std::string &errmsg)
                {
                    response->set_errmsg(errmsg);
                    response->set_success(false);
                };

                // 从请求中取出昵称和密码
                std::string nickName = request->nickname();
                std::string password = request->password();

                // 检查昵称合法性
                if (!nicknameCheck(nickName))
                {
                    error("昵称不合法!");
                    return;
                }

                // 检查密码合法性
                if (!passwordCheck(password))
                {
                    error("密码不合法!");
                    return;
                }

                // 检查昵称在数据库中是否已存在
                if (isNicknameExist(nickName))
                {
                    error("昵称已存在!");
                    return;
                }

                // 向数据库新增数据
                std::string uid = Utils::Uuid();
                if (!_mysql_user->Insert(std::make_shared<User>(uid, nickName, password)))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "mysql insert user data error!");
                    error("数据库操作失败!");
                    return;
                }

                // 向 elasticsearch 服务器中新增数据
                if (!_es_user->Add(uid, "", nickName, "", ""))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "es insert user data error!");
                    error("elasticsearch 操作失败!");
                    return;
                }

                // 组织响应, 设置状态码和消息
                response->set_success(true);
            }

            // 用户登录
            virtual auto UserLogin(google::protobuf::RpcController *controller,
                                   const ::stellar_post::user::UserLoginReq *request,
                                   ::stellar_post::user::UserLoginRsp *response,
                                   ::google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());
                // 定义错误处理函数, 当出错时被调用
                auto error = [this, response](const std::string &errmsg)
                {
                    response->set_errmsg(errmsg);
                    response->set_success(false);
                };

                // 从请求中取出昵称和密码
                std::string nickName = request->nickname();
                std::string password = request->password();

                // 通过昵称获取用户信息, 进行密码一致性检验
                std::shared_ptr<User> u = _mysql_user->SelectNickName(nickName);
                if (u == nullptr || !u->Password().empty() || password != u->Password())
                {
                    error("用户名或密码错误!");
                    return;
                }

                // 根据 redis 中的登录标记信息判断用户是否为重复登录
                if (_redis_status->Exists(u->UserID()))
                {
                    error("用户已登录!");
                    return;
                }

                // 构造会话 ID, 生成会话键值对, 向 redis 中添加会话信息以及登录标记信息
                std::string session_id = Utils::Uuid();
                if (!_redis_session->Add(session_id, u->UserID()))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "redis set session error!");
                    error("redis 操作失败!");
                    return;
                }
                if (!_redis_status->Add(u->UserID()))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "redis set session error!");
                    error("redis 操作失败!");
                    return;
                }

                // 组织响应, 返回生成的会话 ID
                response->set_success(true);
                response->set_login_session_id(session_id);
            }

            // 获取验证码
            virtual auto GetPhoneVerifyCode(google::protobuf::RpcController *controller,
                                            const ::stellar_post::user::PhoneVerifyCodeReq *request,
                                            ::stellar_post::user::PhoneVerifyCodeRsp *response,
                                            ::google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());
                // 定义错误处理函数, 当出错时被调用
                auto error = [this, response](const std::string &errmsg)
                {
                    response->set_errmsg(errmsg);
                    response->set_success(false);
                };

                // 从请求中取出手机号码
                std::string phoneNumber = request->phone_number();

                // 验证手机号码合法性
                if (!phoneCheck(phoneNumber))
                {
                    error("手机号码不合法!");
                    return;
                }

                // 生成验证码 & 验证码 ID
                std::string code = Utils::VCode();
                std::string code_id = Utils::Uuid();

                // 添加验证码 & 验证码 ID 到 redis 验证码键值映射索引中
                if (!_redis_codes->Add(code_id, code))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "redis set code error!");
                    error("redis 操作失败!");
                    return;
                }

                // 基于短信平台 SDK 发送验证码
                if (!_dms_client->Send(phoneNumber, code))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "dms send code error!");
                    error("短信发送失败!");
                    return;
                }

                // 组织响应, 返回生成的验证码
                response->set_success(true);
                response->set_verify_code_id(code_id);
            }

            // 手机号注册
            virtual auto PhoneRegister(google::protobuf::RpcController *controller,
                                       const ::stellar_post::user::PhoneRegisterReq *request,
                                       ::stellar_post::user::PhoneRegisterRsp *response,
                                       ::google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());
                // 定义错误处理函数, 当出错时被调用
                auto error = [this, response](const std::string &errmsg)
                {
                    response->set_errmsg(errmsg);
                    response->set_success(false);
                };

                // 从请求中取出手机号、验证码和验证码 ID
                std::string phoneNumber = request->phone_number();
                std::string code = request->verify_code();
                std::string code_id = request->verify_code_id();

                // 检查注册手机号码合法性
                if (!phoneCheck(phoneNumber))
                {
                    error("手机号码不合法!");
                    return;
                }

                // 从 redis 数据库中进行验证码 ID-验证码 一致性匹配
                std::optional<std::string> code_opt = _redis_codes->Get(code_id);
                if (!code_opt.has_value() || code_opt.value() != code)
                {
                    error("验证码错误!");
                    return;
                }
                _redis_codes->Remove(code_id); // 删除验证码 ID-验证码 键值对

                // 通过数据库查询手机号是否已注册
                if (isPhoneNumberExist(phoneNumber))
                {
                    error("手机号已注册!");
                    return;
                }

                // 向数据库新增用户信息
                std::string uid = Utils::Uuid();
                if (!_mysql_user->Insert(std::make_shared<User>(uid, phoneNumber)))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "mysql insert user data error!");
                    error("数据库操作失败!");
                    return;
                }

                // 向 elasticsearch 服务器中新增用户信息
                if (!_es_user->Add(uid, phoneNumber, uid, "", ""))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "elasticsearch insert user data error!");
                    error("数据库操作失败!");
                    return;
                }

                // 组织响应
                response->set_success(true);
            }

            // 手机号登录
            virtual auto PhoneLogin(google::protobuf::RpcController *controller,
                                    const ::stellar_post::user::PhoneLoginReq *request,
                                    ::stellar_post::user::PhoneLoginRsp *response,
                                    ::google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());
                // 定义错误处理函数, 当出错时被调用
                auto error = [this, response](const std::string &errmsg)
                {
                    response->set_errmsg(errmsg);
                    response->set_success(false);
                };

                // 从请求中取出手机号、验证码和验证码 ID
                std::string phoneNumber = request->phone_number();
                std::string code = request->verify_code();
                std::string code_id = request->verify_code_id();

                // 检查登录手机号码合法性
                if (!phoneCheck(phoneNumber))
                {
                    error("手机号码不合法!");
                    return;
                }

                // 从 redis 数据库中进行验证码 ID-验证码 一致性匹配
                std::optional<std::string> code_opt = _redis_codes->Get(code_id);
                if (!code_opt.has_value() || code_opt.value() != code)
                {
                    error("验证码错误!");
                    return;
                }
                _redis_codes->Remove(code_id); // 删除验证码 ID-验证码 键值对

                // 通过数据库查询手机号是否已存在
                std::shared_ptr<User> u = _mysql_user->SelectPhone(phoneNumber);
                if (u == nullptr)
                {
                    error("手机号未注册!");
                    return;
                }

                // 根据 redis 中登陆标记信息是否存在判断用户是否为重复登录
                if (_redis_status->Exists(u->UserID()))
                {
                    error("用户已登录!");
                    return;
                }

                // 构造会话 ID, 生成会话键值对, 向 redis 中添加会话信息以及登录标记信息
                std::string session_id = Utils::Uuid();
                if (!_redis_session->Add(session_id, u->UserID()))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "redis set session error!");
                    error("redis 操作失败!");
                    return;
                }
                if (!_redis_status->Add(u->UserID()))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "redis set session error!");
                    error("redis 操作失败!");
                    return;
                }

                // 组织响应, 返回生成的会话 ID
                response->set_success(true);
                response->set_login_session_id(session_id);
            }

            // 获取用户信息
            virtual auto GetUserInfo(google::protobuf::RpcController *controller,
                                     const ::stellar_post::user::GetUserInfoReq *request,
                                     ::stellar_post::user::GetUserInfoRsp *response,
                                     ::google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());
                // 定义错误处理函数, 当出错时被调用
                auto error = [this, response](const std::string &errmsg)
                {
                    response->set_errmsg(errmsg);
                    response->set_success(false);
                };

                // 从请求中取出用户 ID
                std::string uid = request->user_id();

                // 通过用户 ID 从数据库查询用户信息
                std::shared_ptr<User> u = _mysql_user->SelectID(uid);
                if (u == nullptr)
                {
                    error("用户不存在!");
                    return;
                }

                // 根据用户信息中的头像 ID 从文件服务器中获取头像文件数据, 组织完整用户信息
                std::string avatarID = u->AvatarID();
                base::UserInfo *userInfo = response->mutable_user_info();
                // uid
                userInfo->set_user_id(std::move(u->UserID()));
                // 昵称
                if (!u->NickName().empty())
                {
                    userInfo->set_nickname(std::move(u->NickName()));
                }
                else
                {
                    userInfo->set_nickname("未设置昵称");
                }
                // 签名
                if (!u->Description().empty())
                {
                    userInfo->set_description(std::move(u->Description()));
                }
                else
                {
                    userInfo->set_description("未设置签名");
                }
                // 手机号
                if (!u->Phone().empty())
                {
                    userInfo->set_phone(std::move(u->Phone()));
                }
                else
                {
                    userInfo->set_phone("未设置手机号");
                }
                // 头像
                if (!avatarID.empty())
                {
                    // 从信道管理对象中获取连接了文件管理子服务的 channel
                    std::optional<Brpc::Channel::brpcChannelPtr> channel_opt = _channel_manager->Get(_file_service_name);
                    if (!channel_opt.has_value())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "file server channel not available!");
                        error("文件服务不可用!");
                        return;
                    }
                    Brpc::Channel::brpcChannelPtr channel = channel_opt.value();

                    // 进行文件子服务的 rpc 请求, 进行头像文件下载
                    file::FileService_Stub stub(channel.get());
                    file::GetSingleFileReq req;
                    file::GetSingleFileRsp rsp;
                    req.set_request_id(request->request_id());
                    req.set_file_id(avatarID);
                    std::shared_ptr<brpc::Controller> cntl = std::make_shared<brpc::Controller>();
                    stub.GetSingleFile(cntl.get(), &req, &rsp, nullptr);
                    if (cntl->Failed() || rsp.success() == false)
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "file server get single file error: {}", cntl->ErrorText());
                        error("文件服务不可用!");
                        return;
                    }
                    userInfo->set_avatar(std::move(rsp.file_data().file_content()));
                }

                // 组织响应, 返回用户信息
                response->set_success(true);
            }

            // 批量获取用户信息
            virtual auto GetMultiUserInfo(google::protobuf::RpcController *controller,
                                          const ::stellar_post::user::GetMultiUserInfoReq *request,
                                          ::stellar_post::user::GetMultiUserInfoRsp *response,
                                          ::google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());
                // 定义错误处理函数, 当出错时被调用
                auto error = [this, response](const std::string &errmsg)
                {
                    response->set_errmsg(errmsg);
                    response->set_success(false);
                };

                // 从请求中取出用户 ID -- 列表
                std::vector<std::string> uidList;
                for (const auto &uid : request->users_id())
                {
                    uidList.push_back(uid);
                }

                // 从数据库进行批量用户信息查询
                std::vector<std::shared_ptr<User>> users = _mysql_user->SelectIDs(uidList);
                if (users.size() != uidList.size())
                {
                    error("部分用户不存在!");
                    return;
                }
                std::vector<std::string> avatarIDs; // 头像 ID 列表
                for (const auto &user : users)
                {
                    if (!user->AvatarID().empty())
                    {
                        avatarIDs.push_back(user->AvatarID());
                    }
                }

                if (!users.empty())
                {
                    // 进行文件子服务的 rpc 请求, 进行头像文件下载
                    file::GetMultiFileRsp rsp;
                    if (!avatarIDs.empty())
                    {
                        // 批量从文件管理子服务获取头像文件数据
                        // 从信道管理对象中获取连接了文件管理子服务的 channel
                        std::optional<Brpc::Channel::brpcChannelPtr> channel_opt = _channel_manager->Get(_file_service_name);
                        if (!channel_opt.has_value())
                        {
                            Log::lg("error", Log::FileName(), Log::Line(),
                                    "file server channel not available!");
                            error("文件服务不可用!");
                            return;
                        }
                        Brpc::Channel::brpcChannelPtr channel = channel_opt.value();

                        file::GetMultiFileReq req;
                        file::FileService_Stub stub(channel.get());
                        req.set_request_id(request->request_id());
                        for (auto &avatarID : avatarIDs)
                        {
                            req.add_file_id_list(std::move(avatarID));
                        }
                        std::shared_ptr<brpc::Controller> cntl = std::make_shared<brpc::Controller>();
                        stub.GetMultiFile(cntl.get(), &req, &rsp, nullptr);
                        if (cntl->Failed() || rsp.success() == false)
                        {
                            Log::lg("error", Log::FileName(), Log::Line(),
                                    "file server get multi file error: {}", cntl->ErrorText());
                            error("文件服务不可用!");
                            return;
                        }
                    }
                    // 将数据添加到响应中
                    for (auto &u : users)
                    {
                        google::protobuf::Map<std::string, stellar_post::base::UserInfo> *userMap =
                            response->mutable_users_info(); // 本次请求响应的用户信息 map
                        google::protobuf::Map<std::string, stellar_post::base::FileDownloadData> *fileMap =
                            rsp.mutable_file_data(); // 批量文件请求响应中的文件数据 map
                        base::UserInfo userInfo;
                        // uid
                        userInfo.set_user_id(u->UserID());
                        // 昵称
                        if (!u->NickName().empty())
                        {
                            userInfo.set_nickname(std::move(u->NickName()));
                        }
                        else
                        {
                            userInfo.set_nickname("未设置昵称");
                        }
                        // 签名
                        if (!u->Description().empty())
                        {
                            userInfo.set_description(std::move(u->Description()));
                        }
                        else
                        {
                            userInfo.set_description("未设置签名");
                        }
                        // 手机号
                        if (!u->Phone().empty())
                        {
                            userInfo.set_phone(std::move(u->Phone()));
                        }
                        else
                        {
                            userInfo.set_phone("未设置手机号");
                        }
                        // 头像
                        if (!u->AvatarID().empty())
                        {
                            userInfo.set_avatar(
                                std::move(fileMap->at(u->AvatarID()).file_content())); // 从文件数据 map 中获取头像文件内容
                        }
                        userMap->insert({u->UserID(), std::move(userInfo)}); // 将用户信息添加到响应中
                    }
                }

                // 组织响应
                response->set_success(true);
            }

            // 设置头像
            virtual auto SetUserAvatar(google::protobuf::RpcController *controller,
                                       const ::stellar_post::user::SetUserAvatarReq *request,
                                       ::stellar_post::user::SetUserAvatarRsp *response,
                                       ::google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());
                // 定义错误处理函数, 当出错时被调用
                auto error = [this, response](const std::string &errmsg)
                {
                    response->set_errmsg(errmsg);
                    response->set_success(false);
                };

                // 从请求中取出用户 ID 和头像文件数据
                const std::string &uid = request->user_id();

                // 从数据库通过用户 ID 进行用户信息查询, 判断用户是否存在
                std::shared_ptr<User> u = _mysql_user->SelectID(uid);
                if (u == nullptr)
                {
                    error("用户不存在!");
                    return;
                }

                // 上传头像文件到文件子服务
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt = _channel_manager->Get(_file_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "file server channel not available!");
                    error("文件服务不可用!");
                    return;
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                file::FileService_Stub stub(channel.get());
                file::PutSingleFileReq req;
                file::PutSingleFileRsp rsp;
                req.set_request_id(request->request_id());
                req.mutable_file_data()->set_file_name("");
                req.mutable_file_data()->set_file_content(request->avatar());
                req.mutable_file_data()->set_file_size(request->avatar().size());
                std::shared_ptr<brpc::Controller> cntl = std::make_shared<brpc::Controller>();
                stub.PutSingleFile(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "file server put single file error: {}", cntl->ErrorText());
                    error("文件服务不可用!");
                    return;
                }
                std::string avatarID = rsp.file_info().file_id(); // 获取返回的头像文件 ID

                // 将返回的头像文件 ID 存储到数据库中
                u->AvatarID(avatarID);
                if (!_mysql_user->Update(u))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "mysql update user data error!");
                    error("mysql 数据库更新用户头像操作失败!");
                    return;
                }

                // 更新 elasticsearch 中的用户信息
                if (!_es_user->Add(u->UserID(), u->Phone(), u->NickName(), u->Description(), avatarID))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "elasticsearch update user data error!");
                    error("elasticsearch 数据库更新用户头像操作失败!");
                    return;
                }

                // 组织响应
                response->set_success(true);
            }

            // 设置昵称
            virtual auto SetUserNickname(google::protobuf::RpcController *controller,
                                         const ::stellar_post::user::SetUserNicknameReq *request,
                                         ::stellar_post::user::SetUserNicknameRsp *response,
                                         ::google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());
                // 定义错误处理函数, 当出错时被调用
                auto error = [this, response](const std::string &errmsg)
                {
                    response->set_errmsg(errmsg);
                    response->set_success(false);
                };

                // 从请求中取出用户 ID 和昵称
                std::string uid = request->user_id();
                std::string nickname = request->nickname();

                // 判断昵称合法性
                if (!nicknameCheck(nickname))
                {
                    error("昵称不合法!");
                    return;
                }

                // 从数据库通过用户 ID 进行用户信息查询, 判断用户是否存在
                std::shared_ptr<User> u = _mysql_user->SelectID(uid);
                if (u == nullptr)
                {
                    error("用户不存在!");
                    return;
                }

                // 检查昵称在数据库中是否已存在
                if (isNicknameExist(nickname))
                {
                    error("昵称已存在!");
                    return;
                }

                // 更新用户昵称到数据库中
                u->NickName(nickname);
                if (!_mysql_user->Update(u))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "mysql update user data error!");
                    error("mysql 数据库更新用户昵称操作失败!");
                    return;
                }

                // 更新 elasticsearch 中的用户信息
                if (!_es_user->Add(u->UserID(), u->Phone(), u->NickName(), u->Description(), u->AvatarID()))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "elasticsearch update user data error!");
                    error("elasticsearch 数据库更新用户昵称操作失败!");
                    return;
                }

                // 组织响应
                response->set_success(true);
            }

            // 设置签名
            virtual auto SetUserDescription(google::protobuf::RpcController *controller,
                                            const ::stellar_post::user::SetUserDescriptionReq *request,
                                            ::stellar_post::user::SetUserDescriptionRsp *response,
                                            ::google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());
                // 定义错误处理函数, 当出错时被调用
                auto error = [this, response](const std::string &errmsg)
                {
                    response->set_errmsg(errmsg);
                    response->set_success(false);
                };

                // 从请求中取出用户 ID 和签名
                std::string uid = request->user_id();
                std::string description = request->description();

                // 判断签名合法性
                if (!descriptionCheck(description))
                {
                    error("签名不合法!");
                    return;
                }

                // 从数据库通过用户 ID 进行用户信息查询, 判断用户是否存在
                std::shared_ptr<User> u = _mysql_user->SelectID(uid);
                if (u == nullptr)
                {
                    error("用户不存在!");
                    return;
                }

                // 检查昵称在数据库中是否已存在
                if (isNicknameExist(description))
                {
                    error("签名已存在!");
                    return;
                }

                // 更新用户签名到数据库中
                u->Description(description);
                if (!_mysql_user->Update(u))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "mysql update user data error!");
                    error("mysql 数据库更新用户签名操作失败!");
                    return;
                }

                // 更新 elasticsearch 中的用户信息
                if (!_es_user->Add(u->UserID(), u->Phone(), u->NickName(), u->Description(), u->AvatarID()))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "elasticsearch update user data error!");
                    error("elasticsearch 数据库更新用户签名操作失败!");
                    return;
                }

                // 组织响应
                response->set_success(true);
            }

            // 设置电话号
            virtual auto SetUserPhoneNumber(google::protobuf::RpcController *controller,
                                            const ::stellar_post::user::SetUserPhoneNumberReq *request,
                                            ::stellar_post::user::SetUserPhoneNumberRsp *response,
                                            ::google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());
                // 定义错误处理函数, 当出错时被调用
                auto error = [this, response](const std::string &errmsg)
                {
                    response->set_errmsg(errmsg);
                    response->set_success(false);
                };

                // 从请求中取出用户 ID 和 电话号 和 验证码 和 验证码 ID
                std::string uid = request->user_id();
                std::string phone = request->phone_number();
                std::string code = request->phone_verify_code();
                std::string code_id = request->phone_verify_code_id();

                // 从 redis 数据库中进行验证码 ID-验证码 一致性匹配
                std::optional<std::string> code_opt = _redis_codes->Get(code_id);
                if (!code_opt.has_value() || code_opt.value() != code)
                {
                    error("验证码错误!");
                    return;
                }
                _redis_codes->Remove(code_id); // 删除验证码 ID-验证码 键值对

                // 判断电话号合法性
                if (!phoneCheck(phone))
                {
                    error("电话号不合法!");
                    return;
                }

                // 从数据库通过用户 ID 进行用户信息查询, 判断用户是否存在
                std::shared_ptr<User> u = _mysql_user->SelectID(uid);
                if (u == nullptr)
                {
                    error("用户不存在!");
                    return;
                }

                // 检查电话号在数据库中是否已存在
                if (isPhoneNumberExist(phone))
                {
                    error("电话号已存在!");
                    return;
                }

                // 更新用户电话号到数据库中
                u->Phone(phone);
                if (!_mysql_user->Update(u))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "mysql update user data error!");
                    error("mysql 数据库更新用户电话号操作失败!");
                    return;
                }

                // 更新 elasticsearch 中的用户信息
                if (!_es_user->Add(u->UserID(), u->Phone(), u->NickName(), u->Description(), u->AvatarID()))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "elasticsearch update user data error!");
                    error("elasticsearch 数据库更新用户电话号操作失败!");
                    return;
                }

                // 组织响应
                response->set_success(true);
            }

        private:
            // 检查昵称合法性
            auto nicknameCheck(std::string_view nickname) noexcept -> bool
            {
                // 字符长度检查 (UTF-8 字符数)
                const size_t len = nickname.size();
                if (len < 4 || len > 48) // 最大字节数(16字符*3字节)
                {
                    return false;
                }

                // 状态跟踪
                size_t charCount = 0;   // 字符计数（非字节）
                bool prevSpace = false; // 前一个字符是空格
                bool firstChar = true;  // 当前是首字符
                size_t i = 0;           // 字节索引

                // UTF-8 解码和检查
                while (i < len)
                {
                    // UTF-8 字符解码 (兼容 ASCII)
                    char c = nickname[i];
                    uint32_t codePoint = 0;
                    size_t charWidth = 0;

                    if (static_cast<uint8_t>(c) <= 0x7F)
                    { // ASCII
                        codePoint = static_cast<uint8_t>(c);
                        charWidth = 1;
                    }
                    else if ((static_cast<uint8_t>(c) & 0xE0) == 0xC0)
                    { // 2字节UTF-8
                        if (i + 1 >= len)
                        {
                            return false;
                        }
                        codePoint = ((static_cast<uint8_t>(nickname[i]) & 0x1F) << 6) |
                                    (static_cast<uint8_t>(nickname[i + 1]) & 0x3F);
                        charWidth = 2;
                    }
                    else if ((static_cast<uint8_t>(c) & 0xF0) == 0xE0)
                    { // 3字节UTF-8
                        if (i + 2 >= len)
                        {
                            return false;
                        }
                        codePoint = ((static_cast<uint8_t>(nickname[i]) & 0x0F) << 12) |
                                    ((static_cast<uint8_t>(nickname[i + 1]) & 0x3F) << 6) |
                                    (static_cast<uint8_t>(nickname[i + 2]) & 0x3F);
                        charWidth = 3;
                    }
                    else
                    {
                        return false; // 无效的UTF-8编码
                    }

                    // 移动字节索引
                    i += charWidth;
                    charCount++;

                    // 字符类型检查
                    bool isSpace = false;
                    bool isValid = false;

                    // ASCII 字符检查 (高效分支)
                    if (codePoint <= 0x7F)
                    {
                        if (std::isalnum(c) || c == '_' || c == '-')
                        {
                            isValid = true;
                        }
                        else if (c == ' ')
                        {
                            isSpace = true;
                            isValid = true;
                        }

                        // 首字符特殊规则
                        if (firstChar && !(std::isalpha(c) || c == '_'))
                        {
                            return false;
                        }
                    }
                    // 中文字符检查 (常用汉字范围)
                    else if (codePoint >= 0x4E00 && codePoint <= 0x9FA5)
                    {
                        isValid = true;

                        // 首字符可以是汉字
                        if (firstChar)
                        {
                            firstChar = false;
                        }
                    }

                    // 验证失败检查
                    if (!isValid)
                    {
                        return false;
                    }

                    // 空格规则检查
                    if (isSpace)
                    {
                        // 禁止连续空格
                        if (prevSpace)
                        {
                            return false;
                        }
                        prevSpace = true;

                        // 禁止尾空格（如果后面没有字符了）
                        if (i >= len)
                        {
                            return false;
                        }
                    }
                    else
                    {
                        prevSpace = false;
                    }

                    // 更新首字符状态
                    if (firstChar)
                    {
                        firstChar = false;
                    }
                }

                // 最终字符长度验证
                return (charCount >= 4 && charCount <= 16);
            }

            // 检查密码合法性
            auto passwordCheck(const std::string &password) -> bool
            {
                // 快速检查长度范围 (6~15)
                const size_t len = password.size();
                if (len < 6 || len > 15)
                    return false;

                // 使用 SIMD 优化检查可打印字符 (32-126)
                return std::ranges::all_of(password, [](char c)
                                           { return c >= 32 && c <= 126; });
            }

            // 检查手机号合法性
            auto phoneCheck(const std::string &phone) -> bool
            {
                // 长度检查
                if (phone.size() != 11)
                    return false;

                // 快速检查首位+第二位（1 + 3-9）
                if (phone[0] != '1' || phone[1] < '3' || phone[1] > '9')
                    return false;

                // SIMD优化的全数字检查
                if (!std::ranges::all_of(phone, [](char c) noexcept
                                         { return static_cast<uint8_t>(c) - '0' < 10; }))
                {
                    return false;
                }

                // 提取前三位号段
                int prefix = std::stoi(phone.substr(0, 3));

                // 中国大陆主要运营商号段（截至2025年）
                static const std::vector<int> validPrefixes = {
                    // 中国移动
                    134, 135, 136, 137, 138, 139, 144, 147, 148, 150, 151, 152, 157, 158, 159,
                    165, 172, 178, 182, 183, 184, 187, 188, 195, 197, 198,
                    // 中国联通
                    130, 131, 132, 140, 145, 146, 155, 156, 166, 167, 175, 176, 185, 186, 196,
                    // 中国电信
                    133, 134, 149, 153, 162, 170, 173, 174, 177, 180, 181, 189, 190, 191, 193, 199,
                    // 虚拟运营商/物联网
                    141, 142, 143, 154, 161, 164, 170, 171};

                // 检查号段是否在预授权列表中
                return std::ranges::find(validPrefixes, prefix) != validPrefixes.end();
            }

            // 检查签名合法性
            auto descriptionCheck(std::string_view description) -> bool
            {
                // 快速失败：空签名
                if (description.empty())
                    return false;

                // 快速检查字节长度范围 (最大50字符 * 4字节/字符)
                if (description.size() > 200)
                    return false;

                // 状态跟踪
                size_t charCount = 0;     // 字符计数（非字节）
                size_t spaceCount = 0;    // 连续空格计数
                bool hasNonSpace = false; // 是否存在非空格字符
                size_t i = 0;             // 字节索引

                // 禁用控制字符和危险符号 (ASCII 0-31 + 删除符)
                constexpr std::array forbiddenAscii = {
                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x0B, 0x0C, 0x0E, 0x0F, 0x10, 0x11, 0x12,
                    0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A,
                    0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x7F};

                // UTF-8 解码和检查
                while (i < description.size())
                {
                    // UTF-8 字符解码
                    const char c = description[i];
                    const uint8_t u = static_cast<uint8_t>(c);

                    // 处理ASCII字符
                    if (u <= 0x7F)
                    {
                        // 禁止控制字符和删除符
                        if (std::find(forbiddenAscii.begin(), forbiddenAscii.end(), u) != forbiddenAscii.end())
                        {
                            return false;
                        }

                        // 检查空格
                        if (c == ' ')
                        {
                            // 禁止开头空格
                            if (charCount == 0)
                                return false;

                            spaceCount++;
                            // 最多允许两个连续空格
                            if (spaceCount > 2)
                                return false;
                        }
                        else
                        {
                            hasNonSpace = true;
                            spaceCount = 0;
                        }

                        charCount++;
                        i++;
                    }
                    // 处理非ASCII字符 (2-4字节UTF-8)
                    else if ((u & 0xE0) == 0xC0)
                    { // 2字节字符
                        if (i + 1 >= description.size())
                            return false;
                        charCount++;
                        i += 2;
                        hasNonSpace = true;
                        spaceCount = 0;
                    }
                    else if ((u & 0xF0) == 0xE0)
                    { // 3字节字符
                        if (i + 2 >= description.size())
                            return false;
                        charCount++;
                        i += 3;
                        hasNonSpace = true;
                        spaceCount = 0;
                    }
                    else if ((u & 0xF8) == 0xF0)
                    { // 4字节字符 (emoji)
                        if (i + 3 >= description.size())
                            return false;
                        charCount++;
                        i += 4;
                        hasNonSpace = true;
                        spaceCount = 0;
                    }
                    else
                    {
                        // 无效的UTF-8编码
                        return false;
                    }

                    // 字符数超限
                    if (charCount > 50)
                        return false;
                }

                // 检查结尾空格
                if (!description.empty() && description.back() == ' ')
                {
                    return false;
                }

                // 检查是否全是空格
                if (!hasNonSpace)
                {
                    return false;
                }

                // 检查最小长度
                return charCount >= 1;
            }

            // 检查昵称在数据库中存在性
            auto isNicknameExist(const std::string &nickname) -> bool
            {
                if (_mysql_user->SelectNickName(nickname))
                {
                    return true; // 昵称已存在
                }
                else
                {
                    return false; // 昵称不存在
                }
            }

            // 检查手机号在数据库中存在性
            auto isPhoneNumberExist(const std::string &phone) -> bool
            {
                if (_mysql_user->SelectPhone(phone))
                {
                    return true; // 手机号已存在
                }
                else
                {
                    return false; // 昵称不存在
                }
            }

        private:
            std::string _file_service_name; // 文件子服务名称, 便于 channel 连接文件子服务信道并发起 rpc 调用

            // std::shared_ptr<elasticlient::Client> _es_client; // elasticsearch 客户端操作句柄 -- 先
            ESUser::esUserPtr _es_user; // elasticsearch user 数据操作句柄 -- 后

            // std::shared_ptr<odb::core::database> _mysql_client; // mysql 数据库操作句柄 -- 先
            UserTable::userTablePtr _mysql_user; // user ODB 数据库操作句柄 -- 后

            // std::shared_ptr<sw::redis::Redis> _redis_client; // redis 数据库操作句柄 -- 先
            Session::sessionPtr _redis_session; // 登录会话操作句柄 -- 后
            Status::statusPtr _redis_status;    // 登录状态操作句柄 -- 后
            Codes::codesPtr _redis_codes;       // 短信验证码操作句柄 -- 后

            // 短信验证平台
            DMS::DMSClient::dmsPtr _dms_client; // 阿里短信验证码 SDK 客户端句柄

            // rpc 调用客户端对象
            std::shared_ptr<Brpc::ChannelManager> _channel_manager; // 服务管理器句柄
        };

        class UserServer
        {
        public:
            using userPtr = std::shared_ptr<UserServer>;

        public:
            UserServer(const Etcd::Discovery::discoveryPtr &serviceDiscovery,
                       const Etcd::Registry::registryPtr &regClient,
                       const std::shared_ptr<brpc::Server> &rpcServer,
                       const std::shared_ptr<elasticlient::Client> &esClient,
                       const std::shared_ptr<odb::core::database> &mysqlClient,
                       const std::shared_ptr<sw::redis::Redis> &redisClient)
                : _service_discovery(serviceDiscovery),
                  _reg_client(regClient),
                  _rpc_server(rpcServer),
                  _es_client(esClient),
                  _mysql_client(mysqlClient),
                  _redis_client(redisClient)
            {
            }
            ~UserServer() = default;

            // 启动 rpc 服务器
            auto Run() const -> void
            {
                _rpc_server->RunUntilAskedToQuit();
            }

        private:
            Etcd::Discovery::discoveryPtr _service_discovery; // 服务发现句柄
            Etcd::Registry::registryPtr _reg_client;          // 服务注册句柄
            std::shared_ptr<brpc::Server> _rpc_server;        // 服务器搭建

            std::shared_ptr<elasticlient::Client> _es_client;   // elasticsearch 客户端操作句柄
            std::shared_ptr<odb::core::database> _mysql_client; // mysql 数据库操作句柄
            std::shared_ptr<sw::redis::Redis> _redis_client;    // redis 数据库操作句柄
        };

        // 建造者模式, 掌控 UserServer 的构造过程
        class UserServerBuilder
        {
        public:
            // 构造 elasticsearch 客户端对象
            auto MakeES(const std::vector<std::string> &hostList) -> void
            {
                _es_client = Operator::ESClientFactory::Create(hostList);
            }

            // 构造 mysql 客户端对象
            auto MakeMySQL(const std::string &user,
                           const std::string &pswd,
                           const std::string &host,
                           const std::string &db,
                           const std::string &cset,
                           int port,
                           int connPoolCount) -> void
            {
                _mysql_client = Operator::ODBFactory::Create(user, pswd, host, db, cset, port, connPoolCount);
            }

            // 构造 redis 客户端对象
            auto MakeRedis(const std::string &host,
                           const int port,
                           const int db,
                           const bool keepAlive) -> void
            {
                _redis_client = Operator::RedisFactory::Create(host, port, db, keepAlive);
            }

            // 构造服务发现 + channel 管理客户端对象
            auto MakeDiscovery(const std::string &regHost,
                               const std::string &baseServiceName,
                               const std::string &fileServiceName) -> void
            {
                _file_service_name = fileServiceName;
                _channel_manager = std::make_shared<Brpc::ChannelManager>();
                _channel_manager->Declared(_file_service_name); // 声明关注的服务名称
                auto putCb = std::bind(&Brpc::ChannelManager::Online, _channel_manager.get(), std::placeholders::_1, std::placeholders::_2);
                auto delCb = std::bind(&Brpc::ChannelManager::Offline, _channel_manager.get(), std::placeholders::_1, std::placeholders::_2);
                _service_discovery = std::make_shared<Etcd::Discovery>(regHost, baseServiceName, putCb, delCb);
            }

            // 构造服务注册客户端对象
            auto MakeRegistry(const std::string &regHost,
                              const std::string &serviceName,
                              const std::string &accessHost) -> void
            {
                _reg_client = std::make_shared<Etcd::Registry>(regHost);
                _reg_client->registry(serviceName, accessHost);
            }

            // 构造短信验证码平台 SDK 操作句柄
            auto MakeDMS(const std::string &accessKeyId,
                         const std::string &accessKeySecret) -> void
            {
                _dms_client = std::make_shared<DMS::DMSClient>(accessKeyId, accessKeySecret);
            }

            // 构造 rpc 服务器
            auto MakeRPC(uint16_t port, int32_t timeOut = -1, uint8_t numThreads = 4) -> void
            {
                if (!_es_client || !_mysql_client || !_redis_client || !_channel_manager || !_dms_client)
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(), "basic components not fully initialized");
                    return;
                }

                _rpc_server = std::make_shared<brpc::Server>();

                // 向服务器对象中新增服务
                UserServiceImpl *userServiceImpl = new UserServiceImpl(_es_client,
                                                                       _mysql_client,
                                                                       _redis_client,
                                                                       _channel_manager,
                                                                       _dms_client,
                                                                       _file_service_name);
                int ret = _rpc_server->AddService(userServiceImpl,
                                                  brpc::ServiceOwnership::SERVER_OWNS_SERVICE);
                // ServiceOwnership: 枚举, 添加服务失败时如何处理
                if (ret == -1)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "add server failed: {}", ret);
                    return;
                }

                brpc::ServerOptions options;
                options.idle_timeout_sec = timeOut;       // 设置空闲超时时间为 n 秒 -- 一般不设置,默认 -1 , 超时会自动关闭
                options.num_threads = numThreads;         // 设置 IO 线程数量 -- 一般不设置,默认 4 , 线程数量会自动根据 cpu 核心数量调整
                ret = _rpc_server->Start(port, &options); // 启动 rpc 服务器
                if (ret == -1)
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(), "start rpc server failed: {}", ret);
                    return;
                }
            }

            auto MakeDaemon(bool use = true, const std::string &workDir = "") -> void
            {
                _use_daemon = use;
                _work_dir = workDir;
            }

            auto Build() -> UserServer::userPtr
            {
                if (!_es_client || !_mysql_client || !_redis_client || !_channel_manager ||
                    !_service_discovery || !_reg_client || !_rpc_server || !_dms_client)
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(), "UserServerBuilder not fully initialized");
                    return nullptr;
                }

                if (_use_daemon)
                {
                    Daemon::Daemon(_work_dir);
                }

                return std::make_shared<UserServer>(_service_discovery,
                                                    _reg_client,
                                                    _rpc_server,
                                                    _es_client,
                                                    _mysql_client,
                                                    _redis_client);
            }

        private:
            Etcd::Registry::registryPtr _reg_client;

            std::shared_ptr<odb::core::database> _mysql_client; // mysql 数据库操作句柄
            std::shared_ptr<elasticlient::Client> _es_client;   // elasticsearch 客户端操作句柄
            std::shared_ptr<sw::redis::Redis> _redis_client;    // redis 数据库操作句柄

            std::string _file_service_name;                         // 文件子服务名称, 便于 channel 连接文件子服务信道并发起 rpc 调用
            std::shared_ptr<Brpc::ChannelManager> _channel_manager; // 服务管理器句柄
            Etcd::Discovery::discoveryPtr _service_discovery;       // 服务注册发现句柄

            DMS::DMSClient::dmsPtr _dms_client; // 阿里短信验证码 SDK 客户端句柄

            std::shared_ptr<brpc::Server> _rpc_server;

            bool _use_daemon;      // 是否启用守护进程
            std::string _work_dir; // 守护进程工作目录
        };
    }
}