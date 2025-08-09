/*
    实现语音识别子服务模块
*/

#pragma once

// C++
#include <string>
#include <optional>
#include <memory>
// Other
#include "./asr/asr.hpp"                  // 语音识别 asr 模块封装
#include "../../common/etcd/etcd.hpp"     // 服务注册 etcd 模块封装
#include "../../common/brpc/brpc.hpp"     // brpc 模块封装
#include "../../common/log/logger.hpp"    // 日志器 spdlog 模块封装
#include "../../common/daemon/daemon.hpp" // 守护进程模块封装
#include "speech.pb.h"                    // protobuf 框架代码

namespace stellar_post
{
    namespace speech
    {
        class SpeechServiceImpl : public SpeechService
        {
        public:
            SpeechServiceImpl(const ASR::ASRClient::asrPtr &asrClient)
                : _asr_client(asrClient)
            {
            }

            ~SpeechServiceImpl() = default;

            virtual auto SpeechRecognition(google::protobuf::RpcController *controller,
                                           const SpeechRecognitionReq *request,
                                           SpeechRecognitionRsp *response,
                                           google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);

                // 1.取出请求中的语音数据
                // 2.调用语音 sdk 模块进行语音识别, 得到响应
                std::optional<std::string> result = _asr_client->Recognize(request->speech_content());
                if (!result)
                {
                    Log::lg("error", Log::FileName(), Log::Line(),
                            "speech recognition failed: {}", request->request_id());
                    response->set_request_id(request->request_id());
                    response->set_success(false);
                    response->set_errmsg("speech recognition failed");
                    return;
                }

                // 3.组织响应
                response->set_request_id(request->request_id());
                response->set_success(true);
                response->set_recognition_result(*result);
            }

        private:
            ASR::ASRClient::asrPtr _asr_client;
        };

        class SpeechServer
        {
        public:
            using speechPtr = std::shared_ptr<SpeechServer>;

        public:
            SpeechServer(const ASR::ASRClient::asrPtr &asrClient,
                         const Etcd::Registry::registryPtr &regClient,
                         const std::shared_ptr<brpc::Server> &rpcServer)
                : _asr_client(asrClient),
                  _reg_client(regClient),
                  _rpc_server(rpcServer)
            {
            }
            ~SpeechServer() = default;

            // 启动 rpc 服务器
            auto Run() const -> void
            {
                _rpc_server->RunUntilAskedToQuit();
            }

        private:
            ASR::ASRClient::asrPtr _asr_client;
            Etcd::Registry::registryPtr _reg_client;
            std::shared_ptr<brpc::Server> _rpc_server;
        };

        // 建造者模式, 掌控 SpeechServer 的构造过程
        class SpeechServerBuilder
        {
        public:
            // 构造语音识别客户端对象
            auto MakeASR(std::string &appId,
                         const std::string &apiKey,
                         const std::string &secretKey) -> void
            {
                _asr_client = std::make_shared<ASR::ASRClient>(appId, apiKey, secretKey);
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
                if (!_asr_client)
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(),
                            "speech server build failed, 语音识别模块未注册");
                    return;
                }

                _rpc_server = std::make_shared<brpc::Server>();

                // 向服务器对象中新增服务
                SpeechServiceImpl *speechServiceImpl = new SpeechServiceImpl(_asr_client);
                int ret = _rpc_server->AddService(speechServiceImpl,
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

            auto Build() -> SpeechServer::speechPtr
            {
                if (!_asr_client || !_reg_client || !_rpc_server)
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(),
                            "speech server build failed, 语音识别 / 服务注册 / rpc 服务器模块未注册");
                    return nullptr;
                }

                if (_use_daemon)
                {
                    Daemon::Daemon(_work_dir);
                }

                return std::make_shared<SpeechServer>(_asr_client, _reg_client, _rpc_server);
            }

        private:
            ASR::ASRClient::asrPtr _asr_client;
            Etcd::Registry::registryPtr _reg_client;
            std::shared_ptr<brpc::Server> _rpc_server;

            bool _use_daemon;      // 是否启用守护进程
            std::string _work_dir; // 守护进程工作目录
        };
    }
}