#pragma once
#include <vector>

namespace wzz {
	template <size_t N>
	class bitset {
	public:
		bitset() {
			_a.resize(N / 32 + 1); // ����ȡ��
		}

		void set(size_t val) {
			size_t i = val / 32;
			size_t j = val % 32;

			_a[i] |= (1 << j); // λ������1�ƶ������а�λ��(��1��Ϊ1)
		}

		void reset(size_t val) {
			size_t i = val / 32;
			size_t j = val % 32;

			_a[i] &= (~(1 << j)); // ��λ��(��Ϊ1Ϊ1������Ϊ0)
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