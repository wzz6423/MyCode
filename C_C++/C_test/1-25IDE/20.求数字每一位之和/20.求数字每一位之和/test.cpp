////输⼊⼀个整数m，求这个整数m的每⼀位之和，并打印
//#define _CRT_SECURE_NO_WARNINGS 1
//
//#include <stdio.h>
//int digit_sum(int m)
//{
//	//定义变量记录每⼀位的和
//	int s = 0;
//	//当前数还不为0，获取其个位数加到s中
//	while (m)
//	{
//		s += m % 10;
//		//删除其个位数
//		m /= 10;
//	}
//	//返回每⼀位的和
//	return s;
//}
//int main()
//{
//	int m = 0;
//	//输⼊整数
//	scanf("%d", &m);
//	//获取每⼀位的和
//	int ret = digit_sum(m);
//	printf("%d\n", ret);
//	return 0;
//}