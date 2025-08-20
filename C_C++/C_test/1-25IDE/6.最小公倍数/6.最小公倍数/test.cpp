//⽅法1
#include <stdio.h>
int main()
{
	int m = 0;
	int n = 0;
	printf("请输入两个整数：\n");
	scanf("%d%d", &m, &n);
	//计算m和n的较⼤值
	//m和n的最⼩公倍数，最⼩也是m和n中较⼤的值
	int k = (m > n ? m : n);
	//重复对k进⾏判断
	while (1)
	{
		//若k被两数同时整除,则k为公倍数,并且此时⼀定是最⼩公倍数
		if (k % m == 0 && k % n == 0)
		{
			printf("%d\n", k);
			break;
		}
		//k的值加⼀,对下⼀个数进⾏判断
		k++;
	}
	return 0;
}
//⽅法2（乘积初最大公约数就是最小公倍数）
#include <stdio.h>
int main()
{
	int m = 0;
	int n = 0;
	printf("请输入两个整数：\n");
	//输⼊
	scanf("%d%d", &m, &n);//18 24
	int k = 0;
	int mul = m * n;
	//辗转相除法求得最⼤公约数
	while (k = m % n)
	{
		m = n;
		n = k;
	}
	printf("%d\n", mul / n);
	return 0;
}