// 等价于#pragma once
#ifndef __MYSTDIO_H__
#define __MYSTDIO_H__

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define SIZE 1024
#define FLUSH_NOW 1
#define FLUSH_LINE 2
#define FLUSH_ALL 4

typedef struct IO_FILE{
  int fileno;
  int flag;
//  char inbuffer[SIZE];
//  int in_pos;
  char outbuffer[SIZE];
  int out_pos;
}_FILE;

_FILE* _fopen(const char* filename, const char* flag);
int _fwrite(_FILE* fp, const char* msg, int len);
void _fclose(_FILE* fp);

#endif
