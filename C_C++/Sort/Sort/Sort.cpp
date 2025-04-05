#include "Sort.h"
#include "Stack.h"
#include "Queue.h"

//打印
void PrintArray(int* arr, int num) {
	assert(arr);
	if (!arr) {
		return;
	}

	for (int i = 0; i < num; ++i) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}

//内排序 -- 比较排序
//插入排序
//直接插入排序
void InsertSort(int* arr, int num) {
	assert(arr);
	if (!arr) {
		return;
	}

	int tmp = 0;

	//1.
	//for (int i = 0; i < num; ++i) {
	//	tmp = arr[i];

	//	for (int j = i - 1; j >= 0; --j) {
	//		if (arr[j] > tmp) {
	//			arr[j + 1] = arr[j];
	//			arr[j] = tmp;
	//		}
	//	}
	//}

	//2.
	for (int i = 1; i < num; ++i) {
		int end = i - 1;
		tmp = arr[i];

		while (end >= 0) {
			if (arr[end] > tmp) {
				arr[end + 1] = arr[end];
				--end;
			}
			else {
				break;
			}
		}

		arr[end + 1] = tmp;
	}
}
//希尔排序
void ShellSort(int* arr, int num) {
	assert(arr);
	if (!arr) {
		return;
	}

	int tmp = 0;

	//1.
	int gap = num;
	while (gap > 1) {
		//gap /= 2;
		gap = gap / 3 + 1;

		for (int i = gap; i < num; ++i) { //直接从gap开始，前面的以及被排过了
			tmp = arr[i];
			int j = i;
			while (j - gap >= 0 && arr[j - gap] > tmp) {
				arr[j] = arr[j - gap];
				j -= gap;
			}
			arr[j] = tmp;
		}
	}

	//2.
	//int gap = num ;
	//while (gap > 1) {
	//	//gap /= 2;
	//	gap = gap / 3 + 1;

	//	for (int i = 0; i < num - gap; ++i) {
	//		int end = i; 
	//		tmp = arr[i + gap];

	//		while (end >= 0) {
	//			if (arr[end] > tmp) {
	//				arr[end + gap] = arr[end];
	//				end -= gap;
	//			}
	//			else {
	//				break;
	//			}
	//		}

	//		arr[end + gap] = tmp;
	//	}
	//}
}

//选择排序
//交换
void Swap(int* num1, int* num2) {
	assert(num1);
	if (!num1) {
		return;
	}
	assert(num2);
	if (!num2) {
		return;
	}

	int tmp = *num1;
	*num1 = *num2;
	*num2 = tmp;
}
//直接选择排序
void SelectSort(int* arr, int num) {
	assert(arr);
	if (!arr) {
		return;
	}

	int left = 0;
	int right = num - 1;
	while (left < right) {
		int mini = left;
		int maxi = right;
		for (int i = left; i < right; ++i) {
			if (arr[i] > arr[maxi]) {
				maxi = i;
			}

			if (arr[i] < arr[mini]) {
				mini = i;
			}
		}

		Swap(&arr[left], &arr[mini]);
		if (left == maxi) {
			maxi = mini;
		}
		Swap(&arr[right], &arr[maxi]);
		++left;
		--right;
	}
}
//堆排序 -- 排升序建大堆
void Adjustdown(int* arr, int num, int parent) {
	assert(arr);
	if (!arr) {
		return;
	}

	int child = parent * 2 + 1;
	while (child < num) {
		if (child + 1 < num && arr[child + 1] > arr[child]) {
			++child;
		}
		if (arr[child] > arr[parent]) {
			Swap(&arr[child], &arr[parent]);

			parent = child;
			child = parent * 2 + 1;
		}
		else {
			break;
		}
	}
}
void HeapSort(int* arr, int num) {
	assert(arr);
	if (!arr) {
		return;
	}

	for (int i = (num - 2) / 2; i >= 0; --i) {
		Adjustdown(arr, num, i);
	}

	int end = num - 1;
	while (end > 0) {
		Swap(&arr[end], &arr[0]);
		Adjustdown(arr, end, 0);
		--end;
	}
}

