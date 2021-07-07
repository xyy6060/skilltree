#include<stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include<string.h>
#include<stdlib.h>
struct msg {
				long mtype;      
				char mtext[0];    
			  };
int main()
{
	//获取键值
	key_t key=ftok("/home/gec",42);
	if(key==-1)
	{
		perror("ftok error:");
		return -1;
	}
	//获取消息队列
	int msg_id=msgget(key, IPC_CREAT | 0666);
	if(msg_id==-1)
	{
		perror("msgget error:");
		return -1;
	}
	//给消息队列发送消息
	char buf[256]={0};
	fgets(buf,256,stdin);
	struct msg*p = malloc(sizeof(struct msg) + strlen(buf)+1);
	p->mtype=100;
	memcpy(p->mtext,buf,strlen(buf)+1);
	int r=msgsnd(msg_id, p, strlen(p->mtext)+1, 0);
	if(r==-1)
	{
		perror("msgsnd error:");
		return -1;
	}
	
}