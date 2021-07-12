
#ifndef __RTSP_SERVER_INTF_H__
#define __RTSP_SERVER_INTF_H__
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */



/*
�ص�������MD5���ܺ�����������
#define LEN_64 64 //����64�ֽ�,��ֱ��д64��
md5Pwd���������64λ���ܺ�����롣
MD5Auth_BuildAuthResonseWithMD5((char *)md5Pwd, 64,
		0, name, surveServer, guserpwd, pSess->nonce,
		NULL, NULL, NULL, DESCRIBE, uri);
*/

/*
  Author callback function
  Ȩ����֤�ص�����:
  ����ֵ����:
  return 0,  success
  return -1, No Auth.passwd error
  return -2, Can't find username
*/
/*rtsp ��������*/
typedef enum
{
	RTSP_PREVIEW,
	RTSP_PLAYBACK
}RTSP_REQ_TYPE;

typedef struct Rtsp_AuthCbArgs_s
{
	RTSP_REQ_TYPE reqType;	//rtsp�������ͣ��ط�or Ԥ��
	int	 channel;			//rtsp�����е�ͨ����
	char uname[32];			//rtsp�����е��û���
	char pwd[64];			//rtsp�����е���������
	char surveServer[64];	//�������Ĳ���
	char nonce[32];			//�������Ĳ���
	char describe[64];		//�������Ĳ���
	char uri[256];			//�������Ĳ���
}Rtsp_AuthCbArgs_t;

typedef int (*RTSP_AUTH_CB)(const Rtsp_AuthCbArgs_t *args, void *userData);


typedef struct _RtspServParamV2
{
	int rtspPort;
	char rtspUserName[32];
	char rtspUserPwd[32];
	void *pDevIntf;
	char AuthFlag;
	char SimulatNum;
	RTSP_AUTH_CB auth_cb;	/*add 2016.01.31 �ص�����*/
	void *userData;			/*add 2016.01.31 �ص���������*/
	int get_data_timeout;
	int  mtkJpegFlag;
	int useHKUrlFlag;
	_RtspServParamV2() : get_data_timeout(5),mtkJpegFlag(0),useHKUrlFlag(0)
	{
	}
}RtspServParam;

#if 0
typedef struct
{
	unsigned char profile; // 0 mainstream, 1 sub, 2 mobile, 3 metadata.
	char video_multicast_addr[16];
	int  video_multicast_port;
	char audio_multicast_addr[16];
	int  audio_multicast_port;
	char metadata_multicast_addr[16];
	int  metadata_multicast_port;
	char set_video;
	char set_audio;
}RtspOnvifMulticastParm;
#endif
/*
����˵��:
dbname:	nvr�����ݿ��е�����
dbpwd :	nvr�����ݿⱣ�����������
md5Pwd:	nvr�����ݿ������������ɵ�����
*/
#if 0
int  RTSP_MD5AuthBuild(const Rtsp_AuthCbArgs_t *args,
	const char *dbname,const char*dbpwd, char *md5Pwd);
#endif

int  RTSP_ThrStart(RtspServParam *serparam, int MaxChNum);
int  RTSP_ThrStop();

#if 0
void RTSP_UpdateParam(char *uname, char *upwd, char uflag);
#endif

/*	����˵����
	@pstreambuf��ý������������buffer, ����RSFrameHead֡ͷ
	@buf_sz: �������ݴ�С
 */

void RTSP_PushStream(const char *pstreambuf, unsigned int buf_sz);

#if 0
/*���ڿ���RTSP ����ͨ����ʹ��״̬*/
/*���ؾɵ�״̬*/
int  RTSP_EnableChn(const int chn);
/*���ؾɵ�״̬*/
int  RTSP_DisenableChn(const int chn);
/*���ص�ǰ״̬*/
int  RTSP_GetEnableChn(const int chn);



/*���ػỰ����*/
int  RTSP_GetSessionCount();
/*���ص�ǰ�ط�������*/
int  RTSP_GetPlayBackCount();
/*���ص�ǰԤ��������*/
int  RTSP_GetPreviewCount();


void RTSP_Onvif_Startmulticst(RtspOnvifMulticastParm *Info);
void RTSP_Onvif_Stopmulticst(unsigned char &profile);
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __GS_RTSP_H__ */

