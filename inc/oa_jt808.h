/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  njtalent Co., Ltd.
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   oa_jt808.h
 *
 * Project:
 * --------
 *   MTK-LBS project
 *
 * Description:
 * ------------
 *   This Module declares OPEN AT's api.
 *
 * Author:
 * -------
 *   zhuqing.
 *
 ****************************************************************************/
#ifndef __OA_JT808_H__
#define __OA_JT808_H__
#include "oa_type.h"
#include "oa_dev_params.h"

#define JT808_FLAG 0x7E
#define TELMAXLEN  32
#define MAXPPPUSERLEN 127 /*最大PPP拨号用户名字符长度*/
#define MAXPPPPWDLEN 127  /*最大PPP拨号密码字符长度*/




//外设参数类型
typedef enum _UPDATE_MODE
{
	UpdateFlash,
	UpdateOnly
}UpdateModeEnum;
/*-----------------------------------------------------------------------------------
*808协议相关定义
-----------------------------------------------------------------------------------*/
#define MAX_MSG_LEN 1024
#define MAX_MSGBODY_LEN 900 //考虑到字符转义故<1024,最大包长，长消息分包时用

#define AUTHENMAXLEN	100		//鉴权码最大长度定义
#define CarIDMAXLEN		15		//车牌号最长
typedef enum
{
	ActionOK=0,	//成功
	ParaError, 
	//其他
	ActionError,
	DevError,
	PPPLinkError,//拨号失败
	PPPLinkOwned,//被占用
	UDPLinkError,
	UDPLinkOK,
	ServregErr,
	ServregOK,
	ServregoutErr,
	ServOK,
	ServErr,
	UpgradNewver, //已是最新升级版本
	ComUpgrad, //com升级中.com升级优先在线升级
	NoSMS,
	UnDefinedSms,
	UnknownError,
}HXSeverLinkRetrun;
//终端与监控平台通信方式，对应eNet
typedef enum
{
	eNET_TCP=0,
	eNET_UDP=1,
	eNET_SMS=2
}NET_TYPE;
#if 0
/*------P808图像分辨率-------------------------------------------------------------*/
#define PROTCL_IMAGRES320_240	0x01	//320*240
#define PROTCL_IMAGRES640_480	0x02	//640*480,default
#define PROTCL_IMAGRES800_600	0x03	//800*600
#define PROTCL_IMAGRES1024_768	0x04	//1024*768
#define PROTCL_IMAGRES176_144	0x05	//176*144
#define PROTCL_IMAGRES352_288	0x06	//352*288
#define PROTCL_IMAGRES704_288	0x07	//704*288
#define PROTCL_IMAGRES704_576	0x08	//704*576
#endif
//位置信息上报方案枚举 需要确认
#define ACC_Rpttype			0
#define ACCLogSta_Rpttype	1
//位置信息上报策略枚举
#define Time_Stratgy		0
#define Diatanc_Stratgy		1
#define TimeDiatanc_Stratgy	2
/*------------位置信息汇报的标志位和状态位定义--------------------------------------*/
/*------------位置信息汇报的标志位和状态位定义--------------------------------------*/
typedef enum{
	StaAlarm0=0,   //对应32b报警标志
	StaSector1,	   //对应32b状态位
	StatusNum
}STA_ALARM;
//位置信息汇报消息中报警标志位定义
#define ALARM_EMERGENCY_k		(1<<0)		//紧急报警，触动报警开关后出发，收到应答后解除
#define ALARM_OVER_SPEED 		(1<<1)		//超速报警，标志维持至报警条件解除
#define ALARM_DRIVE_TIRED		(1<<2)		//疲劳驾驶，标志维持至报警条件解除
#define ALARM_PRE_k				(1<<3)		//预警，
#define ALARM_GNSS_ERR			(1<<4)		//GNSS模块发生故障
#define ALARM_GNSS_ANTENNA		(1<<5)		//GNSS模块天线未接或被剪断
#define ALARM_GNSS_SHORT_CIRCUIT (1<<6)		//GNSS模块天线短路
#define ALARM_POWER_UNDERVOLTAGE (1<<7)		//终端主电源欠压
#define ALARM_POWER_BROWN		(1<<8)		//终端主电源掉电
#define ALARM_DISPLAY_ERR		(1<<9)		//终端LCD 或显示器故障
#define ALARM_TTS_ERR			(1<<10)		//TIS模块故障
#define ALARM_CAMERA_ERR 		(1<<11)		//摄像头故障
#define ALARM_DRIVE_OVERTIME 	(1<<18)		//当天累计驾驶超时
#define ALARM_OVERTIME_PARKING	(1<<19)		//超时停车
#define ALARM_ENTER_AREA		(1<<20)		//进出区域
#define ALARM_TOUCH_LINE_k		(1<<21)		//进出路线
#define ALARM_DRIVE_SHORT_LONG_k (1<<22)	//路段行驶时间不足/过长
#define ALARM_DRIVE_DEVIATE		(1<<23)		//路线偏离报警
#define ALARM_VSS_ERR			(1<<24)		//车辆VSS故障 ？
#define ALARM_OIL_ERR			(1<<25)		//车辆油量异常
#define ALARM_STEAL				(1<<26)		//车辆被盗，通过车辆防盗器
#define ALARM_START_ILLEGAL_k	(1<<27)		//车辆非法点火
#define ALARM_MOVE_ILLEGAL_k 	(1<<28)		//车辆非法移动
#define ALARM_ROLLOVER			(1<<29)		//车辆侧翻报警								
//位置信息汇报消息中状态位定义
#define STA_ACC_ON				(1<<0)		//ACC 0关，1开
#define STA_GPS_FIXED			(1<<1)		//0:未定位，1:定位
#define STA_LAT_SOUTH 			(1<<2)		//0北纬，1南纬
#define STA_LON_WEAST			(1<<3)		//0东经，1西京
#define STA_IN_BUSINESS			(1<<4)		//0运营状态，1运营状态
#define STA_GPS_ENCRYPT			(1<<5)		//0经纬度未经保密插件加密，1加密
#define STA_OIL_BREAK			(1<<10)		//0车辆油路正常，1车辆油路断开
#define STA_CIRCUIT_CUT			(1<<11)		//0车辆电路正常，1车辆电路断开
#define STA_CAR_LOCK			(1<<12)		//0车门解锁，1车门加锁
//#define STA_CLKSENSOR_WHEEL		(1<<6)	//
#define STA_SENSOR_FULLLOAD  	(1<<31)		//卸料传感器 0卸料，1装载
//判断报警是否有效
#define IS_Valid_ALARM_FLAG(AlarmType)	(((AlarmType) == ALARM_EMERGENCY_k)|| \
										 ((AlarmType) == ALARM_OVER_SPEED)||\
										 ((AlarmType) == ALARM_DRIVE_TIRED)|| \
										 ((AlarmType) == ALARM_PRE_k)|| \
										 ((AlarmType) == ALARM_GNSS_ERR)|| \
										 ((AlarmType) == ALARM_GNSS_ANTENNA)|| \
										 ((AlarmType) == ALARM_GNSS_SHORT_CIRCUIT)|| \
										 ((AlarmType) == ALARM_POWER_UNDERVOLTAGE)|| \
										 ((AlarmType) == ALARM_POWER_BROWN)|| \
										 ((AlarmType) == ALARM_DISPLAY_ERR)|| \
										 ((AlarmType) == ALARM_TTS_ERR)|| \
										 ((AlarmType) == ALARM_CAMERA_ERR)|| \
										 ((AlarmType) == ALARM_DRIVE_OVERTIME)|| \
										 ((AlarmType) == ALARM_OVERTIME_PARKING)|| \
										 ((AlarmType) == ALARM_ENTER_AREA)|| \
										 ((AlarmType) == ALARM_TOUCH_LINE_k)|| \
										 ((AlarmType) == ALARM_DRIVE_SHORT_LONG_k)|| \
										 ((AlarmType) == ALARM_DRIVE_DEVIATE)|| \
										 ((AlarmType) == ALARM_VSS_ERR)|| \
										 ((AlarmType) == ALARM_OIL_ERR)|| \
										 ((AlarmType) == ALARM_STEAL)|| \
										 ((AlarmType) == ALARM_START_ILLEGAL_k)|| \
										 ((AlarmType) == ALARM_MOVE_ILLEGAL_k)|| \
										 ((AlarmType) == ALARM_ROLLOVER))
