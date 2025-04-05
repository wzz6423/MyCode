#pragma once
#include <iostream>
using namespace std;

namespace wzz {
	enum color {
		RED,
		BLACK
	};

	template <typename T>
	struct Less {
		bool operator()(const T x, const T y) {
			return x < y;
		}
	};

	template <typename V>
	struct RBTreeNode {
		RBTreeNode<V>* _left;
		RBTreeNode<V>* _right;
		RBTreeNode<V>* _parent;

		V _data;
		color _col;

		RBTreeNode(const V& data) 
			:_left(nullptr)
			,_right(nullptr)
			,_parent(nullptr)
			,_data(data)
			,_col(RED)
		{ }
	};

	template <typename V, typename Ref, typename Ptr>
	struct __TreeIterator {
		typedef RBTreeNode<V> Node;
		typedef __TreeIterator<V, Ref, Ptr> self;
		typedef __TreeIterator<V, V&, V*> iterator;

		Node* _node;

		__TreeIterator(Node* node)
			:_node(node)
		{ }

		__TreeIterator(const iterator& it)
			:_node(it._node)
		{}

		Ref operator*() {
			return _node->_data;
		}

		Ptr operator->() {
			return &_node->_data;
		}

		bool operator!=(const self& it) {
			return _node != it._node;
		}

		bool operator==(const self& it) {
			return _node == it._node;
		}

		self& operator--() {
			if (_node->_left) {
				Node* subright = _node->_left;
				while (subright->_right) {
					subright = subright->_right;
				}
				_node = subright;
			}
			else {
				Node* cur = _node;
				Node* parent = _node->_parent;

				while (parent && cur == parent->_left) {
					cur = parent;
					parent = cur->_parent;
				}
				_node = parent;
			}
			return *this;
		}

		self operator--(int) {
			self tmp(_node);
			if (_node->_left) {
				Node* subright = _node->_left;
				while (subright->_right) {
					subright = subright->_right;
				}
				_node = subright;
			}
			else {
				Node* cur = _node;
				Node* parent = cur->_parent;

				while (parent && cur == parent->_left) {
					cur = parent;
					parent = cur->_parent;
				}
				_node = parent;
			}
			return tmp;
		}

		self& operator++() {
			if (_node->_right) {
				Node* subleft = _node->_right;
				while (subleft->_left) {
					subleft = subleft->_left;
				}
				_node = subleft;
			}
			else {
				Node* cur = _node;
				Node* parent = cur->_parent;
				while (parent && cur == parent->_right) {
					cur = parent;
					parent = cur->_parent;
				}
				_node = parent;
			}
			return *this;
		}

		self operator++(int) {
			self tmp(_node);
			if (_node->_right) {
				Node* subleft = _node->_right;
				while (subleft->_left) {
					subleft = subleft->_left;
				}
				_node = subleft;
			}
			else {
				Node* cur = _node;
				Node* parent = cur->_parent;
				while (parent && cur == parent->_right) {
					cur = parent;
					parent = cur->_parent;
				}
				_node = parent;
			}
			return tmp;
		}
	};

	template <typename K, typename V, typename KeyOfV, typename Compare>
	class RBTree {
	private:
		typedef RBTreeNode<V> Node;
	public:
		typedef __TreeIterator<V, V&, V*> iterator;
		typedef __TreeIterator<V, const V&, const V*> const_iterator;
		// number
		RBTree()
			:_root(nullptr)
		{}

		RBTree(const RBTree<K, V, KeyOfV, Compare>& t) {
			_root = Copy(t._root);
		}

		RBTree<K, V, KeyOfV, Compare>& operator=(RBTree<K, V, KeyOfV, Compare> t) {
			std::swap(_root, t._root);
			return *this;
		}

		~RBTree() {
			Destroy(_root);
		}
		// iterator
		iterator begin() {
			Node* leftMin = _root;
			while (leftMin && leftMin->_left) {
				leftMin = leftMin->_left;
			}
			return iterator(leftMin);
		}

		iterator end() {
			return iterator(nullptr);
		}

		const_iterator begin() const {
			Node* leftMin = _root;
			while (leftMin && leftMin->_left) {
				leftMin = leftMin->_left;
			}
			return const_iterator(leftMin);
		}

		const_iterator end() const {
			return const_iterator(nullptr);
		}

		// modify(
		iterator* Find(const K& key) {
			Node* cur = _root;
			KeyOfV kov;

			while (cur) {
				if (com(kov(cur->_data), key)) {
					cur = cur->_right;
				}
				else if (com(key, kov(cur->_data))) {
					cur = cur->_left;
				}
				else {
					return iterator(cur);
				}
			}
			return iterator(nullptr);
		}

		pair<iterator, bool> Insert(const V& data) {
			if (!_root) {
				_root = new Node(data);
				_root->_col = BLACK;
				return {iterator(_root), true};
			}

			KeyOfV kov;
			Compare com;

			Node* parent = nullptr;
			Node* cur = _root;
			while (cur) {
				if (com(kov(cur->_data), kov(data))) {
					parent = cur;
					cur = cur->_right;
				}
				else if (com(kov(data), kov(cur->_data))) {
					parent = cur;
					cur = cur->_left;
				}
				else {
					return { iterator(nullptr), false };
				}
			}

			cur = new Node(data);
			Node* newnode = cur;
			if (com(kov(parent->_data), kov(data))) {
				parent->_right = cur;
			}
			else if (!com(kov(parent->_data), kov(data))) {
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
				else { // parent == grandparent->_right
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
			return { iterator(newnode), true };
		}
		
		bool IsBalance() {
			return IsBalance(_root);
		}

		int Height() {
			return Height(_root);
		}

	private:
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

			Node* copyroot = new Node(_root->_data);
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

	private:
		Node* _root = nullptr;
	};
}