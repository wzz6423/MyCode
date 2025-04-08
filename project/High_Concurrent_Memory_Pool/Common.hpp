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

//// ����ѿռ�
//inline static void* Alloc(size_t size) {
//	char* _memory = (char*)malloc(size); // Ҳ����ʹ����������ֱ��ʹ�� system call
//	if (_memory == nullptr) {
//		throw std::bad_alloc();
//	}
//	return _memory;
//}
//
//// �ͷŶѿռ�
//inline static void Free(void* ptr) {
//	free(ptr); // Ҳ����ʹ����������ֱ��ʹ�� system call
//}

// malloc Ҳ�Ǿ��� C һ���װ�ģ����صĵ�ַ���ܾ���λ�����������(������), ��˿���ֱ��ʹ�� system call
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

// �������������ڴ���С
static const size_t MAX_BYTES = 256 * 1024;
// ThreadCache & CentralCache ��������ĸ���
static const size_t NFREELIST = 208;
// PageCache ��������ĸ���
static const size_t NPAGES = 129;
// ���ڼ��������ҳ��
static const size_t PAGE_SHIFT = 13;

static void*& NextObj(void* obj) {
	return *(void**)obj;
}

// ��������С�Ķ���ӳ�����
class SizeClass
{
public:
	// ������������10%���ҵ�����Ƭ�˷�
	// [1,128]					8byte����	    freelist[0,16)
	// [128+1,1024]				16byte����	    freelist[16,72)
	// [1024+1,8*1024]			128byte����	    freelist[72,128)
	// [8*1024+1,64*1024]		1024byte����     freelist[128,184)
	// [64*1024+1,256*1024]		8*1024byte����   freelist[184,208)

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

	// ����ӳ�����һ����������Ͱ
	static inline size_t Index(size_t bytes){
		assert(bytes <= MAX_BYTES);

		// ÿ�������ж��ٸ���
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

	// һ��thread cache�����Ļ����ȡ���ٸ�
	static size_t NumMoveSize(size_t size){
		assert(size > 0);

		// [2, 512]��һ�������ƶ����ٸ������(������)����ֵ
		// С����һ���������޸�
		// С����һ���������޵�
		size_t num = MAX_BYTES / size;
		if (num < 2) {
			num = 2;
		}

		if (num > 512) {
			num = 512;
		}

		return num;
	}

	// ����һ����ϵͳ��ȡ����ҳ
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

// FreeList -- ��֯�зֺõ�С���ڴ�
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

// Span -- ��֯δ�зֵĴ���ڴ�
struct Span {
	PAGE_ID _pageId = 0; // ����ڴ���ʼҳ��ҳ��
	size_t  _n = 0;      // ҳ������

	Span* _next = nullptr;	// ˫������Ľṹ
	Span* _prev = nullptr;

	size_t _objSize = 0; // �к�С���ڴ�Ĵ�С
	size_t _useCount = 0; // �к�С���ڴ棬������� thread cache �ļ���
	void* _freeList = nullptr;  // �кõ�С���ڴ����������

	bool _isUse = false; // �Ƿ����ڱ�ʹ��

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

// ��ͷ˫��ѭ������
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
	std::mutex _mtx; // ��ϣͰ-Ͱ��
};