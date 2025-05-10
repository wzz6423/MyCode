/*
    优化使用难度
*/

#pragma once

//C++
#include <iostream>
#include <string>
#include <optional>
// Other
#include "logger.hpp"

namespace Log
{
    // 获取指定日志器
    std::optional<Logger::loggerPtr> GetLogger(const std::string &name)
    {
        std::optional<const Logger::loggerPtr> logPtr = Log::LoggerManager::GetInstance().GetLogger(name);
        if (logPtr)
        {
            return logPtr.value();
        }
        else
        {
            std::cerr << "not found logger..." << std::endl;
            return std::nullopt;
        }
    }

    // 获取默认日志器
    Logger::loggerPtr RootLogger()
    {
        return Log::LoggerManager::GetInstance().RootLogger();
    }
}