#include <stdio.h>
#include <string.h>
int main()
{
	char s1[8];
	char s2[8] = "qwert";
	
	strcpy(s1,"1234567890");
	
	printf("%p\n%p\n",s1,s2);
	printf("%s\n",s1);//1234567890
	printf("%s\n",s2);//90
	
}