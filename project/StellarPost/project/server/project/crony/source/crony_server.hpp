/*
    实现 crony 微服务
*/

#pragma once

// C
#include <cctype>
// C++
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <functional>
#include <algorithm>
#include <ranges>
#include <string_view>
#include <optional>
#include <utility>
// Other
#include "../../common/etcd/etcd.hpp"                    // 服务注册 etcd 模块封装
#include "../../common/brpc/brpc.hpp"                    // brpc 模块封装
#include "../../common/log/logger.hpp"                   // 日志器 spdlog 模块封装
#include "../../common/utils/utils.hpp"                  // 工具函数(生成唯一 uid)模块封装
#include "../../common/daemon/daemon.hpp"                // 守护进程模块封装
#include "../../common/elastic/elastic.hpp"              // elasticsearch 模块封装
#include "../../common/operator/user_elastic.hpp"        // elasticsearch 数据管理客户端模块封装
#include "../../common/operator/operator.hpp"            // mysql 模块封装
#include "../../common/operator/chat_session_member.hpp" // mysql 模块封装
#include "../operator/chat_session_mysql.hpp"            // mysql 模块封装
#include "../operator/crony_apply_mysql.hpp"             // mysql 模块封装
#include "../operator/crony_relation_mysql.hpp"          // mysql 模块封装
#include "base.pb.h"                                     // protobuf 框架代码
#include "crony.pb.h"                                    // protobuf 框架代码
#include "user.pb.h"                                     // protobuf 框架代码
#include "message.pb.h"                                  // protobuf 框架代码
#include "../../common/odb/chat_session_member.hxx"      // odb 数据库操作封装
#include "../odb/chat_session.hxx"                       // odb 数据库操作封装
#include "../odb/crony_apply.hxx"                        // odb 数据库操作封装
#include "../odb/crony_relation.hxx"                     // odb 数据库操作封装
#include "../../common/odb/user.hxx"                     // odb 数据库操作封装
#include "chat_session-odb.hxx"                          // odb 数据库操作封装
#include "crony_apply-odb.hxx"                           // odb 数据库操作封装
#include "crony_relation-odb.hxx"                        // odb 数据库操作封装
#include "chat_session_member-odb.hxx"                   // odb 数据库操作封装
#include "user-odb.hxx"                                  // odb 数据库操作封装

namespace stellar_post
{
    namespace crony
    {
        class CronyServiceImpl : public CronyService
        {
        public:
            CronyServiceImpl() = delete;
            CronyServiceImpl(const std::shared_ptr<elasticlient::Client> &es_client,
                             const std::shared_ptr<odb::core::database> &mysql_client,
                             const std::shared_ptr<Brpc::ChannelManager> &channel_manager,
                             const std::string &user_service_name,
                             const std::string &message_service_name)
                : _es_user(std::make_shared<user::ESUser>(es_client)),
                  _chat_session_table(std::make_shared<ChatSessionTable>(mysql_client)),
                  _crony_apply_table(std::make_shared<CronyApplyTable>(mysql_client)),
                  _crony_relation_table(std::make_shared<CronyRelationTable>(mysql_client)),
                  _chat_session_member_table(std::make_shared<ChatSessionMemberTable>(mysql_client)),
                  _channel_manager(channel_manager),
                  _user_service_name(user_service_name),
                  _message_service_name(message_service_name)
            {
            }

            ~CronyServiceImpl() = default;

            // 好友列表获取
            virtual auto GetCronyList(::google::protobuf::RpcController *controller,
                                      const ::stellar_post::crony::GetCronyListReq *request,
                                      ::stellar_post::crony::GetCronyListRsp *response,
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

                // 提取请求中的关键要素: 用户 ID
                std::string requestId = request->request_id();
                std::string userId = request->user_id();

                // 从数据库获取用户的好友 ID
                std::unordered_set<std::string> cronyIdList = _crony_relation_table->CronyIds(userId);
                if (!cronyIdList.empty())
                {
                    std::optional<std::unordered_map<std::string, base::UserInfo>> res_opt =
                        getUserInfo(requestId, cronyIdList);
                    if (!res_opt.has_value())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(), "get user info error!");
                        error("获取好友信息失败!");
                        return;
                    }

