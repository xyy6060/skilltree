#include <stdio.h>

int main()
{
	int r,a;
	char c;
	
	//r = scanf("abcd%d %c",&a,&c);
	
	//假设用户输入的是ABCD123 A
	
	//a = 未知? c = 未知? r = 0
	//printf("a = %dc = %cr = %d",a,c,r);
	
	r = scanf("%d %c",&a,&c);
	
	//假设用户输入的是123ABCD
	
	//a = 123 c = A r = 2
	printf("a = %dc = %cr = %d",a,c,r);
}
//1 2 3 4 fh   t    u67 u 6  u65 uy 6 u6 u 
