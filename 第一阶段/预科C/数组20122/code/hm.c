#include <stdio.h>
#define N 3
int main()
{
	
	int a[N],i,max,sum;
	
	for(i = 0;i <N;i++)
	{
		scanf("%d",&a[i]);
	}
	/*
	max = a[0];
	for(i = 0;i < N;i++)//以i开头的子数组
	{
		sum = 0;
		
		for(j = i;j < N;j++)
		{
			sum += a[j];
			if(sum > max)
			{
				max = sum;
			}
			
		}
	}
	*/
	sum = 0;
	max = a[0];
	
	for(i = 0;i < N;i++)
	{
		sum += a[i];
		if(max < sum)
		{
			max = sum;
		}
		
		if(sum < 0)
			sum = 0;
	}
	printf("max = %d\n",max);
}