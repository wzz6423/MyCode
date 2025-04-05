#pragma once

#include <iostream>
#include <vector>
#include <semaphore.h>
#include <pthread.h>

static const int defaultcap = 5;

template <typename T>
class RingQueue{
public:
    RingQueue(int cap = defaultcap)
        :cap_(cap),
        ringqueue_(cap),
        c_step_(0),
        p_step_(0)
    {
        sem_intit(&cdata_sem, 0, 0);
        sem_intit(&pspace_sem, 0, 0);

        pthread_mutex_intit(&c_mutex_, nullptr);
        pthread_mutex_intit(&p_mutex_, nullptr);
    }

    void Push(const T& in){
        P(pspace_sem);

        Lock(p_mutex_);
        ringqueue_[p_step_++] = in;
        p_step_ %= cap_;
        Unlock(p_mutex_);

        V(cdata_sem);
    }

    void Pop(T* out){
        P(cdata_sem);

        Lock(c_mutex_);
        *out = ringqueue_[c_step_++];
        c_step_ %= cap_;
        Unlock(c_mutex_);

        P(pspace_sem);
    }

    ~RingQueue(){
        sem_destroy(&cdata_sem);
        sem_destroy(&pspace_sem);

        pthread_mutex_destroy(&c_mutex_);
        pthread_mutex_destroy(&p_mutex_);
    }
private:
    void P(sem_t& sem){
        sem_wait(&sem);
    }

    void V(sem_t& sem){
        sem_post(&sem);
    }

    void Lock(pthread_mutex_t& mutex){
        pthread_mutex_lock(&mutex);
    }

    void Unlock(pthread_mutex_t& mutex){
        pthread_mutex_unlock(&mutex);
    }
private:
    std::vector<T> ringqueue_;
    int cap_;

    int c_step_;
    int p_step_;

    sem_t cdata_sem;
    sem_t pspace_sem;

    pthred_mutex_t c_mutex_;
    pthred_mutex_t p_mutex_;
};