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
 *   oa_at.c
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
#include "oa_at.h"
#include "oa_platform.h"
#include "oa_dev_params.h"
#include "oa_debug.h"

extern DEV_PLAT_PARAS dev_running;
extern DEVICE_PARAMS dev_now_params;
#define POWEROFF_TIME 2000
/*********************************************************
*Function:      do_reset()
*Description:  reset device  
*Return:        void
*Others:         
*********************************************************/
void do_reset(void)
{
	if (ONLINE == dev_running.plat_status){
		oa_soc_send_req();//send last packet before close soc	
		dev_running.plat_check_value = OA_FALSE;//do not check device is online/offline
		if (oa_soc_close_req()){
			dev_running.plat_status = OFFLINE;
		}
	}
	DEBUG("someone let me reset myself , so..., goodbye......");
	oa_sleep(POWEROFF_TIME);
	oa_module_restart(NULL);
}
/*********************************************************
*Function:      do_factory_set()
*Description:  factory set device  
*Return:        void
*Others:         
*********************************************************/
void do_factory_set(void)
{
	if (ONLINE == dev_running.plat_status){
		oa_soc_send_req();//send last packet before close soc	
		dev_running.plat_check_value = OA_FALSE;//do not check device is online/offline
		if (oa_soc_close_req()){
			dev_running.plat_status = OFFLINE;
		}
	}
	DEBUG("someone let me restore the factory settings , so..., goodbye......");
	oa_sleep(POWEROFF_TIME);
	factory_set();
}
/*********************************************************
*Function:      do_powerdown()
*Description:  powerdown device  
*Return:        void
*Others:         
*********************************************************/
void do_powerdown(void)
{
	if (ONLINE == dev_running.plat_status){
		oa_soc_send_req();//send last packet before close soc	
		dev_running.plat_check_value = OA_FALSE;//do not check device is online/offline
		if (oa_soc_close_req()){
			dev_running.plat_status = OFFLINE;
		}
	}
	DEBUG("someone let me powerdown myself, so..., goodbye......in 3 second");
	oa_sleep(POWEROFF_TIME);//delay just for tcp 
	oa_power_shutdown(NULL);
}
/*********************************************************
*Function:      do_turnoff_datatraffic()
*Description:  turn off the data traffic of device  
*Return:        void
*Others:         
*********************************************************/
void do_turnoff_datatraffic(void)
{
	
	if (ONLINE == dev_running.plat_status){
		oa_soc_send_req();//send last packet before close soc	
		dev_running.plat_check_value = OA_FALSE;//do not check device is online/offline
		if (oa_soc_close_req()){
			dev_running.plat_status = OFFLINE;
		}
	}
	oa_sleep(POWEROFF_TIME);
}
/*********************************************************
*Function:      do_turnoff_datatraffic()
*Description:  turn off the data traffic of device  
*Return:        void
*Others:         
*********************************************************/
void do_turnoff_allwireless(void)
{
	
	if (ONLINE == dev_running.plat_status){
		oa_soc_send_req();//send last packet before close soc	
		dev_running.plat_check_value = OA_FALSE;//do not check device is online/offline
		if (oa_soc_close_req()){
			dev_running.plat_status = OFFLINE;
		}
	}
	oa_sleep(POWEROFF_TIME);
	//maybe need do something about sms & call here
}
/*********************************************************
*Function:     do_something_before_reconn()
*Description:  do_something_before_reconn
*Return:		
*Others:         
*********************************************************/
void do_something_before_reconn(void)
{
	params_to_soc_set();
	oa_soc_setting_save();
	/*Setting parameters*/
	oa_soc_setting_init();
	//fill 'g_soc_context' with current paras
	oa_soc_fill_addr_struct();
}
/*********************************************************
*Function:     do_soc_reconn()
*Description:  do_soc_reconn
*Return:		
*Others:         
*********************************************************/
void do_soc_reconn(void)
{
	
	//close soc
	if (ONLINE == dev_running.plat_status){
		oa_soc_send_req();//send last packet before close soc	
		//close socket
		if (oa_soc_close_req()){//after testing it doesn't cause "oa_soc_notify_ind_user_callback" 
			DEBUG("I close soc in order to reconnect!");
			dev_running.plat_status = OFFLINE;
			dev_running.plat_switch = OA_TRUE;
			dev_running.next_step = PLAT_RECONN;
		}
	}
	oa_sleep(POWEROFF_TIME);
	do_something_before_reconn();
}
/*********************************************************
*Function:     just_reconn()
*Description:  just_reconn
*Return:		
*Others:         
*********************************************************/
void just_reconn(void)
{
	
	//close soc
	if (ONLINE == dev_running.plat_status){
		oa_soc_send_req();//send last packet before close soc	
		//close socket
		if (oa_soc_close_req()){//after testing it doesn't cause "oa_soc_notify_ind_user_callback" 
			DEBUG("I close soc in order to reconnect!");
			dev_running.plat_status = OFFLINE;
			dev_running.plat_switch = OA_TRUE;
			dev_running.next_step = PLAT_RECONN;
		}
	}
	oa_sleep(POWEROFF_TIME);
}
/*********************************************************
*Function:     just_close_soc()
*Description:  just_close_soc and do not reconnect it
*Return:		
*Others:         
*********************************************************/
void just_close_soc(void)
{
	
	//close soc
	if (ONLINE == dev_running.plat_status){
		oa_soc_send_req();//send last packet before close soc	
		//close socket
		if (oa_soc_close_req()){//after testing it doesn't cause "oa_soc_notify_ind_user_callback" 
			DEBUG("I close soc and do not reconnect!");
			dev_running.plat_status = OFFLINE;
			//dev_running.plat_switch = OA_TRUE;
			//dev_running.next_step = PLAT_RECONN;
		}
	}
	oa_sleep(POWEROFF_TIME);
}
/*********************************************************
*Function:     ftp_update()
*Description:  ftp_update
*Return:		
*Others:         
*********************************************************/
void ftp_update(void)
{
	oa_char para[64] = {0x0};
	oa_char tmp[16] = {0x0};
	oa_strcat(para, "ftp:");
	oa_strcat(para, dev_now_params.update_server_ip);
	oa_strcat(para, ",");
	sprintf(tmp , "%d,", dev_now_params.update_server_port);
	oa_strcat(para, tmp);
	oa_strcat(para, dev_now_params.ftpusr);
	oa_strcat(para, ",");
	oa_strcat(para, dev_now_params.ftppwd);
	oa_strcat(para, ",");
	oa_strcat(para, dev_now_params.ftp_prog_name);
	oa_strcat(para, ",0");//8:print download bar & restart after finish
	//oa_strcat(para, ",openatdll.dll,0");
	DEBUG("update param:%s", para);
	oa_start_ftp_upgrade(para);
}