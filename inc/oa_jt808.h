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
#define MAXPPPUSERLEN 127 /*���PPP�����û����ַ�����*/
#define MAXPPPPWDLEN 127  /*���PPP���������ַ�����*/




//�����������
typedef enum _UPDATE_MODE
{
	UpdateFlash,
	UpdateOnly
}UpdateModeEnum;
/*-----------------------------------------------------------------------------------
*808Э����ض���
-----------------------------------------------------------------------------------*/
#define MAX_MSG_LEN 1024
#define MAX_MSGBODY_LEN 900 //���ǵ��ַ�ת���<1024,������������Ϣ�ְ�ʱ��

#define AUTHENMAXLEN	100		//��Ȩ����󳤶ȶ���
#define CarIDMAXLEN		15		//���ƺ��
typedef enum
{
	ActionOK=0,	//�ɹ�
	ParaError, 
	//����
	ActionError,
	DevError,
	PPPLinkError,//����ʧ��
	PPPLinkOwned,//��ռ��
	UDPLinkError,
	UDPLinkOK,
	ServregErr,
	ServregOK,
	ServregoutErr,
	ServOK,
	ServErr,
	UpgradNewver, //�������������汾
	ComUpgrad, //com������.com����������������
	NoSMS,
	UnDefinedSms,
	UnknownError,
}HXSeverLinkRetrun;
//�ն�����ƽ̨ͨ�ŷ�ʽ����ӦeNet
typedef enum
{
	eNET_TCP=0,
	eNET_UDP=1,
	eNET_SMS=2
}NET_TYPE;
#if 0
/*------P808ͼ��ֱ���-------------------------------------------------------------*/
#define PROTCL_IMAGRES320_240	0x01	//320*240
#define PROTCL_IMAGRES640_480	0x02	//640*480,default
#define PROTCL_IMAGRES800_600	0x03	//800*600
#define PROTCL_IMAGRES1024_768	0x04	//1024*768
#define PROTCL_IMAGRES176_144	0x05	//176*144
#define PROTCL_IMAGRES352_288	0x06	//352*288
#define PROTCL_IMAGRES704_288	0x07	//704*288
#define PROTCL_IMAGRES704_576	0x08	//704*576
#endif
//λ����Ϣ�ϱ�����ö�� ��Ҫȷ��
#define ACC_Rpttype			0
#define ACCLogSta_Rpttype	1
//λ����Ϣ�ϱ�����ö��
#define Time_Stratgy		0
#define Diatanc_Stratgy		1
#define TimeDiatanc_Stratgy	2
/*------------λ����Ϣ�㱨�ı�־λ��״̬λ����--------------------------------------*/
/*------------λ����Ϣ�㱨�ı�־λ��״̬λ����--------------------------------------*/
typedef enum{
	StaAlarm0=0,   //��Ӧ32b������־
	StaSector1,	   //��Ӧ32b״̬λ
	StatusNum
}STA_ALARM;
//λ����Ϣ�㱨��Ϣ�б�����־λ����
#define ALARM_EMERGENCY_k		(1<<0)		//���������������������غ�������յ�Ӧ�����
#define ALARM_OVER_SPEED 		(1<<1)		//���ٱ�������־ά���������������
#define ALARM_DRIVE_TIRED		(1<<2)		//ƣ�ͼ�ʻ����־ά���������������
#define ALARM_PRE_k				(1<<3)		//Ԥ����
#define ALARM_GNSS_ERR			(1<<4)		//GNSSģ�鷢������
#define ALARM_GNSS_ANTENNA		(1<<5)		//GNSSģ������δ�ӻ򱻼���
#define ALARM_GNSS_SHORT_CIRCUIT (1<<6)		//GNSSģ�����߶�·
#define ALARM_POWER_UNDERVOLTAGE (1<<7)		//�ն�����ԴǷѹ
#define ALARM_POWER_BROWN		(1<<8)		//�ն�����Դ����
#define ALARM_DISPLAY_ERR		(1<<9)		//�ն�LCD ����ʾ������
#define ALARM_TTS_ERR			(1<<10)		//TISģ�����
#define ALARM_CAMERA_ERR 		(1<<11)		//����ͷ����
#define ALARM_DRIVE_OVERTIME 	(1<<18)		//�����ۼƼ�ʻ��ʱ
#define ALARM_OVERTIME_PARKING	(1<<19)		//��ʱͣ��
#define ALARM_ENTER_AREA		(1<<20)		//��������
#define ALARM_TOUCH_LINE_k		(1<<21)		//����·��
#define ALARM_DRIVE_SHORT_LONG_k (1<<22)	//·����ʻʱ�䲻��/����
#define ALARM_DRIVE_DEVIATE		(1<<23)		//·��ƫ�뱨��
#define ALARM_VSS_ERR			(1<<24)		//����VSS���� ��
#define ALARM_OIL_ERR			(1<<25)		//���������쳣
#define ALARM_STEAL				(1<<26)		//����������ͨ������������
#define ALARM_START_ILLEGAL_k	(1<<27)		//�����Ƿ����
#define ALARM_MOVE_ILLEGAL_k 	(1<<28)		//�����Ƿ��ƶ�
#define ALARM_ROLLOVER			(1<<29)		//�����෭����								
//λ����Ϣ�㱨��Ϣ��״̬λ����
#define STA_ACC_ON				(1<<0)		//ACC 0�أ�1��
#define STA_GPS_FIXED			(1<<1)		//0:δ��λ��1:��λ
#define STA_LAT_SOUTH 			(1<<2)		//0��γ��1��γ
#define STA_LON_WEAST			(1<<3)		//0������1����
#define STA_IN_BUSINESS			(1<<4)		//0��Ӫ״̬��1��Ӫ״̬
#define STA_GPS_ENCRYPT			(1<<5)		//0��γ��δ�����ܲ�����ܣ�1����
#define STA_OIL_BREAK			(1<<10)		//0������·������1������·�Ͽ�
#define STA_CIRCUIT_CUT			(1<<11)		//0������·������1������·�Ͽ�
#define STA_CAR_LOCK			(1<<12)		//0���Ž�����1���ż���
//#define STA_CLKSENSOR_WHEEL		(1<<6)	//
#define STA_SENSOR_FULLLOAD  	(1<<31)		//ж�ϴ����� 0ж�ϣ�1װ��
//�жϱ����Ƿ���Ч
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
	DEV_COMMON_rsp	=	0x0001,			//�ն�ͨ��Ӧ��
	PLAT_COMMON_rsp	=	0x8001,			//ƽ̨ͨ��Ӧ��
	HEART_BEAT		=	0x0002,			//����
	REGISTERS		=	0x0100,			//�ն�ע��
	REGISTERS_rsp	=	0x8100,			//�ն�ע��Ӧ��
	UNREGISTERS	=	0x0003,//0x0101,			//�ն�ע��
	LOGIN			=	0x0102,			//�ն˼�Ȩ
	SET_DEV_PARAM	=	0x8103,			//�����ն˲���
	GET_DEV_PARAM	=	0x8104,			//��ѯ�ն˲���
	GET_DEV_PARAM_rsp	=	0x0104,		//��ѯ�ն˲���Ӧ��
	DEV_CONTROL		=	0x8105,			//�ն˿���
	REPORT_LOCATION	=	0x0200,			//λ����Ϣ�㱨
	GET_LOCATION	=	0x8201,			//λ����Ϣ��ѯ
	GET_LOCATION_rsp=	0x0201,			//λ����Ϣ��ѯӦ��
	TRACK_LOCATION_CTL	=	0x8202,		//��ʱλ�ø��ٿ���
	ARTIFICIAL_ACK_ALARM = 0x8203,			//�˹�ȷ�ϱ���
	TEXT_DOWNLOAD	=	0x8300,			//�ı���Ϣ�·�
	SET_EVENT		=	0x8301,			//�¼�����
	REPORT_EVENT	=	0x0301,			//�¼�����
	ASK_DOWNLOAD	=	0x8302,			//�����·�
	ASK_ANSWER		=	0x0302,			//����Ӧ��
	DEMAND_MENU		=	0x8303,			//��Ϣ�㲥�˵�����
	DEMAND_SET_CANCEL	=	0x0303,		//��Ϣ�㲥/ȡ��
	DEMAND_SERVER	=	0x8304,			//��Ϣ����
	CALL_BACK		=	0x8400,			//�绰�ز�
	SET_PHONEBOOK	=	0x8401,			//���õ绰��
	CAR_CTL			=	0x8500,			//��������
	CAR_CTL_rsp		=	0x0500,			//��������Ӧ��
	SET_ROUND_AREA	=	0x8600,			//����Բ������
	DEL_ROUND_AREA	=	0x8601,			//ɾ��Բ������
	SET_SQUARE_AREA	=	0x8602, 		//���þ�������
	DEL_SQUARE_AREA	=	0x8603, 		//ɾ����������
	SET_POLYGON_AREA	=	0x8604, 	//���ö��������
	DEL_POLYGON_AREA	=	0x8605, 	//ɾ�����������
	SET_LINE		=	0x8606, 		//����·��
	DEL_LINE		=	0x8607, 		//ɾ��·��
	GET_DRIVE_DATA	=	0x8700,			//��ʻ��¼�����ݲɼ�����
	REPORT_DRIVE_DATA	=	0x0700,		//��ʻ��¼�������ϴ�
	DOWNLOAD_DRIVE_DATA	=	0x8701,		//��ʻ��¼�ǲ����´�����
	REPORT_BILL		=	0x0701,			//�����˵��ϱ�
	REPORT_DRIVER	=	0x0702,			//��ʻԱ�����Ϣ�ɼ��ϱ�
	REPORT_MEDIA_EVENT	=	0x0800,		//��ý���¼���Ϣ�ϴ�
	REPORT_MEDIA_DATA	=	0x0801,		//��ý�������ϴ�
	REPORT_MEDIA_DATA_rsp	=	0x8800,	//��ý�������ϴ�Ӧ��
	TAKE_PHOTO		=	0x8801,			//����ͷ������������
	SMEDIA_DATA_CHECK	=	0x8802,		//�洢��ý�����ݼ���
	SMEDIA_DATA_CHECK_rsp	=	0x0802,	//�洢��ý�����ݼ���Ӧ��
	REPORT_SMEDIA	=	0x8803,			//�洢��ý�������ϴ�
	START_RECORD	=	0x8804,			//¼����ʼ����
	DOWNLOAD_DATA	=	0x8900,			//��������͸��
	UPLOAD_DATA		=	0x0900,			//��������͸��
	UPLOAD_DATA_COMRESSION	=	0x0901,	//����ѹ���ϱ�
	RSA_PLAT		=	0x8a00,			//ƽ̨RSA��Կ
	RSA_DEV			=	0x0a00,			//�ն�RSA��Կ
	CALLCAR_ORDER	=	0x8B00,		    //�����·�
	ORDER_REQUEST	=	0x0B01,			//��������
	ORDER_ACCEPT	=	0x0B02,			//��������
	CALLCAR_ORDER_CONFIRM =	0x8B01,		//����ȷ�ϣ��������ɹ�
	ORDER_DONE		=	0x0B07,			//�������
	CALLCAR_ORDER_DEL	=	0x8B09,		//����ȡ��

	APTS_OPERATELINE_SET=	0x8C00,		//������Ӫ��·
	APTS_LINE_QUERY	=	0x8C01,			//�豸��������·�Ų�ѯ
	APTS_LINE_QUERY_rsp	=	0x0C01,		//��·�Ų�ѯӦ��
	APTS_INTOUT_STATION =0x0c02,		//����վ�ϱ�
	STADENTDATA_REPPORT =0x0c03,		//ѧ������У��ˢ��
