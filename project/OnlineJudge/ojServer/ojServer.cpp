// C++
#include <memory>
// Other
#include "ojModel.hpp"
#include "ojView.hpp"
#include "ojControl.hpp"
#include "../common/httplib.h"

auto main() -> int
{
    // 用户请求的服务路由功能
    httplib::Server svr;
    std::unique_ptr<control::Control> control = std::make_unique<control::Control>();

    svr.set_base_dir("./wwwroot");

    // 获取题目列表
    svr.Get("/all_questions", [&](const httplib::Request &req, httplib::Response &res)
            {
        // 返回包含所有题目列表的 html 网页
        std::string html = control->AllQuestions();
        res.set_content(html, "text/html; charset=utf-8"); });

    // 根据题目编号获取题目详细内容
    // 正则表达式匹配题目编号
    svr.Get(R"(/question/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        std::string number = req.matches[1];
        std::string html = control->Question(number);
        res.set_content(html, "text/html; charset=utf-8"); });

    // 根据题目编号提交题目, 每道题目测试用例 & 编译运行
    svr.Post(R"(/judge/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
             {
        std::string number = req.matches[1];
        std::string inJson = req.body;
        std::string outJson = control->Judge(number, inJson);
        res.set_content(outJson, "application/json; charset=utf-8"); });

    svr.listen("0.0.0.0", 6423);

    return 0;
}