#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h> // for stdout_color_mt
// #include <spdlog/sinks/basic_file_sink.h> // for basic_logger_mt
// #include <spdlog/sinks/rotating_file_sink.h> // for rotating_logger_mt
// #include <spdlog/async.h> // for async_logger_mt
// #include <spdlog/async_logger.h> // for async_logger_mt
// #include <spdlog/pattern_formatter.h> // for pattern_formatter
// #include <spdlog/details/null_mutex.h> // for null_mutex
// #include <spdlog/details/registry.h> // for registry
// #include <spdlog/details/log_msg.h> // for log_msg
// #include <spdlog/details/log_msg_buffer.h> // for log_msg_buffer
// #include <spdlog/details/thread_pool.h> // for thread_pool
// #include <spdlog/details/os.h> // for os

auto main() -> int {
    // 设置全局的刷新策略
    spdlog::flush_every(std::chrono::seconds(1)); // 设置全局日志刷新策略为每秒刷新一次
    spdlog::flush_on(spdlog::level::info); // 设置全局日志刷新策略为输出等级为info及以上的日志时刷新

    // 设置全局日志默认输出等级(每个日志器可以独立设置)
    spdlog::set_level(spdlog::level::debug); // 设置全局日志输出等级为debug
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [thread %t] %v"); // 设置全局日志输出格式
    // 设置全局日志输出格式为: [2023-10-01 12:00:00.000] [info] [thread 12345] Hello, World!

    // 创建同步日志器
    auto logger = spdlog::stdout_color_mt("default-logger"); // 创建一个名为"default-logger"的彩色控制台日志器
    // auto logger = spdlog::basic_logger_mt("default-logger", "./default.log"); // 创建一个名为"default-logger"的日志器, 输出到文件"./default.log"

    // 初始化异步日志线程
    // spdlog::init_thread_pool(1024, 1); // 初始化异步日志线程池, 1024个缓冲区, 1个线程

    // 创建异步日志器
    // auto logger = spdlog::stdout_color_mt<spdlog::async_factory>("default-logger"); // 创建一个名为"default-logger"的彩色控制台异步日志器
    // auto logger = spdlog::basic_logger_mt<spdlog::async_factory>("default-logger", "./default.log"); // 创建一个名为"default-logger"的异步日志器, 输出到文件"./default.log"

    // 设置日志器刷新策略, 设置日志器输出等级
    logger->flush_on(spdlog::level::info); // 设置日志器刷新策略为输出等级为info及以上的日志时刷新
    logger->set_level(spdlog::level::debug); // 设置日志器输出等级为debug
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [thread %t] %v"); // 设置日志器输出格式
    // 设置日志器输出格式为: [2023-10-01 12:00:00.000] [info] [thread 12345] Hello, World!

    // 输出日志
    logger->info("Hello, World!"); // 输出一条info级别的日志
    logger->debug("This is a debug message"); // 输出一条debug级别的日志
    logger->warn("This is a warning message"); // 输出一条warning级别的日志
    logger->error("This is an error message"); // 输出一条error级别的日志
    logger->critical("This is a critical message"); // 输出一条critical级别的日志
    logger->trace("This is a trace message"); // 输出一条trace级别的日志

    // // 关闭日志器
    // spdlog::drop("default-logger"); // 删除名为"default-logger"的日志器
    // // 关闭全局日志器
    // spdlog::shutdown(); // 关闭全局日志器
    // 关闭所有日志器
    spdlog::drop_all(); // 删除所有日志器
    // 关闭所有日志器的刷新策略
    spdlog::flush_every(std::chrono::seconds(0)); // 重置所有日志器的刷新策略为不刷新
    spdlog::flush_on(spdlog::level::off); // 重置所有日志器的刷新策略为不刷新
    spdlog::set_level(spdlog::level::off); // 重置所有日志器的输出等级为off
    spdlog::set_pattern(""); // 重置所有日志器的输出格式为空

    return 0;
}