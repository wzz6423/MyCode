/*
实现文件存储子服务
    1.实现文件 rpc 服务类 -- 实现 rpc 调用的业务处理接口
    2.实现文件存储子服务的服务器类
    3.实现文件存储子服务类的构造者
*/

#pragma once

// C++
#include <memory>
// Other
#include "../../common/etcd/etcd.hpp"     // 服务注册 etcd 模块封装
#include "../../common/brpc/brpc.hpp"     // brpc 模块封装
#include "../../common/log/logger.hpp"    // 日志器 spdlog 模块封装
#include "../../common/utils/utils.hpp"   // 实用工具类 utils 模块封装
#include "../../common/daemon/daemon.hpp" // 守护进程模块封装
#include "base.pb.h"                      // protobuf 框架代码
#include "file.pb.h"                      // protobuf 框架代码

namespace stellar_post
{
    namespace file
    {
        class FileServiceImpl : public FileService
        {
        public:
            FileServiceImpl(const std::string &storage_path = "./data/")
                : _storage_path(storage_path)
            {
                if (_storage_path.back() != '/')
                {
                    _storage_path += "/";
                }
                checkCreateDir(storage_path);
            }
            ~FileServiceImpl() {}

            // 单文件下载
            virtual auto GetSingleFile(google::protobuf::RpcController *controller,
                                       const GetSingleFileReq *request,
                                       GetSingleFileRsp *response,
                                       google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());

                // 1.取出请求中的文件 ID (文件名)
                std::string fid = request->file_id();
                std::string fileName = _storage_path + fid;

                // 2.将文件 ID 作为文件名, 读取文件数据
                std::string body;
                bool ret = Utils::ReadFile(fileName, &body);
                if (!ret)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "read file {} error!", fid);
                    response->set_success(false);
                    response->set_errmsg("read file error");
                    return;
                }

