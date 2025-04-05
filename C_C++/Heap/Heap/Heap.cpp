#include "Heap.h"

//��ʼ��
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

//����
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
	//���i = 0; i < n; ++i�Ļ�ʱ�临�Ӷ������ϵ���û���𣬶���O(N * logN)
	//���������д����ʱ�临�Ӷȵõ��Ż�ΪO(logN)
}

//����
void HeapDestroy(HP* php) {
	assert(php);
	if (!php) {
		return;
	}

	free(php->a);
	php->a = NULL;
	php->capacity = php->size = 0;
}

//���
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

//ɾ���Ѷ�����
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

//���ضѶ�����
HPDataType HeapTop(HP* php) {
	assert(php);
	assert(!HeapEmpty(php));

	return php->a[0];
}

//�п�
bool HeapEmpty(HP* php) {
	assert(php);

	return (php->size == 0);
}

//���С
int HeapSize(HP* php) {
	assert(php);

	return php->size;
}

//���ϵ����������
//���ϵ��������������ߣ�������ѣ���С�����·ţ��Ѵ�����Ϸ�,ʱ�临�Ӷȣ�O*(N * logN)
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

//���µ����������
//���µ��������������ߣ�������ѣ���С�����·ţ��Ѵ�����Ϸţ�ʱ�临�Ӷȣ�O*(N)
void AdjustDown(HPDataType* a, int n, int parent) {
	assert(a);
	if (!a) {
		return;
	}

	int child = parent * 2 + 1;
	while (child < n) {
		//ѡ�����Һ����д��һ��
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
//֤��
//A.����n��Ԫ�ص�ƽ�������������Ϊ�Sn���������������Ϊh��
//B.���²��Ҷ�ڵ��Ԫ�أ�ֻ����һ��������������ȷ�����������һ�����2 ^ (h - 1)��Ԫ�أ����Ǽٶ�O(1) = 1����ô��һ��Ԫ������ʱ��Ϊ2 ^ (h - 1) �� 1��
//C.������bottom - top�����ѣ�����ڵ����ϲ�Ԫ�ص�ʱ�򣬲�����Ҫͬ�²�����Ԫ�����Ƚϣ�ֻ��Ҫͬ����֮һ��֧���Ƚϣ������Ƚϴ����������ĸ߶ȼ�ȥ��ǰ�ڵ�ĸ߶ȡ���ˣ���x��Ԫ�صļ�����Ϊ2 ^ (x)��(h - x)��
//D.������ͨ�ʽ�ɵ�֪����������Ϊh�Ķ���ѵľ�ȷʱ�临�Ӷ�Ϊ��
//S = 2 ^ (h - 1) �� 1 + 2 ^ (h - 2) �� 2 + ���� + 1 ��(h - 1) ��
//E.ͨ���۲���Ĳ��ó��Ĺ�ʽ��֪������͹�ʽΪ�Ȳ����к͵ȱ����еĳ˻�������ô�λ�������⣬����ʽ��������ͬʱ����2����֪��
//2S = 2 ^ h �� 1 + 2 ^ (h - 1) �� 2 + ���� + 2 ��(h - 1) ��
//�âڼ�ȥ�ٿ�֪�� S = 2 ^ h �� 1 - h + 1 ��
//��h = �Sn ����ۣ��ó����½��ۣ�
//S = n - �Sn + 1 = O(n)

//����
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