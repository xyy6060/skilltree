#include <stdio.h>

int main()
{
	int i,x;
	scanf("%d",&x);
	
	for(i = 2;i < x;i++)
	{
		if(x % i == 0)//只要发现一个因子，不需要往下判断
		{
			printf("no\n");
			break; //提前结束循环
		}
	}
	if(i== x)
	{
		printf("yes\n");
	}
	
}