#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef int HPDataType;

typedef struct Heap
{
	HPDataType* a;
	size_t size;
	size_t capacity;
}HP;

//初始化
void HeapInit(HP* php);
//建堆
void HeapInitArray(HP* php, HPDataType* a, int n);
//销毁
void HeapDestroy(HP* php);
//添加
void HeapPush(HP* php, HPDataType x);
//删除堆顶数据
void HeapPop(HP* php);
//返回堆顶数据
HPDataType HeapTop(HP* php);
//判空
bool HeapEmpty(HP* php);
//查大小
int HeapSize(HP* php);
//向上调整：大根堆
void AdjustUp(HPDataType* a, int child);
//向下调整：大根堆
void AdjustDown(HPDataType* a, int n, int parent);
//交换
void Swap(HPDataType* p1, HPDataType* p2);