#include<stdio.h>

int main()
{
	//step1:open file
	FILE*fp = fopen("test.bin","w+");
	if(fp == NULL)
	{
		perror("fopen ");
		return -1;
	}
	
	//ste2:写操作
	int a[11] = {1,2,3,4,5,6,7,4,5,6,7};
	int re = fwrite(a,sizeof(a[0]),11,fp);
	if(re < 0)
		perror("fwrite :");
	printf("re = %d\n",re);
	
	
	//定位光标
	fseek(fp,0,SEEK_SET);
	//读操作
	int b[11];
	re = fread(b,sizeof(b[0]),11,fp);
	if(re < 0)
		perror("fwrite :");
	printf("re = %d\n",re);
	
	int i;
	for(i = 0;i < 11;i++)
	{
		printf("%d ",b[i]);
	}
	printf("\n");
	
	//step3:close
	fclose(fp);
	
	
	//ste2:
	
	
	
}