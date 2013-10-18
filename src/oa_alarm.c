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
#include "oa_app.h"
#include "oa_lbs2mtk.h"
#include "oa_soc.h"
oa_uint8 emergency_uni[] = {0x7D,0x27,0x60,0x25,0x62,0xA5,0x8B,0x66};
oa_uint8 overspeed_uni[] = {0x8D,0x85,0x90,0x1F,0x62,0xA5,0x8B,0x66};
oa_uint8 tired_drive_uni[] = {0x75,0xB2,0x52,0xB3,0x9A,0x7E,0x9A,0x76};
oa_uint8 gnss_err_uni[] = {0x0,'G',0x0,'N',0x0,'S',0x0,'S',0x6A,0x21,0x57,0x57,0x53,0xD1,0x75,0x1F,0x65,0x45,0x96,0x9C};
oa_uint8 gnss_atenna_uni[] = {0x0,'G',0x0,'N',0x0,'S',0x0,'S',0x59,0x29,0x7E,0xBF,0x67,0x2A,0x63,0xA5,0x62,0x16,0x88,0xAB,0x52,0x6A,0x65,0xAD};
oa_uint8 gnss_short_circuit_uni[] = {0x0,'G',0x0,'N',0x0,'S',0x0,'S',0x59,0x29,0x7E,0xBF,0x77,0xED,0x8D,0xEF};
oa_uint8 drive_overtime_uni[] = {0x5F,0x53,0x59,0x29,0x7D,0x2F,0x8B,0xA1,0x9A,0x7E,0x9A,0x76,0x8D,0x85,0x65,0xF6};
oa_uint8 overtime_park_uni[] = {0x8d,0x85,0x65,0xf6,0x50,0x5c,0x8f,0x66};
oa_uint8 enter_area_uni[] = {0x8F,0xDB,0x53,0x3A,0x57,0xDF};
oa_uint8 out_area_uni[] = {0x51,0xFA,0x53,0x3A,0x57,0xDF};
oa_uint8 enter_derection_uni[] = {0x8F,0xDB,0x51,0xFA,0x8D,0xEF,0x7E,0xBF};

oa_uint8 emergency_gbk[] = {0xBD,0xf4,0xbc,0xb1,0xb1,0xA8,0xbe,0xaf};
oa_uint8 overspeed_gbk[] = {0xb3,0xac,0xcb,0xd9,0xb1,0xA8,0xbe,0xaf};
oa_uint8 tired_drive_gbk[] = {0xc6,0xa3,0xc0,0xcd,0xbc,0xdd,0xca,0xbb};
oa_uint8 gnss_err_gbk[] = {'G','N','S','S',0xc4,0xa3,0xbf,0xe9,0xb7,0xa2,0xc9,0xfa,0xb9,0xca,0xd5,0xcf};
oa_uint8 gnss_atenna_gbk[] = {'G','N','S','S',0xcc,0xec,0xcf,0xdf,0xce,0xb4,0xbd,0xd3,0xbb,0xf2,0xb1,0xbb,0xbc,0xf4,0xb6,0xcf};
oa_uint8 gnss_short_circuit_gbk[] = {'G','N','S','S',0xcc,0xec,0xcf,0xdf,0xb6,0xcc,0xc2,0xb7};
oa_uint8 drive_overtime_gbk[] = {0xb5,0xb1,0xcc,0xec,0xc0,0xdb,0xbc,0xc6,0xbc,0xdd,0xca,0xbb,0xb3,0xac,0xca,0xb1};
oa_uint8 overtime_park_gbk[] = {0xb3,0xac,0xca,0xb1,0xcd,0xa3,0xb3,0xb5};
oa_uint8 enter_area_gbk[] = {0xbd,0xf8,0xc7,0xf8,0xd3,0xf2};
oa_uint8 out_area_gbk[] = {0xb3,0xf6,0xc7,0xf8,0xd3,0xf2};
oa_uint8 enter_derection_gbk[] = {0xbd,0xf8,0xb3,0xf6,0xc2,0xb7,0xcf,0xdf};

