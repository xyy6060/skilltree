#ifndef __RTSP_DEMOLEE_H__
#define __RTSP_DEMOLEE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>

#include <string.h>
#include <time.h>

#include "commonthread.h"
//modCommon::AutoLock tauto(m_mgrlock);	//加锁

#if 1

#ifndef  u8
typedef unsigned char u8;
#endif

#ifndef  u16
typedef unsigned short u16;
#endif

#ifndef u32
typedef unsigned int u32;
#endif
#ifndef S32
typedef int S32;
#endif

typedef unsigned long long u64;

#endif



using namespace std;

#define RTSP_PARSE_OK 					0
#define RTSP_PARSE_IS_RESP  			-4
#define RTSP_PARSE_INVALID_OPCODE	 	-1
#define RTSP_PARSE_INVALID 				-2
#define RTSP_PARSE_ISNOT_RESP 			-3
#define RTSP_BAD_STATUS_BEGIN 			202

#define RTSP_STATUS_CONTINUE            100
#define RTSP_STATUS_OK                  200
#define RTSP_STATUS_ACCEPTED            202
#define RTSP_STATUS_BAD_REQUEST         400
#define RTSP_STATUS_METHOD_NOT_ALLOWED  405
#define RTSP_STATUS_BANDWIDTH_LACK    	453

#define RTP_DEFAULT_SSRC 	41030
#define MAX_RTP_LEN		(1600)
#define MAX_RTCP_LEN	(512)//2015.03.13
#define	NAL_FRAGMENTATION_SIZE		1425/*H264�����ķְ���λ����Ϊ1024*/

#define MAX_RTSP_BUF_LEN	4096
#define LISTENQ     500
#define EINTR              4
#define H264_STARTCODE_LEN    4 /* 00 00 00 01 */


const char *gRtspInvalidMethod = "Invalid Method";


#define RTSP_MAKE_RESP_CMD(req)		(req+100)

#define UCTRL_HEAD "MCTP/1.0"
#define RTSP_VERSION  	"RTSP/1.0"
#define RTSP_HDR_CSEQ 	"CSeq"
#define RTSP_LRLF 		"\r\n"
#define RTSP_SDP					"application/sdp"

#define RTSP_BACKCHANNEL            "Require: www.onvif.org/ver20/backchannel"


#define RTSP_HDR_TRANSPORT 			"Transport"


#define RTSP_METHOD_SETUP      "SETUP"
#define RTSP_METHOD_REDIRECT   "REDIRECT"
#define RTSP_METHOD_PLAY       "PLAY"
#define RTSP_METHOD_PAUSE      "PAUSE"
#define RTSP_METHOD_SESSION    "SESSION"
#define RTSP_METHOD_RECORD     "RECORD"
#define RTSP_METHOD_EXT_METHOD "EXT-"
#define RTSP_METHOD_OPTION     "OPTION"
#define RTSP_METHOD_OPTIONS    "OPTIONS"
#define RTSP_METHOD_DESCRIBE   "DESCRIBE"
#define RTSP_METHOD_GET_PARAM  "GET_PARAMETER"
#define RTSP_METHOD_SET_PARAM  "SET_PARAMETER"
#define RTSP_METHOD_HEARTBEAT  "HEARTBEAT"
#define RTSP_METHOD_TEARDOWN   "TEARDOWN"
#define RTSP_METHOD_INVALID	   "Invalid Method"


#define RTSP_HDR_ACCEPT 			"Accept"
#define RTSP_HDR_USER_AGENT 		"User-Agent"
#define RTSP_HDR_AUTHORIZATION		"Authorization"

#define H264_Get_NalType(c)  ( (c) & 0x1F )
#define H265_Get_NalType(c)  ( ((c)>>1) & 0x3F )


typedef enum{
	RTSP_SESS_PREVIEW,
	RTSP_SESS_PLYBACK
}RtspSessType_e;

