#ifndef _SCHEDUL_SCREEN_H
#define _SCHEDUL_SCREEN_H

#include "oa_gps.h"
#include "oa_type.h"
//��������
#define SCREEN_HD5	1  //D5������
#if (SCREEN_HD5==0)
	#define SCREEN_N990	0 //������N990
#endif

/*Ӳ���������*/
/*--------N990Э��--------*/
#define SchedulN990_Baud 9600 //������

#if SCREEN_N990>0
	#define Schedul_Baud 9600 //������
/*--------N990Э��--------*/
//�����ն�
	#define N990_ONEBYTE_CMD	  0x01 //���ֽ�ָ��
		#define HEARTBEAT_ACK_CMD 0x01 //����Ӧ��
		#define CALL_HANGON_CMD 0x02 //�ӵ绰
		#define CALL_HANGOFF_CMD 0x03 //�һ�
		#define GET_SMS_FORMAT_CMD 0x04 //��ѯ���Ÿ�ʽ
		#define CALL_CENTER_CMD 0x06 //��������
		#define SETSPK_UP_CMD	 0x09 //������
		#define SETSPK_DOWN_CMD	 0x0A //������
		#define GET_SPKVAL_CMD	 0xB0 //��ѯ����
	#define N990_SENDSMS_CMD	 0xA4 //���Ͷ���
	#define CALL_CAR_RSP_CMD   0x26 //����
	#define PICKUP_PASSEGER_CMD 0x27 //�ϳ�
	#define PICKUP_PASSEGER_FAILED_CMD 0x28 //ȡ��������δ�ӵ���
	#define REACH_DESTI_CMD     0x29 //�³�
	
	//�նˡ���
		#define HEART_BEAT_CMD 0x0D
		#define CALL_CAR_CMD 0x23      //��ʼ����
		#define CALL_CAR_INFO_CMD 0x24 //��ϸ����
		#define CALL_CAR_FAIL_CMD 0x25 //����ʧ��
		#define TEXT_INFO_CMD	0xA5   //�ı�����
		#define TEXT_INFO_CMD	0xA5   //�ı�����
		#define N990_RCVSMS_CMD	0xA6   //�ն���
			
	typedef struct _sms_data_result_//��������
	{
		u8 TelNum[21];
		u8 SMSDat[150];
		u8 SMSLen;
	}STRUCT_SMS_DATA;
	typedef union _result_data_
	{
		u8 SUB_CMD_TYPE; //���ֽ�ָ���sub������
		u8 IDLen;
		u8 ID[10];
		u8 failmsgLen;
		u8 FailedMsg[30]; //�˻ص���ԭ��
		STRUCT_SMS_DATA SMS_Data;//���յ��Ķ�������
	//	u8 value;   //���ֽڽ��,������,���õĿ���,���ֽڲ���ֵ��
	//	u16 value16;	 //˫�ֽڽ��
	//	u8 telnum[33];//�������
	//	STRUCT_IP_PORT IP_Port;//���õ�IP ��ַ�Ͷ˿ں� 
	//	#if DRIVE_SIGN_EN
	//	STRUCT_DRIVER_INF driver_inf;//˾����¼����
	//	#endif
	//	#if CUSTOM_CMD
	//	STRUCT_JTT_BUF JTT_MSG	;//���jtt808Э��
	//	#endif
	//	STRUCT_UP_DATA_PARA UP_DATA_PARA;
	//	STRUCT_UP_DATA UP_DATA_STORE;
	//#if PROTCL_HX54
	//	STRUCT_STATION_DATA STATION_DATA;//վ����Ϣ
	//#endif
	}SScrn_result_data;
	
	typedef struct _SScrn_result_
	{
	 u8 CMD_TYPE;  //��ǰ�������������
	 SScrn_result_data result;      //����洢
	}STRUCT_SScrn_Result;

	typedef enum
	{
		TEL_NUM_ENUM=0,
		SMS_DAT_ENUM
	}SMS_DATA_ENUM;
#else
	#define Schedul_Baud 19200

/*--------D5Э��--------*/
//������������������
	#define CENTER_SMS_CMD  0x01

	typedef struct _sms_data_result_//��������
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
/*�豸����*/
#define HAND_DEVICE_OR_SCRN	 1 /*����Ϊ�����������ֱ�,0Ϊ�ֱ�1Ϊ������*/

/*��������*/
#define CUSTOM_CMD 1//�����Զ���������
#define SSCRN_PRINT_EN 1//���Դ�ӡ����
#define DRIVE_SIGN_EN 1 //����˾��ˢ����¼

/*���к�������*/
#define EN_QUEUE_DELDATA 1