extern DEVICE_PARAMS dev_now_params;
extern DEV_PLAT_PARAS dev_running;
extern oa_uint8 acc_status;
extern oa_bool sms_enable;
extern timeout_struct timeout_var;
extern scrn_struct s_s;
extern void oa_screen_demo(void *param);
extern oa_soc_context g_soc_context;
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
	oa_int16 build_ret;
	oa_int16 soc_ret;
	
	if (part >= StatusNum){
		DEBUG("params err!");
		return OA_FALSE;
	}
	if (flag == OA_TRUE){
		if (part == StaAlarm0) alarm_flag = dev_now_params.alarm_mask;
		else if (part == StaSector1) alarm_flag = 0;
		if (alarm_flag & alarm_bit){//alarm is masked
			DEBUG("this alarm is masked!");
			return OA_TRUE;
		}
		else{//alarm is unmasked
			if (SET == status){
				WriteAlarmPara(SET, part, alarm_bit);
			}
			else if (RESET == status){
				WriteAlarmPara(RESET, part, alarm_bit);
			}
			

			if (dev_running.plat_status == OFFLINE || timeout_var.do_timeout == OA_TRUE
				|| g_soc_context.is_blocksend == OA_TRUE){
				ret = DevReq2ServPackag_build_blind(REPORT_LOCATION);
				if (ret)	return OA_TRUE;
				else return OA_FALSE;
			}
			
			build_ret = DevReq2ServPackag_build(REPORT_LOCATION);//upload instantly<!!!>
			if (build_ret > 0){
				soc_ret = oa_soc_send_req();//check datas in buffer & send, concern it send ok.
				if (soc_ret == SOC_SEND_ERR){
					DEBUG("send data err!");
					DevReq2ServPackag_build_blind(REPORT_LOCATION);
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
	else{//just send

		if (dev_running.plat_status == OFFLINE || timeout_var.do_timeout == OA_TRUE
			|| g_soc_context.is_blocksend == OA_TRUE){
			ret = DevReq2ServPackag_build_blind(REPORT_LOCATION);
			if (ret)	return OA_TRUE;
			else return OA_FALSE;
		}
		
		build_ret = DevReq2ServPackag_build(REPORT_LOCATION);
		if (build_ret > 0){
			soc_ret = oa_soc_send_req();//check datas in buffer & send, concern it send ok.
			if (soc_ret == SOC_SEND_ERR){
				DEBUG("send data err!");
				DevReq2ServPackag_build_blind(REPORT_LOCATION);
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
/*********************************************************
*Function:     handle_alarm_status()
*Description:  handle alarm status
*Return:		
*Others:         
*********************************************************/
void handle_alarm_sms(u32 alarm_bit, oa_uint8 alarm_2_driver, in_out_kind kind, u32 area_id)
{
	u32 alarm_flag;
	alarm_struct uni;
	alarm_struct gbk;
	oa_memset(&uni, 0x0, sizeof(uni));
	oa_memset(&gbk, 0x0, sizeof(gbk));
	//sms alarm
	alarm_flag = dev_now_params.alarm_mask;
	if (alarm_flag & alarm_bit){//alarm is masked
		DEBUG("this alarm is masked!");
		return;
	}
	alarm_flag = dev_now_params.alarm_send_sms_mask;
	
	switch (alarm_bit){
		case ALARM_EMERGENCY_k:{
			if ((alarm_flag & alarm_bit) && sms_enable) {
				oa_memcpy(uni.buf, emergency_uni, sizeof(emergency_uni));
				uni.len = sizeof(emergency_uni);
			}

			if (alarm_2_driver) {
				oa_memcpy(gbk.buf, emergency_gbk, sizeof(emergency_gbk));
				gbk.len = sizeof(emergency_gbk);
			}
			
		}break;
		case ALARM_OVER_SPEED:{
			if ((alarm_flag & alarm_bit) && sms_enable) {
				oa_memcpy(uni.buf, overspeed_uni, sizeof(overspeed_uni));
				uni.len = sizeof(overspeed_uni);
			}

			if (alarm_2_driver) {
				oa_memcpy(gbk.buf, overspeed_gbk, sizeof(overspeed_gbk));
				gbk.len = sizeof(overspeed_gbk);
			}
			
		}break;
		case ALARM_DRIVE_TIRED:{
			if ((alarm_flag & alarm_bit) && sms_enable) {
				oa_memcpy(uni.buf, tired_drive_uni, sizeof(tired_drive_uni));
				uni.len = sizeof(tired_drive_uni);
			}

			if (alarm_2_driver) {
				oa_memcpy(gbk.buf, tired_drive_gbk, sizeof(tired_drive_gbk));
				gbk.len = sizeof(tired_drive_gbk);
			}
			
		}break;
		case ALARM_GNSS_ERR:{
			if ((alarm_flag & alarm_bit) && sms_enable){
				oa_memcpy(uni.buf, gnss_err_uni, sizeof(gnss_err_uni));
				uni.len = sizeof(gnss_err_uni);
			}
			

			if (alarm_2_driver) {
				oa_memcpy(gbk.buf, gnss_err_gbk, sizeof(gnss_err_gbk));
				gbk.len = sizeof(gnss_err_gbk);
			}
			
		}break;
		case ALARM_GNSS_ANTENNA:{
			if ((alarm_flag & alarm_bit) && sms_enable) {
				oa_memcpy(uni.buf, gnss_atenna_uni, sizeof(gnss_atenna_uni));
				uni.len = sizeof(gnss_atenna_uni);
			}
			
			if (alarm_2_driver) {
				oa_memcpy(gbk.buf, gnss_atenna_gbk, sizeof(gnss_atenna_gbk));
				gbk.len = sizeof(gnss_atenna_gbk);
			}
			
		}break;
		case ALARM_GNSS_SHORT_CIRCUIT:{
			if ((alarm_flag & alarm_bit) && sms_enable) {
				oa_memcpy(uni.buf, gnss_short_circuit_uni, sizeof(gnss_short_circuit_uni));
				uni.len = sizeof(gnss_short_circuit_uni);
			}
			
			if (alarm_2_driver) {
				oa_memcpy(gbk.buf, gnss_short_circuit_gbk, sizeof(gnss_short_circuit_gbk));
				gbk.len = sizeof(gnss_short_circuit_gbk);
			}
			
		}break;
		case ALARM_DRIVE_OVERTIME:{
			if ((alarm_flag & alarm_bit) && sms_enable) {
				oa_memcpy(uni.buf, drive_overtime_uni, sizeof(drive_overtime_uni));
				uni.len = sizeof(drive_overtime_uni);
			}
			
			if (alarm_2_driver) {
				oa_memcpy(gbk.buf, drive_overtime_gbk, sizeof(drive_overtime_gbk));
				gbk.len = sizeof(drive_overtime_gbk);
			}
			
		}break;
		case ALARM_OVERTIME_PARKING:{
			if ((alarm_flag & alarm_bit) && sms_enable) {
				oa_memcpy(uni.buf, overtime_park_uni, sizeof(overtime_park_uni));
				uni.len = sizeof(overtime_park_uni);
			}
			
			if (alarm_2_driver) {
				oa_memcpy(gbk.buf, overtime_park_gbk, sizeof(overtime_park_gbk));
				gbk.len = sizeof(overtime_park_gbk);
			}
			
		}break;
		case ALARM_ENTER_AREA:{
			u8 temp[64] = {0x0};
			u8 temp2[64] = {0x0};
			u16 len;
			if ((alarm_flag & alarm_bit) && sms_enable) {
				if (kind == out2in) {
					oa_memcpy(uni.buf, enter_area_uni, sizeof(enter_area_uni));
					uni.len = sizeof(enter_area_uni);
					sprintf(temp, " id:%d", area_id);
					len = asc2uc(temp2, temp, oa_strlen(temp));
					oa_memcpy(&uni.buf[uni.len], temp2, len);
					uni.len += len;
				} else if (kind == in2out) {
					oa_memcpy(uni.buf, out_area_uni, sizeof(out_area_uni));
					uni.len = sizeof(out_area_uni);
					sprintf(temp, " id:%d", area_id);
					len = asc2uc(temp2, temp, oa_strlen(temp));
					oa_memcpy(&uni.buf[uni.len], temp2, len);
					uni.len += len;
				}
				
			}
			
			if (alarm_2_driver) {
				oa_memset(temp, 0x0, sizeof(temp));
				oa_memset(temp2, 0x0, sizeof(temp2));
				if (kind == out2in) {
					oa_memcpy(gbk.buf, enter_area_gbk, sizeof(enter_area_gbk));
					gbk.len = sizeof(enter_area_gbk);
					sprintf(temp, " id:%d", area_id);
					len = oa_strlen(temp);
					oa_memcpy(&gbk.buf[gbk.len], temp, len);
					gbk.len += len;
				} else if (kind == in2out) {
					oa_memcpy(gbk.buf, out_area_gbk, sizeof(out_area_gbk));
					gbk.len = sizeof(out_area_gbk);
					sprintf(temp, " id:%d", area_id);
					len = oa_strlen(temp);
					oa_memcpy(&gbk.buf[gbk.len], temp, len);
					gbk.len += len;
				}
			}
			
		}break;
		case ALARM_TOUCH_LINE_k:{
			if ((alarm_flag & alarm_bit) && sms_enable) {
				oa_memcpy(uni.buf, enter_derection_uni, sizeof(enter_derection_uni));
				uni.len = sizeof(enter_derection_uni);
			}
			
			if (alarm_2_driver) {
				oa_memcpy(gbk.buf, enter_derection_gbk, sizeof(enter_derection_gbk));
				gbk.len = sizeof(enter_derection_gbk);
			}
			
		}break;
		default:{
			DEBUG("not sopport!");
			return;
		}
	}

	if ((alarm_flag & alarm_bit) && sms_enable == OA_TRUE){
		DEBUG("send sms");
		oa_sms_test_ucs2(uni.buf, dev_now_params.terminal_sms_num, uni.len);
	}

	if (alarm_2_driver) {
		oa_memset(&s_s, 0x0, sizeof(s_s));
		s_s.buflen = gbk.len;
		oa_memcpy(s_s.sendbuf, gbk.buf, s_s.buflen);
		oa_timer_start(OA_TIMER_ID_12, oa_screen_demo, NULL, 3000);
	}
	
}
