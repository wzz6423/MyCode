// C++
#include <print>
#include <chrono>
#include <thread>
// redis
#include <sw/redis++/redis.h>
// gflags
#include <gflags/gflags.h>

DEFINE_string(ip, "0.0.0.0", "服务器默认 IP 地址, 格式: n.n.n.n");
DEFINE_int32(port, 6379, "服务器默认 port, 格式: 0-65535");
DEFINE_int32(db, 0, "服务器默认库编号, 默认: 0");
DEFINE_bool(keepAlive, true, "是否进行长连接保活, 默认: true");

auto main(int argc, char *argv[]) -> int
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 构造连接选项, 实例化 redis 服务器对象, 连接服务器
    sw::redis::ConnectionOptions opts;
    opts.host = FLAGS_ip;
    opts.port = FLAGS_port;
    opts.db = FLAGS_db;
    opts.keep_alive = FLAGS_keepAlive;

    sw::redis::Redis client(opts);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::println("-------------- test 1 --------------");

    // 添加字符串键值对, 获取字符串键值对, 删除字符串键值对
    // set: 不存在则新增, 已存在则修改
    auto addString = [&]()
    {
        client.set("会话 id 1", "用户 id 1");
        client.set("会话 id 2", "用户 id 2");
        client.set("会话 id 3", "用户 id 3");
        client.set("会话 id 4", "用户 id 4");
        client.set("会话 id 5", "用户 id 5");
    };
    auto delString = [&]()
    {
        client.del("会话 id 3");
    };
    auto changeString = [&]()
    {
        client.set("会话 id 4", "用户 id 8");
    };
    auto getString = [&]()
    {
        for (int i = 1; i <= 5; ++i)
        {
            std::string usr = "会话 id ";
            auto usrString = client.get(usr + std::to_string(i));
            if (usrString)
            {
                std::println("{}{} : {}", usr, i, *usrString);
            }
            else
            {
                std::println("{}{} : {}", usr, i, "not found");
            }
        }
    };

    addString();
    delString();
    changeString();
    getString();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::println("-------------- test 2 --------------");

    // 控制数据有效时间
    auto expiredTime = [&]()
    {
        // 修改数据并设置过期时间 1s
        client.set("会话 id 1", "用户 id 123", std::chrono::milliseconds(1000));
    };

    std::println("-------------- before --------------");
    getString();
    expiredTime();
    std::println("-------------- after --------------");
    getString();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::println("-------------- after 2s --------------");
    getString();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::println("-------------- test 3 --------------");

    // 列表操作, 实现数据的尾插 & 头获取
    auto listTest = [&]()
    {
        client.rpush("群聊 1", "成员 1");
        client.rpush("群聊 1", "成员 2");
        client.rpush("群聊 1", "成员 3");
        client.rpush("群聊 1", "成员 4");
        client.rpush("群聊 1", "成员 5");

        std::vector<std::string> users;
        client.lrange("群聊 1", 0, -1, std::back_inserter(users));

        for (auto &user : users)
        {
            std::println("{}", user);
        }
    };

    listTest();

    return 0;
}