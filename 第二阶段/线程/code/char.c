#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int a = 33;
pthread_mutex_t mutex;//定义了一个互斥锁变量
pthread_cond_t   cond;//定义了一个条件变量
void*pthread(void*arg)
{
	while(a <= 122)
	{
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond,&mutex);
		printf("%c\n",a);
		a++;
		pthread_mutex_unlock(&mutex);
	}
}
int main()
{
	//初始化一个线程互斥锁
	//在任意线程使用该互斥锁之间必须先初始化
	pthread_mutex_init(&mutex,NULL);
	
	pthread_cond_init(&cond,NULL);
	
	pthread_t tid;
	int r = pthread_create(&tid,NULL,pthread,NULL);
	if(r == -1)
	{
		perror("pthread_create error:");
		return -1;
	}
	
	while(a <= 122)
	{
		sleep(1);
		pthread_mutex_lock(&mutex);
		printf("%d\n",a);
		
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}
	
	pthread_join(tid,NULL);
	
	pthread_mutex_destroy(&mutex);
	
	pthread_cond_destroy(&cond);
}