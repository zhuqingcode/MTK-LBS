#ifndef _SCHEDUL_SCREEN_H
#define _SCHEDUL_SCREEN_H

#include "oa_gps.h"
#include "oa_type.h"
//外设类型
#define SCREEN_HD5	1  //D5调度屏
#if (SCREEN_HD5==0)
	#define SCREEN_N990	0 //导航屏N990
#endif

/*硬件相关设置*/
/*--------N990协议--------*/
#define SchedulN990_Baud 9600 //波特率

#if SCREEN_N990>0
	#define Schedul_Baud 9600 //波特率
/*--------N990协议--------*/
//屏→终端
	#define N990_ONEBYTE_CMD	  0x01 //单字节指令
		#define HEARTBEAT_ACK_CMD 0x01 //心跳应答
		#define CALL_HANGON_CMD 0x02 //接电话
		#define CALL_HANGOFF_CMD 0x03 //挂机
		#define GET_SMS_FORMAT_CMD 0x04 //查询短信格式
		#define CALL_CENTER_CMD 0x06 //呼叫中心
		#define SETSPK_UP_CMD	 0x09 //加音量
		#define SETSPK_DOWN_CMD	 0x0A //减音量
		#define GET_SPKVAL_CMD	 0xB0 //查询音量
	#define N990_SENDSMS_CMD	 0xA4 //发送短信
	#define CALL_CAR_RSP_CMD   0x26 //抢单
	#define PICKUP_PASSEGER_CMD 0x27 //上车
	#define PICKUP_PASSEGER_FAILED_CMD 0x28 //取消订单，未接到人
	#define REACH_DESTI_CMD     0x29 //下车
	
	//终端→屏
		#define HEART_BEAT_CMD 0x0D
		#define CALL_CAR_CMD 0x23      //开始电召
		#define CALL_CAR_INFO_CMD 0x24 //详细电召
		#define CALL_CAR_FAIL_CMD 0x25 //抢单失败
		#define TEXT_INFO_CMD	0xA5   //文本抢答
		#define TEXT_INFO_CMD	0xA5   //文本抢答
		#define N990_RCVSMS_CMD	0xA6   //收短信
			
	typedef struct _sms_data_result_//短信内容
	{
		u8 TelNum[21];
		u8 SMSDat[150];
		u8 SMSLen;
	}STRUCT_SMS_DATA;
	typedef union _result_data_
	{
		u8 SUB_CMD_TYPE; //单字节指令的sub命令字
		u8 IDLen;
		u8 ID[10];
		u8 failmsgLen;
		u8 FailedMsg[30]; //退回电召原因
		STRUCT_SMS_DATA SMS_Data;//接收到的短信数据
	//	u8 value;   //单字节结果,如音量,设置的开关,单字节参数值等
	//	u16 value16;	 //双字节结果
	//	u8 telnum[33];//号码参数
	//	STRUCT_IP_PORT IP_Port;//设置的IP 地址和端口号 
	//	#if DRIVE_SIGN_EN
	//	STRUCT_DRIVER_INF driver_inf;//司机登录数据
	//	#endif
	//	#if CUSTOM_CMD
	//	STRUCT_JTT_BUF JTT_MSG	;//打包jtt808协议
	//	#endif
	//	STRUCT_UP_DATA_PARA UP_DATA_PARA;
	//	STRUCT_UP_DATA UP_DATA_STORE;
	//#if PROTCL_HX54
	//	STRUCT_STATION_DATA STATION_DATA;//站点信息
	//#endif
	}SScrn_result_data;
	
	typedef struct _SScrn_result_
	{
	 u8 CMD_TYPE;  //当前处理的命令类型
	 SScrn_result_data result;      //结果存储
	}STRUCT_SScrn_Result;

	typedef enum
	{
		TEL_NUM_ENUM=0,
		SMS_DAT_ENUM
	}SMS_DATA_ENUM;
#else
	#define Schedul_Baud 19200

