#pragma once
#include "Hash.h"
#include <string>

namespace wzz {
	template <typename K, typename V>
	class Unordered_map {
		struct KeyOfV {
			const K& operator()(const pair<const K, V>& kv) {
				return kv.first;
			}
		};
	public:
		typedef typename HashTable<K, pair<const K, V>, KeyOfV>::iterator iterator;
		typedef typename HashTable<K, pair<const K, V>, KeyOfV>::const_iterator const_iterator;

		iterator begin() {
			return _ht.begin();
		}

		iterator end() {
			return _ht.end();
		}

		const_iterator begin() const {
			return _ht.begin();
		}

		const_iterator end() const {
			return _ht.end();
		}

		pair<iterator, bool> insert(const pair<K, V>& kv) {
			return _ht.Insert(kv);
		}

		V& operator[](const K& key) {
			pair<iterator, bool> ret = _ht.Insert({ key, V() });
			return ret.first->second;
		}
	private:
		HashTable<K, pair<const K, V>, KeyOfV> _ht;
	};

	void Test_map() {
		Unordered_map<string, string> m;
		m.insert({ "sort", "ÅÅĞò" });
		m["left"] = "×ó±ß";
		m.insert({ "right", "ÓÒ±ß" });
		m["test"] = "²âÊÔ";
	}
}