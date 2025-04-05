#pragma once

#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>

//打印
void PrintArray(int* arr, int num);

//内排序 -- 比较排序
//插入排序
//直接插入排序
void InsertSort(int* arr, int num);
//希尔排序
void ShellSort(int* arr, int num);

//选择排序
//交换
void Swap(int* num1, int* num2);
//直接选择排序
void SelectSort(int* arr, int num);
//堆排序
void Adjustdown(int* arr, int num, int parent);
void HeapSort(int* arr, int num);

//交换排序
//冒泡排序
void BubbleSort(int* arr, int num);
//快速排序(两路划分)
//快速排序递归
void QuickSort(int* arr, int left, int right);
//快速排序非递归(栈 -- 类似二叉树前序遍历)(处理栈溢出)
void QuickSortNonR(int* arr, int left, int right);
//三数取中(处理有序数据)
void GetMidNumi(int*arr, int* midnumi, int left, int right);
//随机选key(处理有序数据)
void Randomkey(int* rankeyi, int left, int right);
//Horae
void QucikSortHorae(int* arr, int left, int right);
//挖坑法
void QucikSortHole(int* arr, int left, int right);
//双指针法 P -- pointer
void QucikSortTwoP(int* arr, int left, int right);
//三路划分(处理大量重复数据 -- 存在性能损失) R -- route
void QucikSortThrR(int* arr, int left, int right);

//归并排序(递归)
void _MergeSort(int* arr, int begin, int end, int* tmp);
void MergeSort(int* arr, int num);
//归并排序(直接改循环 -- 类似二叉树后序遍历 -> 不好用栈)(非递归)
void MergeSortNonRFir(int* arr, int num);
void MergeSortNonRSec(int* arr, int num);
void MergeSortNonRThi(int* arr, int num);


//外排序 -- 比较排序
//归并排序
void _MergeFile(const char* file1, const char* file2, const char* mfile);
void MergeSortFile(const char* file);

//非比较排序	
//计数排序
void CountSort(int* arr, int num);
//基数排序 -- MSD：最高次序优先(升序)、LSD：最低次序优先(降序) -- 队列
void RadixSort(int* arr, int num, int degree, int radix);//degree:最高位数 --> 排几次，radix:基数(0-9) --> 用几个数排序