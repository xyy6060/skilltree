/**
 * 实现RTP传输H264的应用
 */
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define BUFFER_SIZE		(512 * 1024)

#define RTP_AAC			97
#define RTP_H264		(96)
#define RTP_MAX_SIZE	(1400)

#define H264_FPS		(25)
#define H264_INCREASE	(90000 / H264_FPS)

/**
 * AAC RTP包的时间戳计算:
 * 		假设音频的采样率为44100，即每秒钟采样44100次
 * 	AAC一般将1024次采样编码成一帧，所以一秒就有44100/1024=43帧
 * 	RTP包发送的每一帧数据的时间增量为44100/43=1025
 * 	每一帧数据的时间间隔为1000/43=23ms
 */
#define AAC_TIMESCALE	44100
#define AAC_FPS			(AAC_TIMESCALE / 1000)
#define AAC_INCREASE	(AAC_TIMESCALE / AAC_FPS)
#define AAC_TDELAY_US	(1000*1000 / AAC_FPS)

typedef struct {
	char media[256];	// 媒体文件路径
	char ipaddr[64];	// 目标IP地址
	int port;			// 目标端口号
} rtp_args_t;

/* RTP 数据头：12 bytes */
typedef struct {
	/* byte 0 */
	unsigned char cc:4;		// CSRC个数
	unsigned char x:1;		// 头扩展标志位
	unsigned char p:1;		// 四字节对齐标志位
	unsigned char v:2;		// rtp协议版本
	/* byte 1 */
	unsigned char pt:7;		// rtp负载类型
	unsigned char m:1;		// 标志位（视频帧的结束、一帧音频帧）
	/* byte 2-3 */
	unsigned short seq;		// rtp包的序列号
	/* byte 4-7 */
	unsigned int ts;		// rtp包的时间戳
	/* byte 8-11 */
	unsigned int ssrc;		// rtp的同步源（用来标识RTP的源）
} rtp_hdr_t;

typedef struct {
	unsigned char type:5;	// NALU帧类型
	unsigned char nri:2;	// NALU帧重要性
	unsigned char f:1;		// NALU帧是否出错
} nalu_type_t;

/**
 * AAC的RTP打包：rtp_hdr_t + AU-headers-length + n*AU-header + n*AU
 * 	AU-headers-length(2 bytes):	表示au-header的长度，单位是bit
 * 	AU-header(2 bytes):			高13个bits就是一个AU的字节长度，
 * 	AU：							aac去掉adts的剩余数据
 */
typedef struct {
	unsigned char au_hdrlen_h;
	unsigned char au_hdrlen_l;
	unsigned short size:13;
	unsigned short indexlength:3;
} aac_info_t;

/**
 * rtp分片封包中的 FU indicator
 * 	rtp分片封包中的RTP包：rtp_hdr + fu_ind + fu_hdr + data
 *
	序号		类型		解释
	24		STAP-A	单一时间的组合包
	25		STAP-B	单一时间的组合包
	26		MTAP16	多个时间的组合包
	27		MTAP24	多个时间的组合包
	28		FU-A	分片的单元
	29		FU-B	分片的单元
 */
typedef struct {
	unsigned char type:5;	// rtp的负载类型，当为1~23时，与NALU中的Type是一致的
	unsigned char nri:2;	// rtp负载重要性
	unsigned char f:1;		// rtp负载是否出错
} fu_indicator_t;

typedef struct {
	unsigned char type:5;	// NALU帧类型，同nalu_type_t type
	unsigned char r:1;		// 保留
	unsigned char e:1;		// 为1表示分片的结束
	unsigned char s:1;		// 为1表示分片的开始
} fu_header_t;


static void help(const char *app)
{
	printf("Usage: %s <*.h264> <ipaddr> <vport> <*.aac> <aport>\n", app);
}

/**
 * 解析程序参数
 */
