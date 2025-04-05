//需要查找并替换大量函数名时可以使用ctrl + F
#include "Sort.h"

//PropertyTest--Release
void PropertyTest()
{
	srand(time(0));

	const int N = 10000;
	int* a1 = (int*)malloc(sizeof(int) * N);
	int* a2 = (int*)malloc(sizeof(int) * N);
	int* a3 = (int*)malloc(sizeof(int) * N);
	int* a4 = (int*)malloc(sizeof(int) * N);
	int* a5 = (int*)malloc(sizeof(int) * N);
	int* a6 = (int*)malloc(sizeof(int) * N);
	int* a7 = (int*)malloc(sizeof(int) * N);
	int* a8 = (int*)malloc(sizeof(int) * N);

	for (int i = 0; i < N; ++i)
	{
		a1[i] = rand();
		a2[i] = a1[i];
		a3[i] = a1[i];
		a4[i] = a1[i];
		a5[i] = a1[i];
		a6[i] = a1[i];
		a7[i] = a1[i];
		a8[i] = a1[i];
	}

	int begin1 = clock();
	InsertSort(a1, N);
	int end1 = clock();

	int begin2 = clock();
	ShellSort(a2, N);
	int end2 = clock();

	int begin3 = clock();
	SelectSort(a3, N);
	int end3 = clock();

	int begin4 = clock();
	HeapSort(a4, N);
	int end4 = clock();

	int begin5 = clock();
	BubbleSort(a5, N);
	int end5 = clock();

	int begin6 = clock();
	QuickSort(a6, 0, N - 1);
	int end6 = clock();

	int begin7 = clock();
	MergeSort(a7, N);
	int end7 = clock();

	int begin8 = clock();
	CountSort(a8, N);
	int end8 = clock();

	printf("InsertSort:%d\n", end1 - begin1);
	printf("ShellSort:%d\n", end2 - begin2);
	printf("SelectSort:%d\n", end3 - begin3);
	printf("HeapSort:%d\n", end4 - begin4);
	printf("BubbleSort:%d\n", end5 - begin5);
	printf("QuickSort:%d\n", end6 - begin6);
	printf("MergeSort:%d\n", end7 - begin7);
	printf("CountSort:%d\n", end8 - begin8);

	free(a1);
	free(a2);
	free(a3);
	free(a4);
	free(a5);
	free(a6);
	free(a7);
	free(a8);
	a1 = a2 = a3 = a4 = a5 = a6 = a7 = a8 = NULL;
}

//SortTest
void InsertSortTest() {
	int a[] = { 6,8,9,5,0,1,2,3,4,7,10,11,-5,-2,-1 };
	PrintArray(a, sizeof(a) / sizeof(int));

	InsertSort(a, sizeof(a) / sizeof(int));

	PrintArray(a, sizeof(a) / sizeof(int));
}

void ShellSortTest() {
	int a[] = { 6,8,9,5,0,1,2,3,4,7,10,11,-5,-2,-1 };
	PrintArray(a, sizeof(a) / sizeof(int));

	ShellSort(a, sizeof(a) / sizeof(int));

	PrintArray(a, sizeof(a) / sizeof(int));
}

void SelectSortTest() {
	int a[] = { 6,8,9,5,0,1,2,3,4,7,10,11,-5,-2,-1 };
	PrintArray(a, sizeof(a) / sizeof(int));

	SelectSort(a, sizeof(a) / sizeof(int));

	PrintArray(a, sizeof(a) / sizeof(int));
}

void HeapSortTest() {
	int a[] = { 6,8,9,5,0,1,2,3,4,7,10,11,-5,-2,-1 };
	PrintArray(a, sizeof(a) / sizeof(int));

	HeapSort(a, sizeof(a) / sizeof(int));

	PrintArray(a, sizeof(a) / sizeof(int));
}

void BubbleSortTest() {
	int a[] = { 6,8,9,5,0,1,2,3,4,7,10,11,-5,-2,-1 };
	PrintArray(a, sizeof(a) / sizeof(int));

	BubbleSort(a, sizeof(a) / sizeof(int));

	PrintArray(a, sizeof(a) / sizeof(int));
}

void QuickSortTest() {
	int a[] = { 6,8,9,5,0,1,2,3,4,7,10,11,-5,-2,-1 };
	PrintArray(a, sizeof(a) / sizeof(int));

	QuickSort(a, 0, sizeof(a) / sizeof(int) - 1);
	//QuickSortNonR(a, 0, sizeof(a) / sizeof(int) - 1);

	PrintArray(a, sizeof(a) / sizeof(int));
}

void MergeSortTest() {
	int a[] = { 6,8,9,5,0,1,2,3,4,7,10,11,-5,-2,-1 };
	PrintArray(a, sizeof(a) / sizeof(int));

	MergeSort(a, sizeof(a) / sizeof(int));
	//MergeSortNonRFir(a, sizeof(a) / sizeof(int));
	//MergeSortNonRSec(a, sizeof(a) / sizeof(int));
	//MergeSortNonRThi(a, sizeof(a) / sizeof(int));

	PrintArray(a, sizeof(a) / sizeof(int));
}

void CountSortTest() {
	int a[] = { 6,8,9,5,0,1,2,3,4,7,1,1,2,2,3,10,11,-5,-2,-1 };
	PrintArray(a, sizeof(a) / sizeof(int));

	CountSort(a, sizeof(a) / sizeof(int));

	PrintArray(a, sizeof(a) / sizeof(int));
}

void RadixSortTest() {
	int a[] = { 278, 109, 63, 930, 589, 184, 505, 269, 8, 83 };
	PrintArray(a, sizeof(a) / sizeof(int));

	RadixSort(a, sizeof(a) / sizeof(int), 3, 10);

	PrintArray(a, sizeof(a) / sizeof(int));
}

void MergeSortFileTest() {
	MergeSortFile(".//data.txt");
}

//SimpleTest
void SimpleTest() {
	//InsertSortTest();
	//ShellSortTest();
	//SelectSortTest();
	//HeapSortTest();
	//BubbleSortTest();
	//QuickSortTest();
	//MergeSortTest();
	//CountSortTest();
	//RadixSortTest();
	//MergeSortFileTest();
}

int main() {
	PropertyTest();
	//SimpleTest();

	return 0;
}