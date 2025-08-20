//输⼊10个整数，然后使⽤冒泡排序对数组内容进⾏升序排序，然后打印数组的内容

#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>

int main() {
	int arr[10] = { 0 };
	size_t sz = sizeof(arr) / sizeof(arr[0]);

	printf("请输入10个数字：\n");

	for (int i = 0; i < sz; i++) {
		scanf("%d", &arr[i]);
	}

	for (int i = 0; i < sz - 1; i++) {
		int flag = 0;
		for (int j = 0; j < sz - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				int temp = arr[j + 1];
				arr[j + 1] = arr[j];
				arr[j] = temp;
				flag = 1;
			}
		}
		if (!flag) {
			break;
		}
	}

	for (int i = 0; i < sz; i++) {
		printf("%d ", arr[i]);
	}

	return 0;
}