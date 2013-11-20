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
#include "oa_zfz.h"
#include "oa_gps.h"
//#define OA_GPIO_EINT_01 27
oa_bool sos_handle_polarity_0 = OA_TRUE;
/*do not modify it*/
oa_uint8 eint_data_map_tb[]={1, 6, 5, 2, 1, 1, 4, 2, 2, 2, 2, 3};
oa_uint8 eint_gpio_map_tb[]={66, 28, 5, 11, 27, 29, 33, 34, 51, 60, 74, 25};
oa_uint8 g_first_pullup_flag = OA_TRUE;
#ifdef USE_ZFZ_SENSOR
//ZFZ-------------
zfz_enum zfz_sensor_status = 0;
u8 tz_times = 0;
u8 zz_cal = 0;
u8 fz_cal = 0;
#endif
oa_uint8 acc_status = ACC_OFF;
u32 adc_value = 0;
extern STRUCT_RMC Pos_Inf;
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
	adc_value = volt_result;
}
#ifdef USE_ZFZ_SENSOR
/*********************************************************
*Function:     oa_tst_eint_hisr()
*Description:  callback function for gpio     
*Return:		void
*Others:         
*********************************************************/
void oa_eint_hisr0(void)
{
	oa_uint8 eint_level;

	eint_level =oa_gpio_read(eint_gpio_map_tb[OA_CUR_TEST_EINT_NO1]);
	if (eint_level == LEVEL_LOW) {
		zz_cal++;
		tz_times = 0;
		DEBUG("zz_cal:%d", zz_cal);
	}

	if (zz_cal > ZZ_SHRESHOLD_TIMES) {
			//DEBUG("zz");
			zfz_sensor_status = zz;
			tz_times = 0;
			zz_cal = 0;
			fz_cal = 0;
	}
}
/*********************************************************
*Function:     oa_tst_eint_hisr()
*Description:  callback function for gpio     
*Return:		void
*Others:         
*********************************************************/
void oa_eint_hisr1(void)
{
	oa_uint8 eint_level;
	eint_level = oa_gpio_read(eint_gpio_map_tb[OA_CUR_TEST_EINT_NO]);
	if (eint_level == LEVEL_LOW) {
		fz_cal++;
		tz_times = 0;
		DEBUG("fz_cal:%d", fz_cal);	
	}
	
	if (fz_cal > FZ_SHRESHOLD_TIMES) {
			//DEBUG("fz");
			zfz_sensor_status = fz;
			tz_times = 0;
			fz_cal = 0;
			zz_cal = 0;
	}
	
}
#endif
/*********************************************************
*Function:     oa_gpio_set()
*Description:  gpio settings     
*Return:		void
*Others:         
*********************************************************/
#ifdef USE_ZFZ_SENSOR
void oa_interrupt_init(void){
	oa_uint8 eint_level;
	//GPIO66:EINT0;
	oa_gpio_mode_setup(eint_gpio_map_tb[OA_CUR_TEST_EINT_NO], eint_data_map_tb[OA_CUR_TEST_EINT_NO]);
	oa_gpio_init(0,eint_gpio_map_tb[OA_CUR_TEST_EINT_NO]);
	oa_eint_registration(OA_CUR_TEST_EINT_NO, OA_TRUE, 1, oa_eint_hisr0, OA_TRUE);
	oa_eint_set_sensitivity(OA_CUR_TEST_EINT_NO, EDGE_SENSITIVE);
	oa_eint_set_debounce(OA_CUR_TEST_EINT_NO, 10);
	//GPIO5:EINT2;
	oa_gpio_mode_setup(eint_gpio_map_tb[OA_CUR_TEST_EINT_NO1], 0);    
	oa_gpio_init(0,eint_gpio_map_tb[OA_CUR_TEST_EINT_NO1]);
	//pulldown
	oa_gpio_pull_setup(eint_gpio_map_tb[OA_CUR_TEST_EINT_NO1], 1);
	oa_gpio_pull_selhigh(eint_gpio_map_tb[OA_CUR_TEST_EINT_NO1], 0);

	oa_gpio_mode_setup(eint_gpio_map_tb[OA_CUR_TEST_EINT_NO1], eint_data_map_tb[OA_CUR_TEST_EINT_NO1]);
	oa_gpio_init(0,eint_gpio_map_tb[OA_CUR_TEST_EINT_NO1]);
	oa_eint_registration(OA_CUR_TEST_EINT_NO1, OA_TRUE, 1, oa_eint_hisr1, OA_TRUE);
	oa_eint_set_sensitivity(OA_CUR_TEST_EINT_NO1, EDGE_SENSITIVE);
	oa_eint_set_debounce(OA_CUR_TEST_EINT_NO1, 10);
}
#endif
void pull_down_acc(void) {
	oa_gpio_mode_setup(ACC_GPIO, GPIO_MODE);     /*set gpio 15 as 0 mode(gpio modem)*/
	oa_gpio_init(GPIO_INPUT, ACC_GPIO);
	//pulldown ACC
	DEBUG("pull down ACC");
	oa_gpio_pull_setup(ACC_GPIO, 1);
	oa_gpio_pull_selhigh(ACC_GPIO, 0);
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
	//oa_gpio_mode_setup(ACC_GPIO, GPIO_MODE);     /*set gpio 15 as 0 mode(gpio modem)*/
	oa_gpio_mode_setup(GPSANA_GPIO, GPIO_MODE);
	//oa_gpio_init(GPIO_INPUT, ACC_GPIO);
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
	oa_gpio_write(OIL_POWER_ENABLE, OUT_GPIO);//if it used as oil & power control, connect oil & power defaultly
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
	static oa_uint8 key_cal = 0;
	static oa_uint8 acc_on_cal = 0;
	static oa_uint8 acc_off_cal = 0;
	ret = oa_gpio_read(ACC_GPIO);
	if (!ret){//acc is off
		//key shake
		ret = oa_gpio_read(ACC_GPIO);
		if (!ret){
			acc_off_cal++;
			if (acc_off_cal > 2) {
				if (ReadAlarmPara(StaSector1, STA_ACC_ON) == SET){
					WriteAlarmPara(RESET, StaSector1, STA_ACC_ON);
					handle_alarm_status(StaSector1, STA_ACC_ON, RESET, OA_TRUE);
					DEBUG("ACC status changed:off");
				}
				acc_off_cal = 0;
				acc_on_cal = 0;
				acc_status = ACC_OFF;
				//power off screen
				oa_gpio_write(0, SCRN_POWER);
			}
		}	
	}
	else{//acc is on
		//key shake
		ret = oa_gpio_read(ACC_GPIO);
		if (ret){
			acc_on_cal++;
			if (acc_on_cal > 2) {
				if (ReadAlarmPara(StaSector1, STA_ACC_ON) == RESET){
					WriteAlarmPara(SET, StaSector1, STA_ACC_ON);
					handle_alarm_status(StaSector1, STA_ACC_ON, SET, OA_TRUE);
					DEBUG("ACC status changed:on");
				}
				acc_on_cal = 0;
				acc_off_cal = 0;
				acc_status = ACC_ON;
				//power on screen
				oa_gpio_write(1, SCRN_POWER);
			}
		}
	}
	//ugent alarm detect
	ret = oa_gpio_read(KEY_GPIO);
	if (!ret){
		//key shake
		ret = oa_gpio_read(KEY_GPIO);
		if (!ret){
				key_cal++;
				if (key_cal > 2) {
					if (ReadAlarmPara(StaAlarm0, ALARM_EMERGENCY_k) == RESET){
						handle_alarm_status(StaAlarm0, ALARM_EMERGENCY_k, SET, OA_TRUE);
						handle_alarm_sms(ALARM_EMERGENCY_k, 1, inout_no, 0);
						DEBUG("key alarm");
					}
					key_cal = 0;
				}
				
		}
	}

#ifdef USE_ZFZ_SENSOR
	//zfz sensor
	tz_times++;
	if (tz_times * OA_ACC_RUN_SECOND > TZ_SHRESHOLD_TIME) {
		if (acc_status == ACC_OFF) {
			zfz_sensor_status = tz;
		}
		zz_cal = 0;
		fz_cal = 0;
		tz_times = 0;
	}
#endif
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
