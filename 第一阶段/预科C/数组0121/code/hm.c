#include <stdio.h>


int main()
{
	int i,n;
	scanf("%d",&n);
	for(i = 2;i <= n/2;i++)
	{
		while(n%i == 0)
		{
			printf("%d*",i);
			n /= i;
		}
	}
	printf("%d\n",n);
	
}