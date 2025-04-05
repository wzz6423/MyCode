#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef char BTDataType;

typedef struct BinaryTreeNode
{
	BTDataType data;
	struct BinaryTreeNode* left;
	struct BinaryTreeNode* right;
}BTNode;

// ͨ��ǰ�����������"ABD##E#H##CF##G##"����������
BTNode* BTCreate(BTDataType* a, int n, int* pi);
// ����������
void BTDestory(BTNode* root);
// �������ڵ����
int BTSize(BTNode* root);
// ���������
size_t BTHeight(BTNode* root);
// ������Ҷ�ӽڵ����
int BTLeafSize(BTNode* root);
// ��������k��ڵ����
int BTLevelKSize(BTNode* root, int k);
// ����������ֵΪx�Ľڵ�
BTNode* BTFind(BTNode* root, BTDataType x);
// ������ǰ����� 
void BTPrevOrder(BTNode* root);
// �������������
void BTInOrder(BTNode* root);
// �������������
void BTPostOrder(BTNode* root);
// �������
void BTLevelOrder(BTNode* root);
// �ж϶������Ƿ�����ȫ������
bool BTComplete(BTNode* root);