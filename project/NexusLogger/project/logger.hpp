/*
    日志器模块:
        1.实现抽象基类
        2.实现同步日志器子类
        3.实现异步日志器子类
*/

#pragma once

// C++
#include <algorithm>
#include <string>
#include <unordered_map>
#include <map>
#include <mutex>
#include <atomic>
#include <memory>
// C
#include <cctype>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <cstdarg>
// Other
#include "util.hpp"
#include "level.hpp"
#include "format.hpp"
#include "logsink.hpp"
#include "lopper.hpp"

namespace Log
{
    // 构建日志等级字符串与日志等级的 map 映射
    static const std::map<std::string, LogLevel::Level> _str_to_lev = {{"UNKNOW", LogLevel::Level::UNKNOW},
                                                                       {"DEGBUG", LogLevel::Level::DEBUG},
                                                                       {"INFO", LogLevel::Level::INFO},
                                                                       {"WARN", LogLevel::Level::WARNING},
                                                                       {"WARNING", LogLevel::Level::WARNING},
                                                                       {"ERROR", LogLevel::Level::ERROR},
                                                                       {"FATAL", LogLevel::Level::FATAL}};

    // 抽象日志器基类
    class Logger
    {
    public:
        using loggerPtr = std::shared_ptr<Logger>;

        Logger(const std::string &logger_name, LogLevel::Level level, Formatter::formatterPtr &formatter, const std::vector<LogSink::sinkPtr> &sinks)
            : _logger_name(logger_name),
              _limit_level(level),
              _formatter(formatter),
              _sinks(sinks)
        {
        }

        // 不能使用指针, 只能使用对象
        void operator()(std::string level, const std::string &filename, const size_t line, const std::string &fmt, ...)
        {
            level = Util::ToUpper(level);

            // 判断当前日志等级是否达到输出等级, 达到了进行组织输出, 未达到直接返回
            // 对 fmt 格式化字符串和不定参进行字符串组织, 得到日志消息字符串
            if ((level == "DEBUG" && LogLevel::Level::DEBUG < _limit_level) ||
                (level == "INFO" && LogLevel::Level::INFO < _limit_level) ||
                ((level == "WARNING" || level == "WARN") && LogLevel::Level::WARNING < _limit_level) ||
                (level == "ERROR" && LogLevel::Level::ERROR < _limit_level) ||
                (level == "FATAL" && LogLevel::Level::FATAL < _limit_level))
            {
                return;
            }
            else
            {
                // 将格式化字符串与可变参数进行匹配
                va_list ptostr;
                va_start(ptostr, fmt);
                char *res;
                int ret = vasprintf(&res, fmt.c_str(), ptostr);
                if (ret == -1)
                {
                    std::cerr << "vasprintf 失败..." << std::endl;
                    return;
                }
                va_end(ptostr);                // 指针置空
                std::string payload(res, ret); // 构造有效载荷
                free(res);                     // 置空指针

                Serialize(_str_to_lev.at(level), filename, line, payload);
            }
        }

