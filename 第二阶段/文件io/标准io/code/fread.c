#include<stdio.h>

int main()
{
	//step1:open file
	FILE*fp = fopen("1.txt","r");
	if(fp == NULL)
	{
		perror("fopen ");
		return -1;
	}
	
	//ste2:
	char s[32] = {0};
	 size_t re = fread(s,6,4,fp);
	 printf("re = %d\n",re);
	
	//step3:close
	fclose(fp);
}