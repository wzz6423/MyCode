// 测试通过

#include "../../project/common/log/logger.hpp"

int main() {
    // 创建日志实例（非调试模式）
    auto& logger = cloud_disk::Log::Log::GetInstance(false);
    
    logger("INFO", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is an info message.");
    logger("DEBUG", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is a debug message.");
    logger("WARN", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is a warning message.");
    logger("ERROR", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is an error message.");
    logger("FATAL", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is a fatal message.");
    logger("UNKNOWN", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is an unknown log level message.");
    logger("TRACE", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is a trace message.");
    logger("DEG", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is a debug message with short level.");
    logger("CRITICAL", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is a critical message.");
    logger("CRITI", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is a critical message with short level.");
    logger("WARNING", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is a warning message with long level.");
    logger("ERR", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is an error message with short level.");
    logger("FAT", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is a fatal message with short level.");
    logger("TRA", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is a trace message with short level.");
    logger("DEG", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is a debug message with short level.");
    logger("INFO", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is another info message with a different format: {} {}", 42, "answer");
    logger("UNKNOWN", cloud_disk::Log::FileName(), cloud_disk::Log::Line(), "This is another unknown log level message with a different format: {} {}", 3.14, "pi");

    logger.Flush(); // 确保日志被写入文件

    return 0;
}