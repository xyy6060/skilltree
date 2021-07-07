#ifndef _SQSTACK_H_
#define _SQSTACK_H_
#include <stdlib.h>
typedef char ElemType;
#define MAX 100

struct SqStack	
{
	ElemType data[MAX];
	int top;//用来表示栈顶元素的下标
			//top == -1表示栈中没有元素
};

struct SqStack*InitStack();
void DestroyStack(struct SqStack*s);
void ClearStack(struct SqStack*s) ;
int IsEmptyStack(struct SqStack*s);
int LengthStack(struct SqStack*s);
void GetTop(struct SqStack*s,ElemType* d);
void Pop(struct SqStack*s,ElemType* d);
void Push(struct SqStack*s,ElemType d);
#endif