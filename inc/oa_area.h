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
*   oa_area.h
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
#ifndef __OA_AREA_H__
#define __OA_AREA_H__
#include "oa_type.h"
#include "Area_Judge.h"
//#define AREA_DIR_CONF L"area_dir_conf.ini"
//#define AREA_DIRNAME L"area.dir"
//#define CIRCLE_AREA_DATA L"c:\\area.dir\\circle_area_data"
//#define RECT_AREA_DATA L"c:\\area.dir\\rect_area_data"
//#define PLOY_AREA_DATA L"c:\\area.dir\\ploy_area_data"
#define CIRCLE_AREA_DATA L"circle_area_data"
#define RECT_AREA_DATA L"rect_area_data"
#define POLY_AREA_DATA L"poly_area_data"
#define OA_AREA_DETECT_TIME 5000//5s
#define OA_AREA_DETECT_1TIME 20000//5s
#define OA_AREA_RUN_SECOND (OA_AREA_DETECT_TIME/1000)
#define OA_AREA_ALARM_PERIOD 300//60s

typedef struct
{
	u32 area_id;//区域ID
	Area_Para_struct area_para;	  //区域属性
	u32 center_point_lat;
	u32 center_point_lon;
	u32 radius;
	u8 start_time[6]; //开始时间  如果区域属性depend_time无效则无该位
	u8 stop_time[6];  //结束时间  如果区域属性depend_time无效则无该位
	u16 max_speed;  //区域限速  当区域属性1位为1时有效
	u8 continue_time; //持续时间  单位为秒 当区域属性1位为1时有效
	u8 is_valid;
}circle_area_item;

typedef struct
{
	u32 area_id;//区域ID
	Area_Para_struct area_para;	  //区域属性
	u32 left_up_lat;
	u32 left_up_lon;
	u32 right_down_lat;
	u32 right_down_lon;
	u8 start_time[6]; //开始时间  如果区域属性depend_time无效则无该位
	u8 stop_time[6];  //结束时间  如果区域属性depend_time无效则无该位
	u16 max_speed;  //区域限速  当区域属性1位为1时有效
	u8 continue_time; //持续时间  单位为秒 当区域属性1位为1时有效
	u8 is_valid;
}rect_area_item;

typedef struct
{
	u32 vertax_lat;
	u32 vertax_lon;
}vertax_struct;

typedef struct
{
	u32 area_id;//区域ID
	Area_Para_struct area_para;	  //区域属性
	u8 start_time[6]; //开始时间  如果区域属性depend_time无效则无该位
	u8 stop_time[6];  //结束时间  如果区域属性depend_time无效则无该位
	u16 max_speed;  //区域限速  当区域属性1位为1时有效
	u8 continue_time; //持续时间  单位为秒 当区域属性1位为1时有效
	u16 total_point;
	vertax_struct vertax[MAX_AREA_POINTS];
	u8 is_valid;
}poly_area_item;

typedef enum
{
	invalid = 0x0,
	valid = 0xff,	
}area_valid_enum;

typedef enum
{
	file_read,
	file_write,
}r_w_enum;

typedef enum
{
	one_area,
	all_area,
}del_option_enum;

typedef enum
{
	update_area = 0x0,
	add_area,
	alter_area,
	none_area,
}option_kinds_enum;

typedef enum
{
	area_err,
	area_inside,
	area_outside,
}area_status_enum;
#endif