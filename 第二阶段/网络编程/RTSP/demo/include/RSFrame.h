#ifndef __RSMedia_h__
#define __RSMedia_h__

/* Video */
#define RSVideoEncodeH264	0x01
#define RSVideoH264FFMPEG	0x02
#define RSVideoEncodeMPEG4	0x03
#define RSVideoEncodeMJPEG	0x04
#define RSVideoEncodeJPEG	0x05
#define RSVideoEncodeMP4	0x06
#define RSVideoEncodeH265	0x07
#define RSVideoH265FFMPEG	0x08

/* Audio */
#define RSAudioEncodeG711A	0x01
#define RSAudioEncodeG711U  0x02
#define G711_A 				RSAudioEncodeG711A
#define G711_U	     		RSAudioEncodeG711U
#define ADPCM_DVI4			0x03
#define G726_16KBPS			0x04
#define G726_24KBPS			0x05
#define G726_32KBPS			0x06
#define G726_40KBPS			0x07
#define AMR                 0x08
#define AMRDTX              0x09
#define AAC                 0x0a
#define AT_10116            0x19  /* 8kbps 10116 rfc1890*/
#define AT_G721             0x1a  /* 8kbps G721  rfc1890 */
#define AT_GSM              0x1b  /* 8kbps Gsm   rfc1890 */
#define AT_LPC              0x1c  /* 8kbps lpc   rfc1890 */
#define AT_G722             0x1e  /* 8kbps G722  rfc1890 */
#define AT_G728             0x1f  /* 8kbps G728  rfc1890 */
#define ADPCM_IMA           0x23

#ifndef _WIN32
typedef struct _GUID {
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[ 8 ];
} GUID;

typedef unsigned long long ULONGLONG;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int UINT;


#endif


//////////////////////////////////////////////////////////////////////////
//RSFrame

//RSVFrameHead.System
#define RSSystemPAL		'P'
#define RSSystemNTSC	'N'

typedef struct
{
	WORD Width;
	WORD Height;
	BYTE FrameRate;
	BYTE System;
    WORD WaterMark : 1;//2013-03-04 10:37:18  
    WORD bRefFrame : 1;
    WORD IFrameNoOffset : 11;/*当前帧相对前一I帧的偏移*/
	WORD bFishEye : 1;
    WORD Reserved : 2;

}RSVideoHead;

typedef struct
{
	UINT SampleRate;
	BYTE Channels;
	BYTE BitsPerSample;
	WORD PackLen;
}RSAudioHead;


typedef struct
{
    BYTE ReportType;
    BYTE Version;
    BYTE Reserved[2];
} RSReportHead;
//RSFrameHead.FrameTag
#define RSFrameTag	*(UINT*)"RSFm"

//RSFrameHead.FrameType
#define RSIFrame	'I'
#define RSPFrame	'P'
#define RSVirIFrame	'V'
#define RSAFrame	'A'
#define RSEndFrame   'E'
#define RSReposFrame 'R'
#define RSThnFrame 'T'

//RSFrameHead.EncodeType
//Video
#define RSVideoEncodeH264	0x01
//Audio
#define RSAudioEncodeG711A	0x01

//use ffmpeg to decode
#define RSVideoH264FFMPEG   0x02

typedef struct
{
	UINT FrameTag;

	BYTE FrameType;
	BYTE EncodeType;
	BYTE Channel:7;
	BYTE Encrypted:1; //Encrypt Flag. 0:no Encrypt; 1:Encrypted.
    BYTE bHide:1;
	BYTE bVirtualIEnable:1; //鏄惁缂栬櫄鎷烮甯?
    BYTE StreamType:1;	   //0:涓荤爜娴? 1:瀛愮爜娴?
	BYTE PirAlarm:1;			//0:娌℃湁瑙﹀彂鎶ヨ1:瑙﹀彂pir鎶ヨ
	BYTE MotionAlarm:1; 	//0:娌℃湁瑙﹀彂鎶ヨ1:瑙﹀彂motion鎶ヨ
	BYTE HDAlarm:1;         //0:娌℃湁瑙﹀彂鎶ヨ 1:瑙﹀彂浜哄舰鎶ヨ
	BYTE FDAlarm:1;         //0:娌℃湁瑙﹀彂鎶ヨ 1:瑙﹀彂浜鸿劯鎶ヨ
	BYTE Reserved:1;

	UINT FrameNo;
	UINT FrameSize;
	ULONGLONG Time; //(microseconds) Time/1000,000 = the number of seconds after January 1, 1970 UTC
	ULONGLONG Pts;

	union
	{
		RSVideoHead VideoHead;
		RSAudioHead AudioHead;
        RSReportHead ReportHead;
    };
}RSFrameHead;


