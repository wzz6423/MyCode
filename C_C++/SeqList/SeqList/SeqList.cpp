#include "SeqList.h"

//��ʼ��
void SLInit(SL* ps) {
	assert(ps);
	if (!ps) {
		return;
	}
	ps->pa = (SLDataType*)malloc(sizeof(SL) * ORIGIN_CAPACITY);
	if (!ps->pa) {
		perror("malloc");
		return;
	}
	ps->size = 0;
	ps->capacity = ORIGIN_CAPACITY;
}

//����
void SLDestroy(SL* ps) {
	assert(ps);
	if (!ps) {
		return;
	}
	free(ps->pa);
	ps->pa = NULL;
	ps->capacity = ps->size =  0;
}

//��ӡ
void SLPrint(SL* ps) {
	assert(ps);
	if (!ps) {
		return;
	}
	for (int i = 0; i < ps->size; i++) {
		printf("%d ", ps->pa[i]);
	}
	printf("\n");
}

//����
void SLCheckCapacity(SL* ps) {
	assert(ps);
	if (!ps) {
		return;
	}
	if (ps->size == ps->capacity) {
		SLDataType* tmp = (SLDataType*)realloc(ps->pa, sizeof(SLDataType) * ps->capacity * 2);
		if (!tmp) {
			 perror("realloc");
			 return;
		}
		ps->pa = tmp;
		ps->capacity *= 2;
	}
}

//β��
void SLPushBack(SL* ps, SLDataType x) {
	assert(ps);
	if (!ps) {
		return;
	}
	//1.
	SLCheckCapacity(ps);
	ps->pa[ps->size++] = x;

	//2.
	//SLInsert(ps, ps->size, x);
}

//βɾ
void SLPopBack(SL* ps) {
	assert(ps);
	if (!ps) {
		return;
	}
	//1.
	assert(ps->size);
	ps->size--;

	//2.
	//SLErase(ps, ps->size - 1);
}

//ͷ��
void SLPushFront(SL* ps, SLDataType x) {
	assert(ps);
	if (!ps) {
		return;
	}
	//1.
	SLCheckCapacity(ps);
	size_t sz = ps->size;
	while(sz){
		ps->pa[sz] = ps->pa[sz - 1];
		sz--;
	}
	ps->pa[sz] = x;
	ps->size++;

	//2.
	//SLInsert(ps, 0, x);
}

//ͷɾ
void SLPopFront(SL* ps) {
	assert(ps);
	if (!ps) {
		return;
	}
	//1.
	assert(ps->size);
	size_t sz = ps->size;
	while (sz) {
		ps->pa[ps->size - sz] = ps->pa[ps->size - sz + 1];
		sz--;
	}
	ps->size--;

	//2.
	//SLErase(ps, 0);
}

//�м����
void SLInsert(SL* ps, int pos, SLDataType x) {
	assert(ps);
	if (!ps) {
		return;
	}
	assert(pos >= 0 && pos < ps->size);
	SLCheckCapacity(ps);
	size_t end = ps->size;
	while (end >= pos) {
		ps->pa[end] = ps->pa[end - 1];
		end--;
	}
	ps->pa[pos - 1] = x;
	ps->size++;
}

//�м�ɾ��
void SLErase(SL* ps, int pos) {
	assert(ps);
	if (!ps) {
		return;
	}
	assert(pos >= 0 && pos < ps->size);
	assert(ps->size);
	size_t begin = pos;
	while (ps->size - begin) {
		ps->pa[begin - 1] = ps->pa[begin];
		begin++;
	}
	ps->size--;
}

//����
int SLFind(SL* ps, SLDataType x) {
	assert(ps);

	for (int i = 0; i < ps->size; i++) {
		if (ps->pa[i] == x) {
			return i;
		}
	}
	return -1;
}