#include <stdio.h>

int strlen(char *s)
{
	int n = 0;
	while(*s)//*s != 0
	{
		n++;
		s++;
	}
	
	return n;
}

int main()
{
	//char *s = "12345678";
	//char *s;
	char s[32];
	scanf("%s",s);
	int re = strlen(s);
	printf("re = %d\n",re);
}
	