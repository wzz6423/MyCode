#pragma once
#include <iostream>
#include <assert.h>
using namespace std;

namespace wzz {
	template<typename K, typename V>
	struct AVLTreeNode{ 
		pair<K, V> _kv;
		AVLTreeNode<K, V>* _left;
		AVLTreeNode<K, V>* _right;
		AVLTreeNode<K, V>* _parent;
		int _bf; // balance factor

		AVLTreeNode(const pair<K, V>& kv)
			:_kv(kv)
			,_left(nullptr)
			,_right(nullptr)
			,_parent(nullptr)
			,_bf(0)
		{ }
	};

	template<typename K, typename V>
	class AVLTree {
	private:
		typedef AVLTreeNode<K, V> Node;

	public:
		// number
		AVLTree()
			:_root(nullptr)
		{ }

		AVLTree(const AVLTree<K, V>& t) {
			_root = Copy(t._root);
		}

		AVLTree<K, V>& operator=(AVLTree<K, V> t) {
			std::swap(_root, t._root);
			return *this;
		}

		~AVLTree() {
			Destroy(_root);
		}

		// modify
		bool Insert(const pair<K, V>& kv) {
			if (!_root) {
				_root = new Node(kv);
				return true;
			}

			Node* parent = nullptr;
			Node* cur = _root;
			while (cur) {
				if (cur->_kv.first < kv.first) {
					parent = cur;
					cur = cur->_right;
				}
				else if (cur->_kv.first > kv.first) {
					parent = cur;
					cur = cur->_left;
				}
				else {
					return false;
				}
			}

			cur = new Node(kv);
			if (parent->_kv.first < kv.first) {
				parent->_right = cur;
			}
			else if (parent->_kv.first > kv.first) {
				parent->_left = cur;
			}
			cur->_parent = parent;

			while (parent) {
				if (parent->_left == cur) {
					--parent->_bf;
				}
				else if (parent->_right == cur) {
					++parent->_bf;
				}
				else {
					throw "insert error -- 1\n";
				}

				if (parent->_bf == 0) {
					break;
				}
				else if (parent->_bf == 1 || parent->_bf == -1) {
					cur = parent;
					parent = parent->_parent;
				}
				else if(parent->_bf == 2 || parent->_bf == -2){
					if (parent->_bf == 2 && cur->_bf == 1) {
						Rotate_Left(parent);
					}
					else if (parent->_bf == -2 && cur->_bf == -1) {
						Rotate_Right(parent);
					}
					else if (parent->_bf == 2 && cur->_bf == -1) {
						Rotate_Right_Left(parent);
					}
					else if (parent->_bf == -2 && cur->_bf == 1) {
						Rotate_Left_Right(parent);
					}
					else {
						throw "insert error -- 2\n";
					}
					break;
				}
				else {
					throw "insert error -- 3\n";
				}
				return true;
			}
			return true;
		}

		void Rotate_Left(Node* parent) {
			Node* cur = parent->_right;
			Node* curleft = cur->_left;

			parent->_right = curleft;
			if (curleft) {
				curleft->_parent = parent;
			}
			cur->_left = parent;
			parent->_parent = cur;

			Node* ppnode = parent->_parent;
			if (parent == _root) {
				_root = cur;
				cur->_parent = nullptr;
			}
			else if (parent != _root) {
				if (ppnode->_left == parent) {
					ppnode->_left = cur;
				}
				else if (ppnode->_right == parent) {
					ppnode->_right = cur;
				}
				else {
					throw "Rotate_Left error -- 1\n";
				}
				cur->_parent = ppnode;
			}
			else {
				throw "Rotate_Left error -- 2\n";
			}

			parent->_bf = cur->_bf = 0;
		}

