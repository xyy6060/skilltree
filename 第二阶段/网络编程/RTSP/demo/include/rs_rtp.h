/******************************************************************************
 * Copyright (C), 2008-2011, Grandstream Co., Ltd.
 ******************************************************************************
 File Name     : gs_rtp.h
 Version       : Initial Draft
 Author        : Grandstream video software group
 Created       : 2009/11/19
 Last Modified :
 Description   :
 Function List :
 Note		   : created
 History       :
 1.Date        : 2009/11/23
   Author      : lwx
   Modification:
 ******************************************************************************/

#ifndef __GS_RTP_H__
#define __GS_RTP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include "rs_type.h"

#define 	DEFAULT_RTP_UDP_PORT		33767
#define 	RTP_DEFAULT_SSRC 			41030
#define 	RTP_MAX_SENDER				16
#define 	RTP_VERSION					2

#define RTP_HDR_SET_VERSION(pHDR, val)  ((pHDR)->version = val)
#define RTP_HDR_SET_P(pHDR, val)        ((pHDR)->p       = val)
#define RTP_HDR_SET_X(pHDR, val)        ((pHDR)->x       = val)
#define RTP_HDR_SET_CC(pHDR, val)       ((pHDR)->cc      = val)

#define RTP_HDR_SET_M(pHDR, val)        ((pHDR)->marker  = val)
#define RTP_HDR_SET_PT(pHDR, val)       ((pHDR)->pt      = val)

#define RTP_HDR_SET_SEQNO(pHDR, _sn)    ((pHDR)->seqno  = (_sn))
#define RTP_HDR_SET_TS(pHDR, _ts)       ((pHDR)->ts     = (_ts))

#define RTP_HDR_SET_SSRC(pHDR, _ssrc)    ((pHDR)->ssrc  = _ssrc)

#define RTP_HDR_LEN  					sizeof(RtpHdr_t)





#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

/* total 12Bytes */
typedef struct RtpHdr_s
{

#if (BYTE_ORDER == LITTLE_ENDIAN)
    /* byte 0 */
    u16 cc      :4;   /* CSRC count */
    u16 x       :1;   /* header extension flag */
    u16 p       :1;   /* padding flag */
    u16 version :2;   /* protocol version */

    /* byte 1 */
    u16 pt      :7;   /* payload type */
    u16 marker  :1;   /* marker bit */
#elif (BYTE_ORDER == BIG_ENDIAN)
    /* byte 0 */
    u16 version :2;   /* protocol version */
    u16 p       :1;   /* padding flag */
    u16 x       :1;   /* header extension flag */
    u16 cc      :4;   /* CSRC count */
    /*byte 1*/
    u16 marker  :1;   /* marker bit */
    u16 pt      :7;   /* payload type */
#else
    #error YOU MUST DEFINE BYTE_ORDER == LITTLE_ENDIAN OR BIG_ENDIAN !
#endif


    /* bytes 2, 3 */
    u16 seqno  :16;   /* sequence number */

    /* bytes 4-7 */
    int ts;            /* timestamp in ms */

    /* bytes 8-11 */
    int ssrc;          /* synchronization source */
} RtpHdr_t;


typedef struct Speco_vms_video_s
{
	u16 magicCode;
	char version;
	char headLen;
	char mediaIndicator;
	char frameType;
	u16 dataLen;
	unsigned char seqNo;
	char x1;
	char sensor;
	char motion;
	u16 Width;
	u16 Height;
	u16 frameRate;
	char vInfo;
	char x2;
	unsigned int frameLen;
	char date;
	char month;
	u16 year;
	char sec;
	char min;
	u16 hour;

	unsigned long long vtimeDiff;
} specoVmsVideoStart_t;

typedef struct Speco_vms_audio_s
{
	u16 magicCode;
	char version;
	char headLen;
	char mediaIndicator;
	char frameType;
	u16 dataLen;
	unsigned char seqNo;
	char x;
	char sensor;
	char motion;
} specoVmsAudioStart_t;

typedef struct Speco_vms_me_s
{
	u16 magicCode;
	char version;
	char headLen;
	char mediaIndicator;
	char frameType;
	u16 dataLen;
	unsigned char seqNo;
	char x;
} specoVmsMiddleEnd_t;
#define QUANT_TABLE_LEN			64
#define MJPEG_RTP_LEN			888//(892-4)   //ljh 复位标志占4字节

