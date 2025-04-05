#include "PollServer.hpp"
#include <memory>

int main(){
    std::unique_ptr<PollServer> svr = std::make_unique<PollServer>();
    svr->Init();
    svr->Start();

    return 0;
}