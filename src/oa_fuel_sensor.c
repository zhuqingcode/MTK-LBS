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
 *   oa_fuel_sensor.c
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
#include "oa_debug.h"
#include "oa_hw.h"
#include "oa_gps.h"
#include "oa_jt808.h"
#include "oa_fuel_sensor.h"
#include "oa_dev_params.h"
#ifndef USE_SCREEN
extern u32 adc_value;
extern oa_uint8 acc_status;
extern STRUCT_RMC Pos_Inf;
extern DEVICE_PARAMS dev_now_params;
fuel_sensor_struct fuel_sensor_var = {Fuel_Status_Normal, 0x0, 0x0, 0x0};
/*********************************************************
*Function:      oa_app_fuel()
*Description:  fuel parameters set
*Return:        void
*Others:         
*********************************************************/
u8 fuel_build_part_packet(u8 *p_data) {
	u8 i, check = 0;
	if (NULL == p_data) {
		DEBUG("p_data err!");
		return 0;
	}
	p_data[0] = 0x7e;
	p_data[2] = VERSION;
	p_data[3] = VERSION;
	p_data[4] = Manufacturer_No;
	p_data[5] = Manufacturer_No;
	p_data[6] = Peripheral_Type_No;
	p_data[7] = Fuel_Cmd_Para_Set;
	//D0
	p_data[8] = 0xd0;
	p_data[9] = 0x0F;/*example*/
	//D1
	p_data[10] = 0xd1;
	p_data[11] = 0x0A;/*example*/
	//D2
	p_data[12] = 0xd2;
	short_to_char(&p_data[13], 6854/*example*/);
	//D3
	p_data[15] = 0xd3;
	short_to_char(&p_data[16], 1200/*example*/);
	//D4
	p_data[18] = 0xd4;
	short_to_char(&p_data[19], 120/*example*/);
	p_data[21] = 0x7e;
	
	//add check
	for (i = 4; i < 21; i++) {
		check += p_data[i];
	}
	p_data[1] = check;
	return 22;
}
/*********************************************************
*Function:      oa_app_fuel()
*Description:  fuel parameters set
*Return:        void
*Others:         
*********************************************************/
void oa_app_fuel(void) {
	u8 fuel_data[FUEL_PARA_DATA_LEN] = {0x0};
	u8 len;
	u16 temp;
	
	len = fuel_build_part_packet(fuel_data);
	JT808_dataChg(1, fuel_data + 1, len - 2, &temp);
	len = temp + 2; //total data length
	oa_uart_write(OA_UART3, fuel_data, len);
	oa_timer_start(OA_TIMER_ID_14, oa_app_fuel, NULL, OA_APP_FUEL_SETTIME);
}
/*********************************************************
*Function:     save_st_ad()
*Description:  save_st_ad
*Return:		OA_FALSE:off OA_TRUE:on
*Others:        low is valid
*********************************************************/
oa_bool save_st_ad(JDUGE_PARAM *p) {
	JDUGE_PARAM temp;
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	
	handle = oa_fopen(STANDAND_AD_FILE);
	if (handle < 0)
	{
		/* create new file for setting. */
		handle = oa_fcreate(STANDAND_AD_FILE);
		/* hope never return here. */
		if (handle < 0)
		{
			DEBUG("Create st ad file failed!");
			return OA_FALSE;
		}

		oa_memset(&temp, 0x0, sizeof(temp));
		
		ret = oa_fwrite(handle, &temp, sizeof(temp), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(temp)))
		{
			DEBUG("Init st ad file failed!");
			return OA_FALSE;
		}

		DEBUG("Create st ad file ok!");    

		
	}
	oa_fseek(handle, 0, OA_FILE_BEGIN);
	oa_memcpy(&temp, p, sizeof(temp));
	//write 
	ret = oa_fwrite(handle, &temp, sizeof(temp), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(temp)))
	{
		DEBUG("write st ad file failed!");
		oa_fclose(handle);
		return OA_FALSE;
	}

	oa_fclose(handle);
	return OA_TRUE;
}
/*********************************************************
*Function:     save_st_ad()
*Description:  save_st_ad
*Return:		OA_FALSE:off OA_TRUE:on
*Others:        low is valid
*********************************************************/
oa_bool read_st_ad(JDUGE_PARAM *p)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	JDUGE_PARAM temp;
	handle = oa_fopen(STANDAND_AD_FILE);
	if (handle < 0) {
		return OA_FALSE;
	}
	oa_fseek(handle, 0, OA_FILE_BEGIN);
	/* read setting to g_soc_param. */
	ret = oa_fread(handle, &temp, sizeof(temp), &dummy_read);
	if((ret < 0) || (dummy_read != sizeof(temp))) goto fail;

	ret = oa_fclose(handle);
	oa_memcpy(p, &temp, sizeof(temp));
	return OA_TRUE;
  
