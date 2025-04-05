#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

namespace wzz {
	class UnionFindSet {
	public:
		UnionFindSet(int size)
			:_ufs(size, -1)
		{
		}

		size_t FindRoot(int x) {
			int root = x;
			while (_ufs[root] >= 0) {
				root = _ufs[root];
			}

			// Â·¾¶Ñ¹Ëõ
			while (_ufs[x] >= 0) {
				int parent = _ufs[x];
				_ufs[x] = root;

				x = parent;
			}

			return root;
		}

		bool InSet(int x1, int x2) {
			return FindRoot(x1) == FindRoot(x2);
		}

		void Union(int x1, int x2) {
			int root1 = FindRoot(x1);
			int root2 = FindRoot(x2);

			if (root1 != root2) {
				if (abs(_ufs[root1]) < abs(_ufs[root2])) {
					std::swap(root1, root2);
				}

				_ufs[root1] += _ufs[root2];
				_ufs[root2] = root1;
			}
		}

		size_t SetSize() {
			size_t size = 0;
			for (size_t i = 0; i < _ufs.size(); ++i) {
				if (_ufs[i] < 0) {
					++size;
				}
			}

			return size;
		}

		UnionFindSet() = default;
	private:
		std::vector<int> _ufs;
	};
}