static void analyze_argv(char **argv, rtp_args_t *rtp_video_args, rtp_args_t *rtp_audio_args)
{
	bzero(rtp_video_args->media, 256);
	bzero(rtp_video_args->ipaddr, 64);
	bzero(rtp_audio_args->media, 256);
	bzero(rtp_audio_args->ipaddr, 64);

	// analayze video params
	memcpy(rtp_video_args->media, argv[1], strlen(argv[1]));
	memcpy(rtp_video_args->ipaddr, argv[2], strlen(argv[2]));
	rtp_video_args->port = atoi(argv[3]);

	// analayze audio params
	memcpy(rtp_audio_args->media, argv[4], strlen(argv[4]));
	memcpy(rtp_audio_args->ipaddr, argv[2], strlen(argv[2]));
	rtp_audio_args->port = atoi(argv[5]);

	printf("####################################\n");
	printf("# %s args:\n", argv[0]);
	printf("#    rtp_video_args->media:  %s\n",	rtp_video_args->media);
	printf("#    rtp_video_args->ipaddr: %s\n",	rtp_video_args->ipaddr);
	printf("#    rtp_video_args->port:   %d\n",	rtp_video_args->port);
	printf("#    rtp_audio_args->media:  %s\n",	rtp_audio_args->media);
	printf("#    rtp_audio_args->ipaddr: %s\n",	rtp_audio_args->ipaddr);
	printf("#    rtp_audio_args->port:   %d\n",	rtp_audio_args->port);
	printf("####################################\n");
}

/**
 * @function 获取一帧视频数据
 * 
 * @param fd		h264文件句柄
 * @param buffer	帧数据缓冲区
 * @return 返回该帧的数据大小，失败则返回负数
 */
static int get_a_vframe(const int fd, unsigned char *buffer)
{
	int start_code = 0;
	int length = -1, buf_pos = 0, calibration = 0;
	int size = 0;
	
	unsigned char *tmp_buffer = (unsigned char *)calloc(1, BUFFER_SIZE);
	if(!tmp_buffer) {
		printf("(%s +%d): calloc() failed!\n", __func__, __LINE__);
		return -1;
	}

	length = read(fd, tmp_buffer, BUFFER_SIZE);
	if(length < 0) {
		perror("read() failed!: ");
		free(tmp_buffer);
		return -2;
	}

	/* 切割视频帧 */
	while(buf_pos < length) {
		/* 查看NALU起始码：00 00 01 / 00 00 00 01 */
		if(	tmp_buffer[buf_pos++] == 0x00 
			&& tmp_buffer[buf_pos++] == 0x00) {

			/* 01 */
			if(tmp_buffer[buf_pos++] == 0x01) {
				start_code = 3;
				goto gotnal_head;
			} else {
				buf_pos--;
			}

			/* 00 01 */
			if(	tmp_buffer[buf_pos++] == 0x00 
				&& tmp_buffer[buf_pos++] == 0x01) {
				start_code = 4;
				goto gotnal_head;
			}
		} else {
			continue;
		}

gotnal_head:
		/* NALU的尾部正是下一个NALU的头部，找出下一个NALU，这儿得到这个NALU长度、类型、数据 */
		calibration = buf_pos;
		while (calibration < length) {	
			if( tmp_buffer[calibration++] == 0x00 
				&& tmp_buffer[calibration++] == 0x00){

				/* 01 */
				if(tmp_buffer[calibration++] == 0x01) {
					size = (calibration - 3) - buf_pos;
					break;
				} else {
					calibration--;
				}

				//
				if(tmp_buffer[calibration++] == 0x00 
					&& tmp_buffer[calibration++] == 0x01) {
					size = (calibration - 4) - buf_pos;
					break;
				}
			}
		}

		if(calibration == length){
			/* 不是一个完整的帧，该视频已经播完了 */
			break;;
		}

		memcpy(buffer, tmp_buffer+buf_pos, size);
		lseek(fd, -(length - size - start_code), SEEK_CUR);
		break;
		

	}

	
	free(tmp_buffer);
	return size;
}

/**
 * @function		获取一帧音频数据
 * @param fd		AAC文件句柄
 * @param buffer	帧数据缓冲区
 * 
 * @return	返回该帧数据大小，失败则返回负数
 */
