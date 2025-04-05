#pragma once
#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

namespace wzz {
	class string {
		friend ostream& operator<<(ostream& out, const string& str);
		friend istream& operator>>(istream& in, string& str);
	public:
		const static size_t npos;

		typedef char* iterator;
		typedef const char* const_iterator;
	public:
		//number
		string(const char* st = "") {
			_size = strlen(st);
			_capacity = _size;
			_str = new char[_capacity + 1];
			memcpy(_str, st, _size + 1); // '\0'
		}

		string(const string& str) {
			_size = str._size;
			_capacity = str._capacity;
			_str = new char[_capacity + 1];
			memcpy(_str, str._str, _size + 1);
		}

		//老版本
		//string& operator=(const string& str) {
		//	if (this != &str) {
		//		char* tmp = new char[str._capacity + 1];
		//		memcpy(tmp, str._str, _size + 1);
		//		delete[] _str;
		//		_str = tmp;
		//		_size = str._size;
		//		_capacity = str._capacity;
		//	}
		//	return *this;
		//}

		//新版本
		string& operator=(string str) {
			swap(str);
			return *this;
		}

		~string() {
			delete[] _str;
			_str = nullptr;
			_size = _capacity = 0;
		}

		//iterator
		iterator begin() {
			return _str;
		}

		iterator end() {
			return _str + _size;
		}

		const iterator begin() const {
			return _str;
		}

		const iterator end() const {
			return _str + _size;
		}

		//modify
		void push_back(char ch) {
			if (_size == _capacity) {
				reserve(_capacity * 2);
			}

			_str[_size] = ch;
			++_size;
			_str[_size] = '\0';
		}

		void append(const char* st) {
			size_t len = strlen(st);
			if (_size + len > _capacity) {
				reserve(_size + len);
			}
			memcpy(_str + _size, st, len + 1);
			_size += len;
		}

		string& operator+=(const char ch) {
			push_back(ch);
			return *this;
		}

		string& operator+=(const char* st) {
			append(st);
			return *this;
		}

		void clear() {
			_str[0] = '\0';
			_size = 0;
		}

		void swap(string& str) {
			std::swap(this->_str, str._str);
			std::swap(this->_size, str._size);
			std::swap(this->_capacity, str._capacity);
		}

		const char* c_str() const {
			return _str;
		}

		//capacity
		size_t size() const {
			return _size;
		}

		size_t capacity() const {
			return _capacity;
		}

		bool empty() const {
			return (_size == 0);
		}

		void resize(size_t cap, char c = '\0') {
			if (cap < _size) {
				_str[cap] = '\0';
				_size = cap;
			}
			else {
				reserve(cap);

				for (size_t i = _size; i < _capacity; ++i) {
					_str[i] = c;
				}

				_size = cap;
				_str[_size] = '\0';
			}
		}

		void reserve(size_t cap) {
			if ((cap > _capacity) || ((cap < _capacity) && (cap > _size))) {
				char* tmp = new char[cap + 1];
				memcpy(tmp, _str, _size + 1);
				delete[] _str;
				_str = tmp;
				_capacity = cap;
			}
		}

		//access
		char& operator[](size_t index) {
			assert(index >= 0);
			assert(index < _size);

			return _str[index];
		}

		const char& operator[](size_t index) const {
			assert(index >= 0);
			assert(index < _size);

			return _str[index];
		}
		
		//relation opertors
		bool operator <(const string& str)  const {
			int ret = memcmp(_str, str._str, _size < str._size ? _size : str._size);
			return ret == 0 ? _size < str._size : ret < 0;
		}

		bool operator == (const string & str) const {
			return (_size == str._size) && (0 == memcmp(_str, str._str, _size));
		}

		bool operator <= (const string& str) const {
			return *this < str || *this == str;
		}

		bool operator > (const string& str) const {
			return !(*this <= str);
		}

		bool operator >= (const string& str) const {
			return !(*this < str);
		}

		bool operator != (const string& str) const {
			return !(*this == str);
		}

		//find
		size_t find(char ch, size_t pos = 0)const {
			assert(pos < _size);

			for (size_t i = pos; i < _size; ++i) {
				if (_str[i] == ch) {
					return i;
				}
			}

			return npos;
		}

		size_t find(const char* st, size_t pos = 0) const {
			assert(pos < _size);

			const char* ptr = strstr(_str + pos, st);
			if (ptr) {
				return ptr - _str;
			}
			else {
				return npos;
			}
		}

		//insert & earse
		void insert(size_t pos, char ch, size_t num = 1) {
			assert(pos <= _size);
			assert(pos >= 0);

			if (_size + num > _capacity) {
				reserve(_size + num);
			}

			size_t end = _size;
			while (end >= pos && end != npos) { // -1越界问题
				_str[end + num] = _str[end];
				--end;
			}
			for (size_t i = num; i > 0; --i) {
				_str[pos + i] = ch;
			}

			_size += num;
		}
		
		void insert(size_t pos, const char* st) {
			assert(pos <= _size);
			assert(pos >= 0);

			size_t len = strlen(st);
			if (_size + len > _capacity) {
				reserve(_size + len);
			}

			size_t end = _size;
			while (end >= pos && end != npos) { // -1越界问题
				_str[end + len] = _str[end];
				--end;
			}
			for (size_t i = 0; i < len; ++i) {
				_str[pos + i] = st[i];
			}

			_size += len;
		}

		void earse(size_t pos, size_t len = npos) {
			assert(pos >= 0);
			assert(pos < _size);

			if (len == npos || pos + len >= _size) {
				_size = pos;
				_str[_size] = '\0';
			}
			else {
				size_t end = pos + len;
				while (end <= _size) {
					_str[pos++] = _str[end++];
				}
				_size -= len;
			}
		}

		//substr
		string substr(size_t pos = 0, size_t len = npos) {
			assert(pos < _size);

			size_t n = len;
			if (len == pos || pos + len > _size) {
				n = _size - pos;
			}

			string tmp;
			tmp.reserve(n);
			for (size_t i = pos; i < pos + n; ++i) {
				tmp += _str[i];
			}

			return tmp;
		}
	private:
		char* _str;
		size_t _size;
		size_t _capacity;
	};
	const size_t string::npos = -1;

	ostream& operator<<(ostream& out, const string& str) {
		for (auto e : str) {
			out << e;
		}
		return out;
	}

	istream& operator>>(istream& in, string& str) {
		str.clear();

		char ch = in.get();
		while (ch == ' ' || ch == '\n') {
			ch = in.get();
		}

		char buff[128] = { '\0' };
		int i = 0;

		while (ch != ' ' && ch != '\n') {
			buff[i++] = ch;
			if (i == 127) {
				buff[i] = '\0';
				str += buff;
				i = 0;
			}
			ch = in.get();
		}

		if (i) {
			buff[i] = '\0';
			str += buff;
		}

		return in;
	}

	void string_Test() {
		string s1("Hello World");
		cout << s1 << endl;

		string s2 = s1;
		cout << s2 << endl;

		string s3 = "xxxxxxxxxxxxx";
		s2 = s3;

		s1 += " Hello";
		cout << s1 << endl;
		s1 += 'w';
		cout << s1 << endl;
		s1.append("zz");
		cout << s1 <<endl;

		for (auto e : s1) {
			cout << e;
		}
		cout << endl;

		s2.clear();
		cout << s2 << endl;

		cin >> s2;
		s2.insert(s2.size(), "Thu: Hello Wzz");
		cout << s2;
	}
}