        // 指针、对象均可使用
        void Lg(std::string level, const std::string &filename, const size_t line, const std::string &fmt, ...)
        {
            level = Util::ToUpper(level);

            // 判断当前日志等级是否达到输出等级, 达到了进行组织输出, 未达到直接返回
            // 对 fmt 格式化字符串和不定参进行字符串组织, 得到日志消息字符串
            if ((level == "DEBUG" && LogLevel::Level::DEBUG < _limit_level) ||
                (level == "INFO" && LogLevel::Level::INFO < _limit_level) ||
                ((level == "WARNING" || level == "WARN") && LogLevel::Level::WARNING < _limit_level) ||
                (level == "ERROR" && LogLevel::Level::ERROR < _limit_level) ||
                (level == "FATAL" && LogLevel::Level::FATAL < _limit_level))
            {
                return;
            }
            else
            {
                // 将格式化字符串与可变参数进行匹配
                va_list ptostr;
                va_start(ptostr, fmt);
                char *res;
                int ret = vasprintf(&res, fmt.c_str(), ptostr);
                if (ret == -1)
                {
                    std::cerr << "vasprintf 失败..." << std::endl;
                    return;
                }
                va_end(ptostr);                // 指针置空
                std::string payload(res, ret); // 构造有效载荷
                free(res);                     // 置空指针

                Serialize(_str_to_lev.at(level), filename, line, payload);
            }
        }
        void Unknow(const std::string &filename, const size_t line, const std::string &fmt, ...)
        {
            // 将格式化字符串与可变参数进行匹配
            va_list ptostr;
            va_start(ptostr, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ptostr);
            if (ret == -1)
            {
                std::cerr << "vasprintf 失败..." << std::endl;
                return;
            }
            va_end(ptostr);                // 指针置空
            std::string payload(res, ret); // 构造有效载荷
            free(res);                     // 置空指针

            Serialize(LogLevel::Level::UNKNOW, filename, line, payload);
        }
        void Debug(const std::string &filename, const size_t line, const std::string &fmt, ...)
        {
            // 判断当前日志等级是否达到输出等级, 达到了进行组织输出, 未达到直接返回
            // 对 fmt 格式化字符串和不定参进行字符串组织, 得到日志消息字符串
            if (LogLevel::Level::DEBUG < _limit_level)
            {
                return;
            }
            else
            {
                // 将格式化字符串与可变参数进行匹配
                va_list ptostr;
                va_start(ptostr, fmt);
                char *res;
                int ret = vasprintf(&res, fmt.c_str(), ptostr);
                if (ret == -1)
                {
                    std::cerr << "vasprintf 失败..." << std::endl;
                    return;
                }
                va_end(ptostr);                // 指针置空
                std::string payload(res, ret); // 构造有效载荷
                free(res);                     // 置空指针

                Serialize(LogLevel::Level::DEBUG, filename, line, payload);
            }
        }
        void Info(const std::string &filename, const size_t line, const std::string &fmt, ...)
        {
            // 判断当前日志等级是否达到输出等级, 达到了进行组织输出, 未达到直接返回
            // 对 fmt 格式化字符串和不定参进行字符串组织, 得到日志消息字符串
            if (LogLevel::Level::INFO < _limit_level)
            {
                return;
            }
            else
            {
                // 将格式化字符串与可变参数进行匹配
                va_list ptostr;
                va_start(ptostr, fmt);
                char *res;
                int ret = vasprintf(&res, fmt.c_str(), ptostr);
                if (ret == -1)
                {
                    std::cerr << "vasprintf 失败..." << std::endl;
                    return;
                }
                va_end(ptostr);                // 指针置空
                std::string payload(res, ret); // 构造有效载荷
                free(res);                     // 置空指针

                Serialize(LogLevel::Level::INFO, filename, line, payload);
            }
        }
        void Warning(const std::string &filename, const size_t line, const std::string &fmt, ...)
        {
            // 判断当前日志等级是否达到输出等级, 达到了进行组织输出, 未达到直接返回
            // 对 fmt 格式化字符串和不定参进行字符串组织, 得到日志消息字符串
            if (LogLevel::Level::WARNING < _limit_level)
            {
                return;
            }
            else
            {
                // 将格式化字符串与可变参数进行匹配
                va_list ptostr;
                va_start(ptostr, fmt);
                char *res;
                int ret = vasprintf(&res, fmt.c_str(), ptostr);
                if (ret == -1)
                {
                    std::cerr << "vasprintf 失败..." << std::endl;
                    return;
                }
                va_end(ptostr);                // 指针置空
                std::string payload(res, ret); // 构造有效载荷
                free(res);                     // 置空指针

                Serialize(LogLevel::Level::WARNING, filename, line, payload);
            }
        }
        void Error(const std::string &filename, const size_t line, const std::string &fmt, ...)
        {
            // 判断当前日志等级是否达到输出等级, 达到了进行组织输出, 未达到直接返回
            // 对 fmt 格式化字符串和不定参进行字符串组织, 得到日志消息字符串
            if (LogLevel::Level::ERROR < _limit_level)
            {
                return;
            }
            else
            {
                // 将格式化字符串与可变参数进行匹配
                va_list ptostr;
                va_start(ptostr, fmt);
                char *res;
                int ret = vasprintf(&res, fmt.c_str(), ptostr);
                if (ret == -1)
                {
                    std::cerr << "vasprintf 失败..." << std::endl;
                    return;
                }
                va_end(ptostr);                // 指针置空
                std::string payload(res, ret); // 构造有效载荷
                free(res);                     // 置空指针

                Serialize(LogLevel::Level::ERROR, filename, line, payload);
            }
        }
        void Fatal(const std::string &filename, const size_t line, const std::string &fmt, ...)
        {
            // 判断当前日志等级是否达到输出等级, 达到了进行组织输出, 未达到直接返回
            // 对 fmt 格式化字符串和不定参进行字符串组织, 得到日志消息字符串
            if (LogLevel::Level::FATAL < _limit_level)
            {
                return;
            }
            else
            {
                // 将格式化字符串与可变参数进行匹配
                va_list ptostr;
                va_start(ptostr, fmt);
                char *res;
                int ret = vasprintf(&res, fmt.c_str(), ptostr);
                if (ret == -1)
                {
                    std::cerr << "vasprintf 失败..." << std::endl;
                    return;
                }
                va_end(ptostr);                // 指针置空
                std::string payload(res, ret); // 构造有效载荷
                free(res);                     // 置空指针

                Serialize(LogLevel::Level::FATAL, filename, line, payload);
            }
        }

