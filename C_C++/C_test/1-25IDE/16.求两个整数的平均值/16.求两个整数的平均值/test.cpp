////输⼊2个整数m和n，写⼀个函数average，求2个整数的平均值，考虑整数过⼤溢出的问题
//
////两数本⾝和两数的平均数不会溢出，考虑平均数与其中⼀个数的差：(x + y) / 2 - x = (y - x) / 2；
////要求得(x + y) / 2，我们可以先求得 x 与(y - x) / 2，然后求得他们的和即可；
////x 与(y - x) / 2 在计算过程中都不会溢出，因此这个⽅法成⽴
//
//#define _CRT_SECURE_NO_WARNINGS 1
//
//#include <stdio.h>
//
////int average1(int first, int second) {
////	return (first + second) / 2;
////}
//
//int average2(int first, int second) {
//	return x + (y - x) / 2;
//}
//
//int main() {
//	int first = 0;
//	int second = 0;
//	printf("请输入两个整数 ：\n");
//	scanf("%d%d", &first, &second);
//	//int mean = average1(first, second);
//	int mean = average2(first, second);
//	printf("平均值为：%d", mean);
//	return 0;
//}