/*
    实现入口网关子服务模块搭建
*/

// gatewayServer 模块
#include "gateway_server.hpp"
// gflags
#include <gflags/gflags.h>

// etcd
DEFINE_string(registryHost, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(fileService, "/service/file_service", "文件服务名称");
DEFINE_string(userService, "/service/user_service", "用户服务名称");
DEFINE_string(msgService, "/service/message_service", "消息存储服务名称");
DEFINE_string(transmiteService, "/service/transmite_service", "消息转发服务名称");
DEFINE_string(speechService, "/service/speech_service", "语音服务名称");
DEFINE_string(cronyService, "/service/crony_service", "好友服务名称");

// port(websocket & http)
DEFINE_int32(websocket_port, 16421, "websocket 服务默认监听端口");
DEFINE_int32(http_port, 16422, "http 服务默认监听端口");

// redis
DEFINE_string(redis_host, "127.0.0.1", "host");
DEFINE_int32(redis_port, 6379, "port");
DEFINE_int32(redis_db, 0, "库编号(默认0)");
DEFINE_bool(redis_keepAlive, true, "keepAlive");

// daemon
DEFINE_bool(use_daemon, true, "是否启用守护进程");
DEFINE_string(work_dir, "", "守护进程工作目录");

auto main(int argc, char *argv[]) -> int
{
    // 解析 gflags
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    stellar_post::gateway::GatewayServerBuilder gsb;

    gsb.MakeDiscovery(FLAGS_registryHost, FLAGS_baseService,
                      FLAGS_userService, FLAGS_fileService,
                      FLAGS_cronyService, FLAGS_speechService,
                      FLAGS_transmiteService, FLAGS_msgService);
    gsb.MakeRedis(FLAGS_redis_host, FLAGS_redis_port, FLAGS_redis_db, FLAGS_redis_keepAlive);
    gsb.MakeServer(FLAGS_websocket_port, FLAGS_http_port);
    gsb.MakeDaemon(FLAGS_use_daemon, FLAGS_work_dir); // 守护进程
    auto server = gsb.Build();
    server->Run();

    return 0;
}