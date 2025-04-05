#pragma once
#include <iostream>
using namespace std;

namespace wzz {
	// 素数表
	size_t GetNextPrime(size_t prime)
	{
		static const int __stl_num_primes = 28;
		static const unsigned long __stl_prime_list[__stl_num_primes] =
		{
		  53,         97,         193,       389,       769,
		  1543,       3079,       6151,      12289,     24593,
		  49157,      98317,      196613,    393241,    786433,
		  1572869,    3145739,    6291469,   12582917,  25165843,
		  50331653,   100663319,  201326611, 402653189, 805306457,
		  1610612741, 3221225473, 4294967291
		};

		size_t i = 0;
		for (; i < __stl_num_primes; ++i)
		{
			if (__stl_prime_list[i] > prime)
				return __stl_prime_list[i];
		}

		return __stl_prime_list[i];
	}

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

	template <typename K, typename V>
	struct HashNode {
		pair<K, V> _kv;
		HashNode<K, V>* _next;

		HashNode(const pair<K, V>& kv)
			:_kv(kv)
			,_next(nullptr)
		{ }
	};

	template <typename K, typename V, typename HashFunc = DefaultHashFunc<K>>
	class HashTable {
	private:
		typedef HashNode<K, V> Node;
	public:
		HashTable() {
			_table.resize(10, nullptr);
		}

		~HashTable() {
			for (size_t i = 0; i < _table.size(); ++i) {
				Node* cur = _table[i];
				while (cur) {
					Node* next = cur->_next;
					delete cur;
					cur = next;
				}
				_table[i] = nullptr;
			}
		}

		bool Insert(const pair<K, V>& kv) {
			if (Find(kv.first)) {
				return false;
			}

			HashFunc hf;
			if (_n == _table.size()) {
				size_t newSize = _table.size() * 2;
				vector<Node*> newTable;
				newTable.resize(newSize, nullptr);

				for(size_t i = 0; i < _table.size(); ++i) {
					Node* cur = _table[i];
					while (cur) {
						Node* next = cur->_next;

						size_t hashi = hf(cur->_kv.first) % newSize;
						cur->_next = newTable[hashi];
						newTable[hashi] = cur;

						cur = next;
					}
					_table[i] = nullptr;
				}
				_table.swap(newTable);
			}

			size_t hashi = hf(kv.first) % _table.size();
			Node* newnode = new Node(kv);
			newnode->_next = _table[hashi];
			_table[hashi] = newnode;
			++_n;
			return true;
		}

		Node* Find(const K& key) {
			HashFunc hf;
			size_t hashi = hf(key) % _table.size();
			Node* cur = _table[hashi];
			while (cur) {
				if (cur->_kv.first == key) {
					return cur;
				}
				cur = cur->_next;
			}
			return nullptr;
		}

		bool Erase(const K& key) {
			HashFunc hf;
			size_t hashi = hf(key) % _table.size();
			Node* prev = nullptr;
			Node* cur = _table[hashi];
			while (cur) {
				if (cur->_kv.first == key) {
					if (prev) {
						prev->_next = cur->_next;
					}
					else {
						_table[hashi] = cur->_next;
					}
					delete cur;
					return true;
				}
				prev = cur;
				cur = cur->_next;
			}
			return false;
		}

		void Print() {
			for (size_t i = 0; i < _table.size(); ++i) {
				printf("[%d]->", i);
				Node* cur = _table[i];
				while (cur) {
					cout << cur->_kv.first << ":" << cur->_kv.second << "->";
					cur = cur->_next;
				}
				cout << "NULL" << endl;
			}
			cout << endl;
		}
	private:
		vector<Node*> _table; // 哈希桶
		size_t _n = 0; // 有效数据
	};

	void Test() {
		HashTable<int, int> h;
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