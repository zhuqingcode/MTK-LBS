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
#include "oa_debug.h"
oa_bool set_time = OA_FALSE;
/*********************************************************
*Function:     set_rtc_time()
*Description:  set rtc time
*Return:		
*Others:         
*********************************************************/
u8 set_rtc_time(u8 *time_str)
{
	oa_time_struct t = {0};
	
	if (NULL == time_str){
		DEBUG("paras err!");
		return 1;
	}
	
	t.nYear = 2000 + ((time_str[0]>>4)&0x0F)*10+ (time_str[0]&0x0F);
	t.nMonth = ((time_str[1]>>4)&0x0F)*10+ (time_str[1]&0x0F);//-1
	t.nDay = ((time_str[2]>>4)&0x0F)*10+ (time_str[2]&0x0F);
	t.nHour = ((time_str[3]>>4)&0x0F)*10+ (time_str[3]&0x0F);
	t.nMin = ((time_str[4]>>4)&0x0F)*10+ (time_str[4]&0x0F);
	t.nSec = ((time_str[5]>>4)&0x0F)*10+ (time_str[5]&0x0F);
	oa_set_time(&t);
	set_time = OA_TRUE;
}
/*********************************************************
*Function:     get_rtc_time()
*Description:  get rtc time
*Return:		
*Others:         
*********************************************************/
u8 get_rtc_time(u8 *time_str)
{
	oa_time_struct t = {0};
	
	if (NULL == time_str){
		DEBUG("paras err!");
		return 1;
	}
	
	if (OA_FALSE == set_time){
		time_str[0] = 0;
		time_str[1] = 0;
		time_str[2] = 0;
		time_str[3] = 0;
		time_str[4] = 0;
		time_str[5] = 0;
		return 1;
	}
	oa_get_time(&t);
	time_str[0]= ( (t.nYear/10%10)<<4)+t.nYear%10;
	time_str[1]= ( (t.nMonth/10%10)<<4)+t.nMonth%10;
	time_str[2]= ( (t.nDay/10%10)<<4)+t.nDay%10;
	time_str[3]= ( (t.nHour/10%10)<<4)+t.nHour%10;
	time_str[4]= ( (t.nMin/10%10)<<4)+t.nMin%10;
	time_str[5]= ( (t.nSec/10%10)<<4)+t.nSec%10;
	return 0;
}
/*********************************************************
*Function:     print_rtc_time()
*Description:  print_rtc_time
*Return:		
*Others:         
*********************************************************/
void print_rtc_time(void)
{
	u8 time[6] = {0x0};
	get_rtc_time(time);
	Trace("Time: %x %x %x %x:%x:%x ",time[0],time[1],time[2],time[3],time[4],time[5]);
}