//	RESERVE_PLAT							//ƽ̨������Ϣ����0X8F00-0X8FFF
//	RESERVE_DEV							//�ն�������Ϣ����0X0F00-0X0FFF	
};
//�ն��ϴ��������ж�
#define ISPrtclDevMsg(Devmsg) (DEV_COMMON_rsp==Devmsg ||HEART_BEAT==Devmsg ||REGISTERS==Devmsg ||UNREGISTERS==Devmsg \
 ||LOGIN==Devmsg ||GET_DEV_PARAM_rsp==Devmsg ||REPORT_LOCATION==Devmsg ||GET_LOCATION_rsp==Devmsg \
 ||REPORT_EVENT==Devmsg ||ASK_ANSWER==Devmsg ||DEMAND_SET_CANCEL==Devmsg ||CAR_CTL_rsp==Devmsg \
 ||REPORT_DRIVE_DATA==Devmsg ||REPORT_BILL==Devmsg ||REPORT_DRIVER==Devmsg ||REPORT_MEDIA_EVENT==Devmsg \
 ||REPORT_MEDIA_DATA==Devmsg ||SMEDIA_DATA_CHECK_rsp==Devmsg ||UPLOAD_DATA==Devmsg ||UPLOAD_DATA_COMRESSION==Devmsg \
 ||RSA_DEV==Devmsg||ORDER_REQUEST==Devmsg ||ORDER_ACCEPT==Devmsg ||ORDER_DONE==Devmsg ||APTS_LINE_QUERY_rsp==Devmsg ||APTS_INTOUT_STATION==Devmsg)
