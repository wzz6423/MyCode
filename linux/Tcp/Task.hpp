#pragma once

#include <iostream>
#include <string>
#include "Log.hpp"
#include "Init.hpp"

Init init;

class Task{
public:
    Task(){}

    Task(int sockfd, const std::string& clientip, const uint16_t& clientport)
        :sockfd_(sockfd),
        client_ip_(clientip),
        client_port_(clientport)
    {}

    void run(){
        char buffer[4096];
        ssize_t n = read(sockfd_, buffer, sizeof(buffer) - 1);
        if(n > 0){
            buffer[n] = 0;
            std::cout << "client key# " << buffer << std::endl;
            std::string echo_string = init.translation(buffer);

            n = write(sockfd_, echo_string.c_str(), echo_string.size());
            if(n < 0){
                lg(Warning, "write error, errno: %d, error string: %s", errno, strerror(errno));
            }
        }
        else if(n == 0){
            lg(Info, "%s:%d quit, server close sockfd: %d", client_ip_.c_str(), client_port_, sockfd_);
        }
        else{
            lg(Warning, "read error, sockfd: %d, clientip: %s, clientport: %d", sockfd_, client_ip_.c_str(), client_port_);
        }
        close(sockfd_);
    }

    void operator()(){
        run();
    }

    ~Task(){}
private:
    int sockfd_;
    std::string client_ip_;
    uint16_t client_port_;
};