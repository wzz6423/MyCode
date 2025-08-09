// 测试通过

// redis_operator_user 单元测试

// C++
#include <print>
// Other
#include <gflags/gflags.h>
#include "../../../../project/user/operator/user_redis.hpp"

DEFINE_string(host, "127.0.0.1", "host");
DEFINE_int32(port, 6379, "port");
DEFINE_int32(db, 0, "库编号(默认0)");
DEFINE_bool(keepAlive, true, "keepAlive");

auto seesionTest(std::shared_ptr<sw::redis::Redis> &client)
{
    stellar_post::user::Session session(client);
    session.Add("ssid1", "uid1");
    session.Add("ssid2", "uid2");
    session.Add("ssid3", "uid3");
    session.Add("ssid4", "uid4");

    session.Remove("ssid1");

    std::println("{}", *(session.Get("ssid2"))); // 如果 Get ssid1 会段错误(optional 在正规使用场景要检查)
}

auto statusTest(std::shared_ptr<sw::redis::Redis> &client)
{
    stellar_post::user::Status status(client);
    status.Add("uid1");
    status.Add("uid2");
    status.Remove("uid1");
    std::println("{}", status.Exists("uid1"));
    std::println("{}", status.Exists("uid2"));
}

auto codeTest(std::shared_ptr<sw::redis::Redis> &client)
{
    stellar_post::user::Codes codes(client);
    codes.Add("uid1", "code1", std::chrono::seconds(3));
    codes.Add("uid2", "code2", std::chrono::seconds(3));
    codes.Add("uid3", "code3", std::chrono::seconds(3));

    codes.Remove("uid1");

    // std::println("{}", codes.Get("uid1")); // error
    std::println("{}", *codes.Get("uid2"));
    std::println("{}", *codes.Get("uid3"));

    std::this_thread::sleep_for(std::chrono::seconds(4));
    if(!codes.Get("uid2"))
    {
        std::println("uid2 验证码不存在");
    }
    if(!codes.Get("uid3"))
    {
        std::println("uid3 验证码不存在");
    }
}

auto main(int argc, char *argv[]) -> int
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    auto client = stellar_post::Operator::RedisFactory::Create(
        FLAGS_host, FLAGS_port, FLAGS_db, FLAGS_keepAlive);

    // seesionTest(client);
    // statusTest(client);
    // codeTest(client);

    return 0;
}