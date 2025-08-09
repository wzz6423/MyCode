/*
    实现好友子服务模块搭建
*/

// cronyServer 模块
#include "crony_server.hpp"
// gflags
#include <gflags/gflags.h>

// etcd
DEFINE_string(registryHost, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(userService, "/service/user_service", "用户服务名称");
DEFINE_string(messageService, "/service/message_service", "消息服务名称");
DEFINE_string(instanceName, "/crony_service/instance", "当前实例名称");
DEFINE_string(accessHost, "127.0.0.1:16428", "服务实例外部访问地址");

// rpc
DEFINE_int32(rpc_linstenPort, 16428, "rpc 服务默认监听端口");
DEFINE_int32(rpc_idleTimeout, -1, "rpc 空闲超时时间");
DEFINE_int32(rpc_numThreads, 4, "rpc IO 线程数量");

// mysql
DEFINE_string(user, "aaaaa", "mysql user");
DEFINE_string(pswd, "aaaaa", "mysql crony's password");
DEFINE_string(host, "127.0.0.1", "host");
DEFINE_string(database, "stellar_post", "database");
DEFINE_string(cset, "utf8", "cset");
DEFINE_int32(port, 0, "port");
DEFINE_int32(connPoolCount, 4, "connPoolCount");

// elasticsearch
DEFINE_string(esHost, "http://127.0.0.1:9200/", "elastic search url");

// daemon
DEFINE_bool(use_daemon, true, "是否启用守护进程");
DEFINE_string(work_dir, "", "守护进程工作目录");

auto main(int argc, char *argv[]) -> int
{
    // 解析 gflags
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    stellar_post::crony::CronyServerBuilder csb;
    csb.MakeES({FLAGS_esHost});                                                                                     // elasticsearch 客户端
    csb.MakeMySQL(FLAGS_user, FLAGS_pswd, FLAGS_host, FLAGS_database, FLAGS_cset, FLAGS_port, FLAGS_connPoolCount); // mysql 客户端
    csb.MakeDiscovery(FLAGS_registryHost, FLAGS_baseService, FLAGS_userService, FLAGS_messageService);              // 服务发现 + channel 管理客户端
    csb.MakeRPC(FLAGS_rpc_linstenPort, FLAGS_rpc_idleTimeout, FLAGS_rpc_numThreads);                                // rpc 服务器 -- 前
    csb.MakeRegistry(FLAGS_registryHost, FLAGS_baseService + FLAGS_instanceName, FLAGS_accessHost);                 // 服务注册客户端 -- 后
    csb.MakeDaemon(FLAGS_use_daemon, FLAGS_work_dir);                                                               // 守护进程
    auto server = csb.Build();
    server->Run();

    return 0;
}