//输⼊10个整数，写代码找出其中最⼤值和最⼩值，计算最⼤值和最⼩值的差，并打印出差值结果

//思路：可以使用数组+冒泡排序/数组+封装函数+循环选出最大值和最小值

//#define _CRT_SECURE_NO_WARNINGS 1

//⽅法1
//#include <stdio.h>
//#include <limits.h>
//int main(){
//	int arr[10] = { 0 };
//	//输⼊数据
//	int i = 0;
//	for (i = 0; i < 10; i++)
//	{
//		scanf("%d", &arr[i]);
//	}
//	//将两个最值初始化为第⼀个数
//	int Max = arr[0];
//	int Min = arr[0];
//	//遍历剩余9个数
//	for (i = 1; i < 10; i++)
//	{
//		//判断当前数⼤⼩并更新最值
//		if (arr[i] > Max)
//			Max = arr[i];
//		if (arr[i] < Min)
//			Min = arr[i];
//	}
//	//输出两最值的差
//	printf("%d\n", Max - Min);
//	return 0;
//}

//⽅法2
//#include <stdio.h>
//#include <limits.h>
//int main(){
//		int arr;
//	//输⼊数据
//	scanf("%d", &arr);
//	//将两个最值初始化为第⼀个数
//	int Max = arr;
//	int Min = arr;
//	//遍历剩余9个数
//	int i = 0;
//	for (i = 1; i < 10; i++)
//	{
//		//输⼊数据
//		scanf("%d", &arr);
//		//判断当前数⼤⼩并更新最值
//		if (arr > Max)
//			Max = arr;
//		if (arr < Min)
//			Min = arr;
//	}
//	//输出两最值的差
//	printf("%d\n", Max - Min);
//	return 0;
//}

//方法3
//#include <stdio.h>
//
//int main() {
//	int arr[10] = { 0 };
//	size_t sz = sizeof(arr) / sizeof(arr[0]);
//	printf("请输入10个数字：\n");
//
//	for (int i = 0; i < sz; i++) {
//		scanf("%d", &arr[i]);
//	}
//
//	for (int i = 0; i < sz - 1; i++) {
//		for (int j = 0; j < sz - i - 1; j++) {
//			if (arr[j] > arr[j + 1]) {
//				int temp = arr[j + 1];
//				arr[j + 1] = arr[j];
//				arr[j] = temp;
//			}
//		}
//	}
//
//	printf("%d", arr[sz - 1] - arr[0]);
//
//	return 0;
//}