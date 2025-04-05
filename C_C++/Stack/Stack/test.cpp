#include "Stack.h"

void Test(){
	ST st;
	//���Գ�ʼ��
	STInit(&st);

	//������ջ
	STPush(&st, 1);
	STPush(&st, 2);
	STPush(&st, 3);
	STPush(&st, 4);
	STPush(&st, 5);

	//���Դ�ӡջ��Ԫ�ز���ջ
	while (!STEmpty(&st)) {
		printf("%d ", STTpop(&st));
		STPop(&st);
	}

	//��������
	STDestroy(&st);
}

int main() {
	Test();
	return 0;
}