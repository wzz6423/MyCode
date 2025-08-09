/*
按照流程完成服务器的搭建
    1.参数解析
    2.日志初始化
    3.构造服务器对象, 启动服务器
*/

// fileServer 模块
#include "file_server.hpp"
// gflags
#include <gflags/gflags.h>

DEFINE_string(registryHost, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(instanceName, "/file_service/", "当前实例名称");
DEFINE_string(accessHost, "127.0.0.1:16424", "服务实例外部访问地址");
DEFINE_string(storagePath, "./files/", "文件保存路径");

DEFINE_int32(rpc_linstenPort, 16424, "rpc 服务默认监听端口");
DEFINE_int32(rpc_idleTimeout, -1, "rpc 空闲超时时间");
DEFINE_int32(rpc_numThreads, 9, "rpc IO 线程数量");

// daemon
DEFINE_bool(use_daemon, true, "是否启用守护进程");
DEFINE_string(work_dir, "", "守护进程工作目录");

auto main(int argc, char *argv[]) -> int
{
    // 解析 gflags
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    stellar_post::file::FileServerBuilder fsb;
    fsb.MakeRPC(FLAGS_rpc_linstenPort, FLAGS_rpc_idleTimeout, FLAGS_rpc_numThreads, FLAGS_storagePath);
    fsb.MakeRegistry(FLAGS_registryHost, FLAGS_baseService + FLAGS_instanceName + "instance", FLAGS_accessHost);
    fsb.MakeDaemon(FLAGS_use_daemon, FLAGS_work_dir); // 守护进程
    auto server = fsb.Build();
    server->Run();

    return 0;
}