#pragma once
#include <vector>

namespace wzz {
	template <size_t N>
	class bitset {
	public:
		bitset() {
			_a.resize(N / 32 + 1); // 向上取整
		}

		void set(size_t val) {
			size_t i = val / 32;
			size_t j = val % 32;

			_a[i] |= (1 << j); // 位操作将1移动并进行按位或(有1则为1)
		}

		void reset(size_t val) {
			size_t i = val / 32;
			size_t j = val % 32;

			_a[i] &= (~(1 << j)); // 按位与(均为1为1，否则为0)
		}

		bool test(size_t val) {
			size_t i = val / 32;
			size_t j = val % 32;

			return _a[i] & (1 << j);
		}
	private:
		vector<int> _a;
	};
}