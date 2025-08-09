// 测试通过

// mysql_operator_crony 单元测试实现

// C++
#include <print>
// Other
#include <gflags/gflags.h>
#include "../../../../project/common/utils/utils.hpp"
#include "../../../../project/crony/operator/chat_session_mysql.hpp"
#include "../../../../project/crony/operator/crony_apply_mysql.hpp"
#include "../../../../project/crony/operator/crony_relation_mysql.hpp"

DEFINE_string(user, "aaaaa", "mysql user");
DEFINE_string(pswd, "aaaaa", "musql user's password");
DEFINE_string(host, "127.0.0.1", "host");
DEFINE_string(database, "stellar_post", "database");
DEFINE_string(cset, "utf8", "cset");
DEFINE_int32(port, 0, "port");
DEFINE_int32(connPoolCount, 1, "connPoolCount");

// relation Add Test
auto relationAdd(stellar_post::crony::CronyRelationTable &tb)
{
    tb.Add("userId1", "userId2");
    tb.Add("userId1", "userId3");
}

// relation Select Test
auto relationSelect(stellar_post::crony::CronyRelationTable &tb)
{
    auto relationList = tb.CronyIds("userId1");
    for (auto &relation : relationList)
    {
        std::println("{}", relation);
    }
}

// relation Remove Test
auto relationRemove(stellar_post::crony::CronyRelationTable &tb)
{
    tb.Remove("userId1", "userId2");
}

// relation Exists Test
auto relationExists(stellar_post::crony::CronyRelationTable &tb)
{
    auto exists = tb.Exists("userId1", "userId2");
    std::println("12: {}", exists);
    exists = tb.Exists("userId1", "userId3");
    std::println("13: {}", exists);
    exists = tb.Exists("userId2", "userId3");
    std::println("23: {}", exists);
}

// apply Add Test
auto applyAdd(stellar_post::crony::CronyApplyTable &tb)
{
    std::shared_ptr<stellar_post::crony::CronyApply> apply =
        std::make_shared<stellar_post::crony::CronyApply>(stellar_post::Utils::Uuid(), "userId1", "userId2");
    tb.Add(apply);
    apply = std::make_shared<stellar_post::crony::CronyApply>(stellar_post::Utils::Uuid(), "userId1", "userId3");
    tb.Add(apply);
    apply = std::make_shared<stellar_post::crony::CronyApply>(stellar_post::Utils::Uuid(), "userId2", "userId3");
    tb.Add(apply);
}

// apply Remove Test
auto applyRemove(stellar_post::crony::CronyApplyTable &tb)
{
    tb.Remove("userId1", "userId2");
}

// apply Select Test
auto applySelect(stellar_post::crony::CronyApplyTable &tb)
{
    auto applyList = tb.ApplyUserList("userId3");
    for (auto &apply : applyList)
    {
        std::println("{}", apply);
    }
}

// apply exists Test
auto applyExists(stellar_post::crony::CronyApplyTable &tb)
{
    auto exists = tb.Exists("userId1", "userId2");
    std::println("12: {}", exists);
    exists = tb.Exists("userId1", "userId3");
    std::println("13: {}", exists);
}

// chat session Add Test
// insert into chat_session_member values("", "sessionId2", "userId1");
// insert into chat_session_member values("", "sessionId2", "userId2");
// insert into chat_session_member values("", "sessionId2", "userId3");
auto chatSessionAdd(stellar_post::crony::ChatSessionTable &tb)
{
    std::shared_ptr<stellar_post::crony::ChatSession> cs =
        std::make_shared<stellar_post::crony::ChatSession>("sessionId1",
                                                           "sessionName1",
                                                           stellar_post::crony::ChatType::SINGLE);
    tb.Add(cs);
    cs = std::make_shared<stellar_post::crony::ChatSession>("sessionId2",
                                                            "sessionName2",
                                                            stellar_post::crony::ChatType::GROUP);
    tb.Add(cs);
}

// chat session Select Test
auto chatSessionSelect(stellar_post::crony::ChatSessionTable &tb)
{
    auto cs_opt = tb.Select("sessionId2");
    if (cs_opt.has_value())
    {
        auto cs = cs_opt.value();
        std::println("{}", cs->ChatSessionId());
        std::println("{}", cs->ChatSessionName());
        std::println("{}", cs->ChatSessionType());
    }
    else
    {
        std::println("chat session not found");
    }
}

// chat session single Test
auto chatSessionSingle(stellar_post::crony::ChatSessionTable &tb)
{
    auto res_opt = tb.SelectSingleChatSession("userId1");
    if (res_opt.has_value())
    {
        auto res = res_opt.value();
        for (const auto &r : res)
        {
            std::println("{}", r->_chat_session_id);
            std::println("{}", r->_crony_id);
        }
    }
    else
    {
        std::println("chat session not found");
    }
}

// chat session Select Group Test
auto chatSessionSelectGroup(stellar_post::crony::ChatSessionTable &tb)
{
    auto res_opt = tb.SelectGroupChatSession("userId1");
    if (res_opt.has_value())
    {
        auto res = res_opt.value();
        for (const auto &r : res)
        {
            std::println("{}", r->_chat_session_id);
            std::println("{}", r->_chat_session_name);
        }
    }
    else
    {
        std::println("chat session not found");
    }
}

// chat session Remove Test
auto chatSessionRemove(stellar_post::crony::ChatSessionTable &tb)
{
    tb.Remove("sessionId1");
}

// chat session Remove by user Test
auto chatSessionRemoveByUser(stellar_post::crony::ChatSessionTable &tb)
{
    tb.Remove("userId1", "userId2");
}

auto main(int argc, char *argv[]) -> int
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    auto db = stellar_post::Operator::ODBFactory::Create(FLAGS_user, FLAGS_pswd, FLAGS_host, FLAGS_database, FLAGS_cset,
                                                         FLAGS_port, FLAGS_connPoolCount);
    stellar_post::crony::CronyRelationTable crt(db);
    stellar_post::crony::ChatSessionTable cst(db);
    stellar_post::crony::CronyApplyTable cat(db);

    relationAdd(crt);
    relationSelect(crt);
    relationRemove(crt);
    relationExists(crt);
    applyAdd(cat);
    applyRemove(cat);
    applySelect(cat);
    applyExists(cat);
    chatSessionAdd(cst);
    chatSessionSelect(cst);
    chatSessionSingle(cst);
    chatSessionSelectGroup(cst);
    chatSessionRemove(cst);
    chatSessionRemoveByUser(cst);

    return 0;
}