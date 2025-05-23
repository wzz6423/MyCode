#pragma once

#include "Common.hpp"
#include "ObjectPool.hpp"
#include "PageMap.hpp"

// 单例模式 - 饿汉
class PageCache {
public:
	static PageCache& GetInstance(){
		return _sInst;
	}

	// 获取从对象到 span 的映射
	Span* MapObjToSpan(void* obj) {
		PAGE_ID id = Span::AddToPageId(obj);

		//{
		//	std::lock_guard<std::mutex> mtx(_pageMtx);
		//	std::unordered_map<PAGE_ID, Span*>::iterator ret = _idSpanMap.find(id);
		//	if (ret != _idSpanMap.end()) {
		//		return ret->second;
		//	}
		//}

		// 不用加锁 -- STL容器可能有线程安全问题(没有提前开空间, 进行操作结构会变)
		// 基数树提前开好空间, 读写的两个函数在进行时已经提前加锁, 所以不用加锁
		// 同时基数树在结构上的优势导致读写分离即两个 / 多个线程不会同时访问一个对象
		Span* ret = (Span*)_idSpanMap.get(id);
		if (ret) {
			return ret;
		}
		else {
			assert(false);
			return nullptr;
		}
	}

	// 获取一个 size 页的 span
	Span* NewSpan(size_t size) {
		assert(size > 0);

		// 是否为申请 >128 page 的内存块
		if (size > NPAGES - 1) {
			//void* ptr = Alloc(Span::NumToSize(size)); // malloc
			void* ptr = Alloc(size); // system call
			Span* span = _spanPool.New();

			span->_pageId = Span::AddToPageId(ptr);
			span->_n = size;

			//_idSpanMap[span->_pageId] = span;
			_idSpanMap.set(span->_pageId, span);

			return span;
		}

		// 先检查第 size 桶里面还有没有 span
		if (!_spanLists[size].Empty()) {
			Span* sSpan = _spanLists[size].PopFront();

			// 建立 id 和 span 的映射，方便 central cache 回收小块内存时，查找对应的 span
			for (PAGE_ID i = 0; i < sSpan->_n; ++i) {
				//_idSpanMap[sSpan->_pageId + i] = sSpan;
				_idSpanMap.set(sSpan->_pageId + i, sSpan);
			}

			return sSpan;
		}

		// 检查后面的桶里面有没有 span ，如果有可以把它进行切分
		for (size_t i = size + 1; i < NPAGES; ++i) {
			if (!_spanLists[i].Empty()) {
				Span* nSpan = _spanLists[i].PopFront();
				Span* sSpan = _spanPool.New();

				// 在 nSpan 的头部切一个 size 页下来
				// size 页 span 返回
				// nSpan 再挂到对应映射的位置
				sSpan->_pageId = nSpan->_pageId;
				sSpan->_n = size;

				nSpan->_pageId += size;
				nSpan->_n -= size;

				_spanLists[nSpan->_n].PushFront(nSpan);

				// 存储 nSpan 的首位页号跟 nSpan 映射，方便 page cache 回收内存时进行的合并查找
				//_idSpanMap[nSpan->_pageId] = nSpan;
				//_idSpanMap[nSpan->_pageId + nSpan->_n - 1] = nSpan;
				_idSpanMap.set(nSpan->_pageId, nSpan);
				_idSpanMap.set(nSpan->_pageId + nSpan->_n - 1, nSpan);

				// 建立 id 和 span 的映射，方便 central cache 回收小块内存时，查找对应的 span
				for (PAGE_ID i = 0; i < sSpan->_n; ++i) {
					//_idSpanMap[sSpan->_pageId + i] = sSpan;
					_idSpanMap.set(sSpan->_pageId + i, sSpan);
				}

				return sSpan;
			}
		}

		// 后面没有大页 span
		// 找堆申请一个 128 页的 span
		Span* bigSpan = _spanPool.New();
		//void* ptr = Alloc(Span::NumToSize(NPAGES - 1)); // malloc
		void* ptr = Alloc(NPAGES - 1); // system call
		bigSpan->_pageId = Span::AddToPageId(ptr);
		bigSpan->_n = NPAGES - 1;

		_spanLists[bigSpan->_n].PushFront(bigSpan);
		//_idSpanMap[bigSpan->_pageId] = bigSpan;
		//_idSpanMap[bigSpan->_pageId + bigSpan->_n - 1] = bigSpan;
		_idSpanMap.set(bigSpan->_pageId, bigSpan);
		_idSpanMap.set(bigSpan->_pageId + bigSpan->_n - 1, bigSpan);

		return NewSpan(size);
	}

