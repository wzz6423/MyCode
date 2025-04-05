#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <pthread.h>
#include <unistd.h>

struct ThreadInfo
{
    pthread_t tid;
    std::string name;
};

template <typename T>
class ThreadPool
{
public:
    void Lock()
    {
        pthread_mutex_lock(&mutex_);
    }

    void Unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }

    void ThreadSleep()
    {
        pthread_cond_wait(&cond_, &mutex_);
    }

    void WeakUp()
    {
        pthread_cond_signal(&cond_);
    }

    bool IsQueueEmpty()
    {
        return tasks_.empty();
    }

    std::string GetThreadName(pthread_t tid)
    {
        for (const auto &t : threads_)
        {
            if (t.tid_ == tid)
            {
                return t.name_;
            }
        }
        return "None";
    }

public:
    static void *HandlerTask(void *args)
    {
        ThreadPool<T>* tp = static_cast<ThreadPool<T>*>(args);
        std::string name = tp->GetThreadName(pthread_self());
        while(true){
            tp->Lock();

            while(tp->IsQueueEmpty()){
                tp->ThreadSleep();
            }
            T t = tp->Pop();

            tp->Unlock();
            t();
        }
    }

    void Start()
    {
        int num = threads_.size();
        for(int i = 0; i < num; ++i){
            threads_[i].name_ = "thread-" + std::to_string(i + 1);
            pthread_create(&(threads_[i].tid), nullptr, HandlerTask, this);
        }
    }

    void Push(const T &t)
    {
        Lock();
        tasks_.push(t);
        WeakUp();
        Unlock();
    }

    T Pop()
    {
        T t = tasks_.front();
        tasks_.pop();
        return t;
    }

    static ThreadPool<T> *GetInstance() // 线程池主线程单例
    {
        if(nullptr == tp_){ // 双判断，保证安全的同时减少对锁的申请占用次数提高效率（只有第一次没有创建对象）
            pthread_mutex_lock(&lock_);
            if(nullptr == tp_){
                tp_ = new ThreadPool<T>();
            }
            pthread_mutex_unlock(&mutex_);
        }

        return tp_;
    }

private:
    ThreadPool(int num = deafultnum)
        : threads_(num)
    {
        pthread_mutex_init(&mutex_, nullptr);
        pthread_cond_init(&cond_, nullptr);
    }

    ~ThreadPool()
    {
        pthread_mutex_destory(&mutex_);
        pthread_cond_destory(&cond_);
    }

    ThreadPool(const ThreadPool<T> &) = delete;
    const ThreadPool<T> &operator=(const ThreadPool<T> &) = delete;

private:
    std::vector<ThreadInof> threads_;
    std::queue<T> tasks_;

    pthread_mutex_t mutex_;
    pthread_cond_t cond_;

    static ThreadPool<T> *tp_;
    static pthread_mutex_t lock_;
};

template <typename T>
ThreadPool<T> *ThreadPool<T>::tp_ = nullptr;

template <typename T>
pthread_mutex_t ThreadPool<T>::lock_ = PTHREAD_MUTEX_INITIALIZER;