static int get_a_aframe(const int fd, unsigned char *buffer)
{
	int length = -1;
	int size = 0;
	unsigned char *tmp_buffer = (unsigned char *)calloc(1, BUFFER_SIZE);
	if(!tmp_buffer) {
		printf("calloc() failed!\n");
		return -1;
	}

	length = read(fd, tmp_buffer, BUFFER_SIZE);
	if(length < 7){
		perror("read() failed: ");
		free(tmp_buffer);
		return -2;
	}
	
	// 查找ADTS头
	if((tmp_buffer[0] == 0xff) && ((tmp_buffer[1] & 0xf0) == 0xf0) ) {	//syncword总为0xFFF
		size |= ((tmp_buffer[3] & 0x03) << 11); //high 2 bit
		size |= tmp_buffer[4] << 3; 			//middle 8 bit
		size |= ((tmp_buffer[5] & 0xe0) >> 5);	//low 3bit
	} else {
		printf("not an aac file!\n");
		return -3;
	}

	memcpy(buffer, (tmp_buffer + 7), (size - 7));
	lseek(fd, -(length - size), SEEK_CUR);

	free(tmp_buffer);
	return (size - 7);
}

/**
 * @function			填充RTP数据包头并发送
 * @param buffer		待填充的rtp包（视频数据已就位）
 * @param data_size		rtp包中h264的大小
 * @param rtp_socket	本地rtp客户端网络套接字
 * @param server		对端rtp服务器信息
 * 
 * @return	返回该帧数据大小，失败则返回负数
 */
static int vpacket2rtp(unsigned char *buffer, const int data_size, const int rtp_socket, struct sockaddr_in server)
{
	int ret = -1;
	int packet_size = 0;
	static unsigned short seqnum = 0;
	static unsigned int ts = 0;
	
	rtp_hdr_t *rtp_hdr = (rtp_hdr_t *)&buffer[0];
	rtp_hdr->v			= 2;
	rtp_hdr->m			= 0;
	rtp_hdr->pt			= RTP_H264;
	rtp_hdr->ssrc		= htonl(1111);

	ts += H264_INCREASE;

	/**
	 * h264:
	 *	0		  1~
	 * nalu头	 nalu数据
	 * 
	 * 
	 * RTP 单一NALU模式 packet:
	 *	0~11	  12		  13~
	 * rtp_hdr	fu_ind		h264data
	 * 
	 * 
	 * RTP 分片封包模式 packet:
	 *	0~11	  12		  13		  14~
	 * rtp_hdr	fu_ind		fu_hdr		h264data
	 */
	if(data_size <= RTP_MAX_SIZE) {	// RTP 单一NALU模式
		rtp_hdr->m		= 1;
		rtp_hdr->seq	= htons(seqnum++);
		rtp_hdr->ts		= htonl(ts);

		packet_size = data_size + sizeof(rtp_hdr_t);
		ret = sendto(rtp_socket, buffer, packet_size, 0, (struct sockaddr *)&server, sizeof(server));
		if(ret < 0) {
			perror("sendto() failed: ");
		}

	} else if(data_size > RTP_MAX_SIZE) {	// RTP 分片封包模式
		int i = 0;	// 用于循环计数
		int count = data_size / RTP_MAX_SIZE;		// 拆分成多少个rtp包
		int remain = data_size % RTP_MAX_SIZE;		// 最后一个rtp包的数据大小
		unsigned char packet[1500];	// 用于发送的RTP包

		nalu_type_t *nalu_hdr = (nalu_type_t *)(buffer + sizeof(rtp_hdr_t));
		fu_indicator_t *fu_ind;
		fu_header_t *fu_hdr;

		rtp_hdr->ts		= htonl(ts);

		while(i <= count) {
			bzero(packet, sizeof(packet));
			rtp_hdr->seq = htons(seqnum++);

			if(i == 0) {// 第一个RTP包
				rtp_hdr->m = 0;
				memcpy(packet, rtp_hdr, sizeof(rtp_hdr_t));
			
				fu_ind = (fu_indicator_t *)&packet[12];
				fu_ind->f = nalu_hdr->f;
				fu_ind->nri = nalu_hdr->nri;
				fu_ind->type = 28;// FU-A
	
				fu_hdr = (fu_header_t *)&packet[13];
				fu_hdr->e = 0;
				fu_hdr->r = 0;
				fu_hdr->s = 1;
				fu_hdr->type = nalu_hdr->type;

				memcpy(&packet[14], (buffer + 13), RTP_MAX_SIZE);
				packet_size = RTP_MAX_SIZE + 14;

				ret = sendto(rtp_socket, packet, packet_size, 0, (struct sockaddr *)&server, sizeof(server));
				if(ret < 0) {
					perror("sendto() failed: ");
				}
				i++;
	
			} else if(count == i) {// 最后一个RTP包
				rtp_hdr->m = 1;
				memcpy(packet, rtp_hdr, sizeof(rtp_hdr_t));
				
				fu_ind = (fu_indicator_t *)&packet[12]; 
				fu_ind->f = nalu_hdr->f;
				fu_ind->nri = nalu_hdr->nri;
				fu_ind->type = 28;// FU-A
	
				fu_hdr = (fu_header_t *)&packet[13];
				fu_hdr->r = 0;
				fu_hdr->s = 0;
				fu_hdr->type = nalu_hdr->type;
				fu_hdr->e = 1;

				memcpy(&packet[14], (buffer + 13 + i*RTP_MAX_SIZE), remain);
				packet_size = remain + 14;

				ret = sendto(rtp_socket, packet, packet_size, 0, (struct sockaddr *)&server, sizeof(server));
				if(ret < 0) {
					perror("sendto() failed: ");
				}
				i++;
				
			} else if(i<count && i!=0) {
				rtp_hdr->m = 0;
				memcpy(packet, rtp_hdr, sizeof(rtp_hdr_t));
	
				fu_ind = (fu_indicator_t *)&packet[12]; 
				fu_ind->f = nalu_hdr->f;
				fu_ind->nri = nalu_hdr->nri;
				fu_ind->type = 28;// FU-A
				
				fu_hdr = (fu_header_t *)&packet[13];
				fu_hdr->r = 0;
				fu_hdr->s = 0;
				fu_hdr->e = 0;
				fu_hdr->type = nalu_hdr->type;
				
				memcpy(&packet[14], (buffer + 13 + i*RTP_MAX_SIZE), RTP_MAX_SIZE);
				packet_size = RTP_MAX_SIZE + 14;

				ret = sendto(rtp_socket, packet, packet_size, 0, (struct sockaddr *)&server, sizeof(server));
				if(ret < 0) {
					perror("sendto() failed: ");
				}
				i++;
			}
		}
	}

	return packet_size;
}

