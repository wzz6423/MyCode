//⽔已知1瓶汽⽔1元，2个空瓶可以换⼀瓶汽⽔，输⼊整数n（n >= 0），表⽰n元钱，计算可以多少汽⽔，请编程实现。
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>

//方法0
int main() {
	int num = 0;
	int count = 0;
	printf("请输入您的钱数：\n");
	scanf("%d", &num);
	count = num;

	while (num) {
		int i = num % 2;
		num /= 2;
		count += num;
		if (1 == num && 1 == i) {
			count++;
			break;
		}
	}	
	printf("可以买%d瓶汽水\n", count);

	return 0;
}

//⽅法1：
//int main()
//{
//	int n = 0;
//	int total = 0;//表⽰总共能喝多少汽⽔
//	int empty = 0;//表⽰⼿⾥的空瓶数
//	scanf("%d", &n);
//	//买n瓶汽⽔，并且喝完后剩余n个空瓶
//	total += n;
//	empty += n;
//	//重复⽤空瓶⼦购买汽⽔直⾄空瓶不够2个
//	while (empty >= 2)
//	{
//		total += empty / 2;
//		empty = empty / 2 + empty % 2;
//	}
//	printf("%d\n", total);
//	return 0;
//}

//⽅法2
//int main()
//{
//	int n = 0;
//	int total = 0;//表⽰总共能喝多少汽⽔
//	int empty = 0;//表⽰⼿⾥的空瓶数
//	scanf("%d", &n);
//	//如果我们没钱买汽⽔，则可以喝到0瓶汽⽔，否则喝到2*n-1瓶汽⽔
//	if(n == 0)
//		total = 0;
//    else
//        total = 2 * n - 1;
//    printf("%d\n", total);
//    return 0;
//}