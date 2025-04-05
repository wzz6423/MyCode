#include "processbar.h"

// v3
typedef void(*callback_t)(int);

void downLoad(callback_t cb){
    printf("download_begining:\n");
    int total = 1000;
    int curr = 0;
    while(curr <= total){
        usleep(50000);
        int rate = curr * 100 / total;
        cb(rate);
        curr += 10;
    }
    printf("\n");
    initbar();
}

int main(){
    // v3
    downLoad(processbar);
    downLoad(processbar);
    downLoad(processbar);
    
    // v2
    //int total = 1000;
    //int curr = 0;
    //while(curr <= total){
    //    processbar(curr * 100 / total);
    //    curr += 10;
    //    usleep(50000);
    //}
    //printf("\n");

    // v1
    //processbar(100000);

    return 0;
}
