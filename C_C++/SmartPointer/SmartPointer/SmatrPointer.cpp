//#include <iostream>
//#include <string>
//using namespace std;
//// �򵥹���ʵ��
//template <typename T>
//class SmartPointer {
//public:
//	SmartPointer(T* ptr)
//		:_ptr(ptr)
//	{ }
//
//	T& operator*() {
//		return *_ptr;
//	}
//
//	T* operator->() {
//		return _ptr;
//	}
//
//	~SmartPointer(){
//		cout << "delete" << endl;
//		delete _ptr;
//	}
//private:
//	T* _ptr;
//};
//void test(int x) {
//	SmartPointer<pair<string, string>> pp2 = new pair<string, string>;
//	if (x == 0) {
//		throw "int����ֵΪ0";
//	}
//}
//int main() {
//	SmartPointer<pair<string, string>> pp1 = new pair<string, string>;
//
//	try{
//		SmartPointer<string> ps1 = new string("xxxxxxxxxx");
//		cout << (*ps1) << endl;
//		test(0);
//		SmartPointer<string> ps2 = new string("xxxxxxxxxx");
//	}
//	catch (const char* str) {
//		cout << str << endl;
//	}
//
//	return 0;
//}

// ����lambda��������װ����ʹ�ú�����ָ���ģ��ʵ��
#include "SmartPointer.h"
int main() {

	return 0;
}