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

//���µ����������
void AdjustDown(HPDataType* a, int n, int parent);
//����
void Swap(HPDataType* p1, HPDataType* p2);
// ��������ж�����
void HeapSort(int* a, size_t len);