//////////////////////////////////////////////////////////////////////////
//RSInfo
#define RSPuInfoTag *(unsigned int*)"RSI"
#define RSPuInfoTypeGPS 'G'
#define RSPuInfoTypeRepos 'R'
#define RSPuInfoTypeEnd 'E'

typedef struct __PuInfoHead_t
{
	unsigned int Tag : 24;
	unsigned int Type : 8;
	unsigned int InfoSize;
	unsigned long long Time; //(microseconds) Time/1000,000 = the number of seconds after January 1, 1970 UTC
}PuInfoHead_t;

//pu_info_head_t.type = RSPuInfoTypeGPS, pu_info_head_t.info_size = sizeof(gps_info_t)
typedef struct __GpsInfo_t
{
	double Longitude;
	double Latitude;
	float Speed;
	float SensorSpeed;          //传感器速度，作为可选模块，可能该值无效
	float Azimuth;    			//方向角 正北为0，顺时针方向 0~360
	int Signal3G;               //3G信号
	unsigned int TotalMileage;  //  总行程
	int Temperature;
	char LatitudeDir;       	// 南北半球 'N' or 'S'
	char LongitudeDir;      	// 东西半球 'E' or 'W'
	char Rectified;             // 是否已经检验
	char Status;   				//状态, 'A' 为有效位置, 'V'为非有效接收警告，即当前天线视野上方的卫星个数少于3颗。
	unsigned char IgnitionFlag; //点火标志位。1:点火，0：熄火
	char Reserved[3];
	double LongitudeOffset;
	double LatitudeOffset;
}GpsInfo_t;


//GPS信息体
typedef struct
{
	PuInfoHead_t Head;
	GpsInfo_t Body;
}RSGpsInfoPacket_t;


//////////////////////////////////////////////////////////////////////////
//RSFile

//File content outline
/*
	RSFileHead
	RSFileHeadInfo(extensible)

	Frame ... Frame

	//TailInfo

	RSFileListHead
	(optional list head info)
	RSFileFrameIndex(extensible) ... RSFileFrameIndex(extensible)

	(extensible)
*/

#define RSFileDataAlign	8 //File data alignment

#ifndef C_ASSERT
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
#endif

#define CheckFileDataAlign(type) C_ASSERT(sizeof(type) % RSFileDataAlign == 0)

//RSFileHead.Tag
#define TEKVFileTag	*(ULONGLONG*)"TEKVFile" //Identify TEKVISION media file
#define RSFileTag	*(ULONGLONG*)"ALRSFile" //Identify ALRS media file

//RSFileHead.Status
#define RSFileFull			*(UINT*)"Full"
#define RSFileWritingData	*(UINT*)"Data"


typedef struct
{
	ULONGLONG Tag; //RSFileTag
	UINT HeadInfoSize; //sizeof(RSFileHeadInfo)
	UINT Status; //RSFileFull ...
	ULONGLONG TailInfoOffset; //Tail info offset from the beginning of the file
	GUID Guid; //Unique identifier for this file
}RSFileHead;
CheckFileDataAlign(RSFileHead);

typedef struct
{
	ULONGLONG BeginTime; //Equal to RSFrameHead.Time
	ULONGLONG EndTime; //Equal to RSFrameHead.Time
}RSFileHeadInfo;
CheckFileDataAlign(RSFileHeadInfo);

#define RSFileListTagPrefix			"List"
#define RSFileListTagPrefixValue	*(UINT*)RSFileListTagPrefix
#define RSFileIndexListTag		*(ULONGLONG*)RSFileListTagPrefix##"Idx"
#define RSFileIndexListTagEx	*(ULONGLONG*)"ListIdx"

typedef struct
{
	ULONGLONG Tag; //RSFileIndexListTag ...
	WORD ListHeadInfoSize; //Extra list head info size, 0 for RSFileIndexListTag
	WORD ItemSize; //One subsequent item size, sizeof(RSFileFrameIndex) for RSFileIndexListTag
	UINT ListSize; //Extra list head info size plus all subsequent item size
}RSFileListHead;
CheckFileDataAlign(RSFileListHead);

typedef struct
{
	ULONGLONG Time; //Equal to RSFrameHead.Time
	ULONGLONG Offset; //Frame offset from the beginning of the file
}RSFileFrameIndex;
CheckFileDataAlign(RSFileFrameIndex);

#endif
