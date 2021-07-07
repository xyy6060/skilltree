#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
void*fun(void*arg)
{
	pthread_detach(pthread_self());
	
	int client_fd = *((int*)arg);
	
	char recv[128] = {0};
	//接收和发送
	while(1)
	{
		int ret = read(client_fd,recv,128);
		if(ret > 0)
		{
			printf("recv :%s\n",recv);
		}
		
	}
}

/*
	init_socket:创建一个套接字，进行初始化
	@ip :IP地址
	@port :端口号
	
	返回值：创建的套接字的描述符
*/
int init_socket(char*ip,char*port)
{
	//创建一个套接字
	
	int server_fd = socket(AF_INET,SOCK_STREAM,0);
	if(server_fd == -1)
	{
		perror("socket error:");
		return -1;
	}
	perror("socket:");
	
	//设置服务器的地址结构体
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(port));
	server_addr.sin_addr.s_addr = inet_addr(ip);
	
	//设置套接字的选项
	 int a = 1;
	 int r = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&a,sizeof(a));

	
	//绑定bind
	int ret = bind(server_fd, (struct sockaddr *)&server_addr,sizeof(server_addr));
	if(ret == -1)
	{
		perror("bind error:");
		return -1;
	}
	perror("bind :");
	
	//监听listen
	ret  = listen(server_fd, 3);
	if(ret == -1)
	{
		perror("listen error:");
		return -1;
	}
	perror("listen :");
	
	
	return server_fd;
}
/*
	Communication:接收客户端的请求并发生通信
	
*/
int Communication(int server_fd)
{
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	int client_fd[20];
	int i = 0;
	while(1)
	{
		client_fd[i] =  accept(server_fd, (struct sockaddr *)&client_addr, &len);
		if(client_fd[i] == -1)
		{
			perror("accept error:");
			return -1;
		}
		perror("accept :");
		printf("accept ip:%s,port : %d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
		
		pthread_t tid;
		pthread_create(&tid,NULL,fun,(void*)&client_fd[i]);
		
		i++;
	
	}
	
}
int main(int argc,char**argv)
{
	//创建一个套接字
	int server_fd = init_socket(argv[1],argv[2]);
	
	//对客户端的连接请求做出回应，发生通信
	Communication(server_fd);
	
	//关闭套接字
	close(server_fd);
	
}

//./client 192.168.31.171  8888