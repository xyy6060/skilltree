#include "SqQueue.h"

/*
	InitQueue() : 初始化一个队列
*/
struct SqQueue*InitQueue(int max)
{
	struct SqQueue*s = malloc(sizeof(*s));
	s->data = malloc(max*sizeof(ElemType));
	s->rear = 0;//队尾下标,下一个入队元素的下标
	s->front = 0;//队头下标，下一个出队元素的下标
	s->max_len = max;//数组的最大长度
	s->num = 0;//队列中元素个数
	
	return s;
}
/*
	IsEmptyQueue() :判断是否为空队列
	是 返回1
	不是 返回0
*/
int IsEmptyQueue(struct SqQueue*s)
{
	return s->num == 0 ? 1 : 0;
}
/*
	DestroyQueue() :销毁一个队列
*/
void DestroyQueue(struct SqQueue*s)
{
	ClearQueue(s);
	free(s->data);
	free(s);
}
/*
	ClearQueue()  :清空一个队列
*/
void ClearQueue(struct SqQueue*s)
{
	s->rear = s->front = s->num = 0;
}
/*
	LengthQueue() :队列的长度，返回的队列的元素个数
	
*/
int LengthQueue(struct SqQueue*s)
{
	return s->num;
}
/*
	GetHead()  :返回队列头元素，但是不出队列
*/
void GetHead(struct SqQueue*s,ElemType *d)
{
	if(IsEmptyQueue(s))
	{
		return ;
	}
	
	*d = s->data[s->front];

}
/*
	DeQueue()   :出队列
*/
void DeQueue(struct SqQueue*s,ElemType *d)
{
	if(IsEmptyQueue(s))
	{
		return ;
	}
	
	*d = s->data[s->front];
	s->front++;
	if(s->front == s->max_len)
	{
		s->front = 0;
	}
	
	s->num--;
}

/*

	EnQueue() :入队列
	
*/
void EnQueue(struct SqQueue*s,ElemType d)
{
	//判断队列是否为满
	if(s->num == s->max_len)
	{
		return ;
	}
	
	s->data[s->rear] = d;
	s->rear++;
	if(s->rear == s->max_len)//可能前面有出队元素留下的空位
	{
		s->rear = 0;
	}
	
	s->num++;
}