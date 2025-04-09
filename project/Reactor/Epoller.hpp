#pragma once

#include "Common.h"
#include "nocopy.hpp"
#include "Log.hpp"

class Epoller : public nocopy{
private:
    static const int size = 128;
public:
    Epoller(){
        _epfd = epoll_create1(size);
        if(_epfd == -1){
            lg(Error, "epoll_create error: %s", strerror(errno));
        }
        else{
            lg(Info, "epoll_create success: %d", _epfd);
        }
    }

    int EpollerWait(struct epoll_event revents[], int num, int timeout){
        int n = epoll_wait(_epfd, revents, num, /* _timeout 0 */ timeout);
        return n;
    }

    int EpollerUpdate(int oper, int sock, uint32_t event){
        int n = 0;
        if(oper == EPOLL_CTL_DEL){
            n = epoll_ctl(_epfd, oper, sock, nullptr);
            if(n != 0){
                lg(Error, "epoll_ctl delete error!");
            }
        }
        else{
            // EPOLL_CTL_MOD || EPOLL_CTL_ADD
            struct epoll_event ev;
            ev.events = event;
            ev.data.fd = sock;

            n = epoll_ctl(_epfd, oper, sock, &ev);
            if(n != 0){
                lg(Error, "epoll_ctl error!");
            }
        }
        return n;
    }

    ~Epoller(){
        if(_epfd >= 0){
            close(_epfd);
        }
    }
private:
    int _epfd;
    // int _timeout{3000}; // 30s
};