enum _MSG_ID_e
{
	DEV_COMMON_rsp	=	0x0001,			//终端通用应答
	PLAT_COMMON_rsp	=	0x8001,			//平台通用应答
	HEART_BEAT		=	0x0002,			//心跳
	REGISTERS		=	0x0100,			//终端注册
	REGISTERS_rsp	=	0x8100,			//终端注册应答
	UNREGISTERS	=	0x0003,//0x0101,			//终端注销
	LOGIN			=	0x0102,			//终端鉴权
	SET_DEV_PARAM	=	0x8103,			//设置终端参数
	GET_DEV_PARAM	=	0x8104,			//查询终端参数
	GET_DEV_PARAM_rsp	=	0x0104,		//查询终端参数应答
	DEV_CONTROL		=	0x8105,			//终端控制
	REPORT_LOCATION	=	0x0200,			//位置信息汇报
	GET_LOCATION	=	0x8201,			//位置信息查询
	GET_LOCATION_rsp=	0x0201,			//位置信息查询应答
	TRACK_LOCATION_CTL	=	0x8202,		//临时位置跟踪控制
	ARTIFICIAL_ACK_ALARM = 0x8203,			//人工确认报警
	TEXT_DOWNLOAD	=	0x8300,			//文本信息下发
	SET_EVENT		=	0x8301,			//事件设置
	REPORT_EVENT	=	0x0301,			//事件报告
	ASK_DOWNLOAD	=	0x8302,			//提问下发
	ASK_ANSWER		=	0x0302,			//提问应答
	DEMAND_MENU		=	0x8303,			//信息点播菜单设置
	DEMAND_SET_CANCEL	=	0x0303,		//信息点播/取消
	DEMAND_SERVER	=	0x8304,			//信息服务
	CALL_BACK		=	0x8400,			//电话回拨
	SET_PHONEBOOK	=	0x8401,			//设置电话本
	CAR_CTL			=	0x8500,			//车辆控制
	CAR_CTL_rsp		=	0x0500,			//车辆控制应答
	SET_ROUND_AREA	=	0x8600,			//设置圆形区域
	DEL_ROUND_AREA	=	0x8601,			//删除圆形区域
	SET_SQUARE_AREA	=	0x8602, 		//设置矩形区域
	DEL_SQUARE_AREA	=	0x8603, 		//删除矩形区域
	SET_POLYGON_AREA	=	0x8604, 	//设置多边形区域
	DEL_POLYGON_AREA	=	0x8605, 	//删除多边形区域
	SET_LINE		=	0x8606, 		//设置路线
	DEL_LINE		=	0x8607, 		//删除路线
	GET_DRIVE_DATA	=	0x8700,			//行驶记录仪数据采集命令
	REPORT_DRIVE_DATA	=	0x0700,		//行驶记录仪数据上传
	DOWNLOAD_DRIVE_DATA	=	0x8701,		//行驶记录仪参数下传命令
	REPORT_BILL		=	0x0701,			//电子运单上报
	REPORT_DRIVER	=	0x0702,			//驾驶员身份信息采集上报
	REPORT_MEDIA_EVENT	=	0x0800,		//多媒体事件信息上传
	REPORT_MEDIA_DATA	=	0x0801,		//多媒体数据上传
	REPORT_MEDIA_DATA_rsp	=	0x8800,	//多媒体数据上传应答
	TAKE_PHOTO		=	0x8801,			//摄像头立即拍摄命令
	SMEDIA_DATA_CHECK	=	0x8802,		//存储多媒体数据检索
	SMEDIA_DATA_CHECK_rsp	=	0x0802,	//存储多媒体数据检索应答
	REPORT_SMEDIA	=	0x8803,			//存储多媒体数据上传
	START_RECORD	=	0x8804,			//录音开始命令
	DOWNLOAD_DATA	=	0x8900,			//数据下行透传
	UPLOAD_DATA		=	0x0900,			//数据上行透传
	UPLOAD_DATA_COMRESSION	=	0x0901,	//数据压缩上报
	RSA_PLAT		=	0x8a00,			//平台RSA公钥
	RSA_DEV			=	0x0a00,			//终端RSA公钥
	CALLCAR_ORDER	=	0x8B00,		    //订单下发
	ORDER_REQUEST	=	0x0B01,			//订单抢答
	ORDER_ACCEPT	=	0x0B02,			//订单接受
	CALLCAR_ORDER_CONFIRM =	0x8B01,		//订单确认，即抢单成功
	ORDER_DONE		=	0x0B07,			//订单完成
	CALLCAR_ORDER_DEL	=	0x8B09,		//订单取消

	APTS_OPERATELINE_SET=	0x8C00,		//设置运营线路
	APTS_LINE_QUERY	=	0x8C01,			//设备中所有线路号查询
	APTS_LINE_QUERY_rsp	=	0x0C01,		//线路号查询应答
	APTS_INTOUT_STATION =0x0c02,		//进出站上报
	STADENTDATA_REPPORT =0x0c03,		//学生上下校车刷卡
//	RESERVE_PLAT							//平台下行消息保留0X8F00-0X8FFF
//	RESERVE_DEV							//终端上行消息保留0X0F00-0X0FFF	
};
//终端上传命令字判断
#define ISPrtclDevMsg(Devmsg) (DEV_COMMON_rsp==Devmsg ||HEART_BEAT==Devmsg ||REGISTERS==Devmsg ||UNREGISTERS==Devmsg \
 ||LOGIN==Devmsg ||GET_DEV_PARAM_rsp==Devmsg ||REPORT_LOCATION==Devmsg ||GET_LOCATION_rsp==Devmsg \
 ||REPORT_EVENT==Devmsg ||ASK_ANSWER==Devmsg ||DEMAND_SET_CANCEL==Devmsg ||CAR_CTL_rsp==Devmsg \
 ||REPORT_DRIVE_DATA==Devmsg ||REPORT_BILL==Devmsg ||REPORT_DRIVER==Devmsg ||REPORT_MEDIA_EVENT==Devmsg \
 ||REPORT_MEDIA_DATA==Devmsg ||SMEDIA_DATA_CHECK_rsp==Devmsg ||UPLOAD_DATA==Devmsg ||UPLOAD_DATA_COMRESSION==Devmsg \
 ||RSA_DEV==Devmsg||ORDER_REQUEST==Devmsg ||ORDER_ACCEPT==Devmsg ||ORDER_DONE==Devmsg ||APTS_LINE_QUERY_rsp==Devmsg ||APTS_INTOUT_STATION==Devmsg)
