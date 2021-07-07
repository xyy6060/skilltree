#include<stdio.h>
int main()
{
	int a,b,c,max,min,mid;
	printf("请输入三个整数:\n");
	scanf("%d%d%d",&a,&b,&c);
	
	
	max = a>b?a:b;
	max = max>c? max:c;
	
	//mid = a<b?a:b;
	//mid = mid>c? mid:c;
	
	min = a<b?a:b;
	min = min<c? min:c;

	b = a+b+c-max-min;
	a = min;
	c = max;
	
	
	printf("%d\t%d\t%d\n",a,b,c);
	
	
	

}