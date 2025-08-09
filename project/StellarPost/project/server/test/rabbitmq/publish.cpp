// 测试通过

#include <print>
#include <gflags/gflags.h>
#include "../../../project/common/rabbitmq/rebbitmq.hpp"

DEFINE_string(user, "wzz", "RabbitMQ user");
DEFINE_string(passwd, "WZZx2006z04x23", "RabbitMQ user password");
DEFINE_string(host, "127.0.0.1:5672", "RabbitMQ host and port");
DEFINE_string(exchange, "test_exchange", "Exchange name");
DEFINE_string(queue, "test_queue", "Queue name");
DEFINE_string(routing_key, "test_routing_key", "Routing key");
DEFINE_string(message, "Hello RabbitMQ!", "Message to publish");

auto main(int argc, char *argv[]) -> int
{
    google::ParseCommandLineFlags(&argc, &argv, true);

    stellar_post::MQClient::RabbitMQ rabbitMQ(FLAGS_user, FLAGS_passwd, FLAGS_host);
    rabbitMQ.DeclareComponents(FLAGS_exchange, FLAGS_queue, FLAGS_routing_key);
    for (size_t i = 0; i < 10; ++i)
    {
        std::string message = FLAGS_message + ":" + std::to_string(i);
        bool ret = rabbitMQ.Publish(FLAGS_exchange, message,FLAGS_routing_key);
        if (!ret)
        {
            std::println("Failed to publish message: {}", message);
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(60));

    return 0;
}