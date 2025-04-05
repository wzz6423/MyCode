#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int SLDataType;
#define ORIGIN_CAPACITY 4

//˳���
typedef struct SeqList {
	SLDataType* pa;
	size_t size;
	size_t capacity;
}SL;

//��ʼ��
void SLInit(SL* ps);
//����
void SLDestroy(SL* ps);
//��ӡ
void SLPrint(SL* ps);
//����
void SLCheckCapacity(SL* ps);
//β��
void SLPushBack(SL* ps, SLDataType x);
//βɾ
void SLPopBack(SL* ps);
//ͷ��
void SLPushFront(SL* ps, SLDataType x);
//ͷɾ
void SLPopFront(SL* ps);
//�м����
void SLInsert(SL* ps, int pos, SLDataType x);
//�м�ɾ��
void SLErase(SL* ps, int pos);
//����
int SLFind(SL* ps, SLDataType x);