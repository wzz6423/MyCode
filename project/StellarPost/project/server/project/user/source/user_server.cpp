/*
    实现用户子服务模块搭建
*/

// userServer 模块
#include "user_server.hpp"
// gflags
#include <gflags/gflags.h>

// etcd
DEFINE_string(registryHost, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(fileService, "/service/file_service", "文件服务名称");
DEFINE_string(instanceName, "/user_service/instance", "当前实例名称");
DEFINE_string(accessHost, "127.0.0.1:16425", "服务实例外部访问地址");

// rpc
DEFINE_int32(rpc_linstenPort, 16425, "rpc 服务默认监听端口");
DEFINE_int32(rpc_idleTimeout, -1, "rpc 空闲超时时间");
DEFINE_int32(rpc_numThreads, 4, "rpc IO 线程数量");

// mysql
DEFINE_string(user, "aaaaa", "mysql user");
DEFINE_string(pswd, "aaaaa", "mysql user's password");
DEFINE_string(host, "127.0.0.1", "host");
DEFINE_string(database, "stellar_post", "database");
DEFINE_string(cset, "utf8", "cset");
DEFINE_int32(port, 0, "port");
DEFINE_int32(connPoolCount, 4, "connPoolCount");

// elasticsearch
DEFINE_string(esHost, "http://127.0.0.1:9200/", "elastic search url");

// redis
DEFINE_string(redis_host, "127.0.0.1", "host");
DEFINE_int32(redis_port, 6379, "port");
DEFINE_int32(redis_db, 0, "库编号(默认0)");
DEFINE_bool(redis_keepAlive, true, "keepAlive");

// dms
DEFINE_string(keyId, "your_access_key_id", "DMS 平台访问密钥 ID");
DEFINE_string(keySecret, "your_access_key_secret", "DMS 平台访问密钥 Secret");

// daemon
DEFINE_bool(use_daemon, true, "是否启用守护进程");
DEFINE_string(work_dir, "", "守护进程工作目录");

auto main(int argc, char *argv[]) -> int
{
    // 解析 gflags
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    stellar_post::user::UserServerBuilder usb;
    usb.MakeES({FLAGS_esHost});                                                                                     // elasticsearch 客户端
    usb.MakeMySQL(FLAGS_user, FLAGS_pswd, FLAGS_host, FLAGS_database, FLAGS_cset, FLAGS_port, FLAGS_connPoolCount); // mysql 客户端
    usb.MakeRedis(FLAGS_redis_host, FLAGS_redis_port, FLAGS_redis_db, FLAGS_redis_keepAlive);                       // redis 客户端
    usb.MakeDiscovery(FLAGS_registryHost, FLAGS_baseService, FLAGS_fileService);                                    // 服务发现 + channel 管理客户端
    usb.MakeDMS(FLAGS_keyId, FLAGS_keySecret);                                                                      // 短信验证码平台 SDK 操作句柄
    usb.MakeRPC(FLAGS_rpc_linstenPort, FLAGS_rpc_idleTimeout, FLAGS_rpc_numThreads);                                // rpc 服务器 -- 前
    usb.MakeRegistry(FLAGS_registryHost, FLAGS_baseService + FLAGS_instanceName, FLAGS_accessHost);                 // 服务注册客户端 -- 后
    usb.MakeDaemon(FLAGS_use_daemon, FLAGS_work_dir);                                                               // 守护进程
    auto server = usb.Build();
    server->Run();

    return 0;
}