// 接收端 
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
 

int main()
{
	//创建套接字
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) 
	{   
		perror("socket error");	
		return -1;
	}   
	
	//设置套接字选项，IP地址可重复使用
	int a = 1;
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&a,sizeof(a));
	
	// 设置接收端的地址结构体
	struct sockaddr_in recv;
	bzero(&recv, sizeof(struct sockaddr_in));
	recv.sin_family = AF_INET;
	recv.sin_addr.s_addr =htonl(INADDR_ANY);
	recv.sin_port = htons(6000);
	
	//绑定
	ret = bind(sockfd,(struct sockaddr *)&(recv), sizeof(struct sockaddr_in));
	if(ret == -1) 
	{   
		perror("bind error");	
		return -1;
	}
 
	struct sockaddr_in send;
	socklen_t len = sizeof(send);
	
	while(1)
	{
		char smsg[100] = {0};
		//接受消息
		ret=recvfrom(sockfd, smsg, 100, 0, (struct sockaddr*)&send,&len);
		if(ret<=0)
		{
			perror("recvfrom error");
		}
		else
		{		
			printf("recv ip from:%s  data:%s\n",inet_ntoa(send.sin_addr), smsg);	
		}
	}
	return 0;
}
