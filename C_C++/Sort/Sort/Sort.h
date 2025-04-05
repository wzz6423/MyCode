#pragma once

#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>

//��ӡ
void PrintArray(int* arr, int num);

//������ -- �Ƚ�����
//��������
//ֱ�Ӳ�������
void InsertSort(int* arr, int num);
//ϣ������
void ShellSort(int* arr, int num);

//ѡ������
//����
void Swap(int* num1, int* num2);
//ֱ��ѡ������
void SelectSort(int* arr, int num);
//������
void Adjustdown(int* arr, int num, int parent);
void HeapSort(int* arr, int num);

//��������
//ð������
void BubbleSort(int* arr, int num);
//��������(��·����)
//��������ݹ�
void QuickSort(int* arr, int left, int right);
//��������ǵݹ�(ջ -- ���ƶ�����ǰ�����)(����ջ���)
void QuickSortNonR(int* arr, int left, int right);
//����ȡ��(������������)
void GetMidNumi(int*arr, int* midnumi, int left, int right);
//���ѡkey(������������)
void Randomkey(int* rankeyi, int left, int right);
//Horae
void QucikSortHorae(int* arr, int left, int right);
//�ڿӷ�
void QucikSortHole(int* arr, int left, int right);
//˫ָ�뷨 P -- pointer
void QucikSortTwoP(int* arr, int left, int right);
//��·����(��������ظ����� -- ����������ʧ) R -- route
void QucikSortThrR(int* arr, int left, int right);

//�鲢����(�ݹ�)
void _MergeSort(int* arr, int begin, int end, int* tmp);
void MergeSort(int* arr, int num);
//�鲢����(ֱ�Ӹ�ѭ�� -- ���ƶ������������ -> ������ջ)(�ǵݹ�)
void MergeSortNonRFir(int* arr, int num);
void MergeSortNonRSec(int* arr, int num);
void MergeSortNonRThi(int* arr, int num);


//������ -- �Ƚ�����
//�鲢����
void _MergeFile(const char* file1, const char* file2, const char* mfile);
void MergeSortFile(const char* file);

//�ǱȽ�����	
//��������
void CountSort(int* arr, int num);
//�������� -- MSD����ߴ�������(����)��LSD����ʹ�������(����) -- ����
void RadixSort(int* arr, int num, int degree, int radix);//degree:���λ�� --> �ż��Σ�radix:����(0-9) --> �ü���������