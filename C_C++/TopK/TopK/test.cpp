#define _CRT_SECURE_NO_WARNINGS 1

#include "TopK.h"

void CreatData() {
	size_t sz = 50;
	srand(time(0));

	FILE* fin = fopen("data.txt", "w");
	assert(fin);

	for (int i = 0; i < sz; i++) {
		int tmp = rand() % 100000;
		fprintf(fin," %d\n", tmp);
	}

	fclose(fin);	
	fin = NULL;
}

void TopK(const char* file, int num) {
	int* topk = (int*)malloc(sizeof(int) * num);
	assert(topk);

	FILE* fin = fopen(file, "r");
	assert(fin);

	for (int i = 0; i < num; ++i) {
		fscanf(fin, "%d", &topk[i]);
	}

	for (int i = (num - 2) / 2; i >= 0; --i) {
		AdjustDown(topk, num, i);
	}

	int val = 0;
	int ret = fscanf(fin, "%d", &val);
	while (ret != EOF) {
		if (val > topk[0]) {
			topk[0] = val;
			AdjustDown(topk, num, 0);
		}
		ret = fscanf(fin, "%d", &val);
	}

	for (int i = 0; i < num; ++i) {
		printf("%d ", topk[i]);
	}
	printf("\n");

	free(topk);
	topk = NULL;
	fclose(fin);
	fin = NULL;
}

int main() {
	CreatData();
	TopK("data.txt", 10);

	return 0;
}