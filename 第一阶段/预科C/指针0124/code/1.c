#include <stdio.h>

int main()
{
	int a = 5,b;
	int*p = &a;//把a的地址赋值给p
	//p = &a;
	
	//printf("%p\n",&a);
	//printf("%p\n",p);
	//printf("%p\n",&p);
	
	//p当做左值：p本身的地址
	//p当做右值：a的地址
	
	//*p代表？ *p = *(&a) = a
	
	//*p 作为左值 ，代表&a
	//*p 作为右值，代表a的内容（值）
	
	*p = 1024;
	b = *p;
	
	printf("%d\n",a);
	printf("%d\n",*p);
	printf("%d\n",b);
	printf("%d\n",*&a);
}