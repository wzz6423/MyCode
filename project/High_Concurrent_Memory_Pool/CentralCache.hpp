#pragma once

#include "Common.hpp"
#include "PageCache.hpp"

// ����ģʽ - ����
class CentralCache{
public:
	static CentralCache& GetInstance(){
		return _sInst;
	}

	// ��ȡһ���ǿյ� span
	Span* GetOneSpan(SpanList& list, size_t size, std::unique_lock<std::mutex>& mtx) {
		// �鿴��ǰ spanlist ���Ƿ��л���δ��������span
		SpanList::iterator it = list.Begin();
		while (it != list.End()) {
			if (it->_freeList != nullptr) {
				return it.value();
			}
			else {
				++it;
			}
		}

		// ��� central cache ��Ͱ������������߳��ͷ��ڴ�����������������
		mtx.unlock();

		Span* span = nullptr;
		// û�п��� span , �� page cache Ҫ
		{
			std::lock_guard<std::mutex> mtx(PageCache::GetInstance()._pageMtx);
			span = PageCache::GetInstance().NewSpan(SizeClass::NumMovePage(size));
			span->_isUse = true;
			span->_objSize = size;
		}

		// �Ի�ȡ���� span �����з�, ����Ҫ����, ��ΪĿǰ�����̷߳��ʲ������ span
		// ���� span �Ĵ���ڴ����ʼ��ַ�ʹ���ڴ�Ĵ�С(�ֽ���)
		char* start = (char*)(span->PageIdToAdd(span->_pageId));
		size_t bytes = span->NumToSize(span->_n);
		char* end = start + bytes;

		// �Ѵ���ڴ��г�С��������������������
		// ����һ������ȥ��ͷ������β��
		span->_freeList = start;
		start += size;
		void* tail = span->_freeList;
		while (start < end) {
			NextObj(tail) = start;
			tail = start;
			start += size;
		}
		NextObj(tail) = nullptr;

		// �к� span ��, �� span �ҵ�Ͱ����ȥ��ʱ��, �ټ���
		mtx.lock();
		list.PushFront(span);

		return span;
	}

	// �����Ļ����ȡһ�������Ķ���� thread cache
	size_t FetchRangeObj(void*& start, void*& end, size_t batchNum, size_t size) {
		size_t index = SizeClass::Index(size);
		size_t actualNum = 1;

		{
			std::unique_lock<std::mutex> mtx(_spanLists[index]._mtx);

			Span* span = GetOneSpan(_spanLists[index], size, mtx);
			assert(span);
			assert(span->_freeList);

			// �� span �л�ȡ batchNum ������
			// ������� batchNum �����ж����ö���
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

	// ��һ�������Ķ����ͷŵ� span 
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

				// span �зֳ�ȥ������С���ڴ涼������
				// ��� span �Ϳ����ٻ��ո� page cache , pagecache �ٳ�����ǰ��ҳ�ĺϲ�
				if (span->_useCount == 0) {
					_spanLists[index].Erase(span);
					span->_freeList = nullptr;
					span->_next = nullptr;
					span->_prev = nullptr;

					// �ͷ� span �� page cache ʱ��ʹ�� page cache ��������
					// ��Ͱ��
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