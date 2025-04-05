#include "List.h"

void Test() {
	//���Գ�ʼ��
	LTNode* plist = LTInit();

	//����ͷ��
	LTPushFront(plist, 1);
	LTPushFront(plist, 2);
	LTPushFront(plist, 3);
	LTPushFront(plist, 4);
	LTPrint(plist);

	//����β��
	LTPushBack(plist, 1);
	LTPushBack(plist, 2);
	LTPushBack(plist, 3);
	LTPushBack(plist, 4);
	LTPrint(plist);

	//����ͷɾ
	LTPopFront(plist);
	LTPopFront(plist);
	LTPrint(plist);

	//����βɾ
	LTPopBack(plist);
	LTPopBack(plist);
	LTPrint(plist);

	//����
	LTNode* pos = LTFind(plist,2);
	 
	//���Բ���
	LTInsert(pos, 5);
	pos = NULL;
	LTPrint(plist);

	//����ɾ��
	pos = LTFind(plist, 2);
	LTErase(pos);
	pos = NULL;
	LTPrint(plist);

	//����ɾ��2
	LTRemove(plist, 2);
	LTPrint(plist);

	//��������
	LTDestroy(plist);
}

int main() {
	Test();

	return 0;
}