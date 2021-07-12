//#pragma once
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <netdb.h>
#include <assert.h>
#include <netinet/tcp.h>
#include <list>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/uio.h>

#include "rtsp_push.h"
#include "rtsp_server_intf.h"
#include "rs_rtp.h"
#include "base64.h"
#include "RSFrame.h"



using namespace std;

#define ret_FAIL		(-1)
#define ret_SUCCESS		(0)
#define SERVER_PORT     8554
#define SERVER_RTP_PORT  55532
#define SERVER_RTCP_PORT 55533
#define BUF_MAX_SIZE    (1024*1024)
#define MAX_VOD_CHN		32

static int  max_sessnum = 0; 
unsigned long long 	grtspReaduniqueID;
unsigned int g_TotalBW = 0;

static pthread_t pid = 0;
static int gRtspRun = 0;
static char g_SimulatNum = 0;
//static char g_useHKurl;
static char g_MaxChNum = 0;
unsigned int g_CurrentBW = 0;

static char g_AuthFlag;
static RTSP_AUTH_CB g_auth_cb = NULL;
void *g_auth_cb_args = NULL;


RtpMulticastUdpInfo_t pRtpMulticastUdpInfo[3][RTP_STREAM_MAX];

std::list<const char *> g_getStream;     //用于获取stream
modCommon::Mutex g_rtsplock;
//modCommon::AutoLock tauto(g_rtsplock);	//加锁



//static FILE *gpf = NULL;

RtspSvr_t   gRtspVodSvr;
//DevInterface *g_pRtspDevIntf = NULL;


pthread_mutex_t g_MulticastUdpMutex;
pthread_mutex_t g_mtBandWidt = PTHREAD_MUTEX_INITIALIZER;


#define RTSP_CLEAR_SENDBUF(pSess) 	\
{\
	memset(pSess->sendBuf, 0, MAX_RTSP_BUF_LEN);\
}

#define DBG_RTSP         "[RTSP]"

