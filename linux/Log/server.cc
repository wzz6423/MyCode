#include "command.hpp"
#include "Log.hpp"

using namespace std;

int main(){
    Init init;

    lg.enable(Onefile);

    int fd = open(FIFO_FILE, O_RDONLY);
    if(fd < 0){
        lg(Fatal, "pipe open fail, error string : %s, errno code : %d", strerror(errno), errno);
        exit(FIFO_OPEN_ERROR);
    }
    lg(Info, "server open pipe done, error string : %s, errno code : %d", strerror(errno), errno);

    while(true){
        char buffer[SIZE];
        int x = read(fd, buffer, sizeof(buffer));
        if(x > 0){
            buffer[x] = 0;
            cout << "client say@ " << buffer << endl;
        }
        else if(x == 0){
            lg(Debug, "client quit, server quit too!...");
            break;
        }
        else {
            break;
        }
    }

    close(fd);
    return 0;
}