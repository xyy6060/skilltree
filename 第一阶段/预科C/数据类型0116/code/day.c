#include <stdio.h>
int main()
{
	int n,sum = 16;
	int year,month,day;
	
	//输入出生年月日
	scanf("%d%d%d",&year,&month,&day);
	
	//去掉尾年，把整年总天数算出来
	for(n = year;n<=2018;n++)
	{
		if((n%4 == 0 && n%100 != 0) || (n%400 == 0))//判断是否为闰年
		{
			sum += 366;//年数加366
		}
		else
		{
			sum += 365;
		}
	}

	//去掉出生那日之间的天数
	int monthday[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
	for(n = 1; n < month;n++)
	{
		sum -= 	monthday[n];
	}
	sum -= day;
	
	if(((year%4 == 0 && year%100 != 0) || (year%400 == 0))&& (month > 2))//判断是否为闰年
	{
		sum -= 1;//年数加366
	}
	
	
	printf("sum = %d\n",sum);
}