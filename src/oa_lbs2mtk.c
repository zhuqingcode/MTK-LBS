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
extern char *strtok(char s[], const char *delim);
extern DEVICE_PARAMS dev_now_params;
extern USE_LOCK now_use_lock;
extern oa_uint8 acc_status;
extern STRUCT_RMC Pos_Inf;
extern DEV_PLAT_PARAS dev_running;
extern oa_uint8 KEYWORDS_SIZE;
extern oa_char *p_keyword[];
extern u32 try_unlock;
scrn_struct s_s;
/*********************************************************
*Function:     oa_sms_demo()
*Description:  application entry     
*Return:		void
*Others:         
*********************************************************/
void oa_screen_demo(void *param)
{
	DEBUG("send sms 2 screen");
	if (s_s.buflen > 0)	SScrn_CenterSMS_Send(s_s.sendbuf, s_s.buflen);
	else DEBUG("len err!");
}
/*********************************************************
*Function:      sched_scrn_ana_4trans()
*Description:  analysis the schedule screen sms content
*Return:        u8
*Others:         
*********************************************************/
u8 sched_scrn_ana_4trans(u8 *p_sms, u16 sms_len, u16 *p_act, u8 * p_fbk, u16 *p_fbk_len)
{
	e_keyword key_ret = e_none;
	keyword_context set = {0x0};
	oa_uint8 e_i, i = 0;
	oa_uint8 sn = 0;
	oa_char *p = NULL;
	oa_char prefix[MAX_SMS_NUM][64] = {{0x0}, {0x0}, {0x0}, {0x0}, {0x0}};
	oa_char data[256] = {0x0};
	oa_char sendbuf[256] = {0x0};
	oa_char buf[256] = {0x0};
	oa_uint8 len;
	oa_bool ms_ack;
	u32 try_unlock_inside = 0;
	sms_kind t_s = sms_normal;
	
	DEBUG("sms:%s len:%d", p_sms, sms_len);
	len = sms_len;
	oa_memcpy(data, p_sms, len);
	p = strtok(data, ";");
	if (NULL == p){
		DEBUG("format err");
		return;
	}
	while(NULL != p){
		if (i <= MAX_SMS_NUM){
			oa_strcpy(prefix[i], p);
			i++;
			p = strtok(NULL, ";");
		}
		else break;
	}

	if (prefix[i-1][0] == 'A' || prefix[i-1][0] == 'a' && prefix[i-1][1] == 0x0){
		sn = i -1;
	}
	else if (prefix[i-1][0] == 'N' || prefix[i-1][0] == 'n' && prefix[i-1][1] == 0x0){
		sn = i -1;
	}
	else{
		sn = i;
	}
	ms_ack = OA_TRUE;
	//add ";"
	for (i = 0;i < sn; i++){
		prefix[i][oa_strlen(prefix[i])] = ';';
		DEBUG("%s", prefix[i]);
	}

	
	if (sn <= MAX_SMS_NUM){
		DEBUG("%d x sms", sn);
		for (i = 0; i < sn; i++){
			for (e_i = 0;e_i < KEYWORDS_SIZE;e_i++){
				key_ret = look4keywords4ms(prefix[i], oa_strlen(prefix[i]), &set, e_i, scrn);
				if (e_none == key_ret){
					continue;
				}
				
				handle_keyword4ms(key_ret, &set);
				if (ms_ack == OA_TRUE){
					handle_common4ms(key_ret, buf, &len, scrn, &set);
					DEBUG("\nbuf:%s len:%d", buf, len);
					if (len == 0) return UnDefinedSms;
					if (set.s_k == sms_special){
						t_s = sms_special;
						oa_memcpy(sendbuf, buf, len);
					}
					else if (set.s_k == sms_normal){
						t_s = sms_normal;
						oa_strcat(sendbuf, buf);
					}
					oa_memset(buf, 0x0, sizeof(buf));
				}
				dev_action_handle(&set, scrn);
				if (set.kind == 0x1 && use_is_lock()){
					try_unlock_inside |= TRY_UNLOCK_BIT;
					if (set.act_kind == reconn) {
						try_unlock_inside |= NEED_RECONN_BIT;
					}
				}
				oa_memset(&set, 0x0, sizeof(set));
			}
		}

		if (try_unlock_inside & TRY_UNLOCK_BIT) {
			DEBUG("try unlock");
			try_unlock |= TRY_UNLOCK_BIT;
			dev_running.plat_switch = OA_TRUE;
			dev_running.next_step = PLAT_SOC_INIT;
			if (try_unlock_inside & NEED_RECONN_BIT) {
				try_unlock |= NEED_RECONN_BIT;
			}
		}
		
		if (ms_ack == OA_TRUE){
			if (sms_special == t_s){
				*p_fbk_len = len;
				oa_memcpy(p_fbk, sendbuf, *p_fbk_len);
				*p_act |= CHINESE_SMS_ENABLE;
			}
			else if(sms_normal == t_s){
				*p_fbk_len = oa_strlen(sendbuf);
				oa_memcpy(p_fbk, sendbuf, *p_fbk_len);
			}

			*p_act |= Sms_Ack_Enable;
			return ActionOK;
		}

		
		
	}
	else DEBUG("too many sms");
	return UnDefinedSms;
}

