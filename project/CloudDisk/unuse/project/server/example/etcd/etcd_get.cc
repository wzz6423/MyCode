// C++
#include <iostream>
#include <string>
#include <print>
#include <thread>
// etcd
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>
#include <etcd/Value.hpp>
#include <etcd/Watcher.hpp>

auto callback(const etcd::Response &response) -> void
{
    if (response.is_ok() == false)
    {
        std::cerr << "Failed to get response: " << response.error_message() << std::endl;
        return;
    }

    for (const auto &e : response.events())
    {
        if (e.event_type() == etcd::Event::EventType::PUT)
        {
            std::println("服务信息变化 - PUT: {} : {} -> {} : {}", e.prev_kv().key(), e.prev_kv().as_string(), e.kv().key(), e.kv().as_string());
        }
        else if (e.event_type() == etcd::Event::EventType::DELETE_)
        {
            std::println("服务信息下线 - DELETE: {} : {}", e.prev_kv().key(), e.prev_kv().as_string());
        }
        else
        {
            std::println("服务信息无效 EVENT TYPE: {}", static_cast<int>(e.event_type()));
        }
    }
}

auto main(int argc, char *argv[]) -> int
{
    std::string etcdHost = "http://127.0.0.1:2379";

    // 实例化客户端对象
    etcd::Client client(etcdHost);

    // 获取指定的键值对信息
    auto response = client.ls("/service").get();
    if (response.is_ok() == false)
    {
        std::cerr << "Failed to get response: " << response.error_message() << std::endl;
        return 1;
    }

    int size = response.keys().size();
    for (int i = 0; i < size; ++i)
    {
        std::println("{} 提供服务: {}", response.value(i).as_string(), response.key(i));
    }

    // 实例化键值对事件监控对象
    auto watcher = etcd::Watcher(client, "/service", callback, true);
    watcher.Wait();

    return 0;
}