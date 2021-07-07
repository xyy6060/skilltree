#include<stdio.h>

int main()
{
	//step1:打开文件
	 FILE *fp = fopen("1.txt","r+");
	 if(fp == NULL)//出错处理
	 {
		perror("fopen");
		return -1;
	 }
	  
	//printf("open file success\n");
	perror("fopen");
	
	//step2：文件操作
	int re = fgetc(fp);
	//printf("re = %c\n",re);
	
	fputc('A',fp);
	perror("");
	
	//step3:关闭文件
	 fclose(fp);
}