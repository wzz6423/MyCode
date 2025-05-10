////////////////////////////////////////////////////////////////////////////////////////////////
// use example
#include "../project/Log.h"
void Writer()
{
    Log::Logger::loggerPtr logPtr = Log::LoggerManager::GetInstance().GetLogger("async_logger").value();
    std::string str = "test log-";
    size_t count = 0;
    logPtr->Lg("Unknow", Log::Util::GetFileName(), Log::Util::GetLine(), "%s", "test log");
    logPtr->Lg("debug", Log::Util::GetFileName(), Log::Util::GetLine(), "%s", "test log");
    logPtr->Lg("INFO", Log::Util::GetFileName(), Log::Util::GetLine(), "%s", "test log");
    logPtr->Lg("InFo", Log::Util::GetFileName(), Log::Util::GetLine(), "%s", "test log");
    logPtr->Debug(Log::Util::GetFileName(), Log::Util::GetLine(), "%s", "test log");
    logPtr->Lg("Warning", Log::Util::GetFileName(), Log::Util::GetLine(), "%s", "test log");
    while (count < 500000)
    {
        logPtr->Error(Log::Util::GetFileName(), Log::Util::GetLine(), "test log-%d", count++);
    }
}
auto main() -> int
{
    std::unique_ptr<Log::LoggerBuilder> builder = std::make_unique<Log::GlobalLoggerBuilder>();
    builder->BuildName("async_logger");
    builder->BuildLevel("Warning");
    builder->BuildType(Log::LoggerType::ASYNC_LOGGER);
    builder->BuildASyncNature({false, Log::DefaultBufferSize::MB_50});
    // BuildSink 支持日志数据落地方向扩展
    builder->BuildSink<Log::StdoutSink>();
    builder->BuildSink<Log::FileSink>("../test/testlog/file/flog.log");
    builder->Build();

    Writer();

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// test: Test util.hpp
// #include "util.hpp"
// #include <vector>
// #include <unistd.h>
// int main()
// {
//     测试通过
//     std::cout << "时间函数测试开始..." << std::endl;
//     std::cout << Log::Util::Date::NowTime() << std::endl;
//     std::cout << Log::Util::Date::FormatTime() << std::endl;
//     sleep(1);
//     std::cout << Log::Util::Date::FormatExistTime(Log::Util::Date::NowTime()) << std::endl;
//     std::cout << "时间函数测试结束..." << std::endl;

//     // 测试通过
//     std::cout << "文件函数测试开始..." << std::endl;
//     Log::Util::File::CreateDirectory("./testutil/hello/world");
//     Log::Util::File::CreateFile("./testutil1/hello/world/haha/hehe");
//     Log::Util::File::CreateFile("./testutil2/hehe/haha.txt", "hello world!");
//     Log::Util::File::CreateFile("./testutil3/hello/world/test/haha/hehe/nihao/aaa");
//     bool flag = false;
//     flag = Log::Util::File::IsFileExists("haha");
//     if (flag == false)
//     {
//         std::cout << "haha不存在" << std::endl;
//     }
//     flag = Log::Util::File::IsFileExists("haha.txt");
//     if (flag == false)
//     {
//         std::cout << "haha.txt不存在" << std::endl;
//     }
//     flag = Log::Util::File::IsFileExists("hehe");
//     if (flag == false)
//     {
//         std::cout << "hehe不存在" << std::endl;
//     }
//     flag = Log::Util::File::IsFileExists("world");
//     if (flag == false)
//     {
//         std::cout << "world不存在" << std::endl;
//     }
//     flag = Log::Util::File::IsFileExists("aaa");
//     if (flag == false)
//     {
//         std::cout << "aaa不存在" << std::endl;
//     }
//     std::cout << Log::Util::File::ExtractDirectory("./testutil3/hello/world/test/haha/hehe/nihao/aaa") << std::endl;
//     std::cout << Log::Util::File::ExtractFilename("./testutil3/hello/world/test/haha/hehe/nihao/aaa") << std::endl;
//     std::cout << "文件函数测试结束..." << std::endl;

//     // 测试通过
//     std::cout << "文件函数测试开始..." << std::endl;
//     Log::Util::File::CreateDirectory("./testutil/hello/world");
//     std::cout << "1111111" << std::endl;
//     Log::Util::File::CreateFile("./testutil/hello/world/haha/hehe.txt");
//     Log::Util::File::CreateFile("./testutil/hello/world/haha/haha.txt");
//     std::cout << "2222222" << std::endl;
//     std::string str = "helloworld123456789";
//     std::vector<std::string> test;
//     for (size_t i = 0; i < 10; ++i)
//     {
//         test.push_back("hello world" + std::to_string(i));
//     }
//     std::ofstream ofs1("./testutil/hello/world/haha/hehe.txt", std::ios::binary | std::ios::app);
//     ofs1.write(str.c_str(), str.size());
//     Log::Util::File::WriteLines<std::string>("./testutil/hello/world/haha/haha.txt", str, true);
//     Log::Util::File::WriteLines<std::vector<std::string>>("./testutil/hello/world/haha/haha.txt", test, true);
//     std::cout << "文件函数测试结束..." << std::endl;

//     // 测试通过
//     std::cout << "文件函数测试开始..." << std::endl;
//     bool flag = Log::Util::File::IsFileExists("./testutil/hello/world");
//     if (!flag)
//     {
//         std::cout << "文件不存在" << std::endl;
//     }
//     std::cout << "文件函数测试结束..." << std::endl;

//     return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////
// test: Test level.hpp
// 测试通过
// #include "level.hpp"
// int main(){
//     std::cout << Log::LogLevel::LevelToStdString(Log::LogLevel::Level::UNKNOW).value() << std::endl;
//     std::cout << Log::LogLevel::LevelToStdString(Log::LogLevel::Level::DEBUG).value() << std::endl;
//     std::cout << Log::LogLevel::LevelToStdString(Log::LogLevel::Level::INFO).value() << std::endl;
//     std::cout << Log::LogLevel::LevelToStdString(Log::LogLevel::Level::WARNING).value() << std::endl;
//     std::cout << Log::LogLevel::LevelToStdString(Log::LogLevel::Level::ERROR).value() << std::endl;
//     std::cout << Log::LogLevel::LevelToStdString(Log::LogLevel::Level::FATAL).value() << std::endl;
//     std::cout << Log::LogLevel::LevelToStdString(Log::LogLevel::Level::OFF).value() << std::endl;

//     return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////
// test: Test message.hpp && format.hpp
// 测试通过
// #include "level.hpp"
// #include "format.hpp"
// #include "message.hpp"
// int main(){
//     Log::LogMessage msg(Log::LogLevel::Level::DEBUG, 95, "UnitTest.cpp", "testlogger", "hello world!");
//     Log::Formatter formatmsg;
//     formatmsg.Format(std::cout, msg);
//     return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////
// test: Test logsink.hpp
// 测试通过
// #include "level.hpp"
// #include "format.hpp"
// #include "message.hpp"
// #include "logsink.hpp"
// int main()
// {
//     Log::LogMessage msg(Log::LogLevel::Level::DEBUG, 95, "UnitTest.cpp", "testlogger", "hello world!");
//     Log::Formatter formatmsg;
//     std::string str = formatmsg.Format(msg);

//     Log::LogSink::sinkPtr ptr1 = Log::SinkFactory::Create<Log::StdoutSink>();
//     Log::LogSink::sinkPtr ptr2 = Log::SinkFactory::Create<Log::FileSink>("./testlog/file/log.log");
//     Log::LogSink::sinkPtr ptr3 = Log::SinkFactory::Create<Log::RollBySizeFileSink>("./testlog/size/size-", 1024);
//     Log::LogSink::sinkPtr ptr4 = Log::SinkFactory::Create<Log::RollByTimeFileSink>("./testlog/time/log-", 3, 1024); // 此处后来改为 enum class, 如下方测试所示

//     ptr1->Log(str.c_str(), str.size());
//     ptr2->Log(str.c_str(), str.size());
//     size_t cursize = 0;
//     size_t count = 0;
//     while(cursize < 1024 * 10)
//     {
//         std::string tmp = str + std::to_string(count++);
//         cursize += tmp.size();
//         ptr3->Log(tmp.c_str(), tmp.size());
//         ptr4->Log(tmp.c_str(), tmp.size());
//     }

//     return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////
// test: Test SyncLogger
// 测试通过
// #include <vector>
// #include "logger.hpp"
// int main()
// {
//     std::string loggerName = "sync";
//     Log::LogLevel::Level limitLevel = Log::LogLevel::Level::WARNING;
//     Log::Formatter::formatterPtr forPtr = std::make_shared<Log::Formatter>();

//     Log::LogSink::sinkPtr ptr1 = Log::SinkFactory::Create<Log::StdoutSink>();
//     Log::LogSink::sinkPtr ptr2 = Log::SinkFactory::Create<Log::FileSink>("./testlog/file/log.log");
//     Log::LogSink::sinkPtr ptr3 = Log::SinkFactory::Create<Log::RollBySizeFileSink>("./testlog/size/size-", Log::FileSize::HALF_GB);
//     Log::LogSink::sinkPtr ptr4 = Log::SinkFactory::Create<Log::RollByTimeFileSink>("./testlog/time/log-", Log::TimeGap::MINUTE, Log::FileSize::HALF_GB);
//     std::vector<Log::LogSink::sinkPtr> sinks = {ptr1, ptr2, ptr3, ptr4};

//     Log::Logger::loggerPtr logPtr = std::make_shared<Log::SyncLogger>(loggerName, limitLevel, forPtr, sinks);

//     std::string str = "test log-";
//     size_t cursize = 0;
//     size_t count = 0;
//     logPtr->Lg("Unknow", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("debug", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("INFO", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("InFo", __FILE__, __LINE__, "%s", "test log");
//     // logPtr->Lg("aabbccdd", __FILE__, __LINE__, "%s", "test log"); // 直接抛异常
//     logPtr->Debug(__FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("Warning", __FILE__, __LINE__, "%s", "test log");
//     while (cursize < 1024 * 10)
//     {
//         cursize += 40; // 测试用, 意思一下即可
//         logPtr->Error( __FILE__, __LINE__, "test log-%d", count);
//         logPtr->Fatal(__FILE__, __LINE__, "test log-%d", count++);
//     }

//     return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////
// test: Test synclogger--builder
// 测试通过
// #include "logger.hpp"
// auto main() -> int
// {
//     std::unique_ptr<Log::LoggerBuilder> builder = std::make_unique<Log::LocalLoggerBuilder>();
//     builder->BuildName("sync_logger");
//     builder->BuildLevel("Warning");
//     builder->BuildSink<Log::StdoutSink>();
//     builder->BuildType(Log::LoggerType::SYNC_LOGGER);
//     builder->BuildSink<Log::FileSink>("./testlog/file/log.log");
//     Log::Logger::loggerPtr logPtr = builder->Build();

//     std::string str = "test log-";
//     size_t cursize = 0;
//     size_t count = 0;
//     logPtr->Lg("Unknow", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("debug", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("INFO", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("InFo", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Debug(__FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("Warning", __FILE__, __LINE__, "%s", "test log");
//     while (cursize < 1024 * 10)
//     {
//         cursize += 40; // 测试用, 意思一下即可
//         logPtr->Error(__FILE__, __LINE__, "test log-%d", count);
//         logPtr->Fatal(__FILE__, __LINE__, "test log-%d", count++);
//     }

//     return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////
// test: Test buffer
// wzz@hcss-ecs-e1d4:~/linux_test/Log/testlog/file$ md5sum log.log
// cf45e4ccb5deb4e9c9a3b39fb4c48aa1  log.log
// wzz@hcss-ecs-e1d4:~/linux_test/Log/testlog/file$ md5sum tmplog.log
// cf45e4ccb5deb4e9c9a3b39fb4c48aa1  tmplog.log
// 测试通过
// #include <iostream>
// #include <fstream>
// #include "buffer.hpp"
// auto main() -> int
// {
//     // 读取文件数据, 逐渐写入缓冲区, 将缓冲区数据写入文件, 对比新文件与原文件是否一致
//     std::ifstream ifs("./testlog/file/log.log", std::ios::binary);
//     if (ifs.is_open() == false)
//     {
//         std::cerr << "open fail..." << std::endl;
//     }
//     ifs.seekg(0, std::ios::end);
//     size_t fsize = ifs.tellg();
//     ifs.seekg(0, std::ios::beg);
//     std::string body;
//     body.resize(fsize);
//     ifs.read(&body[0], fsize);
//     if (ifs.good() == false)
//     {
//         std::cerr << "read fail..." << std::endl;
//     }
//     ifs.close();

//     Log::Buffer buf(true, Log::DefaultBufferSize::MB);
//     for (size_t i = 0; i < body.size(); ++i)
//     {
//         buf.Push(&body[i], 1);
//     }
//     std::ofstream ofs("./testlog/file/tmplog.log", (std::ios::binary | std::ios::trunc));
//     if (ofs.is_open() == false)
//     {
//         std::cerr << "open fail..." << std::endl;
//     }
//     for (size_t i = 0; i < fsize; ++i)
//     {
//         ofs.write(buf.Begin(), 1);
//         buf.MoveReader(1);
//     }
//     ofs.close();

//     return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////
// test: Test asynclogger
// 测试通过
// #include "logger.hpp"
// #include <print>
// auto main() -> int
// {
//     std::unique_ptr<Log::LoggerBuilder> builder = std::make_unique<Log::LocalLoggerBuilder>();
//     builder->BuildName("async_logger");
//     builder->BuildLevel("Warning");
//     builder->BuildFormatter("%m%n");
//     builder->BuildType(Log::LoggerType::ASYNC_LOGGER);
//     builder->BuildASyncNature({false, Log::DefaultBufferSize::MB_50});
//     builder->BuildSink<Log::StdoutSink>();
//     builder->BuildSink<Log::FileSink>("../test/testlog/file/flog.log");
//     Log::Logger::loggerPtr logPtr = builder->Build();

//     std::println("logger build success...");

//     std::string str = "test log-";
//     size_t count = 0;
//     logPtr->Lg("Unknow", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("debug", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("INFO", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("InFo", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Debug(__FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("Warning", __FILE__, __LINE__, "%s", "test log");
//     while (count < 500000)
//     {
//         logPtr->Error(__FILE__, __LINE__, "test log-%d", count++);

//         // logPtr->Error(__FILE__, __LINE__, "test log-%d", count);
//         // logPtr->Fatal(__FILE__, __LINE__, "test log-%d", count++);
//     }

//     return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////
// test: Test global logger
// 测试通过
// #include <print>
// #include "logger.hpp"
// void Writer()
// {
//     std::println("enter writer");

//     Log::Logger::loggerPtr logPtr = Log::LoggerManager::GetInstance().GetLogger("async_logger").value();

//     std::string str = "test log-";
//     size_t count = 0;
//     std::println("write begin ...");
//     logPtr->Lg("Unknow", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("debug", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("INFO", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("InFo", __FILE__, __LINE__, "%s", "test log");
//     logPtr->Debug(__FILE__, __LINE__, "%s", "test log");
//     logPtr->Lg("Warning", __FILE__, __LINE__, "%s", "test log");
//     while (count < 500000)
//     {
//         logPtr->Error(__FILE__, __LINE__, "test log-%d", count++);
//     }
// }
// auto main() -> int
// {
//     std::unique_ptr<Log::LoggerBuilder> builder = std::make_unique<Log::GlobalLoggerBuilder>();
//     builder->BuildName("async_logger");
//     builder->BuildLevel("Warning");
//     builder->BuildFormatter("%m%n");
//     builder->BuildType(Log::LoggerType::ASYNC_LOGGER);
//     builder->BuildASyncNature({false, Log::DefaultBufferSize::MB_50});
//     builder->BuildSink<Log::StdoutSink>();
//     builder->BuildSink<Log::FileSink>("../test/testlog/file/flog.log");
//     builder->Build();
//     std::println("build logger success!");
//     Writer();

//     return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////
// test: Test util
// 测试通过
// #include <print>
// #include "util.hpp"
// int main(){
//     std::println("filename: {}, line: {}", Log::Util::GetFileName(), Log::Util::GetLine());

//     return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////
// test: Test all
// 测试通过
// #include "Log.h"
// void Writer()
// {
//     Log::Logger::loggerPtr logPtr = Log::LoggerManager::GetInstance().GetLogger("async_logger").value();

//     std::string str = "test log-";
//     size_t count = 0;
//     logPtr->Lg("Unknow", Log::Util::GetFileName(), Log::Util::GetLine(), "%s", "test log");
//     logPtr->Lg("debug", Log::Util::GetFileName(), Log::Util::GetLine(), "%s", "test log");
//     logPtr->Lg("INFO", Log::Util::GetFileName(), Log::Util::GetLine(), "%s", "test log");
//     logPtr->Lg("InFo", Log::Util::GetFileName(), Log::Util::GetLine(), "%s", "test log");
//     logPtr->Debug(Log::Util::GetFileName(), Log::Util::GetLine(), "%s", "test log");
//     logPtr->Lg("Warning", Log::Util::GetFileName(), Log::Util::GetLine(), "%s", "test log");
//     while (count < 500000)
//     {
//         logPtr->Error(Log::Util::GetFileName(), Log::Util::GetLine(), "test log-%d", count++);
//     }
// }
// auto main() -> int
// {
//     std::unique_ptr<Log::LoggerBuilder> builder = std::make_unique<Log::GlobalLoggerBuilder>();
//     builder->BuildName("async_logger");
//     builder->BuildLevel("Warning");
//     builder->BuildType(Log::LoggerType::ASYNC_LOGGER);
//     builder->BuildASyncNature({true, Log::DefaultBufferSize::MB_50});
//     builder->BuildSink<Log::StdoutSink>();
//     builder->BuildSink<Log::FileSink>("../test/testlog/file/flog.log");
//     builder->Build();
//     Writer();

//     return 0;
// }