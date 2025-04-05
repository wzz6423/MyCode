#pragma once

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_FILE "./myfifo"
#define MODE 0664

enum {
    FIFO_CREATE_ERROR = 1,
    FIFO_DELETE_ERROR,
    FIFO_OPEN_ERROR
};

// pipe：创建匿名管道
// mkfifo：创建命名管道
class Init{
public:
    Init(){
        int n = mkfifo(FIFO_FILE, MODE);
        if(n == -1){
            perror("mkfifo");
            exit(FIFO_CREATE_ERROR);
        }
    }

    ~Init(){
        int m = unlink(FIFO_FILE);
        if(m == -1){
            perror("unlink");
            exit(FIFO_DELETE_ERROR);
        }
    }
};