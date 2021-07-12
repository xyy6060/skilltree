#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "rtsp.h"

/*********************** MICRO ****************************/
#define MAX_FIELD_NUM		5		// 最大字段个数
#define MAX_FIELD_SIZE		128		// 最大字段大小
#define MAX_MESSAGE_SIZE	1024	// 最大消息大小


/*********************** function ****************************/
static char const* get_fmt_time()
{
	static char str_time[128];
	time_t tt = time(NULL);

	bzero(str_time, 128);
	strftime(str_time, sizeof(str_time), "Date: %a, %b %d %Y %H:%M:%S GMT", gmtime(&tt));

	return str_time;
}

/**
 * 解析收到的request，得到其中的rtp端口和rtcp端口
 */
static void parse_transport_header(char const* buffer, int *rtpport, int *rtcpport)
{
	char const *title = "Transport: ";
	const int title_len = strlen(title);
	char field[MAX_FIELD_SIZE];
	
	/* 初始化为默认值 */
	*rtpport	= 5004;
	*rtcpport	= *rtpport + 1; 
	bzero(field, MAX_FIELD_SIZE);
   
	/* 1. 找到Transport字段 */
	while (1) {
		if (*buffer == '\0')
			return;
			
		if (strncasecmp(buffer, title, title_len) == 0)
			break;
		
		++buffer;
	}
   
	/* 2. 解析Transport字段值 */
	char const* fields = buffer + title_len;
	while (sscanf(fields, "%[^;]", field) == 1) {	// 正则表达式: 以;作为输入的结束标志
		if (sscanf(field, "client_port=%d-%d", rtpport, rtcpport) == 2) {
			LOG_INFO("client_port=%d-%d", *rtpport, *rtcpport);
				
		} else if (sscanf(field, "client_port=%d", rtpport) == 1) {
			LOG_INFO("client_port=%d", *rtpport);
		}

		fields += strlen(field);
	   
		while (*fields == ';')
			++fields;
		
		if (*fields == '\0' || *fields == '\r' || *fields == '\n')
			break;
	}
}

static int response_string(char *buffer, 
								const char *cseq, 
								const int field_num, const char field_array[][MAX_FIELD_SIZE], 
								const char *body)
{
	int i, ret;
	int curr_index = 0;
	char response_base[MAX_MESSAGE_SIZE];

	// 1. rtsp response base
	bzero(response_base, MAX_MESSAGE_SIZE);
	ret = sprintf(response_base, "RTSP/1.0 200 OK\r\nCSeq: %s\r\n", cseq);
	memcpy(buffer, response_base, ret);
	curr_index += ret;

	// 2. rtsp response field
	for(i=0; i<field_num; i++) {
		bzero(response_base, MAX_MESSAGE_SIZE);
		ret = sprintf(response_base, "%s\r\n", field_array[i]);
		memcpy((buffer+curr_index), response_base, ret);
		curr_index += ret;
	}

	// 3. rtsp response /r/n
	memcpy((buffer+curr_index), "\r\n", 2);
	curr_index += 2;
	
	// 4. rtsp response body
	if(body && (strlen(body) > 0)) {
		bzero(response_base, MAX_MESSAGE_SIZE);
		ret = sprintf(response_base, "%s\r\n", body);
		memcpy((buffer+curr_index), response_base, ret);
		curr_index += ret;
	}

	LOG_INFO("response ########################");
	printf("%s", buffer);
	LOG_INFO("response ########################");
	return curr_index;
}

int response_options(char *cseq, int sock)
{
	if (sock != 0) {
		int ret;
		char message[MAX_MESSAGE_SIZE];
		int field_num = 0;
		char field_array[MAX_FIELD_NUM][MAX_FIELD_SIZE];

		bzero(message, MAX_MESSAGE_SIZE);
		bzero(field_array, MAX_FIELD_NUM*MAX_FIELD_SIZE);
		
		sprintf(field_array[0], "%s", get_fmt_time());
		field_num++;
		
		sprintf(field_array[1], "Public: %s", "OPTIONS,DESCRIBE,SETUP,PLAY,PAUSE,TEARDOWN");
		field_num++;

		response_string(message, cseq, field_num, field_array, NULL);
		ret = send(sock, message, strlen(message), 0);
		if(ret <= 0) {
			LOG_INFO("send() failed: %d, %s", ret, strerror(errno));
			return FALSE;
		}

		return TRUE;
   }
   return FALSE;
}

