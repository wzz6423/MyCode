#include "Log.hpp"
#include "command.hpp"

int main(){
    Init init;
    int shmid = CreateShm();
    char* shmaddr = (char*)shmat(shmid, nullptr, 0);

    int fd = open(FIFO_FILE, O_RDONLY);
    if(fd < 0){
        lg(Fatal, "error code : %d, error stding : %s", errno, strerror(errno));
        exit(FIFO_OPEN_ERROR);
    }
    struct shmid_ds shmds;
    while(true){
        char c;
        ssize_t s = read(fd, &c, 1);
        if(s == 0){
            break;
        }
        else if(s < 0){
            break;
        }

        std::cout << "client say@ " << shmaddr << std::endl;
        sleep(1);

        shmctl(shmid, IPC_STAT, &shmds);
        std::cout << "shm size " << shmds.shm_segsz << std::endl;
        std::cout << "shm nattch " << shmds.shm_nattch << std::endl;
        printf("shm key : 0x%x\n", shmds.shm_perm.__key);
        std::cout << "shm mode " << shmds.shm_perm.mode << std::endl;
    }

    shmdt(shmaddr);
    shmctl(shmid, IPC_RMID, nullptr);

    close(fd);

    return 0;
}