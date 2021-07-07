#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/sem.h>
#include <errno.h>

int main()
{
	//获取键值
       key_t key = ftok("/home/gec",199);
		if(key == -1)
		{
			perror("ftok error:");
			return -1;
		}
		
	//创建信号量集
	 int sem_id = semget(key,3,IPC_CREAT |IPC_EXCL| 0666);
	 if(sem_id == -1)
	 {
		 if(errno == EEXIST)
		 {
			 semget(key,3,0);
		 }
		 else
		 {
			 perror("semget error:");
			 return -1; 
		 }
		
	 }
	//设置信号量集中的信号量的初始值（仅设置一次）
	//unsigned short vals[3] = {1,2,3};
	//int semctl(sem_id,0,SETALL,vals);
	
	//GETVAL
	int val = semctl(sem_id,0,GETVAL);
	printf("val = %d\n",val);
	
	//p操作
	struct sembuf buf;
	buf.sem_num = 0;
	buf.sem_op = -1;//申请一个资源
	buf.sem_flg = 0;
	
	semop(sem_id,&buf,1);
	 
	printf("hello world\n");
	
	//v操作
	buf.sem_num = 0;
	buf.sem_op = 1;//申请一个资源
	buf.sem_flg = 0;
	semop(sem_id,&buf,1);
	
}