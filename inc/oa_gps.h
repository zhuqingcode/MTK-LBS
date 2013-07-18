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

#define U_BLOX				//gps ģ��
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


//===========================��LBS����ֲ����==============================
//#ifndef _GPS_H_
//#define _GPS_H_

//#include <os_cpu.h>
//#include "stm32f10x.h"
/*GPSӲ��ģ�鶨��*/
#define UBLOX
//#define NO_UBLOX
/*GPS ���Դ�ӡ����*/
#define GPS_PRINT 1		/*0�ر�  1����*/
/*GPS ·�̼��㿪��*/
#define GPS_DISTANCE_EN 1	/*0�ر�  1����*/

#define GPS_DMA_RCV_BUF_SIZE 256   /*DMA���ջ����С*/

/*�����ݼ�ʱ  �����ж�ģ����*/
#define MAX_NO_DATA_TIME 15
/*�������С�ٶ�*/
#define Active_Speed 5
/*��С�ۼƾ���*/
#define MinDistance (0.1)
/*��С�������ä��ʱ�䳤��*/
#define DISTANCE_FRQ 15
/*����ֵλ�ζ���*/
#define NMEA_CHECK_ERROR    (0x01<<0)	/*NMEA����У��ʧ��*/
#define UBX_CHECK_ERROR 	(0x01<<1)   /*UBX����У��ʧ��*/
#define NMEA_GGA_OK 		(0x01<<2)	/*����GPGGA�ֶγɹ�,���Ѷ�λ*/
#define NMEA_GGA_UNFIXED 	(0x01<<3)	/*����GPGGA�ֶγɹ�,δ��λ*/

#define NMEA_RMC_OK 		(0x01<<4)	/*����GPRMC�ֶγɹ�,���Ѷ�λ*/
#define NMEA_RMC_UNFIXED 	(0x01<<5)	/*����GPRMC�ֶγɹ�,δ��λ*/
#define UBX_CFG_RST_OK 		(0x01<<6)	/*ģ�鸴λ�ɹ�*/
#define UBX_CFG_RST_ERROR 	(0x01<<7)	/*ģ�鸴λʧ��*/
#define UBX_CFG_MSG_OK 		(0x01<<8)	/*ģ��������Ϣ�γɹ�*/
#define UBX_CFG_MSG_ERROR 	(0x01<<9)	/*ģ��������Ϣ��ʧ��*/
#define UBX_NAV_MSG_OK 		(0x01<<10)	/*ģ��nav�����óɹ�*/
#define UBX_NAV_MSG_ERROR 	(0x01<<11)	/*ģ��nav������ʧ��*/

#define HW_ANT_OK			(0x01<<12)	/*Ӳ�������������*/
#define HW_ANT_OPEN			(0x01<<13)	/*Ӳ��������߿�·*/

#define GPS_NO_DATA 		(0x01<<31)  /*δ�յ�����*/

//#define NMEA_DATA_NULITY 	(0x01<<2)   /*NMEA������Ч*/
//#define UBX_DATA_NULITY 	(0x01<<3)   /*UBX������Ч*/

//#define UBX_CFG_ANT_OK 		(0x01<<8)	/*ģ�����߼�����óɹ�(δʹ��)*/
//#define UBX_CFG_ANT_ERROR 	(0x01<<9)	/*ģ�����߼������ʧ��(δʹ��)*/
//#define UBX_MON_ANT_OK 		(0x01<<10)	/*ģ���ȡ����״̬�ɹ�(δʹ��)*/
//#define UBX_MON_ANT_ERROR 	(0x01<<11)	/*ģ���ȡ����״̬ʧ��(δʹ��)*/

//#define NMEA_TXT_OK 		(0x01<<14)	/*����GPTXT�ֶγɹ�*/


/*GPS��λ״ֵ̬*/
typedef enum _GPS_FIX_STATE
{
	GPS_FIXED=1,	/*��λ����*/
	GPS_UNFIXED,	/*��λ�쳣*/
}GPS_FIX_STATE;

/*GPS����״ֵ̬*/
typedef enum _GPS_ANT_STATE
{
	GPS_ANT_OK=1,	/*����״̬����*/
	GPS_ANT_OPEN,	/*����״̬��·*/
}GPS_ANT_STATE;


//GPS������Ϣ��¼
typedef struct _RMC_INF
{
	GPS_FIX_STATE 		Fix_Status;/*��λ״̬*/
	u8 					Latitude[8];/*γ�� �����һ ��ʽ:ddmm.mmmm*/
	u8 					Longitude[9];/*���� �����һ ��ʽ:dddmm.mmmm*/
	u8 					East_Indicator;/*������ ASC��E/W��*/
	u8 					North_Indicator;/*�ϱ�γ ASC��N/S��*/
	u8 					Time[6];/*ʱ�� BCD���ʾ*/
	GPS_ANT_STATE 		Ant_Status;/*����״̬,δʹ��*/
	s16 				Height; /*�߶�(m)*/
	u16 				COG;/*���淽�� ���汱Ϊ0,0~360*/
	u16 				Speed;/*�ٶ� km/h*/
}STRUCT_RMC;
/*GPS���ݷ�������*/
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
/*���ݻ�ȡ��ʽ*/
typedef enum _Filed_Format
{
	HEX_Code=0,
	BCD_Code,
	ASC_Code,
	Float_Code,
}FiledFormat;