		void Rotate_Right(Node* parent) {
			Node* cur = parent->_left;
			Node* curright = cur->_right;

			parent->_left = curright;
			if (curright) {
				curright->_parent = parent;
			}
			cur->_right = parent;
			parent->_parent = cur;

			Node* ppnode = parent->_parent;
			if (parent == _root) {
				_root = cur;
				cur->_parent = nullptr;
			}
			else if (parent != _root) {
				if (ppnode->_left == parent) {
					ppnode->_left = cur;
				}
				else if (ppnode->_right == parent) {
					ppnode->_right = cur;
				}
				else {
					throw "Rotate_Right error -- 1\n";
				}
				cur->_parent = ppnode;
			}
			else {
				throw "Rotate_Right error -- 2\n";
			}
			
			parent->_bf = cur->_bf = 0;
		}

		void Rotate_Right_Left(Node* parent) {
			Node* cur = parent->_right;
			Node* curleft = cur->_left;
			int bf = curleft->_bf;

			Rotate_Right(parent->_right);
			Rotate_Left(parent);

			if (bf == 0) {
				cur->_bf = 0;
				curleft->_bf = 0;
				parent->_bf = 0;
			}
			else if (bf == 1) {
				cur->_bf = 0;
				curleft->_bf = 0;
				parent->_bf = -1;
			}
			else if (bf == -1) {
				cur->_bf = 1;
				curleft->_bf = 0;
				parent->_bf = 0;
			}
			else {
				throw "Rotate_Right_Left error -- 1\n";
			}
		}

		void Rotate_Left_Right(Node* parent) {
			Node* cur = parent->_left;
			Node* curright = cur->_right;
			int bf = curright->_bf;

			Rotate_Left(parent->_left);
			Rotate_Right(parent);

			if (bf == 0) {
				cur->_bf = 0;
				curright->_bf = 0;
				parent->_bf = 0;
			}
			else if (bf == 1) {
				cur->_bf = -1;
				curright->_bf = 0;
				parent->_bf = 0;
			}
			else if (bf == -1) {
				cur->_bf = 0;
				curright->_bf = 0;
				parent->_bf = 1;
			}
			else {
				throw "Rotate_Left_Right error -- 1\n";
			}
		}

		// Test(consider only the "true or false" and not the performance)
		bool isBalanceTree() {
			return isBalanceTree(_root);
		}

		int Height() {
			return Height(_root);
		}
		
		void InOrder() {
			_InOrder(_root);
			cout << endl;
		}
	private:
		bool isBalanceTree(Node* root) {
			if (!root) {
				return true;
			}

			int leftHeight = isBalanceTree(root->_left);
			int rightHeight = isBalanceTree(root->_right);

			if (rightHeight - leftHeight != root->_bf) {
				throw "bf error\n";
				return false;
			}

			return abs(rightHeight - leftHeight) < 2
				&& isBalanceTree(root->_left)
				&& isBalanceTree(root->_right);
		}

		int Height(Node* root) {
			if (!root) {
				return 0;
			}

			int leftHeight = isBalanceTree(root->_left);
			int rightHeight = isBalanceTree(root->_right);
			return leftHeight > rightHeight ? leftHeight + 1 : rightHeight + 1;
		}

		Node* Copy(Node* root) {
			if (!root) {
				return nullptr;
			}

			Node* copyroot = new Node(_root->_kv);
			copyroot->_left = Copy(_root->_left);
			copyroot->_right = Copy(_root->_right);
			return copyroot;
		}

		void Destroy(Node*& root) {
			if (!root) {
				return;
			}

			Destroy(root->_left);
			Destroy(root->_right);
			delete root;
			root = nullptr;
		}

		void _InOrder(Node* root) {
			if (!root) {
				return;
			}

			_InOrder(root->_left);
			cout << root->_kv.first << ":" << root->_kv.second << " ";
			_InOrder(root->_right);
		}
	private:
		Node* _root;
	};

	void Test_AVLTree() {
		int a[] = { 16, 3, 7, 11, 9, 26, 18, 14, 15 };
		AVLTree<int, int> t;
		for (auto e : a)
		{
			t.Insert(make_pair(e, e));
		}
		t.InOrder();
	}
}