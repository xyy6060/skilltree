#include <stdio.h>

int main()
{
	int a[5],i;
	for(i = 0;i < 5;i++)
	{
		scanf("%d",a+i);
	}
	int *p = &a[2];
	
	//p[2] =>*(p+2) =>*(&a[2]+2) =>*&a[4] =>a[4]
	
	printf("p[2] = %d\n",p[2]);
	printf("a[4] = %d\n",a[4]);
	
	printf("===============\n");
	for(i = 0;i < 5;i++)
	{
		printf("%d ",*(a+i));
	}
	
	printf("\n");
}