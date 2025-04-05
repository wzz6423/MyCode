#include "SelectServer.hpp"
#include <memory>

int main(){
    std::unique_ptr<SelectServer> svr = std::make_unique<SelectServer>();
    svr->Init();
    svr->Start();

    return 0;
}