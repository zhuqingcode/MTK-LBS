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

extern DEVICE_PARAMS dev_now_params;
extern DEV_PLAT_PARAS dev_running;
extern oa_uint8 acc_status;
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
				if (!ret)	return OA_TRUE;
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
	}
	else{

		if (dev_running.plat_status == OFFLINE){
			ret = DevReq2ServPackag_build_blind(REPORT_LOCATION);
			if (!ret)	return OA_TRUE;
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
