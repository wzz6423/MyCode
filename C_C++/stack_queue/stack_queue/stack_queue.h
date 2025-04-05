#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <deque>
using namespace std;

// ÈÝÆ÷ÊÊÅäÆ÷
namespace wzz {
	template <typename T, typename Container  = deque<T>>
	class stack {
	public:
		void push(const T& val) {
			_con.push_back(val);
		}

		void pop() {
			_con.pop_back();
		}

		T& top() {
			return _con.back();
		}

		size_t size() {
			return _con.size();
		}

		bool empty() {
			return _con.empty();
		}
	private:
		Container _con;
	};
}

namespace pku {
	template <typename T, typename Container = deque<T>>
	class queue{
	public:
		void push(const T& val) {
			_con.push_back(val);
		}

		void pop() {
			_con.pop_front();
		}

		T& front() {
			return _con.front();
		}

		T& back() {
			return _con.back();
		}

		size_t size() {
			return _con.size();
		}

		bool empty() {
			return _con.empty();
		}
	private:
		Container _con;
	}
}