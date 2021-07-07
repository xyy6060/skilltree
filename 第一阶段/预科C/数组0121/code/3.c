#include <stdio.h>


int main()
{
	int x,i,max,min,sum;
	scanf("%d",&x);
	int a[x];
	
	//printf("max = %d,min = %d\n",max,min);
	for(i = 0;i <x;i++)
	{
		scanf("%d",&a[i]);
	}
	
	max = a[0];
	min = a[0];
	for(i = 0; i < x;i++)
	{
		if(max < a[i])
		{
			max = a[i];
		}
		if(min > a[i])
		{
			min = a[i];
		}
	}

	printf("max = %d,min = %d\n",max,min);
}