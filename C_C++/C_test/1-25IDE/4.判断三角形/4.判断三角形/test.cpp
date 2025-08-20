#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>

int main()
{
	int a = 0;
	int b = 0;
	int c = 0;
	printf("请输入三角形的三边：\n");
	scanf("%d%d%d", &a, &b, &c);
	//三⻆形必须满⾜任意两天边⼤于第三条边

	if (a + b > c && a + c > b && b + c > a)
	{
		//三条边相当
		if (a == b && b == c)
		{
			printf("等边三角形\n");
		}
		//任意两条边相等，但是不等于第三条边，此时只需要判断是否有两条边相等
		else if (a == b || a == c || b == c)
		{
			printf("等腰三角形\n");
		}
		//两种情况都不满⾜输出普通三⻆形
		else
		{
			printf("三角形\n");
		}
	}
	//不能构成三⻆形
	else
	{
		printf("不是三角形\n");
	}
	return 0;
}