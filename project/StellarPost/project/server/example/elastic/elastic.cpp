// C++
#include <print>
#include <string>
#include <memory>
// es
#include <elasticlient/client.h>
#include <cpr/cpr.h>

auto main() -> int
{
    // 要进行异常捕获, 防止进程崩溃
    try
    {
        // 构造 es 客户端
        elasticlient::Client client({"http://127.0.0.1:9200/"}); // 最后这个 /相对根目录 是必须有的

        // 发起请求(搜索)
        auto rsp = client.search("user", "_doc", R"({"query": { "match_all":{}}})");

        // 打印响应状态码 & 响应正文
        std::println("状态码: {}, 正文: {}", rsp.status_code, rsp.text);
    }
    catch(std::exception& e)
    {
        std::println("err: {}",e.what());
    }

    return 0;
}