/*
    定义日志消息类, 进行日志中间信息的存储:
    1. 日志输出时间
    2. 日志等级
    3. 源文件名称
    4. 源文件行号
    5. 线程ID
    6. 日志内容
    7. 日志器名称
*/

#pragma once

// C++
#include <string>
#include <thread>
// Other
#include "util.hpp"
#include "level.hpp"

namespace Log
{
    struct LogMessage
    {
    public:
        LogMessage(LogLevel::Level level, const std::string filename, size_t line, const std::string logger, const std::string msg)
            : _ctime(Util::Date::NowTime()),
              _level(level),
              _filename(filename),
              _line(line),
              _id(std::this_thread::get_id()),
              _logger(logger),
              _payload(std::move(msg))
        {
        }

    public:
        time_t _ctime;          // 时间
        LogLevel::Level _level; // 日志等级
        std::string _filename;  // 源码文件名
        size_t _line;           // 行号
        std::thread::id _id;    // 线程ID
        std::string _logger;    // 日志器名称
        std::string _payload;   // 日志内容 -- 有效载荷
    };
}