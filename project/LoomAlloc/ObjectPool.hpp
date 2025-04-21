#pragma once

#include "Common.hpp"

// 定长内存池
template <typename T>
class ObjectPool {
public:
	T* New(size_t mallocSize = 128 * 1024){
		T* obj = nullptr;

		// 优先使用被换回来的自由链表内的内存块对象
		if (_freeList != nullptr) {
			void* next = *((void**)_freeList); // 取到的是 _freeList 第一个内存块对象中存放的下一个内存块对象的地址，同时 *(void**) 可以被自动识别当前系统 x32/x64 以决定读取几字节
			// 如果只是 (void*) 只是转换了读取的数值的类型而没有自动识别读取几字节(指针：x32 -- 4， x64 -- 8)
			obj = (T*)_freeList;
			_freeList = next;
		}
		else {
			// 剩余内存块大小不足一个对象大小时，重新向系统申请内存空间
			if (_remainBytes < sizeof(T)) {
				_remainBytes = mallocSize; // 为什么不用 sizeof(T) 的整数倍向系统申请内存空间: 内存管理以页 (4kb) 为单位, 这样申请更加友好
				//_memory = (char*)Alloc(_remainBytes); // malloc
				_memory = (char*)Alloc(Span::SizeToNum(_remainBytes)); // system call
			}

			obj = (T*)_memory;
			size_t objSize = sizeof(T) < sizeof(void*) ? sizeof(void*) : sizeof(T);
			_memory += objSize;
			_remainBytes -= objSize;
		}

		// 定位new, 显示调用T的构造函数初始化
		new(obj)T;

		return obj;
	}

	void Delete(T* obj) {
		// 显示调用析构函数清理对象
		obj->~T();
		
		// 也可以给 obj 形参用引用，但要创建第三个变量
		*(void**)obj = _freeList;
		_freeList = obj;
	}
private:
	char* _memory = nullptr; // 指向大块内存的指针
	size_t _remainBytes = 0; // 大块内存被切分后剩余的字节数
	void* _freeList = nullptr; // 自由链表
};