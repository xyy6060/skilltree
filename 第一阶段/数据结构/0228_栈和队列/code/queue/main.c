#include <stdio.h>
#include "LinkedQueue.h"

int main()
{
	struct LinkedQueue*s = InitQueue();
	ElemType d;
	while(1)
	{
		scanf("%c",&d);
		if(d == '#')
			break;
		
		EnQueue(s,d);
	}
	DeQueue(s,&d);
	printf("DeQueue:%c\n",d);//A
	printf("length :%d\n",LengthQueue(s));//7
	
	GetHead(s,&d);
	printf("DeQueue:%c\n",d);//S
	printf("length :%d\n",LengthQueue(s));//7
	
	EnQueue(s,'Y');
	printf("length :%d\n",LengthQueue(s));//8
	while(!IsEmptyQueue(s))
	{
		DeQueue(s,&d);
		printf("DeQueue:%c\n",d);//SDFGHJKY
	}
	printf("length :%d\n",LengthQueue(s));//0
	
}
//ASDFGHJK#