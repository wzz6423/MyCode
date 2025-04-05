#pragma once
#include "RBTree.h"

namespace wzz {
	template <typename K, typename Compare = Less<K>>
	class set {
	private:
		struct SetKeyOfV {
			const K& operator()(const K& key) {
				return key;
			}
		};
	public:
		typedef typename RBTree<K, K, SetKeyOfV, Compare>::const_iterator iterator;
		typedef typename RBTree<K, K, SetKeyOfV, Compare>::const_iterator const_iterator;

		iterator begin() const {
			return _t.begin();
		}

		iterator end() const {
			return _t.end();
		}

		pair<iterator, bool> insert(const K& key) {
			pair<typename RBTree<K, K, SetKeyOfV, Compare>::iterator, bool> ret = _t.Insert(key);
			return pair<iterator, bool>(ret.first, ret.second);
		}
	private:
		RBTree<K, K, SetKeyOfV, Compare> _t;
	};
}