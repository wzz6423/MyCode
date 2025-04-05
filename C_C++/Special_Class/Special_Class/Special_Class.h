#pragma once
#include <iostream>
using namespace std;

// 设计一个不能被拷贝的类
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

// 设计一个只能在堆上创建对象的类
// 1.私有构造、拷贝构造
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
// 2.私有析构
class HeapOnly {
	void Destroy() {
		delete this;
	}
private:
	~HeapOnly(){

	}
};

// 设计一个只能在栈上创建对象的类 -- 只能私有构造，不能私有析构(栈上的对象不是new出来的，无法手动释放资源)
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

// 设计一个不能被继承的类3
// Cpp98 -- 私有构造，派生类调不到基类构造
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

// 设计只能创建一个对象的类 -- 单例对象
// 1. 饿汉模式 -- 不管用不用启动程序时都创建一个对象 -- 简单 -- 进程启动慢、如果多个单例对象之间存在耦合可能启动顺序不稳定导致bug
// 多线程高并发环境下频繁使用性能要求高用饿汉避免资源竞争，提高响应速度
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

// 2.懒汉模式(延迟加载) -- 第一次使用实例对象时创建对象，进程启动无负载，多个单例对象的实例启动顺序可控 -- 复杂
class Lazy {
public:
	static Lazy& GetInstance() {
		if (!_psinst) {
			_psinst = new Lazy;
		}
		return *_psinst;
	}

	// 中途需要释放 or 程序结束时做一些行为(如持久化) -- 程序结束自动调用由内部public类程序结束使自动调析构实现
	static void DelInstance() {
		if (_psinst) {
			delete _psinst; 
			// new = operator new + 构造
			// delete = 析构(在析构中进行其余行为) + operator delete
			_psinst = nullptr;
		}
	}

	// 内部类是外部类的天然友元(外部不是内部的友元)，如果定义在外部则需要友元类来实现这个功能
	// 内部类可以直接访问外部的static或用外部的对象访问外部
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