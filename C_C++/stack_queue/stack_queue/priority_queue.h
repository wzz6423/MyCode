#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
using namespace std;

template <typename T>
class Less {
	public:
		bool operator()(const T& x, const T& y) {
			return x < y;
		}
};

template <typename T>
class Greater {
public:
	bool operator()(const T& x, const T& y) {
		return x > y;
	}
};

namespace thu {
	template <typename T, typename Container = vector<T>, typename Compare = less<T>>
	class priority_queue {
	private:
		void AdjustDown(int parent) {
			Compare com;
			int child = parent * 2 + 1;

			while (child < _con.size()) {
				if (child + 1 < _con.size() && com(_con[child], _con[child + 1])) {
					++child;
				}

				if (com(_con[parent], _con[child])) {
					std::swap(_con[child], _con[parent]);
					parent = child;
					child = parent * 2 + 1;
				}
				else {
					break;
				}
			}
		}

		void AdjustUp(int child) {
			Compare com;
			int parent = (child - 1) / 2;

			while (child > 0) {
				if (com(_con[parent], _con[child])) {
					std::swap(_con[child], _con[parent]);
					child = parent;
					parent = (child - 1) / 2;
				}
				else {
					break;
				}
			}
		}

	public:
		priority_queue(){}

		template<typename InputIterator>
		priority_queue(InputIterator first, InputIterator last) {
			while (first != last) {
				_con.push_back(*first);
				++first;
			}

			// ½¨¶Ñ
			for (int i = (_con.size() - 1 - 1) / 2; i >= 0; --i) {
				AdjustDown(i);
			}
		}

		void push(const T& val) {
			_con.push_back(val);

			AdjustUp(_con.size() - 1);
		}

		const T& top() {
			return _con[0];
		}

		void pop() {
			std::swap(_con[0], _con[_con.size() - 1]);
			_con.pop_back();

			AdjustDown(0);
		}

		bool empty() {
			return _con.empty();
		}

		size_t size() {
			return _con.size();
		}

	private:
		Container _con;
	};
}