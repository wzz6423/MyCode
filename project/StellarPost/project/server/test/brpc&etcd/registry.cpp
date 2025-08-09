// 测试通过

// C++
#include <print>
#include <memory>
#include <thread>
#include <chrono>
// test etcd & brpc
#include "../../../project/common/etcd/etcd.hpp"
#include "../../../project/common/brpc/brpc.hpp"
#include "brpc.pb.h"
#include "../../../project/common/log/logger.hpp"
// gflags
#include <gflags/gflags.h>

DEFINE_bool(debug_enable, true, "debug enable, true-调试, false-发布");

DEFINE_string(etcdHost, "127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(accessHost, "127.0.0.1:8000", "服务实例外部访问地址");
DEFINE_int32(linstenPort, 8000, "rpc 服务默认监听端口");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(instanceName, "/echo", "当前实例化目录");

// 继承 EchoService 创建子类, 并实现 rpc 调用的业务功能
class EchoServiceImpl : public brpc::EchoService
{
public:
    EchoServiceImpl() {}
    ~EchoServiceImpl() {}
    virtual void Echo(google::protobuf::RpcController *controller,
                      const ::brpc::EchoRequest *request,
                      ::brpc::EchoResponse *response,
                      ::google::protobuf::Closure *done)
    {
        brpc::ClosureGuard rpcGuard(done); // 确保 done->Run() 被调用

        // 取出 request 中的内容
        // 业务处理
        std::println("收到消息: {}", request->message());

        // 向 response 中填充内容
        std::string responseMessage = "Echo: " + request->message();
        response->set_message(responseMessage);

        // 调用 done 中的 Run() 接口, 结束业务处理 -- 为了防止忘记调用 done->Run(), 使用 ClosureGuard -- 服务端智能管理对象
        // done->Run();
    }
};

auto main(int argc, char *argv[]) -> int
{
    // 关闭 brpc 默认日志输出
    logging::LoggingSettings setlog;
    setlog.logging_dest = logging::LoggingDestination::LOG_TO_NONE; // 设置日志输出目的地为 NONE, 不输出日志
    logging::InitLogging(setlog);

    static stellar_post::Log::Log &lg = stellar_post::Log::Log::GetInstance(FLAGS_debug_enable);

    // 构造服务器对象
    brpc::Server server;

    // 向服务器对象中新增 EchoServiceImpl 服务
    EchoServiceImpl echoServiceImpl;
    int ret = server.AddService(&echoServiceImpl, brpc::ServiceOwnership::SERVER_DOESNT_OWN_SERVICE); // ServiceOwnership: 枚举, 添加服务失败时如何处理
    if (ret == -1)
    {
        std::cerr << "添加服务失败!" << std::endl;
        return -1;
    }
    brpc::ServerOptions options;
    options.idle_timeout_sec = -1; // 设置空闲超时时间为 n 秒 -- 一般不设置,默认 -1 , 超时会自动关闭
    options.num_threads = 1;       // 设置 IO 线程数量 -- 一般不设置,默认 4 , 线程数量会自动根据 cpu 核心数量调整

    // 启动服务器
    ret = server.Start(FLAGS_linstenPort, &options); // 监听端口 8000
    if (ret == -1)
    {
        std::cerr << "启动服务失败!" << std::endl;
        return -1;
    }

    // 注册服务
    stellar_post::Etcd::Registry::registryPtr registry = std::make_shared<stellar_post::Etcd::Registry>(FLAGS_etcdHost);
    registry->registry(FLAGS_baseService + FLAGS_instanceName + "/instance", FLAGS_accessHost);

    server.RunUntilAskedToQuit(); // 阻塞等待请求, 直到调用 server.Stop() 或 Ctrl+C 结束

    return 0;
}