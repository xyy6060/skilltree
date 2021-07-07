#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>



int main()
{
	//获取一个键值
	key_t key = ftok("/home/gec",10086);
	if(key == -1)
	{
		perror("ftok error:");
		return -1;
	}
	
	//创建或打开一个共享内存
	int shm_id = shmget(key,4096,IPC_CREAT|0666);
	if(shm_id == -1)
	{
		perror("shmget error:");
		return ;
	}
	
	//创建一个POSIX有名信号量
	sem_t*data= sem_open("/data",0);
	sem_t*space = sem_open("/space",0);
	//将共享内存映射到当前进程的虚拟内存中
	char*p = shmat(shm_id,NULL,0);
	
	while(1)
	{
		sem_wait(data);//申请数据资源  -1
		fprintf(stderr,"%c",*p);
		sem_post(space);//释放空间资源  +1
	}
	
	shmdt(p);
	return 0;

	
}