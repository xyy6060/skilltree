#include <stdio.h>
struct birthday
{
	int y,m,d;
};		

struct student
{
	int num;
	char name[32];
	int sex;
	int score;
	struct birthday birth;
	
};
int main()
{
	struct student class[3];
	
	int i;
	for(i = 0;i < 3;i++)
	{
		scanf("%d%s%d%d%d%d%d",&class[i].num,class[i].name,&class[i].sex,
						&class[i].score,&class[i].birth.y,&class[i].birth.m,&class[i].birth.d);
	}
	printf("==========\n");
	for(i = 0;i < 3;i++)
	{
		printf("%d\t%s\t%d\t%d\t%d/%d/%d\n",class[i].num,class[i].name,class[i].sex,
						class[i].score,class[i].birth.y,class[i].birth.m,class[i].birth.d);
	}
}