//平台下发命令字判断
#define ISPrtclServMsg(Servmsg) (PLAT_COMMON_rsp==Servmsg|| REGISTERS_rsp==Servmsg|| SET_DEV_PARAM==Servmsg	\
|| GET_DEV_PARAM==Servmsg ||DEV_CONTROL==Servmsg|| GET_LOCATION==Servmsg|| TRACK_LOCATION_CTL==Servmsg || ARTIFICIAL_ACK_ALARM ==  Servmsg\
|| TEXT_DOWNLOAD==Servmsg|| SET_EVENT==Servmsg || ASK_DOWNLOAD==Servmsg|| DEMAND_MENU==Servmsg \
|| DEMAND_SERVER==Servmsg|| CALL_BACK==Servmsg|| SET_PHONEBOOK==Servmsg|| CAR_CTL==Servmsg|| SET_ROUND_AREA==Servmsg \
|| DEL_ROUND_AREA==Servmsg|| SET_SQUARE_AREA==Servmsg|| DEL_SQUARE_AREA==Servmsg|| SET_POLYGON_AREA==Servmsg \
|| DEL_POLYGON_AREA==Servmsg|| SET_LINE==Servmsg|| DEL_LINE==Servmsg|| GET_DRIVE_DATA==Servmsg \
|| DOWNLOAD_DRIVE_DATA==Servmsg|| REPORT_MEDIA_DATA_rsp==Servmsg|| TAKE_PHOTO==Servmsg|| SMEDIA_DATA_CHECK==Servmsg \
|| REPORT_SMEDIA==Servmsg|| START_RECORD==Servmsg|| DOWNLOAD_DATA==Servmsg|| RSA_PLAT==Servmsg ||CALLCAR_ORDER==Servmsg \
||CALLCAR_ORDER_CONFIRM==Servmsg ||CALLCAR_ORDER_DEL==Servmsg ||APTS_OPERATELINE_SET==Servmsg ||APTS_LINE_QUERY==Servmsg)
/*----------------------消息体结构----------------------*/
//消息体属性
typedef struct _MSG_HEAD_bodyattr_s
{
	u16 MsgbodyLen:10;			//消息体属性，消息体长度
	u16 DataEncrypt1:1;			//消息体属性，数据加密方式
	u16 DataEncrypt2:1;
	u16 DataEncrypt3:1;
	u16 subpacket:1;			//消息体属性，分包
	u16 reserve:2;				//消息体属性，保留
}MSG_HEAD_bodyattr;
//没有分包时的消息头
typedef struct _MSG_HEAD
{
	u8 MsgId[2];				//消息ID
	u8 MsgbodyAttr[2];//消息体属性
	u8 lclTel[6];				//本机号
	u8 SequenceId[2];			//流水号
	//4B 消息包封装项
}MSG_HEAD;

//当有分包时候的消息头MSG_HEAD+	MSG_HEAD_sub
typedef struct _MSG_HEAD_sub
{
	u8 totalpacketNum[2];		//消息总包数
	u8 SubpacketNo[2];			//包序号,从1开始
}MSG_HEAD_sub;
//平台通用应答
typedef struct _PLAT_COMMON_rsp_s
{
	u8 SeqId[2];			//对应终端消息的流水号 高0低1
	u8 MsgId[2];			//对应终端消息的ID
	u8 Rslt;				//0:成功/确认,1.失败,2.消息有误,3.不支持,4.报警处理确认
}LBS_PlatComRsp;
//终端注册
typedef struct _DEV_REGISTERS_s
{
	u8 ProvinceId[2];			//省级ID
	u8 CityId[2];				//市县域ID
	u8 productor[5];			//终端制造商编码
	u8 devType[8];				//终端型号，不足地方空格补齐
	u8 devId[7];				//终端ID，由大写字母和数字组成
	u8 CarIdColor;				//车牌颜色
	u8 CarId[8];				//机动车号牌
}LBS_DevRegreq;
//终端注册应答
typedef struct _DEV_REGISTERS_rsp_s
{
	u16 SeqId;		//reg的流水号
	u8 Rslt;			//0:成功,1:车辆已注册2:无该车辆3:终端已经注册4:无该终端
	//鉴权码存入flash	
}LBS_DevRegRsp;
//终端参数tlv列表
typedef struct _param_list_s
{
	u8 paramId[4];				//参数ID
	u8 paramLen;				//参数长度
	u8 paramValue;				//参数值
}PARAM_LIST;
//查询终端参数应答
typedef struct _GET_DEV_PARAM_rsp_s
{
	u8 RspId[2];			//应答流水号
//	u8 paramNum;			//应答参数个数
//	PARAM_LIST paramlist；	//参数值	
}LBS_GetDevParamrsp;
//终端控制命令字
typedef enum _DEV_CONTROL_enum
{
	eOnlineUpgrad_Ctrl=1, //在线升级
	eSpclServ_Ctrl=2,     //特殊平台
	eDevPowoff_Ctrl=3,    //设备关机
	eDevReset_Ctrl=4,     //设备服务
	eDevFactoryset_Ctrl=5,//设备出厂设置
	eCloseDataCommu_Ctrl=6,//仅关闭无线通信的数据业务
	eCloseWCMCommu_Ctrl=7 //关闭所有无线通信
}enum_devCtrl_cmd;
//基本位置信息
typedef struct _BASE_LOCATION_INFO_s
{
	u8 AlarmFlag[4];		//报警标志
	u8 StateBit[4];			//状态位标志
	u8 Latitude[4];			//纬度
	u8 Longitude[4];		//经度
	u8 High[2];				//海拔高度 单位m
	u8 Speed[2];			//速度,1/10km/h
	u8 Direction[2];		//方向,0-359,顺势针，正北
	u8 Time[6];
}BASE_LOCATION_INFO;
//位置附加信息项列表
typedef struct _ADD_LOCATION_INFO_s
{
	u8 AddInfoId;			//附加信息ID	
	u8 AddInfoLen;			//附加信息长度
//	addinfomsg;			//附加信息
}ADD_LOCATION_INFO;

//位置信息汇报
typedef struct _REPORT_LOCATION_s
{
	BASE_LOCATION_INFO BaseLocationInfo;
//	ADD_LOCATION_INFO AddLocationInfo;
}LBS_ReportLocationreq;

