#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

std::string terminal = "./testfile.txt";

int OpenTerminal(){
    int fd = open(terminal.c_str(), O_WRONLY);
    if(fd < 0){
        cerr << "open terminal error" << endl;
        return 1;
    }
    dup2(fd, 2);

    return 0;
}