typedef enum VodSvrType_{
	VOD_SVR_TYPE_RTSP,
	VOD_SVR_TYPE_HTTP,
	VOD_SVR_TYPE_MAX
}VodSvrType_e;

typedef struct 
{	
	char ch_rtpHead[40];
	char headlen;
   char *datapos;
   int  datalen;
   int  offset;
} rtp_send_info_t;


typedef struct PassWd_s
{
	char uname[32];
	char pwd  [32];
	int channel;
}PassWd_t;

 typedef enum VodSvrState{
	 VOD_SVR_STATE_INIT,
	 VOD_SVR_STATE_RUNNING,
	 VOD_SVR_STATE_STOP,
	 VOD_SVR_STATE_BUTT
 }VodSvrState_e;

#if 0
typedef struct _RtspServParamV2
{
	int rtspPort;
	char rtspUserName[32];
	char rtspUserPwd[32];
	void *pDevIntf;
	char AuthFlag;
	//char SimulatNum;
	//RTSP_AUTH_CB auth_cb;	/*add 2016.01.31 �ص�����*/
	void *userData;			/*add 2016.01.31 �ص���������*/
	//int get_data_timeout;
	//int  mtkJpegFlag;
	//int useHKUrlFlag;
	//_RtspServParamV2() : get_data_timeout(5),mtkJpegFlag(0),useHKUrlFlag(0)
	//{
	//}
}RtspServParam;
#endif

typedef enum RtpTransportType{
	RTP_TRANSPORT_TYPE_UDP,
	RTP_TRANSPORT_TYPE_TCP,
	RTP_TRANSPORT_TYPE_BUTT
}RtpTransportType_e;
/*
typedef enum VencType{
	VENC_TYPE_H264 = 0, 
	VENC_TYPE_H265 , 
	VENC_TYPE_MPEG4,
	VENC_TYPE_JPEG,
	VENC_TYPE_MAX, 
}VencType_e;
*/
typedef enum RtpStreamType{
	RTP_STREAM_VIDEO = 0,
	RTP_STREAM_AUDIO,
	RTP_APP_T,
	RTP_STREAM_METADATA,
	RTP_STREAM_MAX
}RtpStreamType_e;

typedef enum TrackId{
	TRACK_ID_VIDEO = 0,
	TRACK_ID_AUDIO,
	TRACK_ID_OTHER = 3,
	TRACK_ID_BACKCHANNEL,
	TRACK_ID_UNKNOWN
}TrackId_e;
/*
typedef enum ConType{
	CON_TYPE_RTSP = 0x200,
	CON_TYPE_UCTRL,
	CON_TYPE_HTTP,
	CON_TYPE_UNDEFINED
}ConType_e;
*/
typedef enum VodSessionState{
	RTSP_STATE_INIT		= 0,
	RTSP_STATE_READY,
	RTSP_STATE_PAUSE,	//2015.12.04
	RTSP_STATE_PLAY,
	RTSP_STATE_STOP,
	RTSP_TCP_EXIT,
	RTSP_STATE_BUTT
}VodSessionState_e;

typedef enum RtspReqMethod{
	RTSP_REQ_METHOD_SETUP = 0,
	RTSP_REQ_METHOD_DESCRIBE,
	RTSP_REQ_METHOD_REDIRECT,
	RTSP_REQ_METHOD_PLAY,
	RTSP_REQ_METHOD_PAUSE,
	RTSP_REQ_METHOD_SESSION,
	RTSP_REQ_METHOD_OPTIONS,
	RTSP_REQ_METHOD_RECORD,
	RTSP_REQ_METHOD_TEARDOWN,
	RTSP_REQ_METHOD_GET_PARAM,
	RTSP_REQ_METHOD_SET_PARAM,
	RTSP_REQ_METHOD_HEARTBEAT,
	RTSP_REQ_METHOD_EXTENSION,
	RTSP_REQ_METHOD_MAX
}RtspReqMethod_e;

