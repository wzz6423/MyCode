/*
    实现 transmite 微服务
*/

#pragma once

// C
#include <cctype>
// C++
#include <string>
#include <vector>
#include <memory>
#include <optional>
// Other
#include "../../common/etcd/etcd.hpp"                    // 服务注册 etcd 模块封装
#include "../../common/brpc/brpc.hpp"                    // brpc 模块封装
#include "../../common/log/logger.hpp"                   // 日志器 spdlog 模块封装
#include "../../common/utils/utils.hpp"                  // 工具函数(生成唯一 uid)模块封装
#include "../../common/daemon/daemon.hpp"                // 守护进程模块封装
#include "../../common/odb/chat_session_member.hxx"      // chat_session_member odb 数据库操作封装
#include "../../common/operator/operator.hpp"            // chat_session_member odb 数据库操作封装
#include "../../common/operator/chat_session_member.hpp" // chat_session_member odb 数据库操作封装
#include "../../common/rabbitmq/rabbitmq.hpp"            // 消息队列(rabbitmq)模块封装
#include "base.pb.h"                                     // protobuf 框架代码
#include "transmite.pb.h"                                // protobuf 框架代码
#include "user.pb.h"                                     // protobuf 框架代码

namespace stellar_post
{
    namespace transmite
    {
        class TransmiteServiceImpl : public MsgTransmiteService
        {
        public:
            TransmiteServiceImpl(const std::shared_ptr<Brpc::ChannelManager> &channel_manager,
                                 const std::shared_ptr<odb::core::database> &mysql_db,
                                 const mqclient::RabbitMQ::mqClientPtr &mq_client,
                                 const std::string &user_service_name,
                                 const std::string &exchange_name,
                                 const std::string &routing_key)
                : _user_service_name(user_service_name),
                  _exchange_name(exchange_name),
                  _channel_manager(channel_manager),
                  _mysql_session_member_table(std::make_shared<crony::ChatSessionMemberTable>(mysql_db)),
                  _mq_client(mq_client),
                  _routing_key(routing_key)
            {
            }

            ~TransmiteServiceImpl() = default;

            virtual auto GetTransmiteTarget(google::protobuf::RpcController *controller,
                                            const NewMessageReq *request,
                                            GetTransmiteTargetRsp *response,
                                            google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());
                // 定义错误处理函数, 当出错时被调用
                auto error = [this, response](const std::string &errmsg)
                {
                    response->set_errmsg(errmsg);
                    response->set_success(false);
                };

                // 从请求中获取信息: 用户 ID, 所属会话 ID, 消息内容
                std::string userID = request->user_id();
                std::string chatSessionID = request->chat_session_id();
                base::MessageContent content = request->message();

                // 进行消息组织: 发送者 -- user, 所属会话, 消息内容, 产生时间, 消息 ID
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "user service {} channel not found", _user_service_name);
                    error("user service channel not found");
                    return;
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                user::UserService_Stub stub(channel.get());
                user::GetUserInfoReq req;
                user::GetUserInfoRsp rsp;
                req.set_request_id(request->request_id());
                req.set_user_id(request->user_id());
                std::shared_ptr<brpc::Controller> cntl = std::make_shared<brpc::Controller>();
                stub.GetUserInfo(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "user server error: ", cntl->ErrorText());
                    error("用户子服务不可用!");
                    return;
                }

                base::MessageInfo message;
                message.set_message_id(Utils::Uuid());
                message.set_chat_session_id(chatSessionID);
                message.set_timestamp(Utils::Now());
                message.mutable_sender()->CopyFrom(rsp.user_info());
                message.mutable_message()->CopyFrom(content);

