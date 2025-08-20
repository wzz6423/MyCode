//写⼀个代码打印1~100000之间的所有的⾃幂数，中间⽤空格分隔
//⾃幂数是指⼀个数的位数的n次⽅等于这个数本⾝。例如，153是⾃幂数，因为 1 ^ 3 + 5 ^ 3 + 3 ^ 3 = 153。
// 
//法一
//#include <stdio.h>
//
//int main() {
//	for (int i = 1; i <= 100000; i++) {
//		int count = 0;
//		int num = 0;
//		int temp = 0;
//		int flag = i;
//
//		for (; ;) {
//			temp = i % 10;
//			if (!i) {
//				break;
//			}
//			count++;
//			i /= 10;
//		}
//		i = flag;
//
//		for (int j = 0; j < count; j++) {
//			int m = 1;
//			temp = i % 10;
//			for (int k = 0; k < count; k++) {
//				m *= temp;
//			}
//			num += m;
//			i /= 10;
//		}
//		i = flag;
//
//		if (i == num) {
//			printf("%d ", i);
//		}
//
//	}
//
//	return 0;
//}

//法二（while循环此处由于for循环，且求和过程使用math.h的函数比用循环更方便）
//#include <stdio.h>
//#include <math.h>
//int main()
//{
//	int i = 0;
//	for (i = 1; i <= 100000; i++)
//	{
//
//		//判断i是否是⾃幂数
//		//1. 计算i的位数n
//		int n = 1;
//		int tmp = i;
//		while (tmp / 10)
//		{
//			n++;
//			tmp /= 10;
//		}
//
//		//2. 计算i的每⼀位的n次⽅之和
//		tmp = i;
//		int sum = 0;
//		while (tmp)
//		{
//			sum += (int)pow(tmp % 10, n);//math.h的函数，作用是计算tmp的n次幂
//			tmp /= 10;
//		}
//
//		//3. 输出
//		if (sum == i)
//			printf("%d ", i);
//	}
//
//	return 0;
//}