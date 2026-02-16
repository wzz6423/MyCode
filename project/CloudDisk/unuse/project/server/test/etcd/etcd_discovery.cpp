// 测试通过

// C++
#include <print>
#include <memory>
#include <thread>
#include <chrono>
// test etcd
#include "../../project/common/etcd/etcd.hpp"
#include "../../project/common/log/logger.hpp"
// gflags
#include <gflags/gflags.h>

DEFINE_string(ectdHost, "127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");

void online(const std::string &key, const std::string &value)
{
    // std::println("online: {} : {}", key, value);
    cloud_disk::Log::lg.dbg("info", "online: {} : {}", key, value);
}

void offline(const std::string &key, const std::string &value)
{
    // std::println("offline: {} : {}", key, value);
    cloud_disk::Log::lg.dbg("info", "offline: {} : {}", key, value);
}

auto main(int argc, char *argv[]) -> int
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    cloud_disk::Etcd::Discovery::discoveryPtr discovery = std::make_shared<cloud_disk::Etcd::Discovery>(FLAGS_ectdHost,
                                                                                                          FLAGS_baseService,
                                                                                                          online, offline);

    std::this_thread::sleep_for(std::chrono::seconds(600));

    return 0;
}