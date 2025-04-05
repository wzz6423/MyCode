#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void Usage(std::string proc){
    std::cout << "\n\rUsage: " << proc << " serverip serverport\n" << std::endl;
}

int main(int argc, char* argv[]){
    if(argc != 3){
        Usage(argv[0]);
        exit(1);
    }
    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    inet_pton(AF_INET, serverip.c_str(), &(server.sin_addr));

    while(true){
        int cnt = 5;
        int isreconnect = false;
        int sockfd = 0;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0){
            std::cerr << "sock error" << std::endl;
            return 1;
        }

        do{
            int n = connect(sockfd, (struct sockaddr*)&server, sizeof(server));
            if(n < 0){
                isreconnect = true;
                --cnt;
                std::cerr << "connect error, reconnect: " << cnt << std::endl;
                sleep(2);
            }
            else{
                break;
            }
        }while(cnt && isreconnect);

        if(cnt == 0){
            std::cerr << "user offline..." << std::endl;
            break;
        }

        std::string message;
        std::cout << "Please Enter@ ";
        std::getline(std::cin, message);

        int n = write(sockfd, message.c_str(), message.size());
        if(n < 0){
            std::cerr << "write error..." << std::endl;
        }

        char inbuffer[4096];
        n = read(sockfd, inbuffer, sizeof(inbuffer) - 1);
        if(n > 0){
            inbuffer[n] = 0;
            std::cout << inbuffer << std::endl;
        }

        close(sockfd);
    }

    return 0;
}