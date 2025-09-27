#include "compileRun.hpp"
#include "../common/httplib.h"

auto main(int argc, char *argv[]) -> int
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    httplib::Server svr;

    svr.Post("/compileRun", [](const httplib::Request &req, httplib::Response &res)
             {
        // 用户请求服务正文是需要的 json string
        // 解析请求参数
        std::string inJson = req.body;
        if (!inJson.empty()) [[likely]]
        {
            std::optional<std::string> outJson_opt = compile_run::CompileRun::Run(inJson);
            if (!outJson_opt) [[unlikely]]
            {
                LOG(ERROR) << "Failed to process request" << std::endl;
                res.status = 500; // Internal Server Error
                res.set_content(R"({"error": "Failed to process request"})", "application/jsonchar-set=utf-8");
                return;
            }

            LOG(ERROR) << "Request body is empty" << std::endl;
            res.set_content(std::move(outJson_opt.value()), "application/json");
        }
        else [[unlikely]]
        {
            res.status = 400; // Bad Request
            res.set_content(R"({"error": "Request body is empty"})", "application/json");
        } });

    svr.listen("0.0.0.0", std::stoi(argv[1]));

    return 0;
}