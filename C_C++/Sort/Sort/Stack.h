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

//��ʼ��
void STInit(ST* ps);
//����
void STDestroy(ST* ps);
//�п�
bool STEmpty(ST* ps);
//����Ч��������
int STSize(ST* ps);
//��ջ
void STPush(ST* ps, STDataType x);
//����ջ��Ԫ��
STDataType STTpop(ST* ps);
//��ջ
void STPop(ST* ps);