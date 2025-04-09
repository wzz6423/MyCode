#pragma once

#include "Common.h"
#include "EventLoop.hpp"
#include "Socket.hpp"
#include "Log.hpp"

class Listener{
public:
    void Accepter(std::shared_ptr<Connection> connection)
    {
        while (true)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(len);
            int sock = ::accept(connection->SockFd(), (struct sockaddr *)&peer, &len);
            if (sock > 0)
            {
                uint16_t peerport = ntohs(peer.sin_port);
                char ipbuf[128];
                inet_ntop(AF_INET, &peer.sin_addr.s_addr, ipbuf, sizeof(ipbuf));
                lg(Debug, "get a new link, get info-> [%S : %d], sock: %d", ipbuf, peerport, sock);

                SetNonBlockOrDie(sock);

                connection->_event_loop_ptr.lock()->AddConnection(sock, EVENT_IN,
                                  std::bind(&EventLoop::Recver, connection->_event_loop_ptr.lock(), std::placeholders::_1),
                                  std::bind(&EventLoop::Sender, connection->_event_loop_ptr.lock(), std::placeholders::_1),
                                  std::bind(&EventLoop::Excepter, connection->_event_loop_ptr.lock(), std::placeholders::_1),
                                  ipbuf, peerport); 
            }
            else
            {
                if (errno == EWOULDBLOCK)
                {
                    break;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
        }
    }
    std::shared_ptr<Sock> _listensock_ptr;
};