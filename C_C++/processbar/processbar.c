#include "processbar.h"

// v2
const char* lable = "|/-\\";
char bar[NUM] = {'\0'};

void processbar(int rate){
    if(rate < 0 || rate > 100) return;

    size_t len = strlen(lable);

    printf("[%-100s][%d%%][%c]\r", bar, rate, lable[rate % len]);
    fflush(stdout);
    bar[rate++] = BODY;
    if(rate < TOP){
        bar[rate] = RIGHT;
    }
}

void initbar(){
    memset(bar, '\0', sizeof(bar));
}

// v1
//void processbar(int speed){
//    char bar[NUM];
//    memset(bar, '\0', sizeof(bar));
//    size_t len = strlen(lable);
//
//    int count = 0;
//    while(count <= TOP){
//        printf("[%-100s][%d%%][%c]\r", bar, count, lable[count % len]);
//        fflush(stdout);
//        bar[count++] = BODY;
//        if(count < TOP){
//            bar[count] = RIGHT;
//        }
//        usleep(speed);
//    }
//    printf("\n");
//}
