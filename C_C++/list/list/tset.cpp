#include "List.h"

void Test() {
	//≤‚ ‘≥ı ºªØ
	LTNode* plist = LTInit();

	//≤‚ ‘Õ∑≤Â
	LTPushFront(plist, 1);
	LTPushFront(plist, 2);
	LTPushFront(plist, 3);
	LTPushFront(plist, 4);
	LTPrint(plist);

	//≤‚ ‘Œ≤≤Â
	LTPushBack(plist, 1);
	LTPushBack(plist, 2);
	LTPushBack(plist, 3);
	LTPushBack(plist, 4);
	LTPrint(plist);

	//≤‚ ‘Õ∑…æ
	LTPopFront(plist);
	LTPopFront(plist);
	LTPrint(plist);

	//≤‚ ‘Œ≤…æ
	LTPopBack(plist);
	LTPopBack(plist);
	LTPrint(plist);

	//≤È’“
	LTNode* pos = LTFind(plist,2);
	 
	//≤‚ ‘≤Â»Î
	LTInsert(pos, 5);
	pos = NULL;
	LTPrint(plist);

	//≤‚ ‘…æ≥˝
	pos = LTFind(plist, 2);
	LTErase(pos);
	pos = NULL;
	LTPrint(plist);

	//≤‚ ‘…æ≥˝2
	LTRemove(plist, 2);
	LTPrint(plist);

	//≤‚ ‘œ˙ªŸ
	LTDestroy(plist);
}

int main() {
	Test();

	return 0;
}