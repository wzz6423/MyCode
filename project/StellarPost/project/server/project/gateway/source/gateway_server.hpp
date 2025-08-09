/*
    实现 gateway 微服务
*/

#pragma once

// C
#include <cctype>
// C++
#include <string>
#include <memory>
#include <functional>
#include <optional>
// Other
#include "../../common/etcd/etcd.hpp"           // 服务注册 etcd 模块封装
#include "../../common/brpc/brpc.hpp"           // brpc 模块封装
#include "../../common/log/logger.hpp"          // 日志器 spdlog 模块封装
#include "../../common/operator/operator.hpp"   // 客户端构造模块封装
#include "../../common/operator/user_redis.hpp" // redis 模块封装
#include "../../common/daemon/daemon.hpp"       // 守护进程模块封装
#include "../httplib/httplib.h"                 // httplib 头文件
#include "../operator/gateway_websocket.hpp"    // websocket 模块封装
#include "base.pb.h"                            // protobuf 框架代码
#include "user.pb.h"                            // protobuf 框架代码
#include "file.pb.h"                            // protobuf 框架代码
#include "crony.pb.h"                           // protobuf 框架代码
#include "gateway.pb.h"                         // protobuf 框架代码
#include "message.pb.h"                         // protobuf 框架代码
#include "notify.pb.h"                          // protobuf 框架代码
#include "speech.pb.h"                          // protobuf 框架代码
#include "transmite.pb.h"                       // protobuf 框架代码

namespace stellar_post
{
    namespace gateway
    {
        class GatewayServer
        {
        public:
            using gatewayPtr = std::shared_ptr<GatewayServer>;

