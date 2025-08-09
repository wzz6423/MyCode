/*
    守护进程
*/

#pragma once

// C
#include <cstdlib>
// C++
#include <iostream>
#include <unistd.h>
#include <string>
// linux system call
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>

namespace stellar_post
{
    namespace Daemon
    {
        class Daemon
        {
        public:
            Daemon(const std::string &workDir = "")
            {
                signal(SIGCLD, SIG_IGN);
                signal(SIGPIPE, SIG_IGN);
                signal(SIGSTOP, SIG_IGN);

                if (fork() > 0)
                {
                    exit(0);
                }
                setsid();

                if (!workDir.empty())
                {
                    chdir(workDir.c_str());
                }

                int fd = open(nullfile.c_str(), O_RDWR);
                if (fd > 0)
                {
                    dup2(fd, 0);
                    dup2(fd, 1);
                    dup2(fd, 2);
                    close(fd);
                }
            }

        private:
            const std::string nullfile = "/dev/null";
        };
    }
}