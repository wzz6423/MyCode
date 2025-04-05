#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Terminal.hpp"

using namespace std;

struct ThreadData{
    struct sockaddr_in server;
    int sockfd;
    std::string serverip;
};

void Usage(std::string proc){
    cout << "\n\rUsage: " << proc << " serverip serverport\n" << endl;
}

void* recv_message(void* args){
    // OpenTerminal();
    ThreadData* td = static_cast<ThreadData*>(args);
    char buffer[1024];
    while(true){
        memset(buffer, 0, sizeof(buffer));
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);

        ssize_t s = recvfrom(td->sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)(&temp), &len);
        if(s > 0){
            buffer[s] = 0;
            cerr << buffer << endl; // cout
        }
    }
}

void* send_message(void* args){
    ThreadData* td = static_cast<ThreadData*>(args);
    string message;
    socklen_t len = sizeof(td->server);
    
    while(true){
        cout << "Please Enter@ ";
        getline(cin, message);

        sendto(td->sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&(td->server), len);
    }
}

int main(int argc, char *argv[]){
    if(argc != 3){
        Usage(argv[0]);
        exit(0);
    }
    string serverip = argv[1];
    uint16_t serverport = stoi(argv[2]);

    struct ThreadData td;
    bzero(&td.server, sizeof(td.server));
    td.server.sin_family = AF_INET;
    td.server.sin_port = htons(serverport);
    td.server.sin_addr.s_addr = inet_addr(serverip.c_str());

    td.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(td.sockfd < 0){
        cout << "socket error" << endl;
        return 1;
    }

    td.serverip = serverip;

    pthread_t recvr;
    pthread_t sender;
    pthread_create(&recvr, nullptr, recv_message, &td);
    pthread_create(&sender, nullptr, send_message, &td);

    pthread_join(recvr, nullptr);
    pthread_join(sender, nullptr);

    close(td.sockfd);
    return 0;
}