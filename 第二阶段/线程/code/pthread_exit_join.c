#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
void*fun(void*arg)
{
	pthread_detach(pthread_self());
	
	printf("argument :%s\n",((char*)arg));//将参数转换为原本类型
	
	sleep(2);
	int *a = malloc(4);
	*a = 9;
	pthread_exit(a);
	
	printf("quit\n");
}
int main()
{
	pthread_t tid;
	int a = 3;
	//pthread_create(&tid,NULL,fun,(void*)&a);
	pthread_create(&tid,NULL,fun,(void*)"string");
	
	void* p;
	//阻塞等待指定的线程退出，并获取它的退出值
	//pthread_join(tid,&p);
	
	printf("exit value :%d\n",*(int*)p);
	
	
}