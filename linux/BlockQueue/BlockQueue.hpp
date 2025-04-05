#pragma once

#include <iostream>
#include <queue>
#include <pthread.h>

template <typename T>
class BlockQueue{
private:
    static const int defaultnum = 20;
public:
    BlockQueue(int maxcap = defaultnum)
        maxcap_(maxcap_)
    {
        pthread_mutex_init(&mutex_, nullptr);
        pthread_cond_init(&c_cond_, nullptr);
        pthread_cond_init(&p_cond_, nullptr);
    }

    T pop(){
        pthread_mutex_lock(&mutex_);
        while(q.size() == 0){
            pthread_cond_wait(&c_cond_, &mutex_);
        }

        T out = q_.front();
        q_pop();

        pthread_cond_signal(&p_cond_);
        pthread_mutex_unlock(&mutex_);

        return out;
    }

    void push(const T& in){
        pthread_mutex_lock(&mutex_);
        while(q_.size() == maxcap_){
            pthread_cond_wait(&p_cond_, &mutex_);
        }

        q_.push(T);

        pthread_cond_signal(&c_cond_);
        pthread_mutex_unlock(&mutex_);
    }

    ~BlockQueue(){
        pthread_mutex_destory(&mutex_);
        pthread_cond_destory(&c_cond_);
        pthread_cond_destory(&p_cond_);
    }
private:
    std::queue<T> q_;
    int maxcap_;
    pthread_mutex_t mutex_;
    pthread_cond_t c_cond_;
    pthread_cond_t p_cond_;

    // int low_water_;
    // int high_water_;
};