typedef struct RtspMethod_s{
	const char  *describe;
	int		opcode;
}RtspMethod_t;


RtspMethod_t gRtspMethod[] =
{
    {RTSP_METHOD_PLAY,          RTSP_REQ_METHOD_PLAY},
    {RTSP_METHOD_PAUSE,         RTSP_REQ_METHOD_PAUSE},
    {RTSP_METHOD_DESCRIBE,      RTSP_REQ_METHOD_DESCRIBE},
    {RTSP_METHOD_SETUP,         RTSP_REQ_METHOD_SETUP},
    {RTSP_METHOD_REDIRECT,      RTSP_REQ_METHOD_REDIRECT},
    {RTSP_METHOD_SESSION,       RTSP_REQ_METHOD_SESSION},
    {RTSP_METHOD_OPTION,        RTSP_REQ_METHOD_OPTIONS},
    {RTSP_METHOD_OPTIONS,       RTSP_REQ_METHOD_OPTIONS},
    {RTSP_METHOD_TEARDOWN,      RTSP_REQ_METHOD_TEARDOWN},
    {RTSP_METHOD_RECORD,        RTSP_REQ_METHOD_RECORD},
    {RTSP_METHOD_GET_PARAM,     RTSP_REQ_METHOD_GET_PARAM},
    {RTSP_METHOD_SET_PARAM,     RTSP_REQ_METHOD_SET_PARAM},
    {RTSP_METHOD_HEARTBEAT,     RTSP_REQ_METHOD_HEARTBEAT},
    {RTSP_METHOD_EXT_METHOD,    RTSP_REQ_METHOD_EXTENSION},
    {0,                         RTSP_REQ_METHOD_MAX}
};

RtspMethod_t gRtspStatu[] =
{
    {"Continue", 100},
    {"OK", 200},
    {"Created", 201},
    {"Accepted", 202},
    {"Non-Authoritative Information", 203},
    {"No Content", 204},
    {"Reset Content", 205},
    {"Partial Content", 206},
    {"Multiple Choices", 300},
    {"Moved Permanently", 301},
    {"Moved Temporarily", 302},
    {"Bad Request", 400},
    {"Unauthorized", 401},
    {"Payment Required", 402},
    {"Forbidden", 403},
    {"Not Found", 404},
    {"Method Not Allowed", 405},
    {"Not Acceptable", 406},
    {"Proxy Authentication Required", 407},
    {"Request Time-out", 408},
    {"Conflict", 409},
    {"Gone", 410},
    {"Length Required", 411},
    {"Precondition Failed", 412},
    {"Request Entity Too Large", 413},
    {"Request-URI Too Large", 414},
    {"Unsupported Media Type", 415},
    {"Bad Extension", 420},
    {"Invalid Parameter", 450},
    {"Parameter Not Understood", 451},
    {"Conference Not Found", 452},
    {"Not Enough Bandwidth", 453},
    {"Session Not Found", 454},
    {"Method Not Valid In This State", 455},
    {"Header Field Not Valid for Resource", 456},
    {"Invalid Range", 457},
    {"Parameter Is Read-Only", 458},
    {"Internal Server Error", 500},
    {"Not Implemented", 501},
    {"Bad Gateway", 502},
    {"Service Unavailable", 503},
    {"Gateway Time-out", 504},
    {"RTSP Version Not Supported", 505},
    {"Extended Error:", 911},
    {0, RTSP_PARSE_INVALID_OPCODE}
};


