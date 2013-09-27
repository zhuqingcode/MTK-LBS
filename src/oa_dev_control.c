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
#include "oa_jt808.h"
#include "oa_sms.h"
extern DEV_PLAT_PARAS dev_running;
extern DEVICE_PARAMS dev_now_params;
extern oa_bool sms_enable;
extern oa_sms_context message;
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
	oa_sleep(2000);
	oa_module_restart(NULL);
}
/*********************************************************
*Function:      set_reset_flag()
*Description:  reset device  
*Return:        void
*Others:         
*********************************************************/
void set_reset_flag(sms_or_uart which, oa_uint8 *p_con)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_write;
	reset_struct r_s = {0x0};
	
	handle = oa_fopen(RESTART_FILE);
	if (handle < 0)
	{
		/* create new file for setting. */
		handle = oa_fcreate(RESTART_FILE);
		/* hope never return here. */
		if (handle < 0)
		{
			DEBUG("Create restart file failed!");
			return;
		}

		oa_memset(&r_s, 0x0, sizeof(r_s));
		ret = oa_fwrite(handle, &r_s, sizeof(r_s), &dummy_write);
		if((ret < 0) || (dummy_write != sizeof(r_s)))
		{
			DEBUG("Init restart file failed!");
			return;
		}
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	r_s.flag = OA_TRUE;
	if (which == sms)	oa_strcat(r_s.sms_nb, message.deliver_num);
	r_s.s_u = which;
	oa_strcpy(r_s.sms_con, p_con);
	ret = oa_fwrite(handle, &r_s, sizeof(r_s), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(r_s)))
	{
		DEBUG("write err!");
		return;
	}

	oa_fclose(handle);
}
/*********************************************************
*Function:      set_reset_flag()
*Description:  reset device  
*Return:        void
*Others:         
*********************************************************/
oa_bool need_send_sms_after_reset(void)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read;
	reset_struct r_s;
	oa_uint8 con[32] = {0x0};
	handle = oa_fopen(RESTART_FILE);
	if (handle < 0) return OA_FALSE;

	ret = oa_fread(handle, &r_s, sizeof(r_s), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(r_s))) goto fail;
	
	if (r_s.flag == OA_TRUE){
		DEBUG("send restart sms");
		oa_memcpy(con, r_s.sms_con, oa_strlen(r_s.sms_con));
		if (r_s.s_u == sms) oa_sms_test_dfalp(con, r_s.sms_nb);
		else if (r_s.s_u == scrn) SScrn_CenterSMS_Send(con, oa_strlen(con));
	}

	oa_fclose(handle);
	oa_fdelete(RESTART_FILE);
	return OA_TRUE;
fail:
	//need delete the file and reset here..
	oa_fclose(handle);
	oa_fdelete(RESTART_FILE);
	return OA_FALSE;
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
	sms_enable = OA_FALSE;
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
*Function:     do_socset_b4_unlock()
*Description:  do_socset_b4_unlock
*Return:		
*Others:         
*********************************************************/
void do_socset_b4_unlock(){
	params_to_soc_set();
	oa_soc_setting_save();
	/*Setting parameters*/
	oa_soc_setting_init();
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
		}
	}

	oa_sleep(POWEROFF_TIME);
	do_something_before_reconn();
	dev_running.plat_switch = OA_TRUE;
	dev_running.next_step = PLAT_RECONN;
	DEBUG("do reconnect!");
}
/*********************************************************
*Function:     do_rereg()
*Description:  do_rereg
*Return:		
*Others:         
*********************************************************/
void do_rereg(void)
{	
	oa_bool ret;
	ret = del_authcode();
	if (OA_TRUE == ret){
		DEBUG("delete authen ok");
		if (OFFLINE == dev_running.plat_status){
			dev_running.next_step = PLAT_DEV_REG;
			dev_running.plat_switch = OA_TRUE;
		}		
		else if (ONLINE == dev_running.plat_status){//if you delete authcode, you must unregsiter & reg again
			dev_running.next_step = PLAT_DEV_UNREG;
			dev_running.plat_switch = OA_TRUE;
		}
	}
	DEBUG("do rereg!");
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
			
		}
	}
	dev_running.plat_switch = OA_TRUE;
	dev_running.next_step = PLAT_RECONN;
	oa_sleep(POWEROFF_TIME);
	DEBUG("do reconnect!");
}
/*********************************************************
*Function:     just_close_soc()
*Description:  just_close_soc and do not reconnect it
*Return:		
*Others:         
*********************************************************/
void conn2specific_server(upgrade_paras *p)
{
	DEBUG("server info: ip:%s port:%d", p->ip, p->port);
	if (!oa_memcmp(p->ip, dev_now_params.m_server_ip, 16)) return;
	fill_soc(p->ip, p->port);
	del_authcode();
	just_reconn();
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
void ftp_update(upgrade_paras *p)
{
	oa_char para[64] = {0x0};
	oa_char tmp[16] = {0x0};
	oa_bool ret;
	oa_strcat(para, "ftp:");
	if (p == NULL) oa_strcat(para, dev_now_params.update_server_ip);
	else oa_strcat(para, p->ip);
	oa_strcat(para, ",");
	if (p == NULL) sprintf(tmp , "%d,", dev_now_params.update_server_port);
	else{
		//sprintf(tmp , "%d,", p->port);
		oa_strcat(tmp, p->port_str);
		oa_strcat(tmp, ",");
	}
	oa_strcat(para, tmp);
	if (p == NULL) oa_strcat(para, dev_now_params.ftpusr);
	else oa_strcat(para, p->usr);
	oa_strcat(para, ",");
	if (p == NULL) oa_strcat(para, dev_now_params.ftppwd);
	else oa_strcat(para, p->pw);
	oa_strcat(para, ",");
	if (p == NULL) oa_strcat(para, dev_now_params.ftp_prog_name);
	else oa_strcat(para, p->fw);
	oa_strcat(para, ",0");//8:print download bar & restart after finish
	//oa_strcat(para, ",openatdll.dll,0");
	DEBUG("update param:%s", para);
	ret = oa_start_ftp_upgrade(para);
	if (ret) DEBUG("start to update!I will restart if update successfully!!!");
}
