#include <stdio.h>
#include <time.h>

int main()
{
	time_t t = time(NULL);//1970~ 现在的秒数
	struct tm*p = localtime(&t);
	
	printf("%d-%d-%d  %d:%d:%d\n",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
}