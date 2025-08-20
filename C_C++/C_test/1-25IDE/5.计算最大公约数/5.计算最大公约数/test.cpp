//⽅法1:
#include <stdio.h>
int main()
{
	int m = 0;
	int n = 0;
	scanf("%d %d", &m, &n);
	//计算找出m和n的较⼩值k
	//因为最⼤公约数最⼤是m和n的较⼩值
	int k = (m > n ? n : m);
	while (1)
	{
		//每次拿k试除m和n，如果不能同时整除，则k--，继续试除
		if (m % k == 0 && n % k == 0)
		{
			break;
		}
		//k的值减⼀,对下⼀个数进⾏判断
		k--;
	}
	printf("%d\n", k);
	return 0;
}
//⽅法2
#include <stdio.h>
int main()
{
	int m = 0;
	int n = 0;
	scanf("%d%d", &m, &n);//18 24
	//辗转相除法（迭代）
	int k = 0;
	//当n不能整除m，即k≠0，更新两个最值重复步骤计算n与m%n的最⼤公约数
	while (k = m % n)
	{
		m = n;
		n = k;
	}
	printf("%d\n", n);
	return 0;
}
//辗转相除法递归实现 gcd(a,b)的结果为a和b的最⼤公约数
int gcd(int a, int b) {
	//特判除数为0时的情况
	if (b == 0) {
		return a;
	}
	//返回b和a%b的最⼤公约数
	return gcd(b, a % b);
}