#define TDEBUG(modname, fmt, ...) \
		do{ \
			struct timeval tval; \
			gettimeofday(&tval, NULL); \
			fprintf(stdout, "%s <%s>(%d) (%ld.%03ld) " fmt "\r\n", modname, __func__, __LINE__, tval.tv_sec, (tval.tv_usec)/1000, ##__VA_ARGS__);fflush(stdout); \
		}while(0)



 char IsIPV6Support()
{
    int probesock = 0;
    probesock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if(probesock <= 0)
    {
		TDEBUG(DBG_RTSP, "device not supoort-IPV6\n");
        return 0;
    }
    else
    {
		TDEBUG(DBG_RTSP, "device supoort-IPV6\n");
        close(probesock);
    }
    return 1;
}

 static u32 getRandNum(const int scop)
 {
	 srand((unsigned)time(NULL)); //用于保证是随机数
	 return rand()%scop;  //用rand产生随机数并设定范围
 }

 void RTSP_SessListLock(void )
 {
	 pthread_mutex_lock(&gRtspVodSvr.rtspListMutex);
 }
 
 void RTSP_SessListUnlock(void)
 {
	 pthread_mutex_unlock(&gRtspVodSvr.rtspListMutex);
 }


 time_t GetTimeDiff(void)
 {
	 time_t utc = 0, rtc = 0, dif = 0;
	 struct tm tm = {0};
 
	 time(&rtc);
	 gmtime_r(&rtc, &tm);
	 utc = mktime(&tm);
	 dif = rtc - utc;
	 //TDEBUG(DBG_RTSP, "rtc=%ld, utc=%ld, dif=%ld\n", rtc, utc, dif);
	 return dif;
 }
 
 time_t Dt2Sectime(const DateTime *mytm)
 {
	 struct tm t  = {0};
	 t.tm_year = mytm->Year - 1900;
	 t.tm_mon  = mytm->Month -1;
	 t.tm_mday = mytm->Day ;
	 t.tm_hour = mytm->Hour;
	 t.tm_min  = mytm->Minute;
	 t.tm_sec  = mytm->Second;
	 time_t utcSec = mktime(&t);
	 //TDEBUG_PRINT("--------sectime:%ld\n", utcSec);
	 return utcSec;
 }

void Tm2Datetime(const struct tm *t, DateTime *datetime)
{
	datetime->Year 	 = t->tm_year + 1900;
	datetime->Month  = t->tm_mon + 1;
	datetime->Day 	 = t->tm_mday;
	datetime->Hour 	 = t->tm_hour;
	datetime->Minute = t->tm_min;
	datetime->Second = t->tm_sec;
}
 
 static void url_time_parse(const char *strTm, DateTime *stuDateTime)
 {
	 int year = 0,month = 0,day = 0,hour = 0,minute = 0,second = 0;
	 //timeZone = get_time_zone(timeDiff);
 
	 sscanf(strTm, "%d%*[^0-9]%d%*[^0-9]%dT%d%*[^0-9]%d%*[^0-9]%dZ",&year,
			&month,&day,&hour,&minute,&second);
	 stuDateTime->Year = year;
	 stuDateTime->Month = month;
	 stuDateTime->Day = day;
	 //stuDateTime->Hour = hour + timeZone;
	 stuDateTime->Hour = hour;
 
	 stuDateTime->Minute = minute;
	 stuDateTime->Second = second;
   //  TDEBUG_PRINT("%d,%d,%d,%d,%d,%d, timeZone = %d\n", year, month, day, hour+timeZone, minute, second, timeZone);
 }



 void RTSP_Multicast_CheckIsClose()
 {
 	#if 0
	 for(int i=0; i<3; i++)
	 {
		 for(int j=0; j<RTP_STREAM_MAX; j++)
		 {
		 
			 if(j != RTP_STREAM_AUDIO && pRtpMulticastUdpInfo[i][j].clientnum == 0 && pRtpMulticastUdpInfo[i][j].psender != NULL)
			 {
				 pthread_mutex_lock(&g_MulticastUdpMutex);
				 
				 if(pRtpMulticastUdpInfo[i][j].psender->rtpSockFd != -1)
					 sock_close(pRtpMulticastUdpInfo[i][j].psender->rtpSockFd);
				 pRtpMulticastUdpInfo[i][j].psender->rtpSockFd = -1; 
				 if(pRtpMulticastUdpInfo[i][j].psender->rtpSockFd != -1)
					 sock_close(pRtpMulticastUdpInfo[i][j].psender->rtcpSockFd);
				 pRtpMulticastUdpInfo[i][j].psender->rtcpSockFd = -1;
				 
				 free(pRtpMulticastUdpInfo[i][j].psender);
				 pRtpMulticastUdpInfo[i][j].psender = NULL;
				 pthread_mutex_unlock(&g_MulticastUdpMutex);
				 TDEBUG(DBG_RTSP, "profile=%d stream=%d sender free \n", i, j );
			 }
			 
		 }
		 
	 }
	 
	if(pRtpMulticastUdpInfo[0][RTP_STREAM_AUDIO].clientnum + pRtpMulticastUdpInfo[1][RTP_STREAM_AUDIO].clientnum + pRtpMulticastUdpInfo[2][RTP_STREAM_AUDIO].clientnum== 0 && pRtpMulticastUdpInfo[0][RTP_STREAM_AUDIO].psender != NULL)
	 {
		 pthread_mutex_lock(&g_MulticastUdpMutex);
		 if(pRtpMulticastUdpInfo[0][RTP_STREAM_AUDIO].psender->rtpSockFd != -1)
			 sock_close(pRtpMulticastUdpInfo[0][RTP_STREAM_AUDIO].psender->rtpSockFd);
		 pRtpMulticastUdpInfo[0][RTP_STREAM_AUDIO].psender->rtpSockFd = -1;  
		 
		 if(pRtpMulticastUdpInfo[0][RTP_STREAM_AUDIO].psender->rtcpSockFd != -1)
			 sock_close(pRtpMulticastUdpInfo[0][RTP_STREAM_AUDIO].psender->rtcpSockFd);
		 pRtpMulticastUdpInfo[0][RTP_STREAM_AUDIO].psender->rtcpSockFd = -1;
 
		 if(pRtpMulticastUdpInfo[0][RTP_STREAM_AUDIO].psender != NULL);
			 free(pRtpMulticastUdpInfo[0][RTP_STREAM_AUDIO].psender);
		 pRtpMulticastUdpInfo[0][RTP_STREAM_AUDIO].psender = NULL;
		 TDEBUG(DBG_RTSP, "audio sender free \n" );
		 pthread_mutex_unlock(&g_MulticastUdpMutex);
	 }
	#endif
 }


S32 TcpListen(const char *host, const char *serv, socklen_t *addrlenp)
{
	int listenfd = 0, n;
	const S32 on = 1;
	struct addrinfo hints, *res = NULL, *ressave = NULL;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;   //被动的，用于bind
	 // 调用者将在bind()函数调用中 使用返回的地址结构
	
	//if(IsIPV6Support())
	//	hints.ai_family = AF_INET6;  // ipv6 
	//else
	hints.ai_family = AF_INET;  // ipv4 
	hints.ai_socktype = SOCK_STREAM;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{
		TDEBUG(DBG_RTSP, "getaddrinfo called error: %s, %s \n", host, serv);
		goto failed;
	}
	ressave = res;

	do 
	{
		TDEBUG(DBG_RTSP, "server_test_1 \n");
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (listenfd < 0)
			continue; 
		TDEBUG(DBG_RTSP, "server_test_2 \n");

		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
			break;

		TDEBUG(DBG_RTSP, "server ip:%s \n", inet_ntoa(((sockaddr_in*)res->ai_addr)->sin_addr));

		close(listenfd);
	} 
	while ( (res = res->ai_next) != NULL);

	if (res == NULL)
	{
		TDEBUG(DBG_RTSP, "errno from final socket() or bind(): %s, %s \n", host, serv);
		goto failed;
	}

	if (listen(listenfd, LISTENQ) < 0)
	{
		TDEBUG(DBG_RTSP, "listen called error: %s, %s \n",host, serv);
		goto failed;
	}

	if (addrlenp)
		*addrlenp = res->ai_addrlen;

	freeaddrinfo(ressave);

	return(listenfd);

failed:
	if(ressave != NULL)
		freeaddrinfo(ressave);
		
	if(listenfd > 0)
		close(listenfd);
	
	return -1;
}


int sock_close(int sock_fd)
{
	if(sock_fd > 2)
	{
		shutdown(sock_fd, SHUT_RDWR);
		return close(sock_fd);
	}
	return 0;
}

void RTSP_SessListDel(RtspSession_t *pSess)
{
    if(pSess != NULL)
    {
        RTSP_SessListLock();
        //list_del(&pSess->rtspSessList);
        unsigned int i;
		for( i = 0 ; i < gRtspVodSvr.rtspSessWorkList.size(); i++)
		{
			if(pSess == gRtspVodSvr.rtspSessWorkList[i])
				break;
		}
		if(i >=  gRtspVodSvr.rtspSessWorkList.size())
		{
      	  TDEBUG(DBG_RTSP,"del pSess error,i=%d\n",i);
		  assert(0);
		}
		else
			gRtspVodSvr.rtspSessWorkList.erase(gRtspVodSvr.rtspSessWorkList.begin()+i);
        delete(pSess);
        pSess = NULL;
		max_sessnum--;

        RTSP_SessListUnlock();
    }
}

int RTSP_SessionClose(RtspSession_t *pSess)
{
    if(pSess == NULL ||max_sessnum==0)
    {
        TDEBUG(DBG_RTSP,"pSess == NULL,maxsessum=%d \n",max_sessnum);
        return ret_FAIL;
    }
	
	TDEBUG(DBG_RTSP,"remoteip=%s,port=%d del uid=%llu,psess=%p \n",pSess->remoteIp,pSess->remotePort,pSess->uniqueID, pSess);
	if(g_TotalBW > 0)
	{
		pthread_mutex_lock(&g_mtBandWidt);
		TDEBUG(DBG_RTSP, "Current session bandWidth: %u , limited BandWidth: %u, g_CurrentBW=%d,psess=%p \n",pSess->curSessBW, g_TotalBW, g_CurrentBW, pSess);
		g_CurrentBW -= pSess->curSessBW ;

		pthread_mutex_unlock(&g_mtBandWidt);
	}
    if(RTP_TRANSPORT_TYPE_TCP == pSess->transportType)
    {
        pSess->sessStat = RTSP_STATE_STOP;
        TDEBUG(DBG_RTSP, "pSess->sessStat = RTSP_STATE_STOP \n");
        if(pSess->tcpThrId > 0)
        {
           // dbg(Dbg, DbgNoPerror, "pthread_join tcp thread id = %ul\n",(int)pSess->tcpThrId);
            pthread_join(pSess->tcpThrId, NULL);
            pSess->tcpThrId = 0;
           // dbg(Dbg, DbgNoPerror, "pthread_join ok\n");
        }
		if (pSess->tcpAppId > 0)
		{
			//dbg(Dbg, DbgNoPerror, "pthread_join tcp thread id = %ul\n", (int)pSess->tcpAppId);
			pthread_join(pSess->tcpAppId, NULL);
			pSess->tcpAppId = 0;
			//dbg(Dbg, DbgNoPerror, "pthread_join ok\n");
		}

        if(pSess->pRtpTcpSender != NULL)
        {
           // dbg(Dbg, DbgNoPerror, "free(pSess->pRtpTcpSender)= %p\n", pSess->pRtpTcpSender);
            free(pSess->pRtpTcpSender);
            pSess->pRtpTcpSender = NULL;
        }
        if(pSess->rtspSockFd > 0)
        {
            sock_close(pSess->rtspSockFd);
            pSess->rtspSockFd = -1;
        }
        if(pSess->isHttpTunnel && pSess->httpTunnelInputSocket > 0)
        {
            sock_close(pSess->httpTunnelInputSocket);
            pSess->httpTunnelInputSocket = -1;
        }
    }
    else
    {
        if(pSess->rtspSockFd > 0)
        {
            sock_close(pSess->rtspSockFd);
            pSess->rtspSockFd = -1;
        }
        pSess->sessStat = RTSP_STATE_STOP;

        if(pSess->udpThrId[RTP_STREAM_AUDIO]>0)
            pthread_join(pSess->udpThrId[RTP_STREAM_AUDIO],NULL);
        if(pSess->udpThrId[RTP_STREAM_VIDEO]>0)
            pthread_join(pSess->udpThrId[RTP_STREAM_VIDEO],NULL);
        if(pSess->udpThrId[2]>0)
            pthread_join(pSess->udpThrId[2],NULL);
        /*Del Audio Sender*/
        if(pSess->pRtpUdpSender[RTP_STREAM_AUDIO] != NULL)
        {
            pSess->pRtpUdpSender[RTP_STREAM_AUDIO]->bActive = false;
			if(pSess->pRtpUdpSender[RTP_STREAM_AUDIO]->rtcpSockFd > 0)
				close(pSess->pRtpUdpSender[RTP_STREAM_AUDIO]->rtcpSockFd );
			if(pSess->pRtpUdpSender[RTP_STREAM_AUDIO]->rtpSockFd > 0)
				close(pSess->pRtpUdpSender[RTP_STREAM_AUDIO]->rtpSockFd );

            free(pSess->pRtpUdpSender[RTP_STREAM_AUDIO]);
            pSess->pRtpUdpSender[RTP_STREAM_AUDIO] = NULL;
        }
        if(pSess->pRtpUdpSender[RTP_STREAM_VIDEO] != NULL)
        {
            pSess->pRtpUdpSender[RTP_STREAM_VIDEO]->bActive = false;

			
			if(pSess->pRtpUdpSender[RTP_STREAM_VIDEO]->rtcpSockFd > 0)
				close(pSess->pRtpUdpSender[RTP_STREAM_VIDEO]->rtcpSockFd );
			if(pSess->pRtpUdpSender[RTP_STREAM_VIDEO]->rtpSockFd > 0)
				close(pSess->pRtpUdpSender[RTP_STREAM_VIDEO]->rtpSockFd );
            free(pSess->pRtpUdpSender[RTP_STREAM_VIDEO]);
            pSess->pRtpUdpSender[RTP_STREAM_VIDEO] = NULL;
        }
		if(pSess->pRtpUdpSender[RTP_STREAM_METADATA] != NULL)
        {
            pSess->pRtpUdpSender[RTP_STREAM_METADATA]->bActive = false;

			if(pSess->pRtpUdpSender[RTP_STREAM_METADATA]->rtcpSockFd > 0)
				close(pSess->pRtpUdpSender[RTP_STREAM_METADATA]->rtcpSockFd );
			if(pSess->pRtpUdpSender[RTP_STREAM_METADATA]->rtpSockFd > 0)
				close(pSess->pRtpUdpSender[RTP_STREAM_METADATA]->rtpSockFd );
            free(pSess->pRtpUdpSender[RTP_STREAM_METADATA]);
            pSess->pRtpUdpSender[RTP_STREAM_METADATA] = NULL;
        }
    }
	pthread_mutex_destroy(&pSess->mRtspRtpSynchronized);
//	TDEBUG(DBG_RTSP, "remoteip=%s,port=%d del uid=%llu,psess=%p ,max_sessnum=%d, ssize=%d \n",
//		pSess->remoteIp,pSess->remotePort,pSess->uniqueID, pSess, max_sessnum, gRtspVodSvr.rtspSessWorkList.size());
    RTSP_SessListDel(pSess);
    return ret_SUCCESS;
}


static void RTSP_SessionStatCheck(RtspSvr_t *pRtspSvr)
{
    RtspSession_t *pRtspSess = NULL;
	for(unsigned int i = 0 ; i < gRtspVodSvr.rtspSessWorkList.size(); i++)
	{
		pRtspSess = gRtspVodSvr.rtspSessWorkList[i];
        if(pRtspSess != NULL)
        {
            if(pRtspSess->sessStat == RTSP_STATE_STOP)
            {
                TDEBUG(DBG_RTSP, "test--STOP---\n");
				for(unsigned int i=0; i<RTP_STREAM_MAX; i++)
				{
					/*
					if(pRtspSess->multicast_setup_finish[i])
					{
						pthread_mutex_lock(&g_MulticastUdpMutex);//udp多播
					
						if(pRtpMulticastUdpInfo[pRtspSess->streamType][i].clientnum != 0)
							pRtpMulticastUdpInfo[pRtspSess->streamType][i].clientnum -= pRtspSess->multicast_setup_finish[i];
						pthread_mutex_unlock(&g_MulticastUdpMutex);
						pRtspSess->multicast_setup_finish[i] = 0;
					}
					*/
				}
				
				if(!pRtspSess->multicast_sessionrelease)
				{	
					printf("close session :%s,ip=%s,port=%d\n", pRtspSess->sessId,pRtspSess->remoteIp,pRtspSess->remotePort);
					RTSP_Multicast_CheckIsClose();
					if(ret_SUCCESS != RTSP_SessionClose(pRtspSess))
	                {
	                    printf("RTSP_SessionClose error. \n");
	                }
				}
				else
				{
					//printf("ch  client clientnum : [%d] \n",pRtpMulticastUdpInfo[pRtspSess->streamType][0].clientnum);
					if(pRtspSess->rtspSockFd > 0)
			        {
			            sock_close(pRtspSess->rtspSockFd);
			            pRtspSess->rtspSockFd = -1;
			        }
				}
            }
        }
    }
}

char SessListExist(RtspSession_t *ps)
{
	RtspSession_t *pSess;
	char flag = 0;

	for(unsigned int i = 0 ; i < gRtspVodSvr.rtspSessWorkList.size(); i++)
	{
		pSess = gRtspVodSvr.rtspSessWorkList[i];
		if (pSess == ps)
		{
			flag = 1;
			break;
		}
	}
	return flag;
}


static int rtsp_new_client_req_accept(int svrSockFd, char *ipAddr,  long int *port)
{
    int sockOptVal = 1;
    struct linger  socklinger = {0};
    struct timeval rtimeout = {0};
	
    struct timeval stimeout = {0};
	rtimeout.tv_sec  = 3;
    rtimeout.tv_usec = 0;
    socklinger.l_onoff  = 1;
    socklinger.l_linger = 0;

	char  s8PeerPort[NI_MAXSERV] = {0}; 
	char  s8PeerIP[NI_MAXHOST]   = {0};
	struct sockaddr_storage cliAddr;
	//struct addrinfo *res = NULL;
	struct addrinfo hints;
		
	memset(&cliAddr, 0, sizeof(struct sockaddr_storage));
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;
	
	socklen_t addrLen = sizeof(cliAddr);
    int cliSockFd = accept(svrSockFd, (struct sockaddr *)&cliAddr,&addrLen);
    if(cliSockFd <= 0)
    {
    	perror("accept");
        printf("rtsp server accept error,sock = %d\n",cliSockFd);
        return 0;
    }
		
	if (getnameinfo((struct sockaddr*)&cliAddr, sizeof(struct sockaddr_storage), s8PeerIP, sizeof(s8PeerIP), s8PeerPort, sizeof(s8PeerPort), NI_NUMERICHOST | NI_NUMERICSERV) != 0)
	{
		printf("get clientip error,\n");
		strcpy(s8PeerIP,"unkown ipaddr");
	}

	*port = strtol(s8PeerPort, NULL, 10);
	strncpy(ipAddr, (const char *)s8PeerIP, 64);

#if 0
	//检查ip
	int s32RetIP=g_pRtspDevIntf->CheckIP((U8*)s8PeerIP); 
	if(s32RetIP != 0 && s32RetIP != 1)
    {
        printf("IP filter falure \n");
	    close(cliSockFd);
		return 0;
    }
#endif
	
#if 1
    if( ret_FAIL == setsockopt(cliSockFd, SOL_SOCKET, SO_RCVTIMEO,(char*)&rtimeout, sizeof(rtimeout)))
    {

        printf("setsockopt cliSockFd = %d error\n\n",cliSockFd);
		perror("setsockopt SOL_SOCKET SO_RECVTIMEO: ");
    }
	stimeout.tv_sec  = 3;
    stimeout.tv_usec = 0;
	if( ret_FAIL == setsockopt(cliSockFd, SOL_SOCKET, SO_SNDTIMEO,(char*)&stimeout, sizeof(stimeout)))
    {

        printf( "setsockopt cliSockFd = %d error\n\n",cliSockFd);
		perror("setsockopt SOL_SOCKET SO_RECVTIMEO: ");
    }
#endif

    if( ret_FAIL == setsockopt(cliSockFd, SOL_SOCKET, SO_LINGER,&socklinger, sizeof(socklinger)))
    {
        printf("setsockopt cliSockFd = %d error\n\n",cliSockFd);
		perror("setsockopt SOL_SOCKET,SO_LINGER: ");

    }
    if( ret_FAIL == setsockopt(cliSockFd ,IPPROTO_TCP, TCP_NODELAY,&sockOptVal, sizeof(int)))
    {
        printf( "setsockopt cliSockFd = %d error\n\n",cliSockFd);
        perror("setsockopt IPPROTO_TCP,TCP_NODELAY: ");
    }
    return cliSockFd;
}

ConType_e RTSP_GetConnectType(const char *recvBuf)
{
    if( NULL != strcasestr(recvBuf,"HTTP") || NULL != strcasestr(recvBuf,"x-sessioncookie"))
    {
        return CON_TYPE_HTTP;
    }
    else if(NULL != strstr(recvBuf, "rtsp"))
    {
        return CON_TYPE_RTSP;
    }
    else if(NULL != strstr(recvBuf, UCTRL_HEAD))
    {
        return CON_TYPE_UCTRL;
    }
    return CON_TYPE_UNDEFINED;
}

static TrackId_e rtsp_HandSetupGetTrackID(const char* url)
{
	int trackId = TRACK_ID_UNKNOWN;
    const char *p = strstr(url,"trackID");
    if(p!=NULL)
    {
        printf("trackid=%s\n",p);
        sscanf(p,"trackID=%d ",&trackId);
    }
	return (TrackId_e)trackId;
}

int RTSP_ResponseMsgCheck(int *stat, char *buf)
{
    u32 state;
    char  version[32];
    char  trash[256];
    u32 parameterNum;

    parameterNum = sscanf(buf, " %31s %u %255s ", version, &state, trash);
    if(strncasecmp(version, "RTSP/", 5))
    {
        return RTSP_PARSE_ISNOT_RESP;
    }

    if(parameterNum < 3 || state == 0)
    {
        return RTSP_PARSE_ISNOT_RESP;
    }

    *stat = state;
    return RTSP_PARSE_IS_RESP;
}

int RTSP_GetReq(char *buf)
{
    int cnt;
    char method[32];
    char object[256];
    char ver[32];
    RtspMethod_t *m;

    *method = *object = '\0';

    cnt = sscanf(buf, " %31s %255s %31s", method, object, ver);
    if(cnt != 3)
    {
        printf("buf: %s\n is not a valid req message \n", buf);
		return RTSP_PARSE_INVALID;
    }

    for(m=gRtspMethod; m->opcode != RTSP_REQ_METHOD_MAX; m++)
    {
        if(!strcmp(m->describe, method))
        {
            break;
        }
    }
    return (m->opcode);
}

int RTSP_GetCseq(char *buf)
{
    int cseq = -1;
    char  *pTmp = NULL;

    pTmp = strcasestr(buf, RTSP_HDR_CSEQ);
    if(pTmp == NULL)
    {
        printf("not found 'CSeq'. \n""----------------------------------\n""%s\n------------------------------\n", buf);
    }
    else
    {
        if(2 != sscanf(pTmp, "CSeq:%d", &cseq))
        {
            //printf("not found 'Cseq'. \n""----------------------------------\n""%s\n------------------------------\n", pTmp);
        }
    }
    return cseq;
}


int RTSP_GetSpecString(char *srcbuf,const char *specstr, char *destbuf)
{
    int flag = 0,stp = 0,enp=0,i=0;
    char *s_ptr = strcasestr(srcbuf,specstr);
    if(s_ptr)
    {
        while(s_ptr[i])
        {
            if(s_ptr[i] =='"' && flag == 0)
            {
                flag = 1;
                stp = i;
            }
            i++;
            if(s_ptr[i] =='"' && flag == 1)
            {
                flag =0;
                enp = i;
                break;
            }
        }
        strncpy(destbuf,s_ptr+stp+1,enp-stp-1);
        destbuf[enp-stp-1] = '\0';
    }
    else
    {
        return -1;
    }
    return 0;
}


int RTSP_ParseUserPwd(char *buff, char *name, char *passwd, char *uri)
{
    char *p = NULL;
    p = strstr(buff, RTSP_HDR_AUTHORIZATION);
    p = p+22;//p ???òusername
    int ret;
    ret = RTSP_GetSpecString(p,"username", name);
    if(ret == -1)
        return ret_FAIL;
    ret = RTSP_GetSpecString(p,"uri", uri);
    if(ret == -1)
        return ret_FAIL;
    ret = RTSP_GetSpecString(p,"response", passwd);
    if(ret == -1)
        return ret_FAIL;

    return ret_SUCCESS;
}


RtpTcpSender_t * RTP_CreateTcpSender(RtspSession_t *pSess)
{

	RtpTcpSender_t *pSender = NULL;
	pSender = (RtpTcpSender_t *)malloc(sizeof(RtpTcpSender_t));
	if(pSender == NULL)
	{
		return NULL;
	}
	memset(pSender, 0, sizeof(RtpTcpSender_t));
	pSender->tcpSockFd = pSess->rtspSockFd;

	pSender->rtpPts[RTP_STREAM_VIDEO] = 0;//
	pSender->rtpSeq[RTP_STREAM_VIDEO] = 1;//3200;
	pSender->rtpPts[RTP_STREAM_AUDIO] = 0;//
	pSender->rtpSeq[RTP_STREAM_AUDIO] = 1;//3200;

	pSender->audioG711Ssrc = RTP_DEFAULT_SSRC + pSess->channel + pSess->streamType;
	pSender->videoH264Ssrc = RTP_DEFAULT_SSRC + pSess->channel + 128 + pSess->streamType;
	return pSender;
}

static int rtsp_TcpHandleSetupInterleaved(const char *line, const RtpStreamType_e streamType,RtspSession_t *pSess)
{
	const char *p = strstr(line, "interleaved");
	if(NULL == p)
	{
		TDEBUG(DBG_RTSP, "can't find \"interleaved\" : %s\n", line);
		return ret_FAIL;
	}
	if( pSess->setupFlag[streamType] == 0 )
	{
		p = strstr(p, "=");
		if(p != NULL)
		{
			sscanf(p+1, "%u", &pSess->pRtpTcpSender->interleaved[streamType].rtp);
		}

		p = strstr(p, "-");
		if(p != NULL)
		{
			sscanf(p+1, "%u", &pSess->pRtpTcpSender->interleaved[streamType].rtcp);
		}
		else
		{
			pSess->pRtpTcpSender->interleaved[streamType].rtcp = pSess->pRtpTcpSender->interleaved[streamType].rtp + 1;
		}

		pSess->setupFlag[streamType] = 1;
	}
	return ret_SUCCESS;
}



const char *RTSP_GetMethodDescrib(int code)
{
    RtspMethod_t *pMethod;

    for(pMethod = gRtspStatu; pMethod->opcode != RTSP_PARSE_INVALID_OPCODE;
        pMethod ++)
    {
        if(pMethod->opcode == code)
        {
            return (pMethod->describe);
        }
    }
    return gRtspInvalidMethod;
}


int RTSP_GetHead(int err, RtspSession_t *pSess)
{
    char *pTmp = NULL;

    RTSP_CLEAR_SENDBUF(pSess);
    pTmp = pSess->sendBuf;
    pTmp += sprintf(pTmp, "%s %d %s\r\n", RTSP_VERSION, err,
                    RTSP_GetMethodDescrib( err ));
    pTmp += sprintf(pTmp,"CSeq: %d\r\n", pSess->lastRecvSeq);
    pTmp += sprintf(pTmp,"Server: Rtsp Server\r\n");
    return (strlen(pSess->sendBuf));
}


int RTSP_AddReplyHeadDate(char *pTmp)
{
	struct tm rest;
    time_t ttm;
    char tmp[64]= {0};
    time(&ttm);
    localtime_r(&ttm,&rest);
    strftime(tmp,sizeof(tmp),"%a, %d %b %Y %T GMT",&rest);

    return sprintf(pTmp, "Date: %s\r\n\r\n",tmp);
}

RtpPt_e RTSP_GetRtpPtByVencType(const int vEncType)
{
	RtpPt_e tpy;
	switch(vEncType)
	{
		case VENC_TYPE_H264:
		case VENC_TYPE_H265:
			tpy = RTP_PT_H264;
			break;
		case VENC_TYPE_JPEG:
			tpy = RTP_PT_JPEG;
			break;
		default:
			tpy = RTP_PT_H264;
			break;
	}
	return tpy;
}



void RtspPlaybackUrlParseParamPair(std::vector <rtspParamPair_t> &pm, const char *req)
{
	rtspParamPair_t tmp;
	bool pflag = true, vflag = false;
	if (req == NULL)
		return;
	while (*req)
	{
		switch (*req)
		{
			do
			{
		default:
			if (*req == '=' || *req == '&' || *req == '\0')
			{
				if (*req == '&' || *req == '\0')
				{
					pm.push_back(tmp);
					tmp.paramKey.clear();
					tmp.value.clear();
				}
				break;
			}
			if (pflag)
				tmp.paramKey += *req;
			if (vflag)
				tmp.value += *req;
			continue;
		case '&':
			pflag = true;
			vflag = false;
			continue;
		case '=':
			pflag = false;
			vflag = true;
			continue;
			} while (*(req++));
		}
	}
}



static  int RTSP_ParsePlybackUrl(const char *url, RtspSession_t *pSess)
{
    /*
        rtsp://<username>:<password>@<ip>:<port>/cam/playback?
        channel=<channelNo>&starttime=<starttime>&endtime=<endtime>
    */
    TDEBUG(DBG_RTSP, "url:%s\n", url);
    const char *pos1 = url;
    const char *pos2 = pos1;
    int ret = 0;
	int tdate[6];
	pSess->pbInfo.timediff = GetTimeDiff();
    if(NULL != (pos1 = strstr(pos1,"rtsp://")))
    {
    	const char *tmpPos = pos1;
		tmpPos += strlen("rtsp://");
		const char *endpos = strchr(tmpPos, '/');

		const char *anotherPos;
		if(endpos == NULL)
		{
			TDEBUG(DBG_RTSP, "parse :%s error \n", url);
	        return ret_FAIL;
		}

		if((anotherPos = strchr(tmpPos, '@')) != NULL)
		{
			tmpPos = ++anotherPos;
		}
		if(endpos-tmpPos < 64)
		{
			strncpy(pSess->hostIp, tmpPos, endpos-tmpPos);
		}
		else
		{
			TDEBUG(DBG_RTSP, "hostName is too long to parse :%s error \n", url);
	        return ret_FAIL;
		}
    }
    else
    {
        return ret_FAIL;
    }
    if((pos1 = strstr(pos1, "playback?"))
       && (pos2 = strstr( pos1, "channel="))
       && (pos2 = strstr( pos1, "starttime="))
       && (pos2 = strchr( pos2, 'T'))
       && (pos2 = strchr( pos2, 'Z'))
       && (pos2 = strstr( pos1, "endtime="))
       && (pos2 = strchr( pos2, 'T'))
       && (pos2 = strchr( pos2, 'Z')))
    {
        pSess->rtspSessType = RTSP_SESS_PLYBACK;
    }
    else
    {
    	
		if((pos1 = strstr(url,"playback/") )!= NULL)
		{
		
			pos1 = strchr(pos1, '/');
			ret = sscanf(pos1+1,"%04d%02d%02d%02d%02d%02d",&tdate[0], &tdate[1],
				   &tdate[2], &tdate[3], &tdate[4], &tdate[5]);
			pSess->pbInfo.startTime.Year = tdate[0];
			pSess->pbInfo.startTime.Month = tdate[1];
			pSess->pbInfo.startTime.Day = tdate[2];
			pSess->pbInfo.startTime.Hour = tdate[3];
			pSess->pbInfo.startTime.Minute = tdate[4];
			pSess->pbInfo.startTime.Second = tdate[5];
			if(ret != 6)
			{
				TDEBUG(DBG_RTSP, "playback url:%s <--->error\n", url);
		        return ret_FAIL;
			}
			
			struct tm tmTime = {0};

			pSess->pbInfo.sTimeSec = Dt2Sectime(&pSess->pbInfo.startTime);
			time_t pct = 0; 
			if(strstr(pos1,"continuous"))
			{
			
			  pct = pSess->pbInfo.sTimeSec + 36000;
				
			}
			else
			{
			
				pct = pSess->pbInfo.sTimeSec + 60;
			}
			localtime_r(&pct, &tmTime);
			pSess->pbInfo.endTime.Year   = tmTime.tm_year + 1900;
			pSess->pbInfo.endTime.Month  = tmTime.tm_mon + 1;
			pSess->pbInfo.endTime.Day	  = tmTime.tm_mday;
			pSess->pbInfo.endTime.Hour   = tmTime.tm_hour;
			pSess->pbInfo.endTime.Minute = tmTime.tm_min;
			pSess->pbInfo.endTime.Second = tmTime.tm_sec;
			pSess->pbInfo.eTimeSec = Dt2Sectime(&pSess->pbInfo.endTime) + pSess->pbInfo.timediff;
			pSess->channel = 0;
			pSess->streamType  = 0;
			
			TDEBUG(DBG_RTSP, "playback :endtime:%d:%d:%d:%d:%d:%d:\n", pSess->pbInfo.endTime.Year , pSess->pbInfo.endTime.Month,
				pSess->pbInfo.endTime.Day,pSess->pbInfo.endTime.Hour ,pSess->pbInfo.endTime.Minute,pSess->pbInfo.endTime.Second  );
			pSess->rtspSessType = RTSP_SESS_PLYBACK;
			return ret_SUCCESS;
		}
		else
		{
			TDEBUG(DBG_RTSP, "---playback url:%s error\n", url);
	        return ret_FAIL;
		}
    }
	pos1 = strchr(pos1, '?');

	std::vector <rtspParamPair_t> vrapam;
	vrapam.clear();
	RtspPlaybackUrlParseParamPair(vrapam,pos1+1);
	if (vrapam.size() < 3 || vrapam.size() > 4)
	{
		TDEBUG(DBG_RTSP, "playback url:%s <--->error\n", url);
		return ret_FAIL;
	}
	if (!strcmp(vrapam[0].paramKey.c_str(), "channel"))
	{
		const char *pt = strstr(vrapam[0].value.c_str(), "ch");
		if (pt)
		{
			pSess->channel = atoi(pt + 2);
			pSess->channel -= 1;
		}
		else
		{
			pt = strstr(vrapam[0].value.c_str(), "ip");
			if (pt)
			{
				pSess->channel = atoi(pt + 2);
				pSess->channel += g_SimulatNum;
				if (pSess->channel > g_SimulatNum)
					pSess->channel -= 1;
			}
			else
			{
				pSess->channel = atoi(vrapam[0].value.c_str());
				pSess->channel -= 1;
			}
		}
	}
	else
	{
		TDEBUG(DBG_RTSP, "playback url:%s <--->error\n", url);
		return ret_FAIL;
	}

	if (!strcmp(vrapam[1].paramKey.c_str(), "starttime"))
	{
		url_time_parse(vrapam[1].value.c_str(), &pSess->pbInfo.startTime);
	}
	else
	{
		TDEBUG(DBG_RTSP, "playback url:%s <--->error\n", url);
		return ret_FAIL;
	}

	if (!strcmp(vrapam[2].paramKey.c_str(), "endtime"))
	{
		url_time_parse(vrapam[2].value.c_str(), &pSess->pbInfo.endTime);
	}
	else
	{
		TDEBUG(DBG_RTSP, "playback url error\n");
		return ret_FAIL;
	}

	if (vrapam.size() == 4)
	{
		if (!strcmp(vrapam[3].paramKey.c_str(), "subtype"))
		{
			pSess->streamType = atoi(vrapam[3].value.c_str());
			//TDEBUG_PRINT("rtsp-streamtype=%d\n", pSess->streamType);
		}
		else
		{
			TDEBUG(DBG_RTSP, "playback url  error\n");
			return ret_FAIL;
		}
	}
	pSess->pbInfo.sTimeSec = Dt2Sectime(&pSess->pbInfo.startTime);
	pSess->pbInfo.eTimeSec = Dt2Sectime(&pSess->pbInfo.endTime) + pSess->pbInfo.timediff;
//     if(pos1 && url_args_parse(pos1, pSess))
//     {
//         dbg(Dbg, DbgNoPerror, "url format:%s", url);
//         ret = GS_FAIL;
//     }
    return ret;
}

static int RTSP_ParsePreviewUrl(const char *url, RtspSession_t *pSess)
{
	if(url == NULL)
		return ret_FAIL;

	char hostName[64] = {0};
	int chn = 0, sttype;
	int ret = 0, validLen;
	const char *startPos, *endpos, *tmpPos, *anotherPos;
	char *flagPos;
	pSess->custom_rtsp = 0;
	tmpPos = strstr(url, "rtsp://");
	if(tmpPos == NULL)
	{
		TDEBUG(DBG_RTSP, "parse :%s error \n", url);
        return ret_FAIL;
	}

	tmpPos += strlen("rtsp://");
	endpos = strchr(tmpPos, '/');
	if(endpos == NULL)
	{
		TDEBUG(DBG_RTSP, "parse :%s error \n", url);
        return ret_FAIL;
	}

	if((anotherPos = strchr(tmpPos, '@')) != NULL)
	{
		tmpPos = ++anotherPos;
	}
		
	if(endpos-tmpPos < 64)
	{
		strncpy(hostName, tmpPos, endpos-tmpPos);
	}
	else
	{
		TDEBUG(DBG_RTSP, "hostName is too long to parse :%s error \n", url);
        return ret_FAIL;
	}

	startPos = endpos;
	if(strstr(startPos, "av_stream") )
	{
		#if 0
		if(!g_useHKurl)
			return ret_FAIL;
		if(RTSP_ParseHikVisionCMSUrl(url, startPos, &chn, &sttype) == ret_FAIL)
		{
			return ret_FAIL;
		}
		pSess->custom_rtsp = 3;
		#endif
	}
	else
	{
		if(strstr(startPos, "/ONVIF_RTSP"))
		{
			startPos = startPos + strlen("/ONVIF_RTSP");
		}
		
		if(strstr(startPos, "/multicast"))
		{
			pSess->is_req_multicast = 1;

			
			if(strstr(startPos, "/ch01/metadata"))
			{
				pSess->is_metadatastream = 1;		
				chn = 0;
				sttype = 0;
				validLen = strlen(startPos);
				if (validLen > 25)
				{
					TDEBUG(DBG_RTSP, "invalid uri parsing :%s error ret: %d  validlen: %d\n",
						url, ret, validLen);
					return ret_FAIL;
				}

			}
			else
			{
				ret = sscanf(startPos, "/multicast/ch%d/%d", &chn, &sttype);
				chn -= 1;
				validLen = strlen(startPos);

				if (ret != 2 || validLen > 18)
				{
					TDEBUG(DBG_RTSP, "invalid uri parsing :%s error ret: %d  validlen: %d\n",
						url, ret, validLen);
					return ret_FAIL;
				}
			}
		}	
		else if(strstr(startPos, "/ch01/metadata"))
		{
			pSess->is_metadatastream = 1;
			chn = 0;
			sttype = 0;
			validLen = strlen(startPos);

			if (validLen > 15)
			{
				TDEBUG(DBG_RTSP, "invalid uri parsing :%s error ret: %d  validlen: %d\n",
					url, ret, validLen);
				return ret_FAIL;
			}
		}
		else if(strstr(startPos, "/ch"))
		{		
			ret = sscanf(startPos, "/ch%d/%d", &chn, &sttype);
			chn -= 1;
			validLen = strlen(startPos);

			if (ret != 2 || validLen > 8 || validLen < 6)
			{
				TDEBUG(DBG_RTSP, "invalid uri parsing :%s error ret: %d  validlen: %d\n",
					url, ret, validLen);
				return ret_FAIL;
			}
		}
		else if(strstr(startPos, "/ip"))
		{
			ret = sscanf(startPos, "/ip%d/%d", &chn, &sttype);
			chn += g_SimulatNum;
			if(chn > g_SimulatNum)
				chn -= 1;
			validLen = strlen(startPos);

			if (ret != 2 || validLen > 8 || validLen < 6)
			{
				TDEBUG(DBG_RTSP, "invalid uri parsing :%s error ret: %d  validlen: %d\n",
					url, ret, validLen);
				return ret_FAIL;
			}
		}
		else if (strstr(startPos, "/StdCh"))
		{
			ret = sscanf(startPos, "/StdCh%d", &sttype);
			chn = 0;
			sttype -= 1;
			pSess->custom_rtsp = 2;
		}
		else if (strstr(startPos,"/Live"))
		{
			ret = sscanf(startPos, "/Live/Channel=%d", &sttype);
			chn = 0;
			if (sttype == -1)
			{
				pSess->custom_rtsp = 4;
				sttype = 0;
			}
			else
			{
				sttype -= 1;
				pSess->custom_rtsp = 1;
			}
		}
		else if (strstr(startPos,"/live"))
		{
			chn = 0;
			sttype = 0;
		}
		else if (strstr(startPos,"/H264?"))
		{
			ret = sscanf(startPos, "/H264?ch=%d&subtype=%d&authbasic",&chn, &sttype);
			chn -= 1;
		}
		else
		{
			TDEBUG(DBG_RTSP, "invalid uri parsing :%s error \n", url);
       		return ret_FAIL;
		}

	}

    if(NULL != strstr(pSess->recvBuf, "Rtsp Client"))
    {
		pSess->custom_rtsp = 5;

    }
    

	if(sttype > 2 || sttype < 0 || chn > g_MaxChNum || chn < 0)
	{
		TDEBUG(DBG_RTSP, "invalid uri parsing :%s error \n", url);
       	return ret_FAIL;
	}

	if((flagPos = strchr(hostName, ':')) != NULL)
	{
		*flagPos = '\0';
	}

	strncpy(pSess->hostIp, hostName, 63);
	pSess->hostIp[63] = '\0';
    pSess->channel = chn;
    pSess->streamType = sttype;
	pSess->rtspSessType = RTSP_SESS_PREVIEW;
	TDEBUG(DBG_RTSP, "ch = %d, sttype = %d, hostip=%s, custom=%d\n",chn, sttype, pSess->hostIp,pSess->custom_rtsp);
	return ret_SUCCESS;

}

char * CommSdpHandle(RtspSession_t *pSess, char *p)
{
	struct timeval tv = {0};
	gettimeofday(&tv, NULL);
	long unsigned int ts = tv.tv_sec * 1000000 + tv.tv_usec;

	p += sprintf(p,"v=0\r\n");
	p += sprintf(p,"o=- %lu %lu IN IP4 %s\r\n", ts, ts, pSess->hostIp);
	if(pSess->backchannel)
		p += sprintf(p,"s=RTSP Session with audiobackchannel\r\n");

	//p += sprintf(p,"s=-\r\n");
	if(pSess->custom_rtsp == 4)
		p += sprintf(p, "i=http80 rtsp554 pt0\r\n");
	//p += sprintf(p,"e=NONE\n");
	if(!pSess->is_req_multicast)
		p += sprintf(p,"c=IN IP4 %s\r\n", pSess->hostIp);
	p += sprintf(p,"t=0 0\r\n");

	/*if now is playback set the npt range*/
	//p += sprintf(p,"a=control:%s\r\n", pSess->uri);
	return p;
}

int RTSP_GetStreamInfo(RtspSession_t *pSess)
{
	 int ret = 0;
    /*H264 TrackID=0 RTP_PT 96*/
	 //获取视频流比特率
	 //获取预览视频音频编码类型
    //pSess->vencType = VENC_TYPE_H265;
    pSess->vencType = VENC_TYPE_H264;
    //pSess->vencType = VENC_TYPE_JPEG;
	pSess->audioEncType = RTP_PT_INVALID;


#if 0
#ifdef DEMO
    //pSess->vencType = VENC_TYPE_H265;
    pSess->vencType = VENC_TYPE_H264;
    //pSess->vencType = VENC_TYPE_JPEG;
	pSess->audioEncType = RTP_PT_INVALID;
//#else

	//获取回放视频音频编码类型
	if(pSess->rtspSessType == RTSP_SESS_PLYBACK)
	{
		while(1)
		{
			ret = rtp_get_playback_videoaudio_info(pSess);
			if(ret > 0)
			{
				break;
			}
			else
			{
        		if(ret == -1)
        		{
        			usleep(100*1000);
           			continue;
        		}
        		else
        		{
          			TDEBUG(DBG_RTSP, "read data error, ret = %d \n", ret);
            		break;
        		}
			}
		}
	}
	else
	{

		//获取视频流比特率
		//获取预览视频音频编码类型
		STREAMINFO_S stinfo;
		RTSP_GetVideoBitRate(pSess->channel, pSess->streamType, &stinfo);

		pSess->mVideoBitRate = stinfo.videoAttr.bitRate;
		pSess->mHeight = stinfo.videoAttr.u32Height;
		pSess->mWidth = stinfo.videoAttr.u32Width;
		pSess->vencType = stinfo.videoAttr.encType;
		pSess->mAudioBitRate = 64;
	}

#endif
#endif
	return ret;
}


void RTSP_GetSessionId(char *sessId, int len)
{
    int i;
    for(i=0; i<len; i++)
    {
        sessId[i] = (char )((random()%10) + '0');
    }
    sessId[len] = 0;
}

static void RTSP_FreeMemory(char **p)
{
	if(p && *p)
	{
		free(*p);
		*p = NULL;
	}
}


static int RTSP_ParseHdrKeyWord(const char *recvBuf, char *pSessKeyVal,const char* keyWord)
{
    const char *pTmp = strstr(recvBuf, keyWord);
    const char *p = NULL;
    if(pTmp != NULL)
    {
        p = pTmp;
        while(0 != (char)(*p))
        {
            if((char)(*p) == 10 || (char)(*p) == 13)
            {
                break;
            }
            if((char)(*p) == ':')
            {
                pTmp = p + 2;//':'??oó?1óD??????
            }
            p ++;
        }
        if(p != pTmp)
        {
            strncpy(pSessKeyVal, pTmp, p - pTmp);
        }
    }
    return strlen(pSessKeyVal);
}

static char *AddAudioSDPInfo(RtspSession_t *pSess, char *psdp)
{
	char *p = psdp;
	char audioEncode[32] = {0};
	if(pSess->audioEncType == RTP_PT_ALAW)
	{
		sprintf(audioEncode, "8 PCMA/%u", pSess->mAudioRealBitRate);
	}
	else if(pSess->audioEncType == RTP_PT_ULAW)
	{
		sprintf(audioEncode, "0 PCMU/%u", pSess->mAudioRealBitRate);
	}
	else if(pSess->audioEncType == RTP_PT_G726)
	{
		sprintf(audioEncode, "%d G726-%d/%u", pSess->audioEncType,pSess->mAudioBitRate, pSess->mAudioRealBitRate);
	}
	else if(pSess->audioEncType == RTP_PT_ADPCM)
	{
		if(pSess->mAudioBitRate != 128)
		{
			sprintf(audioEncode, "98 32kadpcm/%u", pSess->mAudioRealBitRate);
		}
		else
		{
			sprintf(audioEncode, "98 mpeg4-generic/%u/1", pSess->mAudioRealBitRate);
		}
	}
	p += sprintf(p, "m=audio 0 RTP/AVP %d\r\n", pSess->audioEncType);
	if(pSess->is_req_multicast)
	{
		#if 0
		char multicast_ip[32];
		int port;
		g_pRtspDevIntf->OnvifMulticastParamAdapter(pSess->streamType, RTP_STREAM_AUDIO, multicast_ip, port);
		p += sprintf(p,"c=IN IP4 %s\r\n", multicast_ip);
		p += sprintf(p,"a=source-filter:incl IN IP4 %s %s\r\n", multicast_ip, pSess->hostIp);
		#endif
	}
	p += sprintf(p, "a=control:trackID=1\r\n");
	p += sprintf(p, "a=rtpmap:%s\r\n", audioEncode);

	if(pSess->audioEncType == RTP_PT_ADPCM)
	{
		if(pSess->mAudioBitRate == 128) // aac audio
		{
			p += sprintf(p, "a=fmtp 98 streamtype=5; profile-level-id=16; mode=AAC-hbr; config=""\r\n"); // for aac audio rfc3640
		}
	}
	p += sprintf(p, "a=x-onvif-track:audiotrack_token\r\n");
	p += sprintf(p, "b=AS:50\r\n");
	
	return p;
}


char *Find_next_nal(char *stbuf, int datalen)
{
	char *p ;
	if(*stbuf == 0x02 || *stbuf == 0x26)
	{
		//TDEBUG(DBG_RTSP, "return_null_here_!\n");
		return NULL;
	}
	for(p = stbuf ;p < stbuf + datalen - 3;p++ )
	{
		if(*p == 0 && *(p+1) == 0 && *(p+2) == 0 && *(p+3))
		{
			return p + 4;
		}
	}
	if(p == stbuf + datalen - 3 || p > stbuf + 2048)
	{
		//TDEBUG(DBG_RTSP, "return_null_here_!\n");
		return NULL;
	}
	else
	{
		printf("<%s>:(%d),find failed stbuf=%p, p=%p, end=%p\n",__FUNCTION__,__LINE__,stbuf,p,stbuf + datalen - 3);
		return NULL;
	}
}

static void BackChannelHandle(RtspSession_t *pSess, char *p)
{
	char audioEncode[512] = {0};
	int len = 0;
	len += sprintf(audioEncode + len, "a=rtpmap:98 mpeg4-generic/%u/1\r\n", pSess->mAudioRealBitRate);
	len += sprintf(audioEncode + len, "a=rtpmap:0 PCMU/%u\r\n", pSess->mAudioRealBitRate);
	len += sprintf(audioEncode + len, "a=rtpmap:8 PCMA/%u\r\n", pSess->mAudioRealBitRate);
	len += sprintf(audioEncode + len, "a=rtpmap:8 G711/%u\r\n", pSess->mAudioRealBitRate);
	len += sprintf(audioEncode + len, "a=rtpmap:97 G726-16/%u\r\n", pSess->mAudioRealBitRate);
	len += sprintf(audioEncode + len, "a=rtpmap:98 G726-24/%u\r\n", pSess->mAudioRealBitRate);
	len += sprintf(audioEncode + len, "a=rtpmap:99 G726-32/%u\r\n", pSess->mAudioRealBitRate);
	sprintf(audioEncode + len, "a=rtpmap:100 G726-40/%u\r\n", pSess->mAudioRealBitRate);
	p += sprintf(p, "m=audio 0 RTP/AVP 0 8 97 98 99 100\r\n");
	p += sprintf(p, "a=control:trackID=4\r\n");
	p += sprintf(p, "%s", audioEncode);
	p += sprintf(p, "a=sendonly\r\n");
	//p += sprintf(p, "a=Media_header:MEDIAINFO=494D4B48010100000400010000000000000000000000000000000000000000000000000000000000;\r\n");		
}


char *GetVideoInfo(char *pSrc, int srclen, char nType, int *nallen, char flag )
{
	char *pSend = pSrc + H264_STARTCODE_LEN;
	int dataLen = srclen - H264_STARTCODE_LEN;
	char *pNal = NULL;
	int nalType, NalLen;
	while(1)
	{
		if(flag)
			nalType = H265_Get_NalType(*pSend);
		else
			nalType = H264_Get_NalType(*pSend);
		pNal = Find_next_nal(pSend, dataLen);
		if(pNal != NULL)
		{
			NalLen   = pNal - pSend - H264_STARTCODE_LEN;
			dataLen -= NalLen + H264_STARTCODE_LEN;
			if(nalType == nType)
			{
				*nallen = NalLen;
				return pSend;
			}
		}
		else
		{
			return NULL;
		}

		pSend = pNal ;
	}
	return NULL;
}


int  rtp_get_preview_one_iframe(RtspSession_t *pSess ,char *pbuf, int len)//1024
{
	int pktLen = 0;
	char *getData = NULL;
	RSFrameHead *rh;
	TDEBUG(DBG_RTSP, "get one I FRAME !\n");

	while(1)
	{		
		g_rtsplock.lock();
		if(0 == g_getStream.size())
		{
			g_rtsplock.unlock();
			usleep(100);
			continue;
		}
		
		getData = (char *)g_getStream.front();
		g_getStream.pop_front();
		g_rtsplock.unlock();
		
		rh = (RSFrameHead*)(getData);
		TDEBUG(DBG_RTSP, " FrameType:[%c] !\n", rh->FrameType);
		if(rh->FrameType == RSIFrame)
			break;
		else
			free((void *)getData);
	}
	
	TDEBUG(DBG_RTSP, "Width:[%d] Height:[%d] FrameRate:[%d] FrameSize:[%d] FrameType:[%c]!\n", rh->VideoHead.Width, rh->VideoHead.Height, rh->VideoHead.FrameRate, rh->FrameSize, rh->FrameType);
	if(rh->FrameType == RSIFrame)
	{
		pSess->mWidth  = rh->VideoHead.Width;
		pSess->mHeight = rh->VideoHead.Height;
		pSess->mFrameRate = rh->VideoHead.FrameRate;
		pSess->PreVEncType = rh->EncodeType;
		pktLen = rh->FrameSize;
		if(pbuf != NULL)
		{
			memcpy(pbuf, (((char*)rh)+sizeof(RSFrameHead)), len<pktLen ?len:pktLen);
		}
		else
		{
			TDEBUG(DBG_RTSP, "pbuf is NULL!\n");
		}
	}
	free((void *)getData);
	
	return len < pktLen ?len:pktLen;
}



int H264SdpHandle(RtspSession_t *pSess, char *psdp)
{
	TDEBUG(DBG_RTSP, "_264_sdp \n");
	int ret = 0, len = 0;
	char cpps, csps;//cvps
	char *pps = NULL, *sps =NULL;//vps
	char *p = psdp;
	char base64buf[1024]= {0};
	char profile_level_id[8] = {0};
	int pt = 96;
	char *tbuf = (char*)malloc(4096);
	if(NULL == tbuf)
	{
		TDEBUG(DBG_RTSP, "malloc tbuf error!\n");
		return -1;
	}

	p += sprintf(p, "m=video 0 RTP/AVP %d\r\n", pt);
	if(pSess->is_req_multicast)
	{
		#if 0
		char multicast_ip[32];
		int port;
		g_pRtspDevIntf->OnvifMulticastParamAdapter(pSess->streamType, RTP_STREAM_VIDEO, multicast_ip, port);
		p += sprintf(p,"c=IN IP4 %s\r\n", multicast_ip);
		p += sprintf(p,"a=source-filter:incl IN IP4 %s %s\r\n", multicast_ip, pSess->hostIp);
		#endif
	}
	p += sprintf(p, "a=control:trackID=0\r\n");
	p += sprintf(p, "a=rtpmap:%d H264/90000\r\n", pt);
	if (pSess->custom_rtsp == 3)	//customize for HikinVision
		p += sprintf(p, "a=Media_header:MEDIAINFO=494D4B48010100000400010000000000000000000000000000000000000000000000000000000000;\r\n");
	//p += sprintf(p, "b=AS:5000\r\n");
	
	TDEBUG(DBG_RTSP, "_264_rtspSessType:[%d] \n", pSess->rtspSessType);
	switch(pSess->rtspSessType)
	{
		case RTSP_SESS_PLYBACK:
		case RTSP_SESS_PREVIEW:
			ret = rtp_get_preview_one_iframe(pSess , tbuf, 1024);
			break;
	}

	fprintf(stderr, "[DEBUG-PRINT]==============>%s %d\n", __FUNCTION__, __LINE__);
	p += sprintf(p, "a=x-dimensions:%d,%d\r\n", pSess->mWidth, pSess->mHeight);
	p += sprintf(p, "a=framerate:%d\r\n", pSess->mFrameRate);
	p += sprintf(p, "a=bitrate:%d\r\n", pSess->mVideoBitRate);

	TDEBUG(DBG_RTSP, "ret_:[%d] \n", ret);
	if(ret == 0)
	{
		TDEBUG(DBG_RTSP, "get I frame error\n");
		free(tbuf);
		return -1;
	}

	csps = 7;
	sps = GetVideoInfo(tbuf,ret, csps, &len, 0);
	sprintf(profile_level_id, "420029");
	
	TDEBUG(DBG_RTSP, "profile_level_id = %s\n", profile_level_id);
	if(sps != NULL)
	{
		base64_bits_to_64((unsigned char *)base64buf, (const unsigned char *)sps, len);
		p += sprintf(p,"a=fmtp:%d packetization-mode=1;profile-level-id=%s;sprop-parameter-sets=%s,", pt, profile_level_id, base64buf);
	}
	else
	{
		p += sprintf(p,"a=fmtp:96 packetization-mode=1; sprop-parameter-sets=(null)\r\n");
		TDEBUG(DBG_RTSP, "sps not null \n");
		free(tbuf);
		return 0;
	}
	cpps = 8;
	pps = GetVideoInfo(tbuf,ret, cpps, &len, 0);
	if(pps != NULL)
	{
		base64_bits_to_64((unsigned char *)base64buf, (const unsigned char *)pps, len);
		p += sprintf(p,"%s\r\n",base64buf);
	}
	else
	{
		p += sprintf(p,"\r\n");
		free(tbuf);
		return 0;
	}

	if(pSess->audioEncType != RTP_PT_INVALID)
		p = AddAudioSDPInfo(pSess, p);

	p += sprintf(p, "a=recvonly\r\n");
	if(pSess->audioEncType != RTP_PT_INVALID && pSess->backchannel)
	{
		BackChannelHandle(pSess, p);
	}
	if ( pSess->custom_rtsp == 2)
	{
		p += sprintf(p, "m=application 0 RTP/AVP 107\r\n");
		p += sprintf(p, "a=control:trackID=3\r\n");
		p += sprintf(p, "a=rtpmap:107 vnd.onvif.metadata/90000\r\n");
		p += sprintf(p, "a=recvonly\r\n");
	}
	
	free(tbuf);
	return 0;
}


int RTSP_SendReply(int err, int simple, const char *addon, RtspSession_t *pSess)
{
    int n = 0,sendLen = 0;
    char *pTmp = pSess->sendBuf;
    if(simple == 1)
    {
        pTmp += RTSP_GetHead(err, pSess);
    }

    if(addon)
    {
        pTmp += sprintf(pTmp, "%s", addon);
    }

    if(simple)
    {
        strcat(pSess->sendBuf, RTSP_LRLF);
    }


	pthread_mutex_lock(&pSess->mRtspRtpSynchronized);
    n = send(pSess->rtspSockFd, pSess->sendBuf, strlen(pSess->sendBuf), MSG_NOSIGNAL);
	pthread_mutex_unlock(&pSess->mRtspRtpSynchronized);
	sendLen = strlen(pSess->sendBuf);
	if(n != sendLen)
    {
        printf("only send %d byts, we need send %d bytes!\n",n, sendLen);
        return ret_FAIL;
    }

    printf("remoteip=%s,port=%d S->C ================\n",pSess->remoteIp,pSess->remotePort);
    printf("rtsp uid=%llu, sendmsg:  %s \n",pSess->uniqueID, pSess->sendBuf);

    return ret_SUCCESS;
}


int RTSP_EventHandleTeardown(RtspSession_t *pSess)
{
    int ret;

    char *pTmp = pSess->sendBuf;
    pSess->sessStat = RTSP_STATE_STOP;
    pTmp += RTSP_GetHead( RTSP_STATUS_OK, pSess);
    pTmp += sprintf(pTmp,"Session: %s\r\n", pSess->sessId);
    pTmp += sprintf(pTmp,"Connection: Close\r\n");
	pTmp += RTSP_AddReplyHeadDate(pTmp);
    ret = RTSP_SendReply( RTSP_STATUS_OK, 0, NULL, pSess);
	
	printf("uurl =%s,teardown\n",pSess->uri);
    return ret;
}

int RTSP_EventHandlePause(RtspSession_t *pSess)
{
    int ret = 0;
    char *pTmp = pSess->sendBuf;
    bzero(pTmp, MAX_RTSP_BUF_LEN);
    if(RTSP_STATE_PLAY == pSess->sessStat)
    {
        printf("get the pause command ,set the state pause..\n");
        pSess->sessStat = RTSP_STATE_PAUSE;
    }
	pTmp += RTSP_GetHead(RTSP_STATUS_OK, pSess);
	pTmp += sprintf(pTmp,"Session: %s\r\n", pSess->sessId);
	pTmp += RTSP_AddReplyHeadDate(pTmp);
	RTSP_SendReply(RTSP_STATUS_OK, 0, NULL, pSess);
    return ret;
}

int RTSP_EventHandleUnknown(RtspSession_t *pSess)
{
	char *pTmp = pSess->sendBuf;
	
	pTmp += RTSP_GetHead( 404, pSess);
	if(pSess->sessId[0] != '\0')
	{
    	pTmp += sprintf(pTmp,"Session: %s\r\n", pSess->sessId);
	}
	pTmp += RTSP_AddReplyHeadDate(pTmp);
    RTSP_SendReply(404, 0, NULL, pSess);

	return ret_FAIL;
}

int RTSP_EventHandleHeartBeat(RtspSession_t *pSess)
{
	char *pTmp = pSess->sendBuf;

    pTmp += RTSP_GetHead( RTSP_STATUS_OK, pSess);
	if(pSess->sessId[0] != '\0')
	{
    	pTmp += sprintf(pTmp,"Session: %s\r\n", pSess->sessId);
	}
	pTmp += RTSP_AddReplyHeadDate(pTmp);
    return RTSP_SendReply(RTSP_STATUS_OK, 0, NULL, pSess);
}

int RTSP_EventHandleOptions(RtspSession_t *pSess)
{
 	char *pTmp = pSess->sendBuf;
    pTmp += RTSP_GetHead(RTSP_STATUS_OK, pSess);
	printf("lwl_test_rtsp\n");
    if(strstr(pSess->recvBuf, "starttime"))
    {
        pTmp += sprintf(pTmp, "Public: OPTIONS, DESCRIBE, SETUP, PLAY, "
        	"RECORD, PAUSE, TEARDOWN, ANNOUNCE, SET_PARAMETER, GET_PARAMETER\r\n");
    }
    else/*PAUSE*/
	{
        pTmp += sprintf(pTmp, "Public: OPTIONS, DESCRIBE, SETUP, PLAY, "
			"RECORD, TEARDOWN, ANNOUNCE, SET_PARAMETER, GET_PARAMETER\r\n");
    }
	pTmp += RTSP_AddReplyHeadDate(pTmp);

    return RTSP_SendReply(RTSP_STATUS_OK, 0, NULL, pSess);
}

int RTSP_EventHandleSetParam(RtspSession_t *pSess)
{
    char *pTmp = pSess->sendBuf;

    pTmp += RTSP_GetHead( RTSP_STATUS_OK, pSess);
    pTmp += sprintf(pTmp,"Session: %s\r\n", pSess->sessId);
	pTmp += RTSP_AddReplyHeadDate(pTmp);
    return RTSP_SendReply(RTSP_STATUS_OK, 0, NULL, pSess);
}

int RTSP_EventHandleGetParam(RtspSession_t *pSess)
{printf("\033[47;31m   [%s, %d]--------------  \033[0m \n",__func__, __LINE__);
    char *pTmp = pSess->sendBuf;

    pTmp += RTSP_GetHead( RTSP_STATUS_OK, pSess);
    pTmp += sprintf(pTmp,"Session: %s\r\n", pSess->sessId);
	pTmp += RTSP_AddReplyHeadDate(pTmp);
    return RTSP_SendReply(RTSP_STATUS_OK, 0, NULL, pSess);
}

int RTSP_EventHandleSetup(RtspSession_t *pSess)
{
    int ret = 0;
    int chn = 0;
    TrackId_e trackId = TRACK_ID_UNKNOWN;
	RtpStreamType_e streamType = RTP_STREAM_VIDEO;
	u32 ssrc;
    //int rtpPort, rtcpPort;
    char url[255], trash[255];
    char line[255];
    char *p = NULL, *pTmp = NULL;

    if(!sscanf(pSess->recvBuf, " %*s %254s ", url))
    {
        RTSP_SendReply(400, 1, NULL, pSess);
        return ret_FAIL;
    }
	if(pSess->sessStat == RTSP_STATE_PLAY)
	{
		TDEBUG(DBG_RTSP, "pses-playing\n");
		return ret_SUCCESS;
	}

    TDEBUG(DBG_RTSP, "test_rtsp_here_lwl 000 \n");
    trackId = rtsp_HandSetupGetTrackID(url);
    if(trackId == TRACK_ID_VIDEO)
    {
        pSess->reqStreamFlag[RTP_STREAM_VIDEO] = true;
		streamType = RTP_STREAM_VIDEO;
		if (pSess->custom_rtsp == 1)
		{
			pSess->reqStreamFlag[RTP_STREAM_AUDIO] = true;
		}
    }
    else if(trackId == TRACK_ID_AUDIO)
    {
        pSess->reqStreamFlag[RTP_STREAM_AUDIO] = true;
		streamType = RTP_STREAM_AUDIO;
    }
	else if (trackId == TRACK_ID_OTHER)
	{
		if (pSess->custom_rtsp == 1 || pSess->custom_rtsp == 2)
		{
			TDEBUG(DBG_RTSP, " speco motion req \n");
			streamType = RTP_APP_T;
		}
		if(pSess->is_metadatastream)
		{
			TDEBUG(DBG_RTSP, " onvif metadata req \n");
			pSess->reqStreamFlag[RTP_STREAM_METADATA] = true;
			streamType = RTP_STREAM_METADATA;
		}
	}
	else if (trackId == TRACK_ID_BACKCHANNEL)
	{
		if(NULL == strstr(pSess->recvBuf, RTSP_BACKCHANNEL))
	    {
	        RTSP_SendReply(400, 1, NULL, pSess);
	        return ret_FAIL;
	    }
		pSess->reqStreamFlag[RTP_STREAM_AUDIO] = true;
		streamType = RTP_STREAM_AUDIO;
	}
	else
    {
        TDEBUG(DBG_RTSP, "track id = %d error \n", trackId);
        RTSP_SendReply(400, 1, NULL, pSess);
        return ret_FAIL;
    }

    p = strcasestr(pSess->recvBuf, RTSP_HDR_TRANSPORT);
    if(p == NULL)
    {
        TDEBUG(DBG_RTSP, "get rtp transport type  error \n");
        RTSP_SendReply(406, 1, "Require: Transport settings"
                       " of rtp/udp;port=nnnn. ", pSess);
        return ret_FAIL;
    }
	if(strcasestr(p, "unicast"))
		pSess->is_req_multicast = 0;
    /*
     * Transport: RTP/AVP;unicast;client_port=6972-6973;source=10.71.147.222;
     *         server_port=6970-6971;ssrc=00003654
     * trash = "Transport:"
     * line = "RTP/AVP;unicast;client_port=6972-6973;source=10.71.147.222;
     *         server_port=6970-6971;ssrc=00003654"
     */
    if(2 != sscanf(p, "%10s%255s", trash, line))
    {
        TDEBUG(DBG_RTSP, "setup request malformed \n");
        RTSP_SendReply(400, 1, 0, pSess);
        return ret_FAIL;
    }
	if(pSess->is_req_multicast)
		p = strcasestr(line, "port");
	else
		p = strcasestr(line, "client_port");
	
	TDEBUG(DBG_RTSP, "track id = %d , ismulist=%d \n", trackId, pSess->is_req_multicast );
	//区分TCP UDP
#if 0	
	if(p != NULL)
	{
    	pSess->tcpTimeOut = 30;				//RTP over UDP时，RTSP会话保活时间为30秒
    	int keepAlive = 1;   				// 开启keepalive属性. 缺省值: 0(关闭)
		int keepIdle = pSess->tcpTimeOut;   // 如果在60秒内没有任何数据交互,则进行探测. 缺省值:7200(s)
		int keepInterval = 5;   			// 探测时发探测包的时间间隔为5秒. 缺省值:75(s)
		int keepCount = 3;   				// 探测重试的次数. 全部超时则认定连接失效..缺省值:9(次)

		if(ret_FAIL == setsockopt(pSess->rtspSockFd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive)))
		{
        	printf("setsockopt cliSockFd = %d error\n\n",pSess->rtspSockFd);
			perror("setsockopt SOL_SOCKET SO_KEEPALIVE: ");
		}

		if(ret_FAIL == setsockopt(pSess->rtspSockFd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle)))
		{

        	printf("setsockopt cliSockFd = %d error\n\n",pSess->rtspSockFd);
			perror("setsockopt SOL_TCP TCP_KEEPIDLE: ");
		}

		if(ret_FAIL == setsockopt(pSess->rtspSockFd, SOL_TCP, TCP_KEEPINTVL, (void*)&keepInterval, sizeof(keepInterval)))
		{

        	printf("setsockopt cliSockFd = %d error\n\n",pSess->rtspSockFd);
			perror("setsockopt SOL_TCP TCP_KEEPINTVL: ");
		}

		if(ret_FAIL == setsockopt(pSess->rtspSockFd, SOL_TCP, TCP_KEEPCNT, (void*)&keepCount, sizeof(keepCount)))
		{

       		printf("setsockopt cliSockFd = %d error\n\n",pSess->rtspSockFd);
			perror("setsockopt SOL_TCP TCP_KEEPCNT: ");
		}

        p = strstr(p, "=");
        sscanf(p+1, "%d", &rtpPort);

        p = strstr(p, "-");
        sscanf(p+1, "%d", &rtcpPort);
        pSess->transportType = RTP_TRANSPORT_TYPE_UDP;
		RtpPt_e tpy ;
        pTmp = pSess->sendBuf;
		memset(pTmp,0,sizeof(pSess->sendBuf));
        pTmp += RTSP_GetHead(200, pSess);
        pTmp += sprintf(pTmp,"Session: %s;timeout=%d\r\n", pSess->sessId, pSess->tcpTimeOut);
		if(streamType == RTP_STREAM_VIDEO || streamType == RTP_STREAM_METADATA)
		{
       	  	tpy = RTSP_GetRtpPtByVencType(pSess->vencType);
		}
		else
		{
			tpy = (RtpPt_e)pSess->audioEncType;
		}

		printf("req_is_mul? : %d\n", pSess->is_req_multicast);
		if(pSess->is_req_multicast)
		{
			
			//onvif udp多播
			char multicast_ip[32];
			int port;
			int profile = pSess->streamType;
			if(RTP_STREAM_AUDIO == streamType)
				profile = 0;
			g_pRtspDevIntf->OnvifMulticastParamAdapter(profile, streamType, multicast_ip, port);
			pthread_mutex_lock(&g_MulticastUdpMutex);
			pSess->multicast_setup_finish[streamType] = 1;	
			pRtpMulticastUdpInfo[pSess->streamType][streamType].clientnum++;
			if(pRtpMulticastUdpInfo[profile][streamType].psender == NULL)
				pRtpMulticastUdpInfo[profile][streamType].psender = RTP_CreateMulticastUdpSender(pSess, tpy, streamType, multicast_ip, port);
			
			
			
			if(pRtpMulticastUdpInfo[profile][streamType].psender == NULL)
			{
				RTSP_SendReply(400, 1, "service error", pSess);
				pthread_mutex_unlock(&g_MulticastUdpMutex);
				return ret_FAIL;
			}
			
			RTSP_SRV_DBG_PRINT("pSess->streamType=%d streamType=%d \n",profile,streamType );
			pTmp += sprintf(pTmp,"Transport: RTP/AVP;multicast;port=%d-%d;destination=%s;mode=\"play\"\r\n",
						port,   port+1, multicast_ip);	
				
			pthread_mutex_unlock(&g_MulticastUdpMutex);
			
		}
		else
		{
			pSess->pRtpUdpSender[streamType] = RTP_CreateUdpSender( pSess->channel, tpy);
			
			TDEBUG_PRINT("malloc UdpSender...\n");
			if(pSess->pRtpUdpSender[streamType] == NULL)
			{
				RTSP_SendReply(400, 1, "service error", pSess);
				return GS_FAIL;
			}
			if(streamType == RTP_STREAM_METADATA)
				pSess->pRtpUdpSender[streamType]->ssrc += pSess->streamType; 
			pSess->RecvTimeOutCont = 0;
			pSess->pRtpUdpSender[streamType]->bActive = True;
			pSess->pRtpUdpSender[streamType]->remoteRtpPort  = rtpPort;
			pSess->pRtpUdpSender[streamType]->remoteRtcpPort = rtcpPort;
			pSess->pRtpUdpSender[streamType]->avType = AV_TYPE_VIDEO;
			strcpy(pSess->pRtpUdpSender[streamType]->remoteIp, pSess->remoteIp);
			pTmp += sprintf(pTmp,"Transport: RTP/AVP;unicast;client_port=%d-%d;"
							"server_port=%d-%d;ssrc=%x\r\n",
							rtpPort, rtcpPort,
							pSess->pRtpUdpSender[streamType]->rtpPort,
							pSess->pRtpUdpSender[streamType]->rtcpPort,
							pSess->pRtpUdpSender[streamType]->ssrc);			
		}	
    }
	else 
