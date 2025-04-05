#include "SList.h"

void Test() {
	SLTNode* plist = NULL;

	//����β��
	SLTPushBack(&plist, 1);
	SLTPushBack(&plist, 2);
	SLTPushBack(&plist, 3);
	SLTPushBack(&plist, 4);
	SLTPrint(plist);
	
	//����ͷ��
	SLTPushFront(&plist, 4);
	SLTPushFront(&plist, 3);
	SLTPushFront(&plist, 2);
	SLTPushFront(&plist, 1);
	SLTPrint(plist);

	//����βɾ
	SLTPopBack(&plist);
	SLTPopBack(&plist);
	SLTPrint(plist);

	//����ͷɾ
	SLTPopFront(&plist);
	SLTPopFront(&plist);
	SLTPrint(plist);

	//�����м�壨ǰ��
	SLTNode* cur = SLTFind(plist, 2);
	assert(cur);
	SLTInsert(&plist, cur, 10);
	cur = NULL;
	SLTPrint(plist);

	//�����м�ɾ��ǰ��
	cur = SLTFind(plist, 2);
	assert(cur);
	SLTErase(&plist, cur);
	cur = NULL;
	SLTPrint(plist);
	
	//�����м�壨��
	cur = SLTFind(plist, 4);
	assert(cur);
	SLTInsertAfter(cur, 20);
	cur = NULL;
	SLTPrint(plist);
	
	//�����м�ɾ����
	cur = SLTFind(plist, 1);
	assert(cur);
	SLTEraseAfter(cur);
	cur = NULL;
	SLTPrint(plist);

	//��������
	SLTDestroy_second(&plist);
}

int main() {
	Test();

	return 0;
}