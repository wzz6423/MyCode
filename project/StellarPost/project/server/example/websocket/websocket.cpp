// C++
#include <print>
#include <string>
// websocket
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

// // 定义服务器 server 类型
// using Server_t = websocketpp::server<websocketpp::config::asio>;

// auto Open(websocketpp::connection_hdl hdl) -> void
// {
//     std::println("websocket 长连接建立成功!");
// }

// auto Message(Server_t* server, websocketpp::connection_hdl hdl, Server_t::message_ptr msg) -> void
// {
//     // 获取有效载荷数据, 进行业务处理
//     std::string body = msg->get_payload();

//     std::println("收到消息: {}", body);

//     // 响应客户端
//     // 获取通信连接
//     auto conn = server->get_con_from_hdl(hdl);
//     // 发送数据
//     conn->send("server back: " + body, websocketpp::frame::opcode::value::text);
// }

// auto Close(websocketpp::connection_hdl hdl) -> void
// {
//     std::println("websocket 长连接断开成功!");
// }

// auto main() -> int
// {
//     // 实例化服务器对象
//     Server_t server;

//     // 初始化日志输出 -- 关闭日志输出
//     server.set_access_channels(websocketpp::log::alevel::none);

//     // 初始化 ASIO 框架
//     server.init_asio();

//     // 设置 握手成功连接 & 消息处理 & 连接关闭 回调函数
//     server.set_open_handler(Open);

//     auto msgHdl = std::bind(Message, &server, std::placeholders::_1, std::placeholders::_2);
//     server.set_message_handler(msgHdl);

//     server.set_close_handler(Close);

//     // 启用地址重用
//     server.set_reuse_addr(true);

//     // 设置监听端口
//     server.listen(6423);

//     // 开始监听
//     server.start_accept();

//     // 启动服务器
//     server.run();

//     return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 定义服务器 server 类型
using Server_t = websocketpp::server<websocketpp::config::asio>;

auto Open(websocketpp::connection_hdl hdl) -> void
{
    std::println("websocket 长连接建立成功!");
}

// 此处我想设计为 & , 但是使用 std::bind 时引用会报模板错误, std::bind 只能用指针
// 编译器尝试构造 std::tuple 时失败，因为模板实例化复杂，且参数类型不匹配
auto Message(Server_t &server, websocketpp::connection_hdl hdl, Server_t::message_ptr msg) -> void
{
    // 获取有效载荷数据, 进行业务处理
    std::string body = msg->get_payload();

    std::println("收到消息: {}", body);

    // 响应客户端
    // 获取通信连接
    auto conn = server.get_con_from_hdl(hdl);
    // 发送数据
    conn->send("server back: " + body, websocketpp::frame::opcode::value::text);
}

auto Close(websocketpp::connection_hdl hdl) -> void
{
    std::println("websocket 长连接断开成功!");
}

auto main() -> int
{
    // 实例化服务器对象
    Server_t server;

    // 初始化日志输出 -- 关闭日志输出
    server.set_access_channels(websocketpp::log::alevel::none);

    // 初始化 ASIO 框架
    server.init_asio();

    // 设置 握手成功连接 & 消息处理 & 连接关闭 回调函数
    server.set_open_handler(Open);

    // 因此把 std::bind 更改为 lambda, 非常好用!
    // auto msgHdl = std::bind(Message, &server, std::placeholders::_1, std::placeholders::_2);
    auto msgHdl = [&server](auto &&weak_ptr, auto &&msg_ptr)
    {
        Message(server, std::forward<decltype(weak_ptr)>(weak_ptr),
                std::forward<decltype(msg_ptr)>(msg_ptr));
    };
    server.set_message_handler(msgHdl);

    server.set_close_handler(Close);

    // 启用地址重用
    server.set_reuse_addr(true);

    // 设置监听端口
    server.listen(6423);

    // 开始监听
    server.start_accept();

    // 启动服务器
    server.run();

    return 0;
}

// 具体原因:
// 错误的核心在于 std::bind 的机制与类型系统之间的冲突

/*
Message 函数签名：
    void Message(websocketpp::server<websocketpp::config::asio>& server,
                std::weak_ptr<void> weak_ptr,
                std::shared_ptr<websocketpp::message_buffer::message<...>> msg_ptr)
*/

// 当使用 std::bind(Message, server, ph::_1, ph::_2) 时, 编译器需要生成一个可调用对象，其内部结构大致相当于：

/*
    struct BindObject {
        using FuncType = void(*)(Server&, WeakPtr, MsgPtr);
        Server bound_server;  // 问题出在这里！
        std::_Placeholder<1> _1;
        std::_Placeholder<2> _2;

        void operator()(WeakPtr a, MsgPtr b) const {
            FuncType(bound_server, a, b);  // 尝试调用
        }
    };
*/

// 关键冲突点​​
// ​​服务器类型问题​​：
// websocketpp::server 是一个​​不可复制​​的类（这是WebSocket服务器对象的常见设计）
// 但 std::bind ​​试图复制​​服务器对象到它的绑定存储中
// 错误日志关键部分："对‘std::tuple::tuple(...)’的调用没有匹配的函数"

// 类型不匹配细节​​
// 在尝试构造内部元组时：

/*
// 编译器尝试构造这个元组
    std::tuple<
        websocketpp::server<...>,  // 需要复制构造 -> 失败！
        std::_Placeholder<1>,
        std::_Placeholder<2>
    >
*/

// 服务器对象不可复制 -> 没有可用的拷贝构造函数
// 占位符类型 std::_Placeholder<X> 是只声明未定义的辅助类型 -> 无法实例化
// 编译器尝试了47种不同的元组构造函数（全部失败）

// 核心问题总结​​
// ​​不可复制性​​：服务器对象禁用了拷贝构造/赋值运算符
// ​​占位符限制​​：std::placeholder 类型无法作为常规对象使用
// ​​引用语义缺失​​：std::bind 在存储参数时默认进行值拷贝，而不是引用语义

// 而使用 lambda 表达式

/*
    auto msgHdl = [&server](auto weak_ptr, auto msg_ptr) {
        Message(server, weak_ptr, msg_ptr);
    };

    // or

    [&server](auto&& weak_ptr, auto&& msg_ptr) {
        Message(server, std::forward<decltype(weak_ptr)>(weak_ptr),
                std::forward<decltype(msg_ptr)>(msg_ptr));
    };
*/

// Lambda的优势:
// ​​引用捕获​​：[&server] 直接捕获原对象的引用，无需复制
// ​​完美转发​​：使用 auto&& 和 std::forward 保持参数原始类型
// ​​避免中间存储​​：不会创建复杂的元组存储
// ​​编译器友好​​：更简单的语法生成更直接的调用代码