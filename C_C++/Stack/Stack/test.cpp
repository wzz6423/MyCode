#include "Stack.h"

void Test(){
	ST st;
	//测试初始化
	STInit(&st);

	//测试入栈
	STPush(&st, 1);
	STPush(&st, 2);
	STPush(&st, 3);
	STPush(&st, 4);
	STPush(&st, 5);

	//测试打印栈顶元素并出栈
	while (!STEmpty(&st)) {
		printf("%d ", STTpop(&st));
		STPop(&st);
	}

	//测试销毁
	STDestroy(&st);
}

int main() {
	Test();
	return 0;
}