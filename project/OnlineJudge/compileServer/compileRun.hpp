/*
    编译运行模块

    适配用户请求, 定制通信协议字段
    调用编译模块和运行模块
    产生唯一文件 -- 服务被同时请求, 必须保证存储 code 的文件的文件名唯一, 防止不同 code 间影响
*/

#pragma once

// sys call
#include <signal.h>
// Other
#include "compile.hpp"
#include "run.hpp"

namespace compile_run
{
    class CompileRun
    {
    public:
        /**
         * @brief 运行 编译运行模块
         * @param data 用户发送的数据
         *        data: 用户提交的代码
         *        input: 用户给自己提交的代码对应的输入, 不做处理
         *        timeLimit: 时间限制
         *        memoryLimit: 内存限制
         * @return std::optional<std::string> 编译运行结果
         *         status: 状态码(必填)
         *         reason: 请求结果(必填)
         *         stdout: 程序运行结果(选填)
         *         error: 程序的错误结果 -- 编译错误/运行时错误(选填)
         */
        static auto Run(const std::string &data) -> std::optional<std::string>
        {
            // 创建返回值
            Json::Value ret;
            // 生成唯一文件名
            std::string fileName = utils::FileUtil::UniqueId();
            // 建立回调函数
            auto end = [&](int status, const std::string &reason) -> std::optional<std::string>
            {
                ret["status"] = status;
                ret["reason"] = reason;
                clean(fileName);
                return utils::JsonUtil::Serialize(ret);
            };

            // 解析 JSON 数据
            std::optional<Json::Value> jsonData_opt = utils::JsonUtil::UnSerialize(data);
            if (!jsonData_opt.has_value()) [[unlikely]]
            {
                LOG(ERROR) << "Failed to parse JSON data[server error]";
                return end(-1, "Failed to parse JSON data[server error]");
            }
            Json::Value jsonData = std::move(jsonData_opt.value());

            // 处理 jsonData, 获取用户提交的代码及对应的输入
            std::string code = jsonData["code"].asString();
            std::string input = jsonData["input"].asString();
            int timeLimit = jsonData["timeLimit"].asInt();
            int memoryLimit = jsonData["memoryLimit"].asInt();
            // 若提交代码为空则直接返回
            if (!code.size()) [[unlikely]]
            {
                LOG(WARNING) << "user updates code is empty" << std::endl;
                return end(-2, "user updates code is empty");
            }

            // 将用户提交的代码写入文件中 -- 参数是路径 + 唯一文件名 + 后缀
            if (!utils::FileUtil::File(utils::PathUtil::Src(fileName), code)) [[unlikely]]
            {
                LOG(ERROR) << "Failed to write code to file[server error]" << fileName << std::endl;
                return end(-1, "Failed to write code to file[server error]");
            }

            // 对用户提交的代码进行编译
            if (!compile::Compile::Compiler(fileName))
            {
                LOG(ERROR) << "Failed to compile code" << fileName << std::endl;
                // 获取编译报错数据
                std::optional<std::string> error_opt = utils::FileUtil::File(utils::PathUtil::CompileError(fileName));
                if (!error_opt.has_value())
                {
                    LOG(ERROR) << "Failed to read compile error file[server error]" << fileName << std::endl;
                    return end(-1, "Failed to compile code & Failed to read compile error file[server error]");
                }

                ret["error"] = std::move(error_opt.value());
                return end(-3, "Failed to compile code");
            }

            // 运行用户提交的代码
            int exitCode = run::Run::Runner(fileName, timeLimit, memoryLimit);
            if (exitCode > 0) [[likely]]
            {
                LOG(ERROR) << "Failed to run code" << fileName << std::endl;
                // 获取运行时错误数据
                std::optional<std::string> error_opt = utils::FileUtil::File(utils::PathUtil::StdError(fileName));
                if (!error_opt.has_value())
                {
                    LOG(ERROR) << "Failed to read compile error file[server error]" << fileName << std::endl;
                    return end(-1, "Failed to run code & Failed to read compile error file[server error]");
                }

                std::string errorDesc = signalToDesc(exitCode);
                ret["error"] = std::move(error_opt.value());
                return end(exitCode, errorDesc);
            }
            else if (exitCode < 0) [[unlikely]]
            {
                LOG(ERROR) << "Run code error[server error]" << fileName << std::endl;
                return end(-1, "Run code error[server error]");
            }
            else [[likely]]
            {
                // 返回运行结果
                ret["stdout"] = utils::FileUtil::File(utils::PathUtil::StdOut(fileName)).value_or("");
                ret["error"] = utils::FileUtil::File(utils::PathUtil::StdError(fileName)).value_or("");
                return end(0, "success");
            }
        }

    private:
        // 信号对应错误描述
        static auto signalToDesc(int signal) -> std::string
        {
            switch (signal)
            {
            case SIGHUP:
                return "Hangup";
            case SIGINT:
                return "Interrupt";
            case SIGQUIT:
                return "Quit";
            case SIGILL:
                return "Illegal instruction";
            case SIGTRAP:
                return "Trace/breakpoint trap";
            case SIGABRT:
                return "Aborted";
            case SIGBUS:
                return "Bus error";
            case SIGFPE:
                return "Floating point exception";
            case SIGKILL:
                return "Killed";
            case SIGUSR1:
                return "User defined signal 1";
            case SIGSEGV:
                return "Segmentation fault";
            case SIGUSR2:
                return "User defined signal 2";
            case SIGPIPE:
                return "Broken pipe";
            case SIGALRM:
                return "Alarm clock";
            case SIGTERM:
                return "Terminated";
            case SIGSTKFLT:
                return "Stack fault";
            case SIGCHLD:
                return "Child exited";
            case SIGCONT:
                return "Continued";
            case SIGSTOP:
                return "Stopped (signal)";
            case SIGTSTP:
                return "Stopped";
            case SIGTTIN:
                return "Stopped (tty input)";
            case SIGTTOU:
                return "Stopped (tty output)";
            case SIGURG:
                return "Urgent I/O condition";
            case SIGXCPU:
                return "CPU time limit exceeded";
            case SIGXFSZ:
                return "File size limit exceeded";
            case SIGVTALRM:
                return "Virtual timer expired";
            case SIGPROF:
                return "Profiling timer expired";
            case SIGWINCH:
                return "Window size changed";
            case SIGIO:
                return "I/O possible";
            case SIGPWR:
                return "Power failure";
            case SIGSYS:
                return "Bad system call";
            default:
                [[unlikely]] return "Unknown error type: " + std::to_string(signal);
            }
        }

        // 清理临时文件
        static auto clean(const std::string &fileName) -> void
        {
            // 删除 源文件, 可执行文件, 编译错误文件, 标准输出文件, 标准错误文件
            if (!utils::FileUtil::Remove(utils::PathUtil::Src(fileName)) ||
                !utils::FileUtil::Remove(utils::PathUtil::Exe(fileName)) ||
                !utils::FileUtil::Remove(utils::PathUtil::CompileError(fileName)) ||
                !utils::FileUtil::Remove(utils::PathUtil::StdOut(fileName)) ||
                !utils::FileUtil::Remove(utils::PathUtil::StdError(fileName)))
            {
                LOG(ERROR) << "Failed to delete tmp file: " << fileName << std::endl;
            }
        }
    };
}