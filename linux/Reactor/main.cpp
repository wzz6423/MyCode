#include <iostream>
#include <functional>
#include <memory>
#include "Log.hpp"
#include "TcpServer.hpp"
#include "Calculator.hpp"

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

    auto tcpserver_ptr = connection_ptr->_tcp_server_ptr.lock();
    tcpserver_ptr->Sender(connection_ptr);
}

int main(){
    std::shared_ptr<TcpServer> epoll_svr = std::make_shared<TcpServer>(6423, DefaultOnMessage);
    epoll_svr->Init();
    epoll_svr->Loop();

    return 0;
}