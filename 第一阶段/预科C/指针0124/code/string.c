#include <stdio.h>

int main()
{
	char*s = "12345";
	
	printf("%p\n",s);
	printf("%d\n",*(s+1));
	
	*(s+1)  = 'a';//error 
				//没有语法错误，段错误=》试图修改一个常量区间
}