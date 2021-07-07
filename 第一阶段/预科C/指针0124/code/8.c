#include <stdio.h>

int main()
{
	int a;
	
	int *p = &a;
	
	int **p1 = &p;
	
	**p1 = 234;
	
	printf("a = %d\n",a);
}