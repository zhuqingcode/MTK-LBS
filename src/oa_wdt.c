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
*   oa_wdt.c
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
#include "oa_wdt.h"

/*********************************************************
*Function:      oa_app_wdt()
*Description:  fill the watchdog
*Return:        void
*Others:         
*********************************************************/
void oa_app_wdt(void)
{
	//fill the watchdog
	//......
	static oa_bool task_runed = OA_TRUE;
	if (OA_TRUE == task_runed)
	{
		OA_DEBUG_USER("task %s is running......", __FILE__, __func__);
		task_runed = OA_FALSE;
	}
	oa_timer_start(OA_TIMER_ID_1, oa_app_wdt, NULL, OA_WDT_SCHEDULER_PERIOD);
	return;
}