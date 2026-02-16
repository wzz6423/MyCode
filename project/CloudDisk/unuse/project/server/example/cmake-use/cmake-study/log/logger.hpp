#pragma once

/*
    -lspdlog -lfmt -lpthread
*/

// C++
#include <string>
#include <memory>
#include <unordered_set>
#include <algorithm>
#include <source_location>
// C
#include <cctype>
// spdlog
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h> // 标准输出带颜色日志
#include <spdlog/sinks/basic_file_sink.h>    // 基础文件日志
#include <spdlog/sinks/rotating_file_sink.h> // 滚动文件日志
#include <spdlog/async.h>                    // 异步日志支持
#include <spdlog/async_logger.h>             // 异步日志器
#include <spdlog/details/thread_pool.h>      // 线程池支持
#include <fmt/core.h>                        // 格式化库

// 获取当前文件名
inline std::string FileName(const std::source_location &loc = std::source_location::current())
{
    return loc.file_name();
}

// 获取当前行号
inline size_t Line(const std::source_location &loc = std::source_location::current())
{
    return loc.line();
}

class Log
{
public:
    /*
        获取日志单例实例
        debug_mode     是否开启调试模式(默认关闭)
        level          日志级别(默认错误级别)
        logFilePath    日志文件路径(默认 "../runLogs/runLog.log")
        fmt            日志格式(默认 "[%^%Y-%m-%d %H:%M:%S.%f%$][%^%P%$][%^%t%$][%^%l%$] %v")
        logName        日志器名称(默认 "_logger")
        maxFileSize    单个日志文件最大大小(默认 50MB)
        maxFileCount   最大保留日志文件数(默认 3个)
        threadPoolSize 线程池队列大小(默认 8192)
        threadCount    线程池线程数量(默认 1个)
    */
    static Log &GetInstance(
        bool debug_mode = false,
        spdlog::level::level_enum level = spdlog::level::err,
        const std::string &logFilePath = "../runLogs/runLog.log",
        const std::string &fmt = "[%^%Y-%m-%d %H:%M:%S.%f%$][%^%P%$][%^%t%$][%^%l%$] %v",
        const std::string &logName = "_logger",
        const int64_t maxFileSize = 50 * 1024 * 1024, // 50MB
        const size_t maxFileCount = 3,
        const int64_t threadPoolSize = 8192,
        const int threadCount = 1)
    {
        static Log instance(debug_mode, level, logFilePath, fmt, logName,
                            maxFileSize, maxFileCount, threadPoolSize, threadCount);
        return instance;
    }

    /*
     日志记录操作符重载
     level 日志级别(不区分大小写)
     fmt   格式化字符串(编译期检查)
     args  可变参数列表
    */
    template <typename... Args>
    auto operator()(std::string level, const std::string &fileName,
                    const size_t line, fmt::format_string<Args...> fmt, Args &&...args) -> void
    {
        // 统一转换为大写便于比较
        level = ToUpper(level);

        std::string message = fmt::format(fmt, std::forward<Args>(args)...);

        // 处理不同日志级别
        if (level == "TRACE" || level == "TRA")
        {
            _logger->trace("[{}:{}]{}", fileName, line, message);
        }
        else if (level == "DEBUG" || level == "DEG")
        {
            _logger->debug("[{}:{}]{}", fileName, line, message);
        }
        else if (level == "INFO")
        {
            _logger->info("[{}:{}]{}", fileName, line, message);
        }
        else if (level == "WARN" || level == "WARNING")
        {
            _logger->warn("[{}:{}]{}", fileName, line, message);
        }
        else if (level == "ERROR" || level == "ERR")
        {
            _logger->error("[{}:{}]{}", fileName, line, message);
        }
        else if (level == "FATAL" || level == "FAT" || level == "CRITICAL" || level == "CRITI")
        {
            _logger->critical("[{}:{}]{}", fileName, line, message);
        }
        // 处理未知日志级别
        else
        {
            /* 注意：这里需要保持fmt的编译期检查特性
              使用fmt::format处理用户格式，避免格式字符串不匹配
            */
            _logger->warn("[{}:{}], Unknown log level '{}' with message: {}",
                          fileName,
                          line,
                          level,
                          message);
        }
    }

