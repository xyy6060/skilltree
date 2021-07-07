#include <stdio.h>

/*
	find_max：求一个数组的最大值
	@a :数组名
	@n :数组元素个数
	返回值 ：数组的最大值
*/	
int find_max(int a[],int n)
{
	int max = a[0];
	int i;
	for(i = 0;i < n;i++)
	{
		if(a[i] > max)
		{
			max = a[i];
		}
	}
	
	return max;
}
int main()
{
	int a[5];
	int i;
	for(i = 0;i < 5;i++)
	{
		scanf("%d",&a[i]);
	}
	
	//求数组a的最大值
	//int max = find_max(a,5);
	printf("max = %d\n",find_max(a,5));
}