/*--------D5协议--------*/
//调度屏→主机命令字
	#define CENTER_SMS_CMD  0x01

	typedef struct _sms_data_result_//短信内容
	{
		u8 TelNum[33];
		u8 SMSDat[150];
		u8 SMSLen;
	}STRUCT_SMS_DATA;

	typedef enum
	{
		TEL_NUM_ENUM=0,
		SMS_DAT_ENUM
	}SMS_DATA_ENUM;
#endif
/*设备配置*/
#define HAND_DEVICE_OR_SCRN	 1 /*配置为调度屏或者手柄,0为手柄1为调度屏*/

/*程序配置*/
#define CUSTOM_CMD 1//允许自定义命令字
#define SSCRN_PRINT_EN 1//调试打印开关
#define DRIVE_SIGN_EN 1 //允许司机刷卡登录

/*队列函数设置*/
#define EN_QUEUE_DELDATA 1

//#if SSCRN_PRINT_EN
//#define SSCRN_Print(arg...) Trace(PrintDevDebug,##arg) //peijl_test
//#else
//#define SSCRN_Print(arg...)
//#endif

#define Normal_Ack	0x01
#define Operate_Ack	0x02
/*--------D5协议--------*/
//调度屏→主机命令字
#define UNICODE_SMS_CMD 0x02
#define SET_UDP_CMD		0x03
#define SET_LOCAL_TEL_CMD	0x04
#define CALL_DIALUP_CMD		0x07
#define CALL_HANGOFF_CMD	0x08
#define CALL_HANGON_CMD		0x0E
#define CALL_SPKSWITCH_CMD	0x0F
#define ACK_CMD			    0x0D
#define RETURN_CMD			0XB1
#define SET_SPK_CMD		0x16
#define SET_MIC_CMD		0x17
#define DRIVER_SIGN_CMD	0x30
#define SET_TCP_CMD		0x35
//#define COM_UPLOAD_REQACK_CMD 0xC2
//#define COM_UPLOAD_DATAACK_CMD 0xC3
#if CUSTOM_CMD
#define COM_JTT808_CMD 0xC5
#define COM_JTT808_CMD_BACK 0x95
#endif
/*升级指令  PC下发*/
#define UPDATA_REQ_CMD  0x50   //请求升级
#define UPDATA_PARA_CMD  0x51  //下发升级包参数
#define UPDATA_DATA_CMD  0x52  //下发升级内容
/*升级指令 END*/
//#if PROTCL_HX54
	/*站点数据导入 PC下发*/
	#define STATION_DOWNLOAD_CMD  0x53
	/*站点下载启动命令 */
	#define STATION_DOWNLOAD_START_CMD	0x54
//#endif

/*学生刷卡短信模板下发指令*/
#define SCR_SMS_DOWNLOAD_CMD	0x55


//主机→ 调度屏命令字
#define UP_STATUS_CMD	0XA0	//状态上报指令码
#define HOST_CenSMS_CMD	0xA1	//主机中心短信
#define CALL_IN_CMD		0xA5	//来电通知
#define HAND_OFF_CMD	0xA6	//挂话通知
#define HAND_ON_CMD		0xA7	//接电话通知
#define TELSMS_PDU_CMD	0xAF	//PDU短信
#define TELSMS_TXT_CMD	0xB0	//txt短信
#define GROUP_NUM_SEND_CMD	0xB4	//下载集团号码
#define GROUP_NUM_CLR_CMD	0xB5	//清空集团号码
#define SMS_SEND_OK_CMD	0xA4	//txt短信

#define SMS_SEND_FAILD_CMD	0xA3	//txt短信
/*升级指令  回复PC*/
#define UPDATA_VERSION_UP_CMD  0xd0	  //上传当前版本
#define UPDATA_REQ_DATA_CMD    0xd1	  // 请求一包内容
/*升级指令 END*/
//#if PROTCL_HX54
	/*站点数据导入 PC下发*/
	#define STATION_DOWNLOAD_RSP_CMD  0Xd2
	/*站点数据导入开始命令应答 */
	#define STATION_DOWNLOAD_START_RSP_CMD  0Xd4
