#include <stdio.h>


int main()
{
	FILE*fp = fopen("stu.txt","r");
		
	int num;
	//char *name = malloc(100);
	char name[32];
	int score;
	char *class = "CS19167";
		   
	char str[128];
		   
	while(1)
	{
		fscanf(fp,"%d%s%d",&num,name,&score);
		if(feof(fp))
		{
			break;
		}
		//printf("%d\t%s\t%d\n",num,name,score);
		sprintf(str,"class:%s my name is %s",class,name);
		printf("%s\n",str);
	}
	

	fclose(fp);
}