//ƽ̨�·��������ж�
#define ISPrtclServMsg(Servmsg) (PLAT_COMMON_rsp==Servmsg|| REGISTERS_rsp==Servmsg|| SET_DEV_PARAM==Servmsg	\
|| GET_DEV_PARAM==Servmsg ||DEV_CONTROL==Servmsg|| GET_LOCATION==Servmsg|| TRACK_LOCATION_CTL==Servmsg || ARTIFICIAL_ACK_ALARM ==  Servmsg\
|| TEXT_DOWNLOAD==Servmsg|| SET_EVENT==Servmsg || ASK_DOWNLOAD==Servmsg|| DEMAND_MENU==Servmsg \
|| DEMAND_SERVER==Servmsg|| CALL_BACK==Servmsg|| SET_PHONEBOOK==Servmsg|| CAR_CTL==Servmsg|| SET_ROUND_AREA==Servmsg \
|| DEL_ROUND_AREA==Servmsg|| SET_SQUARE_AREA==Servmsg|| DEL_SQUARE_AREA==Servmsg|| SET_POLYGON_AREA==Servmsg \
|| DEL_POLYGON_AREA==Servmsg|| SET_LINE==Servmsg|| DEL_LINE==Servmsg|| GET_DRIVE_DATA==Servmsg \
|| DOWNLOAD_DRIVE_DATA==Servmsg|| REPORT_MEDIA_DATA_rsp==Servmsg|| TAKE_PHOTO==Servmsg|| SMEDIA_DATA_CHECK==Servmsg \
|| REPORT_SMEDIA==Servmsg|| START_RECORD==Servmsg|| DOWNLOAD_DATA==Servmsg|| RSA_PLAT==Servmsg ||CALLCAR_ORDER==Servmsg \
||CALLCAR_ORDER_CONFIRM==Servmsg ||CALLCAR_ORDER_DEL==Servmsg ||APTS_OPERATELINE_SET==Servmsg ||APTS_LINE_QUERY==Servmsg)
/*----------------------��Ϣ��ṹ----------------------*/
//��Ϣ������
typedef struct _MSG_HEAD_bodyattr_s
{
	u16 MsgbodyLen:10;			//��Ϣ�����ԣ���Ϣ�峤��
	u16 DataEncrypt1:1;			//��Ϣ�����ԣ����ݼ��ܷ�ʽ
	u16 DataEncrypt2:1;
	u16 DataEncrypt3:1;
	u16 subpacket:1;			//��Ϣ�����ԣ��ְ�
	u16 reserve:2;				//��Ϣ�����ԣ�����
}MSG_HEAD_bodyattr;
//û�зְ�ʱ����Ϣͷ
typedef struct _MSG_HEAD
{
	u8 MsgId[2];				//��ϢID
	u8 MsgbodyAttr[2];//��Ϣ������
	u8 lclTel[6];				//������
	u8 SequenceId[2];			//��ˮ��
	//4B ��Ϣ����װ��
}MSG_HEAD;

//���зְ�ʱ�����ϢͷMSG_HEAD+	MSG_HEAD_sub
typedef struct _MSG_HEAD_sub
{
	u8 totalpacketNum[2];		//��Ϣ�ܰ���
	u8 SubpacketNo[2];			//�����,��1��ʼ
}MSG_HEAD_sub;
//ƽ̨ͨ��Ӧ��
typedef struct _PLAT_COMMON_rsp_s
{
	u8 SeqId[2];			//��Ӧ�ն���Ϣ����ˮ�� ��0��1
	u8 MsgId[2];			//��Ӧ�ն���Ϣ��ID
	u8 Rslt;				//0:�ɹ�/ȷ��,1.ʧ��,2.��Ϣ����,3.��֧��,4.��������ȷ��
}LBS_PlatComRsp;
//�ն�ע��
typedef struct _DEV_REGISTERS_s
{
	u8 ProvinceId[2];			//ʡ��ID
	u8 CityId[2];				//������ID
	u8 productor[5];			//�ն������̱���
	u8 devType[8];				//�ն��ͺţ�����ط��ո���
	u8 devId[7];				//�ն�ID���ɴ�д��ĸ���������
	u8 CarIdColor;				//������ɫ
	u8 CarId[8];				//����������
}LBS_DevRegreq;
//�ն�ע��Ӧ��
typedef struct _DEV_REGISTERS_rsp_s
{
	u16 SeqId;		//reg����ˮ��
	u8 Rslt;			//0:�ɹ�,1:������ע��2:�޸ó���3:�ն��Ѿ�ע��4:�޸��ն�
	//��Ȩ�����flash	
}LBS_DevRegRsp;
//�ն˲���tlv�б�
typedef struct _param_list_s
{
	u8 paramId[4];				//����ID
	u8 paramLen;				//��������
	u8 paramValue;				//����ֵ
}PARAM_LIST;
//��ѯ�ն˲���Ӧ��
typedef struct _GET_DEV_PARAM_rsp_s
{
	u8 RspId[2];			//Ӧ����ˮ��
//	u8 paramNum;			//Ӧ���������
//	PARAM_LIST paramlist��	//����ֵ	
}LBS_GetDevParamrsp;
//�ն˿���������
typedef enum _DEV_CONTROL_enum
{
	eOnlineUpgrad_Ctrl=1, //��������
	eSpclServ_Ctrl=2,     //����ƽ̨
	eDevPowoff_Ctrl=3,    //�豸�ػ�
	eDevReset_Ctrl=4,     //�豸����
	eDevFactoryset_Ctrl=5,//�豸��������
	eCloseDataCommu_Ctrl=6,//���ر�����ͨ�ŵ�����ҵ��
	eCloseWCMCommu_Ctrl=7 //�ر���������ͨ��
}enum_devCtrl_cmd;
//����λ����Ϣ
typedef struct _BASE_LOCATION_INFO_s
{
	u8 AlarmFlag[4];		//������־
	u8 StateBit[4];			//״̬λ��־
	u8 Latitude[4];			//γ��
	u8 Longitude[4];		//����
	u8 High[2];				//���θ߶� ��λm
	u8 Speed[2];			//�ٶ�,1/10km/h
	u8 Direction[2];		//����,0-359,˳���룬����
	u8 Time[6];
}BASE_LOCATION_INFO;
//λ�ø�����Ϣ���б�
typedef struct _ADD_LOCATION_INFO_s
{
	u8 AddInfoId;			//������ϢID	
	u8 AddInfoLen;			//������Ϣ����
//	addinfomsg;			//������Ϣ
}ADD_LOCATION_INFO;

//λ����Ϣ�㱨
typedef struct _REPORT_LOCATION_s
{
	BASE_LOCATION_INFO BaseLocationInfo;
//	ADD_LOCATION_INFO AddLocationInfo;
}LBS_ReportLocationreq;