//交换排序
//冒泡排序
void BubbleSort(int* arr, int num) {
	assert(arr);
	if (!arr) {
		return;
	}

	bool flag = false;
	for (int i = 0; i < num - 1; ++i) {
		for (int j = 0; j < num - i - 1; ++j) {
			if (arr[j] > arr[j + 1]) {
				Swap(&arr[j], &arr[j + 1]);
				flag = true;
			}
		}
		if (!flag) {
			break;
		}
	}
}
//快速排序(两路划分)
//快速排序递归
void QuickSort(int* arr, int left, int right) {
	assert(arr);
	if (!arr) {
		return;
	}

	//Horae
	//QucikSortHorae(arr, left, right);
	//Hole
	//QucikSortHole(arr, left, right);
	//ThrP
	QucikSortTwoP(arr, left, right);
	//ThrR
	//QucikSortThrR(arr, left, right);
}
//快速排序非递归(栈 -- 类似二叉树前序遍历)(处理栈溢出)
void QuickSortNonR(int* arr, int left, int right) {
	assert(arr);
	if (!arr) {
		return;
	}

	if ((right - left) <= 13) {
		InsertSort(arr + left, right - left + 1);
	}
	else {
		ST st;
		STInit(&st);
		STPush(&st, right);
		STPush(&st, left);

		while (!STEmpty(&st)) {
			int begin = STTpop(&st);
			STPop(&st);
			int end = STTpop(&st);
			STPop(&st);

			int keyi = 0;
			//Randomkey(&keyi, left, right);
			GetMidNumi(arr, &keyi, begin, end);
			if (keyi != begin) {
				Swap(&arr[begin], &arr[keyi]);
				keyi = begin;
			}

			int prev = begin;
			int cur = begin + 1;

			while (cur <= end) {
				if (arr[cur] < arr[keyi] && ++prev != cur) {
					Swap(&arr[cur], &arr[prev]);
				}

				++cur;
			}
			Swap(&arr[keyi], &arr[prev]);

			if (prev + 1 < end) {
				STPush(&st, end);
				STPush(&st, prev + 1);
			}
			if (begin < prev - 1) {
				STPush(&st, prev - 1);
				STPush(&st, begin);
			}
		}
		STDestroy(&st);
	}
}
//三数取中(处理有序数据)
void GetMidNumi(int* arr, int* midnumi, int left, int right) {
	assert(midnumi);
	if (!midnumi) {
		return;
	}

	*midnumi = (right - left) / 2;
	if (arr[left] > arr[*midnumi]) {
		if (arr[right] > arr[*midnumi]) {
			*midnumi = right;
		}
		else {
			*midnumi = left;
		}
	}
	else {
		if (arr[left] > arr[right]) {
			*midnumi = left;
		}
		else {
			*midnumi = right;
		}
	}
}
//随机选key(处理有序数据)
void Randomkey(int* rankeyi, int left, int right) {
	assert(rankeyi);
	if (!rankeyi) {
		return;
	}

	*rankeyi = left + rand() % (right - left);
}
//Horae
void QucikSortHorae(int* arr, int left, int right) {
	assert(arr);
	if (!arr) {
		return;
	}

	if (left >= right) {
		return;
	}

	if ((right - left) <= 13) {
		InsertSort(arr + left, right - left + 1);
	}
	else {
		int keyi = 0;
		//Randomkey(&keyi, left, right);
		GetMidNumi(arr, &keyi, left, right);
		if (keyi != left) {
			Swap(&arr[left], &arr[keyi]);
			keyi = left;
		}

		int begin = left;
		int end = right;

		while (begin < end) {
			while (begin < end && arr[end] >= arr[keyi]) {
				--end;
			}

			while (begin < end && arr[begin] <= arr[keyi]) {
				++begin;
			}

			Swap(&arr[begin], &arr[end]);
		}

		Swap(&arr[keyi], &arr[begin]);

		QucikSortHorae(arr, left, begin - 1);
		QucikSortHorae(arr, end + 1, right);
	}
}
//挖坑法
void QucikSortHole(int* arr, int left, int right) {
	assert(arr);
	if (!arr) {
		return;
	}

	if (left >= right) {
		return;
	}

	if ((right - left) <= 13) {
		InsertSort(arr + left, right - left + 1);
	}
	else {
		int hole = 0;
		//Randomkey(&keyi, left, right);
		GetMidNumi(arr, &hole, left, right);
		if (hole != left) {
			Swap(&arr[left], &arr[hole]);
			hole = left;
		}
		int key = arr[left];

		int begin = left;
		int end = right;

		while (begin < end) {
			while (begin < end && arr[end] >= arr[hole]) {
				--end;
			}
			arr[hole] = arr[end];
			hole = end;

			while (begin < end && arr[begin] <= arr[hole]) {
				++begin;
			}
			arr[hole] = arr[begin];
			hole = begin;
		}
		arr[hole] = key;

		QucikSortHorae(arr, left, begin - 1);
		QucikSortHorae(arr, end, right);
	}
}
//双指针法 P -- pointer
void QucikSortTwoP(int* arr, int left, int right) {
	assert(arr);
	if (!arr) {
		return;
	}

	if (left >= right) {
		return;
	}

	if ((right - left) < 13) {
		InsertSort(arr + left, right - left + 1);
	}
	else {
		int keyi = 0;
		//Randomkey(&keyi, left, right);
		GetMidNumi(arr, &keyi, left, right);
		if (keyi != left) {
			Swap(&arr[left], &arr[keyi]);
			keyi = left;
		}

		int prev = left;
		int cur = left + 1;

		while (cur <= right) {
			if (arr[cur] < arr[keyi] && ++prev != cur) {
				Swap(&arr[cur], &arr[prev]);
			}

			++cur;
		}
		Swap(&arr[keyi], &arr[prev]);

		QucikSortTwoP(arr, left, prev - 1);
		QucikSortTwoP(arr, prev + 1, right);
	}
}
//三路划分(处理大量重复数据 -- 存在性能损失) R -- route
void QucikSortThrR(int* arr, int left, int right) {
	assert(arr);
	if (!arr) {
		return;
	}

	if (left >= right) {
		return;
	}

	if ((right - left) <= 13) {
		InsertSort(arr + left, right - left + 1);
	}
	else {
		int keyi = 0;
		Randomkey(&keyi, left, right);
		//GetMidNumi(arr, &keyi, left, right);
		if (keyi != left) {
			Swap(&arr[left], &arr[keyi]);
			keyi = left;
		}
		int key = arr[keyi];

		int begin = left;
		int cur = left + 1;
		int end = right;

		while (cur <= end) {
			if (arr[cur] < key) {
				Swap(&arr[cur], &arr[begin]);
				++cur;
				++begin;
			}
			else if (arr[cur] > key) {
				Swap(&arr[cur], &arr[end]);
				--end;
			}
			else {
				++cur;
			}
		}

		QucikSortThrR(arr, left, begin - 1);
		QucikSortThrR(arr, end + 1, right);
	}	
}

