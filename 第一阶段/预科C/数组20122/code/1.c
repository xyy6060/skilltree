#include <stdio.h>

int main()
{
	int a[3][4],i,j;
	
	for(i = 0;i < 3;i++)
	{
		for(j = 0;j < 4;j++)
		{
			scanf("%d",&a[i][j]);
		}
	}
	
	/*===
	for(i = 0;i < 3*4;i++)
	{
		
		//scanf("%d",&a[i/4][i%4]);
		
	}
	*/
	
	int max,min,sum = 0;
	max = a[0][0];
	min = a[0][0];
	
	for(i = 0;i < 3;i++)
	{
		for(j = 0;j < 4;j++)
		{
			if(max < a[i][j])
			{
				max = a[i][j];
			}
			if(min > a[i][j])
			{
				min = a[i][j];
			}
			sum += a[i][j]; 
		}
	}
	
	for(i = 0;i < 3;i++)
	{
		for(j = 0;j < 4;j++)
		{
			printf("%d ",a[i][j]);
		}
		printf("\n");
	}
	
	printf("min = %d max = %d sum = %d\n",min,max,sum);
	
}