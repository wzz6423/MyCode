#include "TopK.h"

void AdjustDown(HPDataType* a, int n, int parent) {
	assert(a);
	if (!a) {
		return;
	}

	int child = parent * 2 + 1;
	while (child < n) {
		//选出左右孩子中大的一个
		if (child + 1 < n && a[child + 1] > a[child]) {
			++child;
		}

		if (a[child] > a[parent]) {
			Swap(&a[child], &a[parent]);

			parent = child;
			child = parent * 2 + 1;
		}
		else {
			break;
		}
	}
}

void Swap(HPDataType* p1, HPDataType* p2) {
	assert(p1);
	if (!p1) {
		return;
	}
	assert(p2);
	if (!p2) {
		return;
	}

	HPDataType tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
}