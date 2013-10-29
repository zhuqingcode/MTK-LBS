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
 *   oa_hw_test.c
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
#include "oa_hw_test.h"
#include "oa_gps.h"
extern oa_uint8 acc_status;
extern STRUCT_RMC Pos_Inf;
extern u32 adc_value;
/*********************************************************
*Function:     send_hw_status()
*Description:  send_hw_status     
*Return:		void
*Others:         
*********************************************************/
void send_hw_status(u8 *data, u8 len) {
	Host_Send_Packag(0xD7, data, len);
}
/*********************************************************
*Function:     oa_hardware_test()
*Description:  oa_hardware_test     
*Return:		void
*Others:         
*********************************************************/
void oa_hardware_test(void *param) {
	u8 hw_status[24] = {0x0};
	u32 lon;
	u32 lat;
	STRUCT_RMC gps_info;

	DEBUG("sending hardware info......");
	//check hardware status
	//BYTE0---------------
	//IN0
	hw_status[0] |= oa_gpio_read(IN0_GPIO);
	//IN1
	hw_status[0] |= oa_gpio_read(IN1_GPIO) << 1;
	//BYTE1---------------
	//Power status
	hw_status[1] |= 0x1<<7;
	//ACC status
	hw_status[1] |= acc_status<<6;
	//GPS ANA
	hw_status[1] |= 0x1<<5;
	//Urgent alarm
	hw_status[1] |= oa_gpio_read(KEY_GPIO)<<4;
	//BYTE2----------------
	hw_status[2] = oa_network_get_signal_level();
	//BYTE7~8--------------
	hw_status[7] = adc_value && 0xff;
	hw_status[8] = (adc_value>>8) && 0xff;
	//BYTE11---------------
	if (GPS_FIXED == Pos_Inf.Fix_Status) {
		hw_status[11] = 0x1;
	} else {
		hw_status[11] = 0x0;
	}
	//BYTE12~15------------
	GetPosinf((u8 *)&lon, GPSLon, 5);
	int_to_char_Little_Endian(&hw_status[12], lon);
	//BYTE16~19
	GetPosinf((u8 *)&lat, GPSLat, 5);
	int_to_char_Little_Endian(&hw_status[16], lat);
	memset(&gps_info, 0x0, sizeof(gps_info));
	GPS_GetPosition(&gps_info);//copy gps data into 'gps_info'
	//BYTE20~21
	hw_status[20] = gps_info.Speed & 0xff;
	hw_status[21] = (gps_info.Speed>>8) & 0xff;
	send_hw_status(hw_status, sizeof(hw_status));
	oa_timer_start(OA_TIMER_ID_13, oa_hardware_test, NULL, HW_TEST_TIME);
}