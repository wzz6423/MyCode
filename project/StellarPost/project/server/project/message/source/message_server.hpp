/*
    实现 message 微服务
*/

#pragma once

// C
#include <cctype>
// C++
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_set>
#include <unordered_map>
// Other
#include "../../common/etcd/etcd.hpp"         // 服务注册 etcd 模块封装
#include "../../common/brpc/brpc.hpp"         // brpc 模块封装
#include "../../common/log/logger.hpp"        // 日志器 spdlog 模块封装
#include "../../common/utils/utils.hpp"       // 工具函数(生成唯一 uid)模块封装
#include "../../common/daemon/daemon.hpp"     // 守护进程模块封装
#include "../../common/elastic/elastic.hpp"   // elasticsearch 模块封装
#include "../../common/rabbitmq/rabbitmq.hpp" // RabbitMQ 模块封装
#include "../operator/message_elastic.hpp"    // redis 模块封装
#include "../operator/message_mysql.hpp"      // mysql 模块封装
#include "../operator/message_elastic.hpp"    // elasticsearch 数据管理客户端模块封装
#include "base.pb.h"                          // protobuf 框架代码
#include "file.pb.h"                          // protobuf 框架代码
#include "user.pb.h"                          // protobuf 框架代码
#include "message.pb.h"                       // protobuf 框架代码
#include "../odb/message.hxx"                 // odb 数据库操作封装
#include "message-odb.hxx"                    // odb 数据库操作封装

namespace stellar_post
{
    namespace message
    {
        class MessageServiceImpl : public MsgStorageService
        {
        public:
            MessageServiceImpl() = delete;
            MessageServiceImpl(const std::shared_ptr<elasticlient::Client> &es_client,
                               const std::shared_ptr<odb::core::database> &mysql_client,
                               const std::shared_ptr<Brpc::ChannelManager> &channel_manager,
                               const std::string &file_service_name,
                               const std::string &user_service_name)
                : _es_client(std::make_shared<ESMessage>(es_client)),
                  _mysql_client(std::make_shared<MessageTable>(mysql_client)),
                  _channel_manager(channel_manager),
                  _file_service_name(file_service_name),
                  _user_service_name(user_service_name)
            {
                _es_client->CreateIndex();
            }

            ~MessageServiceImpl() = default;

            // 获取历史消息(时间区间)
            virtual auto GetHistoryMsg(::google::protobuf::RpcController *controller,
                                       const ::stellar_post::message::GetHistoryMsgReq *request,
                                       ::stellar_post::message::GetHistoryMsgRsp *response,
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

                // 提取关键要素 -- 请求 ID, 会话 ID, 起始时间, 结束时间
                std::string requestId = request->request_id();
                std::string chatSessionId = request->chat_session_id();
                boost::posix_time::ptime begin = boost::posix_time::from_time_t(request->start_time());
                boost::posix_time::ptime end = boost::posix_time::from_time_t(request->over_time());

                // 从数据库进行消息查询
                std::vector<std::shared_ptr<Message>> msgList = _mysql_client->SelectRange(chatSessionId, begin, end);
                if (msgList.empty())
                {
                    response->set_success(true);
                    return;
                }

                // 统计文件类型消息的文件 ID
                std::unordered_set<std::string> fileIds;
                for (const auto &msg : msgList)
                {
                    if (msg->FileID().empty() == false)
                    {
                        fileIds.insert(msg->FileID());
                    }
                }

                // 从文件子服务进行批量文件下载
                std::optional<std::unordered_map<std::string, std::string>> fileDatas_opt =
                    getFiles(requestId, fileIds);
                if (fileDatas_opt.has_value() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "get mul file from file server error!");
                    error("get mul file from file server error!");
                    return;
                }
                std::unordered_map<std::string, std::string> fileDatas = std::move(fileDatas_opt.value());

                // 统计所有消息发送者用户 ID, 从用户子服务进行批量用户信息获取
                std::unordered_set<std::string> userIds;
                for (const auto &msg : msgList)
                {
                    userIds.insert(msg->UserID());
                }
                std::optional<std::unordered_map<std::string, base::UserInfo>> userDatas_opt =
                    getUsers(requestId, userIds);
                if (userDatas_opt.has_value() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "get userinfos from user server error!");
                    error("get userinfos from user server error!");
                    return;
                }
                std::unordered_map<std::string, base::UserInfo> userDatas = std::move(userDatas_opt.value());

