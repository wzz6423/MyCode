//二分查找
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
//int main() {
//	int arr[] = { 1,2,3,4,5,6,7,8,9,10 };
//
//	printf("请输入一个1-10的数字（将会打印其数组下标）：\n");
//	int num = 0;
//	scanf("%d", &num);
//
//	int sz = sizeof(arr) / sizeof(arr[0]);
//	int left = 0;
//	int right = sz - 1;
//
//	int flag = 0;
//	while (left <= right){
//		int mid = left + (right - left) / 2;
//		if (arr[mid] < num){
//			left = mid + 1;
//		}
//		else if (arr[mid] > num){
//			right = mid - 1;
//		}
//		else{
//			printf("找到了，下标是:%d\n", mid);
//			flag = 1;
//			break;
//		}
//	}
//	if (flag == 0) {
//		printf("没找到\n");
//	}
//
//	return 0;
//}

//int main() {
//	int arr[] = { 1,2,3,4,5,6,7,8,9,10 };
//
//	printf("请输入一个1-10的数字（将会打印其数组下标）：\n");
//	int num = 0;
//	scanf("%d", &num);
//
//	int sz = sizeof(arr) / sizeof(arr[0]);
//	int left = 0;
//	int right = sz - 1;
//
//	int flag = 0;
//	while (left <= right){
//		int mid = left + ((right - left) >> 1);
//		//右移一位相当于除以2，比除以2更安全（整数溢出）
//		//十进制：11二进制：1011
//		//右移一位二进制：101，十进制：5
//		//十进制：13二进制：1101
//		//右移一位二进制：110，十进制：6
//		//同时避免了浮点数与整数间的转换，效率更高
//		if (arr[mid] < num){
//			left = mid + 1;
//		}
//		else if (arr[mid] > num){
//			right = mid - 1;
//		}
//		else{
//			printf("找到了，下标是:%d\n", mid);
//			flag = 1;
//			break;
//		}
//	}
//	if (flag == 0) {
//		printf("没找到\n");
//	}
//
//	return 0;
//}