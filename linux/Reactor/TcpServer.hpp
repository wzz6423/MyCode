#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <cerrno>
#include <cstring>
#include "Log.hpp"
#include "nocopy.hpp"
#include "Epoller.hpp"
#include "Socket.hpp"
#include "Command.hpp"

class Connection;
class TcpServer;

uint32_t EVENT_IN = (EPOLLIN | EPOLLET);
uint32_t EVENT_OUT = (EPOLLOUT | EPOLLET);
static const int g_buffer_size = 128;

using func_t = std::function<void(std::weak_ptr<Connection>)>;
using except_func = std::function<void(std::weak_ptr<Connection>)>;

class Connection{
public:
    Connection(int sock)
        :_sock(sock)
    {}

    void SetHandler(func_t recv_cb, func_t send_cb, except_func except_cb){
        _recv_cb = recv_cb;
        _send_cb = send_cb;
        _except_cb = except_cb;
    }

    int SockFd(){
        return _sock;
    }

    void AppendInBuffer(const std::string& info){
        _inbuffer += info;
    }

    void AppenOutBuffer(const std::string& info){
        _outbuffer += info;
    }

    std::string& InBuffer(){
        return _inbuffer;
    }

    std::string& OutBuffer(){
        return _outbuffer;
    }

    void SetWeakPtr(std::weak_ptr<TcpServer> tcp_server_ptr){
        _tcp_server_ptr = tcp_server_ptr;
    }

    ~Connection(){}
public:
    func_t _recv_cb;
    func_t _send_cb;
    except_func _except_cb;

    std::weak_ptr<TcpServer> _tcp_server_ptr;

    std::string _ip;
    uint16_t _port;
private:
    int _sock;
    std::string _inbuffer;
    std::string _outbuffer;
};

// enable_shared_from_this:可以提供返回当前对象的this对应的shared_ptr
class TcpServer : public std::enable_shared_from_this<TcpServer>, public nocopy{
private:
    static const int num = 64;
public:
    TcpServer(uint16_t port, func_t OnMessage)
        :_port(port),
        _OnMessage(OnMessage),
        _quit(true),
        _epoller_ptr(new Epoller()),
        _listensock_ptr(new Sock())
    {}

    void Init(){
        _listensock_ptr->Socket();
        SetNonBlockOrDie(_listensock_ptr->Fd());
        _listensock_ptr->Bind(_port);
        _listensock_ptr->Listen();
        lg(Info, "create listen socket success: %d", _listensock_ptr->Fd());
        AddConnection(_listensock_ptr->Fd(), EVENT_IN, std::bind(&TcpServer::Accepter, this, std::placeholders::_1), nullptr, nullptr);
    }

    void AddConnection(int sock, uint32_t event, func_t recv_cb, func_t send_cb, except_func except_cb, const std::string& ip = "0.0.0.0", uint16_t port = 0){
        std::shared_ptr<Connection> new_connection(new Connection(sock));
        
        new_connection->SetWeakPtr(shared_from_this());
        new_connection->SetHandler(recv_cb, send_cb, except_cb);
        new_connection->_ip = ip;
        new_connection->_port = port;

        _connections.insert(std::make_pair(sock, new_connection));

        _epoller_ptr->EpollerUpdate(EPOLL_CTL_ADD, sock, event);
    }

    void Accepter(std::weak_ptr<Connection> conn){
        auto connection = conn.lock();
        while(true){
            struct sockaddr_in peer;
            socklen_t len = sizeof(len);
            int sock = ::accept(connection->SockFd(), (struct sockaddr*)&peer, &len);
            if(sock > 0){
                uint16_t peerport = ntohs(peer.sin_port);
                char ipbuf[128];
                inet_ntop(AF_INET, &peer.sin_addr.s_addr, ipbuf, sizeof(ipbuf));
                lg(Debug, "get a new link, get info-> [%S : %d], sock: %d", ipbuf, peerport, sock);

                SetNonBlockOrDie(sock);

                AddConnection(sock, EVENT_IN, std::bind(&TcpServer::Recver, this, std::placeholders::_1), std::bind(&TcpServer::Sender, this, std::placeholders::_1), std::bind(&TcpServer::Excepter, this, std::placeholders::_1));
            }
            else{
                if(errno == EWOULDBLOCK){
                    break;
                }
                else if(errno == EINTR){
                    continue;
                }
                else{
                    break;
                }
            }
        }
    }

