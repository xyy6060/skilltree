#include <stdio.h>
#include <stdlib.h>
#include "SqQueue.h"
typedef int TElemType;	
struct BiTNode
{
	TElemType data;
	struct BiTNode *lchild,*rchild;//左右孩子节点的指针
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
	
	int l = H(t->lchild);
	int r = H(t->rchild);
	
	return l > r ?l+1 :r+1;
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
	Insert_node:在t指向的二叉排序树上，加入一个元素d,并且保持性排序性
	返回二叉树的根节点的地址
*/
struct BiTNode*Insert_node(struct BiTNode*t,TElemType d)
{
	struct BiTNode*p = NULL;
	//step1：创建一个新的树的数据节点并赋值
	struct BiTNode*pnew = malloc(sizeof(struct BiTNode));
	pnew->data = d;
	pnew->lchild = pnew->rchild = NULL;
	
	//step2:判断是否为空树
	if(t == NULL)
	{
		return pnew;
	}
	
	//step3:找插入位置
	p = t;
	while(1)
	{
		if(pnew->data > p->data)
		{
			//待插入的节点的值要比p大，如果此时p->rchild为空
			//那么pnew就是p的右孩子节点
			//否则	往右边走
			if(p->rchild == NULL)
			{
				p->rchild = pnew;
				break;
			}
			p = p->rchild;
		}
		else if(pnew->data < p->data)
		{
			//待插入的节点的值要比p小，如果此时p->lchild为空
			//那么pnew就是p的左孩子节点
			//否则	往左边走
			if(p->lchild == NULL)
			{
				p->lchild = pnew;
				break;
			}
			p = p->lchild;
		}
		else
		{
			free(pnew);
			break;
		}
	}
	
	
	return t;
}
/*
	delete_x_node:在t指向的二叉排序树上，删除值为x的节点
	并且保持排序性
	
	返回值：删除后的二叉排序树的根节点
*/
struct BiTNode*delete_x_node(struct BiTNode*t,TElemType x)
{
		
	struct BiTNode*p = t;//指向要删除的那个节点，遍历指针
	struct BiTNode*pr = NULL;//指向要删除的那个节点的父节点，遍历指针
	//step1：查找要删除的节点
	while(p)
	{
		if(p->data == x)
			break;
		pr = p;
		if(p->data > x)
		{
			p = p->lchild;
		}
		else
		{
			p = p->rchild;
		}
	}
	if(p == NULL)
	{
		return t;
	}
	
	//step2:删除
delete:
	if(p->lchild == NULL && p->rchild == NULL)
	{
		//要删除的是一个叶子节点
		
		if(p == t)
		{
			//整棵树就只有一个节点
			free(p);
			return NULL;
		}
		else
		{
			if(p == pr->lchild)
			{
				pr->lchild = NULL;
			}
			else
			{
				pr->rchild = NULL;
			}
			free(p);
			return t;
		}
		
	}
	
	if(p->lchild != NULL && p->rchild == NULL)//p只有一个左子树
	{
		if(p == t)
		{
			t = p->lchild;
			p->lchild = NULL;
			free(p);
			return t;
		}
		
		if(p == pr->lchild)
		{
			pr->lchild = p->lchild;
			p->lchild = NULL;
		}
		else
		{
			pr->rchild = p->lchild;
			p->lchild = NULL;
		}
		free(p);
		return t;
	}
	
	if(p->rchild != NULL && p->lchild == NULL)//p只有一个右子树
	{
		if(p == t)
		{
			t = p->rchild;
			p->rchild = NULL;
			//free(p);
			//return t;
		}
		
		if(p == pr->lchild)
		{
			pr->lchild = p->rchild;
			p->rchild = NULL;
		}
		else
		{
			pr->rchild = p->rchild;
			p->rchild = NULL;
		}
		free(p);
		return t;
	}
	
	if(p->rchild != NULL && p->lchild != NULL)//p有两个孩子节点
	{
		struct BiTNode*px = p;
		pr = p;
		p = p->lchild;//去往左子树中找最大
		
		while(p->rchild)
		{
			pr = p;
			p = p->rchild;
		}
		px->data = p->data;
		
		goto delete;
	}
}
/*
	Create_Sort_BiTree:创建一棵二叉排序树
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
	Level(t);
	return 0;
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
	
	t = delete_x_node(t,'H');
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