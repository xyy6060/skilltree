
#ifndef __RTSP_SERVER_INTF_H__
#define __RTSP_SERVER_INTF_H__
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */



/*
回调函数中MD5加密函数调用例子
#define LEN_64 64 //长度64字节,可直接写64。
md5Pwd输出，返回64位加密后的密码。
MD5Auth_BuildAuthResonseWithMD5((char *)md5Pwd, 64,
		0, name, surveServer, guserpwd, pSess->nonce,
		NULL, NULL, NULL, DESCRIBE, uri);
*/

/*
  Author callback function
  权限验证回调函数:
  返回值定义:
  return 0,  success
  return -1, No Auth.passwd error
  return -2, Can't find username
*/
/*rtsp 请求类型*/
typedef enum
{
	RTSP_PREVIEW,
	RTSP_PLAYBACK
}RTSP_REQ_TYPE;

typedef struct Rtsp_AuthCbArgs_s
{
	RTSP_REQ_TYPE reqType;	//rtsp请求类型，回放or 预览
	int	 channel;			//rtsp请求中的通道号
	char uname[32];			//rtsp请求中的用户名
	char pwd[64];			//rtsp请求中的密码密文
	char surveServer[64];	//生成密文参数
	char nonce[32];			//生成密文参数
	char describe[64];		//生成密文参数
	char uri[256];			//生成密文参数
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
	RTSP_AUTH_CB auth_cb;	/*add 2016.01.31 回调函数*/
	void *userData;			/*add 2016.01.31 回调函数参数*/
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
参数说明:
dbname:	nvr上数据库中的名字
dbpwd :	nvr上数据库保存的密码明文
md5Pwd:	nvr上数据库明文密码生成的密文
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

/*	参数说明：
	@pstreambuf：媒体流缓存数据buffer, 包含RSFrameHead帧头
	@buf_sz: 缓存数据大小
 */

void RTSP_PushStream(const char *pstreambuf, unsigned int buf_sz);

#if 0
/*用于控制RTSP 单个通道的使能状态*/
/*返回旧的状态*/
int  RTSP_EnableChn(const int chn);
/*返回旧的状态*/
int  RTSP_DisenableChn(const int chn);
/*返回当前状态*/
int  RTSP_GetEnableChn(const int chn);



/*返回会话总数*/
int  RTSP_GetSessionCount();
/*返回当前回放连接数*/
int  RTSP_GetPlayBackCount();
/*返回当前预览连接数*/
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

