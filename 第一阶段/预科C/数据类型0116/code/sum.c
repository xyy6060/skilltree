#include <stdio.h>
int main()
{
	int n,sum = 0;
	for(n = 1;n <=1000;n++)
	{
		if(n%2 != 0)
		{
			//求和
			sum = sum+n;
			
		}
	}
	printf("sum = %d\n",sum);
}
