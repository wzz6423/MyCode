#include "MyStdio.h"
#include <errno.h>

#define myfile "test.txt"

int main(){
  _FILE* fp = _fopen(myfile, "w");
  if(fp == NULL){
    return 1;
  }

  const char* msg = "Hello World\n";
  
  int cnt = 10;
  while(cnt){
    _fwrite(fp, msg, strlen(msg));
    cnt--;
    //sleep(1);
  }

  _fclose(fp);
  fp = NULL;

  return 0;
}
