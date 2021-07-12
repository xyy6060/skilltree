#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#include "rtsp.h"
#include "rtp_send_av.h"

#define MAX_RTSP_CLIENT		5		// 最多与5个客户端同时通信
#define RTSP_RECV_SIZE      1024	// 最大信息长度
#define PARAM_STRING_MAX    100		// 参数最大长度


static rtsp_client_t rtsp_clients[MAX_RTSP_CLIENT];


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

/**
 * @function 解析rtsp请求字符串
 */
int parse_request_string(char const *req_string, int req_string_size,
						       char* method, int method_size,
						       char* cseq, int cseq_size) 
{
	int cur_index;	// 请求字符串的当前索引位置
	int i;
	int parseSucceeded = FALSE;
	
	/* 读取第一个 空格/tab 之前的字符串: 方法(method) */
	for (cur_index=0; (cur_index < method_size)&&(cur_index < req_string_size); cur_index++) {
		char ch = req_string[cur_index];
		if (ch == ' ' || ch == '\t') {
			parseSucceeded = TRUE;
			break;
		}
		method[cur_index] = ch;
	}
	method[cur_index] = '\0';
	if (!parseSucceeded)
		return FALSE;

	/* 跳过多余的 空格/tab 字符 */
	while (cur_index < req_string_size && (req_string[cur_index] == ' ' || req_string[cur_index] == '\t'))
		++cur_index;

	/* 跳过URL（rtsp://ipaddr:port 或者 rtsp:/） */
	for (i=cur_index; i<req_string_size; i++) {
		if ((req_string[i] == 'r' || req_string[i] == 'R')
			&& (req_string[i+1] == 't' || req_string[i+1] == 'T')
			&& (req_string[i+2] == 's' || req_string[i+2] == 'S')
			&& (req_string[i+3] == 'p' || req_string[i+3] == 'P')
			&& req_string[i+4] == ':' && req_string[i+5] == '/') {
			
			i += 6;
			if (req_string[i] == '/') {
				// rtsp://ipaddr:port
				++i;
				while(i < req_string_size && req_string[i] != '/' && req_string[i] != ' ' && req_string[i] != '\t')
					++i;
			} else {
				// rtsp:/
				--i;
			}
			
			cur_index = i;
			break;
		}
	}

	/* 读取CSeq */
	parseSucceeded = FALSE;
	for (i=cur_index; i<req_string_size; i++) {
		if (req_string[i] == 'C' && req_string[i+1] == 'S' && req_string[i+2] == 'e' &&
			req_string[i+3] == 'q' && req_string[i+4] == ':') {
			i += 5;
			
			int j;
			while (i < req_string_size && (req_string[i] ==  ' ' || req_string[i] == '\t'))
				i++;
			for (j = 0; j < cseq_size-1 && i < req_string_size; ++j,++i) {
				char ch = req_string[i];
				if (ch == '\r' || ch == '\n') {
					parseSucceeded = TRUE;
					break;
				}

				cseq[j] = ch;
			}
			cseq[j] = '\0';
			break;
		}
	}
	if (!parseSucceeded)
		return FALSE;

	return TRUE;
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
		
		char method[PARAM_STRING_MAX];
		char cseq[PARAM_STRING_MAX];

		parse_request_string(buffer, ret, method, sizeof(method), cseq, sizeof(cseq));

		LOG_INFO("request ########################");
		printf("%s", buffer);
		LOG_INFO("request ########################");
		LOG_INFO("method:%s, cseq:%s", method, cseq);

		if(strstr(method, "OPTIONS")) {
			response_options(cseq, pclient->socket);
			
		} else if(strstr(method, "DESCRIBE")) {
			response_describe(cseq, pclient->socket);
			
		} else if(strstr(method, "SETUP")) {
			int rtpport, rtcpport;
			response_setup(cseq, pclient->socket, pclient->sessionid, buffer, &rtpport, &rtcpport);

			rtsp_clients[pclient->index].rtpport = rtpport;
			rtsp_clients[pclient->index].rtcpport= rtcpport;
		} else if(strstr(method, "PLAY")) {
			response_play(cseq, pclient->socket, pclient->sessionid, buffer);
			rtsp_clients[pclient->index].status = RTSP_SENDING;
			LOG_INFO("Start Play, pclient->index = %d", pclient->index);
			
		} else if(strstr(method, "PAUSE")) {
			response_pause(cseq, pclient->socket, buffer);
			
		} else if(strstr(method, "TEARDOWN")) {
			response_teardown(cseq, pclient->socket, pclient->sessionid, buffer);
			rtsp_clients[pclient->index].status = RTSP_IDLE;
			close(pclient->socket);
		}

	}
	
	LOG_INFO("rtsp client(%s) exit!", pclient->sin_addr);
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
	printf("Usage: %s <rtsp port(default:554)> <*.h264> <client ipaddr> <vport> <*.aac> <aport>\n", appname);
}


int main(int argc, char **argv)
{
	int ret = 0;
	/* rtsp */
	rtsp_args_t rtsp_args;
	pthread_t thread_rtsp = 0;

	rtp_args_t rtp_video_args, rtp_audio_args;
	pthread_t video, audio;

	if(argc < 7) {
		help(argv[0]);
		return 0;
	} else {
		rtsp_args.rtsp_port = atoi(argv[1]);
		analyze_argv(&argv[2], &rtp_video_args, &rtp_audio_args);
	}
	
	/* rtsp服务器 */
	pthread_create(&thread_rtsp, NULL, rtsp_server_listen, &rtsp_args);
	if(ret < 0) {
		printf("pthread_create() rtsp_server_listen failed!\n");
		return -1;
	}

	/* rtp视频的推流 */
	ret = pthread_create(&video, NULL, rtp_video_thread, &rtp_video_args);
	if(ret < 0) {
		printf("pthread_create() rtp_video_thrad failed!\n");
		return -1;
	}
	
	/* rtp音频的推流 */
	ret = pthread_create(&audio, NULL, rtp_audio_thread, &rtp_audio_args);
	if(ret < 0) {
		printf("pthread_create() rtp_audio_thrad failed!\n");
		return -1;
	}

	pause();

	return 0;
}


