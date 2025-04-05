#include "Log.hpp"
#include "command.hpp"

int main(){
    int shmid = GetShm();
    char* shmaddr = (char*)shmat(shmid, nullptr, 0);

    int fd = open(FIFO_FILE, O_WRONLY);
    if(fd < 0){
        lg(Fatal, "error code : %d, error stding : %s", errno, strerror(errno));
        exit(FIFO_OPEN_ERROR);
    }

    while(true){
        std::cout << "please enter@ ";
        fgets(shmaddr, 4096, stdin);

        write(fd, "w", 1);
    }

    shmdt(shmaddr);

    close(fd);

    return 0;
}