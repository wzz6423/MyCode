#pragma once

#include "Common.hpp"
#include "ObjectPool.hpp"
#include "PageMap.hpp"

// ����ģʽ - ����
class PageCache {
public:
	static PageCache& GetInstance(){
		return _sInst;
	}

	// ��ȡ�Ӷ��� span ��ӳ��
	Span* MapObjToSpan(void* obj) {
		PAGE_ID id = Span::AddToPageId(obj);

		//{
		//	std::lock_guard<std::mutex> mtx(_pageMtx);
		//	std::unordered_map<PAGE_ID, Span*>::iterator ret = _idSpanMap.find(id);
		//	if (ret != _idSpanMap.end()) {
		//		return ret->second;
		//	}
		//}

		// ���ü��� -- STL�����������̰߳�ȫ����(û����ǰ���ռ�, ���в����ṹ���)
		// ��������ǰ���ÿռ�, ��д�����������ڽ���ʱ�Ѿ���ǰ����, ���Բ��ü���
		// ͬʱ�������ڽṹ�ϵ����Ƶ��¶�д���뼴���� / ����̲߳���ͬʱ����һ������
		Span* ret = (Span*)_idSpanMap.get(id);
		if (ret) {
			return ret;
		}
		else {
			assert(false);
			return nullptr;
		}
	}

	// ��ȡһ�� size ҳ�� span
	Span* NewSpan(size_t size) {
		assert(size > 0);

		// �Ƿ�Ϊ���� >128 page ���ڴ��
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

		// �ȼ��� size Ͱ���滹��û�� span
		if (!_spanLists[size].Empty()) {
			Span* sSpan = _spanLists[size].PopFront();

			// ���� id �� span ��ӳ�䣬���� central cache ����С���ڴ�ʱ�����Ҷ�Ӧ�� span
			for (PAGE_ID i = 0; i < sSpan->_n; ++i) {
				//_idSpanMap[sSpan->_pageId + i] = sSpan;
				_idSpanMap.set(sSpan->_pageId + i, sSpan);
			}

			return sSpan;
		}

		// �������Ͱ������û�� span ������п��԰��������з�
		for (size_t i = size + 1; i < NPAGES; ++i) {
			if (!_spanLists[i].Empty()) {
				Span* nSpan = _spanLists[i].PopFront();
				Span* sSpan = _spanPool.New();

				// �� nSpan ��ͷ����һ�� size ҳ����
				// size ҳ span ����
				// nSpan �ٹҵ���Ӧӳ���λ��
				sSpan->_pageId = nSpan->_pageId;
				sSpan->_n = size;

				nSpan->_pageId += size;
				nSpan->_n -= size;

				_spanLists[nSpan->_n].PushFront(nSpan);

				// �洢 nSpan ����λҳ�Ÿ� nSpan ӳ�䣬���� page cache �����ڴ�ʱ���еĺϲ�����
				//_idSpanMap[nSpan->_pageId] = nSpan;
				//_idSpanMap[nSpan->_pageId + nSpan->_n - 1] = nSpan;
				_idSpanMap.set(nSpan->_pageId, nSpan);
				_idSpanMap.set(nSpan->_pageId + nSpan->_n - 1, nSpan);

				// ���� id �� span ��ӳ�䣬���� central cache ����С���ڴ�ʱ�����Ҷ�Ӧ�� span
				for (PAGE_ID i = 0; i < sSpan->_n; ++i) {
					//_idSpanMap[sSpan->_pageId + i] = sSpan;
					_idSpanMap.set(sSpan->_pageId + i, sSpan);
				}

				return sSpan;
			}
		}

		// ����û�д�ҳ span
		// �Ҷ�����һ�� 128 ҳ�� span
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

	// �ͷſ��� span �� Page Cache �����ϲ����ڵ� span
	void ReleaseSpanToPageCache(Span* span) {
		// >128 page ��ֱ�ӻ�����
		if (span->_n > NPAGES - 1) {
			void* ptr = (void*)Span::PageIdToAdd(span->_pageId);
			Free(ptr, span->_n);
			_spanPool.Delete(span);
			return;
		}

		// �� span ǰ���ҳ�����Խ��кϲ��������ڴ���Ƭ����
		while (true) {
			PAGE_ID prevId = span->_pageId - 1;
			//std::unordered_map<PAGE_ID, Span*>::iterator ret = _idSpanMap.find(prevId);
			Span* ret = (Span*)_idSpanMap.get(prevId);
			// ǰ���ҳ��û�У����ϲ�
			//if (ret == _idSpanMap.end()) {
			//	break;
			//}
			if (ret == nullptr) {
				break;
			}
			
			// ǰ������ҳ�� span ��ʹ�ã����ϲ�
			//Span* prevSpan = ret->second;
			Span* prevSpan = ret;
			if (prevSpan->_isUse == true) {
				break;
			}
			
			// �ϲ������� 128 ҳ�� span �������ҳ�����ƣ����ϲ�
			if (prevSpan->_n + span->_n > NPAGES - 1) {
				break;
			}

			// �ϲ�
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
			// �����ҳ��û�У����ϲ�
			//if (ret == _idSpanMap.end()) {
			//	break;
			//}
			if (ret == nullptr) {
				break;
			}

			// ��������ҳ�� span ��ʹ�ã����ϲ�
			//Span* nextSpan = ret->second;
			Span* nextSpan = ret;
			if (nextSpan->_isUse == true) {
				break;
			}

			// �ϲ������� 128 ҳ�� span �������ҳ�����ƣ����ϲ�
			if (nextSpan->_n + span->_n > NPAGES - 1) {
				break;
			}

			// �ϲ�
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