    // 不带文件名 & 行号版本
    template <typename... Args>
    auto dbg(std::string level, fmt::format_string<Args...> fmt, Args &&...args) -> void
    {
        // 统一转换为大写便于比较
        level = ToUpper(level);

        std::string message = fmt::format(fmt, std::forward<Args>(args)...);

        // 处理不同日志级别
        if (level == "TRACE" || level == "TRA")
        {
            _logger->trace("{}", message);
        }
        else if (level == "DEBUG" || level == "DEG")
        {
            _logger->debug("{}", message);
        }
        else if (level == "INFO")
        {
            _logger->info("{}", message);
        }
        else if (level == "WARN" || level == "WARNING")
        {
            _logger->warn("{}", message);
        }
        else if (level == "ERROR" || level == "ERR")
        {
            _logger->error("{}", message);
        }
        else if (level == "FATAL" || level == "FAT" || level == "CRITICAL" || level == "CRITI")
        {
            _logger->critical("{}", message);
        }
        // 处理未知日志级别
        else
        {
            /* 注意：这里需要保持fmt的编译期检查特性
              使用fmt::format处理用户格式，避免格式字符串不匹配
            */
            _logger->warn("Unknown log level '{}' with message: {}",
                          level,
                          message);
        }
    }

    // 手动刷新
    auto Flush() -> void
    {
        if (_logger)
        {
            _logger->flush();
        }
    }

    ~Log()
    {
        Flush(); // 确保析构时刷新日志 -- 但是到析构这一步, 可能线程已经被销毁, 无法成功 flush , 故建议手动 flush
    }
    // 禁止拷贝构造和赋值
    Log(const Log &) = delete;
    Log &operator=(const Log &) = delete;
    // 禁止移动构造和赋值
    Log(Log &&) = delete;
    Log &operator=(Log &&) = delete;

private:
    // 构造函数私有化
    Log(
        bool debug_mode = false,
        spdlog::level::level_enum level = spdlog::level::err,
        const std::string &logFilePath = "../runLogs/runLog.log",
        const std::string &fmt = "[%^%Y-%m-%d %H:%M:%S.%f%$][%^%P%$][%^%t%$][%^%l%$] %v",
        const std::string &logName = "_logger",
        const int64_t maxFileSize = 50 * 1024 * 1024, // 50MB
        const size_t maxFileCount = 3,
        const int64_t threadPoolSize = 8192,
        const int threadCount = 1)
    {
        // 调试模式：创建标准输出日志器，日志级别最低
        if (debug_mode)
        {
            // 创建带颜色输出的控制台日志器
            _logger = spdlog::stdout_color_mt(logName);
            _logger->set_level(spdlog::level::trace); // 设置输出最低级别
            _logger->flush_on(spdlog::level::trace);  // 立即刷新日志
        }
        // 发布模式：创建滚动文件日志器
        else
        {
            /*
             创建异步滚动文件日志器
              logName      日志器名称
              logFilePath  日志文件路径
              maxFileSize  单个文件最大大小(50MB)
              maxFileCount 最大保留文件数(3个)
            */
            // 只初始化一次线程池
            InitThreadPoolOnce(threadPoolSize, threadCount);

            // 创建异步滚动文件日志器
            _logger = spdlog::rotating_logger_mt<spdlog::async_factory>(
                logName,
                logFilePath,
                maxFileSize,
                maxFileCount);
            _logger->set_level(level); // 设置用户指定日志级别
            _logger->flush_on(level);  // 设置对应刷新级别
        }

        // 设置统一日志格式
        _logger->set_pattern(fmt);
    }

    // 只初始化一次线程池
    static auto InitThreadPoolOnce(size_t queue_size, size_t thread_count) -> void
    {
        static bool inited = false;
        if (!inited)
        {
            spdlog::init_thread_pool(queue_size, thread_count);
            inited = true;
        }
    }

    // 将字符串转换为全大写
    auto ToUpper(const std::string &str) -> std::string
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c)
                       { return std::toupper(c); });
        return result;
    }

    // spdlog日志器实例
    std::shared_ptr<spdlog::logger> _logger;
};

static Log &lg = Log::GetInstance(true, spdlog::level::trace); // for debug