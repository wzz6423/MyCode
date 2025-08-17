#include "search.hpp"
#include "httplib.h"

const std::string wwwroot = "./wwwroot";

auto main() -> int
{
    search::Search search;
    search.Init();

    httplib::Server svr;
    svr.set_base_dir(wwwroot);
    svr.Get("/s", [&search](const httplib::Request &req, httplib::Response &rsp)
            {
        if (!req.has_param("word"))
        {
            rsp.set_content("请输入您要搜索的内容", "text/plain; charset=utf-8");
            return;
        }
        std::string word = req.get_param_value("word");
        std::string res = search.Searcher(word);
        rsp.set_content(res, "application/json"); });

    svr.listen("0.0.0.0", 6423);

    return 0;
}