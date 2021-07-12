#ifndef __RTSP_H__
#define __RTSP_H__

#define TRUE	1
#define FALSE	0

#define LOG_INFO(fmt, args...)	printf("(+%d %s): "fmt"\n", __LINE__, __func__, ##args)

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

int response_options(char *cseq, int sock);

int response_describe(char *cseq, int sock);

int response_setup(char *cseq, int sock, int sessionid, char* recvbuf, int* rtpport, int* rtcpport);


int response_play(char *cseq, int sock, int sessionid, char* recvbuf);


int response_pause(char *cseq, int sock, char *recvbuf);


int response_teardown(char *cseq,int sock,int sessionid,char *recvbuf);







#endif
