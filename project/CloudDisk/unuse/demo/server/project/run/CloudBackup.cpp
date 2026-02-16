// 切换 Config.hpp 中配置文件的路径
// 切换 Server.hpp 中 html 文件路径

#include "../server.h"

CloudBackUp::Data::DataManager* _data;

void HotManager()
{
    CloudBackUp::HotManager hot;
    hot.Run();
}
void Server()
{
    CloudBackUp::Server::Server srv;
    srv.Run();
}
auto main() -> int
{
    _data = new CloudBackUp::Data::DataManager();
    std::thread hotManager(HotManager);
    std::thread serverManager(Server);

    hotManager.join();
    serverManager.join();

    return 0;
}