//#endif
/*学生刷卡短信模板下发指令*/
#define SCR_SMS_DOWNLOAD_RSP_CMD	0xd5

/*其他返回结果*/
#define SCCRN_TIME_OUT 0xFC	   //调度屏等待超时
#define SCCRN_ERROR_RETURN 0xFD	//错误
#define PARA_ERROR_CMD	0xFE //判断自定义命令字参数错误
#define SCCRN_UNEXP_CMD	0xFF //收到未做判定的命令字



#if DRIVE_SIGN_EN
typedef enum
{
	SIGN_IN=0,
	SIGN_OUT
}DRIVER_SIGN_ENUM;
typedef struct _Driver_Info //司机登录结构体
{
	DRIVER_SIGN_ENUM SignFlag;	
	u16 DriverID;
} STRUCT_DRIVER_INF;
typedef enum
{
	SIGN_FLAG_ENUM=0,
	DRIVER_ID_ENUM
}DRIVER_INF_ENUM;
#endif
typedef struct _ip_port_result_//设置IP和PORT
{
	u8 Ip[16];
	u16 port;
}STRUCT_IP_PORT;
typedef enum
{
	IP_ENUM=0,
	PORT_ENUM
}IP_PORT_ENUM;


typedef struct _up_data_para_//升级数据参数, 包数 总长
{
	u32 PacketTotalNum;
	u32 DataSize;
}STRUCT_UP_DATA_PARA;

typedef enum
{
	PACKETS_ENUM=0,
	DATA_SIZE_ENUM
}UP_DATA_PARA_ENUM;


typedef struct _up_data_//升级数据内容
{
	u8 datalen;
	u8 UP_DATA[250];
}STRUCT_UP_DATA;

typedef struct //站点数据内容
{
	u8 ROAD_NUM[4];	  //4字节线路号
	u8 STA_DIRECTION; //运行方向
	u8 STA_TOTAL_NUM; //站点总数
	u8 ROAD_VER;	  //版本
	u8 STA_DATALEN;	  //站点数据长度	
	u8 STA_DATA[220]; //STA_NUM最大10,10*STAT_INFO_SIZE
}STRUCT_STATION_DATA;

typedef struct //站点数据内容
{
	u8 STATE;  //模板信息对应序号
	u8 F_OR_A; //模板信息前后缀
	u8 SMS_LEN;//模板信息内容长度
	u8 SMS_DATA[128]; //模板数据
}STRUCT_SCR_SMS_TEMPLET;

#if PROTCL_HX54
typedef enum
{
	ROAD_NUM_ENUM=0,
	DIRECTION_ENUM,
	TOTAL_NUM_ENUM,
	NUM_ENUM,
	TYPE_ENUM,
	LAT_ENUM,
	LON_ENUM
}STATION_DATA_ENUM;
#endif
#if CUSTOM_CMD
typedef struct _JTT_808_BUF_//打包808协议结构体
{
	u8 datalen;
	u8 databuf[250];
}STRUCT_JTT_BUF;
typedef enum
{
	JTT_DATA_LEN_ENUM=0,
	JTT_DATA_DAT_ENUM
}JTT_808_BUF_ENUM;
#endif

typedef struct _HOST_STATE_
{
	u8 HostType;/* 主机类型 位b0：1 GPRS连通、0 GPRS没连通
									位b2位b1主机类型：00  GPRS型、01 GSM/GPRS型、10其它
									位b6位b5位b4网络类型： 000 网络搜索中、001 中国移动、010 中国联通、110 需解析原字符
									位b7 1 报警*/
	u8 OverSpeed;//超速限制
	u8 CSQ;		 //信号强度 范围:0x00-0x1f
	u8 ACC_State; //ACC状态: 0:已点火 1未点火
	u8 DriveState;	//看车状态
	u8 TCP_UDP;		//'1' 表示当前为TCP连接，'0'表示UDP连接
}STRUCT_HOST_STATE;

