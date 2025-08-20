//某地发⽣了⼀件盗窃案，警察通过排查确定盗窃者必为4个嫌疑⼈的⼀个。
//以下为4个嫌疑⼈的供词 :
//A说：不是我。
//B说：是C。
//C说：是D。
//D说：C在胡说
//已知3个⼈说了真话，1个⼈说的是假话。
//现在请根据这些信息，写⼀个程序来确定到底谁是盗窃者。

#include <stdio.h>

void FindKiller() {
	for (int killer = 'a'; killer <= 'd'; killer++) {
		if (((killer != 'a') + (killer == 'c') + (killer == 'd') + (killer != 'd')) == 3) {
			printf("凶手是：%c", killer);
		}
	}

}

int main() {
	FindKiller();
	return 0;
}