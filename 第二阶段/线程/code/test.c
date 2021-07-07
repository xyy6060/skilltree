#include <stdio.h>
#include <pthread.h>
void*fun(void*arg)
{
	while(1)
	{
		printf("hi\n");
		sleep(20);
		break;
	}
}
int main()
{
	pthread_t tid;
	int a = 3;
	pthread_create(&tid,NULL,fun,(void*)&a);
	while(1)
	{
		printf("hello world\n");
		sleep(1);
	}
	
	pause();
}