#include "HeapSort.h"

int main() {
	HPDataType a[] = { 1,5,8,9,2,3,6,4,7,0 };
	size_t len = sizeof(a) / sizeof(a[0]);
	HeapSort(a, len);

	for (int i = 0; i < len; ++i) {
		printf("%d ", a[i]);
	}

	return 0;
}