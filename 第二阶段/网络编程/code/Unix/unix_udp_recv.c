#include <stdio.h>
#define PATH "/home/gec/unix167"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/un.h>
int main()
{
	//创建套接字
	int server_fd = socket(AF_LOCAL,SOCK_DGRAM,0);
	if(server_fd == -1)
	{
		perror("socket error:");
		return -1;
	}
	perror("socket:");
	
	//设置服务器的地址结构
	struct sockaddr_un un;
	un.sun_family = AF_LOCAL;
	strcpy(un.sun_path,PATH);
	
	//绑定
	int ret = bind(server_fd,(struct sockaddr*)&un,sizeof(un));
	if(ret == -1)
	{
		perror("bind error:");
		return -1;
	}
	perror("bind :");
	
	struct sockaddr_un cli_un;
	socklen_t len = sizeof(cli_un);
	//发送信息
	char str[128] = {0};
	ret = recvfrom(server_fd,str,128,0,(struct sockaddr*)&cli_un,&len);
	if(ret > 0)
	{
		printf("str :%s\n",str);
	}
	
	//关闭套接字
	close(server_fd);
}