/*����ģʽ*/
enum UpdateModeType
{
	ALLUPDATE=0,   /*ȫ������*/
	ACCOFFUPDATE,  /*ACC�ر�ʱֻ����ʱ��*/
	ANTUPDATE,	   /*ֻ��������״̬*/
	UNFIXEDUPDATE, /*δ��λ״̬  ����*/
	HEIGHTUPDATE,  /*���º��θ߶�*/
	ALLCLRUPDATE,  /*ȫ�����*/
};

typedef struct 	GPS_Handle_Def
{
	STRUCT_RMC  GPS_INF;
	FP32    New_Lon;	/*��γ��*/
	FP32    New_Lat;	/*�¾���*/
	FP32    Old_Lon;	/*��һ��γ��*/
	FP32    Old_Lat;	/*��һ�㾭��*/
	FP32    Now_Speed;	/*��ǰ�ٶ� ����һ�ξ���*/
	FP32    Last_Speed;	/*ǰһ���ٶ� ����һ�ξ���*/
	u32 	RtcNowCnt;  /*RTC����*/
	u8		Oldcog;			//�յ����,��һ��ƫ�Ƕ�
	u8  	COGcnt;			//�յ������
	s32		Fixed_Cnt;		/*��λʱ�����*/
	u32		Unfixed_Cnt;		/*δ��λʱ�����*/
	FP32    Speed_Distance;	    /*�ٶȼ���ʱ�ľ��� KM*/
//	FP32	Speed_Tatol;		/*�����ٶȺ� KM*/
	u32 	RtcStartTime;			/*RTC������ʼʱ��*/
	u32 	Speed_Temp;		/*������� �ٶ��м�洢��COG�м�洢��NMEAУ��*/
	u16 	CntTemp;			/*�������ݸ���*/
	u16 	Msg_Data_Len;	    /*������� x��Ϣ����Ч���ݳ���*/
	u16 	DataStr_Len;				/*�ܹ���Ҫ��������ݳ���*/
	u8 		Step;			 	/*�������������*/
	u8 		dot;				/*������� ���㶺�Ÿ���*/
	u8 		CHK_A;				/*������� ����checkA*/
	u8 		CHK_B;			    /*������� ����checkA*/
	u8 		Data;						/*�������һ���ֽ�����*/
	u8 		Pass_Time[6];			/*�ϴε�ʱ��*/
	u8 		SendCnt;
	u8 		GGAFIXFlag;		/*GPGGA�Ƿ�λ��ʾ*/ 
	u8		GGA_P_N_Flag;   /*�߶�ֵ������־λ*/	 
	u8 		Status;
//	u8 		ModelStatus;	 //GPSģ��״̬
//	u8 		GPSAnalyFlag	;//�Ƿ��Ѿ���ȷ����һ�����
//	u32  	GPSDataType;//�������
}GPS_Handle;


/*CFG--RST�������*/
#define RST_CMD_LEN 12
#define Hot_Start 0x01//������
#define Warm_Start 0x02//ů����
#define Cold_Start 0x03//������
#define HardWare_Reset  0x00//Ӳ����λ
#define SoftWare_Reset   0x01//�����λ
#define GPS_SoftWare_Reset 0x02//GPS�����λ
#define GPS_Stop 0x04//ֹͣGPS
#define GPS_Start 0x05//����GPS
/*CFG������PRT�������*/
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



/*�����ӿ����������ⲿ����*/
u8 GPS_SW_Init(void); /*GPSģ���ʼ��*/
u8 GPS_HW_Init(void);/*GPSģ��ͨ�Žӿ�Ӳ������*/
u32 GPS_DataAnaly(void);/*GPS���ݽ���*/
#if	GPS_DISTANCE_EN
u32 GPS_GetDistance(void);/*��ȡ�ۼƵ�·��ֵ,��λ0.1KM*/
void GPS_DistanceAdd(void);/*�ۼƼ���·��*/
#endif
u8 GPS_GetAntaStatus(void);	/*��ȡGPS����״̬*/
u8 GetPosinfASC(u8 *Str,u8 StrSize,u8 *Strlen,u8 Filed);/*��ȡGPS�����ַ���*/
u8 GetPosinf(u8 *Str,u8 Filed,u8 Mode);
u8 GPS_GetTime(u8 *TimeStr);/*��ȡGPSʱ��BCD��*/
u8 GPS_GetPosition(STRUCT_RMC *gps_info);/*��ȡGPS����*/
u8 GPS_GetFixStatus(void);	 /*��ȡGPS��λ״̬*/

oa_double GPS2Point_Distance(oa_double New_Point_Lon,oa_double New_Point_Lat,oa_double Old_Point_Lon,oa_double Old_Pont_Lat );
//#endif
//------------------------------------------
#endif


