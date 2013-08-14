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
	oa_uint32 heartbeat_interval;									//终端心跳发送间隔，单位为秒（s）
	oa_uint32 tcp_ack_timeout;									//TCP 消息应答超时时间，单位为秒（s）
	oa_uint32 tcp_retrans_times;									//TCP 消息重传次数
	oa_uint32 udp_ack_timeout;									//UDP 消息应答超时时间，单位为秒（s）
	oa_uint32 udp_retrans_times;									//UDP 消息重传次数
	oa_uint32 sms_ack_timeout;									//SMS 消息应答超时时间，单位为秒（s）
	oa_uint32 sms_retrans_times;									//SMS 消息重传次数
	//reserve......0x0008-0x000F									//保留
	oa_uint8 m_apn_name[APN_NAME_MAX_LEN];					//主服务器 APN，无线通信拨号访问点。若网络制式为 CDMA，则该处为PPP 拨号号码
	oa_uint8 m_apn_username[APN_USERNAME_NAME_MAX_LEN];		//主服务器无线通信拨号用户名
	oa_uint8 m_apn_password[APN_PASSWORD_MAX_LEN];			//主服务器无线通信拨号密码
	oa_uint8 m_server_ip[SERVER_IP_MAX_LEN];						//主服务器地址ip
	oa_uint8 m_server_dn[SERVER_DN_MAX_LEN];					//主服务器地址域名
	oa_uint8 b_apn_name[APN_NAME_MAX_LEN];						//备分服务器 APN，无线通信拨号访问点。若网络制式为 CDMA，则该处为PPP 拨号号码
	oa_uint8 b_apn_username[APN_USERNAME_NAME_MAX_LEN];		//备分服务器无线通信拨号用户名
	oa_uint8 b_apn_password[APN_PASSWORD_MAX_LEN];				//备分服务器无线通信拨号密码
	oa_uint8 b_server_ip[SERVER_IP_MAX_LEN];						//备分服务器地址ip
	oa_uint8 b_server_dn[SERVER_DN_MAX_LEN];						//备分服务器地址域名
	oa_uint32 server_tcp_port;									//服务器 TCP 端口
	oa_uint32 server_udp_port;									//服务器 UDP 端口
	//not use......0x001A-0x001D
	//reserve......0x001E-0x001F
	oa_uint32 report_strategy;									//位置汇报策略，0：定时汇报；1：定距汇报；2：定时和定距汇报
	oa_uint32 report_type;										//位置汇报方案，0：根据 ACC 状态； 1：根据登录状态和 ACC 状态，先判断登录状态，若登录再根据 ACC 状态			
	oa_uint32 unlogin_reporttime;									//驾驶员未登录汇报时间间隔，单位为秒（s），>0
	//reserve......0x0023-0x0026
	oa_uint32 sleep_reporttime;									//休眠时汇报时间间隔，单位为秒（s），>0
	oa_uint32 urgent_reporttime;									//紧急报警时汇报时间间隔，单位为秒（s），>0
	oa_uint32 default_reporttime;									//缺省时间汇报间隔，单位为秒（s），>0
	//reserve......0x002A-0x002B
	oa_uint32 default_reportdistance;								//缺省距离汇报间隔，单位为米（m），>0
	oa_uint32 unlogin_reportdistance;								//驾驶员未登录汇报距离间隔，单位为米（m），>0	
	oa_uint32 sleep_reportdistance;								//休眠时汇报距离间隔，单位为米（m），>0
	oa_uint32 urgent_reportdistance;								//紧急报警时汇报距离间隔，单位为米（m），>0
	oa_uint32 corner_reportangle;									//拐点补传角度，<180
	oa_uint16 illegal_displace_threshold;							//电子围栏半径（非法位移阈值），单位为米
	//reserve......0x0032-0x003F
	oa_uint8 monitor_platform_num[TEL_NUM_MAX_LEN];				//监控平台电话号码
	oa_uint8 reset_num[TEL_NUM_MAX_LEN];						//复位电话号码，可采用此电话号码拨打终端电话让终端复位
	oa_uint8 restore_factory_settings_num[TEL_NUM_MAX_LEN];		//恢复出厂设置电话号码，可采用此电话号码拨打终端电话让终端恢复出厂设置
	oa_uint8 monitor_platform_sms_num[TEL_NUM_MAX_LEN];			//监控平台 SMS 电话号码
	oa_uint8 terminal_sms_num[TEL_NUM_MAX_LEN];					//接收终端 SMS 文本报警号码
	//oa_uint32 terminal_answer_strategy;							//终端电话接听策略，0：自动接听；1：ACC ON 时自动接听，OFF 时手动接听 
	//oa_uint32 max_talk_time;										//每次最长通话时间，单位为秒（s），0 为不允许通话，0xFFFFFFFF 为不限制
	//oa_uint32 month_max_talk_time;								//当月最长通话时间，单位为秒（s），0 为不允许通话，0xFFFFFFFF 为不限制
	//oa_uint8 monitor_num[TEL_NUM_MAX_LEN];						//监听电话号码
	//oa_uint8 platform_privilege_sms_num[TEL_NUM_MAX_LEN];			//监管平台特权短信号码
	//reserve......0x004A-0x004F
	oa_uint32 alarm_mask;										//报警屏蔽字，与位置信息汇报消息中的报警标志相对应，相应位为 1则相应报警被屏蔽
	oa_uint32 alarm_send_sms_mask;								//报警发送文本 SMS 开关，与位置信息汇报消息中的报警标志相对应，相应位为 1 则相应报警时发送文本 SMS
	//oa_uint32 alarm_shoot_switch;									//报警拍摄开关，与位置信息汇报消息中的报警标志相对应，相应位为1 则相应报警时摄像头拍摄
	//oa_uint32 alarm_shoot_storage_flag;							//报警拍摄存储标志，与位置信息汇报消息中的报警标志相对应，相应位为 1 则对相应报警时拍的照片进行存储，否则实时上传
	//oa_uint32 key_marker;										//关键标志，与位置信息汇报消息中的报警标志相对应，相应位为 1 则对相应报警为关键报警
	oa_uint32 max_speed;										//最高速度，单位为公里每小时（km/h）
	oa_uint32 speed_duration;										//超速持续时间，单位为秒（s）
	oa_uint32 continuous_drive_time_threshold;					//连续驾驶时间门限，单位为秒（s）
	oa_uint32 day_add_drive_time_threshold;						//当天累计驾驶时间门限，单位为秒（s）
	oa_uint32 min_rest_time;										//最小休息时间，单位为秒（s）
	oa_uint32 max_park_time;										//最长停车时间，单位为秒（s）
	//oa_uint32 overspeed_alarm_difference;							//超速报警预警差值，单位为 1/10Km/h
	//oa_uint32 fatigue_driver_difference;							//疲劳驾驶预警差值，单位为秒（s），>0
	//not use......0x005D-0x005E
	//resever......0x005F-0x0063
	//not use......0x0064-0x0065
	//resever......0x0066-0x006F
	//not use......0x0070-0x0074
	//resever......0x0075-0x007F
	oa_uint32 vehicle_odometer;									//车辆里程表读数，1/10km
	oa_uint16 vehicle_province_id;									//车辆所在的省域 ID
	oa_uint16 vehicle_city_id;										//车辆所在的市域 ID
	oa_uint8 vehicle_license[VEHICLE_LICENCE_MAX_LEN];				//公安交通管理部门颁发的机动车号牌
	oa_uint8 plate_color;											//车牌颜色，按照 JT/T415-2006 的 5.4.12
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
