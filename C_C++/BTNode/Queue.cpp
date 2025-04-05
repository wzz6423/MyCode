#include "Queue.h"

//初始化
void QueueInit(Queue* pq) {
	assert(pq);
	if (!pq) {
		return;
	}

	pq->head = pq->tail = NULL;
	pq->size = 0;
}

//销毁
void QueueDestroy(Queue* pq) {
	assert(pq);
	if (!pq) {
		return;
	}

	QNode* del = pq->head;
	QNode* tmp = NULL;
	while (del) {
		tmp = del->next;
		free(del);
		del = tmp;
	}
	tmp = del = NULL;
	pq->head = pq->tail = NULL;
	pq->size = 0;
}

//有效元素个数
int QueueSize(Queue* pq) {
	assert(pq);

	return pq->size;
}

//查空
bool QueueEmpty(Queue* pq) {
	assert(pq);

	return (pq->size == 0);
}

//入队列
void QueuePush(Queue* pq, QDatatype x) {
	assert(pq);
	if (!pq) {
		return;
	}

	QNode* newnode = (QNode*)malloc(sizeof(QNode));
	assert(newnode);
	if (!newnode) {
		return;
	}
	newnode->next = NULL;
	newnode->data = x;

	if (!pq->head) {
		pq->head = pq->tail = newnode;
	}
	else {
		pq->tail->next = newnode;
		pq->tail = pq->tail->next;
	}

	newnode = NULL;
	++pq->size;
}

//出队列
void QueuePop(Queue* pq) {
	assert(pq);
	if (!pq) {
		return;
	}
	assert(pq->head);
	if (!pq->head) {
		return;
	}

	if (!pq->head->next) {
		free(pq->head);
		pq->head = pq->tail = NULL;
	}
	else {
		QNode* del = pq->head;
		pq->head = pq->head->next;
		free(del);
		del = NULL;
	}
	--pq->size;
}

//首元素
QDatatype QueueFront(Queue* pq) {
	assert(pq);

	assert(!QueueEmpty(pq));

	return pq->head->data;
}

//尾元素
QDatatype QueueBack(Queue* pq) {
	assert(pq);

	assert(!QueueEmpty(pq));
	return pq->tail->data;
}