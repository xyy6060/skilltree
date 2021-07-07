#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#define MAX_WAITIN_TASK 1000
#define MAX_ACTIVE_THREAD 20


struct task//任务节点
{
	void*(*do_task)(void*);//函数指针，指向要执行的函数
	void*arg;//一个指针，执行任务函数时候给函数的参数
	
	struct task*next;
};                        

struct thread_pool
{
	pthread_mutex_t mutex;//互斥锁，用来保护那个任务链表的
	pthread_cond_t cond;//条件变量  有任务  =》条件
	bool shutdown;    //线程池的销毁标记
	
	pthread_t *tid;//指向线程ID的数组
	struct task*task_first;//指向第一个任务节点
	unsigned int max_waiting_tasks;//表示最大执行的任务数
	unsigned int waiting_task;//待执行的任务数
	unsigned int active_threads;//活动线程数
	//....
};
void handler(void*arg);
/*
	routine:任务执行函数
*/
void*routine(void*arg);
bool init_pool(struct thread_pool*pool,int tnread_num);
bool add_task(struct thread_pool*pool,void*(*do_task)(void*),void*arg);
bool destroy_pool(struct thread_pool*pool);

#endif