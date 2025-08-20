//写⼀个函数Strlen，可以求字符串⻓度，注：字符串的结束标志是 \0 .
//输⼊⼀个字符串（字符串⻓度⼩于等于30，中间不包含空格），使⽤Strlen函数可以求字符串的⻓度，并返回
#include <stdio.h>
//#include <assert.h>
//指针-指针的⽅式
//size_t Strlen(char *s)
//{
//    assert(str);
//    char *p = s;
//    while(*p != '\0' )
//        p++;
//    return p-s;
//}
//计数器
//size_t Strlen(const void* p){
//    assert(p);
//    char* str = (char*)p;
//    int len = 0;
//    while(*str){
//        len++;
//        str++;
//    }
//    return len;
//}
//int main(){
//    char arr[30] = {0};
//    printf("请输入一个字符串：\n");
//    scanf("%[^\n]s", arr);
//    size_t len = Strlen(arr);
//    printf("长度为：%zd\n", len);
//    return 0;
//}
//函数递归
//size_t Strlen(const char* str)
//{
////递归结束条件
//    if (*str == '\0')
//        return 0;
//    else
////返回1加后⾯⼦字符串的和
//        return 1 + Strlen(str + 1);
//}
//int main()
//{
//    char arr[31] = { 0 };
////输⼊字符串
//    scanf("%[^\n]s", arr);
////获取字符串⻓度
//    size_t len = Strlen(arr);
//    printf("%zd\n", len);
//    return 0;
//}