#endif

	if(NULL != (p = strstr(line, "/TCP")))
	{
		if(pSess->is_req_multicast == 1)		
		{
			printf("tcp request multicast stream %d \n", trackId);
			RTSP_SendReply(400, 1, NULL, pSess);
			printf("tcp request multicast stream %d \n", trackId);
			return ret_FAIL;
		}
		pSess->tcpTimeOut = 120;
        pSess->transportType = RTP_TRANSPORT_TYPE_TCP;
        pTmp = pSess->sendBuf;
        pTmp += RTSP_GetHead(200, pSess);
        pTmp += sprintf(pTmp,"Session: %s;timeout=%d\r\n", pSess->sessId, pSess->tcpTimeOut);
		/*TCP protocl use one common socket send vido and audio,
		  if request twice ,just call once "RTP_CreateTcpSender" func*/
		//TDEBUG_PRINT("malloc TcpSender...1\n");
		
		if(NULL == pSess->pRtpTcpSender)
		{
			pSess->pRtpTcpSender = RTP_CreateTcpSender(pSess);
		}
		if(NULL == pSess->pRtpTcpSender)
		{
			RTSP_SendReply(413, 1, NULL, pSess);
			return ret_FAIL;
		}

		switch(streamType)
		{
			case RTP_STREAM_VIDEO:
				ssrc = pSess->pRtpTcpSender->videoH264Ssrc;
				break;
			case RTP_APP_T:
				ssrc = pSess->pRtpTcpSender->videoH264Ssrc + 128;
				break;
			case RTP_STREAM_AUDIO:
				ssrc = pSess->pRtpTcpSender->audioG711Ssrc;
				break;
			case RTP_STREAM_METADATA:
				ssrc = pSess->pRtpTcpSender->videoH264Ssrc + 128;// 
				break;
			default:break;
		}

		ret = rtsp_TcpHandleSetupInterleaved(p, streamType, pSess);
		if(ret == ret_FAIL)
		{
			printf("setup chn = %d, unsupported transport\n",chn);
			RTSP_SendReply(400, 1, "Unsupported Transport", pSess);
			return ret_FAIL;
		}
		if (streamType != RTP_APP_T)
		{
			pTmp += sprintf(pTmp, "Transport: RTP/AVP/TCP;unicast;"
				"interleaved=%d-%d;ssrc=%08x;mode=\"play\"\r\n",
				pSess->pRtpTcpSender->interleaved[streamType].rtp,
				pSess->pRtpTcpSender->interleaved[streamType].rtcp,
				ssrc);
		}
		else
		{
			pSess->pRtpTcpSender->interleaved[streamType].rtp = 6;
			pSess->pRtpTcpSender->interleaved[streamType].rtcp = 7;
			pTmp += sprintf(pTmp, "Transport: RTP/AVP/TCP;unicast;"
				"interleaved=%d-%d;ssrc=%08x\r\n",
				pSess->pRtpTcpSender->interleaved[streamType].rtp,
				pSess->pRtpTcpSender->interleaved[streamType].rtcp,
				ssrc);
			pTmp += sprintf(pTmp, "x-Dynamic-Rate: 1\r\n");
		}

	}
	else
	{
        TDEBUG(DBG_RTSP, "setup chn = %d, unsupported transport\n",chn);
		RTSP_SendReply(400, 1, "Unsupported Transport", pSess);
		return ret_FAIL;
	}
	pTmp += RTSP_AddReplyHeadDate(pTmp);
    ret = RTSP_SendReply(200, 0, NULL, pSess);
    if(ret == ret_SUCCESS)
    {
        pSess->sessStat = RTSP_STATE_READY;
    }
	
    return ret;
}

