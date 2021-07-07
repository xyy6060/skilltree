#include <stdio.h>
void change(int x,int y)
{
	int t ;
	t = x;
	x = y;
	y = t;
	//printf("x = %d,y = %d\n",x,y);

}
void change2(int* x,int* y)
{
	int t;
	t = *x;
	*x = *y;
	*y = t;
}

int main()
{
	int a = 4,b = 5;
	printf("before:a = %d,b = %d\n",a,b);
	change2(&a,&b);
	printf("after:a = %d,b = %d\n",a,b);
	
}
