// 发送端
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
	
	int opt = 1;
	//设置套接字选项，允许广播
	int nb = 0;
	nb = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
	if(nb == -1)
	{
		perror("setsockopt error");	
		return -1;
	}
	
 	//设置目的地址结构体,广播的IP地址
	struct sockaddr_in dest;
	bzero(&dest, sizeof(struct sockaddr_in));
	dest.sin_family=AF_INET;
	dest.sin_addr.s_addr=inet_addr("192.168.31.255");
	dest.sin_port=htons(6000);
	int nlen=sizeof(dest);
	
	char smsg[] = {"abcdef"};
	while(1)
	{
		sleep(1);
		//发送消息，现发送数据现设定目的地址
		int ret=sendto(sockfd, smsg, strlen(smsg), 0, (struct sockaddr*)&dest, nlen);
		if(ret<0)
		{
			perror("sendto error");	
		}
		else
		{		
			printf("ip:%s\n",inet_ntoa(dest.sin_addr));	
		}
	}
	return 0;
}
