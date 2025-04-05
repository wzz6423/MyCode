#pragma once
#include "RBTree.h"

namespace wzz {
	template <typename K, typename V, typename Compare = Less<V>>
	class map {
		struct MapKeyOfV {
			const K& operator()(const pair<K, V>& kv) {
				return kv.first;
			}
		};
	public:
		typedef typename RBTree<K, pair<const K, V>, MapKeyOfV, Compare>::iterator iterator;
		typedef typename RBTree<K, pair<const K, V>, MapKeyOfV, Compare>::const_iterator const_iterator;

		iterator begin() {
			return _t.begin();
		}

		iterator end() {
			return _t.end();
		}

		const_iterator begin() const {
			return _t.begin();
		}

		const_iterator end() const {
			return _t.end();
		}

		V& operator[](const K& key) {
			pair<iterator, bool> ret = insert({ key, V() });
			return ret.first->second;
		}

		pair<iterator, bool> insert(const pair<K, V>& kv) {
			return _t.Insert(kv);
		}
	private:
		RBTree<K, pair<const K, V>, MapKeyOfV, Compare> _t;
	};
}