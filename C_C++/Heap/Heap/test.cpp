#include "Heap.h"

void Test() {
	HP hp;
	//测试初始化
	HeapInit(&hp);

	//测试添加
	HeapPush(&hp, 1);
	HeapPush(&hp, 2);
	HeapPush(&hp, 3);
	HeapPush(&hp, 4);
	HeapPush(&hp, 5);
	HeapPush(&hp, 6);
	HeapPush(&hp, 7);
	HeapPush(&hp, 5);
	HeapPush(&hp,8);

	//测试查堆顶元素并删除
	size_t len = hp.size;
	while (len--) {
		printf("%d ", HeapTop(&hp));
		HeapPop(&hp);
	}
	printf("\n");

	//测试销毁
	HeapDestroy(&hp);
}

int main() {
	Test();

	return 0;
}