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
}os_kind;

typedef struct
{
	os_kind kind;
	u32 id;
}os_struct;
#endif