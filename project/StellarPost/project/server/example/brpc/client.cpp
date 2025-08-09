// c++
#include <thread>
#include <chrono>
#include <memory>
// brpc
#include <brpc/channel.h>
#include "brpc.pb.h"

void callBack(brpc::Controller *cntl, brpc::EchoResponse *resp)
{
    std::unique_ptr<brpc::Controller> controller(cntl);
    std::unique_ptr<brpc::EchoResponse> response(resp);

    if (controller->Failed()) // 检查 rpc 调用是否失败
    {
        std::cerr << "RPC 调用失败: " << controller->ErrorText() << std::endl;
        return;
    }
    // 处理回调函数, 这里可以添加业务逻辑
    std::cout << "收到响应: " << response->message() << std::endl;
    
    // delete controller; // 删除控制器对象, 释放资源
    // delete response;   // 删除响应对象, 释放资源
}

auto main(int argc, char *argv[]) -> int
{
    // 构造 channel 信道连接服务器
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.connect_timeout_ms = -1;                    // 设置连接超时时间为 -1 毫秒, 连接超时一直等待
    options.timeout_ms = -1;                            // 设置 rpc 请求超时时间为 -1 毫秒, 请求超时一直等待
    options.max_retry = 3;                              // 设置最大重试次数为 3 次, 超过次数后返回错误
    options.protocol = "baidu_std";                     // 设置序列化协议为 baidu_std, 这是 brpc 默认的协议
    int ret = channel.Init("127.0.0.1:8000", &options); // 初始化 channel, 连接到服务器地址
    if (ret == -1)
    {
        std::cerr << "初始化信道失败!" << std::endl;
        return -1;
    }

    // 构造 EchoService_Stub 对象, 用于进行 rpc 调用
    brpc::EchoService_Stub stub(&channel);

    // 进行 rpc 调用, 获取响应
    brpc::EchoRequest request;
    brpc::EchoResponse *response = new brpc::EchoResponse(); // 创建响应对象, 用于接收服务器响应
    request.set_message("Hello World!");                     // 设置请求消息内容

    // new 出 EchoResponse & RpcController -- 没有等待, 如果是在栈上开辟空间, 对象销毁会有错误
    brpc::Controller *controller = new brpc::Controller(); // 创建 rpc 控制器 / 上下文, 用于控制 rpc 调用

    // 异步
    auto clusure = google::protobuf::NewCallback(callBack, controller, response); // 创建回调函数, 用于处理 rpc 响应
    stub.Echo(controller, &request, response, clusure);                           // 异步调用 Echo 方法, 异步请求最后一个 Closure 需要设置

    // 同步
    // stub.Echo(controller, &request, response, nullptr); // 同步调用 Echo 方法, 异步请求最后一个 Closure 需要设置
    // if (controller->Failed()) // 检查 rpc 调用是否失败
    // {
    //     std::cerr << "RPC 调用失败: " << controller->ErrorText() << std::endl;
    //     return -1;
    // }
    // delete controller; // 删除控制器对象, 释放资源
    // delete response;   // 删除响应对象, 释放资源

    // 输出响应结果
    std::cout << "RPC 响应: " << response->message() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(5)); // 等待 5 秒, 确保异步回调函数执行完毕

    return 0;
}
