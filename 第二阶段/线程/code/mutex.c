#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t mutex;//定义了一个互斥锁变量
	
void output(char*s)
{
	while(*s)
	{
		fprintf(stderr,"%c",*s);
		usleep(100);
		s++;
	}
}


void *pthread(void *arg)
{
	pthread_mutex_lock(&mutex);
	output("message from by child \n");
	pthread_mutex_unlock(&mutex);
	
}
int main()
{
	//在任意线程使用该互斥锁之间必须先初始化
	pthread_mutex_init(&mutex,NULL);
	
	//创建一个线程
	pthread_t tid;
	pthread_create(&tid,NULL,pthread,NULL);
	
	//在访问共享资源之前，先加锁
	pthread_mutex_lock(&mutex);
	output("infomation from parent \n");
	pthread_mutex_unlock(&mutex);
	
	//阻塞等待子线程退出，然后回收资源,销毁线程互斥锁
	pthread_join(tid,NULL);
	
	pthread_mutex_destroy(&mutex);
}

