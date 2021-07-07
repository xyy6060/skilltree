#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

int main()
{
	sem_unlink("/data");
	perror("unlink data:");
	sem_unlink("/space");
	perror("unlink space:");
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
	
	//将共享内存映射到当前进程的虚拟内存中
	char*p = shmat(shm_id,NULL,0);
	
	//创建POSIX有名信号量
	sem_t*data = sem_open("/data",O_CREAT,0666,0);//锁数据
	sem_t*space = sem_open("/space",O_CREAT,0666,1);//锁空间
	
	char*msg = "0123456789";
	int i = 0;
	while(1)
	{
		sem_wait(space);   //申请空间资源  -1
		memcpy(p,msg+i,1);
		i = (i+1)%10;
		sem_post(data);   //释放数据资源  +1
	}
	
	shmdt(p);
	return 0;
	
}