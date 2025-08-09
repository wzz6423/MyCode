// 测试通过

// mysql_operator_user 单元测试实现

// C++
#include <print>
// Other
#include <gflags/gflags.h>
#include "../../../../project/message/odb/message.hxx"
#include "../../../../project/message/operator/message_mysql.hpp"
#include "message-odb.hxx"

DEFINE_string(user, "aaaaa", "mysql user");
DEFINE_string(pswd, "aaaaa", "musql user's password");
DEFINE_string(host, "127.0.0.1", "host");
DEFINE_string(database, "stellar_post", "database");
DEFINE_string(cset, "utf8", "cset");
DEFINE_int32(port, 0, "port");
DEFINE_int32(connPoolCount, 1, "connPoolCount");

auto insert(const stellar_post::message::MessageTable &tb)
{
    boost::posix_time::ptime time(boost::posix_time::time_from_string("2021-10-09 02:02:00.000"));
    std::shared_ptr<stellar_post::message::Message> msg =
        std::make_shared<stellar_post::message::Message>("msgID1", "sessionID1", "userID1", 0, time);
    tb.Add(msg);
    time = boost::posix_time::time_from_string("2021-10-09 03:02:00.000");
    msg = std::make_shared<stellar_post::message::Message>("msgID2", "sessionID1", "userID2", 0, time);
    tb.Add(msg);
    time = boost::posix_time::time_from_string("2021-10-09 09:02:00.000");
    msg = std::make_shared<stellar_post::message::Message>("msgID3", "sessionID1", "userID3", 0, time);
    tb.Add(msg);
    time = boost::posix_time::time_from_string("2021-10-10 09:02:00.000");
    msg = std::make_shared<stellar_post::message::Message>("msgID4", "sessionID2", "userID4", 0, time);
    tb.Add(msg);
    time = boost::posix_time::time_from_string("2021-10-10 22:08:00.000");
    msg = std::make_shared<stellar_post::message::Message>("msgID5", "sessionID2", "userID5", 0, time);
    tb.Add(msg);
}

auto remove(const stellar_post::message::MessageTable &tb)
{
    tb.Remove("sessionID2");
}

auto select(const stellar_post::message::MessageTable &tb)
{
    auto res = tb.SelectRencent("sessionID1", 2);
    for (const auto &e : res)
    {
        std::println("{} :{} : {} : {}", e->MessageID(), e->SessionID(), e->UserID(),
                     boost::posix_time::to_simple_string(e->GenerateTime()));
    }

    std::println("---------------------------------------------------------------------------------");

    res = tb.SelectRange("sessionID1", boost::posix_time::time_from_string("2021-10-09 03:02:00.000"),
                         boost::posix_time::time_from_string("2021-10-10 09:02:00.000"));
    for (const auto &e : res)
    {
        std::println("{} :{} : {} : {}", e->MessageID(), e->SessionID(), e->UserID(),
                     boost::posix_time::to_simple_string(e->GenerateTime()));
    }
}

auto main(int argc, char *argv[]) -> int
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    auto db = stellar_post::Operator::ODBFactory::Create(FLAGS_user, FLAGS_pswd, FLAGS_host, FLAGS_database, FLAGS_cset,
                                                         FLAGS_port, FLAGS_connPoolCount);

    stellar_post::message::MessageTable mt(db);
    insert(mt);
    remove(mt);
    select(mt);

    return 0;
}