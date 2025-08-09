/*
    实现用户子服务模块搭建
*/

// messageServer 模块
#include "message_server.hpp"
// gflags
#include <gflags/gflags.h>

// etcd
DEFINE_string(registryHost, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(fileService, "/service/file_service", "文件服务名称");
DEFINE_string(userService, "/service/user_service", "用户服务名称");
DEFINE_string(instanceName, "/message_service/instance", "当前实例名称");
DEFINE_string(accessHost, "127.0.0.1:16427", "服务实例外部访问地址");

// rpc
DEFINE_int32(rpc_linstenPort, 16427, "rpc 服务默认监听端口");
DEFINE_int32(rpc_idleTimeout, -1, "rpc 空闲超时时间");
DEFINE_int32(rpc_numThreads, 4, "rpc IO 线程数量");

// mysql
DEFINE_string(user, "aaaaa", "mysql message");
DEFINE_string(pswd, "aaaaa", "mysql message's password");
DEFINE_string(host, "127.0.0.1", "host");
DEFINE_string(database, "stellar_post", "database");
DEFINE_string(cset, "utf8", "cset");
DEFINE_int32(port, 0, "port");
DEFINE_int32(connPoolCount, 4, "connPoolCount");

// elasticsearch
DEFINE_string(esHost, "http://127.0.0.1:9200/", "elastic search url");

// RabbitMq
DEFINE_string(mq_user, "aaaaa", "rabbitmq user");
DEFINE_string(mq_passwd, "aaaaa", "rabbitmq user's password");
DEFINE_string(mq_host, "127.0.0.1:5672", "rabbitmq host");
DEFINE_string(mq_exchangeName, "msgExchange", "message persistence rabbitmq's exchangeName");
DEFINE_string(mq_queueName, "msgQueue", "message persistence rabbitmq's queueName");
// DEFINE_string(mq_bindingKey, "msgBindingKey", "message persistence rabbitmq's bindingKey"); // 通常 bindingKey 与 queueName 一致
DEFINE_string(mq_bindingKey, "msgQueue", "message persistence rabbitmq's bindingKey");

// daemon
DEFINE_bool(use_daemon, true, "是否启用守护进程");
DEFINE_string(work_dir, "", "守护进程工作目录");

auto main(int argc, char *argv[]) -> int
{
    // 解析 gflags
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    stellar_post::message::MessageServerBuilder msb;
    msb.MakeES({FLAGS_esHost}); // elasticsearch 客户端
    msb.MakeMySQL(FLAGS_user, FLAGS_pswd, FLAGS_host, FLAGS_database, FLAGS_cset,
                  FLAGS_port, FLAGS_connPoolCount);                                                 // mysql 客户端
    msb.MakeDiscovery(FLAGS_registryHost, FLAGS_baseService, FLAGS_fileService, FLAGS_userService); // 服务发现 + channel 管理客户端
    msb.MakeRabbitMQ(FLAGS_mq_user, FLAGS_mq_passwd, FLAGS_mq_host, FLAGS_mq_exchangeName,
                     FLAGS_mq_queueName, FLAGS_mq_bindingKey);                       // RabbitMQ 客户端
    msb.MakeRPC(FLAGS_rpc_linstenPort, FLAGS_rpc_idleTimeout, FLAGS_rpc_numThreads); // rpc 服务器 -- 前
    msb.MakeRegistry(FLAGS_registryHost, FLAGS_baseService + FLAGS_instanceName,
                     FLAGS_accessHost);               // 服务注册客户端 -- 后
    msb.MakeDaemon(FLAGS_use_daemon, FLAGS_work_dir); // 守护进程

    auto server = msb.Build();
    server->Run();

    return 0;
}