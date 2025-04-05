#pragma once

#include <iostream>
#include <memory>
#include <sys/epoll.h>
#include "Socket.hpp"
#include "Epoller.hpp"
#include "Log.hpp"
#include "nocopy.hpp"

uint32_t EVENT_IN = (EPOLLIN);
uint32_t EVENT_OUT = (EPOLLOUT);

class EpollServer : public nocopy{
private:
    static const int num = 64;
public:
    EpollServer(uint16_t port)
        :_listensock_ptr(new Sock()),
        _epoller_ptr(new Epoller()),
        _port(port)
    {}

    void Init(){
        _listensock_ptr->Socket();
        _listensock_ptr->Bind(_port);
        _listensock_ptr->Listen();

        lg(Info, "create listen socket success: %d\n", _listensock_ptr->Fd());
    }

    void Accepter(){
        std::string clientip;
        uint16_t clientport;
        int sock = _listensock_ptr->Accept(&clientip, &clientport);
        if(sock > 0){
            _epoller_ptr->EpollerUpdate(EPOLL_CTL_ADD, sock, EVENT_IN);
            lg(Info, "get a new link, client info@ %s : %d", clientip.c_str(), clientport);
        }
    }

    void Recver(int fd){
        char buffer[1024];
        ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
        if(n > 0){
            buffer[n] = 0;
            std::cout << "get a message: " << buffer << std::endl;

            std::string echo_string = "server echo $ ";
            echo_string += buffer;
            write(fd, echo_string.c_str(), echo_string.size());
        }
        else if(n == 0){
            lg(Info, "client quit, me too, close fd: %d", fd);
            _epoller_ptr->EpollerUpdate(EPOLL_CTL_DEL, fd, 0);
            close(fd);
        }
        else{
            lg(Warning, "recv error, close fd: %d", fd);
            _epoller_ptr->EpollerUpdate(EPOLL_CTL_DEL, fd, 0);
            close(fd);
        }
    }

    void Dispatcher(struct epoll_event revs[], int num){
        for(size_t i = 0; i < num; ++i){
            uint32_t events = revs[i].events;
            int fd = revs[i].data.fd;
            if(events & EVENT_IN){
                if(fd == _listensock_ptr->Fd()){
                    Accepter();
                }
                else{
                    Recver(fd);
                }
            }
            else if(events & EVENT_OUT){
                // ...
            }
            else{
                // ...
            }
        }
    }

    void Start(){
        _epoller_ptr->EpollerUpdate(EPOLL_CTL_ADD, _listensock_ptr->Fd(), EVENT_IN);
        struct epoll_event revs[num];
        while(true){
            int n = _epoller_ptr->EpollerWait(revs, num);
            if(n > 0){
                lg(Debug, "event happened, fd is: %d", revs[0].data.fd);
                Dispatcher(revs, n);
            }
            else if(n == 0){
                lg(Info, "time out ...");
            }
            else{
                lg(Error, "epoll wait error...");
            }
        }
    }

    ~EpollServer(){
        _listensock_ptr->Close();
    }
private:
    std::shared_ptr<Sock> _listensock_ptr;
    std::shared_ptr<Epoller> _epoller_ptr;
    uint16_t _port;
};