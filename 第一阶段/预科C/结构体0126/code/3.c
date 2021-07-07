#include <stdio.h>
union test
{
	int a;
	char b;
};	
int main()
{
	union test s;
	s.a = 0x12345678;
	printf("%x\n",s.b);
}