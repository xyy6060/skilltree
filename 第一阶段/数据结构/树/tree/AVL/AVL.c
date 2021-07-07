#include <stdio.h>
#include <stdlib.h>
#include "SqQueue.h"
#define  max(a,b) ((a) > (b) ? (a) : (b)) 
typedef int TElemType;	
struct BiTNode
{
	TElemType data;
	struct BiTNode *lchild,*rchild;//左右孩子节点的指针
	int height;//高度
};

/*
	per_order:先序遍历一棵树
*/
void per_order(struct BiTNode*t)
{
	if(t == NULL)
		return ;
	printf("%c ",t->data);
	
	per_order(t->lchild);
	
	per_order(t->rchild);
	
}
/*
	mid_order:中序遍历一棵树
*/
void mid_order(struct BiTNode*t)
{
	if(t == NULL)
		return ;
	
	mid_order(t->lchild);
	
	printf("%c ",t->data);
	
	mid_order(t->rchild);
}

/*
	post_order:后序遍历一棵树
*/
void post_order(struct BiTNode*t)
{
	if(t == NULL)
		return ;
	
	post_order(t->lchild);
	
	post_order(t->rchild);
	
	printf("%c ",t->data);
}
/*
	H:求一棵树的高度
*/
int H(struct BiTNode*t)
{
	if(t == NULL)
		return 0;
	
	return t->height;
}
/*
	Level：层次遍历一棵树
*/
void Level(struct BiTNode*t)
{
	if(t == NULL)
		return ;
	
	//初始化一个队列
	struct SqQueue*s = InitQueue(100);
	
	//根节点入队
	EnQueue(s,(void*)t);//void*
	
	while(!IsEmptyQueue(s))
	{
		struct BiTNode*r = NULL;
		//出队
		DeQueue(s,(void**)&r);//void**
		printf("%c ",r->data);
		
		//让出队元素的左右孩子节点入队
		if(r->lchild)
		{
			EnQueue(s,r->lchild);
		}
		
		if(r->rchild)
		{
			EnQueue(s,r->rchild);
		}
	}
	
	printf("\n");
	
	DestroyQueue(s);
}
/*
	SingleRotateWithLeft：对以k2为根节点的二叉树作单向左旋平衡处理
	
	返回平衡处理后新的二叉树的根节点
*/
struct BiTNode*SingleRotateWithLeft(struct BiTNode*k2)
{
	struct BiTNode*k1 = k2->rchild;
	k2->rchild = k1->lchild;
	k1->lchild = k2;
	
	//重新计算高度
	k2->height = max(H(k2->lchild),H(k2->rchild))+1;
	k1->height = max(H(k1->lchild),H(k1->rchild))+1;
	
	return k1;
}
/*
	SingleRotateWithRight：对以k2为根节点的二叉树作单向右旋平衡处理
	
	返回平衡处理后新的二叉树的根节点
*/
struct BiTNode*SingleRotateWithRight(struct BiTNode*k2)
{
	struct BiTNode*k1 = k2->lchild;
	k2->lchild = k1->rchild;
	k1->rchild = k2;
	
	//重新计算高度
	k2->height = max(H(k2->lchild),H(k2->rchild))+1;
	k1->height = max(H(k1->lchild),H(k1->rchild))+1;
	
	return k1;
}
/*
	DoubleRotateLeftRight:对以k2为根节点的二叉树作双向旋转（先左后右）
		先左：先把不平衡节点的左子节点作  单向左
		后右：在对不平衡节点    单向右
	返回平衡处理后新的二叉树的根节点
	
*/
struct BiTNode*DoubleRotateLeftRight(struct BiTNode*k2)
{
	 k2->lchild = SingleRotateWithLeft(k2->lchild);
	 
	 return SingleRotateWithRight(k2);
	 
}

/*
	DoubleRotateRightLeft:对以k2为根节点的二叉树作双向旋转（先右后左）
		先右：先把不平衡节点的右子节点作  单向右
		后左：在对不平衡节点    单向左
	返回平衡处理后新的二叉树的根节点
	
*/
struct BiTNode*DoubleRotateRightLeft(struct BiTNode*k2)
{
	  k2->rchild = SingleRotateWithRight(k2->rchild);
	 
	 return SingleRotateWithLeft(k2);
	 
}

/*
	Insert_node:在t指向的平衡的二叉排序树上，加入一个元素d,并且保持其排序性和平衡性
	返回二叉树的根节点的地址
*/
struct BiTNode*Insert_node(struct BiTNode*t,TElemType d)
{
	struct BiTNode*p = NULL;
	//step1：创建一个新的树的数据节点并赋值
	struct BiTNode*pnew = malloc(sizeof(struct BiTNode));
	pnew->data = d;
	pnew->lchild = pnew->rchild = NULL;
	pnew->height = 1;//注意  为什么是1
	
	//step2:判断是否为空树
	if(t == NULL)
	{
		return pnew;
	}
	
	//step3:找插入位置
	if(d > t->data)
	{
		t->rchild = Insert_node(t->rchild,d);
		t->height = max(H(t->lchild),H(t->rchild))+1;
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		if(H(t->rchild) - H(t->lchild) > 1)//不平衡
		{
			//右边的右边
			if(d > t->rchild->data)
			{
				return SingleRotateWithLeft(t);
			}
			else if(d < t->rchild->data)//右边的左边
			{
				return DoubleRotateRightLeft(t);
			}
		}
	}
	else if(d < t->data)
	{
		t->lchild = Insert_node(t->lchild,d);
		t->height = max(H(t->lchild),H(t->rchild))+1;
		
		if(H(t->lchild) - H(t->rchild) > 1)//不平衡
		{
			//左边的左边
			if(d < t->lchild->data)
			{
				return SingleRotateWithRight(t);
			}
			else if(d > t->lchild->data)//左边的右边
			{
				return DoubleRotateLeftRight(t);
			}
		}
	}
	
	return t;
}

/*
	Create_Sort_BiTree:创建一棵平衡的二叉排序树
*/
struct BiTNode*Create_Sort_BiTree(char *s)
{
	struct BiTNode*root = NULL;
	while(*s)
	{
		root = Insert_node(root,*s);
		s++;
	}
	
	return root;
}


int main()
{
	struct BiTNode*t = Create_Sort_BiTree("EBADCFHGIKJ");
	printf("per_order:\n");
	per_order(t);
	printf("\n");
	
	printf("mid_order:\n");
	mid_order(t);
	printf("\n");
	
	printf("post_order:\n");
	post_order(t);
	printf("\n");
	
	printf("H = %d\n",H(t));
}