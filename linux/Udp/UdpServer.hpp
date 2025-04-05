#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <strings.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>
#include "Log.hpp"

// typedef std::function<std::string(const std::string&)> func_t;
// using func_t = std::function<std::string(const std::string &)>;

enum
{
    SOCKET_ERROE = 1,
    BIND_ERROR
};

uint16_t defaultport = 6423;
std::string defaultip = "0.0.0.0";
const int size = 1024;

class UdpServer
{
public:
    UdpServer(const uint16_t &port = defaultport, const std::string &ip = defaultip)
        : sockfd_(0),
          port_(port),
          ip_(ip),
          isrunning_(false)
    {
    }

    void Init()
    {
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd_ < 0)
        {
            lg(Fatal, "socket create error, sockfd : %d", sockfd_);
            exit(SOCKET_ERROE);
        }
        lg(Info, "socket create success, sockfd: %d", sockfd_);

        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port_);
        local.sin_addr.s_addr = inet_addr(ip_.c_str());

        if (bind(sockfd_, (sockaddr *)&local, sizeof(local)) < 0)
        {
            lg(Fatal, "bind error, errno: %d, error string: %s", errno, strerror(errno));
            exit(BIND_ERROR);
        }
        lg(Info, "bind success,  errno: %d, error string: %s", errno, strerror(errno));
    }

    void Run(/* func_t func */)
    {
        isrunning_ = true;
        char inbuffer[size];
        while(isrunning_){
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            ssize_t n = recvfrom(sockfd_, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr*)&client, &len);
            if(n < 0){
                lg(Warning, "recvfron error, errno: %d, error string: %s", errno, strerror(errno));
                continue;
            }
            inbuffer[n] = 0;
            std::string info = inbuffer;

            uint16_t clientport = ntohs(client.sin_port);
            std::string clientip = inet_ntoa(client.sin_addr);
            CheckUser(client, clientip, clientport);

            Broadcast(info, clientip, clientport);
        }
    }

    void CheckUser(const struct sockaddr_in& client, const std::string clientip, uint16_t clientport){
        auto iter = online_user_.find(clientip);
        if(iter == online_user_.end()){
            online_user_.insert({clientip, client});
            std::cout << "[" << clientip << ":" << clientport << "]" << " comming..." << std::endl;
        }
    }

    void Broadcast(const std::string& info, const std::string& clientip, uint16_t clientport){
        std::string message = "[";
        message += clientip;
        message += ":";
        message += std::to_string(clientport);
        message += "]# ";
        message += info;
        for(const auto& usr: online_user_){
            socklen_t len = sizeof(usr.second);
            sendto(sockfd_, message.c_str(), message.size(), 0, (struct sockaddr*)(&usr.second), len);
        }
        std::cout << "forward send done, message: " << message << std::endl;
    }

    ~UdpServer()
    {
        if (sockfd_ > 0)
        {
            close(sockfd_);
        }
    }

private:
    int sockfd_;
    std::string ip_;
    uint16_t port_;
    bool isrunning_;
    std::unordered_map<std::string, struct sockaddr_in> online_user_;
};