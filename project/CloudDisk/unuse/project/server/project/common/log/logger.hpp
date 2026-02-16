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
#include <format>                            // 格式化库

namespace cloud_disk::Log
{
    // 获取当前文件名
    inline auto FileName(const std::source_location &loc = std::source_location::current()) -> std::string
    {
        return loc.file_name();
    }

    // 获取当前行号
    inline auto Line(const std::source_location &loc = std::source_location::current()) -> size_t
    {
        return loc.line();
    }

    // 日志系统类
    class Log
    {
    public:
        // 获取日志单例实例
        // @param debug_mode 是否开启调试模式(默认关闭)
        // @param level 日志级别(默认错误级别)
        // @param log_file_path 日志文件路径(默认 "../runLogs/runLog.log")
        // @param format 日志格式(默认 "[%^%Y-%m-%d %H:%M:%S.%f%$][%^%P%$][%^%t%$][%^%l%$] %v")
        // @param logger_name 日志器名称(默认 "_logger")
        // @param max_file_size 单个日志文件最大大小(默认 50MB)
        // @param max_file_count 最大保留日志文件数(默认 3个)
        // @param thread_pool_size 线程池队列大小(默认 8192)
        // @param thread_count 线程池线程数量(默认 1个)
        static auto GetInstance(
            bool debug_mode = false,
            spdlog::level::level_enum level = spdlog::level::err,
            const std::string &log_file_path = "../runLogs/runLog.log",
            const std::string &format = "[%^%Y-%m-%d %H:%M:%S.%f%$][%^%P%$][%^%t%$][%^%l%$] %v",
            const std::string &logger_name = "_logger",
            int64_t max_file_size = 50 * 1024 * 1024, // 50MB
            size_t max_file_count = 3,
            int64_t thread_pool_size = 8192,
            int thread_count = 1) -> Log &
        {
            static Log instance(debug_mode, level, log_file_path, format, logger_name,
                                max_file_size, max_file_count, thread_pool_size, thread_count);
            return instance;
        }

        // 记录日志
        // @param level 日志级别(不区分大小写)
        // @param file_name 文件名
        // @param line 行号
        // @param fmt 格式化字符串(编译期检查)
        // @param args 可变参数列表
        template <typename... Args>
        auto operator()(std::string level,
                        const std::string &file_name,
                        size_t line,
                        fmt::format_string<Args...> fmt,
                        Args &&...args) -> void
        {
            // 统一转换为大写便于比较
            level = toUpper(level);

            // 格式化日志消息
            std::string message = fmt::format(fmt, std::forward<Args>(args)...);
            std::string full_message = fmt::format("[{}:{}] {}", file_name, line, message);

            // 根据日志级别记录
            if (level == "TRACE" || level == "TRA")
            {
                _logger->trace(full_message);
            }
            else if (level == "DEBUG" || level == "DEG")
            {
                _logger->debug(full_message);
            }
            else if (level == "INFO")
            {
                _logger->info(full_message);
            }
            else if (level == "WARN" || level == "WARNING")
            {
                _logger->warn(full_message);
            }
            else if (level == "ERROR" || level == "ERR")
            {
                _logger->error(full_message);
            }
            else if (level == "FATAL" || level == "FAT" || level == "CRITICAL" || level == "CRITI")
            {
                _logger->critical(full_message);
            }
            else [[unlikely]]
            {
                // 未知日志级别警告
                _logger->warn("[{}:{}] Unknown log level '{}' with message: {}",
                              file_name, line, level, message);
            }
        }

        // 手动刷新日志
        auto Flush() -> void
        {
            if (_logger) [[likely]]
            {
                _logger->flush();
            }
        }

        // 析构函数 - 确保刷新日志
        ~Log()
        {
            Flush();
        }

        // 禁止拷贝和移动
        Log(const Log &) = delete;
        Log &operator=(const Log &) = delete;
        Log(Log &&) = delete;
        Log &operator=(Log &&) = delete;

    private:
        // 私有构造函数
        Log(bool debug_mode = false,
            spdlog::level::level_enum level = spdlog::level::err,
            const std::string &log_file_path = "../runLogs/runLog.log",
            const std::string &format = "[%^%Y-%m-%d %H:%M:%S.%f%$][%^%P%$][%^%t%$][%^%l%$] %v",
            const std::string &logger_name = "_logger",
            int64_t max_file_size = 50 * 1024 * 1024, // 50MB
            size_t max_file_count = 3,
            int64_t thread_pool_size = 8192,
            int thread_count = 1)
        {
            // 初始化日志系统
            initializeLogger(debug_mode, level, log_file_path, format, logger_name,
                             max_file_size, max_file_count, thread_pool_size, thread_count);
        }

        // 初始化日志器
        void initializeLogger(
            bool debug_mode,
            spdlog::level::level_enum level,
            const std::string &log_file_path,
            const std::string &format,
            const std::string &logger_name,
            int64_t max_file_size,
            size_t max_file_count,
            int64_t thread_pool_size,
            int thread_count)
        {
            if (debug_mode)
            {
                // 调试模式：创建带颜色的控制台日志器
                _logger = spdlog::stdout_color_mt(logger_name);
                _logger->set_level(spdlog::level::trace);
                _logger->flush_on(spdlog::level::trace);
            }
            else
            {
                // 发布模式：创建异步滚动文件日志器
                initializeThreadPool(thread_pool_size, thread_count);
                _logger = spdlog::rotating_logger_mt<spdlog::async_factory>(
                    logger_name,
                    log_file_path,
                    max_file_size,
                    max_file_count);
                _logger->set_level(level);
                _logger->flush_on(level);
            }

            // 设置日志格式
            _logger->set_pattern(format);
        }

        // 初始化线程池
        static void initializeThreadPool(size_t queue_size, size_t thread_count)
        {
            static std::once_flag init_flag;
            std::call_once(init_flag, [queue_size, thread_count]
                           { spdlog::init_thread_pool(queue_size, thread_count); });
        }

        // 将字符串转换为全大写
        auto toUpper(const std::string &str) -> std::string
        {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(),
                           [](unsigned char c)
                           { return std::toupper(c); });
            return result;
        }

    private:
        std::shared_ptr<spdlog::logger> _logger; // spdlog日志器实例
    };

    // static Log &lg = cloud_disk::Log::Log::GetInstance(); // project use
    static Log &lg = cloud_disk::Log::Log::GetInstance(true, spdlog::level::trace); // for debug
}

// 日志宏定义
#define TRACE(format, ...) cloud_disk::Log::lg("TRACE", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), format, ##__VA_ARGS__)
#define DEBUG(format, ...) cloud_disk::Log::lg("DEBUG", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), format, ##__VA_ARGS__)
#define INFO(format, ...) cloud_disk::Log::lg("INFO", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), format, ##__VA_ARGS__)
#define WARNING(format, ...) cloud_disk::Log::lg("WARNING", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), format, ##__VA_ARGS__)
#define ERROR(format, ...) cloud_disk::Log::lg("ERROR", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), format, ##__VA_ARGS__)
#define FATAL(format, ...) cloud_disk::Log::lg("FATAL", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), format, ##__VA_ARGS__)