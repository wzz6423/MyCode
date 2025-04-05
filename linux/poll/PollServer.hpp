#pragma once

#include <iostream>
#include <poll.h>
#include <sys/time.h>
#include "Socket.hpp"

static const uint16_t defaultport = 6423;
static const int fd_num_max = 64;
int defaultfd = -1;
int non_event = 0;

class PollServer{
public:
    PollServer(uint16_t port = defaultport)
        :_port(port)
    {
        for(size_t i = 0; i < fd_num_max; ++i){
            _event_fds[i].fd = defaultfd;
            _event_fds[i].events = non_event;
            _event_fds[i].revents = non_event;
        }
    }

    bool Init(){
        _listensock.Socket();
        _listensock.Bind(_port);
        _listensock.Listen();

        return true;
    }

    void Accepter(){
        std::string clientip;
        uint16_t clientport = 0;
        int sock = _listensock.Accept(&clientip, &clientport);
        if(sock < 0){
            return;
        }
        lg(Info, "accept success, %s : %d, sock fd: %d", clientip.c_str(), clientport, sock);

        int pos = 1;
        for(; pos < fd_num_max; ++pos){
            if(_event_fds[pos].fd == defaultfd){
                break;
            }
        }
        if(pos == fd_num_max){
            lg(Warning, "server is full, close %d now!", sock);
            close(sock);
        }
        else{
            _event_fds[pos].fd = sock;
            _event_fds[pos].events = POLLIN;
            _event_fds[pos].revents = non_event;
            // TODO
        }
    }

    void Recver(int fd, int pos){
        char buffer[1024];
        ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
        if(n > 0){
            buffer[n] = 0;
            std::cout << "get a message: " << buffer << std::endl;
        }
        else if(n == 0){
            lg(Info, "client quit, me too, close fd: %d", fd);
            _event_fds[pos].fd = defaultfd;
            close(fd);
        }
        else{
            lg(Warning, "recv error, fd: %d", fd);
            _event_fds[pos].fd = defaultfd;
            close(fd);
        }
    }

    void Dispatcher(){
        for(size_t i = 0; i < fd_num_max; ++i){
            int fd = _event_fds[i].fd;
            if(fd != defaultfd){
                if(_event_fds[i].revents & POLLIN){
                    if(fd == _listensock.Fd()){
                        Accepter();
                    }
                    else{
                        Recver(fd, i);
                    }
                }
            }
        }
    }

    void Start(){
        _event_fds[0].fd = _listensock.Fd();
        _event_fds[0].events = POLLIN;
        int timeout = 3000; // 3s
        while(true){
            int n = poll(_event_fds, fd_num_max, timeout);
            switch(n){
                case 0:
                    std::cout << "time out..." << std::endl;
                    break;
                case -1:
                    std::cerr << "poll error..." << std::endl;
                    break;
                default:
                    std::cout << "get a new link!" << std::endl;
                    Dispatcher();
                    break;
            }
        }
    }

    void PrintFd(){
        std::cout << "online fd list: ";
        for(size_t i = 0; i < fd_num_max; ++i){
            if(_event_fds[i].fd !=defaultfd){
                std::cout << _event_fds[i].fd << " ";
            }
        }
        std::cout << std::endl;
    }

    ~PollServer(){
        _listensock.Close();
    }
private:
    Sock _listensock;
    uint16_t _port;
    struct pollfd _event_fds[fd_num_max];

    // struct pollfd* _event_fds;
    // int fd_array[fd_num_max];
    // int wfd_array[fd_num_max];
};