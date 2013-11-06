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
#include "oa_fuel_sensor.h"
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
	p_data[18] = 0x7e;
	//add check
	for (i = 4; i < 18; i++) {
		check += p_data[i];
	}
	p_data[1] = check;
	return 19;
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