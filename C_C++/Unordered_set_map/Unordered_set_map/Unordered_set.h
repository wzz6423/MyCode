#pragma once
#include "Hash.h"

namespace wzz {
	template <typename K>
	class Unordered_set {
		struct KeyOfV {
			const K& operator()(const K& key) {
				return key;
			}
		};

	public:
		typedef typename HashTable<K, K, KeyOfV>::const_iterator iterator;
		typedef typename HashTable<K, K, KeyOfV>::const_iterator const_iterator;
		
		const_iterator begin() const {
			return _ht.begin();
		}

		const_iterator end() const {
			return _ht.end();
		}

		pair<const_iterator, bool> insert(const K& key) {
			pair<typename HashTable<K, K, KeyOfV>::iterator, bool> ret = _ht.Insert(key);
			return { ret.first, ret.second };
		}
	private:
		HashTable<K, K, KeyOfV> _ht;
	};

	void Test_set() {
		Unordered_set<int> s;
		s.insert(1);
		s.insert(0);
		s.insert(3);
		s.insert(1);
		s.insert(2);
		s.insert(5);
		s.insert(1);
		s.insert(6);
	}
}