int RTSP_EventHandleDescribe(RtspSession_t *pSess)
{
	//TDEBUG(DBG_RTSP, "RTSP_EventHandleDescribe \n");
    int  ret = 0;
    int  frameRate = 0;
    char surveServer[64] = {0};
    char url[150] = {0};
    char *pSdp = NULL;
    char *pTmp = NULL;
    char *p = NULL;
    int width = 1080 , height = 720;
    Rtsp_AuthCbArgs_t AuthCbArgs;
    bzero(&AuthCbArgs, sizeof(AuthCbArgs));
	//DEVINFO_S DevInfo = { 0 };
	//g_pRtspDevIntf->GetDevInfo(&DevInfo);
    if(pSess == NULL)
    {
        return ret_FAIL;
    }
	if(pSess->sessStat == RTSP_STATE_PLAY)
	{
	    TDEBUG(DBG_RTSP, "pses-playing\n");
	   return ret_SUCCESS;
	}
    if(!sscanf(pSess->recvBuf, "DESCRIBE %149s RTSP/1.0", url))
    {
        RTSP_SendReply(RTSP_STATUS_BAD_REQUEST, 1, NULL, pSess);
        TDEBUG(DBG_RTSP, "sscanf url error \n");
        return ret_FAIL;
    }
    strcpy(pSess->uri, url);
    if(strstr(url, "/cam/playback?") || strstr(url, "/playback/"))
    {
        if(ret_SUCCESS !=RTSP_ParsePlybackUrl(url, pSess) || pSess->channel < 0)
        {
            RTSP_SendReply(400, 1, NULL, pSess);
            TDEBUG(DBG_RTSP, "RTSP_ParsePlybackUrl error \n");
            return ret_FAIL;
        }
        AuthCbArgs.reqType = RTSP_PLAYBACK;
    }
    else
    {
        if(ret_SUCCESS != RTSP_ParsePreviewUrl(url, pSess))
        {
            RTSP_SendReply(400, 1, NULL, pSess);
            TDEBUG(DBG_RTSP, "RTSP_ParsePreviewUrl error \n");
            return ret_FAIL;
        }
        AuthCbArgs.reqType = RTSP_PREVIEW;
    }
	
    if(NULL != strstr(pSess->recvBuf, RTSP_HDR_ACCEPT))
    {
        if(NULL == strcasestr(pSess->recvBuf, RTSP_SDP))
        {
            TDEBUG(DBG_RTSP, "only accept require. \n");
            RTSP_SendReply(551, 1, NULL, pSess);
            return ret_FAIL;
        }
    }

	if(NULL != strstr(pSess->recvBuf, RTSP_BACKCHANNEL))
    {
        pSess->backchannel = 1;
    }
	
    RTSP_ParseHdrKeyWord(pSess->recvBuf, pSess->userAgent, RTSP_HDR_USER_AGENT);

    pTmp = pSess->sendBuf + RTSP_GetHead(200, pSess);
    pTmp += sprintf(pTmp, "Content-Type: application/sdp\r\n");

    pSdp = (char *)malloc(4096);
    if(pSdp == NULL)
    {
        TDEBUG(DBG_RTSP, "malloc pSdp error \n");
        return ret_FAIL;
    }
    bzero(pSdp, 4096);
#if 1
    p = CommSdpHandle(pSess, pSdp);

	if (pSess->custom_rtsp == 4)
	{
		//p += sprintf(p, "a= p%u\r\n", DevInfo.specoPID);//lwl

		p += sprintf(p, "a=control:*\r\n");
		p += sprintf(p, "a=packetization-supported:DH\r\n");
		p += sprintf(p, "a=rtppayload-supported:DH\r\n");
		p += sprintf(p, "a=range:npt=now-\r\n");
	}

	//TDEBUG(DBG_RTSP, "custom_rtsp:[%d] \n",pSess->custom_rtsp);
	
	
	
	
	TDEBUG(DBG_RTSP, "is_metadatastream:[%d] \n",pSess->is_metadatastream);
	if(pSess->is_metadatastream)
	{
		#if 0
		p += sprintf(p, "m=application 0 RTP/AVP 107\r\n");
		if(pSess->is_req_multicast)
		{
			char multicast_ip[32];
			int port;
			g_pRtspDevIntf->OnvifMulticastParamAdapter(pSess->streamType, RTP_STREAM_METADATA, multicast_ip, port);
			p += sprintf(p,"c=IN IP4 %s\r\n", multicast_ip);
		}
		p += sprintf(p, "a=control:trackID=3\r\n");
		p += sprintf(p, "a=rtpmap:107 vnd.onvif.metadata/90000\r\n");
		p += sprintf(p, "a=recvonly\r\n");
		#endif
	}
    if(RTSP_GetStreamInfo(pSess) < 0)
    {
    	RTSP_SendReply(404, 1, NULL, pSess);
		RTSP_FreeMemory(&pSdp);
    	TDEBUG(DBG_RTSP, "get playback video audio info error \n");
        return ret_FAIL;
    }

    switch(pSess->vencType)
	{
        case VENC_TYPE_H264:
            ret = H264SdpHandle(pSess, p);
            break;
		#if 0
        case VENC_TYPE_H265:
            ret = H265SdpHandle(pSess, p);
            break;
        case VENC_TYPE_JPEG:
            ret = JpegSdpHandle(pSess, p);
            break;
		#endif
        default:
            //ret = DefuSdpHandle(pSess, p);
			ret = H264SdpHandle(pSess, p);
			break;
    }

#else
    if(!DemoSdpHandle(pSess, pSdp))
    {
    	RTSP_SendReply(404, 0, NULL, pSess);
		RTSP_FreeMemory(&pSdp);
		return GS_FAIL;;
	}
#endif
	TDEBUG(DBG_RTSP, "SDP ret:[%d] \n", ret);

	if(-1 == ret)
	{
		RTSP_SendReply(404, 1, NULL, pSess);
		RTSP_FreeMemory(&pSdp);
		TDEBUG(DBG_RTSP, "SDP Handle Error \n");
		return ret_FAIL;
	}
	/* G711-A */

    //TDEBUG("\nsdp:\n %s\n", pSdp);
    /* G726-16 */
    //p += sprintf(p,"a=rtpmap:97 G726-%d/8000\r\n",16);
    //p += sprintf(p,"a=ptime:20\r\n\r\n");
    //TDEBUG(DBG_RTSP, "strlen(pSdp) = %d", strlen(pSdp));
    int psdplen = strlen(pSdp);
    pTmp += sprintf(pTmp,"Content-Length: %d\r\n", psdplen);
	pTmp += RTSP_AddReplyHeadDate(pTmp);
    //pTmp += sprintf(pTmp,"Content-Base: %s\r\n\r\n", pSess->uri);
    strcat(pTmp, pSdp);
    //strcpy(pSess->uri, (const char *)url);
	RTSP_FreeMemory(&pSdp);
    return RTSP_SendReply(200, 0, NULL, pSess);
}