typedef struct JpegHdr_s{
	u32 		off:24; 		/* 分段偏移 */
	u32			tspec:8;		/* 类型特定 */
	u8			type;			/* 类型 */
	u8			q;				/* Q值 */
	u8			width;			/* 宽 */
	u8			height;			/* 高 */
}JpegHdr_t;

typedef struct JpegHdrQTable_s{
	u8			mbz;
	u8			precision;
	u8			length[2];
}JpegHdrQTable_t;

typedef struct QTable_s{
	int		len;
	char 	*src;
}QTalbe_t;

typedef struct JpegQTable_s{
	QTalbe_t	table[4];
	char        *scan_data;         /* JPEG码流 */
	int         scan_data_len;      /* JPEG码流长度 */
	u8          interval[2];        /* 复位间隔 */
}JpegQTable_t;

typedef struct JpegRestart_s{
	u8			interval[2];        /* 复位间隔 */
	u16			f:1;                /* F帧 */
	u16			l:1;                /* I帧 */
	u16			count:14;           /* 复位计数 */
}JpegRestart_t;

int RTP_AudioFrameSize(int audioFrameSize);
int  RTP_UdpSendVideoData(RtspSession_t *pSess);
int  RTP_UdpSendAudioData(RtspSession_t *pSess);
int  RTP_UdpRecvRTCP(RtspSession_t *pSess);
int RTP_UdpMulticastSendMediaData(RtspSession_t *pSess);


int  sock_close(int sock_fd);
int  Net_TcpRecvN(int sockFd, char *buf, int len);
int  Net_TcpSendN(int sockFd, char *buf, int size, RtspSession_t * pSess);
RtpUdpSender_t* RTP_CreateUdpSender(int channel,  RtpPt_e pt);
RtpUdpSender_t *RTP_CreateMulticastUdpSender(RtspSession_t *pSess,  RtpPt_e pt,  RtpStreamType_e streamType,  char *multicast_addr , int &port);



void RTP_UdpSendVideoPacket(RtpUdpSender_t *p, u32 pts, int market, char *data, int len, const int extHdrLen);
void RTP_UdpSendAudioPacket(RtpUdpSender_t *p, u32 pts, int market, int len ,char *data, const int extHdrLen);
int  RTP_TcpStartSendMediaData(RtspSession_t *pSess);
int  RTP_UdpStartSendMediaData(RtspSession_t *pSess);
int  RTP_TcpStartRecvBackChData(RtspSession_t *pSess);
int  RTP_UdpStartRecvBackChData(RtspSession_t *pSess);

int  RTP_CheckBase64Stat(int chn);
int  RTP_Base64Init(int chn);

int RTP_TcpPacket1(RtpTcpSender_t *pSender, RtpPt_e payloadType, u32 pts,
									int marker, const int len, char *data, int extHdrLen);

int  RTP_TcpPacket(RtspSession_t *pSess, RtpPt_e payloadType, u32 pts, int marker, const int len, char *data, int extHdrLen);
int  RTP_TcpSend(RtpTcpSender_t *pSender, int type, RtspSession_t * pSess);
void RTP_UdpSendVideo(int type, int chn, u32 pts, int frameType,
										time_t wTime, int len, char *data, RtspSession_t *pSess, const int extHdrLen);
int RTP_UdpPacket(RtpUdpSender_t *pSender, u32 pts, int marker,
													int len, char *data, const int extHdrLen);
int RTP_UdpSend(RtpUdpSender_t  *pSender);

RtpPt_e RTP_ParsePreAudioEncType(int audioEncType, u32* audioBitRate);

RtpPt_e RTP_ParseAudioEncType(int audioEncType, u32* audioBitRate);
int RTP_ParseVideoEncType(int videoEncType);

int  ES_TS_Initialize();
int  ES_TS_RTP_UdpSend(int type, int chn, u32 pts, int frameType, time_t wTime, int len, char *data);
char *GetVideoInfo(char *pSrc, int srclen, char nType, int *nallen, char flag);
/*获取预览帧信息*/
int rtp_get_preview_one_iframe(RtspSession_t *pSess ,char *pbuf, int len);
/*获取回放帧信息*/
int rtp_get_plyback_one_iframe(RtspSession_t *pSess,char *pbuf, int len);

void *RTSP_Onvif_StartmulticstThrFxn(void *pArgs);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __GS_RTP_H__ */

