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
 *   oa_rtc.c
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
void set_rtc_time(u8 *time_str)
{
	u8 err;
	oa_time_struct t = {0};
	t.nYear = 2000 + ((time_str[0]>>4)&0x0F)*10+ (time_str[0]&0x0F);
	t.nMonth = ((time_str[1]>>4)&0x0F)*10+ (time_str[1]&0x0F);//-1
	t.nDay = ((time_str[2]>>4)&0x0F)*10+ (time_str[2]&0x0F);
	t.nHour = ((time_str[3]>>4)&0x0F)*10+ (time_str[3]&0x0F);
	t.nMin = ((time_str[4]>>4)&0x0F)*10+ (time_str[4]&0x0F);
	t.nSec = ((time_str[5]>>4)&0x0F)*10+ (time_str[5]&0x0F);
	oa_set_time(&t);
}