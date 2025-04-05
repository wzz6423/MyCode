#include "SList.h"

//�����½ڵ�
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

//��ӡ
void SLTPrint(SLTNode* phead) {
	SLTNode* cur = phead;
	while (cur) {
		printf("%d->", cur->data);
		cur = cur->next;
	}
	printf("NULL\n");
}

//����-һ��ָ��
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
	//�����õ����ڵ��øú����ĺ������ÿ�
}

//����-����ָ��
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

//β��
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

//ͷ��
void SLTPushFront(SLTNode** pphead, SLTDataType x) {
	assert(pphead);
	if (!pphead) {
		return;
	}

	SLTNode* tmp = *pphead;
	*pphead = BuySLTNode(x);
	(*pphead)->next = tmp;
}

//βɾ
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

//ͷɾ
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

//����
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

//�м�壨ǰ��
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
	//1.��Ҫpphead
	SLTNode* cur = *pphead;
	newnode->next = pos;
	while (cur->next != pos) {
		cur = cur->next;
	}
	cur->next = newnode;
	newnode = NULL;

	//2.����Ҫpphead
	//SLTDataType tmp = newnode->data;
	//newnode->data = pos->data;
	//pos->data = tmp;
	//newnode->next = pos->next;
	//pos->next = newnode;
	//newnode = NULL;
}

//�м�ɾ��ǰ��
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

	//1.��Ҫpphead
	SLTNode* cur = *pphead;
	while (cur->next != pos) {
		cur = cur->next;
	}
	cur->next = pos->next;
	free(pos);
	pos = NULL;

	//2.����Ҫpphead
	//SLTNode* del = pos->next;
	//pos->data = pos->next->data;
	//pos->next = pos->next->next;
	//free(del);
	//del = NULL;
}

//�м�壨��
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

//�м�ɾ����
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