	// 释放空闲 span 到 Page Cache ，并合并相邻的 span
	void ReleaseSpanToPageCache(Span* span) {
		// >128 page 的直接还给堆
		if (span->_n > NPAGES - 1) {
			void* ptr = (void*)Span::PageIdToAdd(span->_pageId);
			Free(ptr, span->_n);
			_spanPool.Delete(span);
			return;
		}

		// 对 span 前后的页，尝试进行合并，缓解内存碎片问题
		while (true) {
			PAGE_ID prevId = span->_pageId - 1;
			//std::unordered_map<PAGE_ID, Span*>::iterator ret = _idSpanMap.find(prevId);
			Span* ret = (Span*)_idSpanMap.get(prevId);
			// 前面的页号没有，不合并
			//if (ret == _idSpanMap.end()) {
			//	break;
			//}
			if (ret == nullptr) {
				break;
			}
			
			// 前面相邻页的 span 在使用，不合并
			//Span* prevSpan = ret->second;
			Span* prevSpan = ret;
			if (prevSpan->_isUse == true) {
				break;
			}
			
			// 合并出超过 128 页的 span 超出最大页数限制，不合并
			if (prevSpan->_n + span->_n > NPAGES - 1) {
				break;
			}

			// 合并
			span->_pageId = prevSpan->_pageId;
			span->_n += prevSpan->_n;

			_spanLists[prevSpan->_n].Erase(prevSpan);
			//_idSpanMap.erase(prevId);
			_idSpanMap.set(prevId, nullptr);
			_spanPool.Delete(prevSpan);
		}

		while (true) {
			PAGE_ID nextId = span->_pageId + span->_n;
			//std::unordered_map<PAGE_ID, Span*>::iterator ret = _idSpanMap.find(nextId);
			Span* ret = (Span*)_idSpanMap.get(nextId);
			// 后面的页号没有，不合并
			//if (ret == _idSpanMap.end()) {
			//	break;
			//}
			if (ret == nullptr) {
				break;
			}

			// 后面相邻页的 span 在使用，不合并
			//Span* nextSpan = ret->second;
			Span* nextSpan = ret;
			if (nextSpan->_isUse == true) {
				break;
			}

			// 合并出超过 128 页的 span 超出最大页数限制，不合并
			if (nextSpan->_n + span->_n > NPAGES - 1) {
				break;
			}

			// 合并
			span->_n += nextSpan->_n;

			_spanLists[nextSpan->_n].Erase(nextSpan);
			//_idSpanMap.erase(nextId);
			_idSpanMap.set(nextId, nullptr);
			_spanPool.Delete(nextSpan);
		}

		_spanLists[span->_n].PushFront(span);
		span->_isUse = false;
		//_idSpanMap[span->_pageId] = span;
		//_idSpanMap[span->_pageId + span->_n - 1] = span;
		_idSpanMap.set(span->_pageId, span);
		_idSpanMap.set(span->_pageId + span->_n - 1, span);
	}
public:
	std::mutex _pageMtx;
private:
	SpanList _spanLists[NPAGES];
	ObjectPool<Span> _spanPool;

	//std::unordered_map<PAGE_ID, Span*> _idSpanMap;
#if (defined(_WIN64) || defined(__LP64__))
	TCMalloc_PageMap3<64 - PAGE_SHIFT> _idSpanMap;
#elif (defined(_WIN32) || defined(__ILP32__))
	//TCMalloc_PageMap1<32 - PAGE_SHIFT> _idSpanMap;
	TCMalloc_PageMap2<32 - PAGE_SHIFT> _idSpanMap;
#endif

	PageCache(){}
	PageCache(const PageCache&) = delete;
	void operator=(const PageCache&) = delete;

	static PageCache _sInst;
};
PageCache PageCache::_sInst;