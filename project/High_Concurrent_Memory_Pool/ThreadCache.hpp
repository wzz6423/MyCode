#pragma once

#include "Common.hpp"
#include "CentralCache.hpp"

class ThreadCache {
public:
	// �����ڴ����
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

	// �ͷ��ڴ����
	void Deallocate(void* ptr, size_t size) {
		assert(ptr);
		assert(size <= MAX_BYTES);

		// �Ҷ�Ӧӳ�����������Ͱ, ��������ȥ
		size_t index = SizeClass::Index(size);
		_freeLists[index].Push(ptr);

		// �������ȴ���һ������������ڴ�ʱ�Ϳ�ʼ��һ�� list �� central cache
		if (_freeLists[index].Size() >= _freeLists[index].MaxSize()) {
			ListTooLong(_freeLists[index], size);
		}
	}

	// �����Ļ����ȡ����
	void* FetchFromCentralCache(size_t index, size_t size) {
		// ����ʼ���������㷨: ƽ��ռ���ʱ��
		// 1���������һ���� central cache һ��Ҫ̫�࣬��ΪҪ̫���˿����ò���
		// 2���������Ҫ��� size ��С�ڴ�������ô batchNum �ͻ᲻��������ֱ������
		// 3��size Խ��, һ���� central cache Ҫ�� batchNum ��ԽС
		// 4��size ԽС, һ���� central cache Ҫ�� batchNum ��Խ��
#if (defined(_WIN64) || defined(_WIN32))
		size_t batchNum = min(_freeLists[index].MaxSize(), SizeClass::NumMoveSize(size)); // Windowsͷ�ļ��� C++ �㷨���ͻ
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

	// �ͷŶ���ʱ, �������ʱ, �����ڴ浽���Ļ���
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