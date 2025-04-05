#include "SList.h"

void Test() {
	SLTNode* plist = NULL;

	//≤‚ ‘Œ≤≤Â
	SLTPushBack(&plist, 1);
	SLTPushBack(&plist, 2);
	SLTPushBack(&plist, 3);
	SLTPushBack(&plist, 4);
	SLTPrint(plist);
	
	//≤‚ ‘Õ∑≤Â
	SLTPushFront(&plist, 4);
	SLTPushFront(&plist, 3);
	SLTPushFront(&plist, 2);
	SLTPushFront(&plist, 1);
	SLTPrint(plist);

	//≤‚ ‘Œ≤…æ
	SLTPopBack(&plist);
	SLTPopBack(&plist);
	SLTPrint(plist);

	//≤‚ ‘Õ∑…æ
	SLTPopFront(&plist);
	SLTPopFront(&plist);
	SLTPrint(plist);

	//≤‚ ‘÷–º‰≤Â£®«∞£©
	SLTNode* cur = SLTFind(plist, 2);
	assert(cur);
	SLTInsert(&plist, cur, 10);
	cur = NULL;
	SLTPrint(plist);

	//≤‚ ‘÷–º‰…æ£®«∞£©
	cur = SLTFind(plist, 2);
	assert(cur);
	SLTErase(&plist, cur);
	cur = NULL;
	SLTPrint(plist);
	
	//≤‚ ‘÷–º‰≤Â£®∫Û£©
	cur = SLTFind(plist, 4);
	assert(cur);
	SLTInsertAfter(cur, 20);
	cur = NULL;
	SLTPrint(plist);
	
	//≤‚ ‘÷–º‰…æ£®∫Û£©
	cur = SLTFind(plist, 1);
	assert(cur);
	SLTEraseAfter(cur);
	cur = NULL;
	SLTPrint(plist);

	//≤‚ ‘œ˙ªŸ
	SLTDestroy_second(&plist);
}

int main() {
	Test();

	return 0;
}