#if 0
//λ����Ϣ��ѯ
typedef struct _GET_LOCATION_s
{
	//none
}LBS_GetLocationreq;
#endif
//808Э��������������
typedef struct _UPGRAD_PARA808_s
{
	u8 URL[2]; //��Ч
	u8 APN[10]; //?
	u8 User[MAXPPPUSERLEN+1];
	u8 Pwd[MAXPPPPWDLEN+1];
//	u8 IP[16];//zq
	u16 Tcpport;
	u16 Udpport;
	u8  Productor[5];
	u8  HW[3];
	u8  FW[2]; //��Ч
	u16 Limittime;
}LBS_OnlineupgraPara;
//λ����Ϣ��ѯӦ��
typedef struct _GET_LOCATION_rsp_s
{
	u8 sequenceId[2];				//Ӧ����ˮ��
	LBS_ReportLocationreq reportLocation;	//λ����Ϣ�㱨
}LBS_GetLocationrsp;
//��ʱλ�ø��ٿ���
typedef struct _TRACK_LOCATION_CTL_s
{
	u8 TimeInterval[2];				//ʱ��������λs��0��ֹͣ���٣���Ϊ0�����޺����ֶ�
	u8 TrackPeriod[4];				//������Ч�ڣ���λs��
}LBS_TrackLocationCtlreq;
//�ı���Ϣ�·�
typedef struct _TEXT_DOWNLOAD_s
{
	u8 flag;				//bit 0.1����1.����2.1:�ն���ʾ����ʾ3.1�ն�TTS����4.1:�������ʾ��5-7����
//	u8 TextInfo[];		//����GBK������1024B��
}LBS_TextDownloadreq;
//�¼������
typedef struct _EVENT_INFO_s
{
	u8 EventId;				//�¼���ID
	u8 EventLen;			//�¼����
//	u8 EventMsg	[];		//�¼������ݣ�����GBK����
}EVENT_INFO;
//�¼�����
typedef struct _SET_EVENT_s
{
	u8 EventType;				//��������0ɾ�������¼����󲻴��ֽ�1����2׷��3�޸�4ɾ���ض����¼������������
	u8 totalNum;				//��������
//	EVENT_INFO eventInfo[totalnum];				�¼������
}LBS_SetEventreq;
//�¼�����
typedef struct _REPORT_EVENT_s
{
	u8 EventId;				//�¼�ID

}LBS_ReportEventreq;
//��ѡ���б�
typedef struct _ANSWER_LIST_s
{
	u8 answerId;			//��ID
	u8 answerLen[2];			//��LEN
//	u8	AnswerString;		//������
}ANSWER_LIST;
//�����·�
typedef struct _ASK_DOWNLOAD_s
{
//	u8 flag;				//0.1:����1-2.����3.1:�ն�TTS����4.1:�������ʾ5-7����
//	u8 AskLen;				//���ⳤ��
//	u8 AskMsg;				//�������ݣ�����GBK����	
//	ANSWER_LIST AnswerList;			//��ѡ���б�
	u8 AnswerID[10];
}LBS_AskDownloadreq;
//����Ӧ��
typedef struct _ASK_ANSWER_rsp_s
{
	u8 sequenceId[2];		//Ӧ����ˮ��
	u8 AnswerId;			//��ID
}LBS_AskAnswerrsp;
//��Ϣ���б�
typedef struct _MENU_NUM_s
{
	u8 MenuType;			//��Ϣ������
	u8 MenuLen[2];			//��Ϣ���Ƴ���	
//	u8 MenuMsg[];			//��Ϣ���ƣ�����GBK����
}MENU_LIST;
//��Ϣ�㲥�˵�����
typedef struct _DEMAND_MENU_s
{
	u8 DemandType;				//��������
	u8 MenuNum;					//��Ϣ������
//	MENU_LIST	menuList;			//��Ϣ���б�	

}LBS_DemandMenureq;
//��Ϣ�㲥/ȡ��
typedef struct _DEMAND_SET_CANCEL_s
{
	u8 MenuType;			//��Ϣ����
	u8 flag;				//0ȡ�� 1�㲥
}LBS_DemandSetCancelreq;
//��Ϣ����
typedef struct _DEMAND_SERVER_s
{
	u8 MenuType;			//��Ϣ����
	u16 MenuLen;			//��Ϣ����
//	u8 MenuMsg[];			//��Ϣ���ݣ���GBK����
}LBS_DemandServreq;
//�绰����ϵ�������ݸ�ʽ
typedef struct _CONTACT_LIST_s
{
	u8 flag;			// 1����2���� 3����/����
	u8 TelLen;			//���볤��
//	u8 TelNum[];		//�绰����
//	u8 ContactLen;		//��ϵ�˳���
//	u8 ContactNum[];	//��ϵ�ˣ���GBK����
}CONTACT_LIST;
//���õ绰��
typedef struct _SET_PHONEBOOK_s
{
	u8 BookType;		//��������0ɾ���ն������д洢����ϵ��1����2׷��3�޸�
	u8 ContactNum;		//��ϵ������
//	CONTACT_LIST ContactList;		//��ϵ����	
}LBS_SetPhonebookreq;
//��������
typedef struct _CAR_CTL_s
{
	u8 CtlFlag;			//���Ʊ�־��bit0:0���Ž���1���ż�����bit1-7����
}LBS_CarCtlreq;
//��������Ӧ��
typedef struct _CAR_CTL_rsp_s
{
	u8 sequenceId[2];								//��ˮ��
}LBS_CarCtlrsp;
//Բ��������
typedef struct _ROUND_AREA_LIST_s
{
	u32 AreaId;					//����ID
	u16 AreaProperty;			//��������bit0.1:����ʱ��bit1.1:����bit2.1:�����򱨾�����ʻԱ
	u32 CenterLatitude;			//���ĵ�γ��
	u32 CenterLongitude;		//���ĵ㾭��
	u32 Radius;					//�뾶
	u8 StartTime[6];			//��ʼʱ��,����������0λΪ0�����޸��ֶ�
	u8 StopTime[6];				//����ʱ��,����������0λΪ0�����޸��ֶ�
	u16 HighSpeed;				//����ٶ�,����������1λΪ0�����޸��ֶ�
	u8 OverSpeedTime;			//���ٳ���ʱ�䣬����������1λΪ0�����޸��ֶ�
}ROUND_AREA_LIST;
//����Բ������
typedef struct _SET_ROUND_AREA_s
{
	u8 AreaType;				//��������0��������1׷������2�޸�����
	u8 AreaNum;					//��������
//	ROUND_AREA_LIST  AreaList;			//������
}LBS_SetRoundAreareq;
//ɾ��Բ������
typedef struct _DEL_ROUND_AREA_s
{
	u8 AreaNum;					//������
//	u32 AreaId1;
//	u32 AreaId2;......
}LBS_DelRoundAreareq;
//����������
typedef struct _SQUARE_AREA_LIST_s
{
	u32 AreaId;					//����ID
	u16 AreaProperty;			//��������bit0.1:����ʱ��bit1.1:����bit2.1:�����򱨾�����ʻԱ
	u32 LeftTopLatitude;			//���ϵ�γ��
	u32 LeftTopLongitude;			//���ϵ㾭��
	u32 RightBottomLatitude;		//���µ�γ��
	u32 RightBottomLongitude;		//���µ㾭��
	u8 StartTime[6];			//��ʼʱ��
	u8 StopTime[6];				//����ʱ��
	u16 HighSpeed;				//����ٶ�,����������1λΪ0�����޸��ֶ�
	u8 OverSpeedTime;			//���ٳ���ʱ�䣬����������1λΪ0�����޸��ֶ�
}SQUARE_AREA_LIST;
//���þ�������
typedef struct _SET_SQUARE_AREA_s
{
	u8 AreaType;				//��������0��������1׷������2�޸�����
	u8 AreaNum; 				//��������
//	SQUARE_AREA_LIST  AreaList;			//������
}LBS_SetSquareAreareq;

