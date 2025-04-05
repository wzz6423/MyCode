#pragma once

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZE 1024
#define LogFile "log.txt"

enum Level
{
    Info = 0,
    Debug,
    Warning,
    Error,
    Fatal
};

enum Print_Goal
{
    Screen = 0,
    Onefile,
    Classfile
};

class Log
{
public:
    Log()
    {
        _print_method = Screen;
        _path = "./log.txt";
    }

    void enable(int method)
    {
        _print_method = method;
    }

    void operator()(int level, const char *format, ...)
    {
        time_t t = time(nullptr);
        struct tm *ctime = localtime(&t);
        char left_buffer[SIZE];
        snprintf(left_buffer, sizeof(left_buffer), "[%s][%d-%d-%d %d:%d:%d]", level_to_string(level).c_str(), ctime->tm_year + 1900,
                 ctime->tm_mon + 1, ctime->tm_mday, ctime->tm_hour, ctime->tm_min, ctime->tm_sec);

        va_list s;
        va_start(s, format);
        char right_buffer[SIZE * 2];
        snprintf(right_buffer, sizeof(right_buffer), format, s);
        va_end(s);

        char log_txt[SIZE * 2];
        snprintf(log_txt, sizeof(log_txt), "%s %s\n", left_buffer, right_buffer);

        print_log(level, log_txt);
    }

    std::string level_to_string(int level)
    {
        switch (level)
        {
        case Info:
            return "Info";
        case Debug:
            return "Debug";
        case Warning:
            return "Warning";
        case Error:
            return "Error";
        case Fatal:
            return "Fatal";
        default:
            return "None";
        }
    }

    void print_log(int level, const std::string &logtxt)
    {
        switch (_print_method)
        {
        case Screen:
            std::cout << logtxt << std::endl;
            break;
        case Onefile:
            print_onefile(LogFile, logtxt);
            break;
        case Classfile:
            print_classfile(level, logtxt);
            break;
        default:
            break;
        }
    }

    void print_onefile(const std::string &logname, const std::string &logtxt)
    {
        std::string _logname = _path + logname;
        int fd = open(_logname.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
        if (fd < 0)
        {
            return;
        }
        write(fd, logtxt.c_str(), logtxt.size());
        close(fd);
    }

    void print_classfile(int level, const std::string &logtxt)
    {
        std::string file_name = LogFile;
        file_name += ".";
        file_name += level_to_string(level);
        print_onefile(file_name, logtxt);
    }

    ~Log()
    {
    }

private:
    int _print_method;
    std::string _path;
};

Log lg;