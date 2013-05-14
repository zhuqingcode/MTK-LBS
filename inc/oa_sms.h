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
 *   oa_sms.h
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
#ifndef __OA_SMS_H__
#define __OA_SMS_H__
#include "oa_type.h"
#include "oa_api.h"

#define SMS_CALLBACK
#define DBG_SMS
#define SMS_HANDLE_DELAY 1000//ms
#define SMS_NUM_MAX_LEN 32
#define SMS_DATA_MAX_LEN 160
#define CONTEXT_MAX_LEN 128
//key words
#define HB "Hearttime"
#define RSP_TCP "RSP_TCP"
#define RSP_UDP "RSP_UDP"
#define RSP_SMS "RSP_SMS"
#define Retry_TCP "Retry_TCP"
#define Retry_UDP "Retry_UDP"
#define Retry_SMS "Retry_SMS"
#define IP "IP"
#define UDPPORT "UDPPORT"
#define TCPPORT "TCPPORT"
#define Sub_IP "Sub_IP"
#define Sub_User "Sub_User"
#define Sub_Pwd "Sub_Pwd"
#define TEL "TEL"
#define VPDNUSR "VPDNUSR"
#define VPDNPWD "VPDNPWD"
#define Rpt_strategy "Rpt_strategy"
#define Rpttime_unlog "Rpttime_unlog"
#define Rpttime_sleep "Rpttime_sleep"
#define Rpttime_alarm "Rpttime_alarm"
#define Rpttime_def "Rpttime_def"
#define Rptdis_unlog "Rptdis_unlog"
#define Rptdis_sleep "Rptdis sleep"
#define Rptdis_alarm "Rptdis alarm"
#define Rptdis_def "Rptdis_def"
#define Rptcog "Rptcog"
#define servertel "servertel"
#define resettel "resettel"
#define factorysettel "factorysettel"
#define alarmsmstel "alarmsmstel"
#define monitertel "monitertel"
#define spclsmstel "spclsmstel"
#define swh_alarmmask "swh_alarmmask"
#define swh_alarmsms "swh_alarmsms"
#define swh_alarmphoto "swh_alarmphoto"
#define swh_alarmphotostore "swh_alarmphotostore"
#define swh_keyalarm "swh_keyalarm"
#define overspeed "overspeed"
#define overspeedtime "overspeedtime"
#define tireddrivetime "tireddrivetime"
#define min_resttime "min_resttime"
#define max_parktime "max_parktime"
#define provincID "provincID"
#define cityID "cityID"
#define carID "carID"
#define carcolor "carcolor"
#define UPIP "UPIP"
#define UPPORT "UPPORT"
#define UPPORT_UDP "UPPORT_UDP"
#define UPPORT_TCP "UPPORT_TCP"
#define UPNET "UPNET"
#define UPFTPUSR "UPFTPUSR"
#define UPFTPPWD "UPFTPPWD"
#define UPPROG_NAME "UPPROG_NAME"
#define STATUS "STATUS"
#define STATICS "STATICS"
#define GPS "GPS"
#define DEV_LOCK "dev_lock" //dev_lock:0 unlock device
#define UPDATE "UPDATE"
#define CLRLOG "CLRLOG"
#define AUTHEN "AUTHEN"
#define RESTART "RESTART"

#define COLON ':'
#define SEMICOLON ';'
#define SEMI_A ";A"
#define SPACE ' '
#define IP_SUFFIX_LEN 3
#define IP_MAX_LEN 15
#define PORT_MAX 65535
#define PRINT_SAMEPARA	Trace("(%s:%s:%d): this parameter is same as the original, so I do nothing...", __FILE__, __func__, __LINE__)
#define ALARMBIT_MAX_LEN 32
#define Rpt_strategy_max_num 3
#define UNLOCK 0

typedef enum
{
	e_HB,
	e_RSP_TCP,
	e_RSP_UDP,
	e_RSP_SMS,
	e_Retry_TCP,
	e_Retry_UDP,
	e_Retry_SMS,
	e_IP,
	e_UDPPORT,
	e_TCPPORT,
	e_Sub_IP,
	e_Sub_User,
	e_Sub_Pwd,
	e_TEL,
	e_VPDNUSR,
	e_VPDNPWD,
	e_Rpt_strategy,
	e_Rpttime_unlog,
	e_Rpttime_sleep,
	e_Rpttime_alarm,
	e_Rpttime_def,
	e_Rptdis_unlog,
	e_Rptdis_sleep,
	e_Rptdis_alarm,
	e_Rptdis_def,
	e_Rptcog,
	e_servertel,
	e_resettel,
	e_factorysettel,
	e_alarmsmstel,
	e_monitertel,
	e_spclsmstel,
	e_swh_alarmmask,
	e_swh_alarmsms,
	e_swh_alarmphoto,
	e_swh_alarmphotostore,
	e_swh_keyalarm,
	e_overspeed,
	e_overspeedtime,
	e_tireddrivetime,
	e_min_resttime,
	e_max_parktime,
	e_provincID,
	e_cityID,
	e_carID,
	e_carcolor,
	e_UPIP,
	e_UPPORT,
	e_UPPORT_UDP,
	e_UPPORT_TCP,
	e_UPNET,
	e_UPFTPUSR,
	e_UPFTPPWD,
	e_UPPROG_NAME,
	e_STATUS,
	e_STATICS,
	e_GPS,
	e_dev_lock,
	e_UPDATE,
	e_CLRLOG,
	e_AUTHEN,
	e_RESTART,
	e_none = 0xff,
}e_keyword;
typedef enum
{
	set = 0x0,
	enquire,
	control,
}command_kind;

typedef enum
{
	e_char = 0x0,
	e_int,
}para_kind;

typedef enum
{
	ack = 0x0,
	noack,
	ackerr,
}ack_kind;

typedef enum
{
	reconn = 0x1,
	para_save,
	unlock_dev,
	update,
	clr_log,
	clr_authcode,
	update_authcode,
	reset,
	no_act = 0xf,
}action_kind;

enum{
	BI = 2,
	DEC = 10,
	HEX = 16,
};

typedef struct
{
	oa_char deliver_num[SMS_NUM_MAX_LEN];
	oa_uint8 data[SMS_DATA_MAX_LEN];
	oa_uint16 len;
	oa_smsal_dcs_enum dcs;
}oa_sms_context;

typedef union
{	
	oa_char con_ch[CONTEXT_MAX_LEN];
	oa_uint32 con_int;
}ch_int_con;

typedef struct
{
	command_kind kind;
	ch_int_con context;
	oa_bool need_ack;
	action_kind act_kind;
}keyword_context;

extern void oa_app_sms(void);
#endif
