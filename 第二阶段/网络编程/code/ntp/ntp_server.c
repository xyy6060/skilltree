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
#include <strings.h>
int main(int argc,char**argv)
{
	int server_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(server_fd == -1)
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
	
	//bind
	int ret = bind(server_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(ret == -1)
	{
		perror("bind error:");
		return -1;
	}
	perror("bind:");
	
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	char buf[128] = {0};
	ret = recvfrom(server_fd,buf,128,0,(struct sockaddr*)&client_addr,&len);
	if(ret > 0)
	{
		printf("recv from IP:%s  %s\n",inet_ntoa(client_addr.sin_addr),buf);
	}
	
	time_t t = time(NULL);//1970~ 现在的秒数
	struct tm*p = localtime(&t);
	
	printf("%d-%d-%d  %d:%d:%d\n",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	bzero(buf,128);
	
	sprintf(buf,"%d-%d-%d  %d:%d:%d",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	
	ret = sendto(server_fd,buf,128,0,(struct sockaddr*)&client_addr,sizeof(client_addr));
	if(ret > 0)
	{
		printf("send ok\n");
	}
	
	close(server_fd);
	
}