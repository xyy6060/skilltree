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
	fd_set  readfd,tmpfd;//用来保存需要监听 的读的描述符. readfd :存储未被标记的文件描述符
	FD_ZERO(&readfd);
	FD_SET(server_fd,&readfd);
	int maxfd = server_fd;//用来保存最大的文件描述符
	struct timeval timeout;//保存溢出的时间
	
	int i;
	while(1)
	{
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		tmpfd = readfd;
		int r = select(maxfd+1,&tmpfd,NULL,NULL, &timeout);
		if(r > 0)//表示有文件描述符就绪  客户端就绪 ：有可读的数据   服务端 ：有客户端来链接
		{
			for(i = 3;i <= maxfd;i++)  //一个个检测哪个文件描述符准备就绪
			{
				if(FD_ISSET(i,&tmpfd))
				{
					if(i == server_fd)//服务器的文件描述符
					{
						int client_fd = accept(server_fd,(struct sockaddr*)&client_addr,&len);
						FD_SET(client_fd,&readfd);//把新的客户端的文件描述符加入到readfd的集合中
						
						//最大的文件描述符可能被更新
						if(client_fd > maxfd)
							maxfd = client_fd;
					}
					else//接收客户端的数据
					{
						//一般在实际工作中通常会采用线程来对数据进行处理
						char buf[128] = {0};
						int re = read(i,buf,128);
						printf("re = %d,buf = %s\n",re,buf);
						
					}
				}
			}
		}
		else if(r == 0)//时间溢出  在5内一直都没有文件描述符是可读（客户端）或 可连接（服务端）
		{
			printf("timeout\n");
		}
		else
		{
			perror("select error:");
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