#include <memory>
#include "EpollServer.hpp"

int main(){
    std::unique_ptr<EpollServer> epoll_svr(new EpollServer(6423));
    epoll_svr->Init();
    epoll_svr->Start();

    return 0;
}