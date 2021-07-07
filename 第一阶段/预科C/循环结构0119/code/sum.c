#include <stdio.h>
/*
	
*/

int main()
{
	int i = 0;
	int sum = 0;
loop:

	i++;
	sum = sum+i;

	if(i < 100)
	{
		goto loop;
	}
	
	printf("sum = %d\n",sum);
}