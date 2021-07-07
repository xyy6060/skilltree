#include <stdio.h>
int sum(int a,int b)
{
	return a+b;
}

int main()
{
	
	int (*p)(int,int);//定义了一个函数指针
	
	p = &sum;// p = sum;
	
	printf("%d\n",sum(3,5));
	
	//*p =>*&sum =>sum
	
	printf("%d\n",(*p)(2,5));
	
	
	//p(3,5);
	printf("%d\n",p(2,7));
	
}