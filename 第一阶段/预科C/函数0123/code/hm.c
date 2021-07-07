#include <stdio.h>

int main()
{
	int a[4][4],i,j,k,max;
	for(i = 0;i < 4;i++)
	{
		for(j = 0;j < 4;j++)
		{
			scanf("%d",&a[i][j]);
		}
	}
	
	for(i= 0;i<4;i++)//行
	{
		max = a[i][0];
		for(j = 0;j < 4;j++)//找到一行中最大的那个值
		{
			if(max < a[i][j])
			{
				max = a[i][j];
			}
		}
		
		for(j = 0;j < 4;j++)//找最大的值的位置
		{
			if(max == a[i][j])
			{
				for(k = 0;k < 4;k++)
				{
					if(a[k][j] < a[i][j])
					{
						goto loop;
					}
				}
				
				printf("鞍点:a[%d][%d] = %d\n",i,j,a[i][j]);
				
			}
			
			loop:
				  ;
		}
		
		
	}
}