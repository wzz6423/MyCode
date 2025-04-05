#pragma once

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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

const int size = 4096;
const std::string path_name = "/home/wzz/test";
const int proj_id = 0x6666;

enum{
    FTOK_ERR = 4,
    CREAT_SHM_ERR
};

key_t GetKey(){
    key_t k = ftok(path_name.c_str(), proj_id);
    if(k < 0){
        lg(Fatal, "ftok error : %s", strerror(errno));
        exit(FTOK_ERR);
    }
    lg(Info, "ftok success , key is : 0x%x", k);
    return k;
}

int GetShareMemHelper(int flag){
    key_t k = GetKey();
    int shmid = shmget(k, size, flag);
    if(shmid < 0){
        lg(Fatal, "creat share memory error : %s", strerror(errno));
        exit(CREAT_SHM_ERR);
    }
    lg(Info, "create share memory success , shmid : %d", shmid);
    return shmid;
}

int CreateShm(){
    return GetShareMemHelper(IPC_CREAT | IPC_EXCL | 0666);
}

int GetShm(){
    return GetShareMemHelper(IPC_CREAT);
}