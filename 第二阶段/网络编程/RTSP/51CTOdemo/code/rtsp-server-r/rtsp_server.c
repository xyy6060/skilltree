#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define MAX_RTSP_CLIENT		1		// 最多与1个客户端同时通信
#define RTSP_RECV_SIZE      1024	// 最大信息长度
#define PARAM_STRING_MAX    100		// 参数最大长度

typedef enum {
	RTSP_IDLE		= 0,	// 空闲，当前没有与客户端对接
	RTSP_CONNECTED	= 1,	// 连接，已经和客户端对接上了
	RTSP_SENDING	= 2,	// 发送，正处于媒体发送状态
} RTSP_STATUS;

typedef struct {
	int rtsp_port;
} rtsp_args_t;

typedef struct
{
	int index;			// 客户端的序号
	int socket;			// 客户端socket
	int status;			// 当前与客户端的状态
	int sessionid;		// 
	int rtpport;		// 
	int rtcpport;		// 
	char sin_addr[32];	// 客户端的ip地址
} rtsp_client_t;

static rtsp_client_t rtsp_clients[MAX_RTSP_CLIENT];

#define LOG_INFO(fmt, args...)	printf("(+%d %s): "fmt"\n", __LINE__, __func__, ##args)

/**
 * @function 初始化服务器socket并返回
 * @param rtsp_port 需要被监听的服务器端口
 * 
 * @return 返回初始化后的本地服务器socket
 */
static int server_init(int rtsp_port)
{
	int ret;
	int server_socket;
	int opt = 1;
	struct sockaddr_in server_addr;
	
	memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;					// 地址族
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 32位IP地址
    server_addr.sin_port = htons(rtsp_port); 			// 16位TCP/UDP端口号

	server_socket = socket(AF_INET, SOCK_STREAM, 0);	// SOCK_STREAM: 提供面向连接的稳定数据传输，即TCP协议。

	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
        return -1;
    }
	
    ret = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    if(ret < 0) {
		LOG_INFO("bind() failed: %d, %s", ret, strerror(errno));
        return -2;
    }
	
    ret = listen(server_socket, 50);   
    if(ret < 0) {
		LOG_INFO("listen() failed: %d, %s", ret, strerror(errno));
		return -3;
    }

	return server_socket;
}


static void * rtsp_client_msg(void *args)
{
	pthread_detach(pthread_self());
	
	int ret;
	char buffer[RTSP_RECV_SIZE];
	rtsp_client_t * pclient = (rtsp_client_t *)args;
	memset(buffer, 0, sizeof(buffer));
	
	LOG_INFO("rtsp client(%s) created!", pclient->sin_addr);
	while(pclient->status != RTSP_IDLE) {
		ret = recv(pclient->socket, buffer, RTSP_RECV_SIZE, 0);
		if(ret < 1) {
			LOG_INFO("rtsp client(%s) recv failed: %d, %s", pclient->sin_addr, ret, strerror(errno));
			rtsp_clients[pclient->index].status = RTSP_IDLE;
			close(pclient->socket);
			break;
		}


		LOG_INFO("request ########################");
		printf("%s", buffer);
		LOG_INFO("request ########################");
	}
	return NULL;
}

static void *rtsp_server_listen(void *args)
{
	rtsp_args_t *prtsp_args = args;
	int sessionid = 1000;
	
	// server
	int server_socket = server_init(prtsp_args->rtsp_port);
	
	// client
	int client_socket;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(struct sockaddr_in);
	
    while ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) >= 0)
    {
		int i = 0;

		LOG_INFO("rtsp client(%s) accepted!\n", inet_ntoa(client_addr.sin_addr));
		for(; i<MAX_RTSP_CLIENT; i++) {
			if(rtsp_clients[i].status == RTSP_IDLE) {
				memset(&rtsp_clients[i], 0, sizeof(rtsp_client_t));
				rtsp_clients[i].index		= i;
				rtsp_clients[i].socket		= client_socket;
				rtsp_clients[i].status		= RTSP_CONNECTED;
				rtsp_clients[i].sessionid	= sessionid++;
				strcpy(rtsp_clients[i].sin_addr, inet_ntoa(client_addr.sin_addr));
				
				pthread_t thread = 0;
				pthread_create(&thread, NULL, rtsp_client_msg, &rtsp_clients[i]);
				
				break;
			}
		}
    }

	return NULL;
}


static void help(const char *appname)
{
	printf("Usage: %s <rtsp port(default:554)>\n", appname);
}


int main(int argc, char **argv)
{
	rtsp_args_t rtsp_args;
	pthread_t thread_rtsp = 0;

	if(argc < 2) {
		help(argv[0]);
		return 0;
	} else {
		rtsp_args.rtsp_port = atoi(argv[1]);
	}
	
	pthread_create(&thread_rtsp, NULL, rtsp_server_listen, &rtsp_args);

	pause();

	return 0;
}

