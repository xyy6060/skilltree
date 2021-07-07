#ifndef _LINKEDSTACK_H_
#define _LINKEDSTACK_H_
#include <stdlib.h>
typedef char ElemType;
struct node
{
	ElemType data;
	struct node*next;
	struct node*prev;
};
struct LinkedStack	
{
	struct node*bottom;//栈底
	struct node*top;//栈顶
	int num;
	//...
};

struct LinkedStack*InitStack();
void DestroyStack(struct LinkedStack*s);
void ClearStack(struct LinkedStack*s) ;
int IsEmptyStack(struct LinkedStack*s);
int LengthStack(struct LinkedStack*s);
void GetTop(struct LinkedStack*s,ElemType* d);
void Pop(struct LinkedStack*s,ElemType* d);
void Push(struct LinkedStack*s,ElemType d);
#endif