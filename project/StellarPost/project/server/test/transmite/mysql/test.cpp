// 测试通过

// mysql_operator_user 单元测试实现

// C++
#include <print>
// Other
#include <gflags/gflags.h>
#include "../../../../project/common/odb/chat_session_member.hxx"
#include "../../../../project/common/operator/chat_session_member.hpp"
#include "chat_session_member-odb.hxx"

DEFINE_string(user, "aaaaa", "mysql user");
DEFINE_string(pswd, "aaaaa", "musql user's password");
DEFINE_string(host, "127.0.0.1", "host");
DEFINE_string(database, "stellar_post", "database");
DEFINE_string(cset, "utf8", "cset");
DEFINE_int32(port, 0, "port");
DEFINE_int32(connPoolCount, 1, "connPoolCount");

auto append(const stellar_post::crony::ChatSessionMemberTable &tb)
{
    std::shared_ptr<stellar_post::crony::ChatSessionMember> csm =
        std::make_shared<stellar_post::crony::ChatSessionMember>("session_id_0", "user_id_0");
    tb.Append(csm);
    csm = std::make_shared<stellar_post::crony::ChatSessionMember>("session_id_1", "user_id_1");
    tb.Append(csm);
    csm = std::make_shared<stellar_post::crony::ChatSessionMember>("session_id_2", "user_id_2");
    std::shared_ptr<stellar_post::crony::ChatSessionMember> csm2 =
        std::make_shared<stellar_post::crony::ChatSessionMember>("session_id_2", "user_id_3");
    tb.Append({csm, csm2});
}

auto select(const stellar_post::crony::ChatSessionMemberTable &tb)
{
    auto res = tb.AllSessionMember("session_id_2");
    for (auto &csm : res)
    {
        std::println("{}", csm);
    }
}

auto remove(const stellar_post::crony::ChatSessionMemberTable &tb)
{
    std::shared_ptr<stellar_post::crony::ChatSessionMember> csm =
        std::make_shared<stellar_post::crony::ChatSessionMember>("session_id_0", "user_id_0");
    tb.Remove(csm);
    tb.Clear("session_id_2");
}

auto main(int argc, char *argv[]) -> int
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    auto db = stellar_post::Operator::ODBFactory::Create(FLAGS_user, FLAGS_pswd, FLAGS_host, FLAGS_database, FLAGS_cset,
                                                         FLAGS_port, FLAGS_connPoolCount);

    stellar_post::crony::ChatSessionMemberTable csmt(db);

    append(csmt);
    select(csmt);
    remove(csmt);

    return 0;
}