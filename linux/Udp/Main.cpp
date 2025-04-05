#include "UdpServer.hpp"
#include <memory>
#include <vector>
#include <cstdio>

void Usage(std::string proc){
    std::cout << "\n\rUsage: " << proc << " port[1024+]\n" << std::endl;
}

std::string Handler(const std::string& str){
    std:: string res = "Server get a message: ";
    res += str;
    std::cout << res << std::endl;

    return res;
}

bool SafeCheck(const std::string& cmd){
    std::vector<std::string> key_words = {
        "rm",
        "mv",
        "cp",
        "kill",
        "sudo",
        "unlink",
        "remove",
        "apt",
        "top",
        "while",
        "uninstall"
    };
    for(const auto& word: key_words){
        auto pos = cmd.find(word);
        if(pos != std::string::npos){
            return false;
        }
    }
    return true;
}

std::string ExcuteCommand(const std::string& cmd){
    if(!SafeCheck(cmd)){
        return "Bad Command";
    }

    FILE* fp = popen(cmd.c_str(), "r");
    if(nullptr == fp){
        perror("popen");
        return "error";
    }
    std::string result;
    char buffer[4096];
    while(true){
        char* ok = fgets(buffer, sizeof(buffer), fp);
        if(ok == nullptr){
            break;
        }
        result += buffer;
    }
    pclose(fp);

    return result;
}

int main(int argc, char *argv[]){
    if(argc != 2){
        Usage(argv[0]);
        exit(0);
    }

    uint16_t port = std::stoi(argv[1]);

    std::unique_ptr<UdpServer> svr(new UdpServer(port));

    svr->Init();
    svr->Run(/* ExcuteCommand */);

    return 0;
}