//写一个函数Swap可以交换两个整数的内容
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>

//方法一：传⼊两个整型指针参数，进⾏交换
//void Swap(int* pa, int* pb)
//{
//	int tmp = *pa;
//	*pa = *pb;
//	*pb = tmp;
//}
//int main()
//{
//	int a = 0;
//	int b = 0;
//	//输⼊
//	scanf("%d %d", &a, &b);
//	//将需要交换值的两个整数变量的地址作为参数传递给函数，进⾏交换
//	Swap(&a, &b);
//	//输出
//	printf("%d %d\n", a, b);
//	return 0;
//}

//方法二：利用位操作符
//void exchange(int* pnum1, int* pnum2) {
//	*pnum1 ^= *pnum2;
//	*pnum2 ^= *pnum1;
//	*pnum1 ^= *pnum2;
//}
//
//int main() {
//	int num1 = 0, num2 = 0;
//	printf("请输入两个整数：\n");
//	scanf("%d%d", &num1, &num2);
//	int* pnum1 = &num1;
//	int* pnum2 = &num2;
//
//	exchange(pnum1, pnum2);
//	printf("%d %d", num1, num2);
//
//	return 0;
//}