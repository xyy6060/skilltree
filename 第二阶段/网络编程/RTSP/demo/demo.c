/******************										***************/
//编译命令：
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
	FILE *fp;		//文件描述符
	unsigned int bufsize;	//缓冲大小
	int frametype;	//帧类型
	unsigned int fsize;		//一帧数据的大小
	unsigned int pts;		//时间戳
	unsigned int relative_pts;
	int frameRate;
	time_t wTime;	//写入时间
	char *buf;		//缓冲指针

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
	
	fseek(rb.fp, 56, SEEK_SET);//跳过文件头
	
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
		
	//读数据
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

		//读取一帧数据放入给定的缓冲区dataBuf中
		if (frameHead.FrameTag != RSFrameTag)
		{
			printf("<%s>:(%d), get frame faile\n",__FUNCTION__,__LINE__);
			//fseek(rb.fp , 56, SEEK_SET);
			return -1;
		}

		//8字节对齐，文件数据为8字节对齐 
		int frameDataSize = ((frameHead.FrameSize + 7)&(~7));
		
		rb.frametype=frameHead.FrameType;
		
		if (frameHead.FrameType == RSAFrame)
		{
			rb.frametype=97;
			//对音频偏移 不读取
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
		
		outfile.open("NoRSFm.264", ios::out | ios::app );//打开 追加写入
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