//#if SSCRN_PRINT_EN
//#define SSCRN_Print(arg...) Trace(PrintDevDebug,##arg) //peijl_test
//#else
//#define SSCRN_Print(arg...)
//#endif

#define Normal_Ack	0x01
#define Operate_Ack	0x02
/*--------D5Э��--------*/
//������������������
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
/*����ָ��  PC�·�*/
#define UPDATA_REQ_CMD  0x50   //��������
#define UPDATA_PARA_CMD  0x51  //�·�����������
#define UPDATA_DATA_CMD  0x52  //�·���������
/*����ָ�� END*/
//#if PROTCL_HX54
	/*վ�����ݵ��� PC�·�*/
	#define STATION_DOWNLOAD_CMD  0x53
	/*վ�������������� */
	#define STATION_DOWNLOAD_START_CMD	0x54
//#endif

/*ѧ��ˢ������ģ���·�ָ��*/
#define SCR_SMS_DOWNLOAD_CMD	0x55


//������ ������������
#define UP_STATUS_CMD	0XA0	//״̬�ϱ�ָ����
#define HOST_CenSMS_CMD	0xA1	//�������Ķ���
#define CALL_IN_CMD		0xA5	//����֪ͨ
#define HAND_OFF_CMD	0xA6	//�һ�֪ͨ
#define HAND_ON_CMD		0xA7	//�ӵ绰֪ͨ
#define TELSMS_PDU_CMD	0xAF	//PDU����
#define TELSMS_TXT_CMD	0xB0	//txt����
#define GROUP_NUM_SEND_CMD	0xB4	//���ؼ��ź���
#define GROUP_NUM_CLR_CMD	0xB5	//��ռ��ź���
#define SMS_SEND_OK_CMD	0xA4	//txt����

#define SMS_SEND_FAILD_CMD	0xA3	//txt����
/*����ָ��  �ظ�PC*/
#define UPDATA_VERSION_UP_CMD  0xd0	  //�ϴ���ǰ�汾
#define UPDATA_REQ_DATA_CMD    0xd1	  // ����һ������
/*����ָ�� END*/
//#if PROTCL_HX54
	/*վ�����ݵ��� PC�·�*/
	#define STATION_DOWNLOAD_RSP_CMD  0Xd2
	/*վ�����ݵ��뿪ʼ����Ӧ�� */
	#define STATION_DOWNLOAD_START_RSP_CMD  0Xd4
//#endif
/*ѧ��ˢ������ģ���·�ָ��*/
#define SCR_SMS_DOWNLOAD_RSP_CMD	0xd5

/*�������ؽ��*/
#define SCCRN_TIME_OUT 0xFC	   //�������ȴ���ʱ
#define SCCRN_ERROR_RETURN 0xFD	//����
#define PARA_ERROR_CMD	0xFE //�ж��Զ��������ֲ�������
#define SCCRN_UNEXP_CMD	0xFF //�յ�δ���ж���������



#if DRIVE_SIGN_EN
typedef enum
{
	SIGN_IN=0,
	SIGN_OUT
}DRIVER_SIGN_ENUM;
typedef struct _Driver_Info //˾����¼�ṹ��
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
typedef struct _ip_port_result_//����IP��PORT
{
	u8 Ip[16];
	u16 port;
}STRUCT_IP_PORT;
typedef enum
{
	IP_ENUM=0,
	PORT_ENUM
}IP_PORT_ENUM;


typedef struct _up_data_para_//�������ݲ���, ���� �ܳ�
{
	u32 PacketTotalNum;
	u32 DataSize;
}STRUCT_UP_DATA_PARA;

typedef enum
{
	PACKETS_ENUM=0,
	DATA_SIZE_ENUM
}UP_DATA_PARA_ENUM;


typedef struct _up_data_//������������
{
	u8 datalen;
	u8 UP_DATA[250];
}STRUCT_UP_DATA;

typedef struct //վ����������
{
	u8 ROAD_NUM[4];	  //4�ֽ���·��
	u8 STA_DIRECTION; //���з���
	u8 STA_TOTAL_NUM; //վ������
	u8 ROAD_VER;	  //�汾
	u8 STA_DATALEN;	  //վ�����ݳ���	
	u8 STA_DATA[220]; //STA_NUM���10,10*STAT_INFO_SIZE
}STRUCT_STATION_DATA;

