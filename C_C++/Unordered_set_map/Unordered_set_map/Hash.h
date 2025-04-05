#pragma once
#include <iostream>
#include <vector>
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

	template <typename V>
	struct HashNode {
		V _data;
		HashNode<V>* _next;

		HashNode(const V& data)
			:_data(data)
			,_next(nullptr)
		{ }
	};

	template <typename K, typename V, typename KeyOfV, typename HashFunc>
	class HashTable;

	template <typename K, typename V, typename Ptr, typename Ref, typename KeyOfV, typename HashFunc>
	struct Hiterator {
		typedef HashNode<V> Node;
		typedef Hiterator<K, V, Ptr, Ref, KeyOfV, HashFunc> self;
		typedef Hiterator<K, V, V*, V&, KeyOfV, HashFunc> iterator;

		Node* _node;
		const HashTable<K, V, KeyOfV, HashFunc>* _pht;

		Hiterator(Node* node, const HashTable<K, V, KeyOfV, HashFunc>* pht)
			:_node(node)
			,_pht(pht)
		{}

		Hiterator(const iterator& it)
			:_node(it._node)
			, _pht(it._pht)
		{}

		Ref operator*() {
			return _node->_data;
		}

		Ptr operator->() {
			return &_node->_data;
		}

		self& operator++() {
			if (_node->_next) {
				_node = _node->_next;
			}
			else {
				KeyOfV kov;
				HashFunc hf;
				size_t hashi = hf(kov(_node->_data)) % _pht->_table.size();
				++hashi;
				while (hashi < _pht->_table.size() && !_pht->_table[hashi]){
					++hashi;
				}
				if (hashi < _pht->_table.size()) {
					_node = _pht->_table[hashi];
				}
				else {
					_node = nullptr;
				}
			}
			return *this;
		}

		bool operator!=(const self& it) {
			return _node != it._node;
		}

		bool operator==(const self& it) {
			return _node == it._node;
		}
	};

	template <typename K, typename V, typename KeyOfV, typename HashFunc = DefaultHashFunc<K>>
	class HashTable {
	private:
		typedef HashNode<V> Node;

		template <typename K, typename V, typename Ptr, typename Ref, typename KeyOfV, typename HashFunc>
		friend struct Hiterator;
	public:
		typedef Hiterator<K, V, V*, V&, KeyOfV, HashFunc> iterator;
		typedef Hiterator<K, V, const V*, const V&, KeyOfV, HashFunc> const_iterator;

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

		iterator begin() {
			for (size_t i = 0; i < _table.size(); ++i) {
				if (!_table[i]) {
					return iterator(_table[i], this);
				}
			}
			return iterator(nullptr, this);
		}

		iterator end() {
			return iterator(nullptr, this);
		}

		const_iterator begin() const {
			for (size_t i = 0; i < _table.size(); ++i) {
				if (!_table[i]) {
					return const_iterator(_table[i], this);
				}
			}
			return const_iterator(nullptr, this);
		}

		const_iterator end() const {
			return const_iterator(nullptr, this);
		}

		pair<iterator, bool> Insert(const V& data) {
			KeyOfV kov;

			iterator it = Find(kov(data));
			if (it != end()) {
				return { it, false };
			}

			HashFunc hf;
			if (_n == _table.size()) {
				size_t newSize = GetNextPrime(_table.size());
				vector<Node*> newTable;
				newTable.resize(newSize, nullptr);

				for (size_t i = 0; i < _table.size(); ++i) {
					Node* cur = _table[i];
					while (cur) {
						Node* next = cur->_next;

						size_t hashi = hf(kov(cur->_data)) % newSize;
						cur->_next = newTable[hashi];
						newTable[hashi] = cur;

						cur = next;
					}
					_table[i] = nullptr;
				}
				_table.swap(newTable);
			}

			size_t hashi = hf(kov(data)) % _table.size();
			Node* newnode = new Node(data);
			newnode->_next = _table[hashi];
			_table[hashi] = newnode;
			++_n;
			return {iterator(newnode, this), this};
		}

		iterator Find(const K& key) {
			HashFunc hf;
			size_t hashi = hf(key) % _table.size();
			Node* cur = _table[hashi];
			KeyOfV kov;
			while (cur) {
				if (kov(cur->_data) == key) {
					return iterator(cur, this);
				}
				cur = cur->_next;
			}
			return iterator(nullptr, this);
		}

		bool Erase(const K& key) {
			HashFunc hf;
			size_t hashi = hf(key) % _table.size();
			Node* prev = nullptr;
			Node* cur = _table[hashi];
			while (cur) {
				if (kov(cur->_data) == key) {
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
	private:
		vector<Node*> _table; // 哈希桶
		size_t _n = 0; // 有效数据
	};
}