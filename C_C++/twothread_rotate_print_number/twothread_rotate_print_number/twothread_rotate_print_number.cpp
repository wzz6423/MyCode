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

// atomic -- ԭ���Խ���֤��x���в���������ֻ�ܼ���Ӻͣ����߳��ڴ�ӡxֵ�Ͳ���ԭ�ӵģ�����Ҳû��
// ����ӡ��ʱ�����t1������ѭ����t2û��������ֵ��һ����ȷ��
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