#if 0
//位置信息查询
typedef struct _GET_LOCATION_s
{
	//none
}LBS_GetLocationreq;
#endif
//808协议在线升级参数
typedef struct _UPGRAD_PARA808_s
{
	u8 URL[2]; //无效
	u8 APN[10]; //?
	u8 User[MAXPPPUSERLEN+1];
	u8 Pwd[MAXPPPPWDLEN+1];
//	u8 IP[16];//zq
	u16 Tcpport;
	u16 Udpport;
	u8  Productor[5];
	u8  HW[3];
	u8  FW[2]; //无效
	u16 Limittime;
}LBS_OnlineupgraPara;
//位置信息查询应答
typedef struct _GET_LOCATION_rsp_s
{
	u8 sequenceId[2];				//应答流水化
	LBS_ReportLocationreq reportLocation;	//位置信息汇报
}LBS_GetLocationrsp;
//临时位置跟踪控制
typedef struct _TRACK_LOCATION_CTL_s
{
	u8 TimeInterval[2];				//时间间隔，单位s，0则停止跟踪，若为0，则无后续字段
	u8 TrackPeriod[4];				//跟踪有效期，单位s，
}LBS_TrackLocationCtlreq;
//文本信息下发
typedef struct _TEXT_DOWNLOAD_s
{
	u8 flag;				//bit 0.1紧急1.保留2.1:终端显示器显示3.1终端TTS播读4.1:广告屏显示，5-7保留
//	u8 TextInfo[];		//经过GBK编码后最长1024B，
}LBS_TextDownloadreq;
//事件项组成
typedef struct _EVENT_INFO_s
{
	u8 EventId;				//事件项ID
	u8 EventLen;			//事件项长度
//	u8 EventMsg	[];		//事件项内容，经过GBK编码
}EVENT_INFO;
//事件设置
typedef struct _SET_EVENT_s
{
	u8 EventType;				//设置类型0删除所有事件，后不带字节1更新2追加3修改4删除特定，事件项无需带内容
	u8 totalNum;				//设置总数
//	EVENT_INFO eventInfo[totalnum];				事件项组成
}LBS_SetEventreq;
//事件报告
typedef struct _REPORT_EVENT_s
{
	u8 EventId;				//事件ID

}LBS_ReportEventreq;
//候选答案列表
typedef struct _ANSWER_LIST_s
{
	u8 answerId;			//答案ID
	u8 answerLen[2];			//答案LEN
//	u8	AnswerString;		//答案内容
}ANSWER_LIST;
//提问下发
typedef struct _ASK_DOWNLOAD_s
{
//	u8 flag;				//0.1:紧急1-2.保留3.1:终端TTS播读4.1:广告屏显示5-7保留
//	u8 AskLen;				//问题长度
//	u8 AskMsg;				//问题内容，经过GBK编码	
//	ANSWER_LIST AnswerList;			//候选答案列表
	u8 AnswerID[10];
}LBS_AskDownloadreq;
//提问应答
typedef struct _ASK_ANSWER_rsp_s
{
	u8 sequenceId[2];		//应答流水号
	u8 AnswerId;			//答案ID
}LBS_AskAnswerrsp;
//信息项列表
typedef struct _MENU_NUM_s
{
	u8 MenuType;			//信息项类型
	u8 MenuLen[2];			//信息名称长度	
//	u8 MenuMsg[];			//信息名称，经过GBK编码
}MENU_LIST;
//信息点播菜单设置
typedef struct _DEMAND_MENU_s
{
	u8 DemandType;				//设置类型
	u8 MenuNum;					//信息项总数
//	MENU_LIST	menuList;			//信息项列表	

}LBS_DemandMenureq;
//信息点播/取消
typedef struct _DEMAND_SET_CANCEL_s
{
	u8 MenuType;			//信息类型
	u8 flag;				//0取消 1点播
}LBS_DemandSetCancelreq;
//信息服务
typedef struct _DEMAND_SERVER_s
{
	u8 MenuType;			//信息类型
	u16 MenuLen;			//信息长度
//	u8 MenuMsg[];			//信息内容，经GBK编码
}LBS_DemandServreq;
//电话本联系人项数据格式
typedef struct _CONTACT_LIST_s
{
	u8 flag;			// 1呼入2呼出 3呼入/呼出
	u8 TelLen;			//号码长度
//	u8 TelNum[];		//电话号码
//	u8 ContactLen;		//联系人长度
//	u8 ContactNum[];	//联系人，经GBK编码
}CONTACT_LIST;
//设置电话本
typedef struct _SET_PHONEBOOK_s
{
	u8 BookType;		//设置类型0删除终端上所有存储的联系人1更新2追加3修改
	u8 ContactNum;		//联系人总数
//	CONTACT_LIST ContactList;		//联系人项	
}LBS_SetPhonebookreq;
//车辆控制
typedef struct _CAR_CTL_s
{
	u8 CtlFlag;			//控制标志，bit0:0车门解锁1车门加锁，bit1-7保留
}LBS_CarCtlreq;
//车辆控制应答
typedef struct _CAR_CTL_rsp_s
{
	u8 sequenceId[2];								//流水号
}LBS_CarCtlrsp;
//圆形区域项
typedef struct _ROUND_AREA_LIST_s
{
	u32 AreaId;					//区域ID
	u16 AreaProperty;			//区域属性bit0.1:根据时间bit1.1:限速bit2.1:进区域报警给驾驶员
	u32 CenterLatitude;			//中心点纬度
	u32 CenterLongitude;		//中心点经度
	u32 Radius;					//半径
	u8 StartTime[6];			//起始时间,若区域属性0位为0，则无该字段
	u8 StopTime[6];				//结束时间,若区域属性0位为0，则无该字段
	u16 HighSpeed;				//最高速度,若区域属性1位为0，则无该字段
	u8 OverSpeedTime;			//超速持续时间，若区域属性1位为0，则无该字段
}ROUND_AREA_LIST;
//设置圆形区域
typedef struct _SET_ROUND_AREA_s
{
	u8 AreaType;				//设置属性0更新区域1追加区域2修改区域
	u8 AreaNum;					//区域总数
//	ROUND_AREA_LIST  AreaList;			//区域项
}LBS_SetRoundAreareq;
//删除圆形区域
typedef struct _DEL_ROUND_AREA_s
{
	u8 AreaNum;					//区域数
//	u32 AreaId1;
//	u32 AreaId2;......
}LBS_DelRoundAreareq;
//矩形区域项
typedef struct _SQUARE_AREA_LIST_s
{
	u32 AreaId;					//区域ID
	u16 AreaProperty;			//区域属性bit0.1:根据时间bit1.1:限速bit2.1:进区域报警给驾驶员
	u32 LeftTopLatitude;			//左上点纬度
	u32 LeftTopLongitude;			//左上点经度
	u32 RightBottomLatitude;		//右下点纬度
	u32 RightBottomLongitude;		//右下点经度
	u8 StartTime[6];			//起始时间
	u8 StopTime[6];				//结束时间
	u16 HighSpeed;				//最高速度,若区域属性1位为0，则无该字段
	u8 OverSpeedTime;			//超速持续时间，若区域属性1位为0，则无该字段
}SQUARE_AREA_LIST;
//设置矩形区域
typedef struct _SET_SQUARE_AREA_s
{
	u8 AreaType;				//设置属性0更新区域1追加区域2修改区域
	u8 AreaNum; 				//区域总数
//	SQUARE_AREA_LIST  AreaList;			//区域项
}LBS_SetSquareAreareq;

