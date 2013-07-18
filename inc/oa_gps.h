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
 *   oa_gps.h
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
#ifndef __OA_GPS_H__
#define __OA_GPS_H__

#include "oa_type.h"
#include "oa_api.h"

#define U_BLOX				//gps 模块
#define GPS_ERR 1
#define GPS_OK 0
#define OA_GPS_VERSION_NO "u-blox"
#define OA_GPS_SCHEDULER_PERIOD 2000//500 //ms
#define OA_GPS_SCHEDULER_1ST 20000
#define OA_MAIN_SCHEDULER_PERIOD 2000 //ms
#define GPS_RUN_SECONDS (OA_GPS_SCHEDULER_PERIOD/1000)
#define BASIC_DRIVETIME 30 * 60//half an hour
#define GPS_COM OA_UART2
#define MILEAGE_FILE L"total_miles.ini"
#define MAX_SPEED 220//220km/h
typedef struct
{
	u32 total_mileage;
}TOTAL_MILE;

typedef struct
{
	oa_uint8 buffer[OA_UART_BUFFER_SIZE];
	oa_uint16 data_len;
	oa_bool has_new_data;
}gps_data_struct;

extern void oa_app_gps(void);
extern void oa_app_gps_init(void);


//===========================从LBS上移植而来==============================
//#ifndef _GPS_H_
//#define _GPS_H_

//#include <os_cpu.h>
//#include "stm32f10x.h"
/*GPS硬件模块定义*/
#define UBLOX
//#define NO_UBLOX
/*GPS 调试打印开关*/
#define GPS_PRINT 1		/*0关闭  1开启*/
/*GPS 路程计算开关*/
#define GPS_DISTANCE_EN 1	/*0关闭  1开启*/

#define GPS_DMA_RCV_BUF_SIZE 256   /*DMA接收缓存大小*/

/*无数据计时  用来判定模块损坏*/
#define MAX_NO_DATA_TIME 15
/*允许的最小速度*/
#define Active_Speed 5
/*最小累计距离*/
#define MinDistance (0.1)
/*最小距离计算盲点时间长度*/
#define DISTANCE_FRQ 15
/*返回值位段定义*/
#define NMEA_CHECK_ERROR    (0x01<<0)	/*NMEA数据校验失败*/
#define UBX_CHECK_ERROR 	(0x01<<1)   /*UBX数据校验失败*/
#define NMEA_GGA_OK 		(0x01<<2)	/*解析GPGGA字段成功,且已定位*/
#define NMEA_GGA_UNFIXED 	(0x01<<3)	/*解析GPGGA字段成功,未定位*/

#define NMEA_RMC_OK 		(0x01<<4)	/*解析GPRMC字段成功,且已定位*/
#define NMEA_RMC_UNFIXED 	(0x01<<5)	/*解析GPRMC字段成功,未定位*/
#define UBX_CFG_RST_OK 		(0x01<<6)	/*模块复位成功*/
#define UBX_CFG_RST_ERROR 	(0x01<<7)	/*模块复位失败*/
#define UBX_CFG_MSG_OK 		(0x01<<8)	/*模块设置消息段成功*/
#define UBX_CFG_MSG_ERROR 	(0x01<<9)	/*模块设置消息段失败*/
#define UBX_NAV_MSG_OK 		(0x01<<10)	/*模块nav段设置成功*/
#define UBX_NAV_MSG_ERROR 	(0x01<<11)	/*模块nav段设置失败*/

#define HW_ANT_OK			(0x01<<12)	/*硬件检测天线正常*/
#define HW_ANT_OPEN			(0x01<<13)	/*硬件检测天线开路*/

#define GPS_NO_DATA 		(0x01<<31)  /*未收到数据*/

//#define NMEA_DATA_NULITY 	(0x01<<2)   /*NMEA数据无效*/
//#define UBX_DATA_NULITY 	(0x01<<3)   /*UBX数据无效*/

