#pragma once
#include <iostream>
#include <vector>
using namespace std;

namespace thu {
	template <typename K>
	struct DefaultHashFunc {
		size_t operator()(const K& key) {
			return static_cast<size_t>(key);
		}
	};
	template <>
	struct DefaultHashFunc<string> {
		size_t operator()(const string& str) {
			size_t hash = 0;
			for (auto ch : str) {
				hash *= 131;
				hash += ch;
			}
			return hash;
		}
	};

	enum STATE {
		EXIST,
		EMPTY,
		DELETE
	};

	template <typename K, typename V>
	struct HashData {
		pair<K, V> _kv;
		STATE _state = EMPTY;
	};

	template <typename K, typename V, typename HashFunc = DefaultHashFunc<K>>
	class open_address {
	public:
		open_address() {
			_table.resize(10);
		}

		bool Insert(const pair<K, V>& kv) {
			if (Find(kv.first)) {
				return false;
			}

			if (_n * 10 / _table.size() >= 7) {
				size_t newSize = _table.size() * 2;
				open_address<K, V, HashFunc> newHT;
				newHT._table.resize(newSize);
				for (size_t i = 0; i < _table.size(); ++i) {
					if (_table[i]._state == EXIST) {
						newHT.Insert(_table[i]._kv);
					}
				}
				_table.swap(newHT._table);
			}

			HashFunc hf;
			size_t hashi = hf(kv.first) % _table.size();
			while (_table[hashi]._state == EXIST) {
				++hashi;
				hashi %= _table.size();
			}
			_table[hashi]._kv = kv;
			_table[hashi]._state = EXIST;
			++_n;

			return true;
		}

		HashData<const K, V>* Find(const K& key) {
			HashFunc hf;
			size_t hashi = hf(key) % _table.size();
			while (_table[hashi]._state != EMPTY) {
				if (_table[hashi]._state == EXIST && _table[hashi]._kv.first == key) {
					return (HashData<const K, V>*) & _table[hashi];
				}

				++hashi;
				hashi %= _table.size();
			}
			return nullptr;
		}

		bool Erase(const K& key) {
			HashData<const K, V>* ret = Find(key);
			if (ret) {
				ret->_state = DELETE;
				--_n;
				return true;
			}
			return false;
		}
	private:
		vector<HashData<K, V>> _table;
		size_t _n = 0;
	};

	void Test() {
		open_address<int, int> h;
		h.Insert({ 1, 1 });
		h.Insert({ 2, 2 });
		h.Insert({ 6, 6 });
		h.Insert({ 4, 4 });
		h.Insert({ 5, 5 });
		h.Insert({ 3, 3 });

		h.Erase(2);
		h.Erase(6);
	}
}