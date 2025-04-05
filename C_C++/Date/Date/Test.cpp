#include "Date.h"

int main() {
	Date d1(2024, 12, 22);
	d1.Print();

	Date d2(d1);
	d2.Print();

	Date d3 = d2;
	d3.Print();

	if (d2 < d1) {
		cout << "AAAAAAA" << endl;
	}
	else {
		cout << "BBBBBBBB" << endl;
	}

	d2 -= 100;
	d2.Print();

	d2 += 200;
	d2.Print();

	d2 -= -100;
	d2.Print();

	d2 += -100;
	d2.Print();

	Date d4 = d2--;
	d2.Print();
	d4.Print();

	d4 = d2++;
	d2.Print();
	d4.Print();

	return 0;
}