#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#define MAX 128
/*
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
*/
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
	 int r = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,&a,sizeof(a));

	
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
	
	//创建用来保存需要监听的文件描述符和事件的结构体数组
	struct pollfd fds[MAX];
	
	//加入服务端的文件描述符和事件到数组
	fds[0].fd = server_fd;
	fds[0].events = POLLIN;//请求监听的事件是可读
	//fds[0].events = 0;
	int client[MAX];
	int cli_num = 0;
	int i =0;
	while(1)
	{
		//使用poll对数组进行监听
		 int r = poll(fds, MAX,5000);
		 if(r > 0)
		 {
			 //找到就绪的文件描述符
			 if(fds[0].revents  & POLLIN)
			 {
				 int client_fd = accept(server_fd,(struct sockaddr*)&client_addr,&len);
				 if(client_fd < 0)
				 {
					 perror("accept error:");
					 return -1;
				 }
				 client[cli_num] = client_fd;
				 cli_num++;
				 
				 fds[cli_num].fd = client_fd;
				 fds[cli_num].events = POLLIN;//请求监听的事件是可读
			 }
			 
			 //判断监听的客户端是否就绪
			 for(i = 1;i <= cli_num;i++)
			 {
				 if(fds[i].revents  & POLLIN)
				 {
					 char buf[128] = {0};
					 int re = read(fds[i+1].fd,buf,128);
					 printf("re = %d,buf = %s\n",re,buf);
						
				 }
			 }
			 
		 }
		 else if(r == 0)
		 {
			 printf("timeout\n");
		 }
		 else 
		 {
			 perror("poll error:");
			 return -1;
		 }
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