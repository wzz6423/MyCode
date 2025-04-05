#include "MyStdio.h"

#define FILE_MODE 0666

_FILE* _fopen(const char* filename, const char* flag){
  assert(filename && flag);
  
  int f = 0;
  int fd = -1;
  if(strcmp(flag, "w") == 0){
    f = (O_CREAT|O_WRONLY|O_TRUNC);
    fd = open(filename, f, FILE_MODE);
  }
  else if(strcmp(flag, "a") == 0){
    f = (O_CREAT|O_WRONLY|O_APPEND);
    fd = open(filename, f, FILE_MODE);
  }
  else if(strcmp(flag, "r") == 0){
    f = O_RDONLY;
    fd = open(filename, f);
  }
  else{
    return NULL;
  }

  if(fd == -1){
    return NULL;
  }

  _FILE* fp = (_FILE*)malloc(sizeof(_FILE));
  if(fp == NULL){
    return NULL;
  }

  fp->fileno = fd;
  fp->flag = FLUSH_LINE;
  //fp->flag = FLUSH_ALL;
  fp->out_pos = 0;

  return fp;
}

int _fwrite(_FILE* fp, const char* msg, int len){
  assert(fp && msg);

  memcpy(&fp->outbuffer[fp->out_pos], msg, len); // 没做异常处理（动态）or局部问题（只成功写入部分）
  fp->out_pos += len;

  if(fp->flag & FLUSH_NOW){
    write(fp->fileno, fp->outbuffer, fp->out_pos);
    fp->out_pos = 0;
  }
  else if(fp->flag & FLUSH_LINE){
    if(fp->outbuffer[fp->out_pos - 1] == '\n'){ // 中间出现\n（遍历查找，切割，拷贝）
      write(fp->fileno, fp->outbuffer, fp->out_pos);
      fp->out_pos = 0;
    }
  }
  else if(fp->flag & FLUSH_ALL){
    if(fp->out_pos == SIZE){
      write(fp->fileno, fp->outbuffer, fp->out_pos);
      fp->out_pos = 0;
    }
  }

  return len;
}

void _fflush(_FILE* fp){
  if(fp->out_pos > 0){
    write(fp->fileno, fp->outbuffer, fp->out_pos);
    fp->out_pos = 0;
  }
}

void _fclose(_FILE* fp){
  if(fp == NULL){
    return;
  }

  _fflush(fp);
  close(fp->fileno);
  free(fp);
}







