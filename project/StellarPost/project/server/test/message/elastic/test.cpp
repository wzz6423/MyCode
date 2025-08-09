// 测试通过

// C++
#include <print>
// Other
#include "../../../../project/message/operator/message_elastic.hpp"
#include <gflags/gflags.h>

DEFINE_string(esHost, "http://127.0.0.1:9200/", "elastic search url");

auto main(int argc, char *argv[]) -> int
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    auto esClient = stellar_post::Operator::ESClientFactory::Create({FLAGS_esHost});
    auto esMsg = std::make_shared<stellar_post::message::ESMessage>(esClient);

    esMsg->Add("user_id1", "message_id1", "chat_session_id1", "Hello World!", 1753768161);
    esMsg->Add("user_id2", "message_id2", "chat_session_id1", "Hello World too!", 1753768161 - 100);
    esMsg->Add("user_id3", "message_id3", "chat_session_id2", "Hello World!", 1753768161 - 200);
    esMsg->Add("user_id4", "message_id4", "chat_session_id1", "Hallo World!", 1753768161 - 150);
    esMsg->Add("user_id5", "message_id5", "chat_session_id2", "Hi World!", 1753768161 + 10);
    if (!esMsg->CreateIndex())
    {
        return -1;
    }

    auto res = esMsg->Search("World", "chat_session_id1");
    auto show = [&]()
    {
        for (auto &msg : res)
        {
            std::println("--------------------------------------");
            std::println("UserID: {}", msg->UserID());
            std::println("ChatSessionID: {}", msg->ChatSessionId());
            std::println("MessageID: {}", msg->MessageID());
            std::println("Content: {}", msg->Content());
            std::println("GenerateTime: {}", boost::posix_time::to_simple_string(msg->GenerateTime()));
        }
    };
    show();

    res = esMsg->Search("World", "chat_session_id2");
    show();

    return 0;
}