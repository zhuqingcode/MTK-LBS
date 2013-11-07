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
 *   oa_fuel_sensor.h
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
#ifndef __OA_FUEL_H__
#define __OA_FUEL_H__

#include "oa_type.h"
#include "oa_api.h"
#define VERSION 0x01
#define Manufacturer_No 0x53
#define Peripheral_Type_No 0x04
#define Fuel_Cmd_Upload 0xf0
#define Fuel_Cmd_Para_Set 0xf1
#define Fuel_Not_Support 0xff
#define Fuel_Not_Support2 0xffff
typedef enum {
	Fuel_Status_Normal = 0x0,
	Fuel_Status_Sud_Down,
	Fuel_Status_Sud_Up,
	Fuel_Status_Err = 0xf,
}Fuel_Status_enum;
#define Fuel_Per_Alarm_Shreshold 0xd0
#define Fuel_Vol_Alarm_Shreshold 0xd1
#define Fuel_Per 0xd2
#define Fuel_Vol 0xd3
#define Fuel_Para_Set_OK 0x0
#define Fuel_Para_Set_ERR 0x1
#define Fuel_Vol 0xd3
#define OA_APP_FUEL_1TIME 60000
#define OA_APP_FUEL_SETTIME 30000
#define FUEL_PARA_DATA_LEN 128
typedef struct {
	Fuel_Status_enum fuel_status;
	u16 fuel_percent;
	u16 fuel_volume;
	u16 fuel_ad;
}fuel_sensor_struct;
#endif

