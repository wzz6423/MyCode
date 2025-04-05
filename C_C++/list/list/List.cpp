#include "List.h"

//Ϊ�������ֽӿڵ�һ���������ٵ�ʱ�򲻴�����ָ�룬ͬ��Ҳ�����ڿ⺯���е�free���õ��øú����ĺ��������жԸ�ָ���ÿ�

//�����½ڵ�
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

//��ʼ��ͷ
LTNode* LTInit() {
	LTNode* phead = BuyListNode(-1);
	phead->next = phead->prev = phead;
	return phead;
}

//����
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

//���������Ƿ�Ϊ��
bool LTEmpty(LTNode* phead) {
	assert(phead);

	return phead->next == phead;
}

//��ӡ����
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

//����
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

//����
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

//ɾ��
void LTErase(LTNode* pos) {
	assert(pos);
	if (!pos) {
		return;
	}

	pos->next->prev = pos->prev;
	pos->prev->next = pos->next;
	free(pos);
}

//ɾ��2��find +	Erase��
void LTRemove(LTNode* phead, LTDataType x) {
	assert(phead);
	if (!phead) {
		return;
	}

	LTNode* pos = LTFind(phead, x);
	LTErase(pos);
}

//β��
void LTPushBack(LTNode* phead, LTDataType x) {
	assert(phead);
	if (!phead) {
		return;
	}
	//1.��ǰ���
	//LTInsert(phead, x);
	
	//2.��дһ��
	LTNode* newnode = BuyListNode(x);
	newnode->prev = phead->prev;
	phead->prev->next = newnode;
	phead->prev = newnode;
	newnode->next = phead;
}

//βɾ
void LTPopBack(LTNode* phead) {
	assert(phead);
	if (!phead) {
		return;
	}
	assert(!LTEmpty(phead));
	//1.��ǰ���
	//LTErase(phead->prev);

	//2.��дһ��
	LTNode* del = phead->prev;
	phead->prev->prev->next = phead;
	phead->prev = phead->prev->prev;
	free(del);
	del = NULL;
}

//ͷ��
void LTPushFront(LTNode* phead, LTDataType x) {
	assert(phead);
	if (!phead) {
		return;
	}
	//1.��ǰ���
	//LTInsert(phead->next, x);

	//2.��дһ��
	LTNode* newnode = BuyListNode(x);
	phead->next->prev = newnode;
	newnode->next = phead->next;
	newnode->prev = phead;
	phead->next = newnode;
}

//ͷɾ
void LTPopFront(LTNode* phead) {
	assert(phead);
	if (!phead) {
		return;
	}
	assert(!LTEmpty(phead));
	//1.��ǰ���
	//LTErase(phead->next);

	//2.��дһ��
	LTNode* del = phead->next;
	phead->next->next->prev = phead;
	phead->next = phead->next->next;
	free(del);
}