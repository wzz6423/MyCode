#pragma once

#include <iostream>
#include <string>
#include <ctime>
#include <pthread.h>
#include <functional>

using callback_t = std::function<void()>;
static int num = 1;

class Thread{
public:
    static void* Routine(void* args){
        Thread* thread = static_cast<Thread*>(args);
        thread->Entery();
        return nullptr;
    }
public:
    Thread(callback_t cb)
        :cb_(cb)
    {}

    void Run(){
        name_ = "thread-" + std::to_string(num++);
        start_timestamp_ = time(nullptr);
        isrunning_ = true;
        pthread_create(&tid_, nullptr, Routine, this);
    }

    void Join(){
        pthread_join(tid_, nullptr);
        isrunning_ = false;
    }

    void Entery(){
        cb_();
    }

    std::string Name(){
        return name_;
    }

    uint64_t StartTimestamp(){
        return start_timestamp_;
    }

    ~Thread(){}
private:
    pthread_t tid_;
    std::string name_;
    uint64_t start_timestamp_;
    bool isrunning_;

    callback_t cb_;
};