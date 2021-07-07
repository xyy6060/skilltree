#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char**argv)
{
	//创建一个套接字
	int client_fd = socket(AF_INET,SOCK_STREAM,0);
	if(client_fd == -1)
	{
		perror("socket error:");
		return -1; 
	}
	perror("socket:");
	//设置服务器的地址结构体
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	
	//连接
	int ret = connect(client_fd,(struct sockaddr *)&server_addr,sizeof(server_addr));
	if(ret == -1)
	{
		perror("connect error:");
		return -1;
	}
	perror("connect:");
	
	
	char send[128] = {0};
	//发送和接收
	while(1)
	{
		fgets(send,128,stdin);
		ret = write(client_fd,send,strlen(send));
		printf("ret = %d\n",ret);
		perror("write:");
	}
	
	
	
	close(client_fd);
	
}

//./client 192.168.31.171  8888