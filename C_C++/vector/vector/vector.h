#pragma once

#include <iostream>
#include <assert.h>

using namespace std;

namespace wzz {
	template <typename T>
	class vector {
	public:
		typedef T* iterator;
		typedef const T* const_iterator;
	public:
		//construct and destory
		vector() {}
		
		vector(int num, const T& value = T()) {
			resize(num, value);
		}

		vector(size_t num, const T& value = T()) {
			resize(num, value);
		}

		template<typename Inputleterator>
		vector(Inputleterator first, Inputleterator last) {
			while (first != last) {
				push_back(*first);
				++first;
			}
		}

		vector(const vector<T>& v) {
			_start = new T[v.capacity()];
			for (size_t i = 0; i < v.size(); ++i) {
				_start[i] = v._start[i];
			}

			_finish = _start + v.size();
			_endofstorage = _start + v.capacity();
		}

		vector<T>& operator=(vector<T> v) {
			swap(v);
			return *this;
		}

		~vector() {
			if (_start) {
				delete[] _start;
				_start = _finish = _endofstorage = nullptr;
			}
		}

		//iterator
		iterator begin() {
			return _start;
		}

		iterator end() {
			return _finish;
		}

		const_iterator begin() const {
			return _start;
		}

		const_iterator end() const {
			return _finish;
		}

		//cbegin 和 cend用的很少，范围for只支持用begin替换（不会用cbegin替换），因此（如果形参有const）必须写const类型的begin重载
		const_iterator cbegin() const {
			return _start;
		}

		const_iterator cend() const {
			return _finish;
		}

		//capacity
		size_t size() const {
			return (_finish - _start);
		}

		size_t capacity() const {
			return (_endofstorage - _start);
		}

		void reserve(size_t n) {
			if ((n > capacity()) || ((n < capacity()) && (n > size()))) {
				size_t sz = size();
				T* tmp = new T[n];
				if (_start) {
					for (size_t i = 0; i < sz; ++i) {
						tmp[i] = _start[i];
					}
					delete[] _start;
				}
				_start = tmp;
				_finish = _start + sz;
				_endofstorage = _start + n;
			}
		}

		void resize(size_t n, const T& value = T()) {
			if (n < size()) {
				_finish = _start + n;
			}
			else {
				reserve(n);

				while (_finish != _start + n) {
					*_finish = value;
					++_finish;
				}
			}
		}

		//access
		T& operator[](size_t pos) {
			assert(pos < size());
			return _start[pos];
		}

		const T& operator[](size_t pos) const {
			assert(pos < size());
			return _start[pos];
		}

		//modify
		void push_back(const T& num) {
			//直接写
			//if (_finish == _endofstorage) {
			//	size_t newcapacity = capacity() == 0 ? 4 : capacity() * 2;
			//	reserve(newcapacity);
			//}
			//*_finish = num;
			//++_finish;
			
			//复用
			insert(end(), num);
		}

		void pop_back() {
			iterator it = end();
			erase(--it);
		}

		void swap(vector<T>& v) {
			std::swap(_start, v._start);
			std::swap(_finish, v._finish);
			std::swap(_endofstorage, v._endofstorage);
		}

		iterator insert(iterator pos, const T& num) {
			assert(pos >= _start && pos <= _finish);

			if (_finish == _endofstorage) {
				size_t len = pos - _start;

				size_t newcapacity = capacity() == 0 ? 4 : capacity() * 2;
				reserve(newcapacity);

				pos = _start + len; //迭代器失效
			}

			iterator end = _finish - 1;
			while (end >= pos) {
				*(end + 1) = *end;
				--end;
			}

			*pos = num;
			++_finish;

			return pos;
		}

		iterator erase(iterator pos) {
			assert(pos >= _start && pos < _finish);

			iterator it = pos + 1;
			while (it != _finish) {
				*(it - 1) = *it;
				++it;
			}

			--_finish;

			return pos;
		}
	private:
		iterator _start = nullptr;
		iterator _finish = nullptr;
		iterator _endofstorage = nullptr;
	};

	template <typename T>
	void Print(const vector<T>& v) {
		for (auto e : v) {
			cout << e << " ";
		}
		cout << endl;
	}

	void vector_Test() {
		vector<int> v;
		v.push_back(1);
		Print(v);
		v.pop_back();
		Print(v);
		v.push_back(2);
		v.push_back(3);
		Print(v);
	}
}