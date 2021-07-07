#ifndef _SQQUEUE_H_
#define _SQQUEUE_H_
#include <stdlib.h>
typedef int ElemType;		
struct SqQueue
{
	ElemType*data;
	int rear;//队尾下标
	int front;//队头下标
	int max_len;//数组的最大长度
	int num;//队列中元素个数
};

struct SqQueue*InitQueue(int max);
void DestroyQueue(struct SqQueue*s);
void ClearQueue(struct SqQueue*s) ;
int IsEmptyQueue(struct SqQueue*s);
int LengthQueue(struct SqQueue*s);
void GetHead(struct SqQueue*s,ElemType* d);
void DeQueue(struct SqQueue*s,ElemType* d);
void EnQueue(struct SqQueue*s,ElemType d);
#endif