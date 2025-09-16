#include <stdio.h>

// 不报错
void test() {
	printf("Hello World!");
}
// VS2013, 警告
// VS2026, 报错
void test(void) {
	printf("Hello World!");
}

int main() {
	test(1, 2, 3, 4);

	return 0;
}