//删除矩形区域
typedef struct _DEL_SQUARE_AREA_s
{
	u8 AreaNum; 				//区域数
//	u32 AreaId1;
//	u32 AreaId2;......
}LBS_DelSquareAreareq;

//顶点项
typedef struct _TOP_LIST_s
{
	u32 topLatitude;			//顶点纬度
	u32 topLongitude;			//顶点经度
}TOP_LIST;
	
//设置多边形区域
typedef struct _SET_POLYGON_AREA_s
{
	u32 AreaId; 				//区域ID
	u16 AreaProperty;			//区域属性bit0.1:根据时间bit1.1:限速bit2.1:进区域报警给驾驶员
	u8 StartTime[6];			//起始时间
	u8 StopTime[6]; 			//结束时间
	u16 HighSpeed;				//最高速度,若区域属性1位为0，则无该字段
	u8 OverSpeedTime;			//超速持续时间，若区域属性1位为0，则无该字段
	u16 Areatop;				//区域顶点数
//	Top_LIST top_list[];			//顶点项	
}LBS_SetPolygonAreareq;

//删除多边形区域
typedef struct _DEL_POLYGON_AREA_s
{
	u8 AreaNum; 				//区域数
//	u32 AreaId1;
//	u32 AreaId2;......
}LBS_DelPolygonAreareq;
//拐点项
typedef struct _TURNNING_LIST_s
{
	u32 TurnningId;			//拐点ID
	u32 LineID;				//路线ID
	u32 TurnningLatitude;			//拐点纬度
	u32 TurnningLongitude;			//拐点经度
	u8 LineWidth;				//路段宽度
	u8 LineProperty;			//路段属性
	u16 DriveMaxTime;			//路段行驶过长阈值，单位s，若路段属性为0位为0，则没有该字段
	u16 DriveMinTime;			//路段行驶不足阈值，单位s，若路段属性为0位为0，则没有该字段
	u16 HighSpeed;				//路段最高速度，单位km/h，单位s，若路段属性为1位为0，则没有该字段
	u8 OverSpeed;				//路段超速持续时间，单位s，单位s，若路段属性为1位为0，则没有该字段
}TURNNING_LIST;
//设置路线
typedef struct _SET_LINE_s
{
	u32 LineId; 				//区域ID
	u16 LineProperty;			//区域属性bit0.1:根据时间bit1.1:限速bit2.1:进区域报警给驾驶员
	u8 StartTime[6];			//起始时间
	u8 StopTime[6]; 			//结束时间
	u16 TurnningNum;				//路线总拐点数
//	TURNNING_LIST turn_list[];			//顶点项	
}LBS_SetLinereq;

//删除线路
typedef struct _DEL_LINE_s
{
	u8 LineNum; 				//区域数0为删除所有线路数
//	u32 AreaId1;
//	u32 AreaId2;......
}LBS_DelLinereq;
//行驶记录数据采集命令
typedef struct _GET_DRIVE_DATA_s
{
	u8 commandId;		//命令字
}LBS_GetDriveDatareq;

//行驶记录数据上传
typedef struct _REPORT_DRIVE_DATA_rsp_s
{
	u8 sequenceId[2];			//流水号
	u8 commandId;			//命令字
//	u8 Datablock[];			//数据块
}LBS_ReportDriveDatarsp;

//行驶记录参数下传命令
typedef struct _DOWNLOAD_DRIVE_DATA_s
{
	u8 commandId;			//命令字
//	u8 DataBlock[];			//数据块
}LBS_DownloadDriveDatareq;

//驾驶员身份信息采集上报
typedef struct _REPORT_DRIVER_s
{
	u8 DriverNameLen;			//驾驶员姓名长度
	u8 DriverName[12];			//驾驶员名称
	u8 DriverID[20];				//身份证编号
	u8 driverCertificateId[40];		//从业资格证编码
	u8 OrgNameLen;				//发证机构名称长度
	u8 OrgName[10];				//从业资格证发证机构名称
}LBS_ReportDriverreq;
typedef struct _REPORT_ELECMENU_s
{
	u8 len[4];
	u8 Content[10]; //just for 808 test
}LBS_RptElecMenu;
typedef enum _MEDIA_TYPE_s
{
	eImage=0,
	eAudio,
	eVideo
};

//摄像头立即拍摄或录像命令
typedef struct _TAKE_PHOTO_s
{
	u8 ChannelId;			//通道ID
	u8 ShootCmd[2];			//0停止拍摄0xFFFF表示摄像，其他表示拍照张数
	u8 ShootTime[2];		//拍照间隔或者录像时间 s
	u8 SaveFlag;			//1.保存0实时上传
	u8 Resolution;			//分辨率1.320*240     2.640*480	3.800*600	4.1024*768	5.176*144	6.352*288	7.704*288	8.704*576
	u8 Quality;				//1-10，1.质量损失最小，10表示压缩比最大
	u8 Brightness;			//亮度0-255
	u8 Contrast;			//对比度，0-127
	u8 Saturation;			//饱和度，0-127
	u8 Color;				//色度，0-255
}TakePhotoStrct;
//录音开始命令
typedef struct _START_RECORD_s
{
	u8 RecordCmd;				//录音命令0停止录音 ，1开始录音
	u8 RecordTime[2];			//录音时间，单位s，0表示一直录音
	u8 StoreFlag;				//保存标志，0实时上传1保存
	u8 AudioSample;				//音频采样率，0:8k, 1:11K, 2:23K, 3:32K, 其他保留
}LBS_StartRecordreq;
//多媒体事件信息上传
typedef struct _REPORT_MEDIA_EVENT_s
{
	u8 MediaId[4];				//多媒体数据ID 与ChannelId对应
	u8 MediaType;				//0:图像1:音频2:视频
	u8 MediaCodeFormat;			//0.JPEG, 1.TIF,2.MP3,3.WAV,4.WMV,其他保留
	u8 EventCode;				//0平台下发指令1定时动作2抢劫报警触发3碰撞侧翻报警触发，其他保留
	u8 ChannelId;				//通道ID
}LBS_Mediainfo;

//多媒体数据上传应答
typedef struct _REPORT_MEDIA_DATA_RSP
{
	u8 MediaId[4];				//多媒体ID
	u8 RspPacketNum;			//重传包总数
	u16 RspPacketID[125];//重传包ID列表
}LBS_ReportMediaDatarsp;


//存储多媒体数据检索
typedef struct _SMEDIA_DATA_CHECK_s
{
	u8 MediaType;				//0:图像1:音频2:视频
	u8 ChannelId;				//0 表示检索改媒体类型的所有通道
	u8 EventCode;				//0平台下发指令1定时动作2抢劫报警触发3碰撞侧翻报警触发，其他保留
	u8 StartTime[6];			//起始时间YY-MM-DD-HH-MM-SS
	u8 StopTime[6]; 			//结束时间
}LBS_SMediaDataCheckreq;