                // 3.组织响应
                response->set_success(true);
                response->mutable_file_data()->set_file_id(fid);
                response->mutable_file_data()->set_file_content(body);
            }
            // 多文件下载
            virtual auto GetMultiFile(google::protobuf::RpcController *controller,
                                      const GetMultiFileReq *request,
                                      GetMultiFileRsp *response,
                                      google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());

                for (size_t i = 0; i < request->file_id_list_size(); ++i)
                {
                    // 1.取出请求中的文件 ID (文件名)
                    std::string fid = request->file_id_list(i);
                    std::string fileName = _storage_path + fid;

                    // 2.将文件 ID 作为文件名, 读取文件数据
                    std::string body;
                    bool ret = Utils::ReadFile(fileName, &body);
                    if (!ret)
                    {
                        Log::lg("error", Log::FileName(), Log::Line(), "read file {} error!", fid);
                        response->set_success(false);
                        response->set_errmsg("read file error");
                        return;
                    }

                    // 3.组织响应
                    base::FileDownloadData data;
                    data.set_file_id(fid);
                    data.set_file_content(body);
                    response->mutable_file_data()->insert({fid, data});
                }

                response->set_success(true);
            }

            // 单文件上传
            virtual auto PutSingleFile(google::protobuf::RpcController *controller,
                                       const PutSingleFileReq *request,
                                       PutSingleFileRsp *response,
                                       google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());

                // 1.为文件生成唯一一个 uuid 作为文件名及文件 ID
                std::string fid = Utils::Uuid();
                std::string fileName = _storage_path + fid;

                // 2.取出请求中的文件数据, 进行文件写入
                bool ret = Utils::WriteFile(fileName, request->file_data().file_content());
                if (!ret)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "write file {} error!", fid);
                    response->set_success(false);
                    response->set_errmsg("write file error");
                    return;
                }

                // 3.组织响应
                response->set_success(true);
                response->mutable_file_info()->set_file_id(fid);
                response->mutable_file_info()->set_file_size(request->file_data().file_size());
                response->mutable_file_info()->set_file_name(request->file_data().file_name());
            }

            // 多文件上传
            virtual auto PutMultiFile(google::protobuf::RpcController *controller,
                                      const PutMultiFileReq *request,
                                      PutMultiFileRsp *response,
                                      google::protobuf::Closure *done) -> void override
            {
                brpc::ClosureGuard rpc_guard(done);
                response->set_request_id(request->request_id());

                for (size_t i = 0; i < request->file_data_size(); ++i)
                {
                    // 1.为文件生成唯一一个 uuid 作为文件名及文件 ID
                    std::string fid = Utils::Uuid();
                    std::string fileName = _storage_path + fid;

                    // 2.取出请求中的文件数据, 进行文件写入
                    bool ret = Utils::WriteFile(fileName, request->file_data(i).file_content());
                    if (!ret)
                    {
                        Log::lg("error", Log::FileName(), Log::Line(), "write file {} error!", fid);
                        response->set_success(false);
                        response->set_errmsg("write file error");
                        return;
                    }

                    // 3.组织响应
                    base::FileMessageInfo *info = response->add_file_info();
                    info->set_file_id(fid);
                    info->set_file_size(request->file_data(i).file_size());
                    info->set_file_name(request->file_data(i).file_name());
                }

                response->set_success(true);
            }

        private:
            auto checkCreateDir(const std::string &path) -> bool
            {
                try
                {
                    // 检查路径是否存在且是目录
                    if (std::filesystem::is_directory(path))
                    {
                        return true;
                    }

                    // 创建目录（包括所有父目录）
                    return std::filesystem::create_directories(path);
                }
                catch (const std::filesystem::filesystem_error &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "Filesystem error: {}", e.what());
                }
                catch (const std::exception &e)
                {
                    Log::lg("error", Log::FileName(), Log::Line(), "General error: {}", e.what());
                }
                return false;
            }

        private:
            std::string _storage_path;
        };

        class FileServer
        {
        public:
            using filePtr = std::shared_ptr<FileServer>;

        public:
            FileServer(const Etcd::Registry::registryPtr &regClient,
                       const std::shared_ptr<brpc::Server> &server)
                : _reg_client(regClient),
                  _rpc_server(server)
            {
            }
            ~FileServer() = default;

            // 启动 rpc 服务器
            auto Run() const -> void
            {
                _rpc_server->RunUntilAskedToQuit();
            }

        private:
            Etcd::Registry::registryPtr _reg_client;
            std::shared_ptr<brpc::Server> _rpc_server;
        };

        // 建造者模式, 掌控 FileServer 的构造过程
        class FileServerBuilder
        {
        public:
            // 构造服务注册客户端对象
            auto MakeRegistry(const std::string &regHost,
                              const std::string &serviceName,
                              const std::string &accessHost) -> void
            {
                _reg_client = std::make_shared<Etcd::Registry>(regHost);
                _reg_client->registry(serviceName, accessHost);
            }

            // 构造 rpc 服务器
            auto MakeRPC(uint16_t port, int32_t timeOut = -1, uint8_t numThreads = 4,
                         const std::string &path = "./data/") -> void
            {
                _rpc_server = std::make_shared<brpc::Server>();

                // 向服务器对象中新增服务
                // std::shared_ptr<FileServiceImpl> fileServiceImpl = std::make_shared<FileServiceImpl>(path);
                // int ret = _rpc_server->AddService(fileServiceImpl.get(),
                //                                   brpc::ServiceOwnership::SERVER_OWNS_SERVICE);
                // 此处不能使用智能指针+给 brpc 对生命周期的管理权、也不能收回 brpc 对生命周期的管理权然后使用智能指针
                // 均会引起 double free
                FileServiceImpl *fileServiceImpl = new FileServiceImpl(path);
                int ret = _rpc_server->AddService(fileServiceImpl,
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

            auto Build() -> FileServer::filePtr
            {
                if (!_reg_client || !_rpc_server)
                {
                    Log::lg("fatal", Log::FileName(), Log::Line(),
                            "file server build failed, 服务注册 / rpc 服务器模块未注册");
                    return nullptr;
                }

                if (_use_daemon)
                {
                    Daemon::Daemon(_work_dir);
                }

                return std::make_shared<FileServer>(_reg_client, _rpc_server);
            }

        private:
            Etcd::Registry::registryPtr _reg_client;
            std::shared_ptr<brpc::Server> _rpc_server;

            bool _use_daemon;      // 是否启用守护进程
            std::string _work_dir; // 守护进程工作目录
        };
    }
}