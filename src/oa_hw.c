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
//#define OA_GPIO_EINT_01 27
oa_bool sos_handle_polarity_0 = OA_TRUE;
/*do not modify it*/
oa_uint8 eint_data_map_tb[]={1, 6, 5, 2, 1, 1, 4, 2, 2, 2, 2, 3};
oa_uint8 eint_gpio_map_tb[]={66, 28, 5, 11, 27, 29, 33, 34, 51, 60, 74, 25};
oa_uint8 g_first_pullup_flag = OA_TRUE;

oa_uint8 acc_status = ACC_ON;
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
	if (ret){//acc is on
		//key shake
		oa_sleep(10);
		ret = oa_gpio_read(ACC_GPIO);
		if (ret){
			if (ReadAlarmPara(StaSector1, STA_ACC_ON) == RESET){
				WriteAlarmPara(SET, StaSector1, STA_ACC_ON);
			}
			acc_status = ACC_OFF;
		}	
	}
	else{//acc is off
		//key shake
		oa_sleep(10);
		ret = oa_gpio_read(ACC_GPIO);
		if (!ret){
			if (ReadAlarmPara(StaSector1, STA_ACC_ON) == SET){
				WriteAlarmPara(RESET, StaSector1, STA_ACC_ON);
			}
			acc_status = ACC_ON;
		}
	}
	//ugent alarm detect
	ret = oa_gpio_read(KEY_GPIO);
	if (ret){
		//key shake
		oa_sleep(10);
		ret = oa_gpio_read(KEY_GPIO);
		if (ret){
			if (ugent_last == OA_TRUE){
				if (ReadAlarmPara(StaAlarm0, ALARM_EMERGENCY_k) == RESET){
					handle_alarm_status(StaAlarm0, ALARM_EMERGENCY_k, SET, OA_TRUE);
				}
			}
			ugent_last = OA_TRUE;
		}
	}
	else{
		//key shake
		oa_sleep(10);
		ret = oa_gpio_read(KEY_GPIO);
		if (!ret){
			if (ReadAlarmPara(StaAlarm0, ALARM_EMERGENCY_k) == SET){
				handle_alarm_status(StaAlarm0, ALARM_EMERGENCY_k, RESET, OA_TRUE);
			}
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
