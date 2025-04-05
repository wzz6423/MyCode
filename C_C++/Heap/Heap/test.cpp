#include "Heap.h"

void Test() {
	HP hp;
	//���Գ�ʼ��
	HeapInit(&hp);

	//�������
	HeapPush(&hp, 1);
	HeapPush(&hp, 2);
	HeapPush(&hp, 3);
	HeapPush(&hp, 4);
	HeapPush(&hp, 5);
	HeapPush(&hp, 6);
	HeapPush(&hp, 7);
	HeapPush(&hp, 5);
	HeapPush(&hp,8);

	//���Բ�Ѷ�Ԫ�ز�ɾ��
	size_t len = hp.size;
	while (len--) {
		printf("%d ", HeapTop(&hp));
		HeapPop(&hp);
	}
	printf("\n");

	//��������
	HeapDestroy(&hp);
}

int main() {
	Test();

	return 0;
}