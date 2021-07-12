#ifndef __RTP_SEND_AV_H__
#define __RTP_SEND_AV_H__


typedef struct {
	char media[256];	// 媒体文件路径
	char ipaddr[64];	// 目标IP地址
	int port;			// 目标端口号
} rtp_args_t;


void analyze_argv(char **argv, rtp_args_t *rtp_video_args, rtp_args_t *rtp_audio_args);

void *rtp_video_thread(void *args);

void *rtp_audio_thread(void *args);


#endif
