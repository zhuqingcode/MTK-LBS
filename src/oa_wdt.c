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
#include "oa_debug.h"
#include "oa_platform.h"
extern DEV_PLAT_PARAS dev_running;
/*********************************************************
*Function:      oa_app_online_offline()
*Description:  oa_app_online_offline
*Return:        void
*Others:         
*********************************************************/
void oa_app_online_offline(void)
{
	static u32 offline_times = 0;
	static u32 sim_times = 0;
	static oa_bool first = OA_FALSE;
	if (OA_FALSE == first) {
		DEBUG("(:(:(:(:(:(:(:(:task is %s running:):):):):):):):)", __func__);
		first = OA_TRUE;
	}
	//online/offline checkout
	if (dev_running.plat_status == OFFLINE) {
		offline_times++;
		if (offline_times * OA_WDT_SCHEDULER_SECONDS > OFFLINE_RESTART_SHRESHOLD) {
			DEBUG("offline time > 25min, reboot!");
			oa_module_restart(NULL);
		}
	} else if (dev_running.plat_status == ONLINE) {
		offline_times = 0;
	}
	//sim card checkout
	if (!oa_sim_network_is_valid()) {
		sim_times++;
		if (sim_times * OA_WDT_SCHEDULER_SECONDS > SIM_RESTART_SHRESHOLD) {
			DEBUG("sim card is invalid time > 5min, reboot!");
			oa_module_restart(NULL);
		}
	} else {
		sim_times = 0;
	}
	oa_timer_start(OA_TIMER_ID_1, oa_app_online_offline, NULL, OA_WDT_SCHEDULER_PERIOD);
	return;
}