#include <stdio.h>
enum weekday{sun,mon = 2,tue,wed,the = 0,fri,sat};

int main()
{
	enum weekday week = sun;
		
	printf("%d %d %d \n",week,tue,sat);//0 3 2
}		
		