        // 获取日志器名称
        const std::string &GetLoggerName()
        {
            return _logger_name;
        }

    protected:
        // 构造日志消息对象, 进行格式化得到格式化的字符串, 进行落地输出
        // 通过传入的参数构造出日志消息对象, 进行日志格式化并落地
        void Serialize(LogLevel::Level level, const std::string &filename, const size_t line, const std::string &payload)
        {
            // 构造 LogMessage 对象
            LogMessage msg(level, filename, line, _logger_name, payload);

            // 通过格式化工具对 LogMessage 进行格式化, 得到格式化日志消息字符串
            std::string log = _formatter->Format(msg);

            // 进行日志落地
            Log(log);
        }

    protected:
        // 抽象接口完成实际的落地输出, 不同的日志器有不同的落地方式
        virtual void Log(const char *data, size_t len) = 0;
        virtual void Log(const std::string &log) final
        {
            Log(log.c_str(), log.size());
        }

    protected:
        std::mutex _mtx;                           // 多线程操作临界资源需加锁
        std::string _logger_name;                  // 日志器名称
        std::atomic<LogLevel::Level> _limit_level; // 限制日志输出等级
        Formatter::formatterPtr _formatter;        // 管理日志格式化类
        std::vector<LogSink::sinkPtr> _sinks;      // 一个日志器可以向不同落地方向写日志
    };

    // 同步日志器 -- 将日志直接通过落地模块句柄进行落地
    class SyncLogger : public Logger
    {
    public:
        SyncLogger(const std::string &logger_name, LogLevel::Level level, Formatter::formatterPtr &formatter, const std::vector<LogSink::sinkPtr> &sinks)
            : Logger(logger_name, level, formatter, sinks)
        {
        }

    protected:
        void Log(const char *data, size_t len)
        {
            {
                std::unique_lock<std::mutex> lock(_mtx);
                if (!_sinks.empty())
                {
                    for (auto &sink : _sinks)
                    {
                        sink->Log(data, len);
                    }
                }
            }
        }
    };

    // 异步日志器 -- 将日志直接通过落地模块句柄进行落地
    class ASyncLogger : public Logger
    {
    public:
        ASyncLogger(const std::string &logger_name, LogLevel::Level level, Formatter::formatterPtr &formatter, const std::vector<LogSink::sinkPtr> &sinks,
                    bool enhance = false, DefaultBufferSize defaultbuffersize = DefaultBufferSize::MB_50)
            : Logger(logger_name, level, formatter, sinks),
              _looper(std::make_shared<ASyncLooper>(std::bind(&ASyncLogger::RealLog, this, std::placeholders::_1), enhance, defaultbuffersize))
        {
        }

    protected:
        // 将数据写入缓冲区
        void Log(const char *data, size_t len)
        {
            _looper->Push(data, len);
        }

        // 设计一个实际将缓冲区中的数据进行落地的函数(lopper 的 _callback)
        void RealLog(Buffer &buf)
        {
            // 无需加锁保护, 内部是串行化的线程安全的
            if (!_sinks.empty())
            {
                for (auto &sink : _sinks)
                {
                    sink->Log(buf.Begin(), buf.ReadableSize());
                }
            }
        }

    private:
        ASyncLooper::asynclooperPtr _looper;
    };

    // 建造者模式来建造日志器而非直接构造日志器, 减轻使用负担
    // 抽象建造者类(日志器对象所需零部件的构建 & 日志器的构建) -- 设置日志器类型, 将不同类型日志器的创建放在同一个日志器建造者类中完成
    // 日志器类型
    enum class LoggerType
    {
        SYNC_LOGGER = 0,
        ASYNC_LOGGER
    };

    class LoggerBuilder
    {
    public:
        LoggerBuilder()
            : _async_nature({false, DefaultBufferSize::MB_50}),
              _logger_type(LoggerType::SYNC_LOGGER),
              _limit_level(LogLevel::Level::DEBUG)
        {
        }

        // 异步日志器缓冲区属性
        void BuildASyncNature(std::pair<bool, DefaultBufferSize> async_nature)
        {
            _async_nature = std::move(async_nature);
        }

        // 日志器类型
        void BuildType(LoggerType logger_type)
        {
            _logger_type = logger_type;
        }

        // 日志器名称
        void BuildName(const std::string &logger_name)
        {
            _logger_name = logger_name;
        }

