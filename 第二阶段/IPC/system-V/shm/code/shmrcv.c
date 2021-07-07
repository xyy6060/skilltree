#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#define PATHNMAE "/home/gec"
#define PRO_ID  19167
int main()
{
	//step1:获取system v ipc 键值
	key_t key = ftok(PATHNMAE,PRO_ID);
	if(key == -1)
	{
		perror("ftok error:");
		return -1;
	}
	
	//step2 :创建或打开一个共享内存区
	 int shm_id = shmget(key,4096,0);
	 if(shm_id == -1)
	 {
		 perror("shmget error:");
		 return -1;
	 }
	
	
	//step3:映射到进程地址空间中
	char * p = shmat(shm_id,NULL,0);
	
	
	//step4:数据传输
	printf("from shm :%s\n",p+sizeof(pid_t));
	
	kill(*((pid_t*)p),SIGINT);
	
	
	shmdt(p);
	
	
}