//ɾ����������
typedef struct _DEL_SQUARE_AREA_s
{
	u8 AreaNum; 				//������
//	u32 AreaId1;
//	u32 AreaId2;......
}LBS_DelSquareAreareq;

//������
typedef struct _TOP_LIST_s
{
	u32 topLatitude;			//����γ��
	u32 topLongitude;			//���㾭��
}TOP_LIST;
	
//���ö��������
typedef struct _SET_POLYGON_AREA_s
{
	u32 AreaId; 				//����ID
	u16 AreaProperty;			//��������bit0.1:����ʱ��bit1.1:����bit2.1:�����򱨾�����ʻԱ
	u8 StartTime[6];			//��ʼʱ��
	u8 StopTime[6]; 			//����ʱ��
	u16 HighSpeed;				//����ٶ�,����������1λΪ0�����޸��ֶ�
	u8 OverSpeedTime;			//���ٳ���ʱ�䣬����������1λΪ0�����޸��ֶ�
	u16 Areatop;				//���򶥵���
//	Top_LIST top_list[];			//������	
}LBS_SetPolygonAreareq;

//ɾ�����������
typedef struct _DEL_POLYGON_AREA_s
{
	u8 AreaNum; 				//������
//	u32 AreaId1;
//	u32 AreaId2;......
}LBS_DelPolygonAreareq;
//�յ���
typedef struct _TURNNING_LIST_s
{
	u32 TurnningId;			//�յ�ID
	u32 LineID;				//·��ID
	u32 TurnningLatitude;			//�յ�γ��
	u32 TurnningLongitude;			//�յ㾭��
	u8 LineWidth;				//·�ο��
	u8 LineProperty;			//·������
	u16 DriveMaxTime;			//·����ʻ������ֵ����λs����·������Ϊ0λΪ0����û�и��ֶ�
	u16 DriveMinTime;			//·����ʻ������ֵ����λs����·������Ϊ0λΪ0����û�и��ֶ�
	u16 HighSpeed;				//·������ٶȣ���λkm/h����λs����·������Ϊ1λΪ0����û�и��ֶ�
	u8 OverSpeed;				//·�γ��ٳ���ʱ�䣬��λs����λs����·������Ϊ1λΪ0����û�и��ֶ�
}TURNNING_LIST;
//����·��
typedef struct _SET_LINE_s
{
	u32 LineId; 				//����ID
	u16 LineProperty;			//��������bit0.1:����ʱ��bit1.1:����bit2.1:�����򱨾�����ʻԱ
	u8 StartTime[6];			//��ʼʱ��
	u8 StopTime[6]; 			//����ʱ��
	u16 TurnningNum;				//·���ܹյ���
//	TURNNING_LIST turn_list[];			//������	
}LBS_SetLinereq;

//ɾ����·
typedef struct _DEL_LINE_s
{
	u8 LineNum; 				//������0Ϊɾ��������·��
//	u32 AreaId1;
//	u32 AreaId2;......
}LBS_DelLinereq;
//��ʻ��¼���ݲɼ�����
typedef struct _GET_DRIVE_DATA_s
{
	u8 commandId;		//������
}LBS_GetDriveDatareq;

//��ʻ��¼�����ϴ�
typedef struct _REPORT_DRIVE_DATA_rsp_s
{
	u8 sequenceId[2];			//��ˮ��
	u8 commandId;			//������
//	u8 Datablock[];			//���ݿ�
}LBS_ReportDriveDatarsp;

//��ʻ��¼�����´�����
typedef struct _DOWNLOAD_DRIVE_DATA_s
{
	u8 commandId;			//������
//	u8 DataBlock[];			//���ݿ�
}LBS_DownloadDriveDatareq;