//多媒体检索项数据格式
typedef struct _MEDIA_DATA_FORMAT_s
{
	u8 MediaType;				//0:图像1:音频2:视频
	u8 ChannelId;				//0 表示检索改媒体类型的所有通道
	u8 EventCode;				//0平台下发指令1定时动作2抢劫报警触发3碰撞侧翻报警触发，其他保留
	LBS_ReportLocationreq reportLocation;	//位置信息汇报
}MEDIA_DATA_FORMAT;

//存储多媒体数据检索应答
typedef struct _SMEDIA_DATA_CHECK_rsp_s
{
//	u8 sequenceId[2];			//流水号
	u8 DataNum[2];			//满足检索条件的多媒体数据总项数
//检索项
}LBS_SMediaDataCheckrsp;

//存储多媒体数据上传命令
typedef struct _REPORT_SMEDIA_s
{
	u8 MediaType;				//0:图像1:音频2:视频
	u8 ChannelId;				//0 表示检索改媒体类型的所有通道
	u8 EventCode;				//0平台下发指令1定时动作2抢劫报警触发3碰撞侧翻报警触发，其他保留
	u8 StartTime[6];			//起始时间YY-MM-DD-HH-MM-SS
	u8 StopTime[6]; 			//结束时间
	u8 DelFlag;					//0保留，1删除
}LBS_ReportSMediareq;

//数据下行透传
typedef struct _DOWNLOAD_DATA_s
{
	u8 DownloadType;				//透传消息类型
	u8 DownloadMsg[12];				//透传消息内容
}LBS_DownloadDatareq;

//数据上行透传
typedef struct _UPLOAD_DATA_s
{
	u8 UploadType;				//透传消息类型
	u8 UploadMsg[12];				//透传消息内容
}LBS_UploadDatareq;
//电话叫车
typedef struct _CALLCAR_ORDER_ST
{
	u8 OrderID[50];
	u8 OrderTel[20];
//	u8 OrderType;
//	u8 OrderTime[17]; //YYYYMMDD HH:MM:SS
	u8 OrderContentlen;
	u8 OrderContent[230]; //内容GBK "类型:即时/预约/指派,要车时间YYYYMMDD HH:MM:SS 说明”
}STRUCT_CALLCARORDER;
//数据压缩上报
typedef struct _UPLOAD_DATA_COMRESSION_s
{
	u32 CompressDataLen;			//压缩数据长度
	u8 CompressMsg[12];				//压缩消息体
}LBS_UploadDataCompressionreq;

//平台RSA公钥
typedef struct _RSA_PLAT_s
{
	u8 e[4];			//平台RSA公钥{e,n}中的e
	u8 n[128];		//RSA公钥{e,n}中的n
}LBS_RSAplat;

//终端RSA公钥
typedef struct _RSA_DEV_s
{
	u8 e[4];			//终端RSA公钥{e,n}中的e
	u8 n[128];		//RSA公钥{e,n}中的n
}LBS_RSAdev;

//监控平台模式：主平台，副平台，特殊平台
typedef enum _SERV_MODE_DEF
{
	eMainServ=0,
	eSubServ,
	eSpclServ
} E_ServMode;
//监控平台socket参数
typedef struct _SERV_SOCKET_STRCT
{
	u8  APN[10];
	u8  User[MAXPPPUSERLEN+1];
	u8  Pwd[MAXPPPPWDLEN+1];
//	u8  IP[16];//zq
	u32 port;
}MonitServSocketPara;
//终端注册状态定义
typedef enum _REGIST_STA
{
	Regin=0x00,		//已注册
	Regout,			//已注销 DB车辆与设备信息删除？
	unRegist=0xFF,	//未注册
//	CarReg,			//车辆已注册err
//	NocarReg,		//车辆不存在err
///	DevReg,			//设备已注册err
//	NodevReg,		//设备不存在err
} eRegsitSta;
//终端自定义固定参数
typedef struct _STRUCT_DEVIC_DEF
{
	u8  Productor[5];//厂商 定长
	u8	DevType[8]; //设备类型	0TGC-20I 定长
	u8	DevID[12];	 //设备ID 	p808取后7位
	u8  Ver_HW[3];   //硬件版本 V15
	u8  Ver_SW[9];  //软件版本	SOFTWARE_VER:V010RE001
}STRUCT_DEVDEF_PARA;

//通信方式，未处理
typedef enum _ENUM_SOCKET_MODE
{
	eUDP_MODE=0,
	eTCP_MODE,
	eSMS_MODE,
};

