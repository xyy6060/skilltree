#include <stdio.h>

int main()
{
	/*
	int a[10]  = {1,2,3,4,5,6,7,8,9,0};
	printf("a = %p\na+1 = %p\n&a+1 = %p\n",a,a+1,&a+1);
	*/
	int b[3][4] = {1,2,3,4,5,6};		
	printf("b = %p\nb+1 = %p\n&b[0]+1 = %p\n&b+1 = %p\n",b,b+1,&b[0]+1,&b+1);
}

	