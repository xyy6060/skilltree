#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
int main(int argc,char**argv)
{
	int client_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(client_fd == -1)
	{
		perror("socket error:");
		return -1;
	}
	perror("socket :");
	//设置服务器的地址
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);	
	
	
	//
	char *s = "hello ntp";
	int ret = sendto(client_fd,s,strlen(s),0,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(ret > 0)
	{
		perror("send to server:");
	}
	
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	
	//
	char buf[128] = {0};
	ret = recvfrom(client_fd,buf,128,0,NULL,NULL);
	if(ret > 0)
	{
		printf("recv message :%s\n",buf);
	}
	
	execlp("date","date","-s",buf,NULL);
	close(client_fd);
	
}