#include <stdio.h>


int main()
{
	int a[5];
	int *p = &a[0];
		
	a[0] = 1;
	printf("a[0] = %d\n",a[0]);
	
	*p = 2;
	printf("a[0] = %d\n",a[0]);
	
	*a = 3;
	printf("a[0] = %d\n",a[0]);
	
	*&a[0] = 4;
	printf("a[0] = %d\n",a[0]);
	
	**&p = 5;
	printf("a[0] = %d\n",a[0]);
	
	p[0] = 6;
	printf("a[0] = %d\n",a[0]);
		
}