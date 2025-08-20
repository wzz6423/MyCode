//输入一个字符串将小写字母变为大写字母，大写字母变为小写字母，其它字符不变（长度小于等于30）

#define _CRT_SECURE_NO_WARNINGS 1

//⽅法1：不使⽤库函数
//#include <stdio.h>
//int main() {
//	char buf[31] = { 0 };
//	//输⼊字符串
//	scanf("%[^\n]s", buf);
//	int i = 0;
//	//遍历字符串
//	while (buf[i]) {
//		//判断当前字符是否为⼩写字⺟
//		if (buf[i] >= 'a' && buf[i] <= 'z')
//			buf[i] -= 32;
//		//判读那当前字符是否为⼤写字⺟
//		else if (buf[i] >= 'A' && buf[i] <= 'Z')
//			buf[i] += 32;
//		i++;
//	}
//	printf("%s\n", buf);
//	return 0;
//}

//⽅法2：使⽤库函数
//#include <stdio.h>
//#include <ctype.h>
//int main() {
//	char buf[31] = { 0 };
//	//输⼊字符串
//	scanf("%[^\n]s", buf);
//	int i = 0;
//	while (buf[i])
//	{
//		//判断当前字符是否为⼩写字⺟
//		if (islower(buf[i]))
//			buf[i] = toupper(buf[i]);
//		//判读当前字符是否为⼤写字⺟
//		else if (isupper(buf[i]))
//			buf[i] = tolower(buf[i]);
//		i++;
//	}
//	printf("%s\n", buf);
//	return 0;
//}