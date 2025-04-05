#pragma once
#pragma once
#include <iostream>
using namespace std;

// �� RBTree ȡ�� forward_list �Ż� Hash (������Ͱ >= 8 ʱ��Ͱ�ṹ�� forward_list ��Ϊ RBTree )
// �ô�������Ͱ����������ĸ������ӽ�Сʱ����Ͱ�Ĳ���Ч�����ɺܸ�
// Q1��������� RBTree & forward_list 
// --- A1.1���ٿ�һ�� vector ��¼ÿ��λ���� RBTree �� forward_list 
// Q1.1���ռ�ռ��̫���е㲻ֵ��
// --- A1.1����λͼ
// --- A1.2��ÿ��Ͱ��һ�� flag ���б���� RBTree �� forward_list (������ vector<pair<Node*, bool>> )
// Q2���Ƿ����������������ӣ� 
// -- A2������Ϊ����
// Q3�����������λͼ��� or ÿ��Ͱ��һ�� flag ���λ����ô�������ݵ�ʱ��
// ����Ǻ�����Ľڵ�û�취ֱ���ƶ����� vector ��ֻ��������ȥ���¹���(�ڵ�ṹ��ͬ)
// --- A3������ֱ�Ӹ��Ľڵ�ṹ���ں� RBTreeNode �� HashNode һ���ڵ�
// ����������
// bollֵ�ֱ��� RBTree �� forward_list 
// Node* left & Node* right ���Ϊ RBTree �����������Ϊ forward_list ��rightʼ��Ϊnullptr��ֻ��left��������
namespace wzz {
	// ������
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
			, _next(nullptr)
		{
		}
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
				size_t newSize = GetNextPrime(_table.size());
				vector<Node*> newTable;
				newTable.resize(newSize, nullptr);

				for (size_t i = 0; i < _table.size(); ++i) {
					Node* cur = _table[i];
					while (cur) {
						Node* next = cur->_next;

						size_t hashi = hf(cur->_kv.first) % newSize;
						cur->_next = newTable[hashi];
						newTable[hashi] = cur;

						cur = next;
					}
					_table[hashi] = nullptr;
				}
				_table.swap(newTable_table);
			}

			size_t hashi = hf(kv.first) % _table.size();

			Node* cur = _table[hashi];
			size_t count = 0;
			while (cur) {
				++count;
				cur = cur->_next;
			}

			Node* newnode = new Node(kv);
			newnode->_next = _table[hahsi];
			_table[hashi] = newnode;
			++_n;

			if (count >= 8) {
				RBTree<K, V> newtree;
				Node* cur = _table[hashi];
				Node* next = nullptr;
				for (size_t i = 0; i <= count; ++i) {
					next = cur->_next;
					newtree.Insert(cur->_kv);
					delete cur;
					cur = next;
				}
				_table[hashi] = newtree;
			}

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
			Node* cur = _table[hahsi];
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
		vector<Node*> _table; // ��ϣͰ
		size_t _n = 0; // ��Ч����
	};
}