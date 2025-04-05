#include "BTNode.h"
#include "Queue.h"

// ͨ��ǰ�����������"ABD##E#H##CF##G##"����������
BTNode* BTCreate(BTDataType* a, int n, int* pi) {
	assert(a);
	if (!a) {
		return NULL;
	}

	if (n == *pi) {
		return NULL;
	}

	if (a[*pi] == '#') {
		(*pi)++;
		return NULL;
	}

	BTNode* newnode = (BTNode*)malloc(sizeof(BTNode));
	assert(newnode);
	if (!newnode) {
		return NULL;
	}
	newnode->data = a[(*pi)++];
	newnode->left = BTCreate(a, n, pi);
	newnode->right = BTCreate(a, n, pi);
	return newnode;
}

// ����������
void BTDestory(BTNode* root) {
	if (!root) {
		return;
	}

	BTDestory(root->left);
	BTDestory(root->right);
	free(root);
}

// �������ڵ����
int BTSize(BTNode* root) {
	return !root ? 0 : BTSize(root->left) + BTSize(root->right) + 1;
}

// ���������
size_t BTHeight(BTNode* root) {
	if (!root) {
		return 0;
	}

	size_t lheight = BTHeight(root->left) + 1;
	size_t rheight = BTHeight(root->right) + 1;

	return lheight > rheight ? lheight : rheight;
}

// ������Ҷ�ӽڵ����
int BTLeafSize(BTNode* root) {
	assert(root);
	if (!root) {
		return 1;
	}
	
	return BTLeafSize(root->left) + BTLeafSize(root->right);
}

// ��������k��ڵ����
int BTLevelKSize(BTNode* root, int k) {	
	if (!root) {
		return 0;
	}

	if (k == 1) {
		return 1;
	}

	return BTLevelKSize(root->left, k - 1) + BTLevelKSize(root->right, k - 1);
}

// ����������ֵΪx�Ľڵ�
BTNode* BTFind(BTNode* root, BTDataType x) {
	if (!root) {
		return NULL;
	}

	if (root->data == x) {
		return root;
	}

	BTNode* lret = BTFind(root->left, x);
	if (lret) {
		return lret;
	}

	BTNode* rret = BTFind(root->right, x);
	if(rret) {
		return rret;
	}

	return NULL;
}

// ������ǰ����� 
void BTPrevOrder(BTNode* root) {
	assert(root);
	if (!root) {
		return;
	}

	if (!root) {
		return;
	}

	printf("%c ", root->data);
	BTPrevOrder(root->left);
	BTPrevOrder(root->right);
}

// �������������
void BTInOrder(BTNode* root) {
	if (!root) {
		return;
	}

	BTInOrder(root->left);
	printf("%c ", root->data);
	BTInOrder(root->right);
}

// �������������
void BTPostOrder(BTNode* root) {
	if (!root) {
		return;
	}

	BTPostOrder(root->left);
	BTPostOrder(root->right);
	printf("%c ", root->data);
}

// �������
void BTLevelOrder(BTNode* root) {
	if (!root) {
		return;
	}

	Queue BT;
	QueueInit(&BT);
	QueuePush(&BT, root);
	
	while (!QueueEmpty(&BT)) {
		BTNode* tmp = QueueFront(&BT);
		QueuePop(&BT);

		if (root->left) {
			QueuePush(&BT, root->left);
		}

		if (root->right) {
			QueuePush(&BT, root->right);
		}
	}

	QueueDestroy(&BT);
}

// �ж϶������Ƿ�����ȫ������
bool BTComplete(BTNode* root) {
	assert(root);

	Queue BT;
	QueueInit(&BT);
	QueuePush(&BT, root);
	
	while (!QueueEmpty(&BT)) {
		BTNode* tmp = QueueFront(&BT);
		QueuePop(&BT);

		if (root->left) {
			QueuePush(&BT, root->left);
		}

		if (root->right) {
			QueuePush(&BT, root->right);
		}
	}

	while (!QueueEmpty(&BT)) {
		BTNode* tmp = QueueFront(&BT);
		QueuePop(&BT);

		if (tmp) {
			QueueDestroy(&BT);
			return false;
		}
	}

	QueueDestroy(&BT);
	return true;
}