                // 获取消息转发列表
                std::optional<std::vector<std::string>> forwardList_opt =
                    _mysql_session_member_table->AllSessionMember(chatSessionID);
                if (!forwardList_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "get session member error");
                    error("获取会话成员错误!");
                    return;
                }
                std::vector<std::string> forwardList = forwardList_opt.value();

                // 将封装完毕的消息发布到消息队列, 待消息存储子服务进行消息持久化
                if (!_mq_client->Publish(_exchange_name, message.SerializeAsString(), _routing_key)) // protobuf 序列化
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "RabbitMQ publish message error");
                    error("消息队列发布消息错误!");
                    return;
                }

                // 组织响应
                response->set_success(true);
                response->mutable_message()->CopyFrom(message);
                for (auto &id : forwardList)
                {
                    response->add_target_id_list(std::move(id));
                }
            }

        private:
            // 用户子服务调用相关信息
            std::string _user_service_name;

            // rpc 调用客户端对象
            std::shared_ptr<Brpc::ChannelManager> _channel_manager; // 服务管理器句柄

            // 聊天会话成员表操作句柄
            crony::ChatSessionMemberTable::csmtPtr _mysql_session_member_table; // user ODB 数据库操作句柄

            // RabbitMQ 交换机名称
            std::string _exchange_name;
            // 消息队列客户端句柄, 用于消息发布
            mqclient::RabbitMQ::mqClientPtr _mq_client;
            // RabbitMQ routing_key
            std::string _routing_key;
        };

        class TransmiteServer
        {
        public:
            using transmitePtr = std::shared_ptr<TransmiteServer>;

        public:
            TransmiteServer(const Etcd::Discovery::discoveryPtr &serviceDiscovery,
                            const Etcd::Registry::registryPtr &regClient,
                            const std::shared_ptr<brpc::Server> &rpcServer,
                            const std::shared_ptr<odb::core::database> &mysqlClient)
                : _service_discovery(serviceDiscovery),
                  _reg_client(regClient),
                  _rpc_server(rpcServer),
                  _mysql_client(mysqlClient)
            {
            }
            ~TransmiteServer() = default;

            // 启动 rpc 服务器
            auto Run() const -> void
            {
                _rpc_server->RunUntilAskedToQuit();
            }

        private:
            Etcd::Discovery::discoveryPtr _service_discovery; // 服务发现句柄
            Etcd::Registry::registryPtr _reg_client;          // 服务注册句柄
            std::shared_ptr<brpc::Server> _rpc_server;        // 服务器搭建

            std::shared_ptr<odb::core::database> _mysql_client; // mysql 数据库操作句柄
        };

        // 建造者模式, 掌控 TransmiteServer 的构造过程
        class TransmiteServerBuilder
        {
        public:
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

            // 构造服务发现 + channel 管理客户端对象
            auto MakeDiscovery(const std::string &regHost,
                               const std::string &baseServiceName,
                               const std::string &userServiceName) -> void
            {
                _user_service_name = userServiceName;
                _channel_manager = std::make_shared<Brpc::ChannelManager>();
                _channel_manager->Declared(_user_service_name); // 声明关注的服务名称
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

            // 构造 RabbitMQ 客户端对象
            auto MakeRabbitMQ(const std::string &user,
                              const std::string &passwd,
                              const std::string &host,
                              const std::string &exchangeName,
                              const std::string &queueName,
                              const std::string &bindingKey) -> void
            {
                _exchange_name = exchangeName;
                _routing_key = bindingKey;
                _mq_client = std::make_shared<mqclient::RabbitMQ>(user, passwd, host);
                _mq_client->DeclareComponents(exchangeName, queueName, bindingKey);
            }

            // 构造 rpc 服务器
            auto MakeRPC(uint16_t port, int32_t timeOut = -1, uint8_t numThreads = 4) -> void
            {
                if (!_mysql_client || !_channel_manager || !_mq_client || _user_service_name.empty() == true)
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(), "basic components not fully initialized");
                    return;
                }

                _rpc_server = std::make_shared<brpc::Server>();

                // 向服务器对象中新增服务
                TransmiteServiceImpl *transmiteServiceImpl = new TransmiteServiceImpl(_channel_manager,
                                                                                      _mysql_client,
                                                                                      _mq_client,
                                                                                      _user_service_name,
                                                                                      _exchange_name,
                                                                                      _routing_key);
                int ret = _rpc_server->AddService(transmiteServiceImpl,
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

            auto Build() -> TransmiteServer::transmitePtr
            {
                if (!_service_discovery || !_reg_client || !_rpc_server || !_mysql_client ||
                    !_mq_client || !_channel_manager || !_service_discovery)
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(), "TransmiteServer not fully initialized");
                    return nullptr;
                }

                if (_use_daemon)
                {
                    Daemon::Daemon(_work_dir);
                }

                return std::make_shared<TransmiteServer>(_service_discovery,
                                                         _reg_client,
                                                         _rpc_server, _mysql_client);
            }

        private:
            std::string _user_service_name; // 用户子服务调用相关信息

            Etcd::Discovery::discoveryPtr _service_discovery; // 服务发现句柄
            Etcd::Registry::registryPtr _reg_client;          // 服务注册句柄
            std::shared_ptr<brpc::Server> _rpc_server;        // 服务器搭建

            std::shared_ptr<odb::core::database> _mysql_client; // mysql 数据库操作句柄

            std::shared_ptr<Brpc::ChannelManager> _channel_manager; // 服务管理器句柄

            mqclient::RabbitMQ::mqClientPtr _mq_client; // 消息队列客户端句柄, 用于消息发布
            std::string _exchange_name;                 // RabbitMQ 交换机名称
            std::string _routing_key;                   // RabbitMQ routing_key

            bool _use_daemon;      // 是否启用守护进程
            std::string _work_dir; // 守护进程工作目录
        };
    }
}