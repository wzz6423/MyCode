// C++
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
// etcd
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>

auto main(int argc, char *argv[]) -> int
{
    std::string etcdHost = "http://127.0.0.1:2379";

    // 实例化客户端对象
    etcd::Client client(etcdHost);

    // 获取租约保活对象 -- 伴随着创建一个指定有效时长的租约
    auto keepAlive = client.leasekeepalive(3).get(); // 3 秒有效租约

    // 获取租约 id
    auto leaseId = keepAlive->Lease();

    // 向 etcd 新增数据
    auto response1 = client.put("/service/usr", "example_value1", leaseId).get();
    auto response2 = client.put("/service/friend", "example_value2", leaseId).get();
    if (response1.is_ok() == false)
    {
        std::cerr << "Put failed: " << response1.error_message() << std::endl;
        return 1;
    }
    if (response2.is_ok() == false)
    {
        std::cerr << "Put failed: " << response2.error_message() << std::endl;
        return 1;
    }

    std::this_thread::sleep_for(std::chrono::seconds(10)); // 睡眠 10s

    return 0;
}