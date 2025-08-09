// 测试通过

#include <print>
#include <chrono>
#include <thread>
#include <gflags/gflags.h>
#include "../../../project/common/rabbitmq/rebbitmq.hpp"

DEFINE_string(user,"aaaaa","RabbitMQ user");
DEFINE_string(passwd,"aaaaa","RabbitMQ user password");
DEFINE_string(host,"127.0.0.1:5672","RabbitMQ host and port");
DEFINE_string(exchange, "test_exchange", "Exchange name");
DEFINE_string(queue, "test_queue", "Queue name");
DEFINE_string(routing_key, "test_routing_key", "Routing key");

auto Callback(const char *message, size_t size) -> void
{
    std::string msg;
    msg.assign(message, size);
    std::println("Received message: {}", msg);
}

auto main(int argc, char* argv[]) -> int
{
    google::ParseCommandLineFlags(&argc, &argv, true);

    stellar_post::MQClient::RabbitMQ rabbitMQ(FLAGS_user, FLAGS_passwd, FLAGS_host);
    rabbitMQ.DeclareComponents(FLAGS_exchange, FLAGS_queue, FLAGS_routing_key);
    rabbitMQ.Consume(FLAGS_queue, Callback);

    std::this_thread::sleep_for(std::chrono::seconds(60));

    return 0;
}