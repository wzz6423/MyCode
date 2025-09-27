/*
    编译模块
*/

#pragma once

// C++
#include <string>
// syscall
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// Other
#include "../common/utils.hpp"
#include "../common/log.hpp"

namespace compile
{
    class Compile
    {
    public:
        Compile() {}
        ~Compile() {}

        /**
         * 对用户发送的代码进行编译
         *
         * @brief 编译
         * @param fileName 文件名
         * @return bool 是否编译成功
         */
        /*
        example:
            fileName: test
            --> ..temp/test.cpp
            --> ..temp/test.exe
            --> ..temp/test.err
        */
        static auto Compiler(const std::string &fileName) -> bool
        {
            // 子进程编译
            pid_t child = fork();
            if (child < 0) [[unlikely]]
            {
                LOG(ERROR) << "fork child process error" << std::endl;
                return false;
            }
            else if (child == 0)
            {
                // 打开错误文件
                umask(0); // 保证生成文件权限准确
                int stderror = open(utils::PathUtil::CompileError(fileName).c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
                if (stderror < 0)
                {
                    LOG(ERROR) << "open error file error" << std::endl;

                    exit(1);
                }

                // 重定向标准错误到文件
                dup2(stderror, 2);
                // 程序替换不影响进程的文件描述符表

                // 子进程: 调用编译器完成对代码的编译功能
                // g++ -o target src -std=c++23
                execlp("g++", "g++", utils::PathUtil::Src(fileName).c_str(), "-o",
                       utils::PathUtil::Exe(fileName).c_str(), "-D", "COMPILER_ONLINE", "-std=c++23", nullptr);

                LOG(ERROR) << "execlp g++ error" << std::endl;

                exit(2);
            }
            else
            {
                waitpid(child, nullptr, 0);
                // 判断是否编译成功 -- 看是否形成可执行程序
                if (utils::FileUtil::IsFileExists(utils::PathUtil::Exe(fileName)))
                {
                    LOG(INFO) << "compile success" << std::endl;
                    return true;
                }

                LOG(ERROR) << "compile error" << std::endl;
                return false;
            }
        }
    };
}