typedef enum RtpPt_
{
    RTP_PT_ULAW             = 0,        /* mu-law */
    RTP_PT_GSM              = 3,        /* GSM */
    RTP_PT_G723             = 4,        /* G.723 */
    RTP_PT_ALAW             = 8,        /* a-law */
    RTP_PT_G722             = 9,        /* G.722 */
    RTP_PT_S16BE_STEREO     = 10,       /* linear 16, 44.1khz, 2 channel */
    RTP_PT_S16BE_MONO       = 11,       /* linear 16, 44.1khz, 1 channel */
    RTP_PT_MPEGAUDIO        = 14,       /* mpeg audio */
    RTP_PT_JPEG             = 26,       /* jpeg */
    RTP_PT_H261             = 31,       /* h.261 */
    RTP_PT_MPEGVIDEO        = 32,       /* mpeg video */
    RTP_PT_MPEG2TS          = 33,       /* mpeg2 TS stream */
    RTP_PT_H263             = 34,       /* old H263 encapsulation */
    //RTP_PT_PRIVATE          = 96,
    RTP_PT_H264             = 96,       /* hisilicon define as h.264 */
    RTP_PT_G726             = 97,       /* hisilicon define as G.726 */
    RTP_PT_ADPCM            = 98,       /* hisilicon define as ADPCM */
    
    RTP_PT_MPEG4			= 99,		/*xsf define as Mpeg4*/
	RTP_PT_H264_DW			= 100,		/*for DianWang*/
	RTP_PT_INVALID          = 127
}RtpPt_e;

typedef struct RtpTcpTicket_s{
	unsigned int		rtp;
	unsigned int		rtcp;
}RtpTcpTicket_t;


typedef struct RtpTcpSender_s{
	RtpTcpTicket_t 		interleaved[RTP_STREAM_MAX];
	unsigned long		audioG726Ssrc;
	unsigned long		audioG711Ssrc;
	unsigned long		videoH264Ssrc;
	unsigned short		rtpSeq[RTP_STREAM_MAX];
	unsigned long		rtpPts[RTP_STREAM_MAX];
	int					channel;
	int					tcpSockFd;
	unsigned char		sendBuf[MAX_RTP_LEN];
	int					sendLen;
}RtpTcpSender_t;


typedef struct
{
	string paramKey;
	string value;
}rtspParamPair_t;

typedef struct
{
	unsigned short Year; //2011
	unsigned char Month; //1 ~ 12
	unsigned char Day; //1 ~ 31
	unsigned char Hour; //0 ~ 23
	unsigned char Minute; //0 ~ 59
	unsigned char Second; //0 ~ 59
	unsigned char Reserved;
}DateTime;


typedef struct RtspPlayBack_s
{
	DateTime  startTime;			//rtsp request start time
	DateTime  endTime;			//rtsp request end time
	unsigned long 	  sTimeSec;			//play  start npt time ,second(units)
	unsigned long 	  eTimeSec;			//play  end  npt time,second(units)
	time_t 	  timediff;
	float	  scale;				//scale
	unsigned long		  rtcprtpseq; 		//��һ��RR�����Ѿ����յ�������к�
	unsigned long		  rtcprtplost;		//��һ��RR ���з���������ʧ�İ�������
	float	  rtcprtplostratio; 	//��ǰ������
	unsigned char		  ext_hdr_C;
	unsigned char		  ext_hdr_D;
	unsigned char		  ext_hdr_E;
	unsigned char		  rateCtrlFlag;
	//FsVisitor *pFsVisitor;
	//RfsFrameInterface *pFsVisitor;

	unsigned char         frist_flag;
	unsigned char         seekflag;
	unsigned long long    RealstartPts;
	float        s_time;
	int         e_time;
}RtspPlayBack_t;

typedef enum RtpSenderType{
	RTP_SENDER_UNICAST		= 0, 	/* ���� */
	RTP_SENDER_MULTICAST	= 1,	/* �鲥 */
	RTP_SENDER_BROADCAST	= 3,	/* �㲥 */
}RtpSenderType_e;

typedef struct RtpStat_s{
	u64				sendPkt;		/* �ѷ��͵İ����� */
	u64				sendByte;		/* �ѷ��͵��ֽ��� */
	u64				sendErr;		/* ����ʧ�ܵĴ��� */
	u64				recvPkt;		/* ���յİ����� */
	u64				recvByte;		/* ���յ��ֽ��� */
	u64				unavailable;	/*  */
	u64				bad;			/* packets that did not appear to be RTP */
	u64				discarded;		/* �����İ� */
	u64				timeoutCnt;		/* ��ʱ�Ĵ��� */
}RtpStat_t;

