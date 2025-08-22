// C++
#include <string>
#include <fstream>
// Other
#include "daemon.hpp"
#include "httplib.h"

const std::string wwwroot = "wwwroot/";

auto main() -> int
{
    // 创建守护进程
    Daemon::Daemon::Create();

    // 注册 hhtp 服务
    httplib::Server svr;

    svr.set_base_dir(wwwroot.c_str());

    svr.Get("/", [](const httplib::Request &req, httplib::Response &rsp)
            {
        std::ifstream file("index.html", std::ios::in | std::ios::binary);

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        if (file) {
            rsp.status = 200;
            rsp.set_content(content, "text/html");
        } else {
            rsp.status = 404;
            rsp.set_content("Not Found", "text/plain");
        } });

    // 启动服务器
    // svr.listen("0.0.0.0", 6423);
    svr.listen("0.0.0.0", 80);

    return 0;
}