//归并排序(递归)
void _MergeSort(int* arr, int begin, int end, int* tmp) {
	assert(arr);
	if (!arr) {
		return;
	}
	assert(tmp);
	if (!tmp) {
		return;
	}

	if (begin >= end) {
		return;
	}

	int mid = (end + begin) / 2;
	_MergeSort(arr, begin, mid, tmp);
	_MergeSort(arr, mid + 1, end, tmp);

	int begin1 = begin, end1 = mid;
	int begin2 = mid + 1, end2 = end;
	int i = begin;
	while (begin1 <= end1 && begin2 <= end2) {
		if (arr[begin1] < arr[begin2]) {
			tmp[i++] = arr[begin1++];
		}
		else {
			tmp[i++] = arr[begin2++];
		}
	}

	while (begin1 <= end1) {
		tmp[i++] = arr[begin1++];
	}

	while (begin2 <= end2) {
		tmp[i++] = arr[begin2++];
	}

	memcpy(arr + begin, tmp + begin, sizeof(int) * (end - begin + 1));
}
void MergeSort(int* arr, int num) {
	assert(arr);
	if (!arr) {
		return;
	}

	int* tmp = (int*)malloc(sizeof(int) * num);
	assert(tmp);
	if (!tmp) {
		perror("malloc");
		return;
	}

	_MergeSort(arr, 0, num - 1, tmp);

	free(tmp);
	tmp = NULL;
}
//归并排序(直接改循环 -- 类似二叉树后序遍历 -> 不好用栈)(非递归)
void MergeSortNonRFir(int* arr, int num) {
	assert(arr);
	if (!arr) {
		return;
	}

	int* tmp = (int*)malloc(sizeof(int) * num);
	assert(tmp);
	if (!tmp) {
		perror("malloc");
		return;
	}

	int gap = 1;
	while (gap < num) {
		for (int i = 0; i < num; i += 2 * gap) {
			int begin1 = i;
			int end1 = begin1 + gap - 1;
			int begin2 = end1 + 1;
			int end2 = begin2 + gap - 1;

			if (end1 >= num || begin2 >= num) {
				break;
			}
			if (end2 >= num) {
				end2 = num - 1;
			}

			int j = begin1;
			while (begin1 <= end1 && begin2 <= end2) {
				if (arr[begin1] < arr[begin2]) {
					tmp[j++] = arr[begin1++];
				}
				else {
					tmp[j++] = arr[begin2++];
				}
			}

			while (begin1 <= end1) {
				tmp[j++] = arr[begin1++];
			}
			while (begin2 <= end2) {
				tmp[j++] = arr[begin2++];
			}

			memcpy(arr + i, tmp + i, sizeof(int) * (end2 - i + 1));
		}
		gap *= 2;
	}

	free(tmp);
	tmp = NULL;
}
void MergeSortNonRSec(int* arr, int num) {
	assert(arr);
	if (!arr) {
		return;
	}

	int* tmp = (int*)malloc(sizeof(int) * num);
	assert(tmp);
	if (!tmp) {
		perror("malloc");
		return;
	}

	int gap = 1;
	while (gap < num) {
		for (int i = 0; i < num; i += 2 * gap) {
			int begin1 = i;
			int end1 = begin1 + gap - 1;
			int begin2 = end1 + 1;
			int end2 = begin2 + gap - 1;

			if (end1 >= num) {
				end1 = num - 2;
				begin2 = num;
				end2 = num - 1;
			}
			else if (begin2 >= num) {
				begin2 = num;
				end2 = num - 1;
			}
			else if (end2 >= num) {
				end2 = num - 1;
			}

			int j = begin1;
			while (begin1 <= end1 && begin2 <= end2) {
				if (arr[begin1] < arr[begin2]) {
					tmp[j++] = arr[begin1++];
				}
				else {
					tmp[j++] = arr[begin2++];
				}
			}

			while (begin1 <= end1) {
				tmp[j++] = arr[begin1++];
			}
			while (begin2 <= end2) {
				tmp[j++] = arr[begin2++];
			}
		}
		memcpy(arr, tmp, sizeof(int) * num);
		gap *= 2;
	}

	free(tmp);
	tmp = NULL;
}
void MergeSortNonRThi(int* arr, int num) {
	assert(arr);
	if (!arr) {
		return;
	}

	int* tmp = (int*)malloc(sizeof(int) * num);
	assert(tmp);
	if (!tmp) {
		perror("malloc");
		return;
	}
	memcpy(tmp, arr, sizeof(int) * num);

	int gap = 1;
	while (gap < num) {
		for (int i = 0; i < num; i += 2 * gap) {
			int begin1 = i;
			int end1 = begin1 + gap - 1;
			int begin2 = end1 + 1;
			int end2 = begin2 + gap - 1;

			if (end1 >= num || begin2 >= num) {
				break;
			}
			if (end2 >= num) {
				end2 = num - 1;
			}

			int j = begin1;
			while (begin1 <= end1 && begin2 <= end2) {
				if (arr[begin1] < arr[begin2]) {
					tmp[j++] = arr[begin1++];
				}
				else {
					tmp[j++] = arr[begin2++];
				}
			}

			while (begin1 <= end1) {
				tmp[j++] = arr[begin1++];
			}
			while (begin2 <= end2) {
				tmp[j++] = arr[begin2++];
			}
		}
		memcpy(arr, tmp, sizeof(int) * num);
		gap *= 2;
	}

	free(tmp);
	tmp = NULL;
}


