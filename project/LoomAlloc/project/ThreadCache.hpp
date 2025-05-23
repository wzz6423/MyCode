#pragma once

#include "Common.hpp"
#include "CentralCache.hpp"

class ThreadCache {
public:
	// 申请内存对象
	void* Allocate(size_t size) {
		assert(size <= MAX_BYTES);

		size_t alignSize = SizeClass::RoundUp(size);
		size_t index = SizeClass::Index(size);

		if (!_freeLists[index].Empty()) {
			return _freeLists[index].Pop();
		}
		else {
			return FetchFromCentralCache(index, alignSize);
		}
	}

	// 释放内存对象
	void Deallocate(void* ptr, size_t size) {
		assert(ptr);
		assert(size <= MAX_BYTES);

		// 找对应映射的自由链表桶, 对象插入进去
		size_t index = SizeClass::Index(size);
		_freeLists[index].Push(ptr);

		// 当链表长度大于一次批量申请的内存时就开始还一段 list 给 central cache
		if (_freeLists[index].Size() >= _freeLists[index].MaxSize()) {
			ListTooLong(_freeLists[index], size);
		}
	}

	// 从中心缓存获取对象
	void* FetchFromCentralCache(size_t index, size_t size) {
		// 满开始反馈调节算法: 平衡空间与时间
		// 1、起初不会一次向 central cache 一次要太多，因为要太多了可能用不完
		// 2、如果不断要这个 size 大小内存需求，那么 batchNum 就会不断增长，直到上限
		// 3、size 越大, 一次向 central cache 要的 batchNum 就越小
		// 4、size 越小, 一次向 central cache 要的 batchNum 就越大
#if (defined(_WIN64) || defined(_WIN32))
		size_t batchNum = min(_freeLists[index].MaxSize(), SizeClass::NumMoveSize(size)); // Windows头文件与 C++ 算法库冲突
#else
		size_t batchNum = std::min(_freeLists[index].MaxSize(), SizeClass::NumMoveSize(size));
#endif
		if (_freeLists[index].MaxSize() == batchNum) {
			++_freeLists[index].MaxSize();
		}

		void* start = nullptr;
		void* end = nullptr;
		size_t actualNum = CentralCache::GetInstance().FetchRangeObj(start, end, batchNum, size);
		assert(actualNum >= 1);

		if (actualNum == 1) {
			assert(start == end);
			return start;
		}
		else {
			_freeLists[index].PushRange(NextObj(start), end, actualNum - 1);
			return start;
		}
	}

	// 释放对象时, 链表过长时, 回收内存到中心缓存
	void ListTooLong(FreeList& list, size_t size) {
		void* start = nullptr;
		void* end = nullptr;
		list.PopRange(start, end, list.MaxSize());

		CentralCache::GetInstance().ReleaseListToSpans(start, size);
	}
private:
	FreeList _freeLists[NFREELIST];
};

// TLS thread local storage
static _declspec(thread) ThreadCache* pTLSThreadCache = nullptr;