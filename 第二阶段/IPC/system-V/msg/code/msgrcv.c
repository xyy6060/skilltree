#include<stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include<string.h>
#include<stdlib.h>
struct msg {
				long mtype;      
				char mtext[256];    
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
	int msg_id=msgget(key, 0);
	if(msg_id==-1)
	{
		perror("msgget error:");
		return -1;
	}
	//从消息队列接收消息
	struct msg *p=malloc(sizeof(struct msg));
	int re=msgrcv(msg_id,p,sizeof(p->mtext),100,0);
	if(re==-1)
	{
		perror("msgrcv error:");
		return -1;
	}
	printf("%s  type=%ld\n",p->mtext,p->mtype);
}