typedef enum AVType{
	AV_TYPE_VIDEO 		= 0,
	AV_TYPE_AUDIO,
	AV_TYPE_AV,
	AV_TYPE_BUTT
}AVType_e;
typedef enum PacketType{
	PACKET_TYPE_RAW 	= 0,	/* �ض�ý��ͷ���ͣ�ͷ��8���ֽ�,����TCP���� */
	PACKET_TYPE_RTP,			/* ��ͨRTP�����ʽ��ͷ12���ֽ� */
	PACKET_TYPE_RTP_STAP,		/* STAP-A�����ʽ������3���ֽڵľ���ͷ��ͷ��15���ֽ� */
	PACKET_TYPE_BUTT
}PacketType_e;

typedef struct RtpUdpSender_s{
	RtpPt_e				pt;
	u32					ssrc;
	u32					rtpSeq;		//rtp head Sequence
	u32					rtpPts;		//rtp head timestamp
	u32					rtcpFirstPts;//init  the same as rtpPts;
	RtpStat_t			stats;
	int					bActive;
	int					rtpPort;
	int					rtcpPort;
	int					remoteRtpPort;
	int					remoteRtcpPort;//2015.03.13
	int					channel;
	int					rtpSockFd;
	int					rtcpSockFd;//2015.03.13
	RtpSenderType_e		senderType;
	RtpTransportType_e	transportType;
	PacketType_e		pktType;
	AVType_e			avType;
	u8					sendBuf[MAX_RTP_LEN];
	u8					rtcpsendBuf[MAX_RTCP_LEN];//2015.03.13
	u32					sendLen;
	u32					rtcpsendLen;
	char				remoteIp[64];
}RtpUdpSender_t;


typedef struct RtpMulticastUdpInfo_s{
	unsigned char   clientnum;	
	unsigned char	is_create_thread;
	RtpUdpSender_t *  psender;
}RtpMulticastUdpInfo_t;

typedef enum tag_FRAMETYPE_E
{        
	FRAME_TYPE_I = 0,
	FRAME_TYPE_P,
	FRAME_TYPE_A,
    FRAME_TYPE_V,
	FRAME_TYPE_ALL,
	FRAME_TYPE_OTHERS
}FRAMETYPE_E;

typedef struct  tag_FRAMETYPE_S
{  
	FRAMETYPE_E enFrameType;
	unsigned int u32FrameLen;
	bool        bIsHasFrame;  /* if 1,has frame,else no */
	char          *ps8Buffer;
}FRAMEINFO_S;


struct RtspSession_t{
	//struct list_head 	rtspSessList;
	int					channel;
	u32					streamType;
	RtspSessType_e		rtspSessType;	// preview or playback
	RtpTransportType_e	transportType;
	char				sessId[16];
	char				ssrc[16];
	char				userAgent[128];
	char				uri[256];
	char                *pTmpRecv;
	char				recvBuf[MAX_RTSP_BUF_LEN];
	char				sendBuf[MAX_RTSP_BUF_LEN];
	char                tmpRecv[MAX_RTSP_BUF_LEN] ;
	char				hostIp[64];
	char				remoteIp[64];
	int					setupFlag[RTP_STREAM_MAX];
	int					rtspSockFd;
	int					inSize;
	int					remotePort;
	u32					curSessBW;
	//RtpTargetHost_t		*rtpTargets[RTP_STREAM_MAX];
	//RtpTargetHost_t		*rtcpTargets[RTP_STREAM_MAX];  //2015.03.13
	int					reqStreamFlag[RTP_STREAM_MAX];
	int					lastSendReq;
	u32					lastSendSeq;
	int					lastRecvSeq;
	pthread_t			tcpThrId;
	pthread_t			tcpAppId;

