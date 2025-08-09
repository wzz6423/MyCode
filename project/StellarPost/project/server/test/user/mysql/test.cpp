// 测试通过

// mysql_operator_user 单元测试实现

// C++
#include <print>
// Other
#include <gflags/gflags.h>
#include "../../../../project/user/odb/user.hxx"
#include "../../../../project/user/operator/user_mysql.hpp"
#include "user-odb.hxx"

DEFINE_string(user, "aaaaa", "mysql user");
DEFINE_string(pswd, "aaaaa", "musql user's password");
DEFINE_string(host, "127.0.0.1", "host");
DEFINE_string(database, "stellar_post", "database");
DEFINE_string(cset, "utf8", "cset");
DEFINE_int32(port, 0, "port");
DEFINE_int32(connPoolCount, 1, "connPoolCount");

auto insert(stellar_post::user::UserTable &user) -> void
{
    std::shared_ptr<stellar_post::user::User> u1 =
        std::make_shared<stellar_post::user::User>("uid1", "nickname1", "password1");
    bool ret = user.Insert(u1);
    if (!ret)
    {
        std::println("insert user1 failed");
    }

    std::shared_ptr<stellar_post::user::User> u2 =
        std::make_shared<stellar_post::user::User>("uid2", "1234567");
    ret = user.Insert(u2);
    if (!ret)
    {
        std::println("insert user2 failed");
    }
}

auto SelectIdAndUpdate(stellar_post::user::UserTable &user) -> void
{
    auto u = user.SelectID("uid1");
    std::println("{} : {} : {} : {}", u->UserID(), u->NickName(), u->Password(), u->Description());
    u->Description("Hello World!");
    user.Update(u);

    u = user.SelectID("uid1");
    std::println("{} : {} : {} : {}", u->UserID(), u->NickName(), u->Password(), u->Description());
}

auto SelectPhone(stellar_post::user::UserTable &user) -> void
{
    auto u = user.SelectPhone("1234567");
    std::println("{} : {} : {}", u->UserID(), u->NickName(), u->Description());
    // std::println("{} : {} : {} : {}", u->UserID(), u->NickName(), u->Password(), u->Description()); 
    // 报错 std::bad_alloc
    // 可能原因: 上面进行修改 update 使用, 故将空字段也申请了内存空间, 而此处仅查看未修改故没有内容的字段没有内存空间申请, 
    // 指针为 nullptr 申请访问会报错
}

auto SelectNickname(stellar_post::user::UserTable &user) -> void
{
    auto u = user.SelectNickName("nickname1");
    std::println("{} : {} : {} : {}", u->UserID(), u->NickName(), u->Password(), u->Description());
}

auto SelectUsers(stellar_post::user::UserTable &user) -> void
{
    std::vector<std::string> ids = {"uid1", "uid2"};
    auto u = user.SelectIDs(ids);
    for (auto &i : u)
    {
        std::println("{} : {}", i->UserID(), i->NickName());
    }
}

auto main(int argc, char *argv[]) -> int
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    auto db = stellar_post::Operator::ODBFactory::Create(FLAGS_user, FLAGS_pswd, FLAGS_host, FLAGS_database, FLAGS_cset,
                                                     FLAGS_port, FLAGS_connPoolCount);

    stellar_post::user::UserTable user(db);

    // 增
    // insert(user);
    // 改
    // SelectIdAndUpdate(user);
    // SelectPhone(user);
    // SelectNickname(user);
    SelectUsers(user);

    return 0;
}