//STRUCT_LBS_CFG成员枚举
typedef enum  _CTRL_PARA_ENUM
{
	eHtTime				=	0x0001,
	eTcptime			=	0x0002,
	eTcpnum				=	0x0003,
	eUdptime			=	0x0004,
	eUdpnum				=	0x0005,
	eSMSTime			=	0x0006,
	eSMSNum				=	0x0007,

	eAPN				=	0x0010,
	eAPNUser			=	0x0011,
	eAPNPwd				=	0x0012,
	eIP					=	0x0013,
	eAPN_sub			=	0x0014,
	eAPNUser_sub		=	0x0015,
	eAPNPwd_sub			=	0x0016,
	eIP_sub				=	0x0017,
	eTcpPort			=	0x0018,
	eUdpPort			=	0x0019,
	
	eRptStrategy		=	0x0020,
	eRptType			=	0x0021,
	eRpttime_unlog		=	0x0022,
	eRpttime_sleep		=	0x0027,
	eRpttime_emergency	=	0x0028,
	eRpttime_def		=	0x0029, //默认时间间隔
	eRptDistanc_def		=	0x002c,	
	eRptDistanc_sleep	=	0x002d,
	eRptDistanc_unlog	=	0x002e,
	eRptDistanc_emergency=	0x002f,
	eRptCog				=	0x0030,
	eFencRadius			=	0x0031,
	eServTel			=	0x0040,
	eResetTel			=	0x0041,
	eFactryTel			=	0x0042,
	eSMSTel				=	0x0043,
	eAlarmSMSTel		=	0x0044,
	eAnswerType 		=	0x0045,
	eMaxCalltime		=	0x0046,
	eMaxCalltime_month	=	0x0047,
	eMonitTel			=	0x0048,
	eSpeclSMSTel		=	0x0049,
	//以下报警相关定义与位置信息报警标志相对应
	eAlarmmaskword		=	0x0050, //报警屏蔽字
	etxtSMSAlarm		=	0x0051, //报警sms报文
	eAlarmPhoto			=	0x0052,	//报警拍照开关
	eAlarmPhotoStor		=	0x0053,	//报警拍照存储标志
	eKeyAlarm			=	0x0054, //关键报警标志
	eOverspeed			=	0x0055,	//超速
	eOverspeedtime		=	0x0056,
	eDrivTime			=	0x0057,
	eDrivTime_day		=	0x0058,
	eRestTime_min		=	0x0059,	//最小休息时间，达到次时间取消疲劳驾驶
	eStopcarTime_max	=	0x005a,	//即超时停车报警时限
	eImagQuality		=	0x0070,
	eImagBright			=	0x0071,	
	eImagContrast		=	0x0072,
	eImagSatuatn		=	0x0073,
	eImagClor			=	0x0074,
	
	eOdometer			=	0x0080, //里程表精度 
	ePrivic				=	0x0081,	//车辆所在的省
	eCity				=	0x0082,	//车辆所在的市
	eCarid				=	0x0083,	//车牌号
	eCarclor			=	0x0084,
	//自定义
	eLclTEL	=0xaaaa,	//本机号
	eLimitTime=0xaaa1,
	eNET=0xaaa2,		//网络类型 主平台
	eNET_sub=0xaaa3,	//备份平台
	eCfaParaMax=0xFFFF,	//无效值
	emanufacturersid =  0x0085,
	etermmodel = 0x0086,
	etermid = 0x0087,
}Enum_CtrlCfgPara;
/*---------------------------------808协议参数------------------------------*/
//平台请求信息
typedef struct _SERV_REQUEST_s
{
	u16 ServSeqid; //流水号
	u16 ServMsgid; //msd
}STRUCT_SERV_REQ;
//终端通用应答
typedef struct _DEV_COMMON_rsp_s
{
	STRUCT_SERV_REQ servReq;
	u8 Rslt;					//0:成功/确认,1.失败,2.消息有误,3.不支持
}LBS_DevRsp;
//////////////////////////////////////////从其他的头文件中提取出来////////////////////////
#define BLOCK_LEN	  16   //块长度
#define CARD_NUM_LEN  4   //有效卡号长度
#define STU_NUM_LEN	  16   //有效学生学号长度
#define STU_NAME_LEN  16   //有效学生姓名长度
#define PARE_PHONE_LEN	16  //有效学生家长号码长度
//站点信息
typedef struct _STAT_INFO
{
	u8 InOutflag;  //对应STATION_IN或STATION_OUT
	u8 stationDirect;
	u8 stationNo;//站点号
	u8 stationType;
	u8 LineVer;
	u32 lineNo;
}StateStrct;
/*存储格式*/
typedef struct
{
	u8 CardNum[CARD_NUM_LEN+1];
	u8 StuNum[STU_NUM_LEN+1];
	u8 StuName[STU_NAME_LEN+1];
	u8 ParePhone[PARE_PHONE_LEN+1];
	u8 aa_flag;	//标志位
}
StuData;
////////////////////////////////////////////////////////////////////////////////////////////////////////
//监控平台协议参数
typedef struct _Protocol_Handle_s
{
	u8 DevSeqId[2];					//终端发送消息流水号 ，0开始，可循环
	u8 DevMsgId[2];					//终端发送消息ID by zhuqing @2013/6/26
	u8 ServSeqId[2];				//平台请求消息流水号
	u8 ServMsgId[2];				//平台请求消息的ID
	u8  Rsp2Serv;					//平台请求应答结果
	//平台对终端请求应答
	LBS_PlatComRsp PlatComrsp;

	MonitServSocketPara spclServ; //监管平台参数
	u16 ExptAckId;					//终端发送请求后期望的消息id global?
//	u16 bRSA;						//是否需要加密，1加密，0不加密
	u8 LocalResult;					//终端执行结果 删除
	u8 PlatResult;					//平台执行结果
	u8 LclTel[TELMAXLEN];			//终端UIM卡号

	u32 pdata;						//组包时数据在flash的存储首址，如多媒体数据
	u32 datalen;					//组包时候需要被传输的数据包长度，留作分包用
	//平台下发报文存储
	u8  devreqrsp[MAX_MSG_LEN];
	u16 devreqrsplen;				//包长度
	u8  *platreq;					//指向平台下发报文buf的指针
	u16 platreqlen;					//平台发送的报文长度 ？？
	u8  Check;						//报文校验结果
	//消息格式
	u8 msgbuf[MAX_MSG_LEN];	//发送报文存储
	u16 msgbuflen;
	MSG_HEAD_sub sub;		//分包属性
	u32 serialnum;			//保存记录仪数据存储区域当前序号
	u8 multipacket;			//在应答处理过程中，如果应答包需要分包，则设置为1
	u32 multipacketlen;		//在应答处理过程中，如果需要分包，则得到数据的总长度

//	LBS_OnlineupgraPara Upgradpara;

	//终端注册
	LBS_DevRegreq DevRegreq;
	//终端注册应答
	LBS_DevRegRsp DevRegRsp;

	//设置终端参数
//	LBS_SetParamreq SetParamreq;
	//查询终端参数应答
//	LBS_GetDevParamrsp GetDevParamrsp;
	//终端控制
//	LBS_DevCtlreq DevCtlreq;
	//位置信息汇报
//-	LBS_ReportLocationreq ReportLocationreq;

	//位置信息查询应答
	LBS_GetLocationrsp GetLocationrsp;
	//终端控制指定监控平台参数
	MonitServSocketPara SpeclServPara;
	u8  Authen[AUTHENMAXLEN+1];		//鉴权码,'\0'结尾，组包用
	//临时位置跟踪控制
	LBS_TrackLocationCtlreq TrackLocationCtlreq;
	//文本信息下发
	LBS_TextDownloadreq TextDownloadreq;
	//事件设置
	LBS_SetEventreq SetEventreq;
	//事件报告
	LBS_ReportEventreq ReportEventreq;
	//提问下发
	LBS_AskDownloadreq AskDownloadreq;
	//提问应答
	LBS_AskAnswerrsp AskAnswerrsp;
	//信息点播菜单设置
	LBS_DemandMenureq DemandMenureq;
	//信息点播/取消
	LBS_DemandSetCancelreq DemandSetCancelreq;
	//信息服务
	LBS_DemandServreq DemandServereq;
	//电话回拨
	u8 CallBackreq[TELMAXLEN+2]; //类型1B+TEL,'\0'结尾
	//设置电话本
	LBS_SetPhonebookreq SetPhonebookreq;
	//车辆控制
	LBS_CarCtlreq CarCtlreq;
	//车辆控制应答
	LBS_CarCtlrsp CarCtlrsp;
#if 0
	//圆形区域项
	ROUND_AREA_LIST;
	//设置圆形区域
	LBS_SetRoundAreareq;
	//删除圆形区域
	LBS_DelRoundAreareq;
	//矩形区域项
	SQUARE_AREA_LIST;
	//设置矩形区域
	LBS_SetSquareAreareq;
	//删除矩形区域
	LBS_DelSquareAreareq;
	//设置多边形区域
	LBS_SetPolygonAreareq;
	//删除多边形区域
	LBS_DelPolygonAreareq;
	//设置路线
	LBS_SetLinereq;
	//删除线路
	LBS_DelLinereq;
#endif
	//行驶记录数据采集命令
	LBS_GetDriveDatareq GetDriveDatareq;
	//行驶记录数据上传
	LBS_ReportDriveDatarsp ReportDriveDatarsp;
	//行驶记录参数下传命令
	LBS_DownloadDriveDatareq DownloadDriveDatareq;

	LBS_RptElecMenu ElecMenu;
	//驾驶员身份信息采集上报
	LBS_ReportDriverreq ReportDriverreq;
	//多媒体事件信息上传
	LBS_Mediainfo MediaEventinfo;
	//多媒体数据上传应答
	LBS_ReportMediaDatarsp ReportMediaDatarsp;
	//摄像头立即拍摄命令
	TakePhotoStrct TakePhotoreqpara;
	//存储多媒体数据检索
	LBS_SMediaDataCheckreq SmediaDataCheckreq;
	//存储多媒体数据检索应答
	LBS_SMediaDataCheckrsp SMediaDataCheckrsp;
	//存储多媒体数据上传命令
	LBS_ReportSMediareq ReportSmediareq;
	//录音开始命令
	LBS_StartRecordreq StartRecordreq;
	//数据下行透传
	LBS_DownloadDatareq DownloadDatareq;
	//数据上行透传
	LBS_UploadDatareq UploadDatareq;
	//数据压缩上报
	LBS_UploadDataCompressionreq UploadDataCompressionreq;
	//平台RSA公钥
	LBS_RSAplat RSAplat;
	//终端RSA公钥
	LBS_RSAdev RSAdev;
	u8 CallcarAccept; //1接受 2已载客
	u8 CallcarResult; //0完成 1未接到人
	STRUCT_CALLCARORDER stCallcarOrder;
	StateStrct stationInfo;
	StuData	studentData;	//学生上下车刷卡数据
}ProtocolHandle;
typedef enum _PROTOCOL_HANDLE_e
{
	eDevSeqid,				//终端发送消息流水号
	eDevMsgid,				//终端发送消息ID by zhuqing @2013/6/26
	eServSeqid,				//平台请求消息流水
	eServMsgid,				//平台请求消息ID
	eRsp2ServSeq,			//平台请求应答结果
	e_Authen,				//鉴权码
	eMediaInfo,				//多媒体信息
	eRsp2DevReq,			//终端请求应答结果
	eRsp2DevReq_Media,		//终端请求应答结果失败时，多媒体失败包信息
	eSpclServPara,			//终端控制的监管平台参数
	e_TrackLocationCtlreq,	//临时位置信息查询请求
	eRecordPara,            //录音参数

//	e_SpclSockt,			//终端控制之指定平台socket参数
	e_ReportEventreq,		//事件报告
	e_AskAnswerrsp,			//提问应答
	e_DemandSetCancl,		// 信息点播/取消
	e_ReportDriverreq_DriverName,		//采集驾驶员身份信息数据//驾驶员名称
	e_ReportDriverreq_DriverID,			//采集驾驶员身份信息数据//身份证编号	
	e_ReportDriverreq_driverCertificateId,//采集驾驶员身份信息数据//从业资格证编码
	e_ReportDriverreq_OrgName,				//采集驾驶员身份信息数据//从业资格证发证机构名称
	e_RSAdev,							//终端RSA公钥消息体
	e_RSAplat,							//平台RSA公钥消息体
	e_ReportMediaDatarsp,				//校验多媒体数据上传应答
//	e_ReportMediaDatarsp_MediaIdList,	//校验多媒体数据上传应答//重传包ID列表
	
	e_CarCtlrsp,			//车辆控制
	e_ReportDriveDatarsp,	//行驶记录数据上传
	e_TextDownloadreq,		//文本信息下发
	e_SetEventreq_EventType,	//事件设置//设置类型
	e_SetEventreq_totalNum,		//事件设置//设置总数
	e_AskDownloadreq,			//提问下发
	e_DemandMenureq_DemandType,	//信息点播菜单设置//设置类型
	e_DemandMenureq_MenuNum,	//信息点播菜单设置//信息项总数
	e_DemandServreq, 			//信息服务
	e_CallBackreq, 				//电话回拨
	e_SetPhonebookreq_BookType,		//设置电话本//类型
	e_SetPhonebookreq_ContackNum,	//设置电话本//数量
	e_CarCtlreq,					//车辆控制
	e_ElecMenuRpt,					//电子运单上报
	e_GetDriveDatareq,				//采集行驶记录数据
	e_TakePhotoreq, 				//拍摄图片
	e_SMediaDataCheckreq, 			//检索终端存储多媒体数据
	e_RSPSMediaDataCheckreq, 		//检索终端存储多媒体数据应答
	e_ReportSMediareq, 				//存储多媒体数据上传命令
	e_CallcarOrder,					//电召数据
	e_CallcarInfoOrder,				//电召详细数据
	e_CallcarAccept,				//订单接受
	e_CallcarResult,				//电召完成 0成功 1未接到人
	e_APTSStationinfo,				//智能公交站点信息
	e_ProHandlMAX
}Protocol_Handle_e;

