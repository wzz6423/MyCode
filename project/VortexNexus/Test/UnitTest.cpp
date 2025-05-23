///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test1: 测试 TimeManager
// #include "TimeManager.hpp"
// class Test{
// public:
//     Test(){
//         std::cout << "构造" << std::endl;
//     }

//     ~Test(){
//         std::cout << "析构" << std::endl;
//     }
// };
// void Delete(Test* ptr){
//     delete ptr;
// }
// int main(){
//     TimerWheel tw;

//     Test* tmp = new Test();

//     tw.TimerAdd(666, 3, std::bind(Delete, tmp));

//     for(size_t i = 0; i < 3; ++i){
//         sleep(1);
//         bool ret = tw.TimerRefresh(666); // 刷新定时任务
//         tw.RunTimerTask(); // 向后移动嘀嗒指针
//         if(ret){
//             std::cout << "刷新定时任务" << std::endl;
//         }
//     }
//     // tw.TimerCancel(666);
//     for(size_t i = 0; i < 40; ++i){
//         sleep(1);
//         std::cout << "------------------------------------" << std::endl;
//         tw.RunTimerTask(); // 向后移动嘀嗒指针
//     }

//     return 0;
// }
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// // Test2: 测试 UrlDecode
// #include "UrlDecode.hpp"
// 乱码
// int main(){
//     std::optional<std::smatch> matches = UrlDecode(std::string("GET /wzz/login?user=admin&passwrd=1234567 HTTP/1.1\r\n"));
//     if(matches){
//         for(auto& s:matches.value()){
//             std::cout << s << std::endl;
//         }
//     }
//     else{
//         std::cerr << "decode error" << std::endl;
//     }

//     return 0;
// }
// 不乱码
// int main(){
//     std::string url("GET /wzz/login?user=admin&passwrd=1234567 HTTP/1.1\r\n");
//     url.erase(std::remove(url.begin(), url.end(), '\r'), url.end());
//     url.erase(std::remove(url.begin(), url.end(), '\n'), url.end());
//     std::regex urlEncodeRule(R"((GET|HEAD|POST|PUT|DELETE)\s+([^?\s]+)(?:\?([^\s]*))?\s+(HTTP\/1\.[01]))");
//     std::smatch matches;
//     bool ret = std::regex_match(url, matches, urlEncodeRule);
//     if(ret){
//         for(auto& s:matches){
//             std::cout << s << std::endl;
//         }
//     }
//     else{
//         std::cerr << "decode error" << std::endl;
//     }

//     return 0;
// }
// 第二版
// int main() {
//     const std::string test_request =
//         "GET /wzz/login?user=admin&passwrd=1234567 HTTP/1.1\r\n";

//     auto result = UrlDecode(test_request);
//     if (result) {
//         std::cout << "Method: " << result->method << "\n"
//                   << "Path: " << result->path << "\n"
//                   << "Query: " << result->query << "\n"
//                   << "Protocol: " << result->protocol << "\n"
//                   << "Version: " << result->version << std::endl;
//     } else {
//         std::cerr << "解析失败" << std::endl;
//     }

//     return 0;
// }
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test3: 测试 Any
// #include "Any.hpp"
// #include <string>
// int main(){
//     Any a;
//     a = 10;
//     if(auto tmp = a.Get<int>()){
//         std::cout << *tmp.value() << std::endl;
//     }
//     a = std::string("hello world!");
//     if(auto t = a.Get<std::string>()){
//         std::cout << *t.value() << std::endl;
//     }

//     return 0;
// }
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test4: 测试 Buffer
// #include "Buffer.hpp"
// int main()
// {
//     // 普通测试
//     // Buffer buf;
//     // std::string str = "hello world!";
//     // buf.WriteStringAndPush(str);

//     // std::optional<std::string> tmp;
//     // tmp = buf.ReadAsStringAndPop(buf.ReadAbleSize());
//     // if(tmp){
//     //     std::cout << tmp.value() << std::endl;
//     // }
//     // std::cout << buf.ReadAbleSize() << std::endl;

//     // 扩容测试
//     // Buffer buf;
//     // std::string str = "hello world!";
//     // for (size_t i = 0; i < 300; ++i)
//     // {
//     //     buf.WriteStringAndPush(str + std::to_string(i) + '\n');
//     // }

//     // std::optional<std::string> tmp;
//     // while (buf.ReadAbleSize())
//     // {
//     //     tmp = buf.ReadAsStringAndPop(buf.ReadAbleSize());
//     //     if (tmp)
//     //     {
//     //         std::cout << tmp.value() << std::endl;
//     //     }
//     // }
//     // std::cout << buf.ReadAbleSize() << std::endl;

//     return 0;
// }
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test5: 测试 SimpleLog
// #include "SimpleLog.hpp"
// int main(){
//     Info_Log("%s", "Hello World!");
//     Debug_Log("%s", "Hello World!");
//     Error_Log("%s", "Hello World!");

//     return 0;
// }
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test6: 测试 HttpServer
#include "../project/Server.hpp"
#include "../project/Http.hpp"

#define WWWROOT "./"
std::string RequestStr(const HttpRequest &req) {
    std::stringstream ss;
    ss << req._method << " " << req._path << " " << req._version << "\r\n";
    for (auto &it : req._params) {
        ss << it.first << ": " << it.second << "\r\n";
    }
    for (auto &it : req._headers) {
        ss << it.first << ": " << it.second << "\r\n";
    }
    ss << "\r\n";
    ss << req._body;
    return ss.str();
}
void Hello(const HttpRequest &req, HttpResponse *rsp) 
{
    rsp->SetContent(RequestStr(req), "text/plain");
}
void Login(const HttpRequest &req, HttpResponse *rsp) 
{
    rsp->SetContent(RequestStr(req), "text/plain");
}
void PutFile(const HttpRequest &req, HttpResponse *rsp) 
{
    std::string pathname = WWWROOT + req._path;
    Util::WriteFile(pathname, req._body);
}
void DelFile(const HttpRequest &req, HttpResponse *rsp) 
{
    rsp->SetContent(RequestStr(req), "text/plain");
}
int main()
{
    HttpServer server(6423);
    server.SetThreadCount(3);
    server.SetBaseDirAndFile(WWWROOT);//设置静态资源根目录，告诉服务器有静态资源请求到来，需要到哪里去找资源文件
    server.Get("/hello", Hello);
    server.Post("/login", Login);
    server.Put("/1234.txt", PutFile);
    server.Delete("/1234.txt", DelFile);
    server.Start();
    return 0;
}