#if SCREEN_HD5>0
	typedef union _result_data_
	{
		STRUCT_SMS_DATA SMS_Data;//接收到的短信数据
		u8 value;   //单字节结果,如音量,设置的开关,单字节参数值等
		u16 value16;	 //双字节结果
		u8 telnum[33];//号码参数
		STRUCT_IP_PORT IP_Port;//设置的IP 地址和端口号 
		#if DRIVE_SIGN_EN
		STRUCT_DRIVER_INF driver_inf;//司机登录数据
		#endif
		#if CUSTOM_CMD
		STRUCT_JTT_BUF JTT_MSG	;//打包jtt808协议
		#endif
		STRUCT_UP_DATA_PARA UP_DATA_PARA;
		STRUCT_UP_DATA UP_DATA_STORE;
		STRUCT_STATION_DATA STATION_DATA;//站点信息
		STRUCT_SCR_SMS_TEMPLET SCR_SMS_TEMPLET;
	}SScrn_result_data;
	typedef struct _SScrn_result_
	{
	 u8 CMD_TYPE;  //当前处理的命令类型
	 SScrn_result_data result;      //结果存储
	}STRUCT_SScrn_Result;
#endif

#if 0
/*函数声明,供外部调用*/
void SScrn_HW_Init(USART_TypeDef *COM);//硬件相关初始化
void SScrn_SW_Init(void);//软件相关初始化
u8 SScrn_DataAnaly(void);  /*调度屏数据解析*/
u8 SScrn_GetResult(u8 *Buf,u8 BufSize,u8 *ReadSize,u8 Cmd,u8 SubField);/*提取从调度屏发来的数据*/
u8 SScrn_Result_Send(u8 result); /*发送处理结果*/
u8 SScrn_CmdResult_Send(u8 Cmd,u8 Ackmode,u8 result);
u8 SScrn_CenterSMS_Send(u8* Str,u16 len);/*向调度屏发送中心短信*/
u8 SScrn_CallIn_Send(u8 *telnum,u8 tellen);/*向调度屏发送有电话打入,上传号码*/
u8 SScrn_HandOff_Send(void);/*向调度屏发送 电话挂断(网络侧)*/
u8 SScrn_HandOn_Send(void);/*向调度屏发送 电话被接听(网络侧)*/
//u8 SScrn_UniSMS_Send(u8 *Tel,u8 Telen,u8 *Time,u8* SMSStr,u8 SMSlen);/*向调度屏发送 unicode格式短信*/
//u8 SScrn_AscSMS_Send(u8 *Tel,u8 Telen,u8 *Time,u8* SMSStr,u8 SMSlen);/*向调度屏发送TXT短信*/
u8 SScrn_SMS_Send(u8 cmd,u8 *Tel,u8 Tellen,u8 *Time,u8 *SMS,u8 smslen);//短信发送
u8 SScrn_SMSResult_Send(u8 result);	/*向调度屏发送短信发送结果*/
u8 SScrn_GroupNum_Send(u8* Name,u8 NameLen,u8* TelNum,u8 TelLen);/*集团号码下载*/
u8 SScrn_GroupNum_Clr(void);/*清空集团号码*/
u8 SScrn_Status_Send(STRUCT_RMC *pGpsInfo,STRUCT_HOST_STATE * Host_State);/*向调度屏发送状态*/

u8 SScrn_Version_Send(u8* VersionStr,u8 StrLen);/*向PC发送当前版本号*/
u8 SScrn_Data_Req(u32 PacketNum); /*请求数据*/
#endif
#if PROTCL_HX54
u8 SScrn_StationData_Rsq(void);
u8 SScrn_GetStationData(u8 *RoadNum,u8 *Direction,u8 * total_num,u8 *sta_num,u8 *sta_type,FP32 * LAT,FP32 *LON);
#endif
#endif