int rtp_ctrl_get_scale(RtspSession_t *pSess)
{
	int scale;
    char *field=NULL;
	field = strstr(pSess->recvBuf, "Scale: ");
    if(field)
    {
        field += strlen("Scale: ");
        pSess->pbInfo.scale = atof(field);
		scale = (int)pSess->pbInfo.scale;
		if(scale > 1 || scale < 0)
		{
			scale = 1;
		}
		else
		{
			scale = 0;
		}
        TDEBUG(DBG_RTSP, "[1]Scale ...%f\n", pSess->pbInfo.scale);
	}
	field = strcasestr(pSess->recvBuf, "Rate-Control");
	if(field)
	{
		field += strlen("Rate-Control: ");
		if(!strncmp(field, "yes", 3))
		{
			pSess->pbInfo.rateCtrlFlag = 1;
		}
		else
		{
			pSess->pbInfo.rateCtrlFlag = 0;
			pSess->pbInfo.ext_hdr_D = 1;
		}
	}
	return 0;
}

int rtp_ctrl_get_range(RtspSession_t *pSess)
{
    DateTime startTime = {0};
	char *range = NULL;
    range = strstr(pSess->recvBuf, "Range: ");
	if(range)
	{
		range += strlen("Range: ");
    	if(!strncmp(range, "npt=", 4))
    	{
    		range += 4;
			struct tm t = {0};
       		time_t sec = pSess->pbInfo.sTimeSec + atof(range);
			pSess->pbInfo.s_time = atof(range);
       		gmtime_r(&sec, &t);
       		Tm2Datetime(&t, &startTime);
  		}
		else if(!strncmp(range, "clock=", 6))
		{
			//RTSP_GetOnvifReplayRangeClock(range, &startTime, &pSess->pbInfo.endTime, pSess->pbInfo.timediff);
		}
		else
		{
			range = NULL;
		}
	}

	char *imidate = strstr(pSess->recvBuf, "Immediate");
	if(imidate)
	{
		imidate += strlen("Immediate: ");
		if(!strncmp(imidate, "yes", 3))
		{
			pSess->pbInfo.ext_hdr_D = 1;
		}
	}
	return 0;
}


char * rtp_ctrl_set_respo(char *pTmp, const RtspSession_t *pSess)
{
	//playback
	pTmp += sprintf(pTmp, "RTP-Info: ");
	#if 0
	switch(pSess->transportType)
    {
        case RTP_TRANSPORT_TYPE_TCP:
			if(pSess->reqStreamFlag[RTP_STREAM_VIDEO])
				pTmp += sprintf(pTmp, "url=%s/trackID=%d;seq=%hu;rtptime=%u",
								 pSess->uri, 0, pSess->pRtpTcpSender->rtpSeq[RTP_STREAM_VIDEO]+1,
								 pSess->pRtpTcpSender->rtpPts[RTP_STREAM_VIDEO]);
			if(pSess->reqStreamFlag[RTP_STREAM_VIDEO] && pSess->reqStreamFlag[RTP_STREAM_AUDIO])
				pTmp += sprintf(pTmp, ",");
			if(pSess->reqStreamFlag[RTP_STREAM_AUDIO])
				pTmp += sprintf(pTmp, "url=%s/trackID=%d;seq=%u;rtptime=%u",
								pSess->uri, 1, pSess->pRtpTcpSender->rtpSeq[RTP_STREAM_AUDIO]+1,
								 pSess->pRtpTcpSender->rtpPts[RTP_STREAM_AUDIO]);
			break;
        case RTP_TRANSPORT_TYPE_UDP:
			if(pSess->reqStreamFlag[RTP_STREAM_VIDEO])
				pTmp += sprintf(pTmp, "url=%s/trackID=%d;seq=%hu;rtptime=%u",
								 pSess->uri, 0, pSess->pRtpUdpSender[RTP_STREAM_VIDEO]->rtpSeq+1,
								 pSess->pRtpUdpSender[RTP_STREAM_VIDEO]->rtpPts);
			if(pSess->reqStreamFlag[RTP_STREAM_VIDEO] && pSess->reqStreamFlag[RTP_STREAM_AUDIO])
				pTmp += sprintf(pTmp, ",");
			if(pSess->reqStreamFlag[RTP_STREAM_AUDIO])
				pTmp += sprintf(pTmp, "url=%s/trackID=%d;seq=%u;rtptime=%u",
								pSess->uri, 1, pSess->pRtpUdpSender[RTP_STREAM_AUDIO]->rtpSeq+1,
								 pSess->pRtpUdpSender[RTP_STREAM_AUDIO]->rtpPts);
			break;
        default:
            break;
    }
	#endif
	pTmp += sprintf(pTmp, "\r\n");

	return pTmp;
}

char * rtp_prev_set_respo(char *pTmp, const RtspSession_t *pSess)
{
	pTmp += sprintf(pTmp, "RTP-Info: ");
    switch(pSess->transportType)
    {
    	case RTP_TRANSPORT_TYPE_UDP:
		case RTP_TRANSPORT_TYPE_BUTT:
			break;
        case RTP_TRANSPORT_TYPE_TCP:
			if(pSess->reqStreamFlag[RTP_STREAM_VIDEO])
				pTmp += sprintf(pTmp, "url=%s/trackID=%d;seq=%hu;rtptime=%llu",
								 pSess->uri, 0, pSess->rtpSeq[RTP_STREAM_VIDEO],
								 pSess->rtpPts[RTP_STREAM_VIDEO]);
			if(pSess->reqStreamFlag[RTP_STREAM_VIDEO] && pSess->reqStreamFlag[RTP_STREAM_AUDIO])
				pTmp += sprintf(pTmp, ",");
			if(pSess->reqStreamFlag[RTP_STREAM_AUDIO])
				pTmp += sprintf(pTmp, "url=%s/trackID=%d;seq=%u;rtptime=%llu",
								pSess->uri, 1, pSess->rtpSeq[RTP_STREAM_AUDIO],
								 pSess->rtpPts[RTP_STREAM_AUDIO]);
            break;
    }
	pTmp += sprintf(pTmp, "\r\n");

	return pTmp;
}

int check_pts(int &div, u32 ptsdiff)
{
	div += (ptsdiff%1000);
	if( div >= 1000)
	{
		div -= 1000 ;
		return 1;
	}
	return 0;
}

int RTP_sendBlock(RtspSession_t *pSess)
{
    int  sfd = 0,  send_count = pSess->vIovcSendInfo.size(), ret = 0 , scount =0, senlen = 0;
	unsigned int i = 0;
	iovec *ibegin = &pSess->vIovcSendInfo[0];
	sfd = pSess->pRtpTcpSender->tcpSockFd;
	#if 0
	char tmpbuf[128]={0};
	for( i = 0 ; i < pSess->vIovcSendInfo.size(); i++)
	{
		fwrite(pSess->vIovcSendInfo[i].iov_base, 1, pSess->vIovcSendInfo[i].iov_len, pSess->f1);
		
		sprintf(tmpbuf,"i=%d, datalen=%d\n",i,pSess->vIovcSendInfo[i].iov_len );
		
		fwrite(tmpbuf, 1, strlen(tmpbuf), pSess->f3);
		bzero(tmpbuf,strlen(tmpbuf));
	}
	#endif
	//TDEBUG(DBG_RTSP, "sending ...\n");
	pthread_mutex_lock(&pSess->mRtspRtpSynchronized);
	while(1)
	{
		
		ret = writev(sfd, ibegin,send_count);
		TDEBUG(DBG_RTSP, "socket-test:ret=%d,sfd=%d \n",ret, sfd);
		if (ret <= 0)
		{ 
			if (errno == EAGAIN  || errno == EINTR )
			{
				TDEBUG(DBG_RTSP, "---errno,errno=%d\n",errno);
				if(errno == EAGAIN )
					TDEBUG(DBG_RTSP, "---errno == EAGAIN  || errno == EINTR,errno=%d\n",errno);
				if(scount++ < 4)
				{
					usleep(1000*200);
					continue;
				}
				break;
			}
			else
			{
				TDEBUG(DBG_RTSP, "socket-error:ret=%d,send_count=%d \n",ret, send_count);
				
				perror("socket-error:");
				
				break;
			}
		}
		senlen += ret;
		if(senlen < pSess->dataoffset)
		{
		
			TDEBUG(DBG_RTSP, "cc1 sendbyte=%d, totalbyte1=%d,tot2=%d, sinfosize=%d, sendcount=%d\n", senlen, pSess->dataoffset,pSess->vSendInfo[i].offset, pSess->vSendInfo.size(), send_count);
			for( i = 0 ; i < pSess->vSendInfo.size(); i++)
			{
				if(senlen < pSess->vSendInfo[i].offset)
				{
					if(senlen > pSess->vSendInfo[i].offset - pSess->vSendInfo[i].headlen)
					{
						send_count = pSess->vIovcSendInfo.size() - (2*i + 1);
						ibegin = &pSess->vIovcSendInfo[2*i + 1];
					}
					else
					{
						send_count = pSess->vIovcSendInfo.size() - 2*i;
						ibegin = &pSess->vIovcSendInfo[2*i];
					}
					break;
				}
			}
			
			TDEBUG(DBG_RTSP, "cc2 sendbyte=%d, totalbyte1=%d,tot2=%d, i=%d, sendcount=%d\n", senlen, pSess->dataoffset,pSess->vSendInfo[i].offset, i, send_count);
		}
		else
		{
			//RTSP_DBG_PRINT("ok sendbyte=%d, totalbyte1=%d,sendcount=%d\n", senlen, pSess->dataoffset, send_count);
			break;
		}
		
	}
	pthread_mutex_unlock(&pSess->mRtspRtpSynchronized);
	return ret;
}



