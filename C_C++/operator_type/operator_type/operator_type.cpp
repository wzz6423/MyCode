#include <iostream>
using namespace std;

class operator_type {
public:
	operator_type(int a, double b)
		:_a(a)
		,_b(b)
	{ }

	operator int(){
		return _a;
	}

	operator double() {
		return _b;
	}

private:
	int _a;
	double _b;
};

void test() {
	operator_type ot(123, 123.456);
	int a = ot;
	double b = ot;
	cout << a << "   " << b << endl;
}

int main() {
	test();

	return 0;
}