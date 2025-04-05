#include "SeqList.h"

void Test() {
	SL s;
	//≤‚ ‘≥ı ºªØ
	SLInit(&s);
	
	//≤‚ ‘Õ∑≤Â”Î¿©»›”Î¥Ú”°
	SLPushFront(&s, 1);
	SLPushFront(&s, 2);
	SLPushFront(&s, 3);
	SLPushFront(&s, 4);
	SLPushFront(&s, 5);
	SLPrint(&s);

	//≤‚ ‘Œ≤≤Â
	SLPushBack(&s, 6);
	SLPushBack(&s, 7);
	SLPushBack(&s, 8);
	SLPrint(&s);

	//≤‚ ‘Õ∑…æ
	SLPopFront(&s);
	SLPopFront(&s);
	SLPrint(&s);

	//≤‚ ‘Œ≤…æ
	SLPopBack(&s);
	SLPopBack(&s);
	SLPopBack(&s);
	SLPopBack(&s);
	SLPopBack(&s);
	SLPopBack(&s);
	SLPopBack(&s);
	SLPopBack(&s);
	SLPrint(&s);

	//≤‚ ‘÷–º‰≤Â
	SLInsert(&s, 2, 20);
	SLInsert(&s, 2, 30);
	SLPrint(&s);
	 
	// ≤‚ ‘÷–º‰…æ
	SLErase(&s, 2);
	SLErase(&s, 2);
	SLPrint(&s);

	//≤‚ ‘≤È’“
	int num = 0;
	num = SLFind(&s, 3);
	printf("%d ", num);
	num = SLFind(&s, 2);
	printf("%d ", num);

	//≤‚ ‘œ˙ªŸ
	SLDestroy(&s);
}

int main() {
	Test();

	return 0;
}