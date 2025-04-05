// cpp include
#include <iostream>
#include <string>
// c of cpp include
#include <cstdio>
#include <cstring>
#include <cstdlib>
// system call include
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM 2 // pipe_fd use
#define LEN 1024 // IO message use

// error enum
enum{
    PIPE_CREATE_ERR = 1,
    FORK_ERR,
    WAIT_ERR
};

// child -> write
void Writer(int wfd){
    // creat massage
    std::string test = "Hello, I am child";
    pid_t self = getpid();
    int number = 0;

    // send massage
    char buffer[LEN];
    while(true){
        sleep(1);
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%s-%d-%d", test.c_str(), self, number++);
        write(wfd, buffer, strlen(buffer));
    }
}

// parent -> read
void Reader(int rfd){
    char buffer[LEN];
    int cnt = 0; // for test
    while(true){
        memset(buffer, 0, sizeof(buffer));
        ssize_t num = read(rfd, buffer, sizeof(buffer));
        if(num > 0){
            buffer[num] = 0;
            std::cout << "Father get a massage[" << getpid() << "]# " << buffer << std::endl;
        }
        else if(num == 0){
            std::cout << "Father read file done!" << std::endl;
            break;
        }
        else{
            std::cerr << "Read error" << std::endl;
            break;
        }

        cnt++;
        if(cnt > 5){
            break;
        }
    }
}

int main(){
    int pipefd[NUM] = {0};
    int n = pipe(pipefd);
    if(n < 0){
        return PIPE_CREATE_ERR;
    }

    pid_t id = fork();
    if(id < 0){
        return FORK_ERR;
    }

    if(id == 0){
        // child
        close(pipefd[0]);

        Writer(pipefd[1]);

        exit(0);
    }
    // father
    close(pipefd[1]);

    Reader(pipefd[0]);
    close(pipefd[0]);

    // for test
    std::cout << "Father close read fd :" << pipefd[0] << std::endl;
    sleep(5);

    int status = 0;
    pid_t rid = waitpid(id, &status, 0);
    if(rid < 0){
        return WAIT_ERR;
    }

    std::cout << "Wait child success , id : " << rid << std::endl;
    sleep(5);

    std::cout << "Father Quit..." << std::endl;

    return 0;
}