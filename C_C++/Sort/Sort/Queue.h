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

//��ʼ��
void QueueInit(Queue* pq);
//����
void QueueDestroy(Queue* pq);
//��ЧԪ�ظ���
int QueueSize(Queue* pq);
//���
bool QueueEmpty(Queue* pq);
//�����
void QueuePush(Queue* pq, QDatatype x);
//������
void QueuePop(Queue* pq);
//��Ԫ��
QDatatype QueueFront(Queue* pq);
//βԪ��
QDatatype QueueBack(Queue* pq);