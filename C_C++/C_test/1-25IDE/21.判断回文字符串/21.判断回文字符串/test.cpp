////输⼊⼀个字符串，判断这个字符串是否是回⽂字符串（字符串的⻓度⼩于等于30，字符串不包含空
////格），如果是回⽂字符串输出Yes，如果不是回⽂字符串输出No。
//#define _CRT_SECURE_NO_WARNINGS 1
//
//#include <stdio.h>
//#include <string.h>
//
//int is_palindrome_string(char arr[])
//{
//	//获取字符串⻓度
//	size_t n = strlen(arr);
//	//定义两个指针分别指向字符串的⾸位和末位
//	char* left = arr;
//	char* right = arr + n - 1;
//	//当两个指针相对位置未发⽣变化时对其进⾏判断
//	while (left < right)
//	{
//		//两字符不相等，返回0，表⽰字符串不是回⽂串
//		if (*left != *right)
//			return 0;
//		//两指针相向移动⼀位
//		left++;
//		right--;
//	}
//	//返回1，表⽰字符串是回⽂串
//	return 1;
//}
//int main()
//{
//	char arr[31];
//	//输⼊
//	scanf("%[^\n]s", arr);
//	//判断字符串是否为回⽂串
//	int ret = is_palindrome_string(arr);
//	if (ret == 1)
//		printf("Yes\n");
//	else
//		printf("No\n");
//	return 0;
//}