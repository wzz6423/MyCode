#pragma once

#include "Common.h"

const std::string nullfile = "/dev/null";

void Daemon (const std::string& cwd = ""){
    signal(SIGCLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGSTOP, SIG_IGN);

    if(fork() > 0){
        exit(0);
    }
    setsid();

    if(!cwd.empty()){
        chdir(cwd.c_str());
    }

    int fd = open(nullfile.c_str(), O_RDWR);
    if(fd > 0){
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
    }
}