typedef struct //վ����������
{
	u8 STATE;  //ģ����Ϣ��Ӧ���
	u8 F_OR_A; //ģ����Ϣǰ��׺
	u8 SMS_LEN;//ģ����Ϣ���ݳ���
	u8 SMS_DATA[128]; //ģ������
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
typedef struct _JTT_808_BUF_//���808Э��ṹ��
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
	u8 HostType;/* �������� λb0��1 GPRS��ͨ��0 GPRSû��ͨ
									λb2λb1�������ͣ�00  GPRS�͡�01 GSM/GPRS�͡�10����
									λb6λb5λb4�������ͣ� 000 ���������С�001 �й��ƶ���010 �й���ͨ��110 �����ԭ�ַ�
									λb7 1 ����*/
	u8 OverSpeed;//��������
	u8 CSQ;		 //�ź�ǿ�� ��Χ:0x00-0x1f
	u8 ACC_State; //ACC״̬: 0:�ѵ�� 1δ���
	u8 DriveState;	//����״̬
	u8 TCP_UDP;		//'1' ��ʾ��ǰΪTCP���ӣ�'0'��ʾUDP����
}STRUCT_HOST_STATE;

#if SCREEN_HD5>0
	typedef union _result_data_
	{
		STRUCT_SMS_DATA SMS_Data;//���յ��Ķ�������
		u8 value;   //���ֽڽ��,������,���õĿ���,���ֽڲ���ֵ��
		u16 value16;	 //˫�ֽڽ��
		u8 telnum[33];//�������
		STRUCT_IP_PORT IP_Port;//���õ�IP ��ַ�Ͷ˿ں� 
		#if DRIVE_SIGN_EN
		STRUCT_DRIVER_INF driver_inf;//˾����¼����
		#endif
		#if CUSTOM_CMD
		STRUCT_JTT_BUF JTT_MSG	;//���jtt808Э��
		#endif
		STRUCT_UP_DATA_PARA UP_DATA_PARA;
		STRUCT_UP_DATA UP_DATA_STORE;
		STRUCT_STATION_DATA STATION_DATA;//վ����Ϣ
		STRUCT_SCR_SMS_TEMPLET SCR_SMS_TEMPLET;
	}SScrn_result_data;
	typedef struct _SScrn_result_
	{
	 u8 CMD_TYPE;  //��ǰ�������������
	 SScrn_result_data result;      //����洢
	}STRUCT_SScrn_Result;
#endif

#if 0
/*��������,���ⲿ����*/
void SScrn_HW_Init(USART_TypeDef *COM);//Ӳ����س�ʼ��
void SScrn_SW_Init(void);//�����س�ʼ��
u8 SScrn_DataAnaly(void);  /*���������ݽ���*/
u8 SScrn_GetResult(u8 *Buf,u8 BufSize,u8 *ReadSize,u8 Cmd,u8 SubField);/*��ȡ�ӵ���������������*/
u8 SScrn_Result_Send(u8 result); /*���ʹ�����*/
u8 SScrn_CmdResult_Send(u8 Cmd,u8 Ackmode,u8 result);
u8 SScrn_CenterSMS_Send(u8* Str,u16 len);/*��������������Ķ���*/
u8 SScrn_CallIn_Send(u8 *telnum,u8 tellen);/*������������е绰����,�ϴ�����*/
u8 SScrn_HandOff_Send(void);/*����������� �绰�Ҷ�(�����)*/
u8 SScrn_HandOn_Send(void);/*����������� �绰������(�����)*/
//u8 SScrn_UniSMS_Send(u8 *Tel,u8 Telen,u8 *Time,u8* SMSStr,u8 SMSlen);/*����������� unicode��ʽ����*/
//u8 SScrn_AscSMS_Send(u8 *Tel,u8 Telen,u8 *Time,u8* SMSStr,u8 SMSlen);/*�����������TXT����*/
u8 SScrn_SMS_Send(u8 cmd,u8 *Tel,u8 Tellen,u8 *Time,u8 *SMS,u8 smslen);//���ŷ���
u8 SScrn_SMSResult_Send(u8 result);	/*����������Ͷ��ŷ��ͽ��*/
u8 SScrn_GroupNum_Send(u8* Name,u8 NameLen,u8* TelNum,u8 TelLen);/*���ź�������*/
u8 SScrn_GroupNum_Clr(void);/*��ռ��ź���*/
u8 SScrn_Status_Send(STRUCT_RMC *pGpsInfo,STRUCT_HOST_STATE * Host_State);/*�����������״̬*/

u8 SScrn_Version_Send(u8* VersionStr,u8 StrLen);/*��PC���͵�ǰ�汾��*/
u8 SScrn_Data_Req(u32 PacketNum); /*��������*/
#endif
#if PROTCL_HX54
u8 SScrn_StationData_Rsq(void);
u8 SScrn_GetStationData(u8 *RoadNum,u8 *Direction,u8 * total_num,u8 *sta_num,u8 *sta_type,FP32 * LAT,FP32 *LON);
#endif
#endif