RtpPt_e RTP_ParseAudioEncType(int audioEncType, u32 *audioBitRate)
{
	/*
		At present ,we add tow diffrent audio source information(G711A AND G711U),if someone need other type of audio
		please add below.
	*/
	RtpPt_e ret = RTP_PT_INVALID;
	#if 0
	switch(audioEncType)
	{
		case G711_A:
			ret = RTP_PT_ALAW;
			break;

		case G711_U:
			ret = RTP_PT_ULAW;
			break;

		case ADPCM_DVI4:
			ret = RTP_PT_ADPCM;
			break;

		case G726_16KBPS:
			*audioBitRate = 16;
			ret = RTP_PT_G726;
			break;
		case G726_24KBPS:
			*audioBitRate = 24;
			ret = RTP_PT_G726;
			break;

		case G726_32KBPS:
			*audioBitRate = 32;
			ret = RTP_PT_G726;
			break;

		case G726_40KBPS:
			*audioBitRate = 40;
			ret = RTP_PT_G726;
			break;
		case AT_AAC:
			*audioBitRate = 8000;
			ret = RTP_PT_ADPCM;
			break;
		default:
			TDEBUG(DBG_RTSP, "RTP_ParseAudioEncType error \n");
			break;
	}
	#endif
	ret = RTP_PT_ALAW;

	return ret;
}

static int rtsp_GetFrame(RtspSession_t *pSess,  u32 *pts, u32 *offset, u32 *type)
{
	//TDEBUG(DBG_RTSP, "rtsp_GetFrame_test!\n");

	int s32Ret, flen = 0;
	RSFrameHead *pframeHead = NULL;
	*type = 0;

#if 0
	char buf[10] = {0};
	s32Ret = read(pSess->socketspair[1], buf, 8);

	if (s32Ret != 8)
	{
		if (errno == ETIMEDOUT)
			TDEBUG(DBG_RTSP, "socketspair read timed out\n");
		else 
			perror("----11plt-socketspair");
		return -2;
	}
	if(strncmp(buf,"HaveData",8) !=0)
	{	
	   TDEBUG(DBG_RTSP, "socketspair read data error buf=%s\n",buf);
	   
	   return -2;
	}
#endif
#if 0
	//读取数据出去准备发送
	g_rtsplock.lock();
	unsigned char *pos = (unsigned char *)g_getStream.front();
	g_getStream.pop_front();
	g_rtsplock.unlock();
#endif


	unsigned char *pos = NULL;
	RSFrameHead *pHead;

	while(1)
	{		
		g_rtsplock.lock();
		if(0 == g_getStream.size())
		{
			g_rtsplock.unlock();
			usleep(100);
			continue;
		}
		
		pos = (unsigned char *)g_getStream.front();
		g_getStream.pop_front();
		g_rtsplock.unlock();
		
		pHead = (RSFrameHead*)(pos);
		if(pHead->FrameType == RSIFrame || pHead->FrameType == RSPFrame)
		{
			break;
		}
		else
		{
			//过滤掉 A 帧
			free((void *)pos);
		}
	}



	//TDEBUG(DBG_RTSP, "rtsp_GetFrame_get data pos!\n");
#if 1
	//RSFrameHead *pHead = (RSFrameHead*)(pos);
	if (pHead->FrameTag != RSFrameTag)
	{
		TDEBUG(DBG_RTSP, "Can't get a correct frame!\n");
		return 0;
	}

	pSess->stFrameInfo.u32FrameLen = sizeof(RSFrameHead) + pHead->FrameSize;
	pSess->stFrameInfo.ps8Buffer = (char*)pos;

	flen =  pHead->FrameSize;
	pframeHead = (RSFrameHead*)(pSess->stFrameInfo.ps8Buffer);

	RtpPt_e audioTypeFrame = RTP_PT_ULAW;
	u32 audioBitRate = 64;
	
	//TDEBUG(DBG_RTSP, "_check_data_ [%c]!\n", pframeHead->FrameType);
	if (pframeHead->FrameType != RSAFrame)
	{
		//*pts = pframeHead->Time / 1000 * 90;
		*offset = sizeof(RSFrameHead);
		*type = pframeHead->FrameType;
		if(pSess->rtpPts[RTP_STREAM_VIDEO]== 123456)
		{
			*pts = pSess->rtpPts[RTP_STREAM_VIDEO];
			pSess->videots = *pts;
		}
		else
		{
			unsigned int ptsdiff = (pframeHead->Pts - pSess->rtpPts[RTP_STREAM_VIDEO] )/1000*90;
			pSess->videots += ptsdiff;

			
			pSess->videots += check_pts(pSess->videoPtsdiv,pframeHead->Pts - pSess->rtpPts[RTP_STREAM_VIDEO])*90;
			*pts = pSess->videots;
		}


		pSess->rtpPts[RTP_STREAM_VIDEO] =  pframeHead->Pts;

	}
	else
	{
		#if 0
		*pts = pframeHead->Time / 1000 * (pSess->mAudioRealBitRate/1000);

		bool have_hisi_head = false;
            short* temp_audio_head = (short *)(pHead + sizeof(RSFrameHead));
        if (temp_audio_head[0] == 0x0100 && temp_audio_head[1] == ((pHead->FrameSize-4)>>1))
            have_hisi_head = true;
		
		if(have_hisi_head)
		{
			*offset = sizeof(RSFrameHead) + 4;
			flen -= 4;
		}
		else			
		    *offset = sizeof(RSFrameHead) ;
		
		*type = AUDIO_TYPE;
		audioTypeFrame = RTP_ParseAudioEncType(pframeHead->EncodeType, &audioBitRate);

		if(pSess->rtpPts[RTP_STREAM_AUDIO]== 123456)
		{
			*pts = pSess->rtpPts[RTP_STREAM_AUDIO];
			pSess->audioPts = *pts;
		}
		else
		{
			unsigned int ptsdiff = (pframeHead->Pts - pSess->rtpPts[RTP_STREAM_AUDIO] )/1000*(pframeHead->AudioHead.SampleRate/1000);
			pSess->audioPts += ptsdiff;
			pSess->audioPts += check_pts(pSess->audioPtsdiv,pframeHead->Pts - pSess->rtpPts[RTP_STREAM_AUDIO])*(pframeHead->AudioHead.SampleRate/1000);

			*pts = pSess->audioPts;
		}
			
		pSess->rtpPts[RTP_STREAM_AUDIO] =  pframeHead->Pts;
		#endif
	}

	if (pSess->sessStat == RTSP_STATE_PLAY && pframeHead->FrameType != RSAFrame)
	{
		if (pframeHead->VideoHead.Width != pSess->mWidth
			|| pSess->mHeight != pframeHead->VideoHead.Height
			|| pSess->PreVEncType != pframeHead->EncodeType)
		//	|| pSess->mVideoBitRate != stinfo.videoAttr.bitRate)
		{
			TDEBUG(DBG_RTSP, "frame info is not the same to before:\n");
			printf("[Frame] Width = %u, Height = %u, EncodeType = %d bitrate =  \n",
				pframeHead->VideoHead.Width, pframeHead->VideoHead.Height, pframeHead->EncodeType);
		//		stinfo.videoAttr.bitRate);

			TDEBUG(DBG_RTSP, "[pSess] Widht = %u, Height = %u,EncodeType = %d bitrate = %d \n",
				pSess->mWidth, pSess->mHeight, pSess->vencType, pSess->mVideoBitRate);
			return -2;
		}
		/* save current 'pframeHead->EncodeType' for judge next frame encode type wheater to modifiy */
		pSess->PreVEncType = pframeHead->EncodeType;
	}
	/*
	if (pSess->sessStat == RTSP_STATE_PLAY && pframeHead->FrameType == RSAFrame)
	{
		if (pSess->audioEncType != audioTypeFrame)
		{
			TDEBUG(DBG_RTSP, "Audio frame info is not the same to before:\n");
			TDEBUG(DBG_RTSP, "[Frame]: audioTypeFrame: %d  audioBitRate: %u\n\n", audioTypeFrame, audioBitRate);
			TDEBUG(DBG_RTSP, "pSess->audioEncType: %d  pSess->mAudioBitRate; %u\n", pSess->audioEncType, pSess->mAudioBitRate);
		}
		pSess->audioEncType = audioTypeFrame;
	}*/
	//TDEBUG(DBG_RTSP, "ret_surccess [%c]Frame \n", pframeHead->FrameType);

	//pSess->_rar->read(boost::bind(&read_callback, _1, pSess, pSess->uniqueID));
#endif

	return flen;
}

int RTP_TcpPacket(RtspSession_t *pSess, RtpPt_e payloadType, u32 pts,
									int marker, const int len, char *data, int extHdrLen)
{
	
	RtpTcpSender_t *pSender = pSess->pRtpTcpSender;
	iovec ivc;
	RtpHdr_t *pRtpHdr = NULL;
	RtpStreamType_e streamType;
	unsigned short *intlvd_ch = (unsigned short *)(pSess->sinfo.ch_rtpHead+2);

	pSender->sendLen = 0;
	pRtpHdr = (RtpHdr_t *)(pSess->sinfo.ch_rtpHead + 4);
	RTP_HDR_SET_VERSION(pRtpHdr, RTP_VERSION);
    RTP_HDR_SET_P(pRtpHdr, 0);
	if(extHdrLen)
	{
    	RTP_HDR_SET_X(pRtpHdr, 1);
	}
	else
	{
    	RTP_HDR_SET_X(pRtpHdr, 0);
	}
    RTP_HDR_SET_CC(pRtpHdr, 0);
    RTP_HDR_SET_M(pRtpHdr, marker);
    RTP_HDR_SET_PT(pRtpHdr, payloadType);
	if((payloadType == RTP_PT_H264) || (payloadType == RTP_PT_MPEG4) || (payloadType == RTP_PT_JPEG))
   	{
		streamType = RTP_STREAM_VIDEO;
		RTP_HDR_SET_SSRC(pRtpHdr, htonl(pSender->videoH264Ssrc));
	}
	else
	{
		streamType = RTP_STREAM_AUDIO;
		RTP_HDR_SET_SSRC(pRtpHdr, htonl(pSender->audioG711Ssrc));
	}
	
	RTP_HDR_SET_SEQNO(pRtpHdr, htons(pSender->rtpSeq[streamType]));		
    RTP_HDR_SET_TS(pRtpHdr, htonl(pts));

	pSender->rtpSeq[streamType]++;		
	pSender->rtpPts[streamType] = pts;

	pSess->sinfo.datapos = data;
    pSender->sendLen = RTP_HDR_LEN + len + extHdrLen ;
	pSess->sinfo.datalen = len ;
	pSess->sinfo.headlen = RTP_HDR_LEN + extHdrLen + 4;
	if(pSess->fuabflag == 1)
	{	
		memcpy(&pSess->sinfo.ch_rtpHead[pSess->sinfo.headlen], pSess->fuab, pSess->fualen);
		pSess->sinfo.headlen += pSess->fualen;
		
		pSender->sendLen = RTP_HDR_LEN + len + extHdrLen + pSess->fualen;
	}
	
 	pSess->sinfo.ch_rtpHead[0] = '$';
	if((payloadType == RTP_PT_H264)
    	    || (payloadType == RTP_PT_MPEG4)
    	    || (payloadType == RTP_PT_JPEG))
    {
		pSess->sinfo.ch_rtpHead[1] = pSender->interleaved[RTP_STREAM_VIDEO].rtp;
	}
	else
	{
		pSess->sinfo.ch_rtpHead[1] = pSender->interleaved[RTP_STREAM_AUDIO].rtp;
	}
	*intlvd_ch = htons((unsigned short) pSender->sendLen);
	
	pSess->dataoffset += len + pSess->sinfo.headlen;
	pSess->sinfo.offset = pSess->dataoffset;
	pSess->vSendInfo.push_back(pSess->sinfo);
	
	ivc.iov_base = pSess->vSendInfo.back().ch_rtpHead;
	ivc.iov_len = pSess->sinfo.headlen;
	
	pSess->vIovcSendInfo.push_back(ivc);
	
	ivc.iov_base = pSess->sinfo.datapos;
	ivc.iov_len = len;
	pSess->vIovcSendInfo.push_back(ivc);
	//fwrite(pSess->sinfo.ch_rtpHead, 1, pSess->sinfo.headlen, pSess->f2);
	//fwrite(data, 1, len, pSess->f2);
	//char tmpbuf[128]={0};
	//sprintf(tmpbuf,"hedlen=%d, datalen=%d\n",pSess->sinfo.headlen, len);
	//fwrite(tmpbuf, 1, strlen(tmpbuf), pSess->f3);
	bzero(&pSess->sinfo,sizeof(rtp_send_info_t));
    return ret_SUCCESS;

}




void RTSP_PushStream(const char *pstreambuf, unsigned int buf_sz)
{
	const char *ptemp = NULL;
	
	if(NULL == pstreambuf)
		return;

#if 0	
	if(gRtspVodSvr.rtspSessWorkList.size() == 0)
	{
		free((void *)pstreambuf);
		return;
	}
#endif

	modCommon::AutoLock tauto(g_rtsplock);	//加锁
	g_getStream.push_back(pstreambuf);
	if(g_getStream.size()>10)
	{
		ptemp = g_getStream.front();
		g_getStream.pop_front();
		if(NULL != ptemp)
		{
			free((void *)ptemp);
		}
	}
	return;
}
/*
int RTSP_GetAppReadStreamCb(AppReadStreamFunc *pfunc)
{
	*pfunc = ReadStream;
	return 0;
}
*/

