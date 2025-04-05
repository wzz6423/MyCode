#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

typedef int HPDataType;

typedef struct Heap
{
	HPDataType* a;
	size_t size;
	size_t capacity;
}HP;

//向下调整：大根堆
void AdjustDown(HPDataType* a, int n, int parent);
//交换
void Swap(HPDataType* p1, HPDataType* p2);
// 对数组进行堆排序
void HeapSort(int* a, size_t len);