#include <stdio.h>

int main()
{
	int l,r,sum,num;
	scanf("%d",&num);
	for(l = 1,r = 1,sum = 1;l <= num/2;)
	{
		if(sum < num) //往右边扩展区间
		{
			r++;
			sum += r;
		}
		else if(sum >num)//把区间最左边的那个数拿掉	
		{
			sum -= l;
			l++;
		}	
		else
		{
			int i;
			//输出处理
			//15 = 1+2+3+4+5
			printf("%d = ",num);
			for(i = l;i<r;i++)
			{
				printf("%d+",i);
			}
			printf("%d\n",r);
			
			r++;
			sum += r;
		}
			
	}
	
}