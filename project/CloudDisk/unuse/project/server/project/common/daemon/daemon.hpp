#pragma once

// C
#include <cstdlib>
// C++
#include <string>
#include <system_error>
// Linux
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
// 日志
#include "../log/logger.hpp"

namespace cloud_disk::Daemon
{
    // 守护进程类
    class Daemon
    {
    public:
        /** 构造函数
         * @param work_dir 守护进程的工作目录（可选）
         */
        explicit Daemon(const std::string &work_dir = "")
        {
            // 忽略特定信号
            ignoreSignals();

            // 第一次 fork - 创建子进程并退出父进程
            forkAndExitParent();

            // 创建新会话并成为会话组长
            createNewSession();

            // 设置工作目录（如果指定）
            setWorkingDirectory(work_dir);

            // 重定向标准输入/输出/错误到 /dev/null
            redirectStandardIO();
        }

        // 禁止拷贝和赋值
        Daemon(const Daemon &) = delete;
        Daemon &operator=(const Daemon &) = delete;

        // 创建守护进程的静态方法
        // @param work_dir 守护进程的工作目录（可选）
        // @return 成功返回 true，失败返回 false
        static auto Create(const std::string &work_dir = "") -> bool
        {
            try
            {
                // 使用 RAII 方式创建守护进程
                static Daemon instance(work_dir);
                return true;
            }
            catch (const std::system_error &e)
            {
                Log::lg("critical", Log::FileName(), Log::Line(),
                                     "Failed to create daemon: {}", e.what());
                return false;
            }
        }

    private:
        // 忽略特定信号
        auto ignoreSignals() -> void
        {
            // 忽略子进程终止信号
            if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
            {
                throw std::system_error(errno, std::system_category(),
                                        "Failed to ignore SIGCHLD");
            }

            // 忽略管道断开信号
            if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
            {
                throw std::system_error(errno, std::system_category(),
                                        "Failed to ignore SIGPIPE");
            }

            // 忽略停止信号
            if (signal(SIGTSTP, SIG_IGN) == SIG_ERR)
            {
                throw std::system_error(errno, std::system_category(),
                                        "Failed to ignore SIGTSTP");
            }
        }

        // 创建子进程并退出父进程
        auto forkAndExitParent() -> void
        {
            const pid_t pid = fork();
            if (pid < 0)
            {
                throw std::system_error(errno, std::system_category(),
                                        "First fork failed");
            }

            // 父进程退出
            if (pid > 0)
            {
                std::exit(EXIT_SUCCESS);
            }
        }

        // 创建新会话并成为会话组长
        auto createNewSession() -> void
        {
            if (setsid() < 0)
            {
                throw std::system_error(errno, std::system_category(),
                                        "setsid failed");
            }
        }

        // 设置工作目录
        auto setWorkingDirectory(const std::string &work_dir) -> void
        {
            if (!work_dir.empty())
            {
                if (chdir(work_dir.c_str()) < 0)
                {
                    throw std::system_error(errno, std::system_category(),
                                            "chdir failed");
                }
            }
        }

        // 重定向标准输入/输出/错误
        auto redirectStandardIO() -> void
        {
            // 打开 /dev/null
            const int null_fd = open(_null_file.c_str(), O_RDWR);
            if (null_fd < 0)
            {
                throw std::system_error(errno, std::system_category(),
                                        "open /dev/null failed");
            }

            // 重定向标准输入
            if (dup2(null_fd, STDIN_FILENO) < 0)
            {
                close(null_fd);
                throw std::system_error(errno, std::system_category(),
                                        "dup2 stdin failed");
            }

            // 重定向标准输出
            if (dup2(null_fd, STDOUT_FILENO) < 0)
            {
                close(null_fd);
                throw std::system_error(errno, std::system_category(),
                                        "dup2 stdout failed");
            }

            // 重定向标准错误
            if (dup2(null_fd, STDERR_FILENO) < 0)
            {
                close(null_fd);
                throw std::system_error(errno, std::system_category(),
                                        "dup2 stderr failed");
            }

            // 关闭原始文件描述符
            close(null_fd);
        }

    private:
        const std::string _null_file = "/dev/null"; // 空设备文件路径
    };
}