#include "SqStack.h"

/*
	InitStack() : 初始化一个栈
*/
struct SqStack*InitStack()
{
	struct SqStack*s = malloc(sizeof(struct SqStack));
	s->top = -1;//用-1表示是一个空栈
	
	return s;
}
/*
	DestroyStack() :销毁一个栈
*/
void DestroyStack(struct SqStack*s)
{
	ClearStack(s);
	free(s);
}
/*
	ClearStack()  :清空一个栈
*/
void ClearStack(struct SqStack*s) 
{
	s->top = -1;
}
/*
	IsEmptyStack() :判断是否为空栈
	返回值：
			1  =》为空
			0 =》不为空
*/
int IsEmptyStack(struct SqStack*s)
{
	return s->top == -1 ? 1 : 0;
}

/*
	LengthStack() :栈的长度，返回的栈的元素个数
*/
int LengthStack(struct SqStack*s)
{
	return s->top+1;
}

/*
	GetTop()  :返回栈顶元素，但是不出栈
*/
void GetTop(struct SqStack*s,ElemType* d)
{
	if(IsEmptyStack(s))
	{
		return ;
	}
	*d = s->data[s->top];
}

/*
	Pop()   :出栈
*/
void Pop(struct SqStack*s,ElemType* d)
{
	if(IsEmptyStack(s))
	{
		return ;
	}
	*d = s->data[s->top];
	s->top--;
}
/*
	Push() :入栈
*/
void Push(struct SqStack*s,ElemType d)
{
	if(s->top == MAX-1)
	{
		return ;
	}
	
	s->data[++s->top] = d;
}