/**
 * @function			填充RTP头
 * @param buffer		待填充的rtp包
 * @param data_size		rtp包中AAC的大小
 * @param rtp_socket	rtp local socket
 * @param server		rtp remote socket
 * 
 * @return	返回该帧数据大小，失败则返回负数
 */
static int apacket2rtp(unsigned char *buffer, const int data_size, const int rtp_socket, struct sockaddr_in server)
{
	int ret = -1;
	int packet_size = 0;
	static unsigned short seqnum = 0;
	static unsigned int ts = 0;
	
	ts += AAC_INCREASE;
	/**
	 * AAC数据一般只有几百个字节，所以只需要单一包发送方式即可
	 */
	rtp_hdr_t *rtp_hdr = (rtp_hdr_t *)&buffer[0];
	rtp_hdr->v			= 2;
	rtp_hdr->m			= 1;
	rtp_hdr->pt			= RTP_AAC;
	rtp_hdr->ssrc		= htonl(2222);
	rtp_hdr->seq		= htons(seqnum++);
	rtp_hdr->ts			= htonl(ts);

	aac_info_t *aac_info = (aac_info_t *)(buffer + sizeof(rtp_hdr_t));
	aac_info->au_hdrlen_h = 0;
	aac_info->au_hdrlen_l = 0x10;
	aac_info->size	= htonl(data_size);
	aac_info->indexlength = 0;

	packet_size = data_size + sizeof(rtp_hdr_t) + sizeof(aac_info_t);
	ret = sendto(rtp_socket, buffer, packet_size, 0, (struct sockaddr *)&server, sizeof(server));
	if(ret < 0) {
		perror("sendto() failed: ");
	}

	return packet_size;
}

