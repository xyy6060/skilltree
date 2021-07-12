/******************										***************/
//�������
//g++ demo.c -o ReadData -I ./include  -L. -lrtsp -ldl -lpthread
/******************										***************/

#include <fstream>
#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "rtsp_server_intf.h"
#include "RSFrame.h"

#define H264_FILE_NAME "testaudio.264"
#define H264_SIZE 		(16*1024)

typedef struct {
	FILE *fp;		//�ļ�������
	unsigned int bufsize;	//�����С
	int frametype;	//֡����
	unsigned int fsize;		//һ֡���ݵĴ�С
	unsigned int pts;		//ʱ���
	unsigned int relative_pts;
	int frameRate;
	time_t wTime;	//д��ʱ��
	char *buf;		//����ָ��

	unsigned int width;		
	unsigned int height;
	void *phead;
}Demo_Buf_t;

typedef struct PassWd_s
{
	char uname[32];
	char pwd  [32];
	int channel;
}PassWd_t;

int main()
{
	unsigned int readNum = 0, headsize = 0;
	Demo_Buf_t rb;
	//char fbuf[4] = {0};
	//AppReadStreamFunc pStream;
	rb.fp = fopen(H264_FILE_NAME, "r");
	if(rb.fp < 0)
	{
		printf("failed to open %s\n", H264_FILE_NAME);
		return -1;
	}

	#if 0
	rb.buf=(char*)malloc(H264_SIZE);
	if(rb.buf==NULL)
	{
		printf("malloc error:%s\n",strerror(errno));
		fclose(rb.fp);
		return -1;
	}
	rb.bufsize=H264_SIZE;
	#endif
	
	fseek(rb.fp, 56, SEEK_SET);//�����ļ�ͷ
	
	//RTSP_GetAppReadStreamCb(&pStream);
	PassWd_t Pass;
	Pass.channel = 1;
	strcpy(Pass.pwd , "111111");
	strcpy(Pass.uname , "admin");
	
	RtspServParam parm;
	parm.rtspPort = 8554;
	parm.pDevIntf = NULL;
	parm.SimulatNum = 0;
	parm.userData = &Pass;
	parm.auth_cb = 0;

	PassWd_t auth;
	strcpy(auth.pwd, "");
	strcpy(auth.uname, "");
	parm.userData = &auth;
	
	parm.AuthFlag = 0;
	strcpy(parm.rtspUserName,"admin");
    memset(parm.rtspUserPwd,0,16);
	RTSP_ThrStart(&parm, 32);
	sleep(2);
	
	ofstream outfile;
		
	//������
	RSFrameHead frameHead;
	while(1)
	{
		memset(&frameHead, 0, sizeof(RSFrameHead));
		rb.buf = NULL;
		rb.bufsize = 0;
		readNum = fread(&frameHead, 1, sizeof(RSFrameHead), rb.fp);
		
		//memcpy(&fbuf, &frameHead.FrameTag, sizeof(fbuf));
		//printf("FrameTag:[%s], FrameType:[%d] EncodeType:[%d] \n", fbuf, frameHead.FrameType, frameHead.EncodeType);
		
		//printf("FrameType:[%c] Width:[%d] Height:[%d]\n",frameHead.FrameType, frameHead.VideoHead.Width, frameHead.VideoHead.Height);
		//usleep(10*1000);
		
		headsize = sizeof(RSFrameHead);
		if (readNum != sizeof(RSFrameHead))
		{
			if(feof(rb.fp))
			{
				return -1;
				//fseek(rb.fp, 56, SEEK_SET);
				continue;
			}
			printf("---Read frame head error!, readnum = %d, sizeof(RSFrameHead) = %d\n",readNum, headsize);
			return -1;
		}

		//��ȡһ֡���ݷ�������Ļ�����dataBuf��
		if (frameHead.FrameTag != RSFrameTag)
		{
			printf("<%s>:(%d), get frame faile\n",__FUNCTION__,__LINE__);
			//fseek(rb.fp , 56, SEEK_SET);
			return -1;
		}

		//8�ֽڶ��룬�ļ�����Ϊ8�ֽڶ��� 
		int frameDataSize = ((frameHead.FrameSize + 7)&(~7));
		
		rb.frametype=frameHead.FrameType;
		
		if (frameHead.FrameType == RSAFrame)
		{
			rb.frametype=97;
			//����Ƶƫ�� ����ȡ
			unsigned char video[4]={0};
			fread(video,1,4,rb.fp);		
			frameDataSize -= 4;
			frameHead.FrameSize -= 4;
		}
		else
			usleep(30*1000);
		
		if (rb.buf == NULL || rb.bufsize < frameDataSize)
		{
			if (rb.buf != NULL)
			{
				free(rb.buf);
			}
			rb.buf = (char *)malloc(frameDataSize+sizeof(RSFrameHead));
			rb.bufsize =frameDataSize+ sizeof(RSFrameHead);
		}
		bzero(rb.buf,rb.bufsize);
		memcpy(rb.buf, (void*)&frameHead, sizeof(RSFrameHead));
		
		readNum = fread(rb.buf + sizeof(RSFrameHead), 1, frameDataSize, rb.fp);
		if (readNum != frameDataSize)
		{
			printf("Frame data has some problem readNum[%d] frameDataSize[%d]!\n", readNum, frameDataSize);
			return -1;
			//fseek(rb.fp , 56, SEEK_SET);
		}
		
		outfile.open("NoRSFm.264", ios::out | ios::app );//�� ׷��д��
		//outfile << rb.buf << endl;
		outfile.write(rb.buf, rb.bufsize);
		outfile.close();
		
		rb.fsize=frameHead.FrameSize;
		printf("demo_con_in_pStream  line=%d  bufsize:[%d] Frame type:[%c] \n",__LINE__, rb.bufsize , frameHead.FrameType);
		RTSP_PushStream(rb.buf, rb.bufsize);
		usleep(10*1000);
		//break;
	}
    return 0;
}