	pthread_t			udpThrId[3];//2015.03.26 ,udpThrId[0] video,udpThrId[1] Audio, 2,rtcpRec
	VodSvrType_e        svrType;
	VodSessionState_e	sessStat;
	char 				nonce[17];
	RtspPlayBack_t		pbInfo;//pPlayBackCtrl;		//playback control
	RtpTcpSender_t		*pRtpTcpSender;
	RtpUdpSender_t      *pRtpUdpSender[RTP_STREAM_MAX];
	int                 vencType;
	RtpPt_e 	    	audioEncType;
	char                APsendFlag;
	u32					mWidth;
	u32					mHeight;
	u32					mFrameRate;
	u32					mVideoBitRate;
	u32                 mAudioBitRate;
		u32             mAudioRealBitRate;
	FRAMEINFO_S         stFrameInfo;
	u32					PreVEncType;	//use for save previouse one frame encode type;judge the encode type weater to modified.
	int                 httpTunnelInputSocket;
    char                isHttpTunnel;
	char                RecvTimeOutCont;
    char                tCount;
	//sem_t               sval;
	int                 socketspair[2];

	u8					tcpTimeOut;
	//long 				heartBeatTimeout;//this timeout args is special for Hikvision CMS.
	pthread_mutex_t 	mRtspRtpSynchronized;
	u32                 videots;
	//u64                 fristvideots;
	u64                 LastPts;
	u64                 LastFramePts;
	u64                 NextSendTime;
	u64                 ptsdif;
	//RfsFrameInterface *ptmpFsVisitor;
	char                custom_rtsp;//1:speco vms , 2:speco nvr, 3: hk nvr, 5 dahua nvr(for 120 I interval jump sec)
	u32                 frameLen;
	u64                 uniqueID;
	u8                  speco_vms_seq[2];

	//rs_buffer::asyn_read *_rar;
	//rs_buffer::shared_buffer _rwb;

	char               is_metadatastream; //0:unsupport  , 1:support
	char               backchannel; //0:unsupport  , 1:support
	char               is_req_multicast;
	char               multicast_sessionrelease; //0 no, 1:yes
	char               multicast_setup_finish[RTP_STREAM_MAX]; //0 no, 1:yes
	//RtspOnvifMulticastParm multicast_info;
	u64					rtpPts[RTP_STREAM_MAX];	
	
	u32					rtpSeq[RTP_STREAM_MAX];
	
	S32                 videoPtsdiv;
	u32                 audioPts;
	
