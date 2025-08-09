#include <string>
#include <print>
#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

auto CallBack(AMQP::TcpChannel &channel, const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) -> void
{
    // 构造消息
    std::string msg;
    msg.assign(message.body(), message.bodySize());

    // 打印消息内容
    std::println("Received message with deal: {}", msg);
    std::println("Received message with no deal: {}", message.body()); // 不进行截取会出现乱码 -- 不以 \0 作为结尾

    // 确认
    channel.ack(deliveryTag, 0);
}

auto main() -> int
{
    // 1.实例化底层网络通信框架的 IO 事件监控句柄
    // auto* loop = ev_default_loop(0); // -- 这两句代码等价
    auto *loop = EV_DEFAULT;

    // 2.实例化 libEVHandler 句柄 -- 将 AMQP 框架与事件循环关联起来
    AMQP::LibEvHandler handler(loop);

    // 3.实例化网络连接对象
    AMQP::Address address("amqp://user:passwd@127.0.0.1:5672/");
    // url 解析使用特殊字符 @ , 密码中使用到如 @ 的特殊字符需要进行 url 编码, 如 @ -> %40 (但是依旧可能失败)
    AMQP::TcpConnection connection(&handler, address);

    // 4.实例化信道对象
    AMQP::TcpChannel channel(&connection);

    // 5.声明交换机
    auto &exchange_deferred = channel.declareExchange("test-exchange", AMQP::ExchangeType::direct);
    exchange_deferred.onError([](const char *msg)
                              {std::println("{}, {}", "声明交换机 test-exchange 失败", msg); exit(1); });
    exchange_deferred.onSuccess([]()
                                { std::println("声明交换机 test-exchange 成功"); });

    // 6.声明队列
    auto &queue_deferred = channel.declareQueue("test-queue");
    queue_deferred.onError([](const char *msg)
                           {std::println("{}, {}", "声明队列 test-queue 失败", msg); exit(1); });
    queue_deferred.onSuccess([]()
                             { std::println("声明队列 test-queue 成功"); });

    // 7.针对交换机和队列进行绑定
    auto &binding_deferred = channel.bindQueue("test-exchange", "test-queue", "test-queue");
    binding_deferred.onError([](const char *msg)
                             { std::println("{}, {}", "绑定队列 test-queue 失败", msg); exit(1); });
    binding_deferred.onSuccess([]()
                               { std::println("绑定队列 test-queue 成功"); });

    // 8.订阅队列消息 -- 设置消息处理回调函数
    auto &consume_deferred = channel.consume("test-queue", "consume-tag");
    consume_deferred.onError([](const char *msg)
                             { std::println("{}, {}", "订阅队列 test-queue 失败", msg); exit(1); });
    consume_deferred.onReceived([&](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered)
                                { CallBack(channel, message, deliveryTag, redelivered); });

    // 9.启动底层网络通信框架 -- 开启 IO 事件监控循环
    ev_run(loop, 0);

    return 0;
}