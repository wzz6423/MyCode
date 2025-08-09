#pragma once

#include <iostream>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define SIZE 4096

class SafeObj
{
public:
    SafeObj()
    {
    }
    void InitObj()
    {
        // 初始化锁
        pthread_mutexattr_t mattr;
        pthread_mutexattr_init(&mattr);
        pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED); // 设置进程间共享
        pthread_mutex_init(&lock, &mattr);

        // 初始化条件变量
        pthread_condattr_t cattr;
        pthread_condattr_init(&cattr);
        pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED); // 设置进程间共享
        pthread_cond_init(&cond, &cattr);

        // 缓冲区清空
        memset(buffer, 0, sizeof(buffer));
    }
    void CleanupObj()
    {
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&cond);
    }
    void LockObj()
    {
        int n = ::pthread_mutex_lock(&lock);
        (void)n;
    }
    void UnlockObj()
    {
        int n = ::pthread_mutex_unlock(&lock);
        (void)n;
    }
    void Wait()
    {
        int n = ::pthread_cond_wait(&cond, &lock);
        (void)n;
    }
    void Signal()
    {
        int n = ::pthread_cond_signal(&cond);
        (void)n;
    }
    void BroadCast()
    {
        int n = ::pthread_cond_broadcast(&cond);
        if (n != 0)
            std::cerr << "broadcast error" << std::endl;
        else
            std::cerr << "broadcast succcess" << std::endl;
    }
    void GetContent(std::string *out)
    {
        *out = buffer;
    }
    void SetContent(const std::string &in)
    {
        memset(buffer, 0, sizeof(buffer));
        strncpy(buffer, in.c_str(), in.size());
    }
    ~SafeObj()
    {
    }

private:
    pthread_mutex_t lock;
    pthread_cond_t cond;
    char buffer[SIZE]; // 未来进程共享的数据区域
};

// 映射的文件，必须以/开头，这是mmap的约定
#define SHARED_MEMORY_FILE "/shm"
#define SHARED_MEMORY_SIZE sizeof(SafeObj)

class MmapMemory
{
public:
    MmapMemory(const std::string &file, int size) : _file(file), _size(size), _fd(-1), _mmap_addr(nullptr)
    {
    }
    void OpenFile()
    {
        _fd = ::shm_open(_file.c_str(), O_CREAT | O_RDWR, 0666);
        if (_fd < 0)
        {
            perror("shm_open");
            exit(1);
        }
    }
    void TruncSharedMemory()
    {
        int n = ftruncate(_fd, _size);
        if (n < 0)
        {
            perror("ftruncate");
            exit(2);
        }
    }
    void *Mmap()
    {
        _mmap_addr = ::mmap(nullptr, _size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
        if (_mmap_addr == MAP_FAILED)
        {
            perror("mmap");
            exit(3);
        }
        return _mmap_addr;
    }
    void RemoveFile()
    {
        int n = ::shm_unlink(_file.c_str());
        if (n < 0)
        {
            perror("shm_unlink");
            exit(4);
        }
    }
    void *MmapAddr()
    {
        return _mmap_addr;
    }
    ~MmapMemory()
    {
        if (_fd > 0)
        {
            ::close(_fd);
            std::cout << "关闭mmap文件" << std::endl;
        }

        int n = ::munmap(_mmap_addr, _size);
        if (n == 0)
        {
            std::cout << "munmap 完成" << std::endl;
        }
    }

private:
    int _fd;
    int _size;
    std::string _file;
    void *_mmap_addr;
};

class MmapMemoryServer : public MmapMemory
{
public:
    MmapMemoryServer() : MmapMemory(SHARED_MEMORY_FILE, SHARED_MEMORY_SIZE)
    {
        MmapMemory::OpenFile();
        MmapMemory::TruncSharedMemory();
        MmapMemory::Mmap();
        obj = static_cast<SafeObj *>(MmapMemory::MmapAddr());
        obj->InitObj();
    }
    // 每个进程都有一次读取数据的机会
    void RecvMessage(std::string *out)
    {
        obj->LockObj();
        obj->Wait();
        obj->GetContent(out);
        obj->UnlockObj();
    }
    ~MmapMemoryServer()
    {
        obj->CleanupObj();
        MmapMemory::RemoveFile();
    }

private:
    SafeObj *obj;
};

class MmapMemoryClient : public MmapMemory
{
public:
    MmapMemoryClient() : MmapMemory(SHARED_MEMORY_FILE, SHARED_MEMORY_SIZE)
    {
        MmapMemory::OpenFile();
        MmapMemory::Mmap();
        obj = static_cast<SafeObj *>(MmapMemory::MmapAddr());
    }
    void SendMessage(const std::string &in)
    {
        obj->LockObj();
        obj->SetContent(in);
        // SafeSharedMemory::Signal(); //唤醒进程
        obj->BroadCast(); // 把他们全部叫醒，让他们自己判断，谁应该活动
        obj->UnlockObj();
    }
    ~MmapMemoryClient()
    {
    }

private:
    SafeObj *obj;
};
