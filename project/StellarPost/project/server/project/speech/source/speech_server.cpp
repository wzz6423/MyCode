/*
    实现语音识别子服务模块搭建
*/

// speechServer 模块
#include "speech_server.hpp"
// gflags
#include <gflags/gflags.h>

DEFINE_string(registryHost, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(instanceName, "/speech_service/instance", "当前实例名称");
DEFINE_string(accessHost, "127.0.0.1:16423", "服务实例外部访问地址");

DEFINE_int32(rpc_linstenPort, 16423, "rpc 服务默认监听端口");
DEFINE_int32(rpc_idleTimeout, -1, "rpc 空闲超时时间");
DEFINE_int32(rpc_numThreads, 4, "rpc IO 线程数量");

DEFINE_string(appId, "your_app_id", "ASR 平台应用 ID");
DEFINE_string(apiKey, "your_api_key", "ASR 平台访问密钥 Key");
DEFINE_string(apiSecret, "your_api_secret", "ASR 平台加密密钥 Secret");

// daemon
DEFINE_bool(use_daemon, true, "是否启用守护进程");
DEFINE_string(work_dir, "", "守护进程工作目录");

auto main(int argc, char *argv[]) -> int
{
    // 解析 gflags
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    stellar_post::speech::SpeechServerBuilder ssb;
    ssb.MakeASR(FLAGS_appId, FLAGS_apiKey, FLAGS_apiSecret);
    ssb.MakeRPC(FLAGS_rpc_linstenPort, FLAGS_rpc_idleTimeout, FLAGS_rpc_numThreads);
    ssb.MakeRegistry(FLAGS_registryHost, FLAGS_baseService + FLAGS_instanceName, FLAGS_accessHost);
    ssb.MakeDaemon(FLAGS_use_daemon, FLAGS_work_dir); // 守护进程
    auto server = ssb.Build();
    server->Run();

    return 0;
}