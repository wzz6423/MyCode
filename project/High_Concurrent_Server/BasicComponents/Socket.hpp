#pragma once

#include <iostream>
#include <string>
#include <optional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "SimpleLog.hpp"

#define MAX_LISTEN_SIZE 1024
class Socket{
public:
    Socket()
        :_sockfd(-1)
    {}

    Socket(int sockfd)
        :_sockfd(sockfd)
    {}

    ~Socket(){
        Close();
    }

    // 获取套接字
    int Fd(){
        return _sockfd;
    }

    // 创建套接字
    std::optional<int> Create(){
        // int socket(int domain, int type, int protocol);
        _sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(_sockfd < 0){
            Error_Log("create socket error!\n");
            return std::nullopt;
        }
        else{
            return _sockfd;
        }
    }

    // 绑定地址信息
    bool Bind(const std::string& ip, const uint16_t& port){
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        socklen_t len = sizeof(addr);
        // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
        int ret = bind(_sockfd, reinterpret_cast<struct sockaddr*>(&addr), len);
        if(ret < 0){
            Error_Log("bind address error!\n");
            return false;
        }
        else{
            return true;
        }
    }

    // 监听
    bool Listen(int backlog = MAX_LISTEN_SIZE){
        // int listen(int sockfd, int backlog);
        int ret = listen(_sockfd, backlog);
        if(ret < 0){
            Error_Log("listen error!\n");
            return false;
        }
        else{
            return true;
        }
    }

    // 向服务器发起连接
    bool Connect(const std::string& ip, const uint16_t port){
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        socklen_t len = sizeof(addr);
        // int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
        int ret = connect(_sockfd, reinterpret_cast<struct sockaddr*>(&addr), len);
        if(ret < 0){
            Error_Log("connect address error!\n");
            return false;
        }
        else{
            return true;
        }
    }

    // 获取新连接
    int Accept(){
        // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        int newfd = accept(_sockfd, nullptr, nullptr);
        if(newfd < 0){
            Error_Log("accept address error!\n");
            return -1;
        }
        else{
            return newfd;
        }
    }

    int Accept(std::string& ip, uint16_t& port){
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        socklen_t len = sizeof(addr);
        // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        int newfd = accept(_sockfd, reinterpret_cast<struct sockaddr*>(&addr), &len);
        if(newfd < 0){
            Error_Log("accept address error!\n");
            return -1;
        }
        else{
            ip = addr.sin_addr.s_addr;
            port = ntohs(addr.sin_port);
            return newfd;
        }
    }

    // 接收数据
    ssize_t Recv(void* buf, size_t len, int flag = 0){
        // ssize_t recv(int sockfd, void *buf, size_t len, int flags);
        ssize_t ret = recv(_sockfd, buf, len, flag);
        if(ret <= 0){
            // EAGAIN 表示当前 socket 接收缓冲区中没有数据, 在非阻塞情况下会出现这个错误
            // EINTR 表示当前 socket 的阻塞等待被信号打断了
            if(errno == EAGAIN || errno == EINTR){
                return 0; // 表示本次没有接收到数据
            }
            else{
                Error_Log("recv error!\n");
                return -1;
            }
        }
        else{
            return ret; // 实际接收到的数据长度
        }
    }

    // 非阻塞接收数据
    ssize_t NonBlockRecv(void* buf, size_t len){
        return Recv(buf, len, MSG_DONTWAIT); // MSG_DONTWAIT 表示当前接收为非阻塞
    }

    // 发送数据
    ssize_t Send(const void* buf, size_t len, int flag = 0){
        if(len == 0){
            return 0;
        }
        // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
        ssize_t ret = send(_sockfd, buf, len, flag);
        if(ret <= 0){
            // EAGAIN 表示当前 socket 接收缓冲区中没有数据, 在非阻塞情况下会出现这个错误
            // EINTR 表示当前 socket 的阻塞等待被信号打断了
            if(errno == EAGAIN || errno == EINTR){
                return 0; // 表示本次没有接收到数据
            }
            else{
                Error_Log("send error!\n");
                return -1;
            }
        }
        else{
            return ret; // 实际发送出去的数据长度
        }
    }

    // 非阻塞发送数据
    ssize_t NonBlockSend(void* buf, size_t len){
        if(len == 0){
            return 0;
        }
        return Send(buf, len, MSG_DONTWAIT); // MSG_DONTWAIT 表示当前接收为非阻塞
    }

    // 关闭套接字
    void Close(){
        if(_sockfd != -1){
            close(_sockfd);
            _sockfd = -1;
        }
    }

    // 创建一个服务端连接
    bool CreateServer(uint16_t port, const std::string& ip = "0.0.0.0", bool blockFlag = false){
        // 创建套接字
        if(Create() == std::nullopt){
            return false;
        }
        // 设置非阻塞
        if(blockFlag){
            if(SetNonBlock() == false){
                return false;
            }
        }

        // 绑定地址
        if(Bind(ip, port) == false){
            return false;
        }

        // 开始监听
        if(Listen() == false){
            return false;
        }

        // 设置地址端口重用
        ReuseAddress();

        return true;
    }

    // 创建一个客户端连接
    bool CreateClient(uint16_t port, const std::string& ip){
        // 创建套接字
        if(Create() == std::nullopt){
            return false;
        }

        // 指向连接服务器
        if(Connect(ip, port) == false){
            return false;
        }

        return true;
    }

    // 设置套接字 ip 地址端口重用
    bool ReuseAddress(){
        // int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
        int val = 1;
        setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, static_cast<void*>(&val), sizeof(int)); // ip地址
        val = 1;
        setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT, static_cast<void*>(&val), sizeof(int)); // 端口号
    }

    // 设置套接字非阻塞
    bool SetNonBlock(){
        // int fcntl(int fd, int cmd, ... /* arg */ );
        int flag = fcntl(_sockfd, F_GETFL, 0);
        fcntl(_sockfd, F_SETFL, flag | O_NONBLOCK);
    }
private:
    int _sockfd; // 套接字
};