//外排序 -- 比较排序
//归并排序
void _MergeFile(const char* file1, const char* file2, const char* mfile) {
	assert(file1);
	if (!file1) {
		return;
	}
	assert(file2);
	if (!file2) {
		return;
	}
	assert(mfile);
	if (!mfile) {
		return;
	}

	FILE* fout1 = fopen(file1, "r");
	assert(fout1);
	if (!fout1) {
		perror("fopen");
		return;
	}
	FILE* fout2 = fopen(file2, "r");
	assert(fout2);
	if (!fout2) {
		perror("fopen");
		return;
	}	
	FILE* fin = fopen(mfile, "w");
	assert(fin);
	if (!fin) {
		perror("fopen");
		return;
	}

	int num1 = 0;
	int num2 = 0;
	int ret1 = fscanf(fout1, "%d\n", &num1);
	int ret2 = fscanf(fout2, "%d\n", &num2);
	while (ret1 != EOF && ret2 != EOF) {
		if (num1 < num2) {
			fprintf(fin, "%d\n", num1);
			ret1 = fscanf(fout1, "%d\n", &num1);
		}
		else {
			fprintf(fin, "%d\n", num2);
			ret2 = fscanf(fout2, "%d\n", &num2);
		}
	}
	while (ret1 != EOF) {
		fprintf(fin, "%d\n", num1);
		ret1 = fscanf(fout1, "%d\n", &num1);
	}
	while (ret2 != EOF) {
		fprintf(fin, "%d\n", num2);
		ret2 = fscanf(fout2, "%d\n", &num2);
	}

	fclose(fout1);
	fout1 = NULL;	
	fclose(fout2);
	fout2 = NULL;	
	fclose(fin);
	fin = NULL;
}
void MergeSortFile(const char* file) {
	assert(file);
	if (!file) {
		return;
	}

	FILE* fout = fopen(file, "r");
	assert(fout);
	if (!fout) {
		perror("fopen");
		return;
	}

	int num = 10;
	int* tmp = (int*)calloc(num, sizeof(int));
	assert(tmp);
	if (!tmp) {
		perror("calloc");
		return;
	}
	int tmpnum = 0;
	int count = 0;
	char profile[10];
	int filei = 1;

	while (fscanf(fout, "%d\n", &tmpnum) != EOF) {
		if (count < num - 1) {
			tmp[count++] = tmpnum;
		}
		else {
			tmp[count++] = tmpnum;
			QuickSort(tmp, 0, num - 1);

			sprintf(profile, "%d", filei++);
			FILE* fin = fopen(profile, "w");
			assert(fin);
			if (!fin) {
				perror("fopen");
				return;
			}

			for (int i = 0; i < num; ++i) {
				fprintf(fin, "%d\n", tmp[i]);
			}

			fclose(fin);
			fin = NULL;

			count = 0;
			memset(tmp, 0, sizeof(int) * num);
		}
	}

	char mfile[20] = "12";
	char file1[20] = "1";
	char file2[20] = "2";
	for (int i = 2; i <= num; ++i) {
		_MergeFile(file1, file2, mfile);

		strcpy(file1, mfile);
		sprintf(file2, "%d", i + 1);
		sprintf(mfile, "%s%d", mfile, i + 1);
	}

	free(tmp);
	tmp = NULL;
	fclose(fout);
	fout = NULL;
}

