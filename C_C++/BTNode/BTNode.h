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

// 通过前序遍历的数组"ABD##E#H##CF##G##"构建二叉树
BTNode* BTCreate(BTDataType* a, int n, int* pi);
// 二叉树销毁
void BTDestory(BTNode* root);
// 二叉树节点个数
int BTSize(BTNode* root);
// 二叉树深度
size_t BTHeight(BTNode* root);
// 二叉树叶子节点个数
int BTLeafSize(BTNode* root);
// 二叉树第k层节点个数
int BTLevelKSize(BTNode* root, int k);
// 二叉树查找值为x的节点
BTNode* BTFind(BTNode* root, BTDataType x);
// 二叉树前序遍历 
void BTPrevOrder(BTNode* root);
// 二叉树中序遍历
void BTInOrder(BTNode* root);
// 二叉树后序遍历
void BTPostOrder(BTNode* root);
// 层序遍历
void BTLevelOrder(BTNode* root);
// 判断二叉树是否是完全二叉树
bool BTComplete(BTNode* root);