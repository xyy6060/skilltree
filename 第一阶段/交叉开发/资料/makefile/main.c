#include <stdio.h>
#include "sum.h"
#include "del.h"

int main()
{
	int a,b;
	scanf("%d%d",&a,&b);
	
	printf("sum = %d\n",sum(a,b));
	printf("del= %d\n",del(a,b));
}