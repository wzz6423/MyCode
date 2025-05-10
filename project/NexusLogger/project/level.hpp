/*
    1. 定义枚举类以定义日志等级
    2. 提供错误等级与字符串间的转化
*/

#pragma once

// C++
#include <iostream>
#include <string>
#include <optional>

namespace Log
{
    struct LogLevel
    {
    public:
        enum class Level
        {
            UNKNOW = 0,
            DEBUG,
            INFO,
            WARNING,
            ERROR,
            FATAL,
            OFF
        };

        static std::optional<const char *> LevelToString(const LogLevel::Level level)
        {
            switch (level)
            {
            case LogLevel::Level::UNKNOW:
                return "Unknow";
            case LogLevel::Level::DEBUG:
                return "Debug";
            case LogLevel::Level::INFO:
                return "Info";
            case LogLevel::Level::WARNING:
                return "Warning";
            case LogLevel::Level::ERROR:
                return "Error";
            case LogLevel::Level::FATAL:
                return "Fatal";
            case LogLevel::Level::OFF:
                return "Off";
            default:
                return "Unknow Log Level"; // 实际在上层使用的时候不会用到
            }
            return std::nullopt;
        }

        static std::optional<const std::string> LevelToStdString(const LogLevel::Level level)
        {
            switch (level)
            {
            case LogLevel::Level::UNKNOW:
                return "Unknow";
            case LogLevel::Level::DEBUG:
                return "Debug";
            case LogLevel::Level::INFO:
                return "Info";
            case LogLevel::Level::WARNING:
                return "Warning";
            case LogLevel::Level::ERROR:
                return "Error";
            case LogLevel::Level::FATAL:
                return "Fatal";
            case LogLevel::Level::OFF:
                return "Off";
            default:
                return "Unknow Log Level";
            }
            return std::nullopt;
        }
    };
}