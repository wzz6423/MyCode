#include "SharedMem.hpp"
#include <string.h>

int main()
{
    MmapMemoryClient cli;
    std::string who;
    while (true)
    {
        std::cout << "Please Enter# ";
        std::getline(std::cin, who);
        cli.SendMessage(who);
        if (who == "end")
        {
            break;
        }
    }

    return 0;
}
