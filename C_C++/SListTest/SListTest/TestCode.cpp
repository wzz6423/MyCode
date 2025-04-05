#include <stdlib.h>
typedef struct ListNode {
    int val;
    struct ListNode* next;
}ListNode;
int main() {
    ListNode* n1 = (ListNode*)malloc(sizeof(ListNode));
    ListNode* n2 = (ListNode*)malloc(sizeof(ListNode));
    ListNode* n3 = (ListNode*)malloc(sizeof(ListNode));
    ListNode* n4 = (ListNode*)malloc(sizeof(ListNode));

    n1->val = 1;
    n2->val = 2;
    n3->val = 3;
    n4->val = 4;

    n1->next = n2;
    n2->next = n3;
    n3->next = n4;
    n4->next = NULL;

    free(n1);
    free(n2);
    free(n3);
    free(n4);
    n1 = n2 = n3 = n4 = NULL;

    return 0;
}