//#define UBX_CFG_ANT_OK 		(0x01<<8)	/*模块天线检测设置成功(未使用)*/
//#define UBX_CFG_ANT_ERROR 	(0x01<<9)	/*模块天线检测设置失败(未使用)*/
//#define UBX_MON_ANT_OK 		(0x01<<10)	/*模块获取天线状态成功(未使用)*/
//#define UBX_MON_ANT_ERROR 	(0x01<<11)	/*模块获取天线状态失败(未使用)*/

//#define NMEA_TXT_OK 		(0x01<<14)	/*解析GPTXT字段成功*/


/*GPS定位状态值*/
typedef enum _GPS_FIX_STATE
{
	GPS_FIXED=1,	/*定位正常*/
	GPS_UNFIXED,	/*定位异常*/
}GPS_FIX_STATE;

/*GPS天线状态值*/
typedef enum _GPS_ANT_STATE
{
	GPS_ANT_OK=1,	/*天线状态正常*/
	GPS_ANT_OPEN,	/*天线状态开路*/
}GPS_ANT_STATE;


//GPS数据信息记录
typedef struct _RMC_INF
{
	GPS_FIX_STATE 		Fix_Status;/*定位状态*/
	u8 					Latitude[8];/*纬度 百万分一 格式:ddmm.mmmm*/
	u8 					Longitude[9];/*经度 百万分一 格式:dddmm.mmmm*/
	u8 					East_Indicator;/*东西经 ASC（E/W）*/
	u8 					North_Indicator;/*南北纬 ASC（N/S）*/
	u8 					Time[6];/*时间 BCD码表示*/
	GPS_ANT_STATE 		Ant_Status;/*天线状态,未使用*/
	s16 				Height; /*高度(m)*/
	u16 				COG;/*地面方向 以真北为0,0~360*/
	u16 				Speed;/*速度 km/h*/
}STRUCT_RMC;
/*GPS数据分类索引*/
typedef enum _RMC_Field
{
	GPSFixedStatus=0,
	GPSLat,
	GPSNInd,
	GPSLon,
	GPSEInd,
	GPSCog,
	GPSTime,
	GPSAnt,
	GPSHeght,
	GPSSpeed, 
}RMC_FField;
/*数据获取格式*/
typedef enum _Filed_Format
{
	HEX_Code=0,
	BCD_Code,
	ASC_Code,
	Float_Code,
}FiledFormat;

/*更新模式*/
enum UpdateModeType
{
	ALLUPDATE=0,   /*全部更新*/
	ACCOFFUPDATE,  /*ACC关闭时只更新时间*/
	ANTUPDATE,	   /*只更新天线状态*/
	UNFIXEDUPDATE, /*未定位状态  更新*/
	HEIGHTUPDATE,  /*更新海拔高度*/
	ALLCLRUPDATE,  /*全部清空*/
};

