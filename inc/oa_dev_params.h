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
 *   oa_dev_params.h
 *
 * Project:
 * --------
 *   MTK-LBS project
 *
 * Description:
 * ------------
 *   This Module declares OPEN AT's api.
 	define device's parameters for protocol 808
 *
 * Author:
 * -------
 *   zhuqing.
 *
 ****************************************************************************/
#ifndef __OA_DEV_PARAMS_H__
#define __OA_DEV_PARAMS_H__
#include "oa_type.h"
#define APN_NAME_MAX_LEN 64
#define APN_USERNAME_NAME_MAX_LEN 64
#define APN_PASSWORD_MAX_LEN 64
#define SERVER_IP_MAX_LEN 16
#define SERVER_DN_MAX_LEN 64
#define TEL_NUM_MAX_LEN 32
#define VEHICLE_LICENCE_MAX_LEN 16
#define MAN_ID_MAX_LEN 5
#define TERM_MODEL_MAX_LEN 20
#define TERM_ID_MAX_LEN 7
#define TERM_TEL_NUM_MAX_LEN 12
#define FTP_MAX_LEN 32
#define DEV_PARAMS_FILE L"dev_paras.ini"
#define DEVID_FILE L"dev_id.ini"
#define DEVID_INI "0000000"
typedef struct
{
	oa_uint32 heartbeat_interval;									//�ն��������ͼ������λΪ�루s��
	oa_uint32 tcp_ack_timeout;									//TCP ��ϢӦ��ʱʱ�䣬��λΪ�루s��
	oa_uint32 tcp_retrans_times;									//TCP ��Ϣ�ش�����
	oa_uint32 udp_ack_timeout;									//UDP ��ϢӦ��ʱʱ�䣬��λΪ�루s��
	oa_uint32 udp_retrans_times;									//UDP ��Ϣ�ش�����
	oa_uint32 sms_ack_timeout;									//SMS ��ϢӦ��ʱʱ�䣬��λΪ�루s��
	oa_uint32 sms_retrans_times;									//SMS ��Ϣ�ش�����
	//reserve......0x0008-0x000F									//����
	oa_uint8 m_apn_name[APN_NAME_MAX_LEN];					//�������� APN������ͨ�Ų��ŷ��ʵ㡣��������ʽΪ CDMA����ô�ΪPPP ���ź���
	oa_uint8 m_apn_username[APN_USERNAME_NAME_MAX_LEN];		//������������ͨ�Ų����û���
	oa_uint8 m_apn_password[APN_PASSWORD_MAX_LEN];			//������������ͨ�Ų�������
	oa_uint8 m_server_ip[SERVER_IP_MAX_LEN];						//����������ַip
	oa_uint8 m_server_dn[SERVER_DN_MAX_LEN];					//����������ַ����
	oa_uint8 b_apn_name[APN_NAME_MAX_LEN];						//���ַ����� APN������ͨ�Ų��ŷ��ʵ㡣��������ʽΪ CDMA����ô�ΪPPP ���ź���
	oa_uint8 b_apn_username[APN_USERNAME_NAME_MAX_LEN];		//���ַ���������ͨ�Ų����û���
	oa_uint8 b_apn_password[APN_PASSWORD_MAX_LEN];				//���ַ���������ͨ�Ų�������
	oa_uint8 b_server_ip[SERVER_IP_MAX_LEN];						//���ַ�������ַip
	oa_uint8 b_server_dn[SERVER_DN_MAX_LEN];						//���ַ�������ַ����
	oa_uint32 server_tcp_port;									//������ TCP �˿�
	oa_uint32 server_udp_port;									//������ UDP �˿�
	//not use......0x001A-0x001D
	//reserve......0x001E-0x001F
	oa_uint32 report_strategy;									//λ�û㱨���ԣ�0����ʱ�㱨��1������㱨��2����ʱ�Ͷ���㱨
	oa_uint32 report_type;										//λ�û㱨������0������ ACC ״̬�� 1�����ݵ�¼״̬�� ACC ״̬�����жϵ�¼״̬������¼�ٸ��� ACC ״̬			
	oa_uint32 unlogin_reporttime;									//��ʻԱδ��¼�㱨ʱ��������λΪ�루s����>0
	//reserve......0x0023-0x0026
	oa_uint32 sleep_reporttime;									//����ʱ�㱨ʱ��������λΪ�루s����>0
	oa_uint32 urgent_reporttime;									//��������ʱ�㱨ʱ��������λΪ�루s����>0
	oa_uint32 default_reporttime;									//ȱʡʱ��㱨�������λΪ�루s����>0
	//reserve......0x002A-0x002B
	oa_uint32 default_reportdistance;								//ȱʡ����㱨�������λΪ�ף�m����>0
	oa_uint32 unlogin_reportdistance;								//��ʻԱδ��¼�㱨����������λΪ�ף�m����>0	
	oa_uint32 sleep_reportdistance;								//����ʱ�㱨����������λΪ�ף�m����>0
	oa_uint32 urgent_reportdistance;								//��������ʱ�㱨����������λΪ�ף�m����>0
	oa_uint32 corner_reportangle;									//�յ㲹���Ƕȣ�<180
	oa_uint16 illegal_displace_threshold;							//����Χ���뾶���Ƿ�λ����ֵ������λΪ��
	//reserve......0x0032-0x003F
	oa_uint8 monitor_platform_num[TEL_NUM_MAX_LEN];				//���ƽ̨�绰����
	oa_uint8 reset_num[TEL_NUM_MAX_LEN];						//��λ�绰���룬�ɲ��ô˵绰���벦���ն˵绰���ն˸�λ
	oa_uint8 restore_factory_settings_num[TEL_NUM_MAX_LEN];		//�ָ��������õ绰���룬�ɲ��ô˵绰���벦���ն˵绰���ն˻ָ���������
	oa_uint8 monitor_platform_sms_num[TEL_NUM_MAX_LEN];			//���ƽ̨ SMS �绰����
	oa_uint8 terminal_sms_num[TEL_NUM_MAX_LEN];					//�����ն� SMS �ı���������
	//oa_uint32 terminal_answer_strategy;							//�ն˵绰�������ԣ�0���Զ�������1��ACC ON ʱ�Զ�������OFF ʱ�ֶ����� 
	//oa_uint32 max_talk_time;										//ÿ���ͨ��ʱ�䣬��λΪ�루s����0 Ϊ������ͨ����0xFFFFFFFF Ϊ������
	//oa_uint32 month_max_talk_time;								//�����ͨ��ʱ�䣬��λΪ�루s����0 Ϊ������ͨ����0xFFFFFFFF Ϊ������
	//oa_uint8 monitor_num[TEL_NUM_MAX_LEN];						//�����绰����
	//oa_uint8 platform_privilege_sms_num[TEL_NUM_MAX_LEN];			//���ƽ̨��Ȩ���ź���
	//reserve......0x004A-0x004F
	oa_uint32 alarm_mask;										//���������֣���λ����Ϣ�㱨��Ϣ�еı�����־���Ӧ����ӦλΪ 1����Ӧ����������
	oa_uint32 alarm_send_sms_mask;								//���������ı� SMS ���أ���λ����Ϣ�㱨��Ϣ�еı�����־���Ӧ����ӦλΪ 1 ����Ӧ����ʱ�����ı� SMS
	//oa_uint32 alarm_shoot_switch;									//�������㿪�أ���λ����Ϣ�㱨��Ϣ�еı�����־���Ӧ����ӦλΪ1 ����Ӧ����ʱ����ͷ����
	//oa_uint32 alarm_shoot_storage_flag;							//��������洢��־����λ����Ϣ�㱨��Ϣ�еı�����־���Ӧ����ӦλΪ 1 �����Ӧ����ʱ�ĵ���Ƭ���д洢������ʵʱ�ϴ�
	//oa_uint32 key_marker;										//�ؼ���־����λ����Ϣ�㱨��Ϣ�еı�����־���Ӧ����ӦλΪ 1 �����Ӧ����Ϊ�ؼ�����
	oa_uint32 max_speed;										//����ٶȣ���λΪ����ÿСʱ��km/h��
	oa_uint32 speed_duration;										//���ٳ���ʱ�䣬��λΪ�루s��
	oa_uint32 continuous_drive_time_threshold;					//������ʻʱ�����ޣ���λΪ�루s��
	oa_uint32 day_add_drive_time_threshold;						//�����ۼƼ�ʻʱ�����ޣ���λΪ�루s��
	oa_uint32 min_rest_time;										//��С��Ϣʱ�䣬��λΪ�루s��
	oa_uint32 max_park_time;										//�ͣ��ʱ�䣬��λΪ�루s��
	//oa_uint32 overspeed_alarm_difference;							//���ٱ���Ԥ����ֵ����λΪ 1/10Km/h
	//oa_uint32 fatigue_driver_difference;							//ƣ�ͼ�ʻԤ����ֵ����λΪ�루s����>0
	//not use......0x005D-0x005E
	//resever......0x005F-0x0063
	//not use......0x0064-0x0065
	//resever......0x0066-0x006F
	//not use......0x0070-0x0074
	//resever......0x0075-0x007F
	oa_uint32 vehicle_odometer;									//������̱������1/10km
	oa_uint16 vehicle_province_id;									//�������ڵ�ʡ�� ID
	oa_uint16 vehicle_city_id;										//�������ڵ����� ID
	oa_uint8 vehicle_license[VEHICLE_LICENCE_MAX_LEN];				//������ͨ�����Ű䷢�Ļ���������
	oa_uint8 plate_color;											//������ɫ������ JT/T415-2006 �� 5.4.12
	//not use......0x0090-0xFFFF;
	//user definition
	oa_uint8 manufacturers_id[MAN_ID_MAX_LEN+1];
	oa_uint8 term_model[TERM_MODEL_MAX_LEN+1];
	oa_uint8 term_id[TERM_ID_MAX_LEN+1];
	oa_uint8 term_tel_num[TERM_TEL_NUM_MAX_LEN];//LclTEL
	oa_uint8 update_server_ip[SERVER_IP_MAX_LEN];	
	oa_uint32 update_server_port;	
	oa_uint8 ftpusr[FTP_MAX_LEN];
	oa_uint8 ftppwd[FTP_MAX_LEN];
	oa_uint8 ftp_prog_name[FTP_MAX_LEN];
}DEVICE_PARAMS;

typedef struct
{
	oa_uint8 dev_id[TERM_ID_MAX_LEN+1];
}dev_id_struct;

extern oa_bool factory_set(void);
extern oa_bool dev_params_init(void);
extern void params_to_soc_set(void);
#endif
