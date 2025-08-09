// 测试通过

// C++
#include <print>
// Other
#include "../../../../project/user/operator/user_elastic.hpp"
#include <gflags/gflags.h>

DEFINE_string(esHost, "http://127.0.0.1:9200/", "elastic search url");

auto main(int argc, char *argv[]) -> int
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    auto esClient = stellar_post::Operator::ESClientFactory::Create({FLAGS_esHost});
    auto esUser = std::make_shared<stellar_post::user::ESUser>(esClient);

    if (!esUser->CreateIndex())
    {
        return -1;
    }
    esUser->Add("用户ID1", "111111", "用户昵称1", "用户描述1", "用户头像ID1");
    esUser->Add("用户ID2", "222222", "用户昵称2", "用户描述2", "用户头像ID2");
    auto res = esUser->Search("用户", {"用户ID1"});
    for (auto &user : res)
    {
        std::println("--------------------------------------");
        std::println("UserID: {}", user->UserID());
        std::println("NickName: {}", user->NickName());
        std::println("Phone: {}", user->Phone());
        std::println("Description: {}", user->Description());
        std::println("AvatarID: {}", user->AvatarID());
    }

    return 0;
}