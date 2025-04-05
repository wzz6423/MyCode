#include "SList.h"

//创建新节点
SLTNode* BuySLTNode(SLTDataType x) {
	SListNode* newnode = (SLTNode*)malloc(sizeof(SLTNode));
	assert(newnode);
	if (newnode == NULL) {
		perror("malloc");
		return NULL;
	}
	newnode->data = x;
	newnode->next = NULL;
	return newnode;
}

//打印
void SLTPrint(SLTNode* phead) {
	SLTNode* cur = phead;
	while (cur) {
		printf("%d->", cur->data);
		cur = cur->next;
	}
	printf("NULL\n");
}

//销毁-一级指针
void SLTDestroy_first(SLTNode* phead) {
	assert(phead);
	if (!phead) {
		return;
	}
	SLTNode* cur = phead;
	SLTNode* tmp = NULL;
	while (cur) {
		tmp = cur->next;
		free(cur);
		cur = tmp;
	}
	//交给用的人在调用该函数的函数内置空
}

//销毁-二级指针
void SLTDestroy_second(SLTNode** pphead) {
	assert(pphead);
	if (!pphead) {
		return;
	}
	assert(*pphead);
	if (!*pphead) {
		return;
	}
	SLTNode* cur = *pphead;
	SLTNode* tmp = NULL;
	while (cur) {
		tmp = cur->next;
		free(cur);
		cur = tmp;
	}
	*pphead = NULL;
}

//尾插
void SLTPushBack(SLTNode** pphead, SLTDataType x) {
	assert(pphead);
	if (!pphead) {
		return;
	}

	if (!*pphead) {
		*pphead = BuySLTNode(x);
	}
	else {
		SLTNode* tail = *pphead;
		while (tail->next) {
			tail= tail->next;
		}
		tail->next = BuySLTNode(x);
	}
}

//头插
void SLTPushFront(SLTNode** pphead, SLTDataType x) {
	assert(pphead);
	if (!pphead) {
		return;
	}

	SLTNode* tmp = *pphead;
	*pphead = BuySLTNode(x);
	(*pphead)->next = tmp;
}

//尾删
void SLTPopBack(SLTNode** pphead) {
	assert(pphead);
	if (!pphead) {
		return;
	}
	assert(*pphead);
	if (!*pphead) {
		return;
	}
	if (!((*pphead)->next)) {
		free(*pphead);
		*pphead = NULL;
	}
	else {
		SLTNode* tail = *pphead;
		while (tail->next->next) {
			tail = tail->next;
		}
		free(tail->next);
		tail->next = NULL;
	}
}

//头删
void SLTPopFront(SLTNode** pphead) {
	assert(pphead);
	if (!pphead) {
		return;
	}
	assert(*pphead);
	if (!*pphead) {
		return;
	}

	SLTNode* del = *pphead;
	*pphead = del->next;
	free(del);
	del = NULL;
}

//查找
SLTNode* SLTFind(SLTNode* phead, SLTDataType x) {
	assert(phead);
	if (!phead) {
		return NULL;
	}

	SLTNode* cur = phead;
	while (cur) {
		if (cur->data == x) {
			return cur;
		}

		cur = cur->next;
	}
	return NULL;
}

//中间插（前）
void SLTInsert(SLTNode** pphead, SLTNode* pos, SLTDataType x) {
	assert(pphead);
	if (!pphead) {
		return;
	}
	assert(*pphead);
	if (!*pphead) {
		return;
	}
	assert(pos);
	if (!pos) {
		return;
	}

	SLTNode* newnode = BuySLTNode(x);
	//1.需要pphead
	SLTNode* cur = *pphead;
	newnode->next = pos;
	while (cur->next != pos) {
		cur = cur->next;
	}
	cur->next = newnode;
	newnode = NULL;

	//2.不需要pphead
	//SLTDataType tmp = newnode->data;
	//newnode->data = pos->data;
	//pos->data = tmp;
	//newnode->next = pos->next;
	//pos->next = newnode;
	//newnode = NULL;
}

//中间删（前）
void SLTErase(SLTNode** pphead, SLTNode* pos) {
	assert(pphead);
	if (!pphead) {
		return;
	}
	assert(*pphead);
	if (!*pphead) {
		return;
	}
	assert(pos);
	if (!pos) {
		return;
	}

	//1.需要pphead
	SLTNode* cur = *pphead;
	while (cur->next != pos) {
		cur = cur->next;
	}
	cur->next = pos->next;
	free(pos);
	pos = NULL;

	//2.不需要pphead
	//SLTNode* del = pos->next;
	//pos->data = pos->next->data;
	//pos->next = pos->next->next;
	//free(del);
	//del = NULL;
}

//中间插（后）
void SLTInsertAfter(SLTNode* pos, SLTDataType x) {
	assert(pos);
	if (!pos) {
		return;
	}

	SLTNode* newnode = BuySLTNode(x);
	newnode->next = pos->next;
	pos->next = newnode;
	newnode = NULL;
}

//中间删（后）
void SLTEraseAfter(SLTNode* pos) {
	assert(pos);
	if (!pos) {
		return;
	}
	assert(pos->next);
	if (!pos->next) {
		return;
	}

	SLTNode* del = pos->next;
	pos->next = pos->next->next;
	free(del);
	del = NULL;
}