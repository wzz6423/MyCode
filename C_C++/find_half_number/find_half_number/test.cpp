//���ֲ���
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
//int main() {
//	int arr[] = { 1,2,3,4,5,6,7,8,9,10 };
//
//	printf("������һ��1-10�����֣������ӡ�������±꣩��\n");
//	int num = 0;
//	scanf("%d", &num);
//
//	int sz = sizeof(arr) / sizeof(arr[0]);
//	int left = 0;
//	int right = sz - 1;
//
//	int flag = 0;
//	while (left <= right){
//		int mid = left + (right - left) / 2;
//		if (arr[mid] < num){
//			left = mid + 1;
//		}
//		else if (arr[mid] > num){
//			right = mid - 1;
//		}
//		else{
//			printf("�ҵ��ˣ��±���:%d\n", mid);
//			flag = 1;
//			break;
//		}
//	}
//	if (flag == 0) {
//		printf("û�ҵ�\n");
//	}
//
//	return 0;
//}

//int main() {
//	int arr[] = { 1,2,3,4,5,6,7,8,9,10 };
//
//	printf("������һ��1-10�����֣������ӡ�������±꣩��\n");
//	int num = 0;
//	scanf("%d", &num);
//
//	int sz = sizeof(arr) / sizeof(arr[0]);
//	int left = 0;
//	int right = sz - 1;
//
//	int flag = 0;
//	while (left <= right){
//		int mid = left + ((right - left) >> 1);
//		//����һλ�൱�ڳ���2���ȳ���2����ȫ�����������
//		//ʮ���ƣ�11�����ƣ�1011
//		//����һλ�����ƣ�101��ʮ���ƣ�5
//		//ʮ���ƣ�13�����ƣ�1101
//		//����һλ�����ƣ�110��ʮ���ƣ�6
//		//ͬʱ�����˸��������������ת����Ч�ʸ���
//		if (arr[mid] < num){
//			left = mid + 1;
//		}
//		else if (arr[mid] > num){
//			right = mid - 1;
//		}
//		else{
//			printf("�ҵ��ˣ��±���:%d\n", mid);
//			flag = 1;
//			break;
//		}
//	}
//	if (flag == 0) {
//		printf("û�ҵ�\n");
//	}
//
//	return 0;
//}