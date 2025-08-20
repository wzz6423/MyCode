#include <stdio.h>
#include <math.h>

int main() {
	for (int i = 101; i <= 200; i += 2){
		int flag = 1;
		for (int j = 2; j <= sqrt(i); j++){
			if (i % j == 0){
				flag = 0;
				break;
			}
		}
		if (flag == 1) {
			printf("%d ", i);
		}
	}

	return 0;
}