    void Recver(std::weak_ptr<Connection> conn){
        if(conn.expired()){
            return;
        }
        auto connection = conn.lock();
        int sock = connection->SockFd();
        while(true){
            char buffer[g_buffer_size];
            memset(buffer, 0, sizeof(buffer));
            ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if( n > 0){
                connection->AppendInBuffer(buffer);
            }
            else if(n == 0){
                lg(Info, "sockfd: %d, client info %s : %d quit...", sock, connection->_ip.c_str(), connection->_port);
                connection->_except_cb(connection);
                return;
            }
            else{
                if(errno == EWOULDBLOCK){
                    break;
                }
                else if(errno == EINTR){
                    continue;
                }
                else{
                    lg(Warning, "sockfd: %d, client info %s : %d recv error...", sock, connection->_ip.c_str(), connection->_port);
                    connection->_except_cb(connection);
                    return;
                }
            }
        }
        _OnMessage(connection);
    }

    void Sender(std::weak_ptr<Connection> conn){
        if(conn.expired()){
            return;
        }
        auto connection = conn.lock();
        auto& outbuffer = connection->OutBuffer();
        while(true){
            ssize_t n = send(connection->SockFd(), outbuffer.c_str(), outbuffer.size(), 0);
            if(n > 0){
                outbuffer.erase(0, n);
                if(outbuffer.empty()){
                    break;
                }
                else if(n == 0){
                    break;
                }
                else{
                    if(errno == EWOULDBLOCK){
                        break;
                    }
                    else if(errno == EINTR){
                        continue;
                    }
                    else{
                        lg(Warning, "sockfd: %d, client info %s : %d send error...", connection->SockFd(), connection->_ip.c_str(), connection->_port);
                        connection->_except_cb(connection);
                        return;
                    }
                }
            }
            if(!outbuffer.empty()){
                EnableEvent(connection->SockFd(), true, true);
            }
            else{
                EnableEvent(connection->SockFd(), true, false);
            }
        }
    }

    void Excepter(std::weak_ptr<Connection> conn){
        if(conn.expired()){
            return;
        }
        auto connection = conn.lock();

        int fd = connection->SockFd();
        lg(Warning, "Excepter hander sockds: %d, client info %s : %d excepter handler", connection->SockFd(), connection->_ip.c_str(), connection->_port);
        
        _epoller_ptr->EpollerUpdate(EPOLL_CTL_DEL, fd, 0);

        lg(Debug, "close %d done...", fd);
        close(fd);

        lg(Debug, "remove %d from _connections...\n", fd);
        _connections.erase(fd);
    }

    void EnableEvent(int sock, bool readable, bool writeable){
        uint32_t events = 0;
        events |= ((readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0) | EPOLLET);
        _epoller_ptr->EpollerUpdate(EPOLL_CTL_MOD, sock, events);
    }

    bool IsConnectionSafe(int fd){
        auto iter = _connections.find(fd);
        if(iter == _connections.end()){
            return false;
        }
        else{
            return true;
        }
    }

    void Dispatcher(int timeout){
        int n = _epoller_ptr->EpollerWait(revs, num, timeout);
        for(size_t i = 0; i < n; ++i){
            uint32_t events = revs[i].events;
            int sock = revs[i].data.fd;

            if((events & EPOLLIN) && IsConnectionSafe(sock)){
                if(_connections[sock]->_recv_cb){
                    _connections[sock]->_recv_cb(_connections[sock]);
                }
            }
            if((events & EPOLLOUT) && IsConnectionSafe(sock)){
                if(_connections[sock]->_send_cb){
                    _connections[sock]->_send_cb(_connections[sock]);
                }
            }
        }
    }

    void Loop(){
        _quit = false;

        while(!_quit){
            Dispatcher(-1);
            PrintConnection();
        }

        _quit = true;
    }

    void PrintConnection(){
        std::cout << "_connections fd list: ";
        for(auto& conn : _connections){
            std::cout << conn.second->SockFd() << ", ";
            std::cout << "inbuffer: " << conn.second->InBuffer().c_str();
        }
        std::cout << std::endl;
    }

    ~TcpServer(){}
private:
    std::shared_ptr<Epoller> _epoller_ptr;
    std::shared_ptr<Sock> _listensock_ptr;
    std::unordered_map<int, std::shared_ptr<Connection>> _connections;
    struct epoll_event revs[num];
    uint16_t _port;
    bool _quit;
    func_t _OnMessage;
};