typedef struct 	GPS_Handle_Def
{
	STRUCT_RMC  GPS_INF;
	FP32    New_Lon;	/*新纬度*/
	FP32    New_Lat;	/*新经度*/
	FP32    Old_Lon;	/*上一点纬度*/
	FP32    Old_Lat;	/*上一点经度*/
	FP32    Now_Speed;	/*当前速度 计算一次距离*/
	FP32    Last_Speed;	/*前一次速度 计算一次距离*/
	u32 	RtcNowCnt;  /*RTC数据*/
	u8		Oldcog;			//拐点计算,上一点偏角度
	u8  	COGcnt;			//拐点计算用
	s32		Fixed_Cnt;		/*定位时间计数*/
	u32		Unfixed_Cnt;		/*未定位时间计数*/
	FP32    Speed_Distance;	    /*速度计算时的距离 KM*/
//	FP32	Speed_Tatol;		/*计算速度和 KM*/
	u32 	RtcStartTime;			/*RTC计数起始时间*/
	u32 	Speed_Temp;		/*任务解析 速度中间存储、COG中间存储和NMEA校验*/
	u16 	CntTemp;			/*计算数据个数*/
	u16 	Msg_Data_Len;	    /*任务解析 x消息中有效数据长度*/
	u16 	DataStr_Len;				/*总共需要处理的数据长度*/
	u8 		Step;			 	/*任务解析处理步骤*/
	u8 		dot;				/*任务解析 计算逗号个数*/
	u8 		CHK_A;				/*任务解析 计算checkA*/
	u8 		CHK_B;			    /*任务解析 计算checkA*/
	u8 		Data;						/*待处理的一个字节数据*/
	u8 		Pass_Time[6];			/*上次的时间*/
	u8 		SendCnt;
	u8 		GGAFIXFlag;		/*GPGGA是否定位标示*/ 
	u8		GGA_P_N_Flag;   /*高度值正负标志位*/	 
	u8 		Status;
//	u8 		ModelStatus;	 //GPS模块状态
//	u8 		GPSAnalyFlag	;//是否已经正确解析一条语句
//	u32  	GPSDataType;//语句类型
}GPS_Handle;


/*CFG--RST命令相关*/
#define RST_CMD_LEN 12
#define Hot_Start 0x01//热启动
#define Warm_Start 0x02//暖启动
#define Cold_Start 0x03//冷启动
#define HardWare_Reset  0x00//硬件复位
#define SoftWare_Reset   0x01//软件复位
#define GPS_SoftWare_Reset 0x02//GPS软件复位
#define GPS_Stop 0x04//停止GPS
#define GPS_Start 0x05//启动GPS
/*CFG———PRT命令相关*/
#define PRT_CMD_LEN 28
#define GPS_USE_USART1   0x01
#define GPS_USE_USART2   0x02
#define NMEA_Proto  0x0002
#define UBX_Proto  0x0001
#define NMEA_UBX_Proto  0x0003
#define USART_Set 0x000008D0

/*CFG--MSG*/
#define MSG_CMD_LEN 11
#define NMEA_GP 0xF0
#define NMEA_PUBX 0xF1
#define GPGGA 0x00
#define GPGLL 0x01
#define GPGSA 0x02
#define GPGSV 0x03
#define GPRMC 0x04
#define GPVTG 0x05
#define GPGRS 0x06
#define GPGST 0x07
#define GPZDA 0x08
#define GPGBS 0x09
#define GPDTM 0x0A
#define GPGPQ 0x40
#define GPTXT 0x41
/*CFG--ANT*/
#define ANT_CMD_LEN 12
#define ANT_POLL_LEN 8



/*函数接口声明，供外部调用*/
u8 GPS_SW_Init(void); /*GPS模块初始化*/
u8 GPS_HW_Init(void);/*GPS模块通信接口硬件设置*/
u32 GPS_DataAnaly(void);/*GPS数据解析*/
#if	GPS_DISTANCE_EN
u32 GPS_GetDistance(void);/*获取累计的路程值,单位0.1KM*/
void GPS_DistanceAdd(void);/*累计计算路程*/
#endif
u8 GPS_GetAntaStatus(void);	/*获取GPS天线状态*/
u8 GetPosinfASC(u8 *Str,u8 StrSize,u8 *Strlen,u8 Filed);/*获取GPS数据字符串*/
u8 GetPosinf(u8 *Str,u8 Filed,u8 Mode);
u8 GPS_GetTime(u8 *TimeStr);/*获取GPS时间BCD码*/
u8 GPS_GetPosition(STRUCT_RMC *gps_info);/*获取GPS数据*/
u8 GPS_GetFixStatus(void);	 /*获取GPS定位状态*/

oa_double GPS2Point_Distance(oa_double New_Point_Lon,oa_double New_Point_Lat,oa_double Old_Point_Lon,oa_double Old_Pont_Lat );
//#endif
//------------------------------------------
#endif