                    std::unordered_map<std::string, base::UserInfo> res = res_opt.value();
                    // 组织响应, 设置状态码和消息
                    for (auto &userInfo : res)
                    {
                        auto user = response->add_crony_list();
                        user->CopyFrom(userInfo.second);
                    }
                }
                response->set_success(true);
            }

            // 好友删除
            virtual auto CronyRemove(::google::protobuf::RpcController *controller,
                                     const ::stellar_post::crony::CronyRemoveReq *request,
                                     ::stellar_post::crony::CronyRemoveRsp *response,
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

                // 提取请求中的关键要素: 用户 ID, 要删除的好友 ID
                std::string requestId = request->request_id();
                std::string userId = request->user_id();
                std::string peerId = request->peer_id();

                // 从好友关系表中删除好友关系信息
                if (!_crony_relation_table->Remove(userId, peerId))
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "crony relation table remove error: {}-{}", userId, peerId);
                    error("删除好友关系失败!");
                    return;
                }

                // 从会话信息表删除聊天会话, 删除会话成员表中的成员信息
                if (!_chat_session_table->Remove(userId, peerId))
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "chat session table remove error: {}-{}", userId, peerId);
                    error("删除好友会话关系失败!");
                    return;
                }

                // 组织响应, 设置状态码和消息
                response->set_success(true);
            }

            // 添加好友 -- 发送好友申请
            virtual auto CronyAdd(::google::protobuf::RpcController *controller,
                                  const ::stellar_post::crony::CronyAddReq *request,
                                  ::stellar_post::crony::CronyAddRsp *response,
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

                // 提取请求中的关键要素: 申请人用户 ID, 被申请用户 ID
                std::string requestId = request->request_id();
                std::string userId = request->user_id();
                std::string peerId = request->respondent_id();

                // 检查好友关系是否已存在
                if (_crony_relation_table->Exists(userId, peerId))
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "crony relation table exists error: {}-{}", userId, peerId);
                    error("好友关系已存在!");
                    return;
                }

                // 判断是否申请过好友
                if (_crony_apply_table->Exists(userId, peerId))
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "crony apply table exists error: {}-{}", userId, peerId);
                    error("好友申请已存在!");
                    return;
                }

                // 向好友关系申请表中添加申请好友关系事件信息
                std::string eventId = Utils::Uuid();
                if (!_crony_apply_table->Add(std::make_shared<CronyApply>(eventId, userId, peerId)))
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "crony apply table add error: {}-{}", userId, peerId);
                    error("添加好友申请事件失败!");
                    return;
                }

                // 组织响应, 设置状态码和消息
                response->set_success(true);
                response->set_notify_event_id(std::move(eventId));

                // 增加逻辑: 如果对方申请过我好友, 则直接添加好友关系
            }

            // 处理好友申请
            virtual auto CronyAddProcess(::google::protobuf::RpcController *controller,
                                         const ::stellar_post::crony::CronyAddProcessReq *request,
                                         ::stellar_post::crony::CronyAddProcessRsp *response,
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

                // 提取请求中的关键要素: 申请人用户 ID, 被申请用户 ID, 申请结果, 事件 ID
                std::string requestId = request->request_id();
                std::string eventId = request->notify_event_id();
                std::string userId = request->user_id();       // 被申请人
                std::string peerId = request->apply_user_id(); // 申请人
                bool agree = request->agree();

                // 判断该申请事件存在性
                if (!_crony_apply_table->Exists(peerId, userId))
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "crony apply table exists error: {}-{}", peerId, userId);
                    error("好友申请未存在!");
                    return;
                }

                // 处理申请事件 -- 删除申请事件
                if (!_crony_apply_table->Remove(peerId, userId))
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "crony apply table remove error: {}-{}", peerId, userId);
                    error("删除好友申请事件失败!");
                    return;
                }
                if (_crony_apply_table->Exists(userId, peerId))
                {
                    if (!_crony_apply_table->Remove(userId, peerId))
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "crony apply table remove error: {}-{}", userId, peerId);
                        error("删除好友申请事件失败!");
                        return;
                    }
                }

                // 同意 -- 新增好友关系 + 创建单聊会话(新增单聊会话信息 + 会话成员)
                std::string chatSessionId;
                if (agree)
                {
                    // 新增好友关系
                    if (!_crony_relation_table->Add(userId, peerId))
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "crony relation table add error: {}-{}", userId, peerId);
                        error("添加好友关系失败!");
                        return;
                    }
                    // 创建单聊会话
                    chatSessionId = Utils::Uuid();
                    if (!_chat_session_table->Add(std::make_shared<ChatSession>(chatSessionId, "", ChatType::SINGLE)))
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "chat session table add error: {}-{}", userId, peerId);
                        error("创建会话失败!");
                        return;
                    }
                    // 新增会话成员
                    if (!_chat_session_member_table->Add({std::make_shared<ChatSessionMember>(userId, chatSessionId),
                                                          std::make_shared<ChatSessionMember>(peerId, chatSessionId)}))
                    {
                        Log::lg("error", Log::FileName(), Log::Line(),
                                "chat session member table add error: {}-{}", userId, peerId);
                        error("添加会话成员失败!");
                        return;
                    }
                }

                // 拒绝 -- 不用额外处理

                // 组织响应, 设置状态码和消息
                response->set_success(true);
                if (!chatSessionId.empty())
                {
                    response->set_new_session_id(chatSessionId);
                }
            }

            // 用户信息搜索 -> 添加
            virtual auto CronySearch(::google::protobuf::RpcController *controller,
                                     const ::stellar_post::crony::CronySearchReq *request,
                                     ::stellar_post::crony::CronySearchRsp *response,
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

                // 提取请求中的关键要素: 搜索关键字(可能是用户 ID, 手机号, 昵称一部分)
                std::string requestId = request->request_id();
                std::string searchKey = request->search_key();
                std::string userId = request->user_id();

                // 根据用户 ID 获取用户的好友 ID 列表
                std::unordered_set<std::string> cronyIdList = _crony_relation_table->CronyIds(userId);
                std::vector<std::string> cronyIdListVec(cronyIdList.begin(), cronyIdList.end());
                cronyIdListVec.push_back(userId);

                // 从 elasticsearch 搜索引擎进行用户信息搜索
                std::vector<std::shared_ptr<user::User>> users = _es_user->Search(searchKey, cronyIdListVec);
                // auto users = _es_user->Search(searchKey, cronyIdListVec);
                if (!users.empty())
                {
                    std::unordered_set<std::string> userIdList;
                    for (auto &user : users)
                    {
                        userIdList.insert(user->UserID());
                    }

                    // 根据获取到的用户 ID, 从用户子服务进行批量用户信息获取
                    std::optional<std::unordered_map<std::string, base::UserInfo>> res_opt =
                        getUserInfo(requestId, userIdList);
                    if (!res_opt.has_value())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(), "get user info error!");
                        error("获取好友信息失败!");
                        return;
                    }

                    std::unordered_map<std::string, base::UserInfo> res = res_opt.value();
                    // 组织响应, 设置状态码和消息
                    for (auto &userInfo : res)
                    {
                        auto user = response->add_user_info();
                        user->CopyFrom(userInfo.second);
                    }
                }
                response->set_success(true);
            }

            // 获取待处理申请好友的信息列表
            virtual auto GetPendingCronyEventList(::google::protobuf::RpcController *controller,
                                                  const ::stellar_post::crony::GetPendingCronyEventListReq *request,
                                                  ::stellar_post::crony::GetPendingCronyEventListRsp *response,
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

                // 提取请求中的关键要素: 用户 ID
                std::string requestId = request->request_id();
                std::string userId = request->user_id();

                // 获取待处理的申请事件信息 -- 被申请人申请事务用户 ID 列表
                std::unordered_set<std::string> userIdList = _crony_apply_table->ApplyUserList(userId);

                // 批量获取申请人用户信息
                if (!userIdList.empty())
                {
                    std::optional<std::unordered_map<std::string, base::UserInfo>> res_opt =
                        getUserInfo(requestId, userIdList);
                    if (!res_opt.has_value())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(), "get user info error!");
                        error("获取好友信息失败!");
                        return;
                    }

                    std::unordered_map<std::string, base::UserInfo> res = res_opt.value();
                    // 组织响应, 设置状态码和消息
                    for (auto &userInfo : res)
                    {
                        auto ev = response->add_event();
                        ev->mutable_sender()->CopyFrom(userInfo.second);
                    }
                }
                response->set_success(true);
            }

            // 会话列表获取 -- 用户登陆成功后展示其历史聊天信息
            virtual auto GetChatSessionList(::google::protobuf::RpcController *controller,
                                            const ::stellar_post::crony::GetChatSessionListReq *request,
                                            ::stellar_post::crony::GetChatSessionListRsp *response,
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

                // 提取请求中的关键要素: 用户 ID
                std::string requestId = request->request_id();
                std::string userId = request->user_id();

                // 从数据库查询用户单聊会话列表
                std::optional<std::vector<std::shared_ptr<SingleChatSessionView>>> singleCrontList_opt =
                    _chat_session_table->SelectSingleChatSession(userId);
                if (!singleCrontList_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "get single chat session error!");
                    error("获取好友信息失败!");
                    return;
                }
                std::vector<std::shared_ptr<SingleChatSessionView>> singleCrontList = singleCrontList_opt.value();

                // 从单聊会话列表中取出所有好友用户 ID
                std::unordered_set<std::string> userIdList;
                for (auto &session : singleCrontList)
                {
                    userIdList.insert(session->_crony_id);
                }

                if (!userIdList.empty())
                {
                    // 从用户子服务获取用户信息
                    std::optional<std::unordered_map<std::string, base::UserInfo>> userInfos_opt =
                        getUserInfo(requestId, userIdList);
                    if (!userInfos_opt.has_value())
                    {
                        Log::lg("error", Log::FileName(), Log::Line(), "get user info error!");
                        error("获取好友信息失败!");
                        return;
                    }
                    std::unordered_map<std::string, base::UserInfo> userInfos = userInfos_opt.value();
                    // 设置响应会话信息
                    // 单聊 -- 会话名称-好友信息, 会话头像-好友头像
                    for (auto &session : singleCrontList)
                    {
                        auto chatSession = response->add_chat_session_info_list();
                        chatSession->set_single_chat_crony_id(session->_crony_id);
                        chatSession->set_chat_session_id(session->_chat_session_id);
                        chatSession->set_chat_session_name(userInfos[session->_crony_id].nickname());
                        chatSession->set_avatar(userInfos[session->_crony_id].avatar());
                        // 根据所有会话 ID, 从消息存储子服务获取会话最后一条消息
                        std::optional<base::MessageInfo> msgInfo_opt = getLastMessage(requestId, session->_chat_session_id);
                        if (!msgInfo_opt.has_value())
                        {
                            continue;
                        }
                        chatSession->mutable_prev_message()->CopyFrom(msgInfo_opt.value());
                    }
                }

                // 从数据库查询用户群聊会话列表
                std::optional<std::vector<std::shared_ptr<GroupChatSessionView>>> groupChatList_opt =
                    _chat_session_table->SelectGroupChatSession(userId);
                if (!groupChatList_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "get group chat session error!");
                    error("获取好友信息失败!");
                    return;
                }
                std::vector<std::shared_ptr<GroupChatSessionView>> groupChatList = groupChatList_opt.value();
                // 设置响应会话信息
                // 群聊
                for (auto &session : groupChatList)
                {
                    auto chatSession = response->add_chat_session_info_list();
                    chatSession->set_chat_session_id(session->_chat_session_id);
                    chatSession->set_chat_session_name(session->_chat_session_name);
                    // 根据所有会话 ID, 从消息存储子服务获取会话最后一条消息
                    std::optional<base::MessageInfo> msgInfo_opt = getLastMessage(requestId, session->_chat_session_id);
                    if (!msgInfo_opt.has_value())
                    {
                        continue;
                    }
                    chatSession->mutable_prev_message()->CopyFrom(msgInfo_opt.value());
                }

                // 组织响应, 设置状态码和消息
                response->set_success(true);
            }

            // 创建群聊会话
            virtual auto ChatSessionCreate(::google::protobuf::RpcController *controller,
                                           const ::stellar_post::crony::ChatSessionCreateReq *request,
                                           ::stellar_post::crony::ChatSessionCreateRsp *response,
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

                // 提取请求中的关键要素: 会话名称, 会话成员用户 ID
                std::string requestId = request->request_id();
                std::string chatSessionName = request->chat_session_name();
                std::string chatSessionId = Utils::Uuid();

                std::vector<std::shared_ptr<ChatSessionMember>> memberList;
                for (auto &member : request->member_id_list())
                {
                    memberList.push_back(std::make_shared<ChatSessionMember>(member, chatSessionId));
                }

                // 生成会话 ID, 向数据库添加会话信息, 添加会话成员信息
                if (!_chat_session_table->Add(std::make_shared<ChatSession>(chatSessionId,
                                                                            chatSessionName,
                                                                            ChatType::GROUP)) ||
                    !_chat_session_member_table->Add(memberList))
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "add group chat session error!");
                    error("创建群聊会话失败!");
                    return;
                }

                // 组织响应, 设置状态码和消息, 组织会话信息
                response->mutable_chat_session_info()->set_chat_session_id(chatSessionId);
                response->mutable_chat_session_info()->set_chat_session_name(chatSessionName);
                response->set_success(true);
            }

            // 获取会话成员列表 -- 用户查看群聊信息进行成员信息展示
            virtual auto GetChatSessionMember(::google::protobuf::RpcController *controller,
                                              const ::stellar_post::crony::GetChatSessionMemberReq *request,
                                              ::stellar_post::crony::GetChatSessionMemberRsp *response,
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

                // 提取请求中的关键要素: 会话 ID
                std::string requestId = request->request_id();
                std::string chatSessionId = request->chat_session_id();

                // 从数据库获取会话成员 ID 列表
                std::optional<std::vector<std::string>> memberIdList_opt =
                    _chat_session_member_table->AllSessionMember(chatSessionId);
                if (!memberIdList_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "get chat session member error!");
                    error("获取会话成员列表失败!");
                    return;
                }
                std::vector<std::string> memberIdList = memberIdList_opt.value();

                // 从用户子服务批量获取用户信息
                std::unordered_set<std::string> userId(memberIdList.begin(), memberIdList.end());
                std::optional<std::unordered_map<std::string, base::UserInfo>> userInfoList_opt =
                    getUserInfo(requestId, userId);
                if (!userInfoList_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "get user info error!");
                    error("获取用户信息失败!");
                    return;
                }
                std::unordered_map<std::string, base::UserInfo> userInfoList = userInfoList_opt.value();

                // 组织响应, 设置状态码和消息
                response->set_request_id(requestId);
                for (const auto &user : userInfoList)
                {
                    auto user_info = response->add_member_info_list();
                    user_info->CopyFrom(user.second);
                }
                response->set_success(true);
            }

        private:
            // 调用用户子服务获取用户信息
            auto getUserInfo(const std::string &requestId, const std::unordered_set<std::string> &userIdList)
                -> std::optional<std::unordered_map<std::string, base::UserInfo>>
            {
                std::unordered_map<std::string, base::UserInfo> res;
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt = _channel_manager->Get(_user_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "user server channel not available!");
                    return std::nullopt;
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                user::GetMultiUserInfoReq req;
                user::GetMultiUserInfoRsp rsp;
                user::UserService_Stub stub(channel.get());
                req.set_request_id(requestId);
                for (auto &id : userIdList)
                {
                    req.add_users_id(id);
                }
                std::shared_ptr<brpc::Controller> cntl = std::make_shared<brpc::Controller>();
                stub.GetMultiUserInfo(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "user server get multi user info error: {}-{}", cntl->ErrorText(), rsp.errmsg());
                    return std::nullopt;
                }

                for (auto &userInfo : rsp.users_info())
                {
                    res[std::move(userInfo.first)] = std::move(userInfo.second);
                }
                return res;
            }

            // 调用消息存储子服务获取最近一条消息
            auto getLastMessage(const std::string &requestId, const std::string &chatSessionId)
                -> std::optional<base::MessageInfo>
            {
                std::optional<Brpc::Channel::brpcChannelPtr> channel_opt = _channel_manager->Get(_message_service_name);
                if (!channel_opt.has_value())
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "message server channel not available!");
                    return std::nullopt;
                }
                Brpc::Channel::brpcChannelPtr channel = channel_opt.value();
                message::GetRecentMsgReq req;
                message::GetRecentMsgRsp rsp;
                message::MsgStorageService_Stub stub(channel.get());
                req.set_request_id(requestId);
                req.set_chat_session_id(chatSessionId);
                req.set_msg_count(1);
                std::shared_ptr<brpc::Controller> cntl = std::make_shared<brpc::Controller>();
                stub.GetRecentMsg(cntl.get(), &req, &rsp, nullptr);
                if (cntl->Failed() || rsp.success() == false)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "message server get recent msg error: {}-{}", cntl->ErrorText(), rsp.errmsg());
                    return std::nullopt;
                }
                if (!(rsp.msg_list_size() > 0))
                {
                    return std::nullopt;
                }
                base::MessageInfo res;
                res.CopyFrom(rsp.msg_list(0));
                return res;
            }

        private:
            // std::shared_ptr<elasticlient::Client> _es_client; // elasticsearch 客户端操作句柄
            user::ESUser::esUserPtr _es_user; // elasticsearch user 数据操作句柄 -- 后

            // std::shared_ptr<odb::core::database> _mysql_client; // mysql 数据库操作句柄
            ChatSessionTable::cstPtr _chat_session_table;               // chat session 数据库操作句柄
            CronyApplyTable::catPtr _crony_apply_table;                 // crony apply 数据库操作句柄
            CronyRelationTable::crtPtr _crony_relation_table;           // crony relation 数据库操作句柄
            ChatSessionMemberTable::csmtPtr _chat_session_member_table; // chat session member 数据库操作句柄

            // rpc 调用客户端对象
            std::shared_ptr<Brpc::ChannelManager> _channel_manager; // 服务管理器句柄
            std::string _user_service_name;                         // user 子服务名称
            std::string _message_service_name;                      // message 子服务名称
        };

        class CronyServer
        {
        public:
            using cronyPtr = std::shared_ptr<CronyServer>;

        public:
            CronyServer(const Etcd::Discovery::discoveryPtr &service_discovery,
                        const Etcd::Registry::registryPtr &registry_client,
                        const std::shared_ptr<brpc::Server> &rpc_server,
                        const std::shared_ptr<elasticlient::Client> &es_client,
                        const std::shared_ptr<odb::core::database> &mysql_client)
                : _service_discovery(service_discovery),
                  _registry_client(registry_client),
                  _rpc_server(rpc_server),
                  _es_client(es_client),
                  _mysql_client(mysql_client)
            {
            }
            ~CronyServer() = default;

            // 启动 rpc 服务器
            auto Run() const -> void
            {
                _rpc_server->RunUntilAskedToQuit();
            }

        private:
            Etcd::Discovery::discoveryPtr _service_discovery; // 服务发现句柄
            Etcd::Registry::registryPtr _registry_client;     // 服务注册句柄
            std::shared_ptr<brpc::Server> _rpc_server;        // 服务器搭建

            std::shared_ptr<elasticlient::Client> _es_client; // elasticsearch 客户端操作句柄

            std::shared_ptr<odb::core::database> _mysql_client; // mysql 数据库操作句柄
        };

        // 建造者模式, 掌控 CronyServer 的构造过程
        class CronyServerBuilder
        {
        public:
            // 构造 elasticsearch 客户端对象
            auto MakeES(const std::vector<std::string> &hostList) -> void
            {
                _es_client = Operator::ESClientFactory::Create(hostList);
            }

            // 构造 mysql 客户端对象
            auto MakeMySQL(const std::string &crony,
                           const std::string &pswd,
                           const std::string &host,
                           const std::string &db,
                           const std::string &cset,
                           int port,
                           int connPoolCount) -> void
            {
                _mysql_client = Operator::ODBFactory::Create(crony, pswd, host, db, cset, port, connPoolCount);
            }

            // 构造服务发现 + channel 管理客户端对象
            auto MakeDiscovery(const std::string &regHost,
                               const std::string &baseServiceName,
                               const std::string &userServiceName,
                               const std::string &messageServiceName) -> void
            {
                _user_service_name = userServiceName;
                _message_service_name = messageServiceName;
                _channel_manager = std::make_shared<Brpc::ChannelManager>();
                _channel_manager->Declared(userServiceName);    // 声明关注的服务名称
                _channel_manager->Declared(messageServiceName); // 声明关注的服务名称
                auto putCb = std::bind(&Brpc::ChannelManager::Online, _channel_manager.get(), std::placeholders::_1, std::placeholders::_2);
                auto delCb = std::bind(&Brpc::ChannelManager::Offline, _channel_manager.get(), std::placeholders::_1, std::placeholders::_2);
                _service_discovery = std::make_shared<Etcd::Discovery>(regHost, baseServiceName, putCb, delCb);
            }

            // 构造服务注册客户端对象
            auto MakeRegistry(const std::string &regHost,
                              const std::string &serviceName,
                              const std::string &accessHost) -> void
            {
                _registry_client = std::make_shared<Etcd::Registry>(regHost);
                _registry_client->registry(serviceName, accessHost);
            }

            // 构造 rpc 服务器
            auto MakeRPC(uint16_t port, int32_t timeOut = -1, uint8_t numThreads = 4) -> void
            {
                if (!_mysql_client || !_es_client || !_channel_manager ||
                    _user_service_name.empty() || _message_service_name.empty() || !_rpc_server)
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(), "basic components not fully initialized");
                    return;
                }

                _rpc_server = std::make_shared<brpc::Server>();

                // 向服务器对象中新增服务
                CronyServiceImpl *cronyServiceImpl = new CronyServiceImpl(_es_client,
                                                                          _mysql_client,
                                                                          _channel_manager,
                                                                          _user_service_name,
                                                                          _message_service_name);
                int ret = _rpc_server->AddService(cronyServiceImpl,
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

            auto Build() -> CronyServer::cronyPtr
            {
                if (!_mysql_client || !_es_client || !_channel_manager ||
                    !_service_discovery || !_registry_client || !_rpc_server ||
                    _user_service_name.empty() || _message_service_name.empty())
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(), "CronyServerBuilder not fully initialized");
                    return nullptr;
                }

                if (_use_daemon)
                {
                    Daemon::Daemon(_work_dir);
                }

                return std::make_shared<CronyServer>(_service_discovery,
                                                     _registry_client,
                                                     _rpc_server,
                                                     _es_client,
                                                     _mysql_client);
            }

        private:
            std::shared_ptr<Brpc::ChannelManager> _channel_manager; // 服务管理器句柄
            Etcd::Discovery::discoveryPtr _service_discovery;       // 服务发现句柄
            Etcd::Registry::registryPtr _registry_client;           // 服务注册句柄

            std::shared_ptr<brpc::Server> _rpc_server; // 服务器搭建
            std::string _user_service_name;            // user 子服务名称
            std::string _message_service_name;         // message 子服务名称

            std::shared_ptr<elasticlient::Client> _es_client; // elasticsearch 客户端操作句柄

            std::shared_ptr<odb::core::database> _mysql_client; // mysql 数据库操作句柄

            bool _use_daemon;      // 是否启用守护进程
            std::string _work_dir; // 守护进程工作目录
        };
    }
}