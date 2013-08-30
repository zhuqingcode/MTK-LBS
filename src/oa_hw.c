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
 *   oa_hw.c
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
#include "oa_type.h"
#include "oa_api.h"
#include "oa_hw.h"
#include "oa_jt808.h"
#include "oa_debug.h"
#include "oa_alarm.h"
//#define OA_GPIO_EINT_01 27
oa_bool sos_handle_polarity_0 = OA_TRUE;
/*do not modify it*/
oa_uint8 eint_data_map_tb[]={1, 6, 5, 2, 1, 1, 4, 2, 2, 2, 2, 3};
oa_uint8 eint_gpio_map_tb[]={66, 28, 5, 11, 27, 29, 33, 34, 51, 60, 74, 25};
oa_uint8 g_first_pullup_flag = OA_TRUE;

oa_uint8 acc_status = ACC_ON;
extern os_struct overspeed_var;
extern area_alarm_addition_struct area_alarm_addition_var;
/*********************************************************
*Function:     oa_tst_eint_hisr()
*Description:  callback function for gpio     
*Return:		void
*Others:         
*********************************************************/
void oa_tst_eint_hisr(void)
{
	oa_uint8 eint_level;
	static oa_uint8 first_reverse = OA_FALSE;
	/*only a test trace, better not to print any traces in EINT HISR.*/
	//DEBUG("%s,oa_tst_eint_hisr called: %d", __FILE__, __func__,sos_handle_polarity_0);
	eint_level =oa_gpio_read(eint_gpio_map_tb[OA_CUR_TEST_EINT_NO]);
	if(eint_level == 0)
	{
		sos_handle_polarity_0 = 1;
		first_reverse = OA_TRUE;
	}
	else
	{
		sos_handle_polarity_0 = 0;
	}
	//sos_handle_polarity_0 = !sos_handle_polarity_0;
	if((first_reverse == OA_FALSE) 
	  && (g_first_pullup_flag == OA_TRUE))
	{
		//dummy func
		DEBUG("dummy eint func for test");
	}
	else
	{
		//add eint handle func
		DEBUG("%s,eint_level:%d,soa_tst_eint_hisr polarity: %d", __FILE__, __func__,eint_level,sos_handle_polarity_0);
	}
	oa_eint_set_polarity(OA_CUR_TEST_EINT_NO, sos_handle_polarity_0);
}
/*********************************************************
*Function:     get_adc_value()
*Description:  get_adc_value
*Return:		void
*Others:         
*********************************************************/
void adc_value_cb(oa_int32 volt_result, oa_double adc_result){
	//OA_DEBUG_USER("volt_result:%d adc_result:%f", volt_result, adc_result);
}
/*********************************************************
*Function:     oa_gpio_set()
*Description:  gpio settings     
*Return:		void
*Others:         
*********************************************************/
void oa_gpio_set(void)
{
#ifdef EINT
	oa_uint8 eint_level;
	/*example to configurate EINT*/ 
	oa_gpio_mode_setup(eint_gpio_map_tb[OA_CUR_TEST_EINT_NO], 0);    
	oa_gpio_init(0,eint_gpio_map_tb[OA_CUR_TEST_EINT_NO]);
	eint_level =oa_gpio_read(eint_gpio_map_tb[OA_CUR_TEST_EINT_NO]);
	if(eint_level == 0)//pulldown
	{
		g_first_pullup_flag = OA_FALSE;
		sos_handle_polarity_0 = 1;
	}
	else //pullup
	{
		g_first_pullup_flag = OA_TRUE;
		sos_handle_polarity_0 = 0;
	}    

	//DEBUG("sos_handle_polarity_0: %d", sos_handle_polarity_0);

	oa_gpio_mode_setup(eint_gpio_map_tb[OA_CUR_TEST_EINT_NO], eint_data_map_tb[OA_CUR_TEST_EINT_NO]);
	oa_gpio_init(0,eint_gpio_map_tb[OA_CUR_TEST_EINT_NO]);
	oa_eint_registration(OA_CUR_TEST_EINT_NO, OA_TRUE, sos_handle_polarity_0, oa_tst_eint_hisr, OA_TRUE);
	oa_eint_set_sensitivity(OA_CUR_TEST_EINT_NO, LEVEL_SENSITIVE);
	oa_eint_set_debounce(OA_CUR_TEST_EINT_NO, 80);
#endif
	//just for ACC & GPS ana detect
	oa_gpio_mode_setup(ACC_GPIO, GPIO_MODE);     /*set gpio 15 as 0 mode(gpio modem)*/
	oa_gpio_mode_setup(GPSANA_GPIO, GPIO_MODE);
	oa_gpio_init(GPIO_INPUT, ACC_GPIO);
	oa_gpio_init(GPIO_INPUT, GPSANA_GPIO);
	//key
	oa_gpio_mode_setup(KEY_GPIO, GPIO_MODE);
	oa_gpio_init(GPIO_INPUT, KEY_GPIO);
	//IN0 & IN1
	oa_gpio_mode_setup(IN0_GPIO, GPIO_MODE);
	oa_gpio_init(GPIO_INPUT, IN0_GPIO);
	oa_gpio_mode_setup(IN1_GPIO, GPIO_MODE);
	oa_gpio_init(GPIO_INPUT, IN1_GPIO);
	//OUT
	oa_gpio_mode_setup(OUT_GPIO, GPIO_MODE);
	oa_gpio_init(GPIO_OUTPUT, OUT_GPIO);
	//SCRN POWER
	oa_gpio_mode_setup(SCRN_POWER, GPIO_MODE);
	oa_gpio_init(GPIO_OUTPUT, SCRN_POWER);
	oa_gpio_write(0, SCRN_POWER);
	//power high
	oa_gpio_mode_setup(POWER_HIGH, GPIO_MODE);
	oa_gpio_init(GPIO_OUTPUT, POWER_HIGH);
	oa_gpio_write(1, POWER_HIGH);
	//adc init:if you want shut down ADC,just call 'oa_adc_get_data(4, 0, NULL);'
	oa_adc_get_data(4, 0, adc_value_cb);
}
/*********************************************************
*Function:     acc_status_detect()
*Description:  detect acc status on/off
*Return:		OA_FALSE:off OA_TRUE:on
*Others:         
*********************************************************/
void acc_status_detect(void *param)
{
	//detect acc status on/off
	oa_uint8 ret;
	static oa_bool ugent_last = OA_FALSE;
	
	ret = oa_gpio_read(ACC_GPIO);
	if (ret){//acc is off
		//key shake
		ret = oa_gpio_read(ACC_GPIO);
		if (ret){
			if (ReadAlarmPara(StaSector1, STA_ACC_ON) == SET){
				WriteAlarmPara(RESET, StaSector1, STA_ACC_ON);
				overspeed_var.kind = no_os;
				area_alarm_addition_var.area_kind = no_spec;
				handle_alarm_status(StaSector1, STA_ACC_ON, RESET, OA_TRUE);
				DEBUG("ACC status changed:off");
			}
			acc_status = ACC_OFF;
			//power off screen
			oa_gpio_write(0, SCRN_POWER);
		}	
	}
	else{//acc is on
		//key shake
		ret = oa_gpio_read(ACC_GPIO);
		if (!ret){
			if (ReadAlarmPara(StaSector1, STA_ACC_ON) == RESET){
				WriteAlarmPara(SET, StaSector1, STA_ACC_ON);
				handle_alarm_status(StaSector1, STA_ACC_ON, SET, OA_TRUE);
				DEBUG("ACC status changed:on");
			}
			acc_status = ACC_ON;
			//power on screen
			oa_gpio_write(1, SCRN_POWER);
		}
	}
	//ugent alarm detect
	ret = oa_gpio_read(KEY_GPIO);
	if (ret){
		//key shake
		ret = oa_gpio_read(KEY_GPIO);
		if (ret){
			if (ugent_last == OA_TRUE){
				//if (ReadAlarmPara(StaAlarm0, ALARM_EMERGENCY_k) == RESET){
					overspeed_var.kind = no_os;
					area_alarm_addition_var.area_kind = no_spec;
					handle_alarm_status(StaAlarm0, ALARM_EMERGENCY_k, SET, OA_TRUE);
					handle_alarm_sms(ALARM_EMERGENCY_k, 1, inout_no, 0);
					DEBUG("key alarm");
				//}
			}
			ugent_last = OA_TRUE;
		}
	}
	else{
		//key shake
		ret = oa_gpio_read(KEY_GPIO);
		if (!ret){
			//if (ReadAlarmPara(StaAlarm0, ALARM_EMERGENCY_k) == SET){
			//	WriteAlarmPara(RESET, StaAlarm0, ALARM_EMERGENCY_k);
			//	DEBUG("cancel key alarm");
			//}
			ugent_last = OA_FALSE;
		}
	}
redoit:
	oa_timer_start(OA_TIMER_ID_6, acc_status_detect, NULL, OA_ACC_RUN);
}
/*********************************************************
*Function:     gps_ana_detect()
*Description:  detect acc status on/off
*Return:		OA_FALSE:off OA_TRUE:on
*Others:        low is valid
*********************************************************/
oa_bool gps_ana_detect(void)
{
	//detect acc status on/off
	oa_uint8 ret;
	ret = oa_gpio_read(GPSANA_GPIO);
	if (ret)	return OA_FALSE;
	else		return OA_TRUE;
}
