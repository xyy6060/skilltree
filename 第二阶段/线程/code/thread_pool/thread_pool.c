#include "thread_pool.h"

/*
	清理函数
*/
void handler(void*arg)
{
	pthread_mutex_unlock((pthread_mutex_t*)arg);
}
/*
	routine:任务执行函数
*/
void*routine(void*arg)
{
	struct thread_pool*pool = (struct thread_pool*) arg;
	struct task*p = NULL;
	
	while(1)
	{
		//访问任务队列前加锁，为防止取消后死锁，注册处理例程
		pthread_cleanup_push(handler,(void*)&pool->mutex);
		pthread_mutex_lock(&pool->mutex);
		
		//===============================
		//若线程池没有关闭，且没有任务，则进入到睡眠状态
		if(!pool->shutdown && pool->waiting_task == 0)
		{
			pthread_cond_wait(&pool->cond,&pool->mutex);//阻塞在这里
								//唤醒的地方
								//1:在添加任务时候
								//2:销毁线程池的时候
		}
		
		//若线程池关闭，且没有任务，立即释放互斥锁并退出
		if(pool->shutdown && pool->waiting_task == 0)
		{
			pthread_mutex_unlock(&pool->mutex);
			pthread_exit(NULL);//
			
		}
		
		//当前有任务，拿任务
		p = pool->task_first;
		pool->task_first = p->next;
		p->next = NULL;
		pool->waiting_task--;
		
		//释放互斥锁，并出栈handler(但是不执行)
		pthread_mutex_unlock(&pool->mutex);
		pthread_cleanup_pop(0);
		
		//执行任务，并且在此期间禁止响应取消请求
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
		(p->do_task)(p->arg);//执行任务
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
		
		free(p);
		
	}
}
/*
	init_pool:初始化一个线程池
	@pool：要初始化的线程池
	@tnread_num：线程池中最先有多少条线程
*/
bool init_pool(struct thread_pool*pool,int thread_num)
{
	pthread_mutex_init(&pool->mutex,NULL);//初始化互斥锁
	pthread_cond_init(&pool->cond,NULL);//初始化条件变量
	
	pool->shutdown = false;//打开
	pool->tid = malloc(sizeof(pthread_t)*MAX_ACTIVE_THREAD);//线程ID数组开辟空间
	
	pool->task_first = NULL;
	
	pool->max_waiting_tasks = MAX_WAITIN_TASK;
	pool->waiting_task = 0;
	pool->active_threads = thread_num;
	
	
	int i;
	for(i = 0;i < thread_num;i++)//创建thread_num线程来执行你的任务
	{
		int r = pthread_create(&(pool->tid)[i],NULL,routine,(void*)pool);
		if(r == -1)
		{
			perror("create thread error:");
			printf("i = %d\n",i);
			return false;
		}
	}
	
	return true;
	
}

/*
	bool add_task:往线程池中投放任务
			
*/
bool add_task(struct thread_pool*pool,void*(*do_task)(void*),void*arg)
{
	//创建一个新的任务节点,并赋值
	struct task*new_task = malloc(sizeof(*new_task));
	new_task->do_task = do_task;
	new_task->arg = arg;
	new_task->next = NULL;
	
	//访问任务队列前加锁
	pthread_mutex_lock(&pool->mutex);
	
	//等待任务数已达到上限，解锁退出
	if(pool->waiting_task == MAX_WAITIN_TASK)
	{
		pthread_mutex_unlock(&pool->mutex);
		free(new_task);
		return false;
	}
	
	struct task*p = pool->task_first;
	if(p == NULL)
	{
		pool->task_first = new_task;
		
	}
	else
	{
		while(p->next != NULL)
		{
			p = p->next;
		}
		p->next = new_task;
		
	}
	pool->waiting_task++;
	pthread_mutex_unlock(&pool->mutex);
	pthread_cond_signal(&pool->cond);
	
	return true;
}

/*
	destroy_pool:销毁线程池
			
*/
bool destroy_pool(struct thread_pool*pool)
{
	pool->shutdown = true;
	
	pthread_cond_broadcast(&pool->cond);//唤醒所有睡眠的线程
	//等待线程退出
	int i;
	for(i = 0;i < pool->active_threads;i++)
	{
		pthread_join(pool->tid[i],NULL);
	}
	
	//free memories
	free(pool->tid);
	
	//mutex  cond
	pthread_mutex_destroy(&pool->mutex);
	pthread_cond_destroy(&pool->cond);
	
}