#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#define IP_REG  "[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}"



int main()
{
	regex_t preg;
	
	//step1 编译正则表达式
	int r = regcomp(&preg,IP_REG,REG_EXTENDED);
	if(r != 0)
	{
		char err[512] = {0};
		size_t len = regerror(r,&preg,err,512);
		if(len > 0)
		{
			printf("err:%s\n",err);
		}
		return -1;
	}
	
	//step2:匹配
	regmatch_t pmatch[2];
	char *str = "fgywefg192.168.3.125fryfy192.168.4.56";//待匹配的字符串
	char *pm = str;
	do
	{
		 r = regexec(&preg,pm,2,pmatch,0);
		 if(r == 0)
		 {
			int i;
			for(i = 0;i < 2;i++)
			{
				//相对于原始字符串的偏移
				printf(" %d %d - %d\n",i,pmatch[i].rm_so+(pm-str),pmatch[i].rm_eo+(pm-str));
			}
			
			pm = pm + pmatch[0].rm_eo;
		 }
		 
		
	}while(r == 0);
}

