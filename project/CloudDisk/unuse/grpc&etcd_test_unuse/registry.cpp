// 测试通过

// C++
#include <print>
#include <memory>
#include <thread>
#include <chrono>
// Other
#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include "../../../project/common/etcd/etcd.hpp"
#include "../../../project/common/grpc/grpc.hpp"
#include "grpc.pb.h"
#include "grpc.grpc.pb.h"

// 定义命令行参数
DEFINE_string(etcdHost, "127.0.0.1:2379", "etcd 服务地址");
DEFINE_string(accessHost, "127.0.0.1:8000", "服务的外部访问地址");
DEFINE_int32(listenPort, 8000, "gRPC 服务监听端口");
DEFINE_string(baseService, "/service", "etcd 中的服务基础路径");
DEFINE_string(instanceName, "/echo", "当前服务实例名称");

// 实现 Echo 服务接口
class EchoServiceImpl : public grpc::EchoService
{
public:
    EchoServiceImpl() {}
    ~EchoServiceImpl() {}

    // 实现 Echo 方法
    auto Echo(::google::protobuf::RpcController *controller,
              const ::grpc::EchoRequest *request,
              ::grpc::EchoResponse *response,
              ::google::protobuf::Closure *done) -> void override
    {
        // 使用智能指针确保 done 被正确调用
        grpc::ClosureGuard guard(done);

        // 处理业务逻辑
        std::println("收到请求: {}", request->message());

        // 构建响应
        std::string echoMessage = "Echo: " + request->message();
        response->set_message(echoMessage);
    }
};

auto main(int argc, char *argv[]) -> int
{
    // 解析命令行参数
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 1. 创建 gRPC 服务器对象
    grpc::ServerBuilder builder;
    builder.AddListeningPort(FLAGS_accessHost, grpc::InsecureServerCredentials());
    EchoServiceImpl echoService;
    builder.RegisterService(&echoService);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

    // 2. 创建 etcd 客户端并注册服务
    std::shared_ptr<cloud_disk::Etcd::Registry> registry =
        std::make_shared<cloud_disk::Etcd::Registry>(FLAGS_etcdHost);

    // 构建服务注册路径和服务地址
    std::string servicePath = FLAGS_baseService + FLAGS_instanceName + "/instance";
    std::string serviceHost = FLAGS_accessHost;

    // 注册服务到 etcd
    if (!registry->registry(servicePath, serviceHost))
    {
        std::cerr << "服务注册失败!" << std::endl;
        return -1;
    }

    // 3. 阻塞等待服务器停止
    server->Wait();

    return 0;
}
