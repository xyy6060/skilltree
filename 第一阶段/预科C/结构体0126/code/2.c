#include <stdio.h>
union test
{
	char a;
	char b;
	
};	
int main()
{
	union test s;
	s.a = 'A';
	// &s.a  == &s.b
	printf("%p \n%p\n",&s.a,&s.b);
	printf("b = %d\n",s.b);//	=>65
}
