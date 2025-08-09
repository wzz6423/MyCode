#include "SharedMem.hpp"
#include <sys/wait.h>
#include <sys/types.h>

void Active(MmapMemoryServer &svr, std::string processname)
{
    std::cout << "process is running: " << processname << std::endl;
    std::string who;
    while (true)
    {
        svr.RecvMessage(&who);
        if (who == processname || who == "all")
        {
            std::cout << processname << " is active!" << std::endl;
        }
        if (who == "end")
        {
            std::cout << processname << " is quit!" << std::endl;
            break;
        }
    }
}

int main()
{
    MmapMemoryServer svr;

    for (int i = 0; i < 10; i++)
    {
        pid_t id = fork();
        if (id == 0)
        {
            std::string name = "process-" + std::to_string(i);
            Active(svr, name);
            exit(0);
        }
    }
    Active(svr, "process-main");

    for (int i = 0; i < 10; i++)
    {
        wait(nullptr);
    }

    return 0;
}
