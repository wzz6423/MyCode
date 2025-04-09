#include "Common.h"
#include "Calculator.hpp"
#include "Daemon.hpp"
#include "EventLoop.hpp"
#include "Log.hpp"

Calculator calculator;

void DefaultOnMessage(std::weak_ptr<Connection> conn){
    if(conn.expired()){
        return;
    }
    auto connection_ptr = conn.lock();

    std::cout << "get message: " << connection_ptr->InBuffer() << std::endl;
    std::string response_str = calculator.Handler(connection_ptr->InBuffer());
    if(response_str.empty()){
        return;
    }
    lg(Debug, "%s", response_str.c_str());
    connection_ptr->AppenOutBuffer(response_str);

    auto event_loop_ptr = connection_ptr->_event_loop_ptr.lock();
    event_loop_ptr->Sender(connection_ptr);
}

int main(){
    Daemon();
    std::shared_ptr<EventLoop> epoll_svr = std::make_shared<EventLoop>(6423, DefaultOnMessage);
    epoll_svr->Init();
    epoll_svr->Loop();

    return 0;
}