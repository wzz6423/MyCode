#pragma once
#include <iostream>
#include <algorithm>
#include <assert.h>
using namespace std;

namespace wzz {
	template<typename T>
	struct list_node {
		typedef list_node<T> Node;

		Node* _next;
		Node* _prev;
		T _val;

		//list_node(const T& val = T())
		//	:_next(nullptr)
		//	, _prev(nullptr)
		//	, _val(val)
		//{}

		//list_node(T&& val)
		//	:_next(nullptr)
		//	, _prev(nullptr)
		//{
		//	swap(_val, val);
		//}

		// 完美转发 -- 直接替代上面两个版本(模板中&&不是右值引用、是可以接受任何类型引用)
		template <typename Ty>
		list_node(Ty&& val)
			:_next(nullptr)
			, _prev(nullptr)
			,_val(forward<Ty>(val))
		{ }
	};

	template <typename T, typename Ref, typename Ptr>
	struct __list_iterator {
		typedef list_node<T> Node;
		typedef __list_iterator<T, Ref, Ptr> self;

		Node* _node;

		__list_iterator(Node* node)
			:_node(node)
		{}

		Ref operator*() {
			return _node->_val;
		}

		Ptr operator->() {
			return &_node->_val;
		}

		self& operator++() {
			_node = _node->_next;
			return *this;
		}

		self operator++(int) {
			self tmp(_node);
			_node = _node->_next;
			return tmp;
		}

		self& operator--() {
			_node = _node->_prev;
			return *this;
		}

		self operator--(int) {
			self tmp(_node);
			_node = _node->_prev;
			return tmp;
		}

		bool operator!=(const self& it) const {
			return _node != it._node;
		}
		
		bool operator==(const self& it) const {
			return _node == it._node;
		}
	};

	template <typename T>
	class list {
	private:
		typedef list_node<T> Node;
		
	public:
		typedef __list_iterator<T, T&, T*> iterator;
		typedef __list_iterator<T, const T&, const T*> const_iterator;

	public:
		//number
		void empty_init() {
			_head = new Node(T());
			_head->_next = _head;
			_head->_prev = _head;
		}

		list() 
			:_head(nullptr)
			,_size(NULL)
		{
			empty_init();
		}

		list(int n, const T& value = T()) 
			:_head(nullptr)
			,_size(NULL)
		{
			empty_init();

			for (int i = 0; i < n; ++i) {
				push_back(value);
			}
		}

		template<typename iterator>
		list(iterator first, iterator last) 
			:_head(nullptr)
			,_size(NULL)
		{
			empty_init();

			while (first != last) {
				push_back(*first);
				++first;
			}
		}

		list(const list<T>& lt) 
			:_head(nullptr)
			, _size(NULL) 
		{
			empty_init();

			const_iterator it = lt.begin();
			while (it != lt.end()) {
				push_back(*it);
				++it;
			}
		}

		list(list<T>&& lt) 
			:_head(nullptr)
			, _size(NULL) 
		{
			empty_init();

			swap(lt);
		}

		list<T>& operator=(const list<T>& lt) {
			list<T> tmp_lt(lt);
			swap(tmp_lt);

			return *this;
		}

		list<T>& operator=(list<T>&& lt) {
			swap(lt);

			return *this;
		}

		~list() {
			clear();

			_head = nullptr;
			_size = NULL;
		}

		//iterator
		iterator begin() {
			return iterator(_head->_next);
		}

		iterator end() {
			return iterator(_head);
		}

		const_iterator begin() const {
			return const_iterator(_head->_next);
		}

		const_iterator end() const {
			return const_iterator(_head);
		}

		//capacity
		size_t size() const {
			return _size;
		}

		bool empty() {
			return _size == NULL;
		}

		//access
		T& front() {
			return *(_head->_next);
		}

		const T& front() const {
			return *(_head->_next);
		}

		T& back() {
			return *_head;
		}

		const T& back() const {
			return *_head;
		}

		//modify
		void push_back(const T& val) {
			insert(end(), val);
		}

		void push_back(T&& val) {
			insert(end(), forward<T>(val));
		}

		template <typename... Args>
		void emplace_back(Args&&... args) {
			int a[] = { _emplace_back(args)... };
		}

		void pop_back(const T& val) {
			insert(begin(), val);
		}

		void push_front() {
			erase(_head->_prev);
		}

		void pop_front() {
			erase(_head->_next);
		}

		iterator insert(iterator pos, const T& val) {
			Node* cur = pos._node;
			Node* newnode = new Node(val);
			cur->_prev->_next = newnode;
			newnode->_next = cur;
			newnode->_prev = cur->_prev;
			cur->_prev = newnode;

			++_size;

			return newnode;
		}

		iterator insert(iterator pos, T&& val) {
			Node* cur = pos._node;
			Node* newnode = new Node(forward<T>(val));
			cur->_prev->_next = newnode;
			newnode->_next = cur;
			newnode->_prev = cur->_prev;
			cur->_prev = newnode;

			++_size;

			return newnode;
		}

		iterator erase(iterator pos) {
			if (_head->_next == _head) {
				return nullptr;
			}

			Node* del = pos._node;
			Node* ret = del->_next;
			del->_next->_prev = del->_prev;
			del->_prev->_next = del->_next;
			delete del;

			--_size;

			return ret;
		}

		void clear() {
			size_t sz = _size;

			for (int i = 0; i < sz; ++i) {
				pop_front();
			}
		}

		void swap(list<T>& lt) {
			std::swap(_head, lt._head);
			std::swap(_size, lt._size);
		}
	private:
		int _emplace_back(T& val) {
			insert(end(), val);
			return 0;
		}

	private:
		Node* _head;
		size_t _size;
	};

	template <typename T>
	void Print(const list<T>& lt) {
		for (auto& e : lt) {
			cout << e << " ";
		}
		cout << endl;
	}

	template <typename T>
	list<T> func(list<T> lt) {
             		return lt;
	}

	void list_Test() {
		list<int> lt;
		lt.push_back(0);
		lt.push_back(1);
		lt.push_back(2);
		lt.push_back(3);
		lt.push_back(4);
		lt.push_back(5);

		Print(lt);

		lt.pop_back(10);
		lt.pop_back(20);

		Print(lt);

		lt.push_front();
		lt.pop_front();

		Print(lt);

		lt.clear();
		Print(lt);

		lt.emplace_back(10,20,30,0,1,2,3,4,5);
		Print(lt);

		list<int> lt2 = func(lt);
		Print(lt2);

		list<int> lt3;
		lt3 = func(lt);
		Print(lt3);
	}
}