                // 组织响应
                for (const auto &msg : msgList)
                {
                    auto msgInfo = response->add_msg_list();
                    msgInfo->set_message_id(msg->MessageID());
                    msgInfo->set_chat_session_id(msg->ChatSessionId());
                    msgInfo->set_timestamp(boost::posix_time::to_time_t(msg->GenerateTime()));
                    msgInfo->mutable_sender()->CopyFrom(userDatas[msg->UserID()]);
                    switch (msg->MessageType())
                    {
                    case base::MessageType::STRING:
                        msgInfo->mutable_message()->set_message_type(base::MessageType::STRING);
                        msgInfo->mutable_message()->mutable_string_message()->set_content(msg->Content());
                        break;
                    case base::MessageType::IMAGE:
                        msgInfo->mutable_message()->set_message_type(base::MessageType::IMAGE);
                        msgInfo->mutable_message()->mutable_image_message()->set_file_id(msg->FileID());
                        msgInfo->mutable_message()->mutable_image_message()->set_image_content(fileDatas[msg->FileID()]);
                        break;
                    case base::MessageType::FILE:
                        msgInfo->mutable_message()->set_message_type(base::MessageType::FILE);
                        msgInfo->mutable_message()->mutable_file_message()->set_file_name(msg->FileName());
                        msgInfo->mutable_message()->mutable_file_message()->set_file_id(msg->FileID());
                        msgInfo->mutable_message()->mutable_file_message()->set_file_size(msg->FileSize());
                        msgInfo->mutable_message()->mutable_file_message()->set_file_contents(fileDatas[msg->FileID()]);
                        break;
                    case base::MessageType::SPEECH:
                        msgInfo->mutable_message()->set_message_type(base::MessageType::SPEECH);
                        msgInfo->mutable_message()->mutable_speech_message()->set_file_id(msg->FileID());
                        msgInfo->mutable_message()->mutable_speech_message()->set_file_contents(fileDatas[msg->FileID()]);
                        break;
                    default:
                        Log::lg("error", Log::FileName(), Log::Line(), "message type error!");
                        error("message type error!");
                        return;
                    }
                }
                response->set_success(true);
            }

            // 获取最近 n 条消息
            virtual auto GetRecentMsg(::google::protobuf::RpcController *controller,
                                      const ::stellar_post::message::GetRecentMsgReq *request,
                                      ::stellar_post::message::GetRecentMsgRsp *response,
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

                // 提取关键要素 -- 请求 ID, 会话 ID, 所需消息条数
                std::string requestId = request->request_id();
                std::string chatSessionId = request->chat_session_id();
                int msgCount = request->msg_count();

                // 从数据库进行消息查询
                std::vector<std::shared_ptr<Message>> msgList = _mysql_client->SelectRencent(chatSessionId, msgCount);
                if (msgList.empty())
                {
                    response->set_success(true);
                    return;
                }

                // 统计文件类型消息的文件 ID
                std::unordered_set<std::string> fileIds;
                for (const auto &msg : msgList)
                {
                    if (msg->FileID().empty() == false)
                    {
                        fileIds.insert(msg->FileID());
                    }
                }

                // 从文件子服务进行批量文件下载
                std::optional<std::unordered_map<std::string, std::string>> fileDatas_opt =
                    getFiles(requestId, fileIds);
                if (fileDatas_opt.has_value() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "get mul file from file server error!");
                    error("get mul file from file server error!");
                    return;
                }
                std::unordered_map<std::string, std::string> fileDatas = std::move(fileDatas_opt.value());

