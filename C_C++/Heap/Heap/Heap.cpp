#include "Heap.h"

//初始化
void HeapInit(HP* php) {
	assert(php);
	if (!php) {
		return;
	}

	HPDataType* newnode = (HPDataType*)malloc(sizeof(HPDataType) * 4);
	assert(newnode);
	if (!newnode) {
		perror("malloc");
		return;
	}
	php->a = newnode;
	php->size = 0;
	php->capacity = 4;
}

//建堆
void HeapInitArray(HP* php, HPDataType* a, int n) {
	assert(php);
	if (!php) {
		return;
	}
	assert(a);
	if (!a) {
		return;
	}

	php->a = a;
	php->size = n;
	php->capacity = n;

	for (int i = (n - 2) / 2; i >= 0; --i) {//(n - 1 - 1) / 2
		AdjustDown(php->a, n, i);
	}
	//如果i = 0; i < n; ++i的话时间复杂度与向上调整没区别，都是O(N * logN)
	//按照上面的写法，时间复杂度得到优化为O(logN)
}

//销毁
void HeapDestroy(HP* php) {
	assert(php);
	if (!php) {
		return;
	}

	free(php->a);
	php->a = NULL;
	php->capacity = php->size = 0;
}

//添加
void HeapPush(HP* php, HPDataType x) {
	assert(php);
	if (!php) {
		return;
	}

	if (php->size == php->capacity) {
		HPDataType* newnode = (HPDataType*)realloc(php->a, sizeof(HPDataType) * php->capacity * 2);
		assert(newnode);
		if (!newnode) {
			perror("realloc");
			return;
		}

		php->capacity *= 2;
		php->a = newnode;
	}

	php->a[php->size++] = x;
	AdjustUp(php->a, php->size - 1);
}

//删除堆顶数据
void HeapPop(HP* php) {
	assert(php);
	if (!php) {
		return;
	}
	assert(!HeapEmpty(php));

	Swap(&php->a[0], &php->a[php->size - 1]);
	--php->size;

	AdjustDown(php->a, php->size, 0);
}

//返回堆顶数据
HPDataType HeapTop(HP* php) {
	assert(php);
	assert(!HeapEmpty(php));

	return php->a[0];
}

//判空
bool HeapEmpty(HP* php) {
	assert(php);

	return (php->size == 0);
}

//查大小
int HeapSize(HP* php) {
	assert(php);

	return php->size;
}

//向上调整：大根堆
//向上调整：从上往下走，（大根堆）把小的往下放，把大的往上放,时间复杂度：O*(N * logN)
void AdjustUp(HPDataType* a, int child) {
	assert(a);
	if (!a) {
		return;
	}

	int parent = (child - 1) / 2;
	while (child > 0) {
		if (a[child] > a[parent]) {
			Swap(&a[child], &a[parent]);

			child = parent;
			parent = (child - 1) / 2;
		}
		else {
			break;
		}
	}
}

//向下调整：大根堆
//向下调整：从下往上走，（大根堆）把小的往下放，把大的往上放，时间复杂度：O*(N)
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
//证明
//A.具有n个元素的平衡二叉树，树高为㏒n，我们设这个变量为h。
//B.最下层非叶节点的元素，只需做一次线性运算便可以确定大根，而这一层具有2 ^ (h - 1)个元素，我们假定O(1) = 1，那么这一层元素所需时间为2 ^ (h - 1) × 1。
//C.由于是bottom - top建立堆，因此在调整上层元素的时候，并不需要同下层所有元素做比较，只需要同其中之一分支作比较，而作比较次数则是树的高度减去当前节点的高度。因此，第x层元素的计算量为2 ^ (x)×(h - x)。
//D.又以上通项公式可得知，构造树高为h的二叉堆的精确时间复杂度为：
//S = 2 ^ (h - 1) × 1 + 2 ^ (h - 2) × 2 + …… + 1 ×(h - 1) ①
//E.通过观察第四步得出的公式可知，该求和公式为等差数列和等比数列的乘积，因此用错位相减法求解，给公式左右两侧同时乘以2，可知：
//2S = 2 ^ h × 1 + 2 ^ (h - 1) × 2 + …… + 2 ×(h - 1) ②
//用②减去①可知： S = 2 ^ h × 1 - h + 1 ③
//将h = ㏒n 带入③，得出如下结论：
//S = n - ㏒n + 1 = O(n)

//交换
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