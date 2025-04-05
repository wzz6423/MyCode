#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef int QDatatype;

typedef struct QueueNode{
	struct QueueNode* next;
	QDatatype data;
}QNode;

typedef struct Queue{
	QNode* head;
	QNode* tail;
	int size;
}Queue;

//初始化
void QueueInit(Queue* pq);
//销毁
void QueueDestroy(Queue* pq);
//有效元素个数
int QueueSize(Queue* pq);
//查空
bool QueueEmpty(Queue* pq);
//入队列
void QueuePush(Queue* pq, QDatatype x);
//出队列
void QueuePop(Queue* pq);
//首元素
QDatatype QueueFront(Queue* pq);
//尾元素
QDatatype QueueBack(Queue* pq);