#pragma once
#include <iostream>
#include <string>
using namespace std;

namespace wzz {
	namespace key {
		template <typename K>
		struct BSTreeNode {
			BSTreeNode<K>* _left;
			BSTreeNode<K>* _right;
			K _key;

			BSTreeNode(const K& key)
				:_left(nullptr)
				, _right(nullptr)
				, _key(key)
			{
			}
		};

		template <typename K>
		class BSTree {
		private:
			typedef BSTreeNode<K> Node;

		public:
			// number
			BSTree()
				:_root(nullptr)
			{}

			BSTree(const BSTree<K>& t) {
				_root = Copy(t._root);
			}

			BSTree<K>& operator=(BSTree<K> t) {
				std::swap(_root, t._root);
				return *this;
			}

			~BSTree() {
				Destroy(_root);
			}

			// modify
			bool Insert_(const K& key) { // 迭代
				if (!_root) {
					_root = new Node(key);
					return true;
				}

				Node* parent = nullptr;
				Node* cur = _root;
				while (cur) {
					if (cur->_key < key) {
						parent = cur;
						cur = cur->_right;
					}
					else if (cur->_key > key) {
						parent = cur;
						cur = cur->_left;
					}
					else {
						return false;
					}
				}

				cur = new Node(key);
				if (parent->_key < key) {
					parent->_right = cur;
				}
				else if (parent->_key > key) {
					parent->_left = cur;
				}

				return true;
			}

			bool Erase_(const K& key) { // 迭代
				Node* parent = nullptr;
				Node* cur = _root;

				while (cur) {
					if (cur->_key < key) {
						parent = cur;
						cur = cur->_right;
					}
					else if (cur->_key > key) {
						parent = cur;
						cur = cur->_left;
					}
					else {
						if (!cur->_left) {
							if (cur == _root) {
								_root = _root->_right;
							}
							else {
								if (parent->_left == cur) {
									parent->_left = cur->_right;
								}
								else if (parent->_right == cur) {
									parent->_right = cur->_right;
								}
							}
						}
						else if (!cur->_right) {
							if (cur == _root) {
								_root = _root->_left;
							}
							else {
								if (parent->_left == cur) {
									parent->_left = cur->_left;
								}
								else if (parent->_right = cur) {
									parent->_right = cur->_left;
								}
							}
						}
						else {
							parent = cur;
							Node* leftMax = cur->_left;

							while (leftMax->_right) {
								parent = leftMax;
								leftMax = leftMax->_right;
							}

							std::swap(cur->_key, leftMax->_key);

							if (parent->_left == leftMax) {
								parent->_left = leftMax->_left;
							}
							else if (parent->_right == leftMax) {
								parent->_right = leftMax->_left;
							}

							cur = leftMax;
						}
						delete cur;
						return true;
					}
				}
				return false;
			}

			bool Find_(const K& key) { // 迭代
				Node* cur = _root;

				while (cur) {
					if (cur->_key < key) {
						cur = cur->_right;
					}
					else if (cur->_key > key) {
						cur = cur->_left;
					}
					else if (cur->_key == key) {
						return true;
					}
					else {
						return false;
					}
				}
				return false;
			}

			bool Insert(const K& key) { // 递归 - 外部调用
				return _Insert(_root, key);
			}

			bool Find(const K& key) { // 递归 - 外部调用
				return _Find(_root, key);
			}

			void InOrder() { // 递归 - 外部调用
				_InOrder(_root);
				cout << endl;
			}

			bool Erase(const K& key) { // 递归 - 外部调用
				return _Erase(_root, key);
			}

		protected:
			// number
			Node* Copy(Node* root) { // 递归 - 内部调用
				if (!root) {
					return nullptr;
				}

				Node* copyroot = new Node(_root->_key);
				copyroot->_left = Copy(_root->_left);
				copyroot->_right = Copy(_root->_right);
				return copyroot;
			}

			void Destroy(Node*& root) { // 递归 - 内部调用
				if (!root) {
					return;
				}

				Destroy(root->_left);
				Destroy(root->_right);
				delete root;
				root = nullptr;
			}

			// modify
			bool _Insert(Node*& root, const K& key) { // 递归 - 内部调用
				if (!root) {
					root = new Node(key);
					return true;
				}

				if (root->_key < key) {
					_Insert(root->_right, key);
				}
				else if (root->_key > key) {
					_Insert(root->_left, key);
				}
				else {
					return false;
				}
			}

