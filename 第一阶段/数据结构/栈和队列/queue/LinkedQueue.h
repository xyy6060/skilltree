#ifndef _LINKEDQUEUE_H_
#define _LINKEDQUEUE_H_
#include <stdlib.h>
typedef char ElemType;
struct node//数据节点
{
	ElemType data;
	struct node *next;
}; 	

struct LinkedQueue//链式队列
{
	struct node*front;//队头
	struct node*rear;//队尾
	int num;//队列元素个数
};
struct LinkedQueue*InitQueue();
void DestroyQueue(struct LinkedQueue*s);
void ClearQueue(struct LinkedQueue*s) ;
int IsEmptyQueue(struct LinkedQueue*s);
int LengthQueue(struct LinkedQueue*s);
void GetHead(struct LinkedQueue*s,ElemType* d);
void DeQueue(struct LinkedQueue*s,ElemType* d);
void EnQueue(struct LinkedQueue*s,ElemType d);
#endif