        // 日志等级
        void BuildLevel(LogLevel::Level level)
        {
            _limit_level = level;
        }
        void BuildLevel(std::string level)
        {
            level = Util::ToUpper(level);

            _limit_level = _str_to_lev.at(level);
        }

        // 日志格式
        void BuildFormatter(const std::string &pattern = "[%D{%Y-%m-%d %H:%M:S}][%t][%c][%f:%l][%p]%T%m%n")
        {
            _formatter = std::make_shared<Formatter>(pattern);
        }

        // 日志落地
        template <typename SinkType, typename... Args>
        void BuildSink(Args &&...args)
        {
            LogSink::sinkPtr psink = SinkFactory::Create<SinkType>(std::forward<Args>(args)...);
            _sinks.push_back(psink);
        }

        // 日志器建造
        virtual Logger::loggerPtr Build() = 0;

    protected:
        std::pair<bool, DefaultBufferSize> _async_nature; // 若为异步日志器, 则需要设置缓冲区相关信息
        LoggerType _logger_type;                          // 日志器类型
        std::string _logger_name;                         // 日志器名称
        LogLevel::Level _limit_level;                     // 限制日志输出等级
        Formatter::formatterPtr _formatter;               // 管理日志格式化类
        std::vector<LogSink::sinkPtr> _sinks;             // 一个日志器可以向不同落地方向写日志
    };

    // 派生出具体建造者类 -- 局部
    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::loggerPtr Build() override
        {
            assert(_logger_name.empty() == false);

            if (_formatter.get() == nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }

            if (_sinks.empty())
            {
                BuildSink<StdoutSink>();
            }

            if (_logger_type == LoggerType::SYNC_LOGGER)
            {
                return std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
            }
            else
            {
                return std::make_shared<ASyncLogger>(_logger_name, _limit_level, _formatter, _sinks, _async_nature.first, _async_nature.second);
            }
        }
    };

    // 日志器管理器
    class LoggerManager
    {
    public:
        // 获取单例
        static LoggerManager &GetInstance()
        {
            static LoggerManager eton; // C++11 后线程安全
            return eton;
        }

        // 添加日志器
        bool Add(const Logger::loggerPtr &logger)
        {
            if (!GetLogger(logger->GetLoggerName()))
            {
                // 不能把加锁放在 if 前面, 否则会导致死锁 -- 放在此处还减轻了锁粒度
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    _loggers.insert({logger->GetLoggerName(), logger});
                }
                return true;
            }
            else
            {
                return false;
            }
        }

        // 判断是否拥有某个logger, 拥有则返回该 logger
        std::optional<const Logger::loggerPtr> GetLogger(const std::string &name)
        {
            {
                std::unique_lock<std::mutex> lock(_mtx);
                std::unordered_map<std::string, Logger::loggerPtr>::const_iterator it = _loggers.find(name);
                if (it == _loggers.end())
                {
                    return std::nullopt;
                }
                else
                {
                    return it->second;
                }
            }
        }

        // 获取默认日志器
        const Logger::loggerPtr &RootLogger()
        {
            return _root_logger;
        }

        ~LoggerManager() = default;

    private:
        LoggerManager()
        {
            // 不能使用全局建造者, 全局建造者要使用 GetInstance , 但第一次实例化时要调用构造, 陷入死循环 / 无法获得单例阻塞
            std::unique_ptr<LoggerBuilder> builder = std::make_unique<LocalLoggerBuilder>();
            builder->BuildName("root");
            _root_logger = builder->Build();
            _loggers.insert(std::make_pair("root", _root_logger));
        }

    private:
        std::mutex _mtx;                                             // 互斥锁, 多线程环境下访问不同日志器保证线程安全
        Logger::loggerPtr _root_logger;                              // 默认日志器
        std::unordered_map<std::string, Logger::loggerPtr> _loggers; // 管理所有日志器的哈希桶
    };

    // 全局(单例)日志器建造者 -- 将日志器添加到单例对象中
    class GlobalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::loggerPtr Build() override
        {
            assert(_logger_name.empty() == false);

            if (_formatter.get() == nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }

            if (_sinks.empty())
            {
                BuildSink<StdoutSink>();
            }

            Logger::loggerPtr newLogger;
            if (_logger_type == LoggerType::SYNC_LOGGER)
            {
                newLogger = std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
            }
            else
            {
                newLogger = std::make_shared<ASyncLogger>(_logger_name, _limit_level, _formatter, _sinks, _async_nature.first, _async_nature.second);
            }
            LoggerManager::GetInstance().Add(newLogger);
            return newLogger;
        }
    };
}