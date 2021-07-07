#include <stdio.h>

int main()
{
	int num,x,y,z;
	for(num = 100;num<=999;num++)
	{
		//求取num的个，十，百
		x=num%10;
		y=num/10%10;
		z=num/100;
		if(x*x*x+y*y*y+z*z*z == num)
		{
			printf("%d ",num);
		}
	}
	printf("\n");
}