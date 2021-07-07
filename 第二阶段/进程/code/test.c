#include <stdio.h>



int a = 6;//.data
int b;//.bss

int main()
{
	char *s = "qwerty";//s 在栈空间  "qwerty" =>.rodata
	static int c = 10; //c  在.data
	if(c)
	{
		int *p = malloc(sizeof(100));//p 在 栈空间
	}
}