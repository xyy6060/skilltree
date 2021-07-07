#include "LinkedStack.h"

/*
	InitStack() : 初始化一个栈
*/
struct LinkedStack*InitStack()
{
	struct LinkedStack*s = malloc(sizeof(*s));
	s->top = s->bottom = NULL;
	s->num = 0;
	
	return s;
}
/*
	DestroyStack() :销毁一个栈
*/
void DestroyStack(struct LinkedStack*s)
{
	ClearStack(s);
	free(s);
}
/*
	ClearStack()  :清空一个栈
*/
void ClearStack(struct LinkedStack*s) 
{
	struct node*p = s->bottom;//从栈底开始删除数据节点
	while(p)
	{
		s->bottom = s->bottom->next;
		if(s->bottom)
		{
			s->bottom->prev = NULL;
		}		
		p->next = NULL;
		free(p);
		p = s->bottom;
	}
	
	s->top = NULL;
	s->num = 0;
}
/*
	IsEmptyStack() :判断是否为空栈
	返回值：
			1  =》为空
			0 =》不为空
*/
int IsEmptyStack(struct LinkedStack*s)
{
	return s->num == 0 ? 1 : 0;
}

/*
	LengthStack() :栈的长度，返回的栈的元素个数
*/
int LengthStack(struct LinkedStack*s)
{
	return s->num;
}

/*
	GetTop()  :返回栈顶元素，但是不出栈
*/
void GetTop(struct LinkedStack*s,ElemType* d)
{
	if(IsEmptyStack(s))
	{
		return ;
	}
	
	*d = s->top->data;
	
}

/*
	Pop() :出栈
*/
void Pop(struct LinkedStack*s,ElemType* d)
{
	if(IsEmptyStack(s))
	{
		return ;
	}
	
	struct node*p = s->top;
	*d = s->top->data;
	s->top = s->top->prev;
	if(s->top)
		s->top->next = NULL;
	p->prev = NULL;
	free(p);
	p = NULL;
	
	s->num--;
	
}
/*
	Push() :入栈
*/
void Push(struct LinkedStack*s,ElemType d)
{
	struct node*pnew = malloc(sizeof(*pnew));
	pnew->next = pnew->prev = NULL;
	pnew->data = d;
	
	if(s->top == NULL)
	{
		s->top = pnew;
		s->bottom = pnew;
	}
	else
	{
		s->top->next = pnew;
		pnew->prev = s->top;
		s->top = pnew;
	}
	s->num++;
}