			bool _Find(Node* root, const K& key) { // 递归 - 内部调用
				if (!root) {
					return false;
				}

				if (root->_key < key) {
					_Find(root->_right, key);
				}
				else if (root->_key > key) {

					_Find(root->_left, key);
				}
				else {
					return false;
				}
			}

			void _InOrder(Node* root) { // 递归 - 内部调用
				if (!root) {
					return;
				}

				_InOrder(root->_left);
				cout << root->_key << " ";
				_InOrder(root->_right);
			}

			bool _Erase(Node*& root, const K& key) { // 递归 - 内部调用
				if (!root) {
					return false;
				}

				if (root->_key < key) {
					return _Erase(root->_right, key);
				}
				else if (root->_key > key) {
					return _Erase(root->_left, key);
				}
				else {
					Node* del = root;

					if (!del->_left) {
						root = root->_right;
					}
					else if (!del->_right) {
						root = root->_left;
					}
					else {
						Node* leftMax = del->_left;
						while (leftMax->_right) {
							leftMax = leftMax->_right;
						}

						std::swap(del->_key, leftMax->_key);

						return _Erase(root->_left, key);
					}

					delete del;
					del = nullptr;
					return true;
				}
			}

		private:
			Node* _root;
		};
	}

	namespace key_value {
		template <typename K, typename V>
		struct BSTreeNode {
			BSTreeNode<K, V>* _left;
			BSTreeNode<K, V>* _right;
			K _key;
			V _value;

			BSTreeNode(const K& key, const V& value)
				:_left(nullptr)
				, _right(nullptr)
				, _key(key)
				,_value(value)
			{}
		};

		template <typename K, typename V>
		class BSTree {
		private:
			typedef BSTreeNode<K, V> Node;

		public:
			// number
			BSTree()
				:_root(nullptr)
			{}

			BSTree(const BSTree<K, V>& t) {
				_root = Copy(t._root);
			}

			BSTree<K, V>& operator=(BSTree<K, V> t) {
				std::swap(_root, t._root);
				return *this;
			}

			~BSTree() {
				Destroy(_root);
			}

			// modify
			bool Insert_(const K& key, const V& value) { // 迭代
				if (!_root) {
					_root = new Node(key, value);
					return true;
				}

				Node* parent = nullptr;
				Node* cur = _root;
				while (cur) {
					if (cur->_key < key) {
						parent = cur;
						cur = cur->_right;
					}
					else if (cur->_key > key) {
						parent = cur;
						cur = cur->_left;
					}
					else {
						return false;
					}
				}

				cur = new Node(key, value);
				if (parent->_key < key) {
					parent->_right = cur;
				}
				else if (parent->_key > key) {
					parent->_left = cur;
				}

				return true;
			}

			bool Erase_(const K& key) { // 迭代
				Node* parent = nullptr;
				Node* cur = _root;

				while (cur) {
					if (cur->_key < key) {
						parent = cur;
						cur = cur->_right;
					}
					else if (cur->_key > key) {
						parent = cur;
						cur = cur->_left;
					}
					else {
						if (!cur->_left) {
							if (cur == _root) {
								_root = _root->_right;
							}
							else {
								if (parent->_left == cur) {
									parent->_left = cur->_right;
								}
								else if (parent->_rihgt == cur) {
									parent->_right = cur->_right;
								}
							}
						}
						else if (!cur->_right) {
							if (cur == _root) {
								_root = _root->_left;
							}
							else {
								if (parent->_left == cur) {
									parent->_left = cur->_left;
								}
								else if (parent->_right = cur) {
									parent->_right = cur->_left;
								}
							}
						}
						else {
							parent = cur;
							Node* leftMax = cur->_left;

							while (leftMax->_right) {
								parent = leftMax;
								leftMax = leftMax->_right;
							}

							std::swap(cur->_key, leftMax->_key);

							if (parent->_left == leftMax) {
								parent->_left = leftMax->_left;
							}
							else if (parent->_right == leftMax) {
								parent->_right = leftMax->_left;
							}

							cur = leftMax;
						}
						delete cur;
						return true;
					}
				}
				return false;
			}

