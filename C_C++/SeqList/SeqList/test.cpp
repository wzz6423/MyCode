#include "SeqList.h"

void Test() {
	SL s;
	//���Գ�ʼ��
	SLInit(&s);
	
	//����ͷ�����������ӡ
	SLPushFront(&s, 1);
	SLPushFront(&s, 2);
	SLPushFront(&s, 3);
	SLPushFront(&s, 4);
	SLPushFront(&s, 5);
	SLPrint(&s);

	//����β��
	SLPushBack(&s, 6);
	SLPushBack(&s, 7);
	SLPushBack(&s, 8);
	SLPrint(&s);

	//����ͷɾ
	SLPopFront(&s);
	SLPopFront(&s);
	SLPrint(&s);

	//����βɾ
	SLPopBack(&s);
	SLPopBack(&s);
	SLPopBack(&s);
	SLPopBack(&s);
	SLPopBack(&s);
	SLPopBack(&s);
	SLPopBack(&s);
	SLPopBack(&s);
	SLPrint(&s);

	//�����м��
	SLInsert(&s, 2, 20);
	SLInsert(&s, 2, 30);
	SLPrint(&s);
	 
	// �����м�ɾ
	SLErase(&s, 2);
	SLErase(&s, 2);
	SLPrint(&s);

	//���Բ���
	int num = 0;
	num = SLFind(&s, 3);
	printf("%d ", num);
	num = SLFind(&s, 2);
	printf("%d ", num);

	//��������
	SLDestroy(&s);
}

int main() {
	Test();

	return 0;
}