void *RTP_TcpSendH264ThrFxn(void *pArgs)
{

	int ret;
	int nalType, pos, leftLen, dataLen, NalLen;
	int pktLen;
	u32 pts, offset, type;
	u8  sToken;
	//char sBuf[MAX_RTP_LEN];
	struct timeval timeout;
	#ifdef __one_copy__
	char	*pBuf = NULL ; 
	#endif 
	char 	*pData = NULL;
	char 	*pSend = NULL;
	char 	*pNal  = NULL;
	RtspSession_t *pSess = (RtspSession_t *)pArgs;
	
	if(pSess == NULL ){
		TDEBUG(DBG_RTSP, "Input params error \n");
		return NULL;
	}
	//RtpPt_e audioEncType = pSess->audioEncType;
	if(pSess->pRtpTcpSender == NULL){
		TDEBUG(DBG_RTSP, "tcpSender is NULL \n");
		pSess->sessStat = RTSP_STATE_STOP;
		return NULL;
	}
	
	prctl(PR_SET_NAME, (unsigned long)"RTP_TcpSH264Thr", 0, 0, 0);
	//int flags = fcntl(pSess->pRtpTcpSender->tcpSockFd, F_GETFL, 0);
	//fcntl(pSess->pRtpTcpSender->tcpSockFd, F_SETFL, flags | O_NONBLOCK);

	timeout.tv_sec  = 5;
	timeout.tv_usec = 0;
	TDEBUG(DBG_RTSP, "thread start remoteip=%s,port=%d add uid=%llu,psess=%p,fd=%d \n",pSess->remoteIp,pSess->remotePort,pSess->uniqueID, pSess, pSess->rtspSockFd);

	setsockopt(pSess->rtspSockFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

#if 0
	RTP_Buf_t rb;
	bzero(&rb,sizeof(rb));
	RTP_buf_init(&rb,1024*64);
#else
#if 0
	int readid ;
	readid = GetReadIdAndBuf( pSess);
	if( readid == 0)
	{
		TDEBUG(DBG_RTSP, "malloc is NULL \n");
		pSess->sessStat = RTSP_STATE_STOP;
		return NULL;
	}
	//DEVINFO_S stDevInfo;
#endif
#endif
	while(RTSP_STATE_PLAY == pSess->sessStat)
	{
		ret = 	rtsp_GetFrame(pSess , &pts, &offset, &type);
		//TDEBUG(DBG_RTSP, "read GetFrame type:[%d] ret:[%d] \n", type , ret);
		if(ret > 0)
		{
			pktLen = ret;
			pData  = pSess->stFrameInfo.ps8Buffer + offset;
		}
		else
		{
			TDEBUG(DBG_RTSP, "read data error, \n");
			break;
		}

		
		if(type == AUDIO_TYPE)
		{
			#if 0
			if (  pSess->reqStreamFlag[RTP_STREAM_AUDIO]){
				if( audioEncType != RTP_PT_INVALID ){	//* lwx, 2010.11.19
					if(RTP_AudioFrameSize(pktLen) == -1) {
						continue;
					}
					RTP_TcpPacket(pSess, (RtpPt_e)audioEncType, pts, 1,pktLen, pData, 0);
				}
			}
			#endif
		}
		else
		{
			if (  pSess->reqStreamFlag[RTP_STREAM_VIDEO])
			{
				pSend  	= pData + H264_STARTCODE_LEN;
				dataLen = pktLen - H264_STARTCODE_LEN;
				while(1)
				{
					pNal = Find_next_nal(pSend, dataLen);
					if(pNal != NULL)
					{
						NalLen   = pNal - pSend - H264_STARTCODE_LEN;
						dataLen -= NalLen + H264_STARTCODE_LEN;
					}
					else
					{
						NalLen = dataLen;
					}

					//TDEBUG(DBG_RTSP, "NalLen:[%d] \n", NalLen);
					if(NalLen <= NAL_FRAGMENTATION_SIZE)
					{
						
						RTP_TcpPacket(pSess, RTP_PT_H264, pts, (pktLen<=NAL_FRAGMENTATION_SIZE?1:0), NalLen, pSend, 0);
					}
					else
					{
						nalType = H264_Get_NalType(*pSend);
						pSess->fualen = 2;
						pSess->fuabflag = 1;		
						pSess->fuab[0] = 0x1c | (*pSend & (~0x1f));

						leftLen = NalLen;
						sToken  = 1;
						pos     = 0;
						while(leftLen > NAL_FRAGMENTATION_SIZE)
						{
							if(RTSP_STATE_PLAY != pSess->sessStat)
							{
								TDEBUG(DBG_RTSP, "pSess stat is not play \n");
								goto RtpTcpThrFxnExit;
							}
							pSess->fuab[1] = (sToken << 7) | nalType;

							RTP_TcpPacket(pSess, RTP_PT_H264, pts, 0,NAL_FRAGMENTATION_SIZE  - sToken, pSend+pos+sToken, 0);
							
							sToken 	= 0;
							leftLen -= NAL_FRAGMENTATION_SIZE;
							pos 	+= NAL_FRAGMENTATION_SIZE;
						}
						if(sToken)
						{
							nalType |= 128;
						}
						pSess->fuab[1] = 64 | nalType;

						RTP_TcpPacket(pSess, RTP_PT_H264, pts, 1, leftLen-sToken, pSend+pos+sToken , 0);
					}
					if(NalLen == dataLen)
						break;
					pSend = pNal ;
				}
			}
		}
		//TDEBUG(DBG_RTSP, "tcp send h264 ... \n");
		if(pSess->vIovcSendInfo.size() == 0)
			continue;
		if(RTP_sendBlock(pSess) <=0)
			break;
		free(pSess->stFrameInfo.ps8Buffer);
		pSess->vSendInfo.clear();
		pSess->vIovcSendInfo.clear();
		pSess->dataoffset = 0;
		pSess->fuabflag = 0;	
	}
RtpTcpThrFxnExit:

	TDEBUG(DBG_RTSP, "tcp send h264 thread exit ... \n");
	pSess->sessStat = RTSP_STATE_STOP;
	TDEBUG(DBG_RTSP, "RtpTcpThrFxnExit !!! \n\n");
	//fclose(	pSess->f1 );
	//fclose(	pSess->f2 );
	//fclose(pSess->f1);
	//pSess->f1 = NULL;

	return NULL;
}


int RTP_TcpSendH264VideoData(RtspSession_t *pSess)
{
	#if 0
	if (pSess->custom_rtsp == 1)
	{
		
		if (0 != pthread_create(&pSess->tcpThrId, NULL, RTP_Speco_vms_thread, pSess))
		{
			TDEBUG(DBG_RTSP, "pthread_create RTP_Speco_vms_thread error \n");
			return ret_FAIL;
		}
	}
	
	else if (pSess->is_metadatastream  && pSess->setupFlag[RTP_STREAM_METADATA] == 1)
	{
		if (0 != pthread_create(&pSess->tcpAppId, NULL, RTP_TcpSendMetadata, pSess)) {
			dbg(Err, DbgPerror, "pthread_create RTP_TcpSendMetadataThrFxn error \n");
			return GS_FAIL;
		}
	}
	else
	#endif
	{
		if (0 != pthread_create(&pSess->tcpThrId, NULL, RTP_TcpSendH264ThrFxn, pSess)) {
			TDEBUG(DBG_RTSP, "pthread_create RTP_TcpSendH264ThrFxn error \n");
			return ret_FAIL;
		}
	}
	
	return ret_SUCCESS;
}


int RTP_TcpStartSendMediaData(RtspSession_t *pSess)
{
	int ret = ret_FAIL;
#ifdef RT_DEBUG
	if(gpf==NULL)
		gpf=fopen("streamfile.265","wb+");
	if(gpf==NULL)
	{
		printf("openfile error:%s\n",strerror(errno));
		return NULL;
	}
#endif
  	
	TDEBUG(DBG_RTSP, "RTP_rtspSessType:[%d], vencType:[%d]\n", pSess->rtspSessType, pSess->vencType);
#if 0	
	if(pSess->rtspSessType == RTSP_SESS_PLYBACK)
	{
			ret = RTP_TcpPlayBackVideoData(pSess);
	}
	else
#endif		
	{
		switch(pSess->vencType)
		{

		case VENC_TYPE_H264:
				ret = RTP_TcpSendH264VideoData(pSess);
				break;
		#if 0
		case VENC_TYPE_H265:
				ret = RTP_TcpSendH265VideoData(pSess);
				break;
		case VENC_TYPE_JPEG:
				ret = RTP_TcpSendJpegVideoData(pSess);
				break;
		#endif
		default:
				TDEBUG(DBG_RTSP, "GetVideoEncType error \n");
				break;
		}
	}
	#if 0
	if ( pSess->custom_rtsp == 2 && pSess->setupFlag[RTP_APP_T] == 1)
	{
		if (0 != pthread_create(&pSess->tcpAppId, NULL, RTP_TcpSendSpecoApp, pSess)) {
			TDEBUG(DBG_RTSP, "pthread_create RTP_TcpSendSpecoApp error \n");
			return ret_FAIL;
		}
	}
	#endif
	return ret;
}


int RTSP_EventHandlePlay(RtspSession_t *pSess)
{
    int ret= ret_SUCCESS;
    char *pTmp = NULL;
	if(pSess->sessStat == RTSP_STATE_PLAY && pSess->rtspSessType == RTSP_SESS_PREVIEW )
	{
	    TDEBUG(DBG_RTSP, "pses-playing\n");
		return ret_SUCCESS;
	}

#if 0
    if((pSess->sessStat == RTSP_STATE_PAUSE
		||pSess->sessStat == RTSP_STATE_PLAY) && pSess->rtspSessType == RTSP_SESS_PLYBACK)
    {
		char *pTmp = pSess->sendBuf;

		rtp_ctrl_get_range(pSess);
		rtp_ctrl_get_scale(pSess);
#ifndef DEMO
		if(pSess->pbInfo.s_time > 0.1f)
			ret = getSeekNpt(pSess);
		else
		{
			ret = (pSess->LastFramePts - pSess->pbInfo.RealstartPts) / 1000000;
			pSess->LastPts = pSess->LastFramePts * 90;
			if (pSess->videots != 0)
				pSess->videots += pSess->ptsdif / 1000;
			else
				pSess->videots = pSess->LastPts / 1000;
		
			if (RTP_TRANSPORT_TYPE_TCP == pSess->transportType)
			{
				if(pSess->pRtpTcpSender != NULL)
				{
					pSess->pRtpTcpSender->rtpPts[RTP_STREAM_VIDEO] = pSess->videots;
				}
			}
			else
			{
				if(pSess->pRtpUdpSender[RTP_STREAM_VIDEO] != NULL)
				{
					pSess->pRtpUdpSender[RTP_STREAM_VIDEO]->rtpPts = pSess->videots;
				}
			}
		}
#endif
		pTmp += RTSP_GetHead(RTSP_STATUS_OK, pSess);
		pTmp += sprintf(pTmp,"Session: %s\r\n", pSess->sessId);
		
		pTmp += sprintf(pTmp, "Range: npt=%d-%d\r\n",ret, pSess->pbInfo.e_time);
		pTmp = rtp_ctrl_set_respo(pTmp, pSess);
		pTmp += RTSP_AddReplyHeadDate(pTmp);
		RTSP_SendReply(RTSP_STATUS_OK, 0, NULL, pSess);
		pSess->pbInfo.seekflag = 1;
		pSess->pbInfo.frist_flag = 1;

#ifndef DEMO
		if (pSess->pbInfo.s_time > 0.1f)
		{
			RfsFrameInterface::RecordTimeS rs;
			memcpy(&rs, &pSess->pbInfo.startTime, sizeof(RfsFrameInterface::RecordTimeS));
			pSess->pbInfo.pFsVisitor->seek(rs);
		}

#endif
		pSess->pbInfo.s_time = 0.1f;
		pSess->sessStat = RTSP_STATE_PLAY;
        return ret_SUCCESS;
    }
#endif

	TDEBUG(DBG_RTSP, "rtspSessType:[%d]!\n",pSess->rtspSessType);//0
	if(pSess->rtspSessType == RTSP_SESS_PLYBACK && strstr(pSess->recvBuf, "Require: onvif-replay"))
	{
		#if 0
		ret = RTSP_EventHandleOnvifReplay(pSess);
		if(ret == ret_FAIL)
		{
			TDEBUG(DBG_RTSP, "onvif-replay:Range format error!\n");
			RTSP_SendReply(RTSP_STATUS_BAD_REQUEST, 1, "Request error! \n", pSess);
			return ret;
		}
		#endif
    }
    pTmp = pSess->sendBuf;
    pTmp += RTSP_GetHead(RTSP_STATUS_OK, pSess);
    pTmp += sprintf(pTmp,"Session: %s;timeout=%d\r\n", pSess->sessId, pSess->tcpTimeOut);

	TDEBUG(DBG_RTSP, "transportType:[%d]!\n",pSess->transportType);
    if(pSess->rtspSessType == RTSP_SESS_PLYBACK)
    {
    	#if 0
#ifndef DEMO
		getSeekNpt(pSess);
#endif
        pTmp = rtp_ctrl_set_respo(pTmp, pSess);
		if(pSess->rtspSessType == RTSP_SESS_PLYBACK && strstr(pSess->recvBuf, "Require: onvif-replay"))
			pSess->pbInfo.e_time = Dt2Sectime(&pSess->pbInfo.endTime) + pSess->pbInfo.timediff - Dt2Sectime(&pSess->pbInfo.startTime);
		else
			pSess->pbInfo.e_time = Dt2Sectime(&pSess->pbInfo.endTime) - Dt2Sectime(&pSess->pbInfo.startTime);
		
		if (pSess->pbInfo.e_time < 0)
		{
			TDEBUG(DBG_RTSP, "RTSP request time error\n");
			return ret_FAIL;
		}
		pTmp += sprintf(pTmp, "Range: npt=0.0-%d\r\n", pSess->pbInfo.e_time);
		#endif
    }
	else
	{
		if (RTP_TRANSPORT_TYPE_TCP == pSess->transportType)
		{
			pSess->rtpPts[RTP_STREAM_VIDEO] = 123456;
			pSess->rtpSeq[RTP_STREAM_VIDEO] = getRandNum(100);
			pSess->rtpPts[RTP_STREAM_AUDIO] = 123456;
			pSess->rtpSeq[RTP_STREAM_AUDIO] = pSess->pRtpTcpSender->rtpSeq[RTP_STREAM_VIDEO] + 101;   
			pSess->pRtpTcpSender->rtpPts[RTP_STREAM_VIDEO] = 123456;
			pSess->pRtpTcpSender->rtpSeq[RTP_STREAM_VIDEO] = pSess->rtpSeq[RTP_STREAM_VIDEO];
			pSess->pRtpTcpSender->rtpPts[RTP_STREAM_AUDIO] = 123456;
			pSess->pRtpTcpSender->rtpSeq[RTP_STREAM_AUDIO] = pSess->rtpSeq[RTP_STREAM_AUDIO];	 
			
			pTmp = rtp_prev_set_respo(pTmp, pSess);
		}
	}

#ifndef DEMO
#if 0
	//检查带宽 lwl
	ret = RTSP_StartCheckCurrentBW(pSess);
	if (ret == ret_FAIL)
	{
		TDEBUG(DBG_RTSP, "preview: There is not enough bandwidth at device!\n");
		RTSP_SendReply(RTSP_STATUS_BANDWIDTH_LACK, 1, NULL, pSess);
		return ret_FAIL;
	}
#endif		
#endif

    pTmp += RTSP_AddReplyHeadDate(pTmp);

    ret = RTSP_SendReply(RTSP_STATUS_OK, 0, NULL, pSess);
    if(ret == ret_SUCCESS)
    {
        pSess->sessStat = RTSP_STATE_PLAY;
    }
	else
	{
	   return ret_FAIL;
	}
	

#if 0	
	if(NULL != strstr(pSess->recvBuf, RTSP_BACKCHANNEL))
    {
       	if (RTP_TRANSPORT_TYPE_TCP == pSess->transportType)
       	{
			if (ret_SUCCESS != RTP_TcpStartRecvBackChData(pSess))
			{
				//dbg(Err, DbgNoPerror, "RTP_TcpRecvBackChannelData error \n ");
				pSess->sessStat = RTSP_STATE_STOP;
				RTSP_SendReply(911, 1, "create tcp thread error \n", pSess);
				return ret_FAIL;
			}
		}
		else if(RTP_TRANSPORT_TYPE_UDP ==  pSess->transportType)
		{
			if (ret_SUCCESS != RTP_UdpStartRecvBackChData(pSess))
			{
				//dbg(Err, DbgNoPerror, "RTP_UdpStartRecvBackChData error \n ");
				pSess->sessStat = RTSP_STATE_STOP;
				RTSP_SendReply(911, 1, "create tcp thread error \n", pSess);
				return ret_FAIL;
			}
		}
    }
	else if(strcasestr(pSess->recvBuf, "multicast") != NULL && pSess->is_req_multicast)
	{
		if(RTP_TRANSPORT_TYPE_UDP ==  pSess->transportType)
		{
			
			if(pSess->rtspSessType == RTSP_SESS_PREVIEW){

				if (GS_SUCCESS != RTP_UdpMulticastSendMediaData(pSess))
				{
					pSess->sessStat = RTSP_STATE_STOP;
					RTSP_SendReply(911, 1, "create udp thread error \n", pSess);
					return GS_FAIL;
				}
			
			}
			//RTP_UdpRecvRTCPData(pSess);
		}
	}
	else
#endif
	{
		
		if (RTP_TRANSPORT_TYPE_TCP == pSess->transportType)
		{

			if (ret_SUCCESS != RTP_TcpStartSendMediaData(pSess))
			{
				TDEBUG(DBG_RTSP, "RTP_TcpStartSendMediaData error \n ");
				pSess->sessStat = RTSP_STATE_STOP;
				RTSP_SendReply(911, 1, "create tcp thread error \n", pSess);
				return ret_FAIL;
			}
		}
		#if 0
		else if(RTP_TRANSPORT_TYPE_UDP ==  pSess->transportType)
		{

			if (ret_SUCCESS != RTP_UdpStartSendMediaData(pSess))
			{
				pSess->sessStat = RTSP_STATE_STOP;
				RTSP_SendReply(911, 1, "create udp thread error \n", pSess);
				return ret_FAIL;
			}
		}
		#endif
		else
		{ 
			pSess->sessStat = RTSP_STATE_STOP;
			
			RTSP_SendReply(911, 1, "create rtspthread error \n", pSess);
		    return ret_FAIL;
		}
	}
    return ret;
}


int RTSP_EventHandle(int event, int stat, RtspSession_t *pSess)
{printf("\033[47;31m   [%s, %d]--------------  \033[0m \n",__func__, __LINE__);
    int ret;
    printf("remoteip=%s,port=%d type=%d, uid=%llu, C->S ===recvmsg: %s \n",pSess->remoteIp,pSess->remotePort,pSess->transportType, pSess->uniqueID, pSess->recvBuf );
    switch(event)
    {
        case RTSP_REQ_METHOD_OPTIONS:
            ret = RTSP_EventHandleOptions(pSess);
            break;

        case RTSP_REQ_METHOD_TEARDOWN:
            ret = RTSP_EventHandleTeardown(pSess);
            break;

        case RTSP_REQ_METHOD_SETUP:
            ret = RTSP_EventHandleSetup(pSess);
            break;

        case RTSP_REQ_METHOD_DESCRIBE:
            ret = RTSP_EventHandleDescribe(pSess);
            break;

        case RTSP_REQ_METHOD_PLAY:
            ret = RTSP_EventHandlePlay(pSess);
            break;
#if 1
        case RTSP_REQ_METHOD_PAUSE:
            ret = RTSP_EventHandlePause(pSess);
            break;

        case RTSP_REQ_METHOD_SET_PARAM:
            ret = RTSP_EventHandleSetParam(pSess);
            break;

		case RTSP_REQ_METHOD_GET_PARAM:
			ret = RTSP_EventHandleGetParam(pSess);
			break;
		
		case RTSP_REQ_METHOD_HEARTBEAT:
			ret = RTSP_EventHandleHeartBeat(pSess);
			break;
#endif
        default:
            ret = RTSP_EventHandleUnknown(pSess);
            break;
    }
    return ret;
}


int RTSP_SessionProcess(RtspSession_t *pSess)
{
    int stat;
	u32 seqNum = 0;
    int opcode;
    int cseq = -1;
    if(pSess == NULL)
    {
        TDEBUG(DBG_RTSP, "pSess == NULL \n");
        return ret_FAIL;
    }

    if(RTSP_PARSE_IS_RESP == RTSP_ResponseMsgCheck(&stat, pSess->recvBuf))
    {
        if(seqNum != pSess->lastSendSeq + 1)
        {
           printf("last send sn is %d != resp seq = %d\n",pSess->lastSendSeq, seqNum);
        }
        opcode = RTSP_MAKE_RESP_CMD(pSess->lastSendReq);
        if(stat > RTSP_BAD_STATUS_BEGIN)
        {
           printf( "response had status = %d. \n", stat);
        }
    }
    else
    {
        opcode = RTSP_GetReq(pSess->recvBuf);
        if(opcode == RTSP_PARSE_INVALID)
        {

			if (pSess->recvBuf[0] == '$' && pSess->recvBuf[1] == 1)
			{
				printf("rtcp msg\n");
				return ret_SUCCESS;
			}
			printf("Bad request line encountered.  Expected 4 valid tokens.  Message discarded. %s\n", pSess->recvBuf);
            return ret_FAIL;
        }
        cseq = RTSP_GetCseq(pSess->recvBuf);
        if((cseq >= 0) && (pSess->lastRecvSeq < cseq) )
        {
            pSess->lastRecvSeq = cseq;
        }
        else
        {
            printf("invalid cseq = %d, lastseq=%d \n", cseq, pSess->lastRecvSeq);
        }
        stat = 0;
    }

    return RTSP_EventHandle(opcode, stat, pSess);
}

void RTSP_SessListAdd(RtspSession_t *pSess)
{
    if(pSess != NULL)
    {
        RTSP_SessListLock();
		pSess->uniqueID = grtspReaduniqueID++;
		
		gRtspVodSvr.rtspSessWorkList.push_back(pSess);
		max_sessnum++;
        RTSP_SessListUnlock();
    }
}

RtspSession_t *RTSP_SessionCreate(int sockFd, char *s8PeerIP,  long int *s8PeerPort, VodSvrType_e vodSvrType)
{
    //??????????????5??????
    if( sockFd <= 0||max_sessnum>256)
    {
        TDEBUG(DBG_RTSP, "sock = %d , error!!!\n", sockFd);
        return NULL;
    }
    RtspSession_t *pSess = new RtspSession_t(sockFd, s8PeerIP, (int*)s8PeerPort, vodSvrType);


    if(pSess == NULL)
    {
        TDEBUG(DBG_RTSP,"new pSess error\n");
        return NULL;
    }
	
    RTSP_GetSessionId(pSess->sessId, 8);
    RTSP_SessListAdd(pSess);
	
	TDEBUG(DBG_RTSP, "remoteip=%s,port=%d add uid=%llu,psess=%p,fd=%d \n",pSess->remoteIp,pSess->remotePort,pSess->uniqueID, pSess, sockFd);
   	TDEBUG(DBG_RTSP, "max_sessnum = %d, psess=%p, uid=%llu\n",max_sessnum, pSess, pSess->uniqueID);
    return pSess;
}



/*create session struct and hande the first request cmd.*/
static int rtsp_rtsp_srv_sess_create(int cliSockFd, char *s8PeerIP,  long int *s8PeerPort, const char *recvBuf, const int recvLen , RtspSvr_t *pRtspSvr)
{printf("\033[47;31m   [%s, %d]--------------  \033[0m \n",__func__, __LINE__);
	int ret = 1;
    RtspSession_t *pSess = RTSP_SessionCreate(cliSockFd, s8PeerIP, s8PeerPort, pRtspSvr->svrType);
    if(pSess == NULL)
    {

		printf("RTSP_SessionCreate error!\n");
        if(cliSockFd > 2)
        {
            sock_close(cliSockFd);
        }
        return -1;
    }
	TDEBUG(DBG_RTSP, "RTSP_SessionCreate success\n");

	bzero(pSess->recvBuf, MAX_RTSP_BUF_LEN);
    memcpy(pSess->recvBuf, recvBuf, recvLen);
    pSess->inSize = recvLen;
    ret = RTSP_SessionProcess(pSess);
    if(ret != ret_SUCCESS)
    {
        TDEBUG(DBG_RTSP, "fist session msg handle, RTSP_SessionProcess() error!\n");
        RTSP_SessionClose(pSess);
        ret = -1;
    }
	else
		pSess->inSize = 0;

    return ret;
}

void RTSP_CloseAllClient(void)
{
    RtspSession_t *pSess = NULL;
    //RtspSvr_t *pRtspSvr = &gRtspVodSvr;
	for(unsigned int i = 0 ; i < gRtspVodSvr.rtspSessWorkList.size(); i++)
	{
		pSess = gRtspVodSvr.rtspSessWorkList[i];
        if(pSess != NULL && !pSess->multicast_sessionrelease)
        {
            //dbg(Dbg, DbgNoPerror, "sess is not null ......\n");
            if(ret_SUCCESS != RTSP_SessionClose(pSess))
            {
                //dbg(Err, DbgNoPerror, "RTSP_SessionClose error. \n");
            }
        }
    }
}


static int rtsp_new_client_req_handle(int srvfd, RtspSvr_t *pRtspSvr)
{
    static char recvBuf[MAX_RTSP_BUF_LEN] = {0};
	char s8PeerIP[64]   = {0};
	long int s8PeerPort = -1; 
	int ret = 0;
    int cliSockFd = rtsp_new_client_req_accept(srvfd, s8PeerIP, &s8PeerPort);
    if(cliSockFd <= 0)
    {
        printf("cliSockFd = %d", cliSockFd);
        return 0;
    }
    memset(recvBuf, 0, MAX_RTSP_BUF_LEN);
    int recvLen = recv(cliSockFd, recvBuf, MAX_RTSP_BUF_LEN, 0);
    if(recvLen <= 0)
    {
        sock_close(cliSockFd);
        printf("rtsp main loop recv new request msg error!\n");
        return 0;
    }
	if (strstr(recvBuf, "\r\n\r\n") == NULL)
	{
		recvBuf[recvLen] = '\0';
		printf(" request msg error fd=%d!,recv=%s\n",cliSockFd,recvBuf);
	}
	//printf("new connect fd=%d, port=%d\n", cliSockFd, s8PeerPort);

    ConType_e conType = RTSP_GetConnectType(recvBuf);
	TDEBUG(DBG_RTSP, "conType test_here \n");
    switch(conType)
    {
        case CON_TYPE_RTSP:
            ret  = rtsp_rtsp_srv_sess_create(cliSockFd, s8PeerIP, &s8PeerPort, recvBuf, recvLen, pRtspSvr);
			printf("rtsp main loop recv new request, RTSP session create (socket = %d)!\n", cliSockFd);
			break;
        /*case CON_TYPE_HTTP:
            ret = rtsp_http_srv_sess_create(cliSockFd, s8PeerIP, &s8PeerPort, recvBuf, recvLen, pRtspSvr);
			printf("rtsp main loop recv new request, HTTP session create (socket = %d)!\n", cliSockFd);
			break;
        case CON_TYPE_UCTRL:
            printf("unimplemented request protocol :%d!\n",conType);
        case CON_TYPE_UNDEFINED:*/
        default:
            printf("undefined request protocol :%d!\n", conType);
            if(cliSockFd > 0)
            {
                sock_close(cliSockFd);
                ret = ret_FAIL;
            }
            break;
    }
    if(ret == ret_FAIL)
    {
        printf("close this connect fd=%d ,and continue accept new client! \n", cliSockFd);
    }
    return ret_SUCCESS;
}

void RTSP_SetAllSockFd(fd_set *sockFds, int *maxFd, RtspSvr_t *pRtspSvr)
{
    RtspSession_t *pSess = NULL;

	for(unsigned int i = 0 ; i < gRtspVodSvr.rtspSessWorkList.size(); i++)
    {
        pSess = gRtspVodSvr.rtspSessWorkList[i];
        if(pSess != NULL )
        {
        	/*当当前播放状态为回放的播放或暂停，则不添加socket到select，由回放发送线程处理*/

		// if(pSess->rtspSessType == RTSP_SESS_PLYBACK
			//	&& (pSess->sessStat == RTSP_STATE_PLAY|| pSess->sessStat == RTSP_STATE_PAUSE)
			//	&& RTP_TRANSPORT_TYPE_TCP ==  pSess->transportType)
				
			//RTSP_DBG_PRINT("add fdset fd=%d, pSess->rtspSessType=%d, pSess->sessStat=%d\n", pSess->rtspSockFd, pSess->rtspSessType ,pSess->sessStat);
			if(pSess->rtspSessType == RTSP_SESS_PLYBACK
				&& (pSess->sessStat == RTSP_STATE_PLAY
				|| pSess->sessStat == RTSP_STATE_PAUSE))
				
        	{
        		continue;
        	}
				
            if(pSess->rtspSockFd > 0)
            {
                if(pSess->isHttpTunnel && pSess->httpTunnelInputSocket > 0)
                {
                    FD_SET(pSess->httpTunnelInputSocket, sockFds);
                    *maxFd = (*maxFd >= pSess->httpTunnelInputSocket) ? *maxFd :
                             pSess->httpTunnelInputSocket;
                }
                else
                {
                    FD_SET(pSess->rtspSockFd, sockFds);
                    *maxFd = (*maxFd >= pSess->rtspSockFd) ? *maxFd :
                             pSess->rtspSockFd;
                }
            }
        }
    }
}

static int proccess_msg(RtspSession_t *pSess)
{printf("\033[47;31m   [%s, %d]--------------  \033[0m \n",__func__, __LINE__);
	int msgLen = 0;
	char *p = pSess->recvBuf;
	char *pstart = NULL;
	short rtcp_body_len;
	if(pSess->inSize < 0 || pSess->inSize > MAX_RTSP_BUF_LEN)
	{
	  TDEBUG(DBG_RTSP, "insize=%d eererror\n", pSess->inSize);
	  return ret_FAIL;
	}
	p[pSess->inSize] = '\0';

	while (pSess->inSize > 0)
	{
		if (p[0] == '$')
		{
			if (pSess->inSize < 4)
				break;
			rtcp_body_len = (short)(((unsigned char)p[2] << 8) + (unsigned char)p[3]);

			msgLen = 4 + rtcp_body_len;
			if (pSess->inSize < msgLen)
				break;
			pSess->inSize -= msgLen;
			p += msgLen;
		}
		else
		{
			p[pSess->inSize] = '\0';
			pstart = strstr(p, "\r\n\r\n");
			if (pstart == NULL)
			{
			    TDEBUG(DBG_RTSP, "do not find rnrn, p=%s,insize=%d", p,pSess->inSize);
				break;
			}
			msgLen = (pstart - p) + 4;
			if (pSess->recvBuf != p)
			{
				memmove(pSess->recvBuf, p, pSess->inSize);
				pSess->recvBuf[pSess->inSize] = '\0';
			}
			
			pSess->inSize -= msgLen;
			p = pSess->recvBuf + msgLen;
			int handleStat = RTSP_SessionProcess(pSess);
			if (handleStat != ret_SUCCESS)
				return handleStat;
			//RTSP_DBG_PRINT("msglen=%d,insize=%d,recv=%s\n", msgLen, pSess->inSize,p);
		}
	}
	
	//RTSP_DBG_PRINT("msglen=%d,insize=%d,recv=%s\n", msgLen, pSess->inSize,p);
	if (pSess->recvBuf != p && pSess->inSize > 0)
		memmove(pSess->recvBuf, p, pSess->inSize);
	return ret_SUCCESS;
}


int RTSP_HTTP_Base64Process(const char* pRecvBuf, char *pBase64Out )
{
    int ret = 0,i = 0;
	if(strlen(pRecvBuf) < MAX_RTSP_BUF_LEN)
	{
	   char *ptmp = (char*)malloc(strlen(pRecvBuf)+1);
	  while(*pRecvBuf)
	  {
	      if(*pRecvBuf == '\r' || *pRecvBuf == '\n' || *pRecvBuf == '\t' ||  *pRecvBuf == ' ')
	      {
	        pRecvBuf++;
		  	continue;
	      }
		  ptmp[i] = *pRecvBuf;
		  pRecvBuf++;
		  i++;
	  }
	  ptmp[i] = '\0';
	  ret = base64_64_to_bits(pBase64Out, ptmp);
	  free(ptmp);
	}
	else
	{
	   TDEBUG(DBG_RTSP, "recvbuf msgtoolong\n");
	   ret = 0;
	}
//    ret = RTSP_Base64Dec(pBase64Out, pRecvBuf, strlen(pRecvBuf));
    return ret;

}


static int rtsp_lst_client_req_handle(int svrSockFd, fd_set *psockFds,RtspSvr_t *pRtspSvr)
{
#if 1
    RtspSession_t *pSess = NULL;
    int handleStat = ret_SUCCESS;
    int cliSockFd = -1;
    int recvLen = 0;
	for(int i = 0 ; i < gRtspVodSvr.rtspSessWorkList.size(); i++)
	{
		pSess = gRtspVodSvr.rtspSessWorkList[i];
        if(pSess != NULL)
        {
            cliSockFd = pSess->rtspSockFd;
            if ( pSess->isHttpTunnel && pSess->httpTunnelInputSocket > 0 )
            {
                cliSockFd = pSess->httpTunnelInputSocket;
            }
            if(cliSockFd <= 0)
            {
                usleep(100*1000);
                continue;
            }
            if(svrSockFd != cliSockFd  && FD_ISSET(cliSockFd, psockFds))
            {

				if(!(pSess->backchannel && pSess->transportType ==  RTP_TRANSPORT_TYPE_TCP && (pSess->sessStat == RTSP_STATE_PLAY || pSess->sessStat == RTSP_STATE_STOP)))
				{
					//RTSP_DBG_PRINT("pSess->backchannel :%d pSess->sessStat=%d\n", pSess->backchannel, pSess->sessStat);
	                handleStat = ret_FAIL;
					if (pSess->inSize >= MAX_RTSP_BUF_LEN - 1)
					{
						pSess->recvBuf[MAX_RTSP_BUF_LEN - 1] = '\0';
						TDEBUG(DBG_RTSP, "------recvmsg toolong ip=%s, port=%d, msg:%s\n", pSess->remoteIp, pSess->remotePort, pSess->recvBuf);
						for (int i = 0; i < 1024; i++)
						{
							TDEBUG(DBG_RTSP, "msg[0]:%x\n",  pSess->recvBuf[i]);
						}
						pSess->inSize = 0;
						memset(pSess->recvBuf, 0, MAX_RTSP_BUF_LEN);
						continue;
					}
					
					
					recvLen = recv(cliSockFd, pSess->recvBuf + pSess->inSize,MAX_RTSP_BUF_LEN - pSess->inSize -1, 0);
	                if(recvLen <= 0)
	                {
						if (errno == EINTR)
							continue;
	                    
						TDEBUG(DBG_RTSP, "recv err close ch %d client ip : [%s] , fd=%d\n",pSess->channel, pSess->remoteIp, cliSockFd);

	                    handleStat = ret_FAIL;
	                    goto __RtspSessprocessErr;
	                }

					if (pSess->isHttpTunnel)
					{
					
					    memset(pSess->tmpRecv,0,sizeof(pSess->tmpRecv));
						pSess->recvBuf[pSess->inSize + recvLen] = '\0';
					//	RTSP_DBG_PRINT(" inseze=%d,retlen=%d, prec=%p,recv:%s\n", pSess->inSize, recvLen,pSess->recvBuf,pSess->recvBuf);
						
						recvLen = RTSP_HTTP_Base64Process(pSess->recvBuf + pSess->inSize, pSess->tmpRecv);

						memcpy(pSess->recvBuf + pSess->inSize, pSess->tmpRecv, recvLen);
					}
					pSess->inSize += recvLen;
					//RTSP_DBG_PRINT("cliSockFd=%d recvLen :%d pSess->inSize=%d zhprecvbuf=%s\n", cliSockFd,recvLen, pSess->inSize,pSess->recvBuf);
				
					//dbg(Info,DbgNoPerror,"pSess->recvBuf zhp = %s \n", pSess->recvBuf);
					handleStat = proccess_msg(pSess);
			}
            __RtspSessprocessErr:
                if(handleStat != ret_SUCCESS)
                {
                    TDEBUG(DBG_RTSP, "close ch %d client ip : [%s] , fd=%d\n",pSess->channel, pSess->remoteIp, cliSockFd);
                    if(cliSockFd > 0)
                    {
                        FD_CLR(cliSockFd, psockFds);
                    }
					//RTSP_SRV_DBG_PRINT("ch client clientnum : [%d] \n",pRtpMulticastUdpInfo[pSess->streamType][0].clientnum);	
					for(int i=0; i<RTP_STREAM_MAX; i++)
					{
						if(pSess->multicast_setup_finish[i])
						{
							//RTSP_SRV_DBG_PRINT("ch %d client multicast_setup_finish : [%d] \n",i,pSess->multicast_setup_finish[i]);	
							pthread_mutex_lock(&g_MulticastUdpMutex);
							if(pRtpMulticastUdpInfo[pSess->streamType][i].clientnum)
								pRtpMulticastUdpInfo[pSess->streamType][i].clientnum -= pSess->multicast_setup_finish[i];
							pthread_mutex_unlock(&g_MulticastUdpMutex);
							pSess->multicast_setup_finish[i] = 0;
						}
					}
					//RTSP_SRV_DBG_PRINT("ch  client clientnum : [%d] \n",pRtpMulticastUdpInfo[pSess->streamType][0].clientnum);
					if(!pSess->multicast_sessionrelease)
                    {
                    	TDEBUG(DBG_RTSP, "ch  client clientnum : [%d] \n",pRtpMulticastUdpInfo[pSess->streamType][0].clientnum);
                    	RTSP_SessionClose(pSess);
					}
					else
					{
						//RTSP_SRV_DBG_PRINT("ch  client clientnum : [%d] \n",pRtpMulticastUdpInfo[pSess->streamType][0].clientnum);
						if(pSess->rtspSockFd > 0)
				        {
				            sock_close(pSess->rtspSockFd);
				            pSess->rtspSockFd = -1;
				        }
					}
                    //RTSP_SessListPrint(pRtspSvr);
                }
            }
        }
    }
    return 0;
#endif
}


void *RTSP_SvrThrFxn(void *pArgs)
{
    RtspSvr_t *pRtspSvr = (RtspSvr_t *)&gRtspVodSvr;
    struct timeval timeout;
    int maxFd = 0;
    int svrSockFd = pRtspSvr->rtspSvrSockFd;//554
    fd_set sockFds;
    int ret = 0;
	
	prctl(PR_SET_NAME, (unsigned long)"RTSP_SvrThrFxn", 0, 0, 0);

    while(gRtspRun)
    {
        RTSP_SessionStatCheck(pRtspSvr);
        FD_ZERO(&sockFds);
        FD_SET(svrSockFd, &sockFds);
		maxFd = svrSockFd;
        RTSP_SetAllSockFd(&sockFds, &maxFd, pRtspSvr);
		
        timeout.tv_sec  = 2;
        timeout.tv_usec = 0;/* 2 s timeout */
        ret = select(maxFd + 1, &sockFds, NULL, NULL, &timeout);
		
        if(ret < 0)
        {
			printf("-------!!!!!rtsp-select erro \n");
            if(errno == EINTR)
            {
                continue;
            }
			printf("select err:%s\n", strerror(errno));
			break;
        }
        else if(ret == ret_SUCCESS)
        {
            continue;
        }
        else
        {
            if(FD_ISSET(svrSockFd, &sockFds))
            {
                ret = rtsp_new_client_req_handle(svrSockFd, pRtspSvr);	 
            }
			rtsp_lst_client_req_handle(svrSockFd, &sockFds, pRtspSvr);
		}
    }
    printf("server exit ... gRtspRun=%d\n",gRtspRun);
    /* close all tcp thread first */
    RTSP_CloseAllClient();
    sock_close(svrSockFd);
    printf("[rtsp] server exit ok \n");
    return NULL;
}



int RTSP_SvrListen(RtspSvr_t *pRtspSvr)
{
	char s8Serv[8];
	snprintf( s8Serv, sizeof(s8Serv), "%d", pRtspSvr->rtspSvrPort );//554

	if((pRtspSvr->rtspSvrSockFd = TcpListen( NULL, s8Serv, NULL )) <= 0)
	{
	    return ret_FAIL;
	}
	TDEBUG(DBG_RTSP, "RTSP_SvrListen listenID:[%d]\n", pRtspSvr->rtspSvrSockFd);
	return ret_SUCCESS;
}

int RTSP_SvrCreate(int port, int chnNum, VodSvrType_e vodSvrType, RtspSvr_t *pRtspSvr)
{
    pRtspSvr->svrType       = vodSvrType;
    pRtspSvr->vodSvrState   = VOD_SVR_STATE_INIT;
    pRtspSvr->rtspSvrPort   = port;
    pRtspSvr->rtspSvrSockFd = 0;
    pthread_mutex_init(&pRtspSvr->rtspListMutex, NULL);
    //pthread_mutex_init(&g_UdpPortMutex, NULL);
	//pthread_mutex_init(&g_MulticastUdpMutex, NULL);
	//memset(pRtpMulticastUdpInfo, 0, sizeof(RtpMulticastUdpInfo_t) * 3 * RTP_STREAM_MAX);
    return ret_SUCCESS;
}

int RTSP_SvrThrStart(pthread_t *pid)
{
    if(ret_SUCCESS != RTSP_SvrListen(&gRtspVodSvr))
    {
		TDEBUG(DBG_RTSP, "RTSP_SvrListen error \n");
        return ret_FAIL;
    }
	TDEBUG(DBG_RTSP, "RTSP_SvrListen SUCCESS\n");

    if(ret_SUCCESS != pthread_create(pid, NULL, RTSP_SvrThrFxn, NULL))
    {
		TDEBUG(DBG_RTSP, "pthread_create error \n");
        return ret_FAIL;
    }
    return ret_SUCCESS;
}

int RTSP_ThrStop()
{
#ifdef DEMO
    printf("RTSP_ThrStop################!!!!!");
#endif
    gRtspRun = 0;
    if(pid != 0)
    {
        pthread_join(pid, NULL);
        //RtspSvr_t *pRtspSvr = &gRtspVodSvr[VOD_SVR_TYPE_RTSP];
        //pRtspSvr->vodSvrState=VOD_SVR_STATE_STOP;
        pid = 0;
        TDEBUG(DBG_RTSP, "-------------!!rtsp serv stop success\n");
        return 0;
    }
    else
    {
        TDEBUG(DBG_RTSP, "!!!!!!-----------rtsp serv stop error\n");
        return -1;
    }
}


int RTSP_ThrStart(RtspServParam *pSer, int MaxChNum)
{
	TDEBUG(DBG_RTSP, "RTSP_ThrStart\n");
	int rtspPort = 0;
	gRtspRun = 1;
	if(pSer->rtspPort == 0)
		rtspPort = 554;
	else
		rtspPort = pSer->rtspPort;
	
	if(ret_SUCCESS != RTSP_SvrCreate(rtspPort, MAX_VOD_CHN,VOD_SVR_TYPE_RTSP, &gRtspVodSvr))
    {
   		TDEBUG(DBG_RTSP, "RTSP_SvrCreate error \n");
        return ret_FAIL;
    }
	TDEBUG(DBG_RTSP, "RTSP_SvrCreate success\n");

	if(ret_SUCCESS != RTSP_SvrThrStart(&pid))
	{
		TDEBUG(DBG_RTSP, "RTSP_SvrThrStart error \n");
	    return ret_FAIL;
	}
	
	return ret_SUCCESS;
}
#if 0
int main()
{
	PassWd_t Pass;
	Pass.channel = 1;
	strcpy(Pass.pwd , "admin");
	strcpy(Pass.uname , "admin");
	
	RtspServParam parm;
	parm.userData = &Pass;
	parm.rtspPort = 11010;
	
	RTSP_ThrStart(&parm, 32);
    return 0;
}
#endif





