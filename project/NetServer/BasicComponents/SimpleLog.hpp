#pragma once

#include <string>
#include <cstdio>
#include <ctime>

enum{
    Info = 0,
    Debug,
    Warning,
    Error
};

#define DEFAULE_LOG_LEVEL Debug

#define Log(level, format, ...) do{ \
    if((level) < (DEFAULE_LOG_LEVEL)) break; \
    time_t current_time = time(nullptr); \
    struct tm* ltm = localtime(&current_time); \
    char tmp[32] = {0}; \
    strftime(tmp, sizeof(tmp) - 1, "%H:%M:%S", ltm); \
    std::string lev; \
    switch(level){ \
        case 0 : lev = "Info"; break; \
        case 1 : lev = "Debug"; break; \
        case 2 : lev = "Error"; break; \
        default : lev = "Unknown"; break; \
    } \
    fprintf(stdout, "[%s] [%s %s:%d] " format "\n", lev.c_str(), tmp, __FILE__, __LINE__, ##__VA_ARGS__); \
    }while(false)

#define Info_Log(format, ...) Log(Info, format, ##__VA_ARGS__)
#define Debug_Log(format, ...) Log(Debug, format, ##__VA_ARGS__)
#define Warning_Log(format, ...) Log(Warning, format, ##__VA_ARGS__)
#define Error_Log(format, ...) Log(Error, format, ##__VA_ARGS__)