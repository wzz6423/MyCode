#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include "Log.hpp"
#include "ThreadPool.hpp"
#include "Task.hpp"
#include "Daemon.hpp"

const int deafultfd = -1;
const std::string defaultip = "0.0.0.0";
const int backlog = 10;

enum{
    Usage_Error = 1,
    Socket_Error,
    Bind_Error,
    Listen_Error
};

class TcpServer;

class ThreadData{
public:
    ThreadData(int& fd, const std::string& ip, const uint16_t& p, TcpServer* t)
        :_sockfd(fd),
        _client_ip(ip),
        _client_port(p),
        _tsvr(t)
        {}
public:
    int _sockfd;
    std::string _client_ip;
    uint16_t _client_port;
    TcpServer* _tsvr;
};

class TcpServer{
public:
    TcpServer(const uint16_t& port, const std::string& ip = defaultip)
        :listen_sock_(deafultfd),
        port_(port),
        ip_(ip)
    {}

    void InitServer(){
        listen_sock_ = socket(AF_INET, SOCK_STREAM, 0);
        if(listen_sock_ < 0){
            lg(Fatal, "create socket error: %d, error string: %s", errno, strerror(errno));
            exit(Socket_Error);
        }
        lg(Info, "create socket success, listen_sock_: %d", listen_sock_);

        int opt = 1;
        setsockopt(listen_sock_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port_);
        inet_aton(ip_.c_str(), &(local.sin_addr));
        // local.sin_addr.s_addr = INADDR_ANY;

        if(bind(listen_sock_, (struct sockaddr*)&local, sizeof(local)) < 0){
            lg(Fatal, "bind error, errno: %d, error string: %s", errno, strerror(errno));
            exit(Bind_Error);
        }
        lg(Info, "bind socket success, listen_sock_: %d", listen_sock_);

        if(listen(listen_sock_, backlog) < 0){
            lg(Fatal, "listen error, errno: %d, error string: %s", errno, strerror(errno));
            exit(Listen_Error);
        }
        lg(Info, "listen success, listen_sock_: %d", listen_sock_);
    }

    static void* Routine(void* args){
        pthread_detach(pthread_self());
        ThreadData* td = static_cast<ThreadData*>(args);
        td->_tsvr->Service(td->_sockfd, td->_client_ip, td->_client_port);
        close(td->_sockfd);
        delete td;
        return nullptr;
    }

    void Service(int sockfd, const std::string& clientip, const uint16_t& clientport){
        char buffer[4096];
        while(true){
            ssize_t n = read(sockfd, buffer, sizeof(buffer) - 1);
            if(n > 0){
                buffer[n] = 0;
                std::cout << "client say@ " << buffer << std::endl;
                std::string echo_string = "tcpserver echo# ";
                echo_string += buffer;

                write(sockfd, echo_string.c_str(), echo_string.size());
            }
            else if(n == 0){
                lg(Info, "%s:%d quit, server close sockfd: %d", clientip.c_str(), clientport, sockfd);
                break;
            }
            else{
                lg(Warning, "read error, sockfd: %d, client ip: %s, client port: %d", sockfd, clientip.c_str(), clientport);
                break;
            }
        }
    }

    void Start(){
        Daemon();
        ThreadPool<Task>::GetInstance()->Start();
        lg(Info, "tcpserver is running...");

        while(true){
            // 获取新连接
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            int sockfd = accept(listen_sock_, (struct sockaddr*)&client, &len);
            if(sockfd < 0){
                lg(Warning, "accept error, errno: %d, error string: %s", errno, strerror(errno));
                continue;
            }
            lg(Info, "accept success, errno: %d, error string: %s", errno, strerror(errno));
            uint16_t clientport = ntohs(client.sin_port);
            char clientip[32];
            inet_ntop(AF_INET, &(client.sin_addr), clientip, sizeof(clientip));
            lg(Info, "get a new link..., sockfd: %d, client ip:%s, client port: %d", sockfd, clientip, clientport);

            // 单进程
            // Service(sockfd, clientip, clientport);
            // close(sockfd);

            // 多进程
            // pid_t id = fork();
            // if(id == 0){
            //     // child
            //     close(listen_sock_);
            //     if(fork() > 0){
            //         exit(0);
            //     }
            //     Service(sockfd, clientip, clientport);
            //     close(sockfd);
            //     exit(0);
            // }
            // close(sockfd);
            // pid_t rid = waitpid(id, nullptr, 0);
            // (void)rid;

            // 多线程
            // ThreadData* td = new ThreadData(sockfd, clientip, clientport, this);
            // pthread_t tid;
            // pthread_create(&tid, nullptr, Routine, td);

            // 线程池
            Task t(sockfd, clientip, clientport);
            ThreadPool<Task>::GetInstance()->Push(t);            
        }
    }

    ~TcpServer(){}
private:
    int listen_sock_;
    uint16_t port_;
    std::string ip_;
};