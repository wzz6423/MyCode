#include "Stack.h"

//初始化
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

//销毁
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

//判空
bool STEmpty(ST* ps) {
	assert(ps);

	return (ps->top == -1);
}

//查有效数据数量
int STSize(ST* ps) {
	assert(ps);

	return ps->top;
}

//入栈
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

//返回栈顶元素
STDataType STTpop(ST* ps) {
	assert(ps);
	assert(!STEmpty(ps));
	if (STEmpty(ps)) {
		exit(-1);
	}

	return ps->p[ps->top];
}

//出栈
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