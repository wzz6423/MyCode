/*
    日志模块 -- qdebug()
*/

#pragma once

// C
#include <cstring>
// C++
#include <iostream>
#include <string>

namespace logger
{
    // 日志级别
    enum LogLevel
    {
        INFO,
        DEBUG,
        WARNING,
        ERROR,
        FATAL
    };

    // 获取当前时间戳
    static auto timeStamp() -> std::string
    {
        struct timeval time;
        gettimeofday(&time, nullptr);
        return std::to_string(time.tv_sec * 1000 + time.tv_usec / 1000);
    }

    [[nodiscard]] inline auto Log(const std::string &level, const std::string &fileName, int line) -> std::ostream &
    {
        // 添加日志等级
        std::string message = "[" + level + "]";

        // 添加报错文件名称
        message += "[" + fileName + "]";

        // 添加报错行
        message += "[" + std::to_string(line) + "]";

        // 添加报错时间
        message += "[" + timeStamp() + "]";

        std::cout << message;

        return std::cout;
    }
}
// 日志宏
#define LOG(level) logger::Log(#level, __FILE__, __LINE__)