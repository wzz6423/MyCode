#pragma once 

#include <iostream>
#include <sys/select.h>
#include <sys/time.h>
#include "Socket.hpp"

static const uint16_t defaultport = 6423;
static const int fd_num_max = (sizeof(fd_set) * 8);
int defaultfd = -1;

class SelectServer{
public:
    SelectServer(uint16_t port = defaultport)
        :_port(port)
    {
        for(int i = 0; i < fd_num_max; ++i){
            fd_array[i] = defaultfd;
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
            if(fd_array[pos] == defaultfd){
                break;
            }
        }
        if(pos == fd_num_max){
            lg(Warning, "server fd_array is full, close %d now!", sock);
            close(sock);
        }
        else{
            fd_array[pos] = sock;
            PrintFd();
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
            lg(Info, "client quit, me too, close fd is : %d", fd);
            close(fd);
            fd_array[pos] = defaultfd;
        }
        else{
            lg(Warning, "recv error: fd is : %d", fd);
            close(fd);
            fd_array[pos] = defaultfd;
        }
    }

    void Dispatcher(fd_set& rfds){
        for(int i = 0; i < fd_num_max; ++i){
            int fd = fd_array[i];
            if(fd != defaultfd){
                if(FD_ISSET(fd, &rfds)){
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
        int linstensock = _listensock.Fd();
        fd_array[0] = linstensock;
        while(true){
            fd_set rfds;
            FD_ZERO(&rfds);

            int maxfd = fd_array[0];
            for(int i = 0; i < fd_num_max; ++i){
                if(fd_array[i] != defaultfd){
                    FD_SET(fd_array[i], &rfds);
                    if(maxfd < fd_array[i]){
                        maxfd = fd_array[i];
                        lg(Info, "max fd update, max fd is: %d", maxfd);
                    }
                }
            }

            struct timeval timeout = {0, 0};
            int n = select(maxfd + 1, &rfds, nullptr, nullptr, /* timeout */ nullptr);
            switch(n){
                case 0:
                    std::cout << "time out, timeout: " << timeout.tv_sec << "." << timeout.tv_usec << std::endl;
                    break;
                case 1:
                    std::cerr << "select error" << std::endl;
                    break;
                default:
                    std::cout << "get a new link!" << std::endl;
                    Dispatcher(rfds);
                    break;
            }
        }
    }

    void PrintFd(){
        std::cout << "online fd list: " ;
        for(int i = 0; i < fd_num_max; ++i){
            if(fd_array[i] != defaultfd){
                std::cout << fd_array[i] << " ";
            }
        }
        std::cout << std::endl;
    }

    ~SelectServer(){
        _listensock.Close();
    }

private:
    Sock _listensock;
    uint16_t _port;
    int fd_array[fd_num_max];
    // int wfd_array[fd_num_max];
};