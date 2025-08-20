//输⼊⼀个字符串，写⼀个函数将⼀个字符串的内容逆序过来。
//#include <stdio.h>
//#include <string.h>
//void reverse(char* str)
//{
//	//利⽤库函数求得字符串⻓度
//		int len = strlen(str);
//	//定义两个字符串指针分别指向字符串⾸位和末位
//	char* left = str;
//	char* right = str + len - 1;
//	//当左指针在右指针左边时，进⾏交换
//	while (left < right)
//	{
//		char tmp = *left;
//		*left = *right;
//		*right = tmp;
//		//两个指针相向移动⼀位
//		left++;
//		right--;
//	}
//}
//int main()
//{
//	char arr[31] = { 0 };
//	//输⼊字符串
//	scanf("%[^\n]s", arr);
//	//对字符串进⾏逆序
//	reverse(arr);
//	printf("%s\n", arr);
//	return 0;
//}