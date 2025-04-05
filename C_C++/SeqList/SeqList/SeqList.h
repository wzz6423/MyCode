#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int SLDataType;
#define ORIGIN_CAPACITY 4

//顺序表
typedef struct SeqList {
	SLDataType* pa;
	size_t size;
	size_t capacity;
}SL;

//初始化
void SLInit(SL* ps);
//销毁
void SLDestroy(SL* ps);
//打印
void SLPrint(SL* ps);
//扩容
void SLCheckCapacity(SL* ps);
//尾插
void SLPushBack(SL* ps, SLDataType x);
//尾删
void SLPopBack(SL* ps);
//头插
void SLPushFront(SL* ps, SLDataType x);
//头删
void SLPopFront(SL* ps);
//中间插入
void SLInsert(SL* ps, int pos, SLDataType x);
//中间删除
void SLErase(SL* ps, int pos);
//查找
int SLFind(SL* ps, SLDataType x);