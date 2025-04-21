#pragma once

// C++
#include <iostream>
#include <vector>
//#include <unordered_map>
#include <thread>
#include <mutex>
// C
#include <ctime>
#include <cstdlib>
#include <cassert>

#if (defined(_WIN64) || defined(_WIN32))
#include <Windows.h>
#else
#include <algorithm
#include <unistd.h>
#include <sys/mman.h>
#endif

#if (defined(_WIN64) || defined(__LP64__))
	using PAGE_ID = unsigned long long;
#elif (defined(_WIN32) || defined(__ILP32__))
	using PAGE_ID = size_t;
#endif

//// 申请堆空间
//inline static void* Alloc(size_t size) {
//	char* _memory = (char*)malloc(size); // 也可以使用条件编译直接使用 system call
//	if (_memory == nullptr) {
//		throw std::bad_alloc();
//	}
//	return _memory;
//}
//
//// 释放堆空间
//inline static void Free(void* ptr) {
//	free(ptr); // 也可以使用条件编译直接使用 system call
//}

// malloc 也是经过 C 一层封装的，返回的地址可能经过位运算出现问题(概率性), 因此考虑直接使用 system call
inline static void* Alloc(size_t kpage){
#ifdef _WIN32
	void* ptr = VirtualAlloc(0, kpage << 13, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
	void* ptr = mmap(NULL, kpage << 13, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
	if (ptr == nullptr)
		throw std::bad_alloc();

	return ptr;
}

inline static void Free(void* ptr, size_t kpage = 0){
#ifdef _WIN32
	if (!VirtualFree(ptr, 0, MEM_RELEASE)) {
		throw "free error";
	}
#else
	if (munmap(ptr, kpage << 13) == -1) {
		throw "free error";
	}
#endif
}

// 单个申请最大的内存块大小
static const size_t MAX_BYTES = 256 * 1024;
// ThreadCache & CentralCache 自由链表的个数
static const size_t NFREELIST = 208;
// PageCache 自由链表的个数
static const size_t NPAGES = 129;
// 用于计算申请的页数
static const size_t PAGE_SHIFT = 13;

static void*& NextObj(void* obj) {
	return *(void**)obj;
}

// 计算对象大小的对齐映射规则
class SizeClass
{
public:
	// 整体控制在最多10%左右的内碎片浪费
	// [1,128]					8byte对齐	    freelist[0,16)
	// [128+1,1024]				16byte对齐	    freelist[16,72)
	// [1024+1,8*1024]			128byte对齐	    freelist[72,128)
	// [8*1024+1,64*1024]		1024byte对齐     freelist[128,184)
	// [64*1024+1,256*1024]		8*1024byte对齐   freelist[184,208)

	//size_t _RoundUp(size_t size, size_t alignNum){
	//	size_t alignSize;
	//	if (size % alignNum != 0){
	//		alignSize = (size / alignNum + 1)*alignNum;
	//	}
	//	else{
	//		alignSize = size;
	//	}

	//	return alignSize;
	//}

	static inline size_t _RoundUp(size_t bytes, size_t alignNum) {
		return ((bytes + alignNum - 1) & ~(alignNum - 1));
	}

	static inline size_t RoundUp(size_t size){
		if (size <= 128){
			return _RoundUp(size, 8);
		}
		else if (size <= 1024){
			return _RoundUp(size, 16);
		}
		else if (size <= 8 * 1024)
		{
			return _RoundUp(size, 128);
		}
		else if (size <= 64 * 1024){
			return _RoundUp(size, 1024);
		}
		else if (size <= 256 * 1024){
			return _RoundUp(size, 8 * 1024);
		}
		else{
			return _RoundUp(size, 1 << PAGE_SHIFT);
		}
	}

	//size_t _Index(size_t bytes, size_t alignNum){
	//	if (bytes % alignNum == 0){
	//		return bytes / alignNum - 1;
	//	}
	//	else{
	//		return bytes / alignNum;
	//	}
	//}

	static inline size_t _Index(size_t bytes, size_t align_shift){
		return ((bytes + ( 1 << align_shift) - 1 ) >> align_shift) - 1;
	}

	// 计算映射的哪一个自由链表桶
	static inline size_t Index(size_t bytes){
		assert(bytes <= MAX_BYTES);

		// 每个区间有多少个链
		static int group_array[4] = { 16, 56, 56, 56 };
		if (bytes <= 128) {
			return _Index(bytes, 3);
		}
		else if (bytes <= 1024) {
			return _Index(bytes - 128, 4) + group_array[0];
		}
		else if (bytes <= 8 * 1024) {
			return _Index(bytes - 1024, 7) + group_array[1] + group_array[0];
		}
		else if (bytes <= 64 * 1024) {
			return _Index(bytes - 8 * 1024, 10) + group_array[2] + group_array[1] + group_array[0];
		}
		else if (bytes <= 256 * 1024) {
			return _Index(bytes - 64 * 1024, 13) + group_array[3] + group_array[2] + group_array[1] + group_array[0];
		}
		else {
			assert(false);
		}

		return -1;
	}

	// 一次thread cache从中心缓存获取多少个
	static size_t NumMoveSize(size_t size){
		assert(size > 0);

		// [2, 512]，一次批量移动多少个对象的(慢启动)上限值
		// 小对象一次批量上限高
		// 小对象一次批量上限低
		size_t num = MAX_BYTES / size;
		if (num < 2) {
			num = 2;
		}

		if (num > 512) {
			num = 512;
		}

		return num;
	}

	// 计算一次向系统获取几个页
	static size_t NumMovePage(size_t size){
		size_t num = NumMoveSize(size);
		size_t npage = num * size;

		npage >>= PAGE_SHIFT;
		if (npage == 0) {
			npage = 1;
		}

		return npage;
	}
};

// FreeList -- 组织切分好的小块内存
class FreeList {
public:
	void Push(void* obj) {
		assert(obj);

		NextObj(obj) = _freeList;
		_freeList = obj;
		++_size;
	}

	void PushRange(void* start, void* end, size_t n) {
		NextObj(end) = _freeList;
		_freeList = start;
		_size += n;
	}

	void* Pop() {
		assert(_freeList);

		void* obj = _freeList;
		_freeList = NextObj(_freeList);
		--_size;

		return obj;
	}

	void PopRange(void*& start, void*& end, size_t n) {
		assert(n <= _size);
		
		start = _freeList;
		end = _freeList;

		for (size_t i = 0; i < n - 1; ++i) {
			end = NextObj(end);
		}

		_freeList = NextObj(end);
		NextObj(end) = nullptr;
		_size -= n;
	}

	bool Empty() {
		return _freeList == nullptr;
	}

	size_t& MaxSize() {
		return _maxSize;
	}

	size_t Size() {
		return _size;
	}
private:
	void* _freeList = nullptr;
	size_t _maxSize = 1;
	size_t _size = 0;
};

// Span -- 组织未切分的大块内存
struct Span {
	PAGE_ID _pageId = 0; // 大块内存起始页的页号
	size_t  _n = 0;      // 页的数量

	Span* _next = nullptr;	// 双向链表的结构
	Span* _prev = nullptr;

	size_t _objSize = 0; // 切好小块内存的大小
	size_t _useCount = 0; // 切好小块内存，被分配给 thread cache 的计数
	void* _freeList = nullptr;  // 切好的小块内存的自由链表

	bool _isUse = false; // 是否正在被使用

	static PAGE_ID PageIdToAdd(PAGE_ID pageId) {
		return pageId << PAGE_SHIFT;
	}

	static PAGE_ID AddToPageId(void* ptr) {
		return (PAGE_ID)ptr >> PAGE_SHIFT;
	}

	static size_t NumToSize(size_t n) {
		return n << PAGE_SHIFT;
	}

	static size_t SizeToNum(size_t n) {
		return n >> PAGE_SHIFT;
	}
};

class __spanlist_iterator {
private:
	using iterator = Span*;
	using self = __spanlist_iterator;
public:
	__spanlist_iterator(iterator spanptr)
		:_spanptr(spanptr)
	{}

	iterator operator++() {
		_spanptr = _spanptr->_next;
		return _spanptr->_next;
	}

	iterator operator--() {
		_spanptr = _spanptr->_prev;
		return _spanptr->_prev;
	}

	bool operator!=(self it) const {
		return _spanptr != it._spanptr;
	}

	Span* value() const {
		return _spanptr;
	}

	Span* operator->() {
		return &(*_spanptr);
	}
private:
	iterator _spanptr = nullptr;
};

// 带头双向循环链表
class SpanList {
public:
	using iterator = __spanlist_iterator;
public:
	SpanList() {
		_head = new Span();
		_head->_next = _head;
		_head->_prev = _head;
	}

	iterator Begin() {
		return _head->_next;
	}

	iterator End() {
		return _head;
	}

	bool Empty() {
		return _head->_next == _head;
	}

	void PushFront(Span* span) {
		Insert(Begin().value(), span);
	}

	Span* PopFront() {
		Span* front = _head->_next;
		Erase(front);
		return front;
	}

	void Insert(Span* pos, Span* newSpan) {
		assert(_head);
		assert(pos);
		assert(newSpan);

		// prev newSpan pos
		Span* prev = pos->_prev;
		prev->_next = newSpan;
		newSpan->_prev = prev;
		newSpan->_next = pos;
		pos->_prev = newSpan;
	}

	void Erase(Span* pos){
		assert(pos);
		assert(_head);
		assert(_head->_next);
		assert(pos != _head);

		// prev pos next
		pos->_prev->_next = pos->_next;
		pos->_next->_prev = pos->_prev;
	}
private:
	Span* _head;
public:
	std::mutex _mtx; // 哈希桶-桶锁
};