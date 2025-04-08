#pragma once

#include "Common.hpp"
#include "PageCache.hpp"

// 单例模式 - 饿汉
class CentralCache{
public:
	static CentralCache& GetInstance(){
		return _sInst;
	}

	// 获取一个非空的 span
	Span* GetOneSpan(SpanList& list, size_t size, std::unique_lock<std::mutex>& mtx) {
		// 查看当前 spanlist 中是否有还有未分配对象的span
		SpanList::iterator it = list.Begin();
		while (it != list.End()) {
			if (it->_freeList != nullptr) {
				return it.value();
			}
			else {
				++it;
			}
		}

		// 解掉 central cache 的桶锁，如果其他线程释放内存对象回来，不会阻塞
		mtx.unlock();

		Span* span = nullptr;
		// 没有空闲 span , 找 page cache 要
		{
			std::lock_guard<std::mutex> mtx(PageCache::GetInstance()._pageMtx);
			span = PageCache::GetInstance().NewSpan(SizeClass::NumMovePage(size));
			span->_isUse = true;
			span->_objSize = size;
		}

		// 对获取到的 span 进行切分, 不需要加锁, 因为目前其他线程访问不到这个 span
		// 计算 span 的大块内存的起始地址和大块内存的大小(字节数)
		char* start = (char*)(span->PageIdToAdd(span->_pageId));
		size_t bytes = span->NumToSize(span->_n);
		char* end = start + bytes;

		// 把大块内存切成小块用自由链表链接起来
		// 先切一块下来去做头，方便尾插
		span->_freeList = start;
		start += size;
		void* tail = span->_freeList;
		while (start < end) {
			NextObj(tail) = start;
			tail = start;
			start += size;
		}
		NextObj(tail) = nullptr;

		// 切好 span 后, 把 span 挂到桶里面去的时候, 再加锁
		mtx.lock();
		list.PushFront(span);

		return span;
	}

	// 从中心缓存获取一定数量的对象给 thread cache
	size_t FetchRangeObj(void*& start, void*& end, size_t batchNum, size_t size) {
		size_t index = SizeClass::Index(size);
		size_t actualNum = 1;

		{
			std::unique_lock<std::mutex> mtx(_spanLists[index]._mtx);

			Span* span = GetOneSpan(_spanLists[index], size, mtx);
			assert(span);
			assert(span->_freeList);

			// 从 span 中获取 batchNum 个对象
			// 如果不够 batchNum 个，有多少拿多少
			start = span->_freeList;
			end = start;
			for (size_t i = 0; ((i < batchNum - 1) && (NextObj(end) != nullptr)); (++i, ++actualNum)) {
				end = NextObj(end);
			}
			span->_freeList = NextObj(end);
			NextObj(end) = nullptr;
			span->_useCount += actualNum;
		}

		return actualNum;
	}

	// 将一定数量的对象释放到 span 
	void ReleaseListToSpans(void* start, size_t size) {
		size_t index = SizeClass::Index(size);

		{
			std::unique_lock<std::mutex> mtx(_spanLists[index]._mtx);

			while (start) {
				void* next = NextObj(start);

				Span* span = PageCache::GetInstance().MapObjToSpan(start);
				NextObj(start) = span->_freeList;
				span->_freeList = start;
				--span->_useCount;

				// span 切分出去的所有小块内存都回来了
				// 这个 span 就可以再回收给 page cache , pagecache 再尝试做前后页的合并
				if (span->_useCount == 0) {
					_spanLists[index].Erase(span);
					span->_freeList = nullptr;
					span->_next = nullptr;
					span->_prev = nullptr;

					// 释放 span 给 page cache 时，使用 page cache 的锁即可
					// 解桶锁
					mtx.unlock();

					{
						std::lock_guard<std::mutex> pmtx(PageCache::GetInstance()._pageMtx);
						PageCache::GetInstance().ReleaseSpanToPageCache(span);
					}

					mtx.lock();
				}
				start = next;
			}
		}
	}
private:
	SpanList _spanLists[NFREELIST];
private:
	CentralCache(){}
	CentralCache(const CentralCache&) = delete;
	CentralCache& operator=(const CentralCache&) const = delete;

	static CentralCache _sInst;
};

CentralCache CentralCache::_sInst;