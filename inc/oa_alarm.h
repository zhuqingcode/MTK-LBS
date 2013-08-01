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
*   oa_alarm.h
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
#ifndef __OA_ALARM_H__
#define __OA_ALARM_H__
#include "oa_type.h"
#include "oa_api.h"
typedef enum
{
	no_spec,
	os_circle,
	os_rect,
	os_poly,
	line_sec,
	no_os = 0xff,
}os_kind;

typedef struct
{
	os_kind kind;
	u32 id;
}os_struct;

typedef enum
{
	out2in,
	in2out,
}in_out_kind;

typedef struct
{
	os_kind area_kind;
	u32 id;
	in_out_kind in_out;
}area_alarm_addition_struct;

typedef struct
{
	oa_uint8 buf[64];
	oa_uint8 len;
}alarm_struct;


#endif