	S32                 audioPtsdiv;

	
	int                 dataoffset;
	char                fuabflag;
	char                fuab[3];
	int                fualen;
	rtp_send_info_t sinfo;
	std::vector<rtp_send_info_t> vSendInfo;     
	std::vector<iovec> vIovcSendInfo;
	RtspSession_t(int sockFd, char *s8PeerIP,   int *s8PeerPort, VodSvrType_e vodSvrType)
	{
		//memset(&rtspSessList, 0, sizeof(rtspSessList));
		transportType= RTP_TRANSPORT_TYPE_TCP;
		rtspSockFd   = sockFd;
		svrType      = vodSvrType;
		channel      = -1;
		sessStat     = RTSP_STATE_INIT;
		tcpThrId     = 0;
		tcpAppId = 0;
		streamType = 0;
		
		bzero(sessId,sizeof(sessId));
		
		bzero(ssrc,sizeof(ssrc));
		
		bzero(userAgent,sizeof(userAgent));

		
		bzero(uri,sizeof(uri));
		
		bzero(recvBuf,sizeof(recvBuf));
		
		bzero(sendBuf,sizeof(sendBuf));
		
		bzero(tmpRecv,sizeof(tmpRecv));

		
		bzero(hostIp,sizeof(hostIp));
		
		bzero(remoteIp,sizeof(remoteIp));
		
		bzero(setupFlag,sizeof(setupFlag));
		
		bzero(udpThrId,sizeof(udpThrId));
		
		bzero(nonce,sizeof(nonce));
		curSessBW = 0;
		reqStreamFlag[RTP_STREAM_VIDEO] = false;
		reqStreamFlag[RTP_STREAM_AUDIO] = false;
		reqStreamFlag[RTP_STREAM_METADATA] = false;
		rtspSessType = RTSP_SESS_PREVIEW;
		
		bzero(&pbInfo,sizeof(pbInfo));
		pbInfo.rateCtrlFlag = 1;//default ,the rtsp server control the delivery speed.
		pbInfo.scale = 1.0f;
		pbInfo.rtcprtplost = 0;
		pbInfo.s_time = 0.1f;
		pbInfo.rtcprtplostratio = 0.5;

		vencType = 0;
		audioEncType = RTP_PT_INVALID;
		APsendFlag = 0;
		mWidth = 0;
        mHeight = 0;
		mFrameRate = 0;

		mVideoBitRate =0;
		mAudioBitRate = 0;
		mAudioRealBitRate = 0;
		stFrameInfo.u32FrameLen = 0;
		stFrameInfo.ps8Buffer = NULL;
		pRtpTcpSender = NULL;
		pRtpTcpSender = NULL;
		PreVEncType = 0;
		httpTunnelInputSocket = 0;
		isHttpTunnel = 0;
		RecvTimeOutCont = 0;
		tCount = 0;
		tcpTimeOut = 0;

		pTmpRecv = tmpRecv;
		//ptmpFsVisitor = NULL;
		videots = 0;
		inSize = 0;
		socketspair[0] = -1;
		socketspair[1] = -1;
		lastRecvSeq = -1;
		lastSendReq = 0;
		lastSendSeq = 0;
		LastPts = 0;
		
		LastFramePts = 0;
		NextSendTime = 0;
		ptsdif = 0;		
		custom_rtsp = 0;
		frameLen = 0;
		uniqueID = 0;
		speco_vms_seq[0] = 0;
		speco_vms_seq[1] = 0;
		
		backchannel = 0;
		is_req_multicast = 0;
		multicast_sessionrelease = 0;
		
		audioPts = 0;
		dataoffset = 0;
		for(int i=0; i<RTP_STREAM_MAX; i++)
		{
			multicast_setup_finish[i] = 0;
			pRtpUdpSender[i] = NULL;
			setupFlag[i] = 0;
			rtpPts[i] = 123456;
			rtpSeq[i] = 0;
		}
		is_metadatastream = 0;
		//bzero(&multicast_info,sizeof(multicast_info));
		
		pthread_mutex_init(&mRtspRtpSynchronized, NULL);
		if(s8PeerIP != NULL)
			strncpy(remoteIp, s8PeerIP, 64);
		
		if(s8PeerPort != NULL)
			remotePort = *s8PeerPort;
		
		vSendInfo.reserve(600);
		vSendInfo.clear();
		vIovcSendInfo.clear();
		
		bzero(&sinfo,sizeof(rtp_send_info_t));
		bzero(fuab, sizeof(fuab));
		fuabflag = 0;
		fualen = 0;
		videoPtsdiv = 0;
		audioPtsdiv = 0;
	}
	//FILE *f1, *f2, *f3;
};



 struct RtspSvr_t{
	int 			rtspSvrSockFd;
	int 			rtspLoopbackFd;
	int 			rtspSvrPort;
	VodSvrState_e		vodSvrState;
	//RtpUdpSender_t		*pRtpUdpSender[MAX_VOD_CHN][RTP_STREAM_MAX];
	std::vector<RtspSession_t*> rtspSessWorkList;
	//RtspSession_t		rtspSessWorkList;
	VodSvrType_e		svrType;
	int 			udpSendActive;
	pthread_mutex_t		rtspListMutex;
	RtspSvr_t()
	{
		rtspSessWorkList.clear();
	}
};




#endif