//终端请求结果
typedef enum
{
	RspOK = 0,    //应答成功
	RspError,	//注册应答车辆已被注册
	RspMsgerr,	//注册应答数据库无该车辆
	RspUnsurport,	//终端已被注册
	RspAlarmCheck,	//数据库无该终端
	RspPackgerr,    //应答数据包错误
	ServNorsp,  //平台未应答
	ServUnlog_SendErr,  //未登录到平台或数据发送错误
	RspErrOther, //参数错误,组包错误等

	PLAT_TREMREG_ACK = 0x10,
	PLAT_COMMON_ACK = 0x11,
	PLAT_REQ,
	
}Enum_DevReqRsult;
u8 WriteAlarmPara(u8 AcktionMode,u8 AlarmSector,u32 AlarmBits);
u8 WriteAllBitAlarmPara(u8 AlarmSector,u32 AlarmBits);
u8 ReadAlarmPara(u8 AlarmSector,u32 AlarmBits);
u8 ReadAllBitAlarmPara(u8 AlarmSector,u32 *AlarmBits );
u8 get_driverecord_datalen(ProtocolHandle *pprotHandle, u8 commandID);
u8 ReadLbsCfgPara(Enum_CtrlCfgPara CfgPara,u8 *pValue,u8 *buflen); //监控平台参数读函数 peijl_20121016 添加
u8 WriteLbsCfgPara(Enum_CtrlCfgPara CfgPara, u8 *pValue,u8 len,UpdateModeEnum UpdateMode); //监控平台参数读函数

//extern oa_bool term_reg_enpacket(oa_uint8 *buf, oa_uint16 *p_len);
typedef enum
{
	wireless_update = 0x0,
	conn_to_specified_server,
	term_powerdown,
	term_reset,
	factory_setting,
	turnoff_datatraffic,
	turnoff_allwireless,
	none = 0xf,
}dev_control_type;

typedef enum
{
	dev_active,//设备主动发起
	plat_active,//平台主动发起
}dev_plat_active;

typedef struct
{
	oa_uint8 ip[SERVER_IP_MAX_LEN];
	oa_uint16 port;
	oa_uint8 port_str[SERVER_IP_MAX_LEN];
	oa_uint8 usr[SERVER_IP_MAX_LEN];
	oa_uint8 pw[SERVER_IP_MAX_LEN];
	oa_uint8 fw[SERVER_IP_MAX_LEN];
}upgrade_paras;
 #endif
