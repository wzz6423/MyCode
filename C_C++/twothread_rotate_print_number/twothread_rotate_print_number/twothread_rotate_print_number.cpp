#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable> 
#include <atomic>

// rotate_print : mutex with RAII
//int main() {
//	int x = 1;
//	bool flag = true;
//	std::mutex mtx;
//	std::condition_variable cv;
//
//	std::thread t1([&]() {
//		for (size_t i = 0; i < 10; ++i) {
//			std::unique_lock<std::mutex> lock(mtx);
//			while (flag) {
//				cv.wait(lock);
//			}
//
//			std::cout << std::this_thread::get_id() << ", number is : " << x++ << std::endl;
//
//			flag = true;
//			cv.notify_one();
//		}
//	});
//
//	std::thread t2([&]() {
//		for (size_t i = 0; i < 10; ++i) {
//			std::unique_lock<std::mutex> lock(mtx);
//			while (!flag) {
//				cv.wait(lock);
//			}
//
//			std::cout << std::this_thread::get_id() << ", number is : " << x++ << std::endl;
//
//			flag = false;
//			cv.notify_one();
//		}
//	});
//
//	t1.join();
//	t2.join();
//
//	return 0;
//}

// atomic -- 原子性仅保证对x进行操作，所以只能计算加和，在线程内打印x值就不是原子的，加锁也没用
// （打印的时候可能t1跑完了循环，t2没跑完所有值不一定正确）
//int main() {
//	std::atomic<int> x = 0;
//	std::mutex mtx;
//
//	std::thread t1([&]() {
//		for (size_t i = 0; i < 1000; ++i) {
//			//std::cout << std::this_thread::get_id() << ", number is : " << x++ << std::endl;
//			++x;
//		}
//		mtx.lock();
//		std::cout << std::this_thread::get_id() << ", number is : " << x << std::endl;
//		mtx.unlock();
//	});
//
//	std::thread t2([&]() {
//		for (size_t i = 0; i < 1000; ++i) {
//			++x;
//		}
//		mtx.lock();
//		std::cout << std::this_thread::get_id() << ", number is : " << x << std::endl;
//		mtx.unlock();
//	});
//
//	t1.join();
//	t2.join();
//
//	std::cout << x << std::endl;
//
//	return 0;
//}