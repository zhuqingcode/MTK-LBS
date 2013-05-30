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
 *   oa_lbs2mtk.c
 *
 * Project:
 * --------
 *   MTK-LBS project
 *
 * Description:
 * ------------
 *   This Module declares OPEN AT's api.
 *	just for transplant some code to mtk-lbs & I think tranplanting is a bad idea to this project and that makes source code
 *	looks like a shit
 * Author:
 * -------
 *   zhuqing.
 *
 ****************************************************************************/
#include "oa_type.h"
#include "oa_api.h"
#include "oa_sms.h"
#include "oa_uart.h"
#include "oa_dev_params.h"
#include "oa_use_lock.h"
#include "oa_gps.h"
#include "oa_platform.h"
#include "oa_lbs2mtk.h"
#include "oa_jt808.h"
#include "oa_debug.h"

extern DEVICE_PARAMS dev_now_params;
extern USE_LOCK now_use_lock;
extern oa_uint8 acc_status;
extern STRUCT_RMC Pos_Inf;
extern DEV_PLAT_PARAS dev_running;
extern oa_uint8 KEYWORDS_SIZE;
extern oa_char *p_keyword[];
#if 0
/*********************************************************
*Function:      need_ack_check()
*Description:  search key word in message
*Return:        void
*Others:         
*********************************************************/
oa_bool set_enquiry_check_4trans(oa_char *p_key, oa_uint8 e_len, keyword_context *p_set, 
																	e_keyword e_kind, u16 len)
{
	char *p = NULL;
	oa_char *p_SEMICOLON = NULL; 
	oa_char temp[128] = {0x0};
	oa_uint8 copy_len = 0;
	if (NULL == p_key || e_len == 0 || p_set == NULL){
		Trace("(%s:%s:%d): err!", __FILE__, __func__, __LINE__);
		return OA_FALSE;
	}

	copy_len = len;
	p = p_key+e_len;
	if (*p == COLON){//means set
		p_set->kind = set;
		p++;
		p_SEMICOLON = oa_strchr(p_key, SEMICOLON);
		if (NULL != p_SEMICOLON)	oa_memcpy(temp, p, p_SEMICOLON-p);
		else oa_memcpy(temp, p, copy_len - (e_len+1));
		switch (e_kind){
			case e_HB:
			case e_RSP_TCP:
			case e_RSP_UDP:
			case e_Retry_TCP:
			case e_Retry_UDP:
			case e_Retry_SMS:{
				if (oa_is_digit_str(temp, oa_strlen(temp))){
					p_set->context.con_int = oa_atoi(temp);
				}
				else{
					Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
					return OA_FALSE;
				}
			}break;
			case e_IP:
			case e_Sub_IP:
			case e_UPIP:{
				if (ISAscIPValid(temp, oa_strlen(temp))){
					oa_memcpy(p_set->context.con_ch, temp, oa_strlen(temp));
				}
				else{
					Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
					return OA_FALSE;
				}
			}break;
			case e_TCPPORT:
			case e_UDPPORT:
			case e_UPPORT:{
				if (oa_is_digit_str(temp, oa_strlen(temp))){
					u32 tmp = oa_atoi(temp);
					if (tmp <= PORT_MAX){
						p_set->context.con_int = tmp;
					}
					else{
						Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
						return OA_FALSE;
					}
				}
				else{
					Trace("(%s:%s:%d): format err!", __FILE__, __func__, __LINE__);
					return OA_FALSE;
				}
			}break;
			case e_TEL:
			case e_resettel:
			case e_factorysettel:
			case e_servertel:
			case e_alarmsmstel:{
				if (oa_is_phone_addr_str(temp, oa_strlen(temp))){
					if (oa_strlen(temp) < TEL_NUM_MAX_LEN)	oa_memcpy(p_set->context.con_ch, temp, oa_strlen(temp));
					else{
						Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
						return OA_FALSE;
					}
				}
				else{
					Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
					return OA_FALSE;
				}
			}break;
			case e_Rpt_strategy:{
				if (oa_is_digit_str(temp, oa_strlen(temp))){
					u32 tmp;
					tmp = oa_atoi(temp);
					if (tmp < Rpt_strategy_max_num)	p_set->context.con_int = tmp;
					else{
						Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
						return OA_FALSE;
					}
				}
				else{
					Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
					return OA_FALSE;
				}
			}break;
			case e_Rpttime_sleep:
			case e_Rpttime_def:
			case e_overspeed:
			case e_min_resttime:
			case e_tireddrivetime:
			case e_max_parktime:{
				if (oa_is_digit_str(temp, oa_strlen(temp))){
					p_set->context.con_int = oa_atoi(temp);
				}
				else{
					Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
					return OA_FALSE;
				}
			}break;
			case e_swh_alarmsms:
			case e_swh_alarmmask:{
				if (digit01_string_check(temp, oa_strlen(temp))){
					if (oa_strlen(temp) <= ALARMBIT_MAX_LEN){
						//Trace("(%s:%s:%d): temp:%s!", __FILE__, __func__, __LINE__, temp);
						p_set->context.con_int = atobi(temp, oa_strlen(temp));
						//Trace("(%s:%s:%d): p_set->context.con_int:%u!", __FILE__, __func__, __LINE__, p_set->context.con_int);
					}	
					else{
						Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
						return OA_FALSE;
					}
				}
				else{
					Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
					return OA_FALSE;
				}
			}break;
			case e_provincID:
			case e_cityID:{
				if (oa_is_digit_str(temp, oa_strlen(temp))){
					p_set->context.con_int = oa_atoi(temp);
					if (p_set->context.con_int > 65535){
						Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
						return OA_FALSE;
					}
				}
				else{
					Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
					return OA_FALSE;
				}
			}break;
			case e_carID:{
				if ((oa_strlen(temp) < VEHICLE_LICENCE_MAX_LEN) && digit_alpha_check(temp, oa_strlen(temp))){
					oa_memcpy(p_set->context.con_ch, temp, oa_strlen(temp));
				}
				else{
					Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
					return OA_FALSE;
				}
			}break;
			case e_carcolor:{
				if (oa_is_digit_str(temp, oa_strlen(temp))){
					p_set->context.con_int = oa_atoi(temp);
					if (p_set->context.con_int > 4){
						Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
						return OA_FALSE;
					}
				}
			}break;
			case e_UPFTPUSR:
			case e_UPFTPPWD:{
				if (digit_alpha_check(temp, oa_strlen(temp))){
					if (oa_strlen(temp) < FTP_MAX_LEN)		oa_memcpy(p_set->context.con_ch, temp, oa_strlen(temp));
					else{
						Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
						return OA_FALSE;
					}
				}
				else{
					Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
					return OA_FALSE;
				}
			}break;
			case e_UPPROG_NAME:{
				if (oa_strlen(temp) < FTP_MAX_LEN)		oa_memcpy(p_set->context.con_ch, temp, oa_strlen(temp));
				else{
					Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
					return OA_FALSE;
				}
			}break;
			case e_AUTHEN:{
				if (oa_strlen(temp) > 0 && oa_strlen(temp) <= AUTHEN_CODE_MAX_LEN){
					oa_memcpy(p_set->context.con_ch, temp, oa_strlen(temp));
				}
				else if (0 == oa_strlen(temp))	p_set->context.con_ch[0] = 0xff;
			}break;
			case e_dev_lock:{
				if (oa_is_digit_str(temp, oa_strlen(temp))){
					p_set->context.con_int = oa_atoi(temp);
					Trace("(%s:%s:%d): dev unlock!", __FILE__, __func__, __LINE__);
					if (p_set->context.con_int != UNLOCK){//only for unlock 
						Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
						return OA_FALSE;
					}
				}
			}break;
			case e_DEVID:{
				if (oa_strlen(temp) == DEVID_LEN)	oa_memcpy(p_set->context.con_ch, temp, DEVID_LEN);
				else{
					Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
					return OA_FALSE;
				}
			}break;
			default:{
				Trace("(%s:%s:%d): e_kind:%d", __FILE__, __func__, __LINE__, e_kind);
				Trace("(%s:%s:%d): not support!", __FILE__, __func__, __LINE__);
				return OA_FALSE;
			}break;
		}
		
	}
	else if (*p == SEMICOLON){//means enquiry
		p_set->kind = enquire;
	}
	else{
		Trace("(%s:%s:%d): format err!", __FILE__, __func__, __LINE__);
		return OA_FALSE;
	}
	return OA_TRUE;
}
#endif
#if 0
/*********************************************************
*Function:      need_ack_check_4trans()
*Description:  search key word in message
*Return:        void
*Others:         
*********************************************************/
ack_kind need_ack_check_4trans(oa_char *p)
{
	oa_char *p_SEMICOLON = NULL; 
	if (NULL == p){
		Trace("(%s:%s:%d): err!", __FILE__, __func__, __LINE__);
		return ackerr;
	}
	p_SEMICOLON = oa_strchr(p, SEMICOLON);
	if (p_SEMICOLON != NULL){
		if (0x0 != *(p_SEMICOLON+2)){
			Trace("(%s:%s:%d): format err!", __FILE__, __func__, __LINE__);
			return ackerr;
		}
		if (*(p_SEMICOLON+1) == 'A' || *(p_SEMICOLON+1) == 'a')	return ack;
		else if (*(p_SEMICOLON+1) == 'N' || *(p_SEMICOLON+1) == 'n')	return noack;
		else if (*(p_SEMICOLON+1) == 0x0)	return ack;//this is different from SMS
		else{
			Trace("(%s:%s:%d): format err!", __FILE__, __func__, __LINE__);
			return ackerr;
		}
	}
	else	{
		
		return noack;//p_set->need_ack = OA_FALSE;
	}	
}
#endif
#if 0
/*********************************************************
*Function:      lookfor_keywords_4trans()
*Description:  search key word in message
*Return:        void
*Others:         
*********************************************************/
e_keyword lookfor_keywords_4trans(u8 *p_sms, u16 sms_len, keyword_context *p_set, oa_uint8 e_i)
{
	oa_char *p_key = NULL;
	u8 temp[256] = {0x0};
	oa_char ch;
	ack_kind ack_ret;
	oa_uint8 e_len;
	oa_bool ret;
//	oa_uint8 e_i;
	
	if (NULL == p_set || e_i == e_none || NULL == p_sms || sms_len > 256){
		Trace("(%s:%s:%d): p_set err!", __FILE__, __func__, __LINE__);
		return e_none;
	}

	oa_memcpy(temp, p_sms, sms_len);
	p_key = oa_strstr(temp, p_keyword[e_i]);

	
	
	if (NULL != p_key){
		if (p_key != &temp[0])		return e_none;

		//need ack check
		ack_ret = need_ack_check_4trans(p_key);
		if (ack_ret == ackerr)	return e_none;
		else if (ack_ret == ack)	p_set->need_ack = OA_TRUE;//means need ack
		else if (ack_ret == noack)  p_set->need_ack = OA_FALSE;
		//set/enquiry check
		e_len = oa_strlen(p_keyword[e_i]);
		ret = set_enquiry_check_4trans(p_key, e_len, p_set, e_i, sms_len);
		if (OA_FALSE == ret) return e_none;
		else return e_i;
	}


	return e_none;
}
#endif
#if 0
/*********************************************************
*Function:      handle_keyword()
*Description:  handle the keyword
*Return:        void
*Others:         
*********************************************************/
void handle_common_4trans(e_keyword key_kind, keyword_context *p_set, u8 *p_fbk, u16 *p_fbk_len)
{
	oa_bool ret;
	char temp[16] = {0x0};
	char enquire_temp[128] = {0x0};
	
	if (p_set->kind == enquire || set == p_set->kind){//else if (p_set->kind == enquire){
		switch(key_kind){
			case e_HB:{
				sprintf(enquire_temp, "Hearttime:%d"/*very important here*/, dev_now_params.heartbeat_interval);
			}break;
			case e_RSP_TCP:{
				sprintf(enquire_temp, "RSP_TCP:%d"/*very important here*/, dev_now_params.tcp_ack_timeout);
			}break;
			case e_RSP_UDP:{
				sprintf(enquire_temp, "RSP_UDP:%d"/*very important here*/, dev_now_params.udp_ack_timeout);
			}break;
			case e_RSP_SMS:{
				sprintf(enquire_temp, "RSP_SMS:%d"/*very important here*/, dev_now_params.sms_ack_timeout);
			}break;
			case e_Retry_TCP:{
				sprintf(enquire_temp, "Retry_TCP:%d"/*very important here*/, dev_now_params.tcp_retrans_times);
			}break;
			case e_Retry_UDP:{
				sprintf(enquire_temp, "Retry_UDP:%d"/*very important here*/, dev_now_params.udp_retrans_times);
			}break;
			case e_Retry_SMS:{
				sprintf(enquire_temp, "Retry_SMS:%d"/*very important here*/, dev_now_params.sms_retrans_times);
			}break;
			case e_IP:{
				oa_strcat(enquire_temp, "IP:");
				oa_strcat(enquire_temp, dev_now_params.m_server_ip);
			}break;
			case e_TCPPORT:{
				sprintf(enquire_temp, "TCPPORT:%d"/*very important here*/, dev_now_params.server_tcp_port);
			}break;
			case e_UDPPORT:{
				sprintf(enquire_temp, "UDPPORT:%d"/*very important here*/, dev_now_params.server_tcp_port);
			}break;
			case e_TEL:{
				oa_strcat(enquire_temp, "TEL:");
				oa_strcat(enquire_temp, dev_now_params.term_tel_num);
			}break;
			case e_Rpt_strategy:{
				sprintf(enquire_temp, "Rpt_strategy:%d", dev_now_params.report_strategy);
			}break;
			case e_Rpttime_sleep:{
				sprintf(enquire_temp, "Rpttime_sleep:%d", dev_now_params.sleep_reporttime);
			}break;
			case e_Rpttime_def:{
				sprintf(enquire_temp, "Rpttime_def:%d", dev_now_params.default_reporttime);
			}break;
			case e_servertel:{
				oa_strcat(enquire_temp, "servertel:");
				oa_strcat(enquire_temp, dev_now_params.monitor_platform_num);
			}break;
			case e_resettel:{
				oa_strcat(enquire_temp, "resettel:");
				oa_strcat(enquire_temp, dev_now_params.reset_num);
			}break;
			case e_factorysettel:{
				oa_strcat(enquire_temp, "factorysettel:");
				oa_strcat(enquire_temp, dev_now_params.restore_factory_settings_num);
			}break;
			case e_alarmsmstel:{
				oa_strcat(enquire_temp, "alarmsmstel:");
				oa_strcat(enquire_temp, dev_now_params.terminal_sms_num);
			}break;
			case e_swh_alarmmask:{
				oa_char tmp[33] = {0x0};
				oa_itoa(dev_now_params.alarm_mask, tmp, BI);
				oa_strcat(enquire_temp, "swh_alarmmask:");
				oa_strcat(enquire_temp, tmp);
			}break;
			case e_swh_alarmsms:{
				oa_char tmp[33] = {0x0};
				oa_itoa(dev_now_params.alarm_send_sms_mask, tmp, BI);
				//Trace("(%s:%s:%d): temp:%s!", __FILE__, __func__, __LINE__, tmp);
				oa_strcat(enquire_temp, "swh_alarmsms:");
				oa_strcat(enquire_temp, tmp);
			}break;
			case e_overspeed:{
				sprintf(enquire_temp, "overspeed:%d", dev_now_params.max_speed);
			}break;
			case e_overspeedtime:{
				sprintf(enquire_temp, "overspeedtime:%d", dev_now_params.speed_duration);
			}break;
			case e_min_resttime:{
				sprintf(enquire_temp, "min_resttime:%d", dev_now_params.min_rest_time);
			}break;
			case e_max_parktime:{
				sprintf(enquire_temp, "max_parktime:%d", dev_now_params.max_park_time);
			}break;
			case e_tireddrivetime:{
				sprintf(enquire_temp, "tireddrivetime:%d", dev_now_params.continuous_drive_time_threshold);
			}break;
			case e_provincID:{
				sprintf(enquire_temp, "provincID:%d", dev_now_params.vehicle_province_id);
			}break;
			case e_cityID:{
				sprintf(enquire_temp, "cityID:%d", dev_now_params.vehicle_city_id);
			}break;
			case e_carID:{
				oa_strcat(enquire_temp, "carID:");
				oa_strcat(enquire_temp, dev_now_params.vehicle_license);
			}break;
			case e_carcolor:{
				sprintf(enquire_temp, "carcolor:%d", dev_now_params.plate_color);
			}break;
			case e_UPIP:{
				oa_strcat(enquire_temp, "UPIP:");
				oa_strcat(enquire_temp, dev_now_params.update_server_ip);
			}break;
			case e_UPPORT:{
				sprintf(enquire_temp, "UPPORT:%d", dev_now_params.update_server_port);
			}break;
			case e_UPFTPUSR:{
				oa_strcat(enquire_temp, "UPFTPUSR:");
				oa_strcat(enquire_temp, dev_now_params.ftpusr);
			}break;
			case e_UPFTPPWD:{
				oa_strcat(enquire_temp, "UPFTPPWD:");
				oa_strcat(enquire_temp, dev_now_params.ftppwd);
			}break;
			case e_UPPROG_NAME:{
				oa_strcat(enquire_temp, "UPPROG_NAME:");
				oa_strcat(enquire_temp, dev_now_params.ftp_prog_name);
			}break;
			case e_STATUS:{
				status_extract(enquire_temp);
			}break;
			case e_GPS:{
				gps_extract(enquire_temp);
			}break;
			case e_dev_lock:{
				sprintf(enquire_temp, "dev_lock:%d", now_use_lock.lock);
			}break;
			case e_UPDATE:{
				oa_strcat(enquire_temp, "doing update......");
			}break;
			case e_CLRLOG:{
				oa_strcat(enquire_temp, "CLRLOG");
			}break;
			case e_AUTHEN:{
				oa_uint8 code[AUTHEN_CODE_MAX_LEN] = 0x0;
				oa_uint8 len;
				if (read_authen_code(code, &len)){
					oa_strcat(enquire_temp, code);
				}
			}break;	
			case e_RESTART:{
				oa_strcat(enquire_temp, "RESTART OK");
			}break;
			case e_DEVID:{
				oa_strcat(enquire_temp, dev_now_params.term_id);
			}break;
			default:{
				oa_strcat(enquire_temp, "not support!");
			}break;
		}
		
 	}

	*p_fbk_len = oa_strlen(enquire_temp);
	oa_memcpy(p_fbk, enquire_temp, *p_fbk_len);
	
	
}
#endif
#if 0
/*********************************************************
*Function:      handle_keyword()
*Description:  handle the keyword
*Return:        void
*Others:         
*********************************************************/
void handle_keyword_4trans(e_keyword key_kind, keyword_context *p_set, u16 *p_act, u8 *p_fbk, u16 *p_fbk_len)
{
	oa_bool ret;
	char temp[16] = {0x0};
	char enquire_temp[64] = {0x0};
	switch (key_kind){
		case e_HB:{
			if (p_set->kind == set)	{
				if (dev_now_params.heartbeat_interval == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.heartbeat_interval = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_RSP_TCP:{
			if (p_set->kind == set)	{
				if (dev_now_params.tcp_ack_timeout == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.tcp_ack_timeout = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_RSP_UDP:{
			if (p_set->kind == set)	{
				if (dev_now_params.udp_ack_timeout == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.udp_ack_timeout = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_RSP_SMS:{
			if (p_set->kind == set)	{
				if (dev_now_params.sms_ack_timeout == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.sms_ack_timeout = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Retry_TCP:{
			if (p_set->kind == set)	{
				if (dev_now_params.tcp_retrans_times == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.tcp_retrans_times = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Retry_UDP:{
			if (p_set->kind == set)	{
				if (dev_now_params.udp_retrans_times == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.udp_retrans_times = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Retry_SMS:{
			if (p_set->kind == set)	{
				if (dev_now_params.sms_retrans_times == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.sms_retrans_times = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_IP:{
			if (p_set->kind == set){
				u8 ip_len;
				if (oa_strlen(dev_now_params.m_server_ip) == oa_strlen(p_set->context.con_ch)){//length is equal
					ip_len = oa_strlen(dev_now_params.m_server_ip);
					if (!oa_strncmp(dev_now_params.m_server_ip, p_set->context.con_ch, ip_len)){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.m_server_ip, 0x0, sizeof(dev_now_params.m_server_ip));
						oa_memcpy(dev_now_params.m_server_ip, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = reconn;
					}
				}
				else{//not equal
					//Trace("(%s:%s:%d): oa_strlen(p_set->context.con_ch):%d!", __FILE__, __func__, __LINE__, oa_strlen(p_set->context.con_ch));
					oa_memset(dev_now_params.m_server_ip, 0x0, sizeof(dev_now_params.m_server_ip));
					oa_memcpy(dev_now_params.m_server_ip, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					//Trace("(%s:%s:%d): m_server_ip:%s!", __FILE__, __func__, __LINE__, dev_now_params.m_server_ip);
					p_set->act_kind = reconn;
				}
			}		
		}break;
		case e_TCPPORT:{
			if (p_set->kind == set)	{
				if (dev_now_params.server_tcp_port == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.server_tcp_port = p_set->context.con_int;
					p_set->act_kind = reconn;
				}
			}	
		}break;
		case e_UDPPORT:{
			if (p_set->kind == set)	{
				if (dev_now_params.server_udp_port == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.server_udp_port = p_set->context.con_int;
					p_set->act_kind = reconn;
				}
			}	
		}break;
		case e_TEL:{
			if (p_set->kind == set){
				if (oa_strlen(dev_now_params.term_tel_num) == oa_strlen(p_set->context.con_ch)){
					if (!oa_strncmp(dev_now_params.term_tel_num, p_set->context.con_ch, oa_strlen(dev_now_params.term_tel_num))){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.term_tel_num, 0x0, sizeof(dev_now_params.term_tel_num));
						oa_memcpy(dev_now_params.term_tel_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{
					oa_memset(dev_now_params.term_tel_num, 0x0, sizeof(dev_now_params.term_tel_num));
					oa_memcpy(dev_now_params.term_tel_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Rpt_strategy:{
			if (p_set->kind == set)	{
				if (dev_now_params.report_strategy == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.report_strategy = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
				
			}
		}break;
		case e_Rpttime_sleep:{
			if (p_set->kind == set)	{
				if (dev_now_params.sleep_reporttime == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.sleep_reporttime = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Rpttime_def:{
			if (p_set->kind == set){
				if (dev_now_params.default_reporttime == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.default_reporttime = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_servertel:{
			if (p_set->kind == set){
				if (oa_strlen(dev_now_params.monitor_platform_num) == oa_strlen(p_set->context.con_ch)){
					if (!oa_strncmp(dev_now_params.monitor_platform_num, p_set->context.con_ch, oa_strlen(dev_now_params.monitor_platform_num))){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.monitor_platform_num, 0x0, sizeof(dev_now_params.monitor_platform_num));
						oa_memcpy(dev_now_params.monitor_platform_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					oa_memset(dev_now_params.monitor_platform_num, 0x0, sizeof(dev_now_params.monitor_platform_num));
					oa_memcpy(dev_now_params.monitor_platform_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					p_set->act_kind = para_save;
					
				}
			}
		}break;
		case e_resettel:{
			if (p_set->kind == set){
				if (oa_strlen(dev_now_params.reset_num) == oa_strlen(p_set->context.con_ch)){
					if (!oa_strncmp(dev_now_params.reset_num, p_set->context.con_ch, oa_strlen(dev_now_params.reset_num))){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;

					}
					else{//not equal
						oa_memset(dev_now_params.reset_num, 0x0, sizeof(dev_now_params.reset_num));
						oa_memcpy(dev_now_params.reset_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					oa_memset(dev_now_params.reset_num, 0x0, sizeof(dev_now_params.reset_num));
					oa_memcpy(dev_now_params.reset_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					p_set->act_kind = para_save;
					
				}
			}
		}break;
		case e_factorysettel:{
			if (p_set->kind == set){
				if (oa_strlen(dev_now_params.restore_factory_settings_num) == oa_strlen(p_set->context.con_ch)){
					if (!oa_strncmp(dev_now_params.restore_factory_settings_num, p_set->context.con_ch, oa_strlen(dev_now_params.restore_factory_settings_num))){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;

					}
					else{//not equal
						oa_memset(dev_now_params.restore_factory_settings_num, 0x0, sizeof(dev_now_params.restore_factory_settings_num));
						oa_memcpy(dev_now_params.restore_factory_settings_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					oa_memset(dev_now_params.restore_factory_settings_num, 0x0, sizeof(dev_now_params.restore_factory_settings_num));
					oa_memcpy(dev_now_params.restore_factory_settings_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					p_set->act_kind = para_save;

				}
			}
			
		}break;
		case e_alarmsmstel:{
			if (p_set->kind == set){
				if (oa_strlen(dev_now_params.terminal_sms_num) == oa_strlen(p_set->context.con_ch)){
					if (!oa_strncmp(dev_now_params.terminal_sms_num, p_set->context.con_ch, oa_strlen(dev_now_params.terminal_sms_num))){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;

					}
					else{//not equal
						oa_memset(dev_now_params.terminal_sms_num, 0x0, sizeof(dev_now_params.terminal_sms_num));
						oa_memcpy(dev_now_params.terminal_sms_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					oa_memset(dev_now_params.terminal_sms_num, 0x0, sizeof(dev_now_params.terminal_sms_num));
					oa_memcpy(dev_now_params.terminal_sms_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					p_set->act_kind = para_save;
					
				}
			}
		}break;
		case e_swh_alarmmask:{
			if (p_set->kind == set){
				if (dev_now_params.alarm_mask == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{//not equal
					dev_now_params.alarm_mask = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_swh_alarmsms:{
			if (p_set->kind == set){
				if (dev_now_params.alarm_send_sms_mask == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{//not equal
					dev_now_params.alarm_send_sms_mask = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_overspeed:{
			if (p_set->kind == set)	{
				if (dev_now_params.max_speed == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.max_speed = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_overspeedtime:{
			if (p_set->kind == set)	{
				if (dev_now_params.speed_duration == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.speed_duration = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_tireddrivetime:{
			if (p_set->kind == set)	{
				if (dev_now_params.continuous_drive_time_threshold == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.continuous_drive_time_threshold = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_min_resttime:{
			if (p_set->kind == set){
				if (dev_now_params.min_rest_time == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.min_rest_time = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_max_parktime:{
			if (p_set->kind == set){
				if (dev_now_params.max_park_time == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.max_park_time = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_provincID:{
			if (p_set->kind == set)	{
				if (dev_now_params.vehicle_province_id == (u16)p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.vehicle_province_id = (u16)p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_cityID:{
			if (p_set->kind == set){
				if (dev_now_params.vehicle_city_id == (u16)p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.vehicle_city_id = (u16)p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_carID:{
			if (p_set->kind == set){
				if (oa_strlen(dev_now_params.vehicle_license) == oa_strlen(p_set->context.con_ch)){
					if (!oa_strncmp(dev_now_params.vehicle_license, p_set->context.con_ch, oa_strlen(dev_now_params.vehicle_license))){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;

					}
					else{//not equal
						oa_memset(dev_now_params.vehicle_license, 0x0, sizeof(dev_now_params.vehicle_license));
						oa_memcpy(dev_now_params.vehicle_license, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
						oa_memset(dev_now_params.vehicle_license, 0x0, sizeof(dev_now_params.vehicle_license));
						oa_memcpy(dev_now_params.vehicle_license, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
				}
				
			}
		}break;
		case e_carcolor:{
			if (p_set->kind == set){
				if (dev_now_params.plate_color == (u8)p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.plate_color = (u8)p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_UPIP:{
			if (p_set->kind == set){
				u8 ip_len;
				if (oa_strlen(dev_now_params.update_server_ip) == oa_strlen(p_set->context.con_ch)){//length is equal
					ip_len = oa_strlen(dev_now_params.update_server_ip);
					if (!oa_strncmp(dev_now_params.update_server_ip, p_set->context.con_ch, ip_len)){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.update_server_ip, 0x0, sizeof(dev_now_params.update_server_ip));
						oa_memcpy(dev_now_params.update_server_ip, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					//Trace("(%s:%s:%d): oa_strlen(p_set->context.con_ch):%d!", __FILE__, __func__, __LINE__, oa_strlen(p_set->context.con_ch));
					oa_memset(dev_now_params.update_server_ip, 0x0, sizeof(dev_now_params.update_server_ip));
					oa_memcpy(dev_now_params.update_server_ip, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					//Trace("(%s:%s:%d): m_server_ip:%s!", __FILE__, __func__, __LINE__, dev_now_params.m_server_ip);
					p_set->act_kind = para_save;
				}
			}	
		}break;
		case e_UPPORT:{
			if (p_set->kind == set)	{
				if (dev_now_params.update_server_port == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.update_server_port = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_UPFTPUSR:{
			if (p_set->kind == set){
				u8 ip_len;
				if (oa_strlen(dev_now_params.ftpusr) == oa_strlen(p_set->context.con_ch)){//length is equal
					ip_len = oa_strlen(dev_now_params.ftpusr);
					if (!oa_strncmp(dev_now_params.ftpusr, p_set->context.con_ch, ip_len)){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.ftpusr, 0x0, sizeof(dev_now_params.ftpusr));
						oa_memcpy(dev_now_params.ftpusr, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					//Trace("(%s:%s:%d): oa_strlen(p_set->context.con_ch):%d!", __FILE__, __func__, __LINE__, oa_strlen(p_set->context.con_ch));
					oa_memset(dev_now_params.ftpusr, 0x0, sizeof(dev_now_params.ftpusr));
					oa_memcpy(dev_now_params.ftpusr, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					//Trace("(%s:%s:%d): m_server_ip:%s!", __FILE__, __func__, __LINE__, dev_now_params.m_server_ip);
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_UPFTPPWD:{
			if (p_set->kind == set){
				u8 ip_len;
				if (oa_strlen(dev_now_params.ftppwd) == oa_strlen(p_set->context.con_ch)){//length is equal
					ip_len = oa_strlen(dev_now_params.ftppwd);
					if (!oa_strncmp(dev_now_params.ftppwd, p_set->context.con_ch, ip_len)){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.ftppwd, 0x0, sizeof(dev_now_params.ftpusr));
						oa_memcpy(dev_now_params.ftppwd, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					//Trace("(%s:%s:%d): oa_strlen(p_set->context.con_ch):%d!", __FILE__, __func__, __LINE__, oa_strlen(p_set->context.con_ch));
					oa_memset(dev_now_params.ftppwd, 0x0, sizeof(dev_now_params.ftppwd));
					oa_memcpy(dev_now_params.ftppwd, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					//Trace("(%s:%s:%d): m_server_ip:%s!", __FILE__, __func__, __LINE__, dev_now_params.m_server_ip);
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_UPPROG_NAME:{
			if (p_set->kind == set){
				u8 ip_len;
				if (oa_strlen(dev_now_params.ftp_prog_name) == oa_strlen(p_set->context.con_ch)){//length is equal
					ip_len = oa_strlen(dev_now_params.ftp_prog_name);
					if (!oa_strncmp(dev_now_params.ftp_prog_name, p_set->context.con_ch, ip_len)){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.ftp_prog_name, 0x0, sizeof(dev_now_params.ftp_prog_name));
						oa_memcpy(dev_now_params.ftp_prog_name, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					//Trace("(%s:%s:%d): oa_strlen(p_set->context.con_ch):%d!", __FILE__, __func__, __LINE__, oa_strlen(p_set->context.con_ch));
					oa_memset(dev_now_params.ftp_prog_name, 0x0, sizeof(dev_now_params.ftp_prog_name));
					oa_memcpy(dev_now_params.ftp_prog_name, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					//Trace("(%s:%s:%d): m_server_ip:%s!", __FILE__, __func__, __LINE__, dev_now_params.m_server_ip);
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_STATUS:{
			
		}break;
		case e_GPS:{

		}break;
		case e_CLRLOG:{
			p_set->act_kind = clr_log;
		}break;
		case e_AUTHEN:{
			if (p_set->kind == set){
				if (p_set->context.con_ch[0] == 0xff)	p_set->act_kind = clr_authcode;
				else if (oa_strlen(p_set->context.con_ch) > 0){
					p_set->act_kind = update_authcode;
				}
			}			
		}break;
		case e_UPDATE:{
			p_set->act_kind = update;
		}break;
		case e_RESTART:{
			p_set->act_kind = reset;
		}break;
		case e_dev_lock:{
			if (p_set->kind == set){
				if (p_set->context.con_int == UNLOCK){
					if (use_is_lock())	use_unlock();
				}
				
			}
		}break;
		case e_DEVID:{
			if (p_set->kind == set){
				if (!oa_strncmp(dev_now_params.term_id, p_set->context.con_ch, DEVID_LEN)){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.term_id, 0x0, sizeof(dev_now_params.term_id));
						oa_memcpy(dev_now_params.term_id, p_set->context.con_ch, DEVID_LEN);
						p_set->act_kind = para_save;
					}
			}
		}break;
		case e_none:{
			Trace("(%s:%s:%d): not support!", __FILE__, __func__, __LINE__);
		}break;
		default:{
			Trace("(%s:%s:%d): not support!", __FILE__, __func__, __LINE__);
			break;
		}
	}
	
	if (p_set->need_ack == OA_TRUE){
		handle_common_4trans(key_kind, p_set, p_fbk, p_fbk_len);
		*p_act |= Sms_Ack_Enable;
	}
	else {
		*p_act &= ~Sms_Ack_Enable;
		*p_act |= Sms_Ack_Force_DISABLE;
	}
	switch (p_set->act_kind){
		case para_save:{
			ret = dev_params_save();
			if (ret == OA_TRUE)	print_key_dev_params();
		}break;
		case reconn:{
			ret = dev_params_save();
			if (ret == OA_TRUE)	print_key_dev_params();
			do_soc_reconn();
		}break;
		case update:{
			ftp_update();
		}break;
		case reset:{
			do_reset();
		}break;
		case clr_log:{
			clear_miles();
			del_blinddata();
		}break;
		case clr_authcode:{
			oa_bool ret;
			if (OFFLINE == dev_running.plat_status)	break;

			ret = del_authcode();
			if (OA_TRUE == ret){//if you delete authcode, you must unregsiter & reg again
				dev_running.next_step = PLAT_DEV_UNREG;
				dev_running.plat_switch = OA_TRUE;
			}
		}break;
		case update_authcode:{
			oa_bool ret = save_authen_code((oa_uint8 *)p_set->context.con_ch, 
										oa_strlen(p_set->context.con_ch));
			if (OA_TRUE == ret){//if you update authcode, you must unregsiter & reg again
				dev_running.next_step = PLAT_DEV_UNREG;
				dev_running.plat_switch = OA_TRUE;
			}
		}break;
		default:break;
	}	

	p_set->act_kind = no_act;
	
}
#endif
/*********************************************************
*Function:      sched_scrn_ana_4trans()
*Description:  analysis the schedule screen sms content
*Return:        u8
*Others:         
*********************************************************/
u8 sched_scrn_ana_4trans(u8 *p_sms, u16 sms_len, u16 *p_act, u8 * p_fbk, u16 *p_fbk_len)
{
	//do about sms
	e_keyword key_ret = e_none;
	keyword_context set = {0x0};
	oa_uint8 e_i;
	DEBUG("sms:%s len:%d", p_sms, sms_len);
	//do not support multiple sms
	for (e_i = 0;e_i < KEYWORDS_SIZE;e_i++){
		
		key_ret = lookfor_keywords_loop(p_sms, sms_len, &set, e_i, scrn)/*lookfor_keywords_4trans(p_sms, sms_len, &set, e_i)*/;
		if (e_none == key_ret){
			//Trace("(%s:%s:%d): not support!", __FILE__, __func__, __LINE__);
			continue;
		}
		//handle_keyword_4trans(key_ret, &set, p_act, p_fbk, p_fbk_len);
		handle_keyword(p_act, p_fbk, p_fbk_len, key_ret, &set, scrn);
		oa_memset(&set, 0x0, sizeof(set));
		return ActionOK;
	}
	
	return UnDefinedSms;
}

