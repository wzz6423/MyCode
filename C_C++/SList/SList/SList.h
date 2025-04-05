#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int SLTDataType;

//������
typedef struct SListNode {
	SLTDataType data;
	struct SListNode* next;
}SLTNode;

//�����½ڵ�
SLTNode* BuySLTNode(SLTDataType x);
//��ӡ
void SLTPrint(SLTNode* phead);
//����-һ��ָ��
void SLTDestroy_first(SLTNode* phead);
//����-����ָ��
void SLTDestroy_second(SLTNode** phead);
//β��
void SLTPushBack(SLTNode** pphead, SLTDataType x);
//ͷ��
void SLTPushFront(SLTNode** pphead, SLTDataType x);
//βɾ
void SLTPopBack(SLTNode** pphead);
//ͷɾ
void SLTPopFront(SLTNode** pphead);
//����
SLTNode* SLTFind(SLTNode* phead, SLTDataType x);
//�м�壨ǰ��
void SLTInsert(SLTNode** pphead, SLTNode* pos, SLTDataType x);
//�м�ɾ��ǰ��
void SLTErase(SLTNode** pphead, SLTNode* pos);
//�м�壨��
void SLTInsertAfter(SLTNode* pos, SLTDataType x);
//�м�ɾ����
void SLTEraseAfter(SLTNode* pos);