#pragma once
#include <iostream>
using namespace std;

// ���һ�����ܱ���������
// Cpp98
class CopyBan {
private:
	CopyBan(const CopyBan&);
	CopyBan& operator=(const CopyBan&);
};
// Cpp11
class CopyBan {
private:
	CopyBan(const CopyBan&) = delete;
	CopyBan& operator=(const CopyBan&) = delete;
};

// ���һ��ֻ���ڶ��ϴ����������
// 1.˽�й��졢��������
class HeapOnly {
	static HeapOnly* CreatObject() {
		return new HeapOnly;
	}
private:
	HeapOnly() ;

	// Cpp98
	HeapOnly(const HeapOnly&);
	HeapOnly& operator=(const HeapOnly&);

	// Cpp11
	HeapOnly(const HeapOnly&) = delete;
	HeapOnly& operator=(const HeapOnly&) = delete;
};
// 2.˽������
class HeapOnly {
	void Destroy() {
		delete this;
	}
private:
	~HeapOnly(){

	}
};

// ���һ��ֻ����ջ�ϴ���������� -- ֻ��˽�й��죬����˽������(ջ�ϵĶ�����new�����ģ��޷��ֶ��ͷ���Դ)
class StackOnly {
public:
	static StackOnly CreatObject() {
		StackOnly st;
		return st;
	}
private:
	StackOnly() {

	}
};

// ���һ�����ܱ��̳е���3
// Cpp98 -- ˽�й��죬��������������๹��
class NonInherit {
public:
	static NonInherit GetInstance() {
		return NonInherit();
	}
private:
	NonInherit() {}
};
// Cpp11
class NonInherit final {};

// ���ֻ�ܴ���һ��������� -- ��������
// 1. ����ģʽ -- �����ò�����������ʱ������һ������ -- �� -- ��������������������������֮�������Ͽ�������˳���ȶ�����bug
// ���̸߲߳���������Ƶ��ʹ������Ҫ����ö���������Դ�����������Ӧ�ٶ�
class Hungry{
public:
	static Hungry& GetInstance() {
		return _sinst;
	}
private:
	Hungry(){}

	Hungry(const Hungry&) = delete;
	Hungry& operator=(const Hungry&) = delete;

	static Hungry _sinst;
};
Hungry Hungry::_sinst;

// 2.����ģʽ(�ӳټ���) -- ��һ��ʹ��ʵ������ʱ�������󣬽��������޸��أ�������������ʵ������˳��ɿ� -- ����
class Lazy {
public:
	static Lazy& GetInstance() {
		if (!_psinst) {
			_psinst = new Lazy;
		}
		return *_psinst;
	}

	// ��;��Ҫ�ͷ� or �������ʱ��һЩ��Ϊ(��־û�) -- ��������Զ��������ڲ�public��������ʹ�Զ�������ʵ��
	static void DelInstance() {
		if (_psinst) {
			delete _psinst; 
			// new = operator new + ����
			// delete = ����(�������н���������Ϊ) + operator delete
			_psinst = nullptr;
		}
	}

	// �ڲ������ⲿ�����Ȼ��Ԫ(�ⲿ�����ڲ�����Ԫ)������������ⲿ����Ҫ��Ԫ����ʵ���������
	// �ڲ������ֱ�ӷ����ⲿ��static�����ⲿ�Ķ�������ⲿ
	class Del {
	public:
		~Del() {
			DelInstance();
		}
	};
private:
	Lazy(){}
	~Lazy(){}

	Lazy(const Lazy&) = delete;
	Lazy& operator=(const Lazy&) = delete;

	static Del _del;
	static Lazy* _psinst;
};
Lazy* Lazy::_psinst = nullptr;
Lazy::Del Lazy::_del;