fail:
	//need delete the file and reset here..
	oa_fclose(handle);
	return OA_FALSE;
}
/*********************************************************
*Function:     oa_app_ad()
*Description:  detect acc status on/off
*Return:		OA_FALSE:off OA_TRUE:on
*Others:        low is valid
*********************************************************/
oa_bool look4_oil_vol(u16 ad_value, u16 *oil_value) {
	JDUGE_PARAM temp;
	oa_bool ret;
	u16 oil_temp_value;
	u16 i;
	ret = read_st_ad(&temp);
	if (OA_TRUE == ret) {
		for (i = 0; i < temp.JDUGE_Data_Num; i++) {
			if (ad_value <= temp.data[i].measure_value) {
				break;
			}
		}

		if (i > 0) {
			DEBUG("look for oil volume!");
			DEBUG("measure_value:%d real_value:%d", temp.data[i].measure_value, temp.data[i].real_value);
			DEBUG("measure_value:%d real_value:%d", temp.data[i -1].measure_value, temp.data[i-1].real_value);
			oil_temp_value = ((temp.data[i].real_value*ad_value)-(temp.data[i-1].real_value*ad_value)
						-(temp.data[i-1].measure_value*temp.data[i].real_value)
						+(temp.data[i-1].real_value*temp.data[i].measure_value))
						/(temp.data[i].measure_value-temp.data[i-1].measure_value);
		} else {
			oil_temp_value = temp.data[0].real_value;
		}

		*oil_value = oil_temp_value;
		return OA_TRUE;
	} else {
		DEBUG("read oil volume err!");
		return OA_FALSE;
	}
	

}
/*********************************************************
*Function:     oa_app_ad()
*Description:  detect acc status on/off
*Return:		OA_FALSE:off OA_TRUE:on
*Others:        low is valid
*********************************************************/
#define AD_SAMPLES 120
void oa_app_ad(void *para) {
	oa_uint32 ad_value;
	static oa_uint32 ad_array[AD_SAMPLES] = {0x0};
	static oa_uint8 ad_p = 0;
	oa_uint8 i;
	oa_uint32 ad_add = 0;
	oa_bool ret;
	u16 oil_value;
	//.....
	static u16 fuel_vol_last = 0;
	static u16 times = 0;
	static oa_bool fuel_record = OA_FALSE;
	//adc init:if you want shut down ADC,just call 'oa_adc_get_data(4, 0, NULL);'
	ad_value = 2/*·ÖÑ¹ÏµÊý*/ * oa_adc_get_data(4, 1, NULL);
	adc_value = ad_value/1000;
	DEBUG("adc_value:%d(mv)", adc_value);
#if 1
	if (ad_p < AD_SAMPLES) {
		ad_array[ad_p] = ad_value/1000;
		ad_p++;
	} else if (ad_p >= AD_SAMPLES) {
		ad_p = 0;
		for (i = 0; i < AD_SAMPLES; i++) {
			ad_add += ad_array[i];
		}
		//DEBUG("average_ad_value:%d(mv)", ad_add/AD_SAMPLES);
		ret = look4_oil_vol(ad_add/AD_SAMPLES, &oil_value);
#else
		ret = look4_oil_vol(adc_value, &oil_value);
#endif
		
		if (ret == OA_TRUE) {
			//get oil vol * 10
			fuel_sensor_var.fuel_volume = oil_value * 10;
			DEBUG("oil_vol:%d", fuel_sensor_var.fuel_volume);
			//steal fuel
			if (Pos_Inf.Speed == 0 || acc_status == ACC_OFF) {
				//record it
				if (fuel_record == OA_FALSE) {
					fuel_record = OA_TRUE;
					fuel_vol_last = fuel_sensor_var.fuel_volume;
				}
				//5min
				times++;
				if (times > Fuel_Check_Shreshold_Time) {
					times = 0;
					if (fuel_vol_last - fuel_sensor_var.fuel_volume > 10 * dev_now_params.def_oil_shreshold) {
						DEBUG("ALARM_OIL_ERR!");
						handle_alarm_status(StaAlarm0, ALARM_OIL_ERR, SET, OA_TRUE);
						WriteAlarmPara(RESET, StaAlarm0, ALARM_OIL_ERR);
					}
				}
			} else if (Pos_Inf.Speed > 0) {
				fuel_record = OA_FALSE;
				fuel_vol_last = 0;
				times = 0;
			}
		}
	}
	oa_timer_start(OA_TIMER_ID_15, oa_app_ad, NULL, 1000);
}
#endif