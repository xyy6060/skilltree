#include <stdio.h>
#include <stdint.h>
#define N 5

int main()
{
	int max,second,i,a[N];
	for(i = 0 ;i < N;i++)
	{
		scanf("%d",&a[i]);
		
	}
	
	max = a[0];//99
	second = INT32_MIN;//99
	
	for(i =0;i < N;i++)
	{
		if(a[i] > max)
		{
			second = max;
			max = a[i];
		}
		if((a[i] < max && a[i] > second))
		{
			second = a[i];
		}
	}
	
	//==================
	
	printf("%d\n",second);
		
}