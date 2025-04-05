#include <iostream>
using namespace std;

// 1.
// 结束条件的函数
//void _Print() {
//	cout << endl;
//}
//template<typename T, typename ...Args>
//void _Print(T value, Args... args) {
//	cout << value << " ";
//	_Print(args...);
//}
//template <typename ...Args>
//void Print(Args... args) {
//	_Print(args...);
//}

// 2.
//void Print() {
//	cout << endl;
//}
//template <typename V>
//void _Print(V v) {
//	cout << v << " ";
//}
//template <typename ...Args>
//void Print(Args... args) {
//	int a[] = { (_Print(args),0)... };
//	cout << endl;
//}

// 3.
//void Print() {
//	cout << endl;
//}
//template <typename V>
//int _Print(V v) {
//	cout << v << " ";
//	return 0;
//}
//template <typename ...Args>
//void Print(Args... args) {
//	int a[] = { _Print(args)... };
//	cout << endl;
//}

// Test
//int main() {
//	Print();
//	Print(1);
//	Print(1, 2, 3, 4.4);
//	Print(1, 2, 3, 4, 5, 6.6, "xxxxxx");
//	Print(1,2,3,4,5,6.6,'y', "xxxxxx");
//	return 0;
//}

// 实用举例 -- emplace_back
//class Date{
//public:
//	Date(int year = 1, int month = 1, int day = 1)
//		:_year(year)
//		, _month(month)
//		, _day(day){
//		cout << "Date构造" << endl;
//	}
//	Date(const Date& d)
//		:_year(d._year)
//		, _month(d._month)
//		, _day(d._day){
//		cout << "Date拷贝构造" << endl;
//	}
//private:
//	int _year;
//	int _month;
//	int _day;
//};
//template <class ...Args>
//Date* Create(Args... args){
//	Date* ret = new Date(args...);
//	return ret;
//}
//int main(){
//	Date* p1 = Create();
//	Date* p2 = Create(2023);
//	Date* p3 = Create(2023, 9);
//	Date* p4 = Create(2023, 9, 27);
//
//	Date d(2023, 1, 1);
//	Date* p5 = Create(d);
//
//	return 0;
//}