//非比较排序	
//计数排序
void CountSort(int* arr, int num) {
	assert(arr);
	if (!arr) {
		return;
	}

	int min = arr[0];
	int max = arr[0];
	for (int i = 1; i < num; ++i) {
		if (arr[i] < min) {
			min = arr[i];
		}
		if (arr[i] >max) {
			max = arr[i];
		}
	}

	int range = max - min + 1;
	int* CountA = (int*)calloc(range, sizeof(int));
	assert(CountA);
	if (!CountA) {
		perror("calloc");
		return;
	}

	for (int i = 0; i < num; ++i) {
		CountA[arr[i] - min]++;
	}

	int j = 0;
	for (int i = 0; i < range; ++i) {
		while (CountA[i]--) {
			arr[j++] = i + min;
		}
	}
}
//基数排序 -- MSD：最高次序优先(升序)、LSD：最低次序优先(降序) -- 队列
void RadixSort(int* arr, int num, int degree, int radix) {
	assert(arr);
	if (!arr) {
		return;
	}

	//Queue q[radix];//请改为clang,否则只能用宏定义
	Queue q[10];//测试用
	for (int i = 0; i < radix; ++i) {
		QueueInit(&q[i]);
	}

	int degree1 = 1;
	while (degree1 <= degree) {
		int ret = 0;
		for (int i = 0; i < num; ++i) {
			//getkey -- 得到关键字
			int value = arr[i];
			int degree2 = degree1;
			while (--degree2 >= 0) {
				ret = value % 10;
				value /= 10;
			}

			//distribute分发
			QueuePush(&q[ret], arr[i]);
		}

		//collect回收
		int j = 0;
		for (int i = 0; i < num; ++i) {
			while (!QueueEmpty(&q[i])) {
				arr[j++] = QueueFront(&q[i]);
				QueuePop(&q[i]);
			}
		}

		++degree1;
	}
}