int response_describe(char *cseq, int sock)
{
	if (sock != 0) {
		int ret;
		int field_num = 0;
		char field_array[MAX_FIELD_NUM][MAX_FIELD_SIZE];
		char message[MAX_MESSAGE_SIZE];
		char msg_sdp[MAX_MESSAGE_SIZE];	// sdp描述

		char *body = msg_sdp;
		bzero(message, MAX_MESSAGE_SIZE);
		bzero(body, MAX_MESSAGE_SIZE);
		bzero(field_array, MAX_FIELD_NUM*MAX_FIELD_SIZE);

		// 准备sdp - video(h264)
		body += sprintf(body, "%s\r\n", "m=video 5004 RTP/AVP 96");
		body += sprintf(body, "%s\r\n", "a=rtpmap:96 H264/90000");
		body += sprintf(body, "%s\r\n", "a=framerate:25");
		// 准备sdp - audio(aac)
		body += sprintf(body, "%s\r\n", "m=audio 5006 RTP/AVP 97");
		body += sprintf(body, "%s\r\n", "a=rtpmap:97 MPEG4-GENERIC/44100/2");
		body += sprintf(body, "%s\r\n", "a=fmtp:97 profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3; config=1210");

		sprintf(field_array[0], "%s", get_fmt_time());
		field_num++;
		
		sprintf(field_array[1], "Content-Type: application/sdp");
		field_num++;
		
		sprintf(field_array[2], "Content-length: %zu", strlen(msg_sdp));
		field_num++;
		
		response_string(message, cseq, field_num, field_array, msg_sdp);
		ret = send(sock, message, strlen(message), 0);
		if(ret <= 0) {
			LOG_INFO("send() failed: %d, %s", ret, strerror(errno));
			return FALSE;
		}

		return TRUE;
   }

   return FALSE;
}

int response_setup(char *cseq, int sock, int sessionid, char* recvbuf, int* rtpport, int* rtcpport)
{
	if (sock != 0) {
		int ret;
		char message[MAX_MESSAGE_SIZE];
		int field_num = 0;
		char field_array[MAX_FIELD_NUM][MAX_FIELD_SIZE];
		
		bzero(message, MAX_MESSAGE_SIZE);
		bzero(field_array, MAX_FIELD_NUM*MAX_FIELD_SIZE);

		parse_transport_header(recvbuf, rtpport, rtcpport);
		
		sprintf(field_array[0], "%s", get_fmt_time());
		field_num++;
		
		sprintf(field_array[1], "Transport: RTP/AVP;unicast;destination=%s;client_port=%d-%d", 
				"", *rtpport, *rtcpport);
		field_num++;
		
		sprintf(field_array[2], "Session: %d", sessionid);
		field_num++;
		
		response_string(message, cseq, field_num, field_array, NULL);
		ret = send(sock, message, strlen(message), 0);
		if(ret <= 0) {
			LOG_INFO("send() failed: %d, %s", ret, strerror(errno));
			return FALSE;
		}

		return TRUE;
   }
   return FALSE;
}

int response_play(char *cseq, int sock, int sessionid, char* recvbuf)
{
   if (sock != 0) {
	   int ret;
	   char message[MAX_MESSAGE_SIZE];
	   int field_num = 0;
	   char field_array[MAX_FIELD_NUM][MAX_FIELD_SIZE];
	   
	   bzero(message, MAX_MESSAGE_SIZE);
	   bzero(field_array, MAX_FIELD_NUM*MAX_FIELD_SIZE);
	   
	   sprintf(field_array[0], "%s", get_fmt_time());
	   field_num++;
	   
	   sprintf(field_array[1], "Session: %d", sessionid);
	   field_num++;
	   
	   response_string(message, cseq, field_num, field_array, NULL);
	   ret = send(sock, message, strlen(message), 0);
	   if(ret <= 0) {
		   LOG_INFO("send() failed: %d, %s", ret, strerror(errno));
		   return FALSE;
	   }
	   
	   return TRUE;
   }
   return FALSE;
}

int response_pause(char *cseq, int sock, char *recvbuf)
{
   if (sock != 0) {
	   int ret;
	   char message[MAX_MESSAGE_SIZE];
	   int field_num = 0;
	   char field_array[MAX_FIELD_NUM][MAX_FIELD_SIZE];
	   
	   bzero(message, MAX_MESSAGE_SIZE);
	   bzero(field_array, MAX_FIELD_NUM*MAX_FIELD_SIZE);

	   sprintf(field_array[0], "%s", get_fmt_time());
	   field_num++;
	   
	   response_string(message, cseq, field_num, field_array, NULL);
	   ret = send(sock, message, strlen(message), 0);
	   if(ret <= 0) {
		   LOG_INFO("send() failed: %d, %s", ret, strerror(errno));
		   return FALSE;
	   }
	   
	   return TRUE;
   }
   return FALSE;
}

int response_teardown(char *cseq, int sock, int sessionid, char *recvbuf)
{
   if (sock != 0) {
	   int ret;
	   char message[MAX_MESSAGE_SIZE];
	   int field_num = 0;
	   char field_array[MAX_FIELD_NUM][MAX_FIELD_SIZE];
	   
	   bzero(message, MAX_MESSAGE_SIZE);
	   bzero(field_array, MAX_FIELD_NUM*MAX_FIELD_SIZE);
	   
	   sprintf(field_array[0], "%s", get_fmt_time());
	   field_num++;
	   
	   sprintf(field_array[1], "Session: %d", sessionid);
	   field_num++;
	   
	   response_string(message, cseq, field_num, field_array, NULL);
	   ret = send(sock, message, strlen(message), 0);
	   if(ret <= 0) {
		   LOG_INFO("send() failed: %d, %s", ret, strerror(errno));
		   return FALSE;
	   }
	   
	   return TRUE;
   }
   return FALSE;
}




