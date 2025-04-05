#include "Stack.h"

//��ʼ��
void STInit(ST* ps) {
	assert(ps);
	if (!ps) {
		return;
	}

	ps->p = (STDataType*)malloc(sizeof(int) * 4);
	assert(ps->p);
	if (!ps->p) {
		perror("malloc");
		return;
	}
	ps->top = -1;
	ps->capacity = 4;
}

//����
void STDestroy(ST* ps) {
	assert(ps);
	if (!ps) {
		return;
	}

	free(ps->p);
	ps->p = NULL;
	ps->top = -1;
	ps->capacity = 0;
}

//�п�
bool STEmpty(ST* ps) {
	assert(ps);

	return (ps->top == -1);
}

//����Ч��������
int STSize(ST* ps) {
	assert(ps);

	return ps->top;
}

//��ջ
void STPush(ST* ps, STDataType x) {
	assert(ps);
	if (!ps) {
		return;
	}

	if (ps->top + 1 == ps->capacity) {
		STDataType* newpointer = (STDataType*)realloc(ps->p, sizeof(STDataType) * ps->capacity * 2);
		assert(newpointer);
		if (!newpointer) {
			perror("realloc");
			return;
		}
		ps->p = newpointer;
		ps->capacity *= 2;
	}
	ps->p[++(ps->top)] = x;
}

//����ջ��Ԫ��
STDataType STTpop(ST* ps) {
	assert(ps);
	assert(!STEmpty(ps));
	if (STEmpty(ps)) {
		exit(-1);
	}

	return ps->p[ps->top];
}

//��ջ
void STPop(ST* ps) {
	assert(ps);
	if (!ps) {
		return;
	}

	assert(!STEmpty(ps));
	if (STEmpty(ps)) {
		return;
	}
	ps->top--;
}