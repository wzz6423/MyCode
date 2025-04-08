#pragma once

#include "Common.hpp"

// �����ڴ��
template <typename T>
class ObjectPool {
public:
	T* New(size_t mallocSize = 128 * 1024){
		T* obj = nullptr;

		// ����ʹ�ñ������������������ڵ��ڴ�����
		if (_freeList != nullptr) {
			void* next = *((void**)_freeList); // ȡ������ _freeList ��һ���ڴ������д�ŵ���һ���ڴ�����ĵ�ַ��ͬʱ *(void**) ���Ա��Զ�ʶ��ǰϵͳ x32/x64 �Ծ�����ȡ���ֽ�
			// ���ֻ�� (void*) ֻ��ת���˶�ȡ����ֵ�����Ͷ�û���Զ�ʶ���ȡ���ֽ�(ָ�룺x32 -- 4�� x64 -- 8)
			obj = (T*)_freeList;
			_freeList = next;
		}
		else {
			// ʣ���ڴ���С����һ�������Сʱ��������ϵͳ�����ڴ�ռ�
			if (_remainBytes < sizeof(T)) {
				_remainBytes = mallocSize; // Ϊʲô���� sizeof(T) ����������ϵͳ�����ڴ�ռ�: �ڴ������ҳ (4kb) Ϊ��λ, ������������Ѻ�
				//_memory = (char*)Alloc(_remainBytes); // malloc
				_memory = (char*)Alloc(Span::SizeToNum(_remainBytes)); // system call
			}

			obj = (T*)_memory;
			size_t objSize = sizeof(T) < sizeof(void*) ? sizeof(void*) : sizeof(T);
			_memory += objSize;
			_remainBytes -= objSize;
		}

		// ��λnew, ��ʾ����T�Ĺ��캯����ʼ��
		new(obj)T;

		return obj;
	}

	void Delete(T* obj) {
		// ��ʾ�������������������
		obj->~T();
		
		// Ҳ���Ը� obj �β������ã���Ҫ��������������
		*(void**)obj = _freeList;
		_freeList = obj;
	}
private:
	char* _memory = nullptr; // ָ�����ڴ��ָ��
	size_t _remainBytes = 0; // ����ڴ汻�зֺ�ʣ����ֽ���
	void* _freeList = nullptr; // ��������
};