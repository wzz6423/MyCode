#include "MsgQueue.hpp"

int main()
{
    Client client;
    while (true)
    {
        // 只让client发送消息
        std::string input;
        std::cout << "Please input message: ";
        std::getline(std::cin, input);
        client.Send(MSG_TYPE_CLIENT, input);
        if (input == "exit")
        {
            break;
        }
    }
    return 0;
}