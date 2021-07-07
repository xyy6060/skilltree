//接收端
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
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
	
	//设置socket选项，加入组播
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr=inet_addr("224.1.1.1");//设置组播的IP地址
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);//本机IP地址
	
	int ret=setsockopt(sockfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq));
	if(ret == -1)
	{
		perror("setsockopt error");
		return -1;
	}
	perror("setsockopt");
	
	//设置套接字选项，允许IP重复使用
	int a = 1;
	ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT,&a,sizeof(a));
	
	//设置接收端的地址结构体
	struct sockaddr_in recv_addr;
	bzero(&recv_addr,sizeof(recv_addr));
	recv_addr.sin_family=AF_INET;
	recv_addr.sin_port=htons(atoi(argv[1]));
	recv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	//绑定，自己的IP地址
	ret = bind(sockfd,(struct sockaddr*)&recv_addr,sizeof(recv_addr));
	if(ret == -1)
	{
		perror("bind error");
		return -1;
	}
	perror("bind");
	
	struct sockaddr_in send_addr;
	socklen_t len=sizeof(send_addr);
	
	while(1)
	{
		char buf[128]={0};
		//接收
		ret=recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&send_addr,&len);
		if(ret == -1)
		{
			perror("recvfrom error");
			return -1;
		}
		printf("recv from ip: %s  data: %s\n",inet_ntoa(send_addr.sin_addr),buf);
	}
	
	//关闭套接字
	close(sockfd);
	
}



































