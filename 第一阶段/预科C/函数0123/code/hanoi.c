#include <stdio.h>
/*
	Hanoi:把n个盘子从A移动到C，中间利用B
		
		输入参数：
			n :多少个盘子
			A :起点
			B ：中转点
			C ：终点
*/
void Hanoi(int n,char A,char B,char C)
{
	if(n == 1)
	{
		printf("%c->%c\n",A,C);
		return ;
	}
	Hanoi(n-1,A,C,B);
	
	printf("%c->%c\n",A,C);
	
	Hanoi(n-1,B,A,C);
	
}
int main()
{
	int n;
	scanf("%d",&n);
	
	Hanoi(n,'A','B','C');
}