static void *rtp_video_thread(void *args)
{
	int ret = 0;
	rtp_args_t *rtp_video_args = (rtp_args_t *)args;
	int fd = 0;					// 媒体文件句柄
	int rtp_socket = -1;		// 本地的socket（客户端）
	struct sockaddr_in server;	// 目标socket（服务器）
	unsigned char *buffer = NULL;
	

	/* 打开一个媒体文件 */
	fd = open(rtp_video_args->media, O_RDONLY);
	if(fd < 0) {
		perror("open() failed: ");
		goto __exit;
	}

	buffer = (unsigned char *)calloc(1, BUFFER_SIZE);
	if(!buffer) {
		printf("calloc() failed");
		goto __exit;
	}
	
	/* 与对端建立一个socket连接 */
	rtp_socket = socket(AF_INET, SOCK_DGRAM, 0);	// IPv4 数据报（UDP）

	server.sin_family 		= AF_INET;								// IPv4
	server.sin_port 		= htons(rtp_video_args->port);			// 端口号
	server.sin_addr.s_addr 	= inet_addr(rtp_video_args->ipaddr);	// IP地址
	ret = connect(rtp_socket, (struct sockaddr *)&server, sizeof(server));
	if(ret < 0){
		perror("connect() failed: ");
		goto __exit;
	}
	
	while(1) {
		/* 取一帧数据 */
		bzero(buffer, BUFFER_SIZE);
		ret = get_a_vframe(fd, buffer+sizeof(rtp_hdr_t));
		if(ret <= 0) {
			goto __exit;
		}

#if 0
		nalu_type_t *nalu_type;
		nalu_type = (nalu_type_t *)(buffer+sizeof(rtp_hdr_t));
		printf("ret = %d, type = %X\n", ret, nalu_type->type);
#endif
		/* rtp包的封装并发送数据 */
		ret = vpacket2rtp(buffer, ret, rtp_socket, server);
		usleep(40 * 1000);
	}

__exit:
	if(fd > 0)
		close(fd);
	
	if(buffer)
		free(buffer);

	if(rtp_socket > 0)
		close(rtp_socket);

	return NULL;
}

static void *rtp_audio_thread(void *args)
{
	int ret = 0;
	rtp_args_t *rtp_video_args = (rtp_args_t *)args;
	int fd = 0;					// 媒体文件句柄
	int rtp_socket = -1;		// 本地的socket（客户端）
	struct sockaddr_in server;	// 目标socket（服务器）
	unsigned char *buffer = NULL;
	

	/* 打开一个媒体文件 */
	fd = open(rtp_video_args->media, O_RDONLY);
	if(fd < 0) {
		perror("open() failed: ");
		goto __exit;
	}

	buffer = (unsigned char *)calloc(1, BUFFER_SIZE);
	if(!buffer) {
		printf("calloc() failed");
		goto __exit;
	}
	
	/* 与对端建立一个socket连接 */
	rtp_socket = socket(AF_INET, SOCK_DGRAM, 0);	// IPv4 数据报（UDP）

	server.sin_family 		= AF_INET;								// IPv4
	server.sin_port 		= htons(rtp_video_args->port);			// 端口号
	server.sin_addr.s_addr 	= inet_addr(rtp_video_args->ipaddr);	// IP地址
	ret = connect(rtp_socket, (struct sockaddr *)&server, sizeof(server));
	if(ret < 0){
		perror("connect() failed: ");
		goto __exit;
	}

	while(1) {
		bzero(buffer, BUFFER_SIZE);
		ret = get_a_aframe(fd, (buffer + sizeof(rtp_hdr_t) + sizeof(aac_info_t)));
		if(ret <= 0) {
			goto __exit;
		}
		
		ret = apacket2rtp(buffer, ret, rtp_socket, server);
		usleep(AAC_TDELAY_US);
	}

__exit:
	if (fd > 0)
		close(fd);
	if(buffer)
		free(buffer);
	if(rtp_socket > 0)
		close(rtp_socket);

	return NULL;
}

int main(int argc, char **argv)
{
	int ret = 0;
	rtp_args_t rtp_video_args, rtp_audio_args;
	
	if(argc < 6) {
		help(argv[0]);
		return 0;
	}

	// 程序参数解析
	analyze_argv(argv, &rtp_video_args, &rtp_audio_args);
	
#if 1
	// rtp视频的推流
	pthread_t video;
	ret = pthread_create(&video, NULL, rtp_video_thread, &rtp_video_args);
	if(ret < 0) {
		printf("pthread_create() rtp_video_thrad failed!\n");
		return -1;
	}
#endif
	
#if 1
		// rtp音频的推流
		pthread_t audio;
		ret = pthread_create(&audio, NULL, rtp_audio_thread, &rtp_audio_args);
		if(ret < 0) {
			printf("pthread_create() rtp_audio_thrad failed!\n");
			return -1;
		}
#endif



	pause();

	return 0;
}