//��ʻԱ�����Ϣ�ɼ��ϱ�
typedef struct _REPORT_DRIVER_s
{
	u8 DriverNameLen;			//��ʻԱ��������
	u8 DriverName[12];			//��ʻԱ����
	u8 DriverID[20];				//���֤���
	u8 driverCertificateId[40];		//��ҵ�ʸ�֤����
	u8 OrgNameLen;				//��֤�������Ƴ���
	u8 OrgName[10];				//��ҵ�ʸ�֤��֤��������
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

//����ͷ���������¼������
typedef struct _TAKE_PHOTO_s
{
	u8 ChannelId;			//ͨ��ID
	u8 ShootCmd[2];			//0ֹͣ����0xFFFF��ʾ����������ʾ��������
	u8 ShootTime[2];		//���ռ������¼��ʱ�� s
	u8 SaveFlag;			//1.����0ʵʱ�ϴ�
	u8 Resolution;			//�ֱ���1.320*240     2.640*480	3.800*600	4.1024*768	5.176*144	6.352*288	7.704*288	8.704*576
	u8 Quality;				//1-10��1.������ʧ��С��10��ʾѹ�������
	u8 Brightness;			//����0-255
	u8 Contrast;			//�Աȶȣ�0-127
	u8 Saturation;			//���Ͷȣ�0-127
	u8 Color;				//ɫ�ȣ�0-255
}TakePhotoStrct;
//¼����ʼ����
typedef struct _START_RECORD_s
{
	u8 RecordCmd;				//¼������0ֹͣ¼�� ��1��ʼ¼��
	u8 RecordTime[2];			//¼��ʱ�䣬��λs��0��ʾһֱ¼��
	u8 StoreFlag;				//�����־��0ʵʱ�ϴ�1����
	u8 AudioSample;				//��Ƶ�����ʣ�0:8k, 1:11K, 2:23K, 3:32K, ��������
}LBS_StartRecordreq;
//��ý���¼���Ϣ�ϴ�
typedef struct _REPORT_MEDIA_EVENT_s
{
	u8 MediaId[4];				//��ý������ID ��ChannelId��Ӧ
	u8 MediaType;				//0:ͼ��1:��Ƶ2:��Ƶ
	u8 MediaCodeFormat;			//0.JPEG, 1.TIF,2.MP3,3.WAV,4.WMV,��������
	u8 EventCode;				//0ƽ̨�·�ָ��1��ʱ����2���ٱ�������3��ײ�෭������������������
	u8 ChannelId;				//ͨ��ID
}LBS_Mediainfo;

//��ý�������ϴ�Ӧ��
typedef struct _REPORT_MEDIA_DATA_RSP
{
	u8 MediaId[4];				//��ý��ID
	u8 RspPacketNum;			//�ش�������
	u16 RspPacketID[125];//�ش���ID�б�
}LBS_ReportMediaDatarsp;


//�洢��ý�����ݼ���
typedef struct _SMEDIA_DATA_CHECK_s
{
	u8 MediaType;				//0:ͼ��1:��Ƶ2:��Ƶ
	u8 ChannelId;				//0 ��ʾ������ý�����͵�����ͨ��
	u8 EventCode;				//0ƽ̨�·�ָ��1��ʱ����2���ٱ�������3��ײ�෭������������������
	u8 StartTime[6];			//��ʼʱ��YY-MM-DD-HH-MM-SS
	u8 StopTime[6]; 			//����ʱ��
}LBS_SMediaDataCheckreq;

//��ý����������ݸ�ʽ
typedef struct _MEDIA_DATA_FORMAT_s
{
	u8 MediaType;				//0:ͼ��1:��Ƶ2:��Ƶ
	u8 ChannelId;				//0 ��ʾ������ý�����͵�����ͨ��
	u8 EventCode;				//0ƽ̨�·�ָ��1��ʱ����2���ٱ�������3��ײ�෭������������������
	LBS_ReportLocationreq reportLocation;	//λ����Ϣ�㱨
}MEDIA_DATA_FORMAT;

//�洢��ý�����ݼ���Ӧ��
typedef struct _SMEDIA_DATA_CHECK_rsp_s
{
//	u8 sequenceId[2];			//��ˮ��
	u8 DataNum[2];			//������������Ķ�ý������������
//������
}LBS_SMediaDataCheckrsp;

//�洢��ý�������ϴ�����
typedef struct _REPORT_SMEDIA_s
{
	u8 MediaType;				//0:ͼ��1:��Ƶ2:��Ƶ
	u8 ChannelId;				//0 ��ʾ������ý�����͵�����ͨ��
	u8 EventCode;				//0ƽ̨�·�ָ��1��ʱ����2���ٱ�������3��ײ�෭������������������
	u8 StartTime[6];			//��ʼʱ��YY-MM-DD-HH-MM-SS
	u8 StopTime[6]; 			//����ʱ��
	u8 DelFlag;					//0������1ɾ��
}LBS_ReportSMediareq;

//��������͸��
typedef struct _DOWNLOAD_DATA_s
{
	u8 DownloadType;				//͸����Ϣ����
	u8 DownloadMsg[12];				//͸����Ϣ����
}LBS_DownloadDatareq;

//��������͸��
typedef struct _UPLOAD_DATA_s
{
	u8 UploadType;				//͸����Ϣ����
	u8 UploadMsg[12];				//͸����Ϣ����
}LBS_UploadDatareq;
//�绰�г�
typedef struct _CALLCAR_ORDER_ST
{
	u8 OrderID[50];
	u8 OrderTel[20];
//	u8 OrderType;
//	u8 OrderTime[17]; //YYYYMMDD HH:MM:SS
	u8 OrderContentlen;
	u8 OrderContent[230]; //����GBK "����:��ʱ/ԤԼ/ָ��,Ҫ��ʱ��YYYYMMDD HH:MM:SS ˵����
}STRUCT_CALLCARORDER;
//����ѹ���ϱ�
typedef struct _UPLOAD_DATA_COMRESSION_s
{
	u32 CompressDataLen;			//ѹ�����ݳ���
	u8 CompressMsg[12];				//ѹ����Ϣ��
}LBS_UploadDataCompressionreq;

//ƽ̨RSA��Կ
typedef struct _RSA_PLAT_s
{
	u8 e[4];			//ƽ̨RSA��Կ{e,n}�е�e
	u8 n[128];		//RSA��Կ{e,n}�е�n
}LBS_RSAplat;

//�ն�RSA��Կ
typedef struct _RSA_DEV_s
{
	u8 e[4];			//�ն�RSA��Կ{e,n}�е�e
	u8 n[128];		//RSA��Կ{e,n}�е�n
}LBS_RSAdev;

//���ƽ̨ģʽ����ƽ̨����ƽ̨������ƽ̨
typedef enum _SERV_MODE_DEF
{
	eMainServ=0,
	eSubServ,
	eSpclServ
} E_ServMode;
//���ƽ̨socket����
typedef struct _SERV_SOCKET_STRCT
{
	u8  APN[10];
	u8  User[MAXPPPUSERLEN+1];
	u8  Pwd[MAXPPPPWDLEN+1];
//	u8  IP[16];//zq
	u32 port;
}MonitServSocketPara;
//�ն�ע��״̬����
typedef enum _REGIST_STA
{
	Regin=0x00,		//��ע��
	Regout,			//��ע�� DB�������豸��Ϣɾ����
	unRegist=0xFF,	//δע��
//	CarReg,			//������ע��err
//	NocarReg,		//����������err
///	DevReg,			//�豸��ע��err
//	NodevReg,		//�豸������err
} eRegsitSta;
//�ն��Զ���̶�����
typedef struct _STRUCT_DEVIC_DEF
{
	u8  Productor[5];//���� ����
	u8	DevType[8]; //�豸����	0TGC-20I ����
	u8	DevID[12];	 //�豸ID 	p808ȡ��7λ
	u8  Ver_HW[3];   //Ӳ���汾 V15
	u8  Ver_SW[9];  //����汾	SOFTWARE_VER:V010RE001
}STRUCT_DEVDEF_PARA;

//ͨ�ŷ�ʽ��δ����
typedef enum _ENUM_SOCKET_MODE
{
	eUDP_MODE=0,
	eTCP_MODE,
	eSMS_MODE,
};

//STRUCT_LBS_CFG��Աö��
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
	eRpttime_def		=	0x0029, //Ĭ��ʱ����
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
	//���±�����ض�����λ����Ϣ������־���Ӧ
	eAlarmmaskword		=	0x0050, //����������
	etxtSMSAlarm		=	0x0051, //����sms����
	eAlarmPhoto			=	0x0052,	//�������տ���
	eAlarmPhotoStor		=	0x0053,	//�������մ洢��־
	eKeyAlarm			=	0x0054, //�ؼ�������־
	eOverspeed			=	0x0055,	//����
	eOverspeedtime		=	0x0056,
	eDrivTime			=	0x0057,
	eDrivTime_day		=	0x0058,
	eRestTime_min		=	0x0059,	//��С��Ϣʱ�䣬�ﵽ��ʱ��ȡ��ƣ�ͼ�ʻ
	eStopcarTime_max	=	0x005a,	//����ʱͣ������ʱ��
	eImagQuality		=	0x0070,
	eImagBright			=	0x0071,	
	eImagContrast		=	0x0072,
	eImagSatuatn		=	0x0073,
	eImagClor			=	0x0074,
	
	eOdometer			=	0x0080, //��̱��� 
	ePrivic				=	0x0081,	//�������ڵ�ʡ
	eCity				=	0x0082,	//�������ڵ���
	eCarid				=	0x0083,	//���ƺ�
	eCarclor			=	0x0084,
	//�Զ���
	eLclTEL	=0xaaaa,	//������
	eLimitTime=0xaaa1,
	eNET=0xaaa2,		//�������� ��ƽ̨
	eNET_sub=0xaaa3,	//����ƽ̨
	eCfaParaMax=0xFFFF,	//��Чֵ
	emanufacturersid =  0x0085,
	etermmodel = 0x0086,
	etermid = 0x0087,
}Enum_CtrlCfgPara;
/*---------------------------------808Э�����------------------------------*/
//ƽ̨������Ϣ
typedef struct _SERV_REQUEST_s
{
	u16 ServSeqid; //��ˮ��
	u16 ServMsgid; //msd
}STRUCT_SERV_REQ;
//�ն�ͨ��Ӧ��
typedef struct _DEV_COMMON_rsp_s
{
	STRUCT_SERV_REQ servReq;
	u8 Rslt;					//0:�ɹ�/ȷ��,1.ʧ��,2.��Ϣ����,3.��֧��
}LBS_DevRsp;
//////////////////////////////////////////��������ͷ�ļ�����ȡ����////////////////////////
#define BLOCK_LEN	  16   //�鳤��
#define CARD_NUM_LEN  4   //��Ч���ų���
#define STU_NUM_LEN	  16   //��Чѧ��ѧ�ų���
#define STU_NAME_LEN  16   //��Чѧ����������
#define PARE_PHONE_LEN	16  //��Чѧ���ҳ����볤��
//վ����Ϣ
typedef struct _STAT_INFO
{
	u8 InOutflag;  //��ӦSTATION_IN��STATION_OUT
	u8 stationDirect;
	u8 stationNo;//վ���
	u8 stationType;
	u8 LineVer;
	u32 lineNo;
}StateStrct;
/*�洢��ʽ*/
typedef struct
{
	u8 CardNum[CARD_NUM_LEN+1];
	u8 StuNum[STU_NUM_LEN+1];
	u8 StuName[STU_NAME_LEN+1];
	u8 ParePhone[PARE_PHONE_LEN+1];
	u8 aa_flag;	//��־λ
}
StuData;
////////////////////////////////////////////////////////////////////////////////////////////////////////
//���ƽ̨Э�����
typedef struct _Protocol_Handle_s
{
	u8 DevSeqId[2];					//�ն˷�����Ϣ��ˮ�� ��0��ʼ����ѭ��
	u8 DevMsgId[2];					//�ն˷�����ϢID by zhuqing @2013/6/26
	u8 ServSeqId[2];				//ƽ̨������Ϣ��ˮ��
	u8 ServMsgId[2];				//ƽ̨������Ϣ��ID
	u8  Rsp2Serv;					//ƽ̨����Ӧ����
	//ƽ̨���ն�����Ӧ��
	LBS_PlatComRsp PlatComrsp;

	MonitServSocketPara spclServ; //���ƽ̨����
	u16 ExptAckId;					//�ն˷����������������Ϣid global?
//	u16 bRSA;						//�Ƿ���Ҫ���ܣ�1���ܣ�0������
	u8 LocalResult;					//�ն�ִ�н�� ɾ��
	u8 PlatResult;					//ƽִ̨�н��
	u8 LclTel[TELMAXLEN];			//�ն�UIM����

	u32 pdata;						//���ʱ������flash�Ĵ洢��ַ�����ý������
	u32 datalen;					//���ʱ����Ҫ����������ݰ����ȣ������ְ���
	//ƽ̨�·����Ĵ洢
	u8  devreqrsp[MAX_MSG_LEN];
	u16 devreqrsplen;				//������
	u8  *platreq;					//ָ��ƽ̨�·�����buf��ָ��
	u16 platreqlen;					//ƽ̨���͵ı��ĳ��� ����
	u8  Check;						//����У����
	//��Ϣ��ʽ
	u8 msgbuf[MAX_MSG_LEN];	//���ͱ��Ĵ洢
	u16 msgbuflen;
	MSG_HEAD_sub sub;		//�ְ�����
	u32 serialnum;			//�����¼�����ݴ洢����ǰ���
	u8 multipacket;			//��Ӧ��������У����Ӧ�����Ҫ�ְ���������Ϊ1
	u32 multipacketlen;		//��Ӧ��������У������Ҫ�ְ�����õ����ݵ��ܳ���

//	LBS_OnlineupgraPara Upgradpara;

	//�ն�ע��
	LBS_DevRegreq DevRegreq;
	//�ն�ע��Ӧ��
	LBS_DevRegRsp DevRegRsp;

	//�����ն˲���
//	LBS_SetParamreq SetParamreq;
	//��ѯ�ն˲���Ӧ��
//	LBS_GetDevParamrsp GetDevParamrsp;
	//�ն˿���
//	LBS_DevCtlreq DevCtlreq;
	//λ����Ϣ�㱨
//-	LBS_ReportLocationreq ReportLocationreq;

	//λ����Ϣ��ѯӦ��
	LBS_GetLocationrsp GetLocationrsp;
	//�ն˿���ָ�����ƽ̨����
	MonitServSocketPara SpeclServPara;
	u8  Authen[AUTHENMAXLEN+1];		//��Ȩ��,'\0'��β�������
	//��ʱλ�ø��ٿ���
	LBS_TrackLocationCtlreq TrackLocationCtlreq;
	//�ı���Ϣ�·�
	LBS_TextDownloadreq TextDownloadreq;
	//�¼�����
	LBS_SetEventreq SetEventreq;
	//�¼�����
	LBS_ReportEventreq ReportEventreq;
	//�����·�
	LBS_AskDownloadreq AskDownloadreq;
	//����Ӧ��
	LBS_AskAnswerrsp AskAnswerrsp;
	//��Ϣ�㲥�˵�����
	LBS_DemandMenureq DemandMenureq;
	//��Ϣ�㲥/ȡ��
	LBS_DemandSetCancelreq DemandSetCancelreq;
	//��Ϣ����
	LBS_DemandServreq DemandServereq;
	//�绰�ز�
	u8 CallBackreq[TELMAXLEN+2]; //����1B+TEL,'\0'��β
	//���õ绰��
	LBS_SetPhonebookreq SetPhonebookreq;
	//��������
	LBS_CarCtlreq CarCtlreq;
	//��������Ӧ��
	LBS_CarCtlrsp CarCtlrsp;
#if 0
	//Բ��������
	ROUND_AREA_LIST;
	//����Բ������
	LBS_SetRoundAreareq;
	//ɾ��Բ������
	LBS_DelRoundAreareq;
	//����������
	SQUARE_AREA_LIST;
	//���þ�������
	LBS_SetSquareAreareq;
	//ɾ����������
	LBS_DelSquareAreareq;
	//���ö��������
	LBS_SetPolygonAreareq;
	//ɾ�����������
	LBS_DelPolygonAreareq;
	//����·��
	LBS_SetLinereq;
	//ɾ����·
	LBS_DelLinereq;
#endif
	//��ʻ��¼���ݲɼ�����
	LBS_GetDriveDatareq GetDriveDatareq;
	//��ʻ��¼�����ϴ�
	LBS_ReportDriveDatarsp ReportDriveDatarsp;
	//��ʻ��¼�����´�����
	LBS_DownloadDriveDatareq DownloadDriveDatareq;

	LBS_RptElecMenu ElecMenu;
	//��ʻԱ�����Ϣ�ɼ��ϱ�
	LBS_ReportDriverreq ReportDriverreq;
	//��ý���¼���Ϣ�ϴ�
	LBS_Mediainfo MediaEventinfo;
	//��ý�������ϴ�Ӧ��
	LBS_ReportMediaDatarsp ReportMediaDatarsp;
	//����ͷ������������
	TakePhotoStrct TakePhotoreqpara;
	//�洢��ý�����ݼ���
	LBS_SMediaDataCheckreq SmediaDataCheckreq;
	//�洢��ý�����ݼ���Ӧ��
	LBS_SMediaDataCheckrsp SMediaDataCheckrsp;
	//�洢��ý�������ϴ�����
	LBS_ReportSMediareq ReportSmediareq;
	//¼����ʼ����
	LBS_StartRecordreq StartRecordreq;
	//��������͸��
	LBS_DownloadDatareq DownloadDatareq;
	//��������͸��
	LBS_UploadDatareq UploadDatareq;
	//����ѹ���ϱ�
	LBS_UploadDataCompressionreq UploadDataCompressionreq;
	//ƽ̨RSA��Կ
	LBS_RSAplat RSAplat;
	//�ն�RSA��Կ
	LBS_RSAdev RSAdev;
	u8 CallcarAccept; //1���� 2���ؿ�
	u8 CallcarResult; //0��� 1δ�ӵ���
	STRUCT_CALLCARORDER stCallcarOrder;
	StateStrct stationInfo;
	StuData	studentData;	//ѧ�����³�ˢ������
}ProtocolHandle;
typedef enum _PROTOCOL_HANDLE_e
{
	eDevSeqid,				//�ն˷�����Ϣ��ˮ��
	eDevMsgid,				//�ն˷�����ϢID by zhuqing @2013/6/26
	eServSeqid,				//ƽ̨������Ϣ��ˮ
	eServMsgid,				//ƽ̨������ϢID
	eRsp2ServSeq,			//ƽ̨����Ӧ����
	e_Authen,				//��Ȩ��
	eMediaInfo,				//��ý����Ϣ
	eRsp2DevReq,			//�ն�����Ӧ����
	eRsp2DevReq_Media,		//�ն�����Ӧ����ʧ��ʱ����ý��ʧ�ܰ���Ϣ
	eSpclServPara,			//�ն˿��Ƶļ��ƽ̨����
	e_TrackLocationCtlreq,	//��ʱλ����Ϣ��ѯ����
	eRecordPara,            //¼������

//	e_SpclSockt,			//�ն˿���ָ֮��ƽ̨socket����
	e_ReportEventreq,		//�¼�����
	e_AskAnswerrsp,			//����Ӧ��
	e_DemandSetCancl,		// ��Ϣ�㲥/ȡ��
	e_ReportDriverreq_DriverName,		//�ɼ���ʻԱ�����Ϣ����//��ʻԱ����
	e_ReportDriverreq_DriverID,			//�ɼ���ʻԱ�����Ϣ����//���֤���	
	e_ReportDriverreq_driverCertificateId,//�ɼ���ʻԱ�����Ϣ����//��ҵ�ʸ�֤����
	e_ReportDriverreq_OrgName,				//�ɼ���ʻԱ�����Ϣ����//��ҵ�ʸ�֤��֤��������
	e_RSAdev,							//�ն�RSA��Կ��Ϣ��
	e_RSAplat,							//ƽ̨RSA��Կ��Ϣ��
	e_ReportMediaDatarsp,				//У���ý�������ϴ�Ӧ��
//	e_ReportMediaDatarsp_MediaIdList,	//У���ý�������ϴ�Ӧ��//�ش���ID�б�
	
	e_CarCtlrsp,			//��������
	e_ReportDriveDatarsp,	//��ʻ��¼�����ϴ�
	e_TextDownloadreq,		//�ı���Ϣ�·�
	e_SetEventreq_EventType,	//�¼�����//��������
	e_SetEventreq_totalNum,		//�¼�����//��������
	e_AskDownloadreq,			//�����·�
	e_DemandMenureq_DemandType,	//��Ϣ�㲥�˵�����//��������
	e_DemandMenureq_MenuNum,	//��Ϣ�㲥�˵�����//��Ϣ������
	e_DemandServreq, 			//��Ϣ����
	e_CallBackreq, 				//�绰�ز�
	e_SetPhonebookreq_BookType,		//���õ绰��//����
	e_SetPhonebookreq_ContackNum,	//���õ绰��//����
	e_CarCtlreq,					//��������
	e_ElecMenuRpt,					//�����˵��ϱ�
	e_GetDriveDatareq,				//�ɼ���ʻ��¼����
	e_TakePhotoreq, 				//����ͼƬ
	e_SMediaDataCheckreq, 			//�����ն˴洢��ý������
	e_RSPSMediaDataCheckreq, 		//�����ն˴洢��ý������Ӧ��
	e_ReportSMediareq, 				//�洢��ý�������ϴ�����
	e_CallcarOrder,					//��������
	e_CallcarInfoOrder,				//������ϸ����
	e_CallcarAccept,				//��������
	e_CallcarResult,				//������� 0�ɹ� 1δ�ӵ���
	e_APTSStationinfo,				//���ܹ���վ����Ϣ
	e_ProHandlMAX
}Protocol_Handle_e;

