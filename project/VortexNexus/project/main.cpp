#include "Http.hpp"

#define WWWROOT "../wwwroot/"
#define DEFAULT_SRC "index.html"

int main()
{
    HttpServer server(6423);
    server.SetThreadCount(9);
    server.SetBaseDirAndFile(WWWROOT, DEFAULT_SRC);//设置静态资源根目录，告诉服务器有静态资源请求到来，需要到哪里去找资源文件
    server.Start();

    return 0;
}