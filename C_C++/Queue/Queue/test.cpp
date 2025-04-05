#include "Queue.h"

void Test() {
	Queue q;
	QueueInit(&q);
	QueuePush(&q, 1);
	QueuePush(&q, 2);
	QueuePush(&q, 3);
	QueuePush(&q, 4);

	QNode* cur = q.head;
	while (cur) {
		printf("%d ", cur->data);
		cur = cur->next;
	}
	printf("->1\n");

	//QueuePop(&q);
	//QueuePop(&q);
	//QueuePop(&q);
	//QueuePop(&q);
	int count = q.size;
	while (count--) {
		QueuePop(&q);
	}
	printf("->2\n");

	cur = q.head;
	while (cur) {
		printf("%d ", cur->data);
		cur = cur->next;
	}
	printf("->3\n");

	QueueDestroy(&q);
}

int main() {
	Test();

	return 0;
}