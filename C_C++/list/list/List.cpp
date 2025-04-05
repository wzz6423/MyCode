#include "List.h"

//为尽量保持接口的一致性在销毁等时候不传二级指针，同样也类似于库函数中的free，让调用该函数的函数中自行对该指针置空

//创建新节点
LTNode* BuyListNode(LTDataType x) {
	LTNode* newnode = (LTNode*)malloc(sizeof(LTNode));
	assert(newnode);
	if (!newnode) {
		perror("malloc");
		return NULL;
	}
	newnode->data = x;
	newnode->next = newnode->prev = NULL;
	return newnode;
}

//初始化头
LTNode* LTInit() {
	LTNode* phead = BuyListNode(-1);
	phead->next = phead->prev = phead;
	return phead;
}

//销毁
void LTDestroy(LTNode* phead) {
	assert(phead);
	if (!phead) {
		return;
	}

	LTNode* cur = phead->next;
	LTNode* next = NULL;
	while (cur != phead) {
		next = cur->next;
		free(cur);
		cur = next;
	}
	free(phead);
}

//检查该链表是否为空
bool LTEmpty(LTNode* phead) {
	assert(phead);

	return phead->next == phead;
}

//打印链表
void LTPrint(LTNode* phead) {
	assert(phead);
	if (!phead) {
		return;
	}

	LTNode* cur = phead->next;
	printf("<=head=>");
	while (cur != phead) {
		printf("%d=>", cur->data);
		cur = cur->next;
	}
	printf("\n");
}

//查找
LTNode* LTFind(LTNode* phead, LTDataType x) {
	assert(phead);
	if (!phead) {
		return NULL;
	}

	LTNode* cur = phead->next;
	while (cur != phead) {
		if (cur->data == x) {
			return cur;
		}
		cur = cur->next;
	}
	return NULL;
}

//插入
void LTInsert(LTNode* pos, LTDataType x) {
	assert(pos);
	if (!pos) {
		return;
	}

	LTNode* newnode = BuyListNode(x);
	newnode->prev = pos->prev;
	newnode->prev->next = newnode;
	newnode->next = pos;
	pos->prev = newnode;
}

//删除
void LTErase(LTNode* pos) {
	assert(pos);
	if (!pos) {
		return;
	}

	pos->next->prev = pos->prev;
	pos->prev->next = pos->next;
	free(pos);
}

//删除2（find +	Erase）
void LTRemove(LTNode* phead, LTDataType x) {
	assert(phead);
	if (!phead) {
		return;
	}

	LTNode* pos = LTFind(phead, x);
	LTErase(pos);
}

//尾插
void LTPushBack(LTNode* phead, LTDataType x) {
	assert(phead);
	if (!phead) {
		return;
	}
	//1.用前面的
	//LTInsert(phead, x);
	
	//2.再写一个
	LTNode* newnode = BuyListNode(x);
	newnode->prev = phead->prev;
	phead->prev->next = newnode;
	phead->prev = newnode;
	newnode->next = phead;
}

//尾删
void LTPopBack(LTNode* phead) {
	assert(phead);
	if (!phead) {
		return;
	}
	assert(!LTEmpty(phead));
	//1.用前面的
	//LTErase(phead->prev);

	//2.再写一个
	LTNode* del = phead->prev;
	phead->prev->prev->next = phead;
	phead->prev = phead->prev->prev;
	free(del);
	del = NULL;
}

//头插
void LTPushFront(LTNode* phead, LTDataType x) {
	assert(phead);
	if (!phead) {
		return;
	}
	//1.用前面的
	//LTInsert(phead->next, x);

	//2.再写一个
	LTNode* newnode = BuyListNode(x);
	phead->next->prev = newnode;
	newnode->next = phead->next;
	newnode->prev = phead;
	phead->next = newnode;
}

//头删
void LTPopFront(LTNode* phead) {
	assert(phead);
	if (!phead) {
		return;
	}
	assert(!LTEmpty(phead));
	//1.用前面的
	//LTErase(phead->next);

	//2.再写一个
	LTNode* del = phead->next;
	phead->next->next->prev = phead;
	phead->next = phead->next->next;
	free(del);
}