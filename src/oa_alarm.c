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
*   oa_alarm.c
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
#include "oa_alarm.h"
#include "oa_jt808.h"
#include "oa_dev_params.h"
#include "oa_platform.h"
#include "oa_hw.h"
#include "oa_debug.h"
#include "oa_sms.h"
extern DEVICE_PARAMS dev_now_params;
extern DEV_PLAT_PARAS dev_running;
extern oa_uint8 acc_status;
extern oa_sms_context sms_fail;
extern oa_bool sms_enable;
extern void sms_send_feedback_func(os_sms_result send_ret);
/*********************************************************
*Function:     handle_alarm_status()
*Description:  handle alarm status
*Return:		
*Others:         
*********************************************************/
oa_bool handle_alarm_status(STA_ALARM part, u32 alarm_bit, flag_status status, oa_bool flag)
{

	u32 alarm_flag;
	oa_bool ret;
	oa_uint16 build_ret;
	oa_uint16 soc_ret;
	
	if (part >= StatusNum){
		DEBUG("params err!");
		return OA_FALSE;
	}
	if (flag == OA_TRUE){
		alarm_flag = dev_now_params.alarm_mask;
		if (alarm_flag & alarm_bit){//alarm is masked
			DEBUG("this alarm is masked!");
			return OA_TRUE;
		}
		else{//alarm is unmasked
			if (SET == status){
				if (ReadAlarmPara(part, alarm_bit) == RESET)	WriteAlarmPara(SET, part, alarm_bit);
			}
			else if (RESET == status){
				if (ReadAlarmPara(part, alarm_bit) == SET)	WriteAlarmPara(RESET, part, alarm_bit);
			}
			
			
			if (dev_running.plat_status == OFFLINE){
				ret = DevReq2ServPackag_build_blind(REPORT_LOCATION);
				if (ret)	return OA_TRUE;
				else return OA_FALSE;
			}
			
			build_ret = DevReq2ServPackag_build(REPORT_LOCATION);//upload instantly<!!!>
			if (build_ret > 0){
				soc_ret = oa_soc_send_req();//check datas in buffer & send, concern it send ok.
				if (soc_ret != build_ret){
					DEBUG("send data is not equal!");
					return OA_FALSE;
				}
				else{
					return OA_TRUE;
				}
			}
			else{
				DEBUG("build packet err!");
				return OA_FALSE;
			}
			
		}
		//sms alarm
		alarm_flag = dev_now_params.alarm_send_sms_mask;
		if ((alarm_flag & alarm_bit) && sms_enable == OA_TRUE){
			char enquire_temp[32] = {0x0};
			oa_char nb_tmp[4] = {0x0};
			nb_kind nb = err_nb;
			
			switch (alarm_bit){
				case ALARM_EMERGENCY_k:{
					oa_strcat(enquire_temp, "ALARM_EMERGENCY_k");
				}break;
				case ALARM_OVER_SPEED:{
					oa_strcat(enquire_temp, "ALARM_OVER_SPEED");
				}break;
				case ALARM_DRIVE_TIRED:{
					oa_strcat(enquire_temp, "ALARM_DRIVE_TIRED");
				}break;
				case ALARM_GNSS_ERR:{
					oa_strcat(enquire_temp, "ALARM_GNSS_ERR");
				}break;
				case ALARM_GNSS_ANTENNA:{
					oa_strcat(enquire_temp, "ALARM_GNSS_ANTENNA");
				}break;
				case ALARM_GNSS_SHORT_CIRCUIT:{
					oa_strcat(enquire_temp, "ALARM_GNSS_SHORT_CIRCUIT");
				}break;
				case ALARM_DRIVE_OVERTIME:{
					oa_strcat(enquire_temp, "ALARM_DRIVE_OVERTIME");
				}break;
				case ALARM_OVERTIME_PARKING:{
					oa_strcat(enquire_temp, "ALARM_OVERTIME_PARKING");
				}break;
				case ALARM_ENTER_AREA:{
					oa_strcat(enquire_temp, "ALARM_ENTER_AREA");
				}break;
				default:return;
			}

			oa_strncpy(nb_tmp, &dev_now_params.terminal_sms_num[0], 3);
			nb = telecom_num_check(nb_tmp);
			if (tele_nb == nb){
				//DEBUG("enquire_temp:%s nb:%s", enquire_temp, message.deliver_num);
				oa_sms_test_dfalp(enquire_temp, dev_now_params.terminal_sms_num);
			}
			else if (err_nb != nb){
				oa_sms_send_req(sms_send_feedback_func, dev_now_params.terminal_sms_num, enquire_temp, oa_strlen(enquire_temp), OA_SMSAL_DEFAULT_DCS);
				oa_memcpy(sms_fail.data, enquire_temp, oa_strlen(enquire_temp));
				sms_fail.len = oa_strlen(enquire_temp);
				oa_memcpy(sms_fail.deliver_num, dev_now_params.terminal_sms_num, oa_strlen(dev_now_params.terminal_sms_num));
				sms_fail.dcs = OA_SMSAL_DEFAULT_DCS;
			}
		}
	}
	else{//just send

		if (dev_running.plat_status == OFFLINE){
			ret = DevReq2ServPackag_build_blind(REPORT_LOCATION);
			if (ret)	return OA_TRUE;
			else return OA_FALSE;
		}
		
		build_ret = DevReq2ServPackag_build(REPORT_LOCATION);
		if (build_ret > 0){
			soc_ret = oa_soc_send_req();//check datas in buffer & send, concern it send ok.
			if (soc_ret != build_ret){
				DEBUG("send data is not equal!");
				return OA_FALSE;
			}
			else{
				return OA_TRUE;
			}
		}
		else{
			DEBUG("build packet err!");
			return OA_FALSE;
		}
	}
	

}
