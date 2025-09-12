module Math;
import std;

int Add(int a, int b) {
	return a + b;
}

//auto Add(auto a, auto b) {
//	return a + b;
//}

//template<typename T>
//T add(T a, T b) {
//	return a + b;
//}

auto Print(std::string msg) -> void {
	std::println(msg);
}

//auto Print(auto&& msg) -> void {
//	std::println(msg);
//}