//�ն�������
typedef enum
{
	RspOK = 0,    //Ӧ��ɹ�
	RspError,	//ע��Ӧ�����ѱ�ע��
	RspMsgerr,	//ע��Ӧ�����ݿ��޸ó���
	RspUnsurport,	//�ն��ѱ�ע��
	RspAlarmCheck,	//���ݿ��޸��ն�
	RspPackgerr,    //Ӧ�����ݰ�����
	ServNorsp,  //ƽ̨δӦ��
	ServUnlog_SendErr,  //δ��¼��ƽ̨�����ݷ��ʹ���
	RspErrOther, //��������,��������

	PLAT_TREMREG_ACK = 0x10,
	PLAT_COMMON_ACK = 0x11,
	PLAT_REQ,
	
}Enum_DevReqRsult;
u8 WriteAlarmPara(u8 AcktionMode,u8 AlarmSector,u32 AlarmBits);
u8 WriteAllBitAlarmPara(u8 AlarmSector,u32 AlarmBits);
u8 ReadAlarmPara(u8 AlarmSector,u32 AlarmBits);
u8 ReadAllBitAlarmPara(u8 AlarmSector,u32 *AlarmBits );
u8 get_driverecord_datalen(ProtocolHandle *pprotHandle, u8 commandID);
u8 ReadLbsCfgPara(Enum_CtrlCfgPara CfgPara,u8 *pValue,u8 *buflen); //���ƽ̨���������� peijl_20121016 ���
u8 WriteLbsCfgPara(Enum_CtrlCfgPara CfgPara, u8 *pValue,u8 len,UpdateModeEnum UpdateMode); //���ƽ̨����������

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
	dev_active,//�豸��������
	plat_active,//ƽ̨��������
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
