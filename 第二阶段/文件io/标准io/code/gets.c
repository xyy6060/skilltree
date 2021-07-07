#include<stdio.h>

int main()
{
	char s[8];
	char s1[8] = "fyeg";
	
	fgets(s,8,stdin);
	
	printf("s = %s\ns1 = %s\n",s,s1);
	
	char *s2 = "gfuerfgiurgi";
	
	puts(s2);
}