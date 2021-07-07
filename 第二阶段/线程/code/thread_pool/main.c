#include "thread_pool.h"

void*fun(void*arg)
{
	while(1)
	{
		printf("arg = %d\n",*(int*)arg);
		sleep(1);
	}
}
int main()
{
	//创建一个线程池，并初始化
	struct thread_pool pool;
	bool ok = init_pool(&pool,3);
	
	int a[5] = {1,2,3,4,5};
	int i = 0;
	for(;i < 5;i++)
	{
		add_task(&pool,fun,(void*)&a[i]);
	}
	
	destroy_pool(&pool);
}