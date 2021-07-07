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
		//
	int client_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(client_fd == -1)
	{
		perror("socket error:");
		return -1;
	}
	perror("socket:");
	
	//设置服务器的ip和端口号
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	char buf[128] = {0};
	socklen_t len = sizeof(server_addr);
	//
	gets(buf);
	int ret = sendto(client_fd,buf,strlen(buf),0,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(ret >= 0)
	{
		printf("send ok\n");
		memset(buf,0,128);
		recvfrom(client_fd,buf,sizeof(buf),0,(struct sockaddr*)&server_addr,&len);
		printf("buf :%s\n",buf);
	}
	
	
	
	close(client_fd);
}