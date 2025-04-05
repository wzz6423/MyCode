#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int SLTDataType;

//单链表
typedef struct SListNode {
	SLTDataType data;
	struct SListNode* next;
}SLTNode;

//创建新节点
SLTNode* BuySLTNode(SLTDataType x);
//打印
void SLTPrint(SLTNode* phead);
//销毁-一级指针
void SLTDestroy_first(SLTNode* phead);
//销毁-二级指针
void SLTDestroy_second(SLTNode** phead);
//尾插
void SLTPushBack(SLTNode** pphead, SLTDataType x);
//头插
void SLTPushFront(SLTNode** pphead, SLTDataType x);
//尾删
void SLTPopBack(SLTNode** pphead);
//头删
void SLTPopFront(SLTNode** pphead);
//查找
SLTNode* SLTFind(SLTNode* phead, SLTDataType x);
//中间插（前）
void SLTInsert(SLTNode** pphead, SLTNode* pos, SLTDataType x);
//中间删（前）
void SLTErase(SLTNode** pphead, SLTNode* pos);
//中间插（后）
void SLTInsertAfter(SLTNode* pos, SLTDataType x);
//中间删（后）
void SLTEraseAfter(SLTNode* pos);