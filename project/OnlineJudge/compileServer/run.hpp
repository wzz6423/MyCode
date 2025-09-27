/*
    运行模块
*/

#pragma once

// C++
#include <string>
#include <vector>
// sys call
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>
// Other
#include "../common/utils.hpp"
#include "../common/log.hpp"

namespace run
{
    class Run
    {
    public:
        Run() = default;
        ~Run() = default;

        /**
         * 运行用户代码
         *
         * @brief 运行
         * @param fileName 文件名 -- 只需要输入文件名, 前缀后缀使用 utils 中的方法进行处理
         * @param timeLimit 时间资源限制
         * @param memoryLimit 内存资源限制(KB)
         * @return int 运行结果
         *         > 0: 进程异常, 退出时收到信号, 返回值就是收到的信号
         *         == 0: 正常运行完毕, 结果不知道, 保存在临时文件中
         *         < 0: 内部错误 --> 打开文件失败 / fork 子进程失败
         * -----------------------------------------------------------------------------------------------
         * 运行结果:
         * 代码跑完 & 结果正确
         * 代码跑完 & 结果不正确
         * 代码没跑完 -- 异常
         *
         * Runner 方法不关心结果正确性 -- 由测试用例决定, 上层调用者关心
         * 只考虑是否正确运行完毕
         * -----------------------------------------------------------------------------------------------
         * 进程启动时
         * 标准输入: 不处理
         * 标准输出: 进程运行完毕结果
         * 标准错误: 获取运行时错误信息
         * --> 写在同名文件中
         */
        static auto Runner(const std::string &fileName, int timeLimit, int memoryLimit) -> int
        {
            // 文件资源路径
            std::string execute = utils::PathUtil::Exe(fileName);
            std::string errorFile = utils::PathUtil::StdError(fileName);
            std::string inFile = utils::PathUtil::StdIn(fileName);
            std::string outFile = utils::PathUtil::StdOut(fileName);

            // 打开文件便于子进程重定向 -- 子进程如果打开文件失败不易进行报错
            umask(0); // 保证生成文件权限准确
            int inFd = open(inFile.c_str(), O_RDONLY);
            int outFd = open(outFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
            int errorFd = open(errorFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (inFd < 0 || outFd < 0 || errorFd < 0) [[unlikely]]
            {
                LOG(ERROR) << "open file error" << std::endl;
                return -1;
            }

            // fork 出子进程执行测试进程
            pid_t child = fork();
            if (child < 0) [[unlikely]]
            {
                // fork error
                LOG(ERROR) << "fork child process error" << std::endl;

                // 关闭打开的文件
                close(inFd);
                close(outFd);
                close(errorFd);

                return -2;
            }
            else if (child == 0)
            {
                // Child process
                // 重定向
                dup2(inFd, 0);
                dup2(outFd, 1);
                dup2(errorFd, 2);

                // set proc limit
                setProcLimit(timeLimit, memoryLimit);

                // Execute user code
                /**
                 * @param 1(file) 执行谁
                 * @param 2(arg) 怎么执行
                 * @param 3 表示参数输入结束
                 */
                execlp(execute.c_str(), execute.c_str(), nullptr);
                exit(1);
            }
            else
            {
                // Parent process
                // 父进程不需要重定向 -- 直接关闭
                close(inFd);
                close(outFd);
                close(errorFd);

                int status;
                waitpid(child, &status, 0); // 阻塞等待
                // 进程运行异常, Linux 上一定是收到了信号
                // return status & 0x7F;
                if (WIFEXITED(status))
                {
                    return WEXITSTATUS(status);
                }
            }

            return 0;
        }

    private:
        static auto setProcLimit(int timeLimit, int memoryLimit) -> void
        {
            // Set resource limits
            // memory
            struct rlimit memLimit;
            memLimit.rlim_cur = memoryLimit * 1024; // 转为 KB
            memLimit.rlim_max = RLIM_INFINITY; // 不限制
            setrlimit(RLIMIT_AS, &memLimit);

            // cpu -- time
            struct rlimit cpuLimit;
            cpuLimit.rlim_cur = timeLimit;
            cpuLimit.rlim_max = RLIM_INFINITY; // 不限制
            setrlimit(RLIMIT_CPU, &cpuLimit);
        }
    };
}