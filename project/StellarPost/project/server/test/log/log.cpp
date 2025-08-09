// 测试通过

#include "../../../project/common/log/logger.hpp"

int main() {
    // 创建日志实例（非调试模式）
    auto& logger = stellar_post::Log::Log::GetInstance(false);
    
    logger("INFO", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is an info message.");
    logger("DEBUG", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is a debug message.");
    logger("WARN", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is a warning message.");
    logger("ERROR", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is an error message.");
    logger("FATAL", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is a fatal message.");
    logger("UNKNOWN", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is an unknown log level message.");
    logger("TRACE", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is a trace message.");
    logger("DEG", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is a debug message with short level.");
    logger("CRITICAL", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is a critical message.");
    logger("CRITI", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is a critical message with short level.");
    logger("WARNING", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is a warning message with long level.");
    logger("ERR", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is an error message with short level.");
    logger("FAT", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is a fatal message with short level.");
    logger("TRA", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is a trace message with short level.");
    logger("DEG", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is a debug message with short level.");
    logger("INFO", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is another info message with a different format: {} {}", 42, "answer");
    logger("UNKNOWN", stellar_post::Log::FileName(), stellar_post::Log::Line(), "This is another unknown log level message with a different format: {} {}", 3.14, "pi");

    logger.Flush(); // 确保日志被写入文件

    return 0;
}