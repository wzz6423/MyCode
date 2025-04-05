#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef int STDataType;

typedef struct Stack {
	STDataType* p;
	int top;
	int capacity;
}ST;

//初始化
void STInit(ST* ps);
//销毁
void STDestroy(ST* ps);
//判空
bool STEmpty(ST* ps);
//查有效数据数量
int STSize(ST* ps);
//入栈
void STPush(ST* ps, STDataType x);
//返回栈顶元素
STDataType STTpop(ST* ps);
//出栈
void STPop(ST* ps);