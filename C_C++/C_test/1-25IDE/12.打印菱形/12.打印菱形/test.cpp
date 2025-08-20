//输⼊⼀个整数n，打印对应2* n - 1⾏的菱形图案，⽐如，输⼊7，输出如下图案，图案总共13⾏
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>

//第一种
//int main() {
//	int num = 0;
//	printf("请输入一个整数作为菱形的长、宽\n");
//	scanf("%d", &num);
//
//	for (int i = 0; i <= num / 2; i++) {
//		int j = 0;
//		for (j = 0; j < num / 2 - i; j++) {
//			printf(" ");
//		}
//
//		for (int k = j + 1; k <= num - j; k++) {
//			printf("*");
//		}
//		printf("\n");
//	}
//
//	for (int i = num - 1; i > num / 2; i--) {
//		int j = 0;
//		for (j = 0; j < num - i; j++) {
//			printf(" ");
//		}
//		for (int k = j + 1; k <= num - j; k++) {
//			printf("*");
//		}
//		printf("\n");
//	}
//
//	return 0;
//}

//第二种
//int main()
//{
//	int n = 0;
//	//输⼊
//	scanf("%d", &n);
//	//打印
//	//打印上半部分的n⾏
//	int i = 0;
//	for (i = 0; i < n; i++)
//	{
//		//打印空格
//		int j = 0;
//		for (j = 0; j < n - 1 - i; j++)
//		{
//			printf(" ");
//		}
//		//打印*
//			for (j = 0; j < 2 * i + 1; j++)
//			{
//				printf("*");
//			}
//		printf("\n");
//	}
//	//打印下半部分的n-1⾏
//	for (i = 0; i < n; i++)
//	{
//		//打印空格
//		int j = 0;
//		for (j = 0; j <= i; j++)
//		{
//			printf(" ");
//		}
//		//打印*
//		for (j = 0; j < 2 * (n - 1 - i) - 1; j++)
//		{
//			printf("*");
//		}
//		printf("\n");
//	}
//	return 0;
//}