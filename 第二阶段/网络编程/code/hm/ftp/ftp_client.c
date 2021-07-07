#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

//#include <pthread.h>

/*
	init_socket:创建一个套接字，进行初始化
	@ip :IP地址
	@port :端口号
	
	返回值：创建的套接字的描述符
*/
int init_socket(char*ip,char*port)
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
	server_addr.sin_port = htons(atoi(port));
	server_addr.sin_addr.s_addr = inet_addr(ip);
	
	//连接
	int ret = connect(client_fd,(struct sockaddr *)&server_addr,sizeof(server_addr));
	if(ret == -1)
	{
		perror("connect error:");
		return -1;
	}
	perror("connect:");
	
	return client_fd;
}

/*
	ls :把某个目录下的文件名发送给客户端
*/
int Recv_ls(int sock_fd)
{
	while(1)
	{
		char buf[128] = {0};
		int ret = recv(sock_fd,buf,sizeof(buf),0);
		if(ret <= 0)
		{
			perror("recv ls error:");
			break;
		}
		if(strcmp(buf,"over") == 0)
		{
			break;
		}
		printf("*****%s*****\n",buf);
	}
}
/*
	send_file:发送文件给服务端
*/
int send_file(int sock_fd,char*filename)
{
	struct stat st;
	stat(filename,&st);
	int filesize = st.st_size;
	int ret = send(sock_fd,&filesize,sizeof(int),0);
	if(ret <= 0)
	{
		perror("send error:");
		return -1;
	}
	
	int fd = open(filename,O_RDONLY);
	if(fd == -1)
	{
		perror("open error:");
		return -1;
	}
	
	int readsize = 0;
	char buf[1024] = {0};
	while(readsize < filesize)
	{
		ret = read(fd,buf,sizeof(buf));
		if(ret > 0)
		{
			readsize += ret;
			send(sock_fd,buf,ret,0);
		}
	}
	
	close(fd);
}
/*
	recv_file:接收来自服务端的文件
*/
int recv_file(int sock_fd,char*filename)
{
	int filesize;
	int ret = recv(sock_fd,&filesize,sizeof(int),0);
	if(ret <= 0)
	{
		perror("recv error:");
		return -1;
	}
	printf("filesize = %d\n",filesize);
	
	int fd = open(filename,O_RDWR | O_CREAT,0666);
	if(fd <= 0)
	{
		perror("open error:");
		return -1;
	}
	int readsize = 0;
	char buf[1024] = {0};
	while(readsize < filesize)
	{
		ret = recv(sock_fd,buf,sizeof(buf),0);
		if(ret > 0)
		{
			readsize += ret;
			write(fd,buf,ret);
		}
	}
	
	close(fd);
	
}
/*
	
*/
int Send_cmd(int client_fd)
{
	while(1)
	{
		char str[128] = {0};
		char cmd[128] = {0};
		char filename[128] = {0};
		printf("input :ls / get xx.xxx / put xx.xxx /quit\n");
		fgets(str,sizeof(str),stdin);
		send(client_fd,str,sizeof(str),0);
		sscanf(str,"%s %s",cmd,filename);
		
		if(strcmp(cmd,"ls") == 0)
		{
			//ls
			Recv_ls(client_fd);
		}
		else if(strcmp(cmd,"get") == 0)
		{
			//get
			recv_file(client_fd,filename);
		}
		else if(strcmp(cmd,"put") == 0)
		{
			//put
			send_file(client_fd,filename);
		}
		else if(strcmp(cmd,"quit") == 0)
		{
			break;
		}
		else
		{
			printf("no this cmd\n");
			continue;
		}
	}
	
}
int main(int argc,char**argv)
{
	int client_fd = init_socket(argv[1],argv[2]);
	
	//接受客户端的命令操作
	Send_cmd(client_fd);
}