			Node* Find_(const K& key) { // 迭代
				Node* parent = nullptr;
				Node* cur = _root;

				while (cur) {
					if (cur->_key < key) {
						parent = cur;
						cur = cur->_right;
					}
					else if (cur->_key > key) {
						parent = cur;
						cur = cur->_left;
					}
					else if (cur->_key == key) {
						if (parent->_left == cur) {
							return parent->_left;
						}
						else if (parent->_right == cur) {
							return parent->_rihgt;
						}
						else {
							return nullptr;
						}
					}
					else {
						return nullptr;
					}
				}
				return nullptr;
			}

			bool Insert(const K& key, const V& value) { // 递归 - 外部调用
				return _Insert(_root, key, value);
			}

			Node* Find(const K& key) { // 递归 - 外部调用
				return _Find(_root, key);
			}

			void InOrder() { // 递归 - 外部调用
				_InOrder(_root);
				cout << endl;
			}

			bool Erase(const K& key) { // 递归 - 外部调用
				return _Erase(_root, key);
			}

		protected:
			// number
			Node* Copy(Node* root) { // 递归 - 内部调用
				if (!root) {
					return nullptr;
				}

				Node* copyroot = new Node(_root->_key);
				copyroot->_left = Copy(_root->_left);
				copyroot->_right = Copy(_root->_right);
				return copyroot;
			}

			void Destroy(Node*& root) { // 递归 - 内部调用
				if (!root) {
					return;
				}

				Destroy(root->_left);
				Destroy(root->_right);
				delete root;
				root = nullptr;
			}

			// modify
			bool _Insert(Node*& root, const K& key, const V& value) { // 递归 - 内部调用
				if (!root) {
					root = new Node(key, value);
					return true;
				}

				if (root->_key < key) {
					_Insert(root->_right, key, value);
				}
				else if (root->_key > key) {
					_Insert(root->_left, key, value);
				}
				else {
					return false;
				}
			}

			Node* _Find(Node* root, const K& key) { // 递归 - 内部调用
				if (!root) {
					return nullptr;
				}

				if (root->_key < key) {
					_Find(root->_right, key);
				}
				else if (root->_key > key) {

					_Find(root->_left, key);
				}
				else {
					return root;
				}
			}

			void _InOrder(Node* root) { // 递归 - 内部调用
				if (!root) {
					return;
				}

				_InOrder(root->_left);
				cout << root->_key << ":" << root->_value << " ";
				_InOrder(root->_right);
			}

			bool _Erase(Node*& root, const K& key) { // 递归 - 内部调用
				if (!root) {
					return false;
				}

				if (root->_key < key) {
					return _Erase(root->_right, key);
				}
				else if (root->_key > key) {
					return _Erase(root->_left, key);
				}
				else {
					Node* del = root;

					if (!del->_left) {
						root = root->_right;
					}
					else if (!del->_right) {
						root = root->_left;
					}
					else {
						Node* leftMax = del->_left;
						while (leftMax->_right) {
							leftMax = leftMax->_right;
						}

						std::swap(del->_key, leftMax->_key);

						return _Erase(root->_left, key);
					}

					delete del;
					del = nullptr;
					return true;
				}
			}

		private:
			Node* _root;
		};
	}

	void test_BSTree() {
		//int a[] = { 8,3,1,10,6,4,7,14,13 };
		//key::BSTree<int> t;
		//t.Insert(0);
		//t.Insert(2);
		//t.Insert(2);
		//t.Insert_(1);
		//t.Insert_(4);
		//t.Insert(1);
		//t.Insert_(5);
		//for (auto& e : a) {
		//	t.Insert_(e);
		//}
		//t.InOrder();
		//for (auto& e : a) {
		//	t.Erase_(e);
		//}
		//t.InOrder();

		//key_value::BSTree<string, string> dict;
		//dict.Insert("insert", "插入");
		//dict.Insert("sort", "排序");
		//dict.Insert("right", "右边");
		//dict.Insert("left", "左边");
		//dict.InOrder();

		//string arr[] = { "西瓜", "西瓜", "苹果", "西瓜", "苹果", "苹果", "西瓜", "苹果", "香蕉", "苹果", "香蕉" };
		//key_value::BSTree<string, int> countTree;
		//for (auto& str : arr)
		//{
		//	auto ret = countTree.Find(str);
		//	if (ret == nullptr){
		//		countTree.Insert(str, 1);
		//	}
		//	else{
		//		ret->_value++;
		//	}
		//}
		//countTree.InOrder();
	}
}