                // 统计所有消息发送者用户 ID, 从用户子服务进行批量用户信息获取
                std::unordered_set<std::string> userIds;
                for (const auto &msg : msgList)
                {
                    userIds.insert(msg->UserID());
                }
                std::optional<std::unordered_map<std::string, base::UserInfo>> userDatas_opt =
                    getUsers(requestId, userIds);
                if (userDatas_opt.has_value() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "get userinfos from user server error!");
                    error("get userinfos from user server error!");
                    return;
                }
                std::unordered_map<std::string, base::UserInfo> userDatas = std::move(userDatas_opt.value());

                // 组织响应
                for (const auto &msg : msgList)
                {
                    auto msgInfo = response->add_msg_list();
                    msgInfo->set_message_id(msg->MessageID());
                    msgInfo->set_chat_session_id(msg->ChatSessionId());
                    msgInfo->set_timestamp(boost::posix_time::to_time_t(msg->GenerateTime()));
                    msgInfo->mutable_sender()->CopyFrom(userDatas[msg->UserID()]);
                    switch (msg->MessageType())
                    {
                    case base::MessageType::STRING:
                        msgInfo->mutable_message()->set_message_type(base::MessageType::STRING);
                        msgInfo->mutable_message()->mutable_string_message()->set_content(msg->Content());
                        break;
                    case base::MessageType::IMAGE:
                        msgInfo->mutable_message()->set_message_type(base::MessageType::IMAGE);
                        msgInfo->mutable_message()->mutable_image_message()->set_file_id(msg->FileID());
                        msgInfo->mutable_message()->mutable_image_message()->set_image_content(fileDatas[msg->FileID()]);
                        break;
                    case base::MessageType::FILE:
                        msgInfo->mutable_message()->set_message_type(base::MessageType::FILE);
                        msgInfo->mutable_message()->mutable_file_message()->set_file_name(msg->FileName());
                        msgInfo->mutable_message()->mutable_file_message()->set_file_id(msg->FileID());
                        msgInfo->mutable_message()->mutable_file_message()->set_file_size(msg->FileSize());
                        msgInfo->mutable_message()->mutable_file_message()->set_file_contents(fileDatas[msg->FileID()]);
                        break;
                    case base::MessageType::SPEECH:
                        msgInfo->mutable_message()->set_message_type(base::MessageType::SPEECH);
                        msgInfo->mutable_message()->mutable_speech_message()->set_file_id(msg->FileID());
                        msgInfo->mutable_message()->mutable_speech_message()->set_file_contents(fileDatas[msg->FileID()]);
                        break;
                    default:
                        Log::lg("error", Log::FileName(), Log::Line(), "message type error!");
                        error("message type error!");
                        return;
                    }
                }
                response->set_success(true);
            }

            // 查询消息
            virtual auto MsgSearch(::google::protobuf::RpcController *controller,
                                   const ::stellar_post::message::MsgSearchReq *request,
                                   ::stellar_post::message::MsgSearchRsp *response,
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

                // 提取关键要素 -- 请求 ID, 会话 ID, 搜索关键字
                std::string requestId = request->request_id();
                std::string chatSessionId = request->chat_session_id();
                std::string searchKey = request->search_key();

                // 从 elasticsearch 进行关键字消息查询 -- 只针对文本消息
                std::vector<std::shared_ptr<Message>> msgList = _es_client->Search(searchKey, chatSessionId);
                if (msgList.empty())
                {
                    response->set_success(true);
                    return;
                }

                // 组织所有消息的用户 ID, 从用户子服务进行批量用户信息获取
                std::unordered_set<std::string> userIds;
                for (const auto &msg : msgList)
                {
                    userIds.insert(msg->UserID());
                }
                std::optional<std::unordered_map<std::string, base::UserInfo>> userDatas_opt =
                    getUsers(requestId, userIds);
                if (userDatas_opt.has_value() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "get userinfos from user server error!");
                    error("get userinfos from user server error!");
                    return;
                }
                std::unordered_map<std::string, base::UserInfo> userDatas = std::move(userDatas_opt.value());

                // 组织响应
                for (const auto &msg : msgList)
                {
                    auto msgInfo = response->add_msg_list();
                    msgInfo->set_message_id(msg->MessageID());
                    msgInfo->set_chat_session_id(msg->ChatSessionId());
                    msgInfo->set_timestamp(boost::posix_time::to_time_t(msg->GenerateTime()));
                    msgInfo->mutable_sender()->CopyFrom(userDatas[msg->UserID()]);
                    msgInfo->mutable_message()->mutable_string_message()->set_content(msg->Content());
                }
                response->set_success(true);
            }

        public:
            // 对 RabbitMQ 中的消息进行存储
            auto onMessage(const char *body, size_t sz) -> void
            {
                // 取出序列化的消息内容, 反序列化, 得到消息结构
                base::MessageInfo info;
                if (!info.ParseFromArray(body, sz))
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "consume rabbitmq message error: parse array error");
                    return;
                }

                std::optional<std::string> ret;
                std::string content;
                std::string fileId;
                std::string fileName;
                int64_t fileSize;
                // 根据不同的消息类型进行不同处理
                switch (info.message().message_type())
                {
                // 文本类型消息, 取元信息存储到 elasticsearch
                case base::MessageType::STRING:
                    content = info.message().string_message().content();
                    if (!_es_client->Add(info.sender().user_id(), info.message_id(), info.chat_session_id(),
                                         content, info.timestamp()))
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "consume rabbitmq message error: add  message to elasticsearch error");
                        return;
                    }
                    break;
                // 图片/语音/文件类型消息, 取出数据存储到文件子服务中, 并获取文件 ID
                case base::MessageType::IMAGE:
                    ret = putFile("", info.message().image_message().image_content(),
                                  info.message().image_message().image_content().size());
                    if (!ret.has_value())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "consume rabbitmq message error: put 【image】 single file error");
                        return;
                    }
                    fileId = std::move(*ret);
                    break;
                case base::MessageType::FILE:
                    fileName = info.message().file_message().file_name();
                    fileSize = info.message().file_message().file_size();
                    ret = putFile(fileName,
                                  info.message().file_message().file_contents(),
                                  fileSize);
                    if (!ret.has_value())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "consume rabbitmq message error: put 【file】 single file error");
                        return;
                    }
                    fileId = std::move(*ret);
                    break;
                case base::MessageType::SPEECH:
                    ret = putFile("", info.message().speech_message().file_contents(),
                                  info.message().speech_message().file_contents().size());
                    if (!ret.has_value())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "consume rabbitmq message error: put 【speech】 single file error");
                        return;
                    }
                    fileId = std::move(*ret);
                    break;
                default:
                    Log::lg("error", Log::FileName(), Log::Line(), "message type error!");
                    return;
                }

                // 提取消息的元信息存储到 mysql 数据库
                std::shared_ptr<Message> msg =
                    std::make_shared<Message>(info.message_id(), info.chat_session_id(), info.sender().user_id(),
                                              info.message().message_type(),
                                              boost::posix_time::from_time_t(info.timestamp()));
                switch (info.message().message_type())
                {
                case base::MessageType::STRING:
                    msg->Content(std::move(content));
                    break;
                case base::MessageType::IMAGE:
                    msg->FileID(std::move(fileId));
                    break;
                case base::MessageType::FILE:
                    msg->FileName(std::move(fileName));
                    msg->FileSize(fileSize);
                    msg->FileID(std::move(fileId));
                    break;
                case base::MessageType::SPEECH:
                    msg->FileID(std::move(fileId));
                    break;
                }
                if (!_mysql_client->Add(msg))
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "consume rabbitmq message error: add message to mysql error");
                    return;
                }
            }

        private:
            // 上传文件数据到文件子服务, 得到文件 ID
            auto putFile(const std::string &filename, const std::string &body, const int64_t fsize)
                -> std::optional<std::string>
            {
                std::string fileId;
                std::optional<stellar_post::Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_file_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "file server channel not available!");
                    return std::nullopt;
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                file::FileService_Stub stub(channel.get());
                file::PutSingleFileReq req;
                file::PutSingleFileRsp rsp;
                req.mutable_file_data()->set_file_name(filename);
                req.mutable_file_data()->set_file_size(fsize);
                req.mutable_file_data()->set_file_content(body);
                std::shared_ptr<brpc::Controller> cntl = std::make_shared<brpc::Controller>();
                stub.PutSingleFile(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "file server put single file error: {}", cntl->ErrorText());
                    return std::nullopt;
                }
                return std::move(rsp.file_info().file_id());
            }

            // 根据文件 ID 列表从文件子服务下载多个文件数据
            auto getFiles(const std::string &requestId, const std::unordered_set<std::string> &fileIdList)
                -> std::optional<std::unordered_map<std::string, std::string>>
            {
                std::unordered_map<std::string, std::string> items;
                std::optional<stellar_post::Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_file_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "file server channel not available!");
                    return std::nullopt;
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                file::FileService_Stub stub(channel.get());
                file::GetMultiFileReq req;
                req.set_request_id(requestId);
                for (const auto &id : fileIdList)
                {
                    req.add_file_id_list(id);
                }
                file::GetMultiFileRsp rsp;
                std::shared_ptr<brpc::Controller> cntl = std::make_shared<brpc::Controller>();
                stub.GetMultiFile(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "file server get single file error: {}", cntl->ErrorText());
                    return std::nullopt;
                }
                for (auto &e : rsp.file_data())
                {
                    items[std::move(e.first)] = std::move(e.second.file_content());
                }
                return items;
            }

            // 根据用户 ID 从用户子服务获取用户信息
            auto getUsers(const std::string &requestId, const std::unordered_set<std::string> &userIdList)
                -> std::optional<std::unordered_map<std::string, base::UserInfo>>
            {
                std::unordered_map<std::string, base::UserInfo> items;
                std::optional<stellar_post::Brpc::Channel::brpcChannelPtr> channel_opt =
                    _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "user server channel not available!");
                    return std::nullopt;
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                user::UserService_Stub stub(channel.get());
                user::GetMultiUserInfoReq req;
                req.set_request_id(requestId);
                for (const auto &id : userIdList)
                {
                    req.add_users_id(id);
                }
                user::GetMultiUserInfoRsp rsp;
                std::shared_ptr<brpc::Controller> cntl = std::make_shared<brpc::Controller>();
                stub.GetMultiUserInfo(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "user server get userinfo error: {}", cntl->ErrorText());
                    return std::nullopt;
                }
                for (auto &e : rsp.users_info())
                {
                    items[std::move(e.first)] = std::move(e.second);
                }
                return items;
            }

        private:
            std::string _file_service_name; // 文件子服务名称, 便于 channel 连接文件子服务信道并发起 rpc 调用
            std::string _user_service_name; // 用户子服务名称, 便于 channel 连接用户子服务信道并发起 rpc 调用

            // std::shared_ptr<elasticlient::Client> _es_client; // elasticsearch 客户端操作句柄 -- 先
            ESMessage::esmPtr _es_client; // elasticsearch message 数据操作句柄 -- 后

            // std::shared_ptr<odb::core::database> _mysql_client; // mysql 数据库操作句柄 -- 先
            MessageTable::msgPtr _mysql_client; // message ODB 数据库操作句柄 -- 后

            // rpc 调用客户端对象
            std::shared_ptr<Brpc::ChannelManager> _channel_manager; // 服务管理器句柄
        };

        class MessageServer
        {
        public:
            using messagePtr = std::shared_ptr<MessageServer>;

        public:
            MessageServer(const Etcd::Discovery::discoveryPtr &serviceDiscovery,
                          const Etcd::Registry::registryPtr &regClient,
                          const std::shared_ptr<brpc::Server> &rpcServer,
                          const std::shared_ptr<elasticlient::Client> &esClient,
                          const std::shared_ptr<odb::core::database> &mysqlClient,
                          const mqclient::RabbitMQ::mqClientPtr &mq_client)
                : _service_discovery(serviceDiscovery),
                  _reg_client(regClient),
                  _rpc_server(rpcServer),
                  _es_client(esClient),
                  _mysql_client(mysqlClient),
                  _mq_client(mq_client)
            {
            }
            ~MessageServer() = default;

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

            mqclient::RabbitMQ::mqClientPtr _mq_client; // RabbitMQ 消息队列客户端句柄
        };

        // 建造者模式, 掌控 MessageServer 的构造过程
        class MessageServerBuilder
        {
        public:
            // 构造 elasticsearch 客户端对象
            auto MakeES(const std::vector<std::string> &hostList) -> void
            {
                _es_client = Operator::ESClientFactory::Create(hostList);
            }

            // 构造 mysql 客户端对象
            auto MakeMySQL(const std::string &message,
                           const std::string &pswd,
                           const std::string &host,
                           const std::string &db,
                           const std::string &cset,
                           int port,
                           int connPoolCount) -> void
            {
                _mysql_client = Operator::ODBFactory::Create(message, pswd, host, db, cset, port, connPoolCount);
            }

            // 构造服务发现 + channel 管理客户端对象
            auto MakeDiscovery(const std::string &regHost,
                               const std::string &baseServiceName,
                               const std::string &fileServiceName,
                               const std::string &userServiceName) -> void
            {
                _file_service_name = fileServiceName;
                _user_service_name = userServiceName;
                _channel_manager = std::make_shared<Brpc::ChannelManager>();
                _channel_manager->Declared(_file_service_name); // 声明关注的服务名称
                _channel_manager->Declared(_user_service_name); // 声明关注的服务名称
                auto putCb = std::bind(&Brpc::ChannelManager::Online, _channel_manager.get(), std::placeholders::_1, std::placeholders::_2);
                auto delCb = std::bind(&Brpc::ChannelManager::Offline, _channel_manager.get(), std::placeholders::_1, std::placeholders::_2);
                _service_discovery = std::make_shared<Etcd::Discovery>(regHost, baseServiceName, putCb, delCb);
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
                _queue_name = queueName;
                _mq_client = std::make_shared<mqclient::RabbitMQ>(user, passwd, host);
                _mq_client->DeclareComponents(exchangeName, queueName, bindingKey);
            }

            // 构造服务注册客户端对象
            auto MakeRegistry(const std::string &regHost,
                              const std::string &serviceName,
                              const std::string &accessHost) -> void
            {
                _reg_client = std::make_shared<Etcd::Registry>(regHost);
                _reg_client->registry(serviceName, accessHost);
            }

            // 构造 rpc 服务器
            auto MakeRPC(uint16_t port, int32_t timeOut = -1, uint8_t numThreads = 4) -> void
            {
                if (!_es_client || !_mysql_client || !_channel_manager || _file_service_name.empty() || _user_service_name.empty())
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(), "basic components not fully initialized");
                    return;
                }

                _rpc_server = std::make_shared<brpc::Server>();

                // 向服务器对象中新增服务
                MessageServiceImpl *messageServiceImpl = new MessageServiceImpl(_es_client,
                                                                                _mysql_client,
                                                                                _channel_manager,
                                                                                _file_service_name,
                                                                                _user_service_name);
                int ret = _rpc_server->AddService(messageServiceImpl,
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

                // 进行队列消息订阅
                _mq_client->Consume(_queue_name, [&](const char *body, size_t sz)
                                    { messageServiceImpl->onMessage(body, sz); });
            }

            auto MakeDaemon(bool use = true, const std::string &workDir = "") -> void
            {
                _use_daemon = use;
                _work_dir = workDir;
            }

            auto Build() -> MessageServer::messagePtr
            {
                if (!_es_client || !_mysql_client || !_channel_manager || !_service_discovery ||
                    !_reg_client || !_mq_client || !_rpc_server || _file_service_name.empty() ||
                    _exchange_name.empty() || _queue_name.empty() || _user_service_name.empty())
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(), "MessageServerBuilder not fully initialized");
                    return nullptr;
                }

                if (_use_daemon)
                {
                    Daemon::Daemon(_work_dir);
                }

                return std::make_shared<MessageServer>(_service_discovery,
                                                       _reg_client,
                                                       _rpc_server,
                                                       _es_client,
                                                       _mysql_client,
                                                       _mq_client);
            }

        private:
            Etcd::Registry::registryPtr _reg_client;

            std::shared_ptr<odb::core::database> _mysql_client; // mysql 数据库操作句柄
            std::shared_ptr<elasticlient::Client> _es_client;   // elasticsearch 客户端操作句柄

            std::string _file_service_name;                         // 文件子服务名称, 便于 channel 连接文件子服务信道并发起 rpc 调用
            std::shared_ptr<Brpc::ChannelManager> _channel_manager; // 服务管理器句柄
            Etcd::Discovery::discoveryPtr _service_discovery;       // 服务注册发现句柄

            std::string _user_service_name; // 用户子服务名称, 便于 channel 连接用户子服务信道并发起 rpc 调用

            std::shared_ptr<brpc::Server> _rpc_server;

            mqclient::RabbitMQ::mqClientPtr _mq_client; // 消息队列客户端句柄, 用于消息发布
            std::string _exchange_name;                 // RabbitMQ 交换机名称
            std::string _queue_name;                    // RabbitMQ 队列名称

            bool _use_daemon;      // 是否启用守护进程
            std::string _work_dir; // 守护进程工作目录
        };
    }
}