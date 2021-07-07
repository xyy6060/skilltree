//发送端
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>
/*
	函数功能：往组播上发送数据（用UDP协议）
*/
int main(int argc,char**argv)
{
	//创建套接字
	int sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd == -1)
	{
		perror("socket error");
		return -1;
	}
	perror("socket");
	
	//设置目的地址结构体,组播IP地址
	struct sockaddr_in dest;
	bzero(&dest,sizeof(dest));
	dest.sin_family=AF_INET;
	dest.sin_port=htons(atoi(argv[1]));//服务端的端口号
	dest.sin_addr.s_addr=inet_addr("224.1.1.1");
	
	
	char buf[128]={0};
	while(1)
	{
		gets(buf);
		//发送
		int ret=sendto(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&dest,sizeof(dest));
		if(ret == -1)
		{
			perror("sendto error");
			return -1;
		}
		perror("sendto");
	}
	
	//关闭套接字
	close(sockfd);
	
}



































































