// C++
#include <print>
// httplib
#include "httplib.h"

auto main() -> int
{
    // 实例化服务器对象
    httplib::Server server;

    // 注册回调函数 -- 格式: void(const httplib::Request&, httplib::Response&);
    server.Get("/hi", [](const httplib::Request &req, httplib::Response &rsp)
               { 
                std::println("method: {}, path: {}", req.method, req.path);
                for(auto& it : req.headers)
                {
                    std::println("{} : {}", it.first, it.second);
                }
                rsp.set_content("Hello World!~", "text/html");
                rsp.status = 200; });

    // 启动服务器
    server.listen("0.0.0.0", 6423);

    return 0;
}