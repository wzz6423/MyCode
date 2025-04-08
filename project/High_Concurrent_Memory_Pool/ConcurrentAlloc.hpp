#pragma once

#include "Common.hpp"
#include "ThreadCache.hpp"

static void* ConcurrentAlloc(size_t size) {
	if (size > MAX_BYTES) {
		size_t alignSize = SizeClass::RoundUp(size);
		size_t kpage = Span::AddToPageId((void*)alignSize);

		Span* span = nullptr;
		{
			std::lock_guard<std::mutex> mtx(PageCache::GetInstance()._pageMtx);
			span = PageCache::GetInstance().NewSpan(kpage);
			span->_objSize = size;
		}

		void* ptr = (void*)Span::PageIdToAdd((PAGE_ID)span->_pageId);
		return ptr;
	}
	else {
		// ͨ�� TLS ÿ���߳������Ļ�ȡ�Լ���ר���� ThreadCache ����
		if (pTLSThreadCache == nullptr) {
			//static ObjectPool<ThreadCache> tcPool; // ���� static ��ͻ���ֶ��߳�ͬʱ����һ���ٽ���Դ��������ô�ڶ����ڴ���о���Ҫ����
			ObjectPool<ThreadCache> tcPool;
			pTLSThreadCache = tcPool.New();
		}

		return pTLSThreadCache->Allocate(size);
	}
}

static void ConcurrentFree(void* ptr) {
	Span* span = PageCache::GetInstance().MapObjToSpan(ptr);
	size_t size = span->_objSize;

	if (size > MAX_BYTES) {
		{
			std::lock_guard<std::mutex> mtx(PageCache::GetInstance()._pageMtx);
			PageCache::GetInstance().ReleaseSpanToPageCache(span);
		}
	}
	else {
		assert(pTLSThreadCache);
		pTLSThreadCache->Deallocate(ptr, size);
	}
}