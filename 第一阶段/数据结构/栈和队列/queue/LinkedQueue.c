#include "LinkedQueue.h"

/*
	InitQueue() : 初始化一个队列
*/
struct LinkedQueue*InitQueue()
{
	struct LinkedQueue*s = malloc(sizeof(*s));
	s->front = s->rear = NULL;
	s->num = 0;
	
	return s;
}
/*
	IsEmptyQueue() :判断是否为空队列
	是 返回1
	不是 返回0
*/
int IsEmptyQueue(struct LinkedQueue*s)
{
	return s->num == 0  ? 1 : 0;
}
/*
	DestroyQueue() :销毁一个队列
*/
void DestroyQueue(struct LinkedQueue*s)
{
	ClearQueue(s);
	free(s);
}
/*
	ClearQueue()  :清空一个队列
*/
void ClearQueue(struct LinkedQueue*s)
{
	if(IsEmptyQueue(s))
		return ;
	
	struct node*p = s->front;
	while(p)
	{
		s->front = s->front->next;
		p->next = NULL;
		free(p);
		p = s->front;
	}
	
	s->rear = NULL;
	s->num = 0;
	
}
/*
	LengthQueue() :队列的长度，返回的队列的元素个数
	
*/
int LengthQueue(struct LinkedQueue*s)
{
	return s->num;
}
/*
	GetHead()  :返回队列头元素，但是不出队列
*/
void GetHead(struct LinkedQueue*s,ElemType *d)
{
	if(IsEmptyQueue(s))
		return ;
	
	*d = s->front->data;

}
/*
	DeQueue()   :出队列
*/
void DeQueue(struct LinkedQueue*s,ElemType *d)
{
	if(IsEmptyQueue(s))
		return ;
	
	struct node*p = s->front;
	*d = s->front->data;
	s->front = s->front->next;
	if(s->front == NULL)//全部出队后，头尾都为NULL
	{
		s->rear = NULL;
	}
	p->next = NULL;
	free(p);
	
	p = NULL;
	s->num--;
}

/*

	EnQueue() :入队列
	
*/
void EnQueue(struct LinkedQueue*s,ElemType d)
{
	struct node*pnew = malloc(sizeof(*pnew));
	pnew->data = d;
	pnew->next = NULL;
	
	if(s->rear == NULL)
	{
		s->rear = pnew;
		s->front = pnew;
	}
	else
	{
		s->rear->next = pnew;
		s->rear = pnew;
	}
	s->num++;
}