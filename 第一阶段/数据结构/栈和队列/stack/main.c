#include <stdio.h>
#include "SqStack.h"
int main()
{
	struct SqStack*s = InitStack();
	ElemType d;
	while(1)
	{
		scanf("%c",&d);
		if(d == '#')
			break;
		Push(s,d);
	}
	
	ClearStack(s);
	
}
