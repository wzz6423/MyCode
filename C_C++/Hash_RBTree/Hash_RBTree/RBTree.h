#pragma once
#include <iostream>
using namespace std;

namespace wzz {
	enum color {
		RED,
		BLACK
	};

	template <typename K, typename V>
	struct RBTreeNode {
		RBTreeNode<K, V>* _left;
		RBTreeNode<K, V>* _right;
		RBTreeNode<K, V>* _parent;

		pair<K, V> _kv;
		color _col;

		RBTreeNode(const pair<K, V>& kv) 
			:_left(nullptr)
			,_right(nullptr)
			,_parent(nullptr)
			,_kv(kv)
			,_col(RED)
		{ }
	};

	template <typename K, typename V>
	class RBTree {
	private:
		typedef RBTreeNode<K, V> Node;
	public:
		// number
		RBTree()
			:_root(nullptr)
		{}

		RBTree(const RBTree<K, V>& t) {
			_root = Copy(t._root);
		}

		RBTree<K, V>& operator=(RBTree<K, V> t) {
			std::swap(_root, t._root);
			return *this;
		}

		~RBTree() {
			Destroy(_root);
		}
		// modify
		bool Insert(const pair<K, V>& kv) {
			if (!_root) {
				_root = new Node(kv);
				_root->_col = BLACK;
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

			while (parent && parent->_col == RED) {
				Node* grandparent = parent->_parent;

				if (parent == grandparent->_left) {
					Node* uncle = grandparent->_right;

					// uncle存在且为红 -- p、u改BLACK，g改RED
					if (uncle && uncle->_col == RED) {
						parent->_col = uncle->_col = BLACK;
						grandparent->_col = RED;

						cur = grandparent;
						parent = cur->_parent;
					}
					else { // uncle不存在或为黑 -- p改BLACK，g改RED
						if (cur == parent->_left) {
							//     g
							//   p
							// c
							Rotate_Right(grandparent);
							parent->_col = BLACK;
							grandparent->_col = RED;
						}
						else {
							//     g
							// p
							//   c
							Rotate_Left(parent);
							Rotate_Right(grandparent);

							cur->_col = BLACK;
							grandparent->_col = RED;
						}
						break;
					}
				}
				else { // parent == grandparen->_right
					Node* uncle = grandparent->_left;

					if (uncle && uncle->_col == RED) {
						parent->_col = uncle->_col = BLACK;
						grandparent->_col = RED;

						cur = grandparent;
						parent = cur->_parent;
					}
					else {
						if (cur == parent->_right) {
							// g
							//   p
							//     c
							Rotate_Left(grandparent);
							grandparent->_col = RED;
							parent->_col = BLACK;
						}
						else {
							// g
							//    p
							//  c
							Rotate_Right(parent);
							Rotate_Left(grandparent);

							cur->_col = BLACK;
							grandparent->_col = RED;
						}
						break;
					}
				}
			}
			_root->_col = BLACK;
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
		}

		bool CheckColor(Node* root, int blacknum, int benchmark) { // benchmark 基准
			if (!root) {
				if (blacknum != benchmark) {
					return false;
				}

				return true;
			}

			if (root->_col == BLACK) {
				++blacknum;
			}

			if (root->_col == RED && root->_parent && root->_parent->_col == RED) {
				throw "continuous red points\n";
				return false;
			}

			return CheckColor(root->_left, blacknum, benchmark)
				&& CheckColor(root->_right, blacknum, benchmark);
		}

		bool IsBalance() {
			return IsBalance(_root);
		}

		int Height() {
			return Height(_root);
		}

		void InOrder() {
			_InOrder(_root);
			cout << endl;
		}
	private:
		bool IsBalance(Node* root) {
			if (!root) {
				return true;
			}

			if (root->_col != BLACK) {
				return false;
			}

			int benckmark = 0;
			Node* cur = _root;
			while (cur) {
				if (cur->_col == BLACK) {
					++benckmark;
				}
				cur = cur->_left;
			}

			return CheckColor(root, 0, benckmark);
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
		Node* _root = nullptr;
	};

	void Test_RBTree() {
		int a[] = { 4, 2, 6, 1, 3, 5, 15, 7, 16, 14 };
		RBTree<int, int> t;
		for (auto e : a)	{
			t.Insert({e, e});
			cout << "Insert:" << e << "->" << t.IsBalance() << endl;
		}
	}
}