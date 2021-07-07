#include <stdio.h>

int main()
{
	int x,p,n,y;
	scanf("%d%d%d%d",&x,&p,&n,&y);
	
	int re = ((x&((1<<n)-1))<<(sizeof(x)*8-n)) | (x >>n);
	
	printf("re = %d\n",re);
	
}