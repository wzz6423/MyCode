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

DEFINE_bool(debug_enable, true, "debug enable, true-调试, false-发布");

DEFINE_string(ectdHost, "127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(baseService, "/service", "服务监控根目录");
DEFINE_string(instanceName, "/friend", "当前实例化目录");

auto main(int argc, char *argv[]) -> int
{
    static stellar_post::Log::Log &lg = stellar_post::Log::Log::GetInstance(FLAGS_debug_enable);

    stellar_post::Etcd::Registry::registryPtr registry = std::make_shared<stellar_post::Etcd::Registry>(FLAGS_ectdHost);
    registry->registry(FLAGS_baseService + FLAGS_instanceName + "/instance", "test_value");

    std::this_thread::sleep_for(std::chrono::seconds(600));

    return 0;
}