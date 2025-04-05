#pragma once
namespace wzz {
	template <typename Iterator, typename Ref, typename Ptr>
	struct Reverse_Iterator {
		typedef Reverse_Iterator<Iterator, Ref, Ptr> self;

		Iterator _it;

		Reverse_Iterator(Iterator it)
			:_it(it)
		{ }

		Ref operator*() {
			Iterator tmp = _it;
			return *(--tmp);
		}

		Ptr operator->() {
			return &(operator*());
		}

		self& operator++() {
			--_it;
			return *this;
		}

		self& operator++(int) {
			_it--;
			return *this;
		}

		self& operator--() {
			++_it;
			return *this;
		}

		self& operator--(int) {
			_it++;
			return *this;
		}

		bool operator==(const self& s) const {
			return _it == s._it;
		}

		bool operator!=(const self& s) const {
			return _it != s._it;
		}
	};
}