// 性能测试

// C++ -- for time, print
#include <chrono>
#include <print>
// Other -- Log
#include "../project/Log.h"

auto Bench(const std::string &loggerName, size_t threadCount, size_t messageCount, size_t MessageLen) -> void
{
    // 获取日志器
    Log::Logger::loggerPtr logPtr;
    std::optional<Log::Logger::loggerPtr> logPtr_op = Log::GetLogger(loggerName);
    if (!logPtr_op)
    {
        return;
    }
    else{
        logPtr = logPtr_op.value();
    }
    std::println("测试日志数量: {}, 测试日志总大小(kb): {}", messageCount, (messageCount * MessageLen / 1024));

    // 组织指定长度的日志消息
    std::string msg(MessageLen - 1, 'A'); // 换行符

    // 创建指定数量线程
    std::vector<std::thread> threads;
    std::vector<double> costTime(threadCount);              // 记录每个线程耗时
    size_t threadMessageCount = messageCount / threadCount; // 每个线程输出日志的数量
    for (size_t i = 0; i < threadCount; ++i)
    {
        threads.emplace_back([&, i]()
                             {
                                 // 线程函数内开始计时
                                auto start = std::chrono::high_resolution_clock::now();

                                 // 开始循环写日志
                                 for(size_t j = 0; j < threadMessageCount; ++j){
                                    logPtr->Lg("fatal", Log::Util::GetFileName(), Log::Util::GetLine(),"%s", msg.c_str());
                                 }

                                 // 函数内部结束计时
                                 auto end = std::chrono::high_resolution_clock::now();
                                 std::chrono::duration<double> cost = end - start;
                                 costTime[i] = cost.count();
                                 std::println("线程 {} 耗时 {} s, 输出日志数量: {}", i, cost, threadMessageCount); });
    }

    // 计算总耗时 -- 多线程场景下, 各个线程并发处理, 因此耗时最高的是总时间, 而不是相加
    // 先等待线程退出
    for (size_t i = 0; i < threadCount; ++i)
    {
        threads[i].join();
    }
    double maxCost = costTime[0];
    for (auto &e : costTime)
    {
        if (e > maxCost)
        {
            maxCost = e;
        }
    }

    // 计算测试结果
    size_t messageSecond = messageCount / maxCost;                       // 每秒输出条数
    size_t messageSize = (messageCount * MessageLen) / (maxCost * 1024); // 每秒输出日志大小(kb)

    // 输出打印测试结果
    std::println("总耗时: {}s", maxCost);
    std::println("每秒输出日志数量: {}, 每秒输出日志大小(kb): {}", messageSecond, messageSize);
}

auto SyncBench()
{
    std::unique_ptr<Log::LoggerBuilder> builder = std::make_unique<Log::GlobalLoggerBuilder>();
    builder->BuildName("sync_logger");
    builder->BuildLevel("Warning");
    builder->BuildFormatter("%m%n");
    builder->BuildType(Log::LoggerType::SYNC_LOGGER);
    builder->BuildASyncNature({true, Log::DefaultBufferSize::MB_50});
    builder->BuildSink<Log::FileSink>("../testLog/file/syncLog.log");
    builder->Build();

    Bench("sync_logger", 1, 1000000, 100);
    // Bench("sync_logger", 3, 1000000, 100);
}

auto ASyncBench()
{
    std::unique_ptr<Log::LoggerBuilder> builder = std::make_unique<Log::GlobalLoggerBuilder>();
    builder->BuildName("async_logger");
    builder->BuildLevel("Warning");
    builder->BuildFormatter("%m%n");
    builder->BuildType(Log::LoggerType::ASYNC_LOGGER);
    builder->BuildASyncNature({true, Log::DefaultBufferSize::MB_50});
    builder->BuildSink<Log::FileSink>("../testLog/file/asyncLog.log");
    builder->Build();

    // Bench("async_logger", 1, 1000000, 100);
    Bench("async_logger", 3, 1000000, 100);
}

auto main() -> int
{
    // SyncBench();
    try
    {
        ASyncBench();
    }
    catch (std::bad_optional_access e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}

// 测试结果
// Sync
// 单线程
// 测试日志数量: 1000000, 测试日志总大小(kb): 97656
// 线程 0 耗时 2.02888s s, 输出日志数量: 1000000
// 总耗时: 2.028882929s
// 每秒输出日志数量: 492882, 每秒输出日志大小(kb): 48133
// 多线程
// 测试日志数量: 1000000, 测试日志总大小(kb): 97656
// 线程 1 耗时 1.61189s s, 输出日志数量: 333333
// 线程 0 耗时 1.6327s s, 输出日志数量: 333333
// 线程 2 耗时 1.63554s s, 输出日志数量: 333333
// 总耗时: 1.635540717s
// 每秒输出日志数量: 611418, 每秒输出日志大小(kb): 59708

// ASync
// 单线程
// 测试日志数量: 1000000, 测试日志总大小(kb): 97656
// 线程 0 耗时 2.80186s s, 输出日志数量: 1000000
// 总耗时: 2.801862455s
// 每秒输出日志数量: 356905, 每秒输出日志大小(kb): 34854
// 多线程
// 测试日志数量: 1000000, 测试日志总大小(kb): 97656
// 线程 2 耗时 2.42653s s, 输出日志数量: 333333
// 线程 1 耗时 2.48065s s, 输出日志数量: 333333
// 线程 0 耗时 2.49908s s, 输出日志数量: 333333
// 总耗时: 2.499084236s
// 每秒输出日志数量: 400146, 每秒输出日志大小(kb): 39076