        public:
            GatewayServer(const std::shared_ptr<Brpc::ChannelManager> &channelManager,
                          const std::shared_ptr<sw::redis::Redis> &redisClient,
                          const Etcd::Discovery::discoveryPtr &service_discovery,
                          const int &websocketPort,
                          const int &httpPort,
                          const std::string &user_service_name,
                          const std::string &file_service_name,
                          const std::string &crony_service_name,
                          const std::string &speech_service_name,
                          const std::string &transmite_service_name,
                          const std::string &message_service_name)
                : _channel_manager(channelManager),
                  _redis_session(std::make_shared<user::Session>(redisClient)),
                  _redis_status(std::make_shared<user::Status>(redisClient)),
                  _user_service_name(user_service_name),
                  _file_service_name(file_service_name),
                  _crony_service_name(crony_service_name),
                  _speech_service_name(speech_service_name),
                  _transmite_service_name(transmite_service_name),
                  _message_service_name(message_service_name),
                  _service_discovery(service_discovery),
                  _connections(std::make_shared<Connection>())
            {
                // 初始化日志输出 -- 关闭日志输出
                _websocket_server.set_access_channels(websocketpp::log::alevel::none);
                // 初始化 ASIO 框架
                _websocket_server.init_asio();
                // 设置 握手成功连接 & 消息处理 & 连接关闭 回调函数
                auto openHdl = [&](auto &&weak_ptr)
                {
                    onOpen(std::forward<decltype(weak_ptr)>(weak_ptr));
                };
                _websocket_server.set_open_handler(openHdl);
                auto messageHdl = [&](auto &&weak_ptr, auto &&msg_ptr)
                {
                    onMessage(std::forward<decltype(weak_ptr)>(weak_ptr), std::forward<decltype(msg_ptr)>(msg_ptr));
                };
                _websocket_server.set_message_handler(messageHdl);
                auto closeHdl = [&](auto &&weak_ptr)
                {
                    onClose(std::forward<decltype(weak_ptr)>(weak_ptr));
                };
                _websocket_server.set_close_handler(closeHdl);
                // 启用地址重用
                _websocket_server.set_reuse_addr(true);
                // 设置监听端口
                _websocket_server.listen(websocketPort);
                // 开始监听
                _websocket_server.start_accept();

                _http_server.Post(get_phone_verify_code, [&](const httplib::Request &request, httplib::Response &response)
                                  { onGetPhoneVerifyCode(request, response); });
                _http_server.Post(username_register, [&](const httplib::Request &request, httplib::Response &response)
                                  { onUserRegister(request, response); });
                _http_server.Post(username_login, [&](const httplib::Request &request, httplib::Response &response)
                                  { onUserLogin(request, response); });
                _http_server.Post(phone_register, [&](const httplib::Request &request, httplib::Response &response)
                                  { onPhoneRegister(request, response); });
                _http_server.Post(phone_login, [&](const httplib::Request &request, httplib::Response &response)
                                  { onPhoneLogin(request, response); });
                _http_server.Post(get_user_info, [&](const httplib::Request &request, httplib::Response &response)
                                  { onGetUserInfo(request, response); });
                _http_server.Post(set_avatar, [&](const httplib::Request &request, httplib::Response &response)
                                  { onSetUserAvatar(request, response); });
                _http_server.Post(set_nickname, [&](const httplib::Request &request, httplib::Response &response)
                                  { onSetUserNickname(request, response); });
                _http_server.Post(set_description, [&](const httplib::Request &request, httplib::Response &response)
                                  { onSetUserDescription(request, response); });
                _http_server.Post(set_phone, [&](const httplib::Request &request, httplib::Response &response)
                                  { onSetUserPhoneNumber(request, response); });
                _http_server.Post(get_crony_list, [&](const httplib::Request &request, httplib::Response &response)
                                  { onGetCronyList(request, response); });
                _http_server.Post(add_crony_apply, [&](const httplib::Request &request, httplib::Response &response)
                                  { onCronyAdd(request, response); });
                _http_server.Post(add_crony_process, [&](const httplib::Request &request, httplib::Response &response)
                                  { onCronyAddProcess(request, response); });
                _http_server.Post(remove_crony, [&](const httplib::Request &request, httplib::Response &response)
                                  { onCronyRemove(request, response); });
                _http_server.Post(search_crony, [&](const httplib::Request &request, httplib::Response &response)
                                  { onCronySearch(request, response); });
                _http_server.Post(get_chat_session_list, [&](const httplib::Request &request, httplib::Response &response)
                                  { onGetChatSessionList(request, response); });
                _http_server.Post(create_chat_session, [&](const httplib::Request &request, httplib::Response &response)
                                  { onChatSessionCreate(request, response); });
                _http_server.Post(get_chat_session_member, [&](const httplib::Request &request, httplib::Response &response)
                                  { onGetChatSessionMember(request, response); });
                _http_server.Post(get_pending_crony_events, [&](const httplib::Request &request, httplib::Response &response)
                                  { onGetPendingCronyEventList(request, response); });
                _http_server.Post(get_history, [&](const httplib::Request &request, httplib::Response &response)
                                  { onGetHistoryMsg(request, response); });
                _http_server.Post(get_recent, [&](const httplib::Request &request, httplib::Response &response)
                                  { onGetRecentMsg(request, response); });
                _http_server.Post(search_history, [&](const httplib::Request &request, httplib::Response &response)
                                  { onMsgSearch(request, response); });
                _http_server.Post(new_message, [&](const httplib::Request &request, httplib::Response &response)
                                  { onGetTransmiteTarget(request, response); });
                _http_server.Post(get_single_file, [&](const httplib::Request &request, httplib::Response &response)
                                  { onGetSingleFile(request, response); });
                _http_server.Post(get_multi_file, [&](const httplib::Request &request, httplib::Response &response)
                                  { onGetMultiFile(request, response); });
                _http_server.Post(put_single_file, [&](const httplib::Request &request, httplib::Response &response)
                                  { onPutSingleFile(request, response); });
                _http_server.Post(put_multi_file, [&](const httplib::Request &request, httplib::Response &response)
                                  { onPutMultiFile(request, response); });
                _http_server.Post(recognition, [&](const httplib::Request &request, httplib::Response &response)
                                  { onSpeechRecognition(request, response); });

                _http_thread = std::thread([&]()
                                           { _http_server.listen("0.0.0.0", httpPort); });
                _http_thread.detach();
            }

            ~GatewayServer()
            {
            }

            // 启动
            auto Run() -> void
            {
                _websocket_server.run();
            }

        private:
            // websocket 回调函数
            // 握手成功连接
            auto onOpen(websocketpp::connection_hdl hdl) -> void
            {
                // 无需处理, 长连接仅用于事件推送, 在服务端接收到第一条消息时进行连接管理即可
            }

            // 消息处理
            auto onMessage(websocketpp::connection_hdl hdl, Server_t::message_ptr msg) -> void
            {
                // 收到消息时, 根据消息中的会话 ID 进行身份识别, 将客户端长连接添加管理
                // 获取通信连接
                auto conn = _websocket_server.get_con_from_hdl(hdl);

                // 获取消息
                std::string message = msg->get_payload();

                // 针对消息内容进行反序列化 -- ClientAuthenticationReq: 身份认证对象
                ClientAuthenticationReq req;
                if (!req.ParseFromString(message))
                {
                    // 反序列化失败, 关闭连接
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    // conn->close(websocketpp::close::status::invalid_payload, "invalid payload");
                    _websocket_server.close(hdl, websocketpp::close::status::invalid_payload, "invalid payload");
                    return;
                }

                // 在会话信息缓存中, 查找会话信息, 使用 Redis 进行验证
                std::string sessionId = req.session_id();
                std::optional<std::string> userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    // 会话信息不存在, 关闭连接
                    Log::lg("error", Log::FileName(), Log::Line(), "session not found");
                    _websocket_server.close(hdl, websocketpp::close::status::unsupported_data, "unsupported data");
                    return;
                }
                std::string userId = userId_opt.value();

                // 会话信息存在则添加长连接管理
                _connections->Add(userId, sessionId, conn);

                // 开启长连接保活机制
                keepAlive(conn);
            }

            // 连接关闭
            auto onClose(websocketpp::connection_hdl hdl) -> void
            {
                // 长连接断开清理资源
                // 通过连接对象获取用户 ID 与登录会话 ID
                auto conn = _websocket_server.get_con_from_hdl(hdl);
                std::optional<std::unique_ptr<Connection::ClientInfo>> clientInfo =
                    _connections->GetClientInfo(conn);
                if (!clientInfo.has_value())
                {
                    // 连接对象不存在, 关闭连接
                    Log::lg("error", Log::FileName(), Log::Line(), "connection not found");
                    _websocket_server.close(hdl, websocketpp::close::status::unsupported_data, "unsupported data");
                    return;
                }
                std::string userId = clientInfo.value()->_user_id;
                std::string sessionId = clientInfo.value()->_session_id;

                // 移除登陆会话信息
                if (!_redis_session->Remove(sessionId))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "remove session failed");
                }

                // 移除登陆状态信息
                if (!_redis_status->Remove(userId))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "remove status failed");
                }

                // 移除长连接管理数据
                _connections->Remove(conn);
            }

            // http 回调函数
            // 格式: void(const httplib::Request&, httplib::Response&);
            // 手机号验证码获取
            auto onGetPhoneVerifyCode(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                user::PhoneVerifyCodeReq req;
                user::PhoneVerifyCodeRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 将请求转发给用户子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::user::UserService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.GetPhoneVerifyCode(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 得到用户子服务响应后, 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 用户名注册
            auto onUserRegister(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                user::UserRegisterReq req;
                user::UserRegisterRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 将请求转发给用户子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::user::UserService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.UserRegister(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 得到用户子服务响应后, 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 用户名登录
            auto onUserLogin(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                user::UserLoginReq req;
                user::UserLoginRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 将请求转发给用户子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::user::UserService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.UserLogin(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 得到用户子服务响应后, 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 手机号注册
            auto onPhoneRegister(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                user::PhoneRegisterReq req;
                user::PhoneRegisterRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 将请求转发给用户子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::user::UserService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.PhoneRegister(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 得到用户子服务响应后, 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 手机号登录
            auto onPhoneLogin(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                user::PhoneLoginReq req;
                user::PhoneLoginRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 将请求转发给用户子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::user::UserService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.PhoneLogin(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 得到用户子服务响应后, 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 获取用户信息
            auto onGetUserInfo(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                user::GetUserInfoReq req;
                user::GetUserInfoRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给用户子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::user::UserService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.GetUserInfo(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 得到用户子服务响应后, 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 设置用户头像
            auto onSetUserAvatar(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                user::SetUserAvatarReq req;
                user::SetUserAvatarRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给用户子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::user::UserService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.SetUserAvatar(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 得到用户子服务响应后, 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 设置用户昵称
            auto onSetUserNickname(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                user::SetUserNicknameReq req;
                user::SetUserNicknameRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给用户子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::user::UserService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.SetUserNickname(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 得到用户子服务响应后, 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 设置用户签名
            auto onSetUserDescription(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                user::SetUserDescriptionReq req;
                user::SetUserDescriptionRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给用户子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::user::UserService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.SetUserDescription(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 得到用户子服务响应后, 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 设置用户手机号
            auto onSetUserPhoneNumber(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                user::SetUserPhoneNumberReq req;
                user::SetUserPhoneNumberRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给用户子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::user::UserService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.SetUserPhoneNumber(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 得到用户子服务响应后, 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 获取好友列表
            auto onGetCronyList(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                crony::GetCronyListReq req;
                crony::GetCronyListRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给好友子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_crony_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::crony::CronyService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.GetCronyList(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 得到好友子服务响应后, 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 添加好友
            auto onCronyAdd(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                crony::CronyAddReq req;
                crony::CronyAddRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给好友子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_crony_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::crony::CronyService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.CronyAdd(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 如果申请事件成功创建且获取被申请方长连接成功, 对对方进行事件推送通知
                // 获取被申请方长连接
                auto conn_opt = _connections->Get(req.respondent_id());
                if (!conn_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "user not found");
                }
                else
                {
                    auto conn = conn_opt.value();

                    // 获取被申请方用户信息
                    std::optional<std::shared_ptr<user::GetUserInfoRsp>> userRsp =
                        getUserInfo(rsp.request_id(), req.respondent_id());
                    if (!userRsp.has_value())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(), "get user info failed");
                        return error("get user info failed");
                    }

                    // 组织事件通知消息
                    notify::NotifyMessage notifyMsg;
                    notifyMsg.set_notify_type(notify::NotifyType::CRONY_ADD_APPLY_NOTIFY);
                    notifyMsg.mutable_crony_add_apply()->mutable_user_info()->CopyFrom(userRsp.value()->user_info());
                    // 发送事件通知消息
                    conn->send(notifyMsg.SerializeAsString(), websocketpp::frame::opcode::value::binary);
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 添加好友处理
            auto onCronyAddProcess(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                crony::CronyAddProcessReq req;
                crony::CronyAddProcessRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权, 获取被申请人用户 ID
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 提取要素: 处理结果, 申请人
                bool agree = req.agree();
                std::string applyUserId = req.apply_user_id();

                // 将请求转发给好友子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_crony_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::crony::CronyService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.CronyAddProcess(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 如果申请事件成功创建且获取被申请方长连接成功, 对对方进行事件推送通知
                // 获取被申请方用户信息
                std::optional<std::shared_ptr<user::GetUserInfoRsp>> reqUserRsp =
                    getUserInfo(rsp.request_id(), userId_opt.value());
                if (!reqUserRsp.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "get user info failed");
                    return error("get user info failed");
                }
                // 获取申请方长连接
                auto applyConn_opt = _connections->Get(applyUserId);
                if (!applyConn_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "user not found");
                }
                else
                {
                    auto applyConn = applyConn_opt.value();

                    // 组织事件通知消息
                    notify::NotifyMessage notifyMsg;
                    notifyMsg.set_notify_type(notify::NotifyType::CRONY_ADD_PROCESS_NOTIFY);
                    notifyMsg.mutable_crony_process_result()->mutable_user_info()->CopyFrom(reqUserRsp.value()->user_info());
                    notifyMsg.mutable_crony_process_result()->set_agree(agree);
                    // 发送事件通知消息 -- 申请方
                    applyConn->send(notifyMsg.SerializeAsString(), websocketpp::frame::opcode::value::binary);
                }

                // 如果处理结果是同意, 会伴随单聊会话的创建, 故需要对双方进行会话创建的通知
                if (agree)
                {
                    // 获取被申请方长连接
                    auto reqConn_opt = _connections->Get(userId_opt.value());
                    if (!reqConn_opt.has_value())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(), "user not found");
                    }
                    else
                    {
                        auto reqConn = reqConn_opt.value();
                        // 获取申请方用户信息
                        std::optional<std::shared_ptr<user::GetUserInfoRsp>> applyUserRsp =
                            getUserInfo(rsp.request_id(), applyUserId);
                        if (!applyUserRsp.has_value())
                        {
                            Log::lg("error", Log::FileName(), Log::Line(), "get user info failed");
                            return error("get user info failed");
                        }

                        // 被申请人 -- 填写申请人信息
                        // 组织事件通知消息
                        notify::NotifyMessage reqnotifyMessage;
                        reqnotifyMessage.set_notify_type(notify::NotifyType::CHAT_SESSION_CREATE_NOTIFY);
                        reqnotifyMessage.mutable_new_chat_session_info()->mutable_chat_session_info()->set_single_chat_crony_id(applyUserId);
                        reqnotifyMessage.mutable_new_chat_session_info()->mutable_chat_session_info()->set_chat_session_id(rsp.new_session_id());
                        reqnotifyMessage.mutable_new_chat_session_info()->mutable_chat_session_info()->set_chat_session_name(applyUserRsp.value()->user_info().nickname());
                        reqnotifyMessage.mutable_new_chat_session_info()->mutable_chat_session_info()->set_avatar(applyUserRsp.value()->user_info().avatar());
                        // 发送事件通知消息
                        reqConn->send(reqnotifyMessage.SerializeAsString(), websocketpp::frame::opcode::value::binary);
                    }

                    if (applyConn_opt.has_value())
                    {
                        // 申请人 -- 填写被申请人信息
                        // 组织事件通知消息
                        auto applyConn = applyConn_opt.value();
                        notify::NotifyMessage applynotifyMessage;
                        applynotifyMessage.set_notify_type(notify::NotifyType::CHAT_SESSION_CREATE_NOTIFY);
                        applynotifyMessage.mutable_new_chat_session_info()->mutable_chat_session_info()->set_single_chat_crony_id(userId_opt.value());
                        applynotifyMessage.mutable_new_chat_session_info()->mutable_chat_session_info()->set_chat_session_id(rsp.new_session_id());
                        applynotifyMessage.mutable_new_chat_session_info()->mutable_chat_session_info()->set_chat_session_name(reqUserRsp.value()->user_info().nickname());
                        applynotifyMessage.mutable_new_chat_session_info()->mutable_chat_session_info()->set_avatar(reqUserRsp.value()->user_info().avatar());
                        // 发送事件通知消息
                        applyConn->send(applynotifyMessage.SerializeAsString(), websocketpp::frame::opcode::value::binary);
                    }
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 删除好友
            auto onCronyRemove(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                crony::CronyRemoveReq req;
                crony::CronyRemoveRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给好友子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_crony_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::crony::CronyService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.CronyRemove(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 如果删除事件成功创建且获取被删除方长连接成功, 对对方进行事件推送通知
                // 获取被删除方长连接
                auto conn_opt = _connections->Get(req.peer_id());
                if (!conn_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "user not found");
                }
                else
                {
                    auto conn = conn_opt.value();

                    // 组织事件通知消息
                    notify::NotifyMessage notifyMsg;
                    notifyMsg.set_notify_type(notify::NotifyType::CRONY_REMOVE_NOTIFY);
                    notifyMsg.mutable_crony_remove()->set_user_id(userId_opt.value());
                    // 发送事件通知消息
                    conn->send(notifyMsg.SerializeAsString(), websocketpp::frame::opcode::value::binary);
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 搜索好友
            auto onCronySearch(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                crony::CronySearchReq req;
                crony::CronySearchRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给好友子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_crony_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::crony::CronyService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.CronySearch(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 获取待处理好友事件列表
            auto onGetPendingCronyEventList(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                crony::GetPendingCronyEventListReq req;
                crony::GetPendingCronyEventListRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给好友子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_crony_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::crony::CronyService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.GetPendingCronyEventList(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 获取聊天会话列表
            auto onGetChatSessionList(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                crony::GetChatSessionListReq req;
                crony::GetChatSessionListRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给好友子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_crony_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::crony::CronyService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.GetChatSessionList(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 获取会话成员列表
            auto onGetChatSessionMember(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                crony::GetChatSessionMemberReq req;
                crony::GetChatSessionMemberRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给好友子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_crony_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::crony::CronyService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.GetChatSessionMember(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 创建聊天会话
            auto onChatSessionCreate(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                crony::ChatSessionCreateReq req;
                crony::ChatSessionCreateRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给好友子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_crony_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::crony::CronyService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.ChatSessionCreate(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 如果申请事件成功创建且获取群员长连接成功, 对群员进行事件推送通知
                for (const auto &e : req.member_id_list())
                {
                    // 获取群员长连接
                    auto conn_opt = _connections->Get(e);
                    if (!conn_opt.has_value())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(), "user not found");
                        continue;
                    }
                    auto conn = conn_opt.value();

                    // 组织事件通知消息
                    notify::NotifyMessage notifyMsg;
                    notifyMsg.set_notify_type(notify::NotifyType::CHAT_SESSION_CREATE_NOTIFY);
                    notifyMsg.mutable_new_chat_session_info()->mutable_chat_session_info()->CopyFrom(rsp.chat_session_info());
                    // 发送事件通知消息
                    conn->send(notifyMsg.SerializeAsString(), websocketpp::frame::opcode::value::binary);
                }

                // 将响应内容进行序列化作为 http 响应正文
                rsp.clear_chat_session_info();
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 获取聊天会话历史消息
            auto onGetHistoryMsg(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                message::GetHistoryMsgReq req;
                message::GetHistoryMsgRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给消息存储子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_message_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::message::MsgStorageService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.GetHistoryMsg(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 获取聊天会话最近消息
            auto onGetRecentMsg(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                message::GetRecentMsgReq req;
                message::GetRecentMsgRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给消息存储子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_message_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::message::MsgStorageService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.GetRecentMsg(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 搜索聊天会话历史消息
            auto onMsgSearch(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                message::MsgSearchReq req;
                message::MsgSearchRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给消息存储子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_message_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::message::MsgStorageService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.MsgSearch(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 获取单文件
            auto onGetSingleFile(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                file::GetSingleFileReq req;
                file::GetSingleFileRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给文件子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_file_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::file::FileService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.GetSingleFile(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 获取多文件
            auto onGetMultiFile(const httplib::Request &request, httplib::Response &response) -> void
            { // 从请求中取出 http 请求正文, 将正文进行反序列化
                file::GetMultiFileReq req;
                file::GetMultiFileRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给文件子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_file_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::file::FileService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.GetMultiFile(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 上传单文件
            auto onPutSingleFile(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                file::PutSingleFileReq req;
                file::PutSingleFileRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给文件子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_file_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::file::FileService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.PutSingleFile(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 上传多文件
            auto onPutMultiFile(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                file::PutMultiFileReq req;
                file::PutMultiFileRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给文件子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_file_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::file::FileService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.PutMultiFile(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 语音识别
            auto onSpeechRecognition(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                speech::SpeechRecognitionReq req;
                speech::SpeechRecognitionRsp rsp;
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给语音识别子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_speech_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::speech::SpeechService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.SpeechRecognition(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 将响应内容进行序列化作为 http 响应正文
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

            // 消息转发
            auto onGetTransmiteTarget(const httplib::Request &request, httplib::Response &response) -> void
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                transmite::NewMessageReq req;
                transmite::NewMessageRsp rsp;               // 给客户端的响应
                transmite::GetTransmiteTargetRsp targetRsp; // 请求子服务的响应
                // 定义错误回调
                auto error = [&](const std::string &errmsg)
                {
                    rsp.set_success(false);
                    rsp.set_errmsg(errmsg);
                    response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
                };
                if (!req.ParseFromString(request.body))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "deserialize message failed");
                    return error("deserialize message failed");
                }

                // 进行客户端身份识别鉴权
                std::string sessionId = req.session_id();
                auto userId_opt = _redis_session->Get(sessionId);
                if (!userId_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "session binds user not found");
                    return error("session binds user not found");
                }
                req.set_user_id(userId_opt.value());

                // 将请求转发给好友子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_transmite_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return error("channel not found");
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::transmite::MsgTransmiteService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.GetTransmiteTarget(cntl.get(), &req, &targetRsp, nullptr);
                if (cntl->Failed() || targetRsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return error("用户服务不可用!");
                }

                // 如果申请事件成功创建且获取群员长连接成功, 对群员进行事件推送通知
                for (const auto &e : targetRsp.target_id_list())
                {
                    // 如果是消息发送者则不进行消息推送
                    if (e == userId_opt.value())
                    {
                        continue;
                    }
                    // 获取群员长连接
                    auto conn_opt = _connections->Get(e);
                    if (!conn_opt.has_value())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(), "user not found");
                        continue;
                    }
                    auto conn = conn_opt.value();

                    // 组织事件通知消息
                    notify::NotifyMessage notifyMsg;
                    notifyMsg.set_notify_type(notify::NotifyType::CHAT_MESSAGE_NOTIFY);
                    notifyMsg.mutable_new_message_info()->mutable_message_info()->CopyFrom(targetRsp.message());
                    // 发送事件通知消息
                    conn->send(notifyMsg.SerializeAsString(), websocketpp::frame::opcode::value::binary);
                }

                // 将响应内容进行序列化作为 http 响应正文
                rsp.set_request_id(req.request_id());
                rsp.set_success(targetRsp.success());
                rsp.set_errmsg(targetRsp.errmsg());
                response.set_content(rsp.SerializeAsString(), "application/x-protobuf");
            }

        private:
            // 开启 websocket 长连接保活机制, 发送 ping 包进行探测
            auto keepAlive(const Server_t::connection_ptr &conn) -> void
            {
                if (conn && conn->get_state() == websocketpp::session::state::value::open)
                {
                    conn->ping("");
                    // 设置定时任务进行保活, 60000ms(1min) 发送一次 ping 包
                    _websocket_server.set_timer(60000, [&](const std::error_code &ec)
                                                { keepAlive(conn); });
                }
            }

            // 调用用户子服务获取用户信息
            auto getUserInfo(const std::string &requestId, const std::string &userId)
                -> std::optional<std::shared_ptr<user::GetUserInfoRsp>>
            {
                // 从请求中取出 http 请求正文, 将正文进行反序列化
                user::GetUserInfoReq req;
                std::shared_ptr<user::GetUserInfoRsp> rsp = std::make_shared<user::GetUserInfoRsp>();
                req.set_request_id(requestId);
                req.set_user_id(userId);

                // 将请求转发给用户子服务进行业务处理
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "channel not found");
                    return std::nullopt;
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                stellar_post::user::UserService_Stub stub(channel.get());
                std::unique_ptr<brpc::Controller> cntl = std::make_unique<brpc::Controller>();
                stub.GetUserInfo(cntl.get(), &req, rsp.get(), nullptr);
                if (cntl->Failed() || rsp->success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "server cannot use, error: {}", cntl->ErrorText());
                    return std::nullopt;
                }

                return std::make_optional(rsp);
            }

        private:
            inline static const std::string get_phone_verify_code = "/service/user/get_phone_verify_code";
            inline static const std::string username_register = "/service/user/username_register";
            inline static const std::string username_login = "/service/user/username_login";
            inline static const std::string phone_register = "/service/user/phone_register";
            inline static const std::string phone_login = "/service/user/phone_login";
            inline static const std::string get_user_info = "/service/user/get_user_info";
            inline static const std::string set_avatar = "/service/user/set_avatar";
            inline static const std::string set_nickname = "/service/user/set_nickname";
            inline static const std::string set_description = "/service/user/set_description";
            inline static const std::string set_phone = "/service/user/set_phone";
            inline static const std::string get_crony_list = "/service/crony/get_crony_list";
            inline static const std::string get_crony_info = "/service/crony/get_crony_info";
            inline static const std::string add_crony_apply = "/service/crony/add_crony_apply";
            inline static const std::string add_crony_process = "/service/crony/add_crony_process";
            inline static const std::string remove_crony = "/service/crony/remove_crony";
            inline static const std::string search_crony = "/service/crony/search_crony";
            inline static const std::string get_chat_session_list = "/service/crony/get_chat_session_list";
            inline static const std::string create_chat_session = "/service/crony/create_chat_session";
            inline static const std::string get_chat_session_member = "/service/crony/get_chat_session_member";
            inline static const std::string get_pending_crony_events = "/service/crony/get_pending_crony_events";
            inline static const std::string get_history = "/service/message_storage/get_history";
            inline static const std::string get_recent = "/service/message_storage/get_recent";
            inline static const std::string search_history = "/service/message_storage/search_history";
            inline static const std::string new_message = "/service/message_transmit/new_message";
            inline static const std::string get_single_file = "/service/file/get_single_file";
            inline static const std::string get_multi_file = "/service/file/get_multi_file";
            inline static const std::string put_single_file = "/service/file/put_single_file";
            inline static const std::string put_multi_file = "/service/file/put_multi_file";
            inline static const std::string recognition = "/service/speech/recognition";

        private:
            // rpc 调用客户端对象
            // std::shared_ptr<brpc::Server> _rpc_server; // rpc 服务器句柄
            std::shared_ptr<Brpc::ChannelManager> _channel_manager; // 服务管理器句柄
            std::string _user_service_name;                         // 用户子服务名称
            std::string _file_service_name;                         // 文件子服务名称
            std::string _crony_service_name;                        // 好友子服务名称
            std::string _speech_service_name;                       // 语音子服务名称
            std::string _transmite_service_name;                    // 消息转发子服务名称
            std::string _message_service_name;                      // 消息存储子服务名称

            // std::shared_ptr<sw::redis::Redis> _redis_client; // redis 数据库操作句柄
            user::Session::sessionPtr _redis_session; // 登录会话操作句柄
            user::Status::statusPtr _redis_status;    // 登录状态操作句柄

            Etcd::Discovery::discoveryPtr _service_discovery; // 服务注册发现句柄

            // 连接管理
            Connection::connPtr _connections;
            // 服务搭建
            Server_t _websocket_server;   // websocket 服务器句柄
            httplib::Server _http_server; // http 服务器句柄

            // 线程
            std::thread _http_thread; // http 服务线程
        };

        // 建造者模式, 掌控 GatewayServer 的构造过程
        class GatewayServerBuilder
        {
        public:
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
                               const std::string &user_service_name,
                               const std::string &file_service_name,
                               const std::string &crony_service_name,
                               const std::string &speech_service_name,
                               const std::string &transmite_service_name,
                               const std::string &message_service_name) -> void
            {
                _user_service_name = user_service_name;
                _file_service_name = file_service_name;
                _crony_service_name = crony_service_name;
                _speech_service_name = speech_service_name;
                _transmite_service_name = transmite_service_name;
                _message_service_name = message_service_name;
                _channel_manager = std::make_shared<Brpc::ChannelManager>();
                _channel_manager->Declared(_file_service_name);      // 声明关注的服务名称
                _channel_manager->Declared(_crony_service_name);     // 声明关注的服务名称
                _channel_manager->Declared(_speech_service_name);    // 声明关注的服务名称
                _channel_manager->Declared(_transmite_service_name); // 声明关注的服务名称
                _channel_manager->Declared(_message_service_name);   // 声明关注的服务名称
                _channel_manager->Declared(_user_service_name);      // 声明关注的服务名称
                auto putCb = std::bind(&Brpc::ChannelManager::Online, _channel_manager.get(), std::placeholders::_1, std::placeholders::_2);
                auto delCb = std::bind(&Brpc::ChannelManager::Offline, _channel_manager.get(), std::placeholders::_1, std::placeholders::_2);
                _service_discovery = std::make_shared<Etcd::Discovery>(regHost, baseServiceName, putCb, delCb);
            }

            auto MakeServer(const int &websocketPort, const int &httpPort) -> void
            {
                _websocket_port = websocketPort;
                _http_port = httpPort;
            }

            auto MakeDaemon(bool use = true, const std::string &workDir = "") -> void
            {
                _use_daemon = use;
                _work_dir = workDir;
            }

            auto Build() -> GatewayServer::gatewayPtr
            {
                if (!_redis_client || !_channel_manager || !_service_discovery ||
                    _user_service_name.empty() || _file_service_name.empty() ||
                    _crony_service_name.empty() || _speech_service_name.empty() ||
                    _transmite_service_name.empty() || _message_service_name.empty() ||
                    _websocket_port == -1 || _http_port == -1)
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(), "GatewayServerBuilder not fully initialized");
                    return nullptr;
                }

                if (_use_daemon)
                {
                    Daemon::Daemon(_work_dir);
                }

                return std::make_shared<GatewayServer>(_channel_manager,
                                                       _redis_client,
                                                       _service_discovery,
                                                       _websocket_port,
                                                       _http_port,
                                                       _user_service_name,
                                                       _file_service_name,
                                                       _crony_service_name,
                                                       _speech_service_name,
                                                       _transmite_service_name,
                                                       _message_service_name);
            }

        private:
            std::shared_ptr<sw::redis::Redis> _redis_client;        // redis 数据库操作句柄
            std::shared_ptr<Brpc::ChannelManager> _channel_manager; // 服务管理器句柄
            Etcd::Discovery::discoveryPtr _service_discovery;       // 服务注册发现句柄

            std::string _user_service_name;      // 用户子服务名称
            std::string _file_service_name;      // 文件子服务名称
            std::string _crony_service_name;     // 好友子服务名称
            std::string _speech_service_name;    // 语音子服务名称
            std::string _transmite_service_name; // 消息转发子服务名称
            std::string _message_service_name;   // 消息存储子服务名称

            int _websocket_port = -1; // websocket port
            int _http_port = -1;      // http port

            bool _use_daemon;      // 是否启用守护进程
            std::string _work_dir; // 守护进程工作目录
        };
    }
}