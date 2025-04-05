#pragma once
// 智能指针头文件 #include <allocator> / <memory>
#include <iostream>
using namespace std;

namespace SmartPtr {
	template <typename T>
	class auto_ptr {
	public:
		auto_ptr(T* ptr)
			:_ptr(ptr)
		{}

		auto_ptr(auto_ptr<T>& ptr)
			:_ptr(ptr._ptr)
		{
			ptr._ptr = nullptyr;
		}

		T& operator*() {
			return *_ptr;
		}

		T* operator->() {
			return _ptr;
		}

		~auto_ptr() {
			delete _ptr;
		}
	private:
		T* _ptr;
	};

	template <typename T>
	class unique_ptr {
	public:
		unique_ptr(T* ptr)
			:_ptr(ptr)
		{}

		T& operator*() {
			return *_ptr;
		}

		T* operator->() {
			return _ptr;
		}

		~unique_ptr() {
			delete _ptr;
		}
	private:
		unique_ptr(unique_ptr<T>& ptr) = delete;
		unique_ptr<T>& operator = (unique_pttr<T>& ptr) = delete;
	private:
		T* _ptr;
	};

	template <typename T>
	class shared_ptr {
	public:
		shared_ptr(T* ptr)
			:_ptr(ptr)
			,_count(new int(1))
		{}

		template <typename D>
		shared_ptr(T* ptr, D del)
			:_ptr(ptr)
			,_count(new int(1))
			,_del(del)
		{}

		shared_ptr(const shared_ptr<T>& ptr) 
			:_ptr(ptr._ptr)
			,_count(ptr._count)
		{
			++(*_count);
		}

		shared_ptr<T>& operator=(const shared_ptr<T>& ptr) {
			if (_ptr == ptr._ptr) {
				return *this;
			}

			~shared_ptr(); // 原先就存在的，原先就指向对象的，如果只有它一个指针指向此时改变指向需析构原先指向的对象

			_ptr = ptr._ptr;
			_count = ptr._count;
			++(*_count);
			return *this;
		}

		T& operator*() {
			return *_ptr;
		}

		T* operator->() {
			return _ptr;
		}

		int use_count() const {
			return *_count;
		}

		T* get() const {
			return _ptr;
		}

		~shared_ptr() {
			if (--(*_count) == 0) {
				_del(_ptr);
				delete _count;
			}
		}
	private:
		T* _ptr;
		int* _count;
		function<void(T*)> _del = [](T* ptr) {delete ptr; };
	};

	template <typename T>
	class weak_ptr {
	public:
		weak_ptr()
			:_ptr(nullptr)
		{}

		weak_ptr(const shared_ptr<T>& ptr)
			:_ptr(ptr.get())
		{}

		T& operator*() {
			return *_ptr;
		}

		T* operator->() {
			return _ptr;
		}
	private:
		T* _ptr;
	};
}