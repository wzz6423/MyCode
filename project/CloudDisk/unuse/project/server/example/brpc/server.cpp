// C++
#include <print>
#include <iostream>
// brpc
#include <brpc/server.h>
#include <butil/logging.h>
#include "brpc.pb.h"

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
    ret = server.Start(8000, &options); // 监听端口 8000
    if (ret == -1)
    {
        std::cerr << "启动服务失败!" << std::endl;
        return -1;
    }
    server.RunUntilAskedToQuit(); // 阻塞等待请求, 直到调用 server.Stop() 或 Ctrl+C 结束

    return 0;
}