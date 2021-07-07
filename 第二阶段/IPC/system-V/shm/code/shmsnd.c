#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>


#define PATHNMAE "/home/gec"
#define PRO_ID  19167

int shm_id;
void Rmshm(int sig)
{
	shmctl(shm_id,IPC_RMID,NULL);
}
int main()
{
	signal(SIGINT,Rmshm);
	//step1:获取system v ipc 键值
	key_t key = ftok(PATHNMAE,PRO_ID);
	if(key == -1)
	{
		perror("ftok error:");
		return -1;
	}
	
	//step2 :创建或打开一个共享内存区
	 shm_id = shmget(key,4096,IPC_CREAT|0666);
	 if(shm_id == -1)
	 {
		 perror("shmget error:");
		 return -1;
	 }
	
	
	//step3:映射到进程地址空间中
	char * p = shmat(shm_id,NULL,0);
	
	//step4:数据传输
	pid_t pid = getpid();
	
	memcpy(p,&pid,sizeof(pid_t));
	
	fgets(p+sizeof(pid_t),4096,stdin);
	
	shmdt(p);
	
	pause();
	
	
}