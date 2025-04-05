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

//��ʼ��
void HeapInit(HP* php);
//����
void HeapInitArray(HP* php, HPDataType* a, int n);
//����
void HeapDestroy(HP* php);
//���
void HeapPush(HP* php, HPDataType x);
//ɾ���Ѷ�����
void HeapPop(HP* php);
//���ضѶ�����
HPDataType HeapTop(HP* php);
//�п�
bool HeapEmpty(HP* php);
//���С
int HeapSize(HP* php);
//���ϵ����������
void AdjustUp(HPDataType* a, int child);
//���µ����������
void AdjustDown(HPDataType* a, int n, int parent);
//����
void Swap(HPDataType* p1, HPDataType* p2);