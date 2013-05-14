/*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  MobileTech(Shanghai) Co., Ltd.
 *
 *****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   oa_app.c
 *
 * Project:
 * --------
 *   OPEN AT project
 *
 * Description:
 * ------------
 *   This Module gives a sample app on openat framework.
 *
 * Author:
 * -------
 *   None!
 *
 ****************************************************************************/

#include "oa_type.h"
#include "oa_api.h"
#include "oa_setting.h"
#include "oa_soc.h"
/*--------BEG: Customer code----------*/
#include "oa_hw.h"
#include "oa_sw.h"
#include "oa_wdt.h"
#include "oa_uart.h"
#include "oa_gps.h"
#include "oa_dev_params.h"
#include "oa_callback_func_reg.h"
#include "oa_use_lock.h"
#include "oa_jt808.h"
#include "oa_platform.h"
#include "oa_blinddata.h"
#include "oa_lbs2mtk.h"
DEV_PLAT_PARAS dev_running =
{
	PLAT_SOC_INIT,
	OFFLINE,
	OA_TRUE,
	none,
	0,
	OA_TRUE,
};
oa_bool acc_short_conning = OA_FALSE;//just used for send a location data when acc is off
extern DEVICE_PARAMS dev_now_params;
extern oa_uint8 acc_status;
extern void App_TaskSScrnSendManage(void *Para);
/*--------END: Customer code----------*/
oa_char OA_VERSION_NO[]="v4.0.0ep";
/*****************************************************************
*-----------------Customer define for trace and at cmd control-------- *
*1. g_oa_trace_level: which trace levels user care for;
*2. g_oa_trace_port : which port trace info will output;
*3. g_oa_ps_port     : at cmd control port, default is OA_UART1;
*4. g_oa_tst_port     : Catcher port, default is OA_UART3;
*NOTE: Be sure g_oa_ps_port with g_oa_tst_port is different;
******************************************************************/
oa_trace_level_enum g_oa_trace_level = OA_TRACE_USER;    //Define debug information output level.
oa_trace_port_enum  g_oa_trace_port  = OA_TRACE_PORT_1; //Define debug information output port.
#ifdef AT_PS
oa_uart_enum          g_oa_ps_port    = OA_UART1;             //Define the AT ps port, If set OA_UART_AT(virtual AT port) , can release UART1
#else
oa_uart_enum          g_oa_ps_port    =OA_UART_AT;     
#endif
oa_uart_enum          g_oa_tst_port    = OA_UART_NULL;      //Define the Catcher port,
/*********************************************************
*Function:     oa_app_plat_link()
*Description:  entry function for normal scheduler and prompting     
*Return:		void
*Others:         
*********************************************************/
void oa_app_plat_link(void *para)
{	
	static oa_bool task_runed = OA_TRUE;
	oa_uint16 build_ret;
	oa_uint16 soc_ret;
	
	if (OA_TRUE == task_runed){
		OA_DEBUG_USER("<<<<<<<<<<<<<task %s is running......>>>>>>>>>>>>>", __func__);
		task_runed = OA_FALSE;
	}
	//�����ظ�����ĳһ������
	if (OA_TRUE == dev_running.plat_switch){
		switch (dev_running.next_step){
			case PLAT_SOC_INIT:{
				if (oa_sim_network_is_valid()){
					OA_DEBUG_USER("GSM network init finished!");
					/*soc init*/
					oa_soc_init();//soc paras & callback register
					oa_soc_state_check();//check & connect
					dev_running.plat_switch = OA_FALSE;
				}
				else{
					OA_DEBUG_USER("sim network is invalue!");
					break;
				}
				break;
			}
			case PLAT_RECONN:{
				if (oa_sim_network_is_valid()){
					//OA_DEBUG_USER("GSM network init finished!");
					/*soc init*/
					//oa_soc_init();//soc paras & callback register
					oa_soc_state_check();//check & connect
					dev_running.plat_switch = OA_FALSE;
				}
				else{
					OA_DEBUG_USER("sim network is invalue!");
					break;
				}
				break;
			}
			case PLAT_DEV_REG:{
				if (g_soc_context.state != OA_SOC_STATE_ONLINE){
					break;
				}
				build_ret = DevReq2ServPackag_build(REGISTERS);
				if (!build_ret){
					OA_DEBUG_USER("(%s:%s:%d): build register packet err!", __FILE__, __func__, __LINE__);
					break;
				}
				else{
					soc_ret = oa_soc_send_req();
					if (build_ret == soc_ret && has_reg()){//send ok & has authen code
						dev_running.plat_switch = OA_FALSE;
					}
				}
				break;
			}
			//login & authen
			case PLAT_DEV_LOGIN:{
				if (g_soc_context.state != OA_SOC_STATE_ONLINE){
					break;
				}
				build_ret = DevReq2ServPackag_build(LOGIN);
				if (!build_ret){
					OA_DEBUG_USER("(%s:%s:%d): build login packet err!", __FILE__, __func__, __LINE__);
					break;
				}
				else{
					soc_ret = oa_soc_send_req();
					if (build_ret == soc_ret){
						dev_running.plat_switch = OA_FALSE;
						dev_running.doing_what = authen;//I am doing authen
					}
				}
				break;
			}
			case PLAT_DEV_UNREG:{
				if (g_soc_context.state != OA_SOC_STATE_ONLINE || dev_running.plat_status != ONLINE){
					break;
				}
				build_ret = DevReq2ServPackag_build(UNREGISTERS);
				if (!build_ret){
					OA_DEBUG_USER("(%s:%s:%d): build unlogin packet err!", __FILE__, __func__, __LINE__);
					break;
				}
				else{
					soc_ret = oa_soc_send_req();
					if (build_ret == soc_ret){
						dev_running.plat_switch = OA_FALSE;
						dev_running.doing_what = unreg;//I am doing unreg
					}
				}
			}
			break;
			default:break;
		}
	}
	else{//detect is online/offline
		if (dev_running.plat_check_value == OA_TRUE){
			if (oa_sim_network_is_valid()){
				//Trace("(%s:%s:%d): acc_status:%d acc_short_conning:%d", __FILE__, __func__, __LINE__,acc_status,acc_short_conning);
				if ((acc_status == ACC_ON) || (acc_short_conning == OA_TRUE)){//acc is on
					//Trace("(%s:%s:%d): check soc status!", __FILE__, __func__, __LINE__);
					oa_soc_state_check();//check & connect
				}
			}
			else{
				OA_DEBUG_USER("(%s:%s:%d): sim network is invalue!", __FILE__, __func__, __LINE__);
			}
		}
	}

	oa_timer_start(OA_APP_SCHEDULER_ID, oa_app_plat_link, NULL, OA_APP_PLAT_LINK);
	return;
	
	#if 0
	if(oa_sim_network_is_valid())
	{
		//sim and network is invalid, can driver network event
		if(!first_valid)
		{
			OA_DEBUG_USER("GSM network init finished!");
			/*soc init*/
			oa_soc_init();
			first_valid = OA_TRUE;
		}
		oa_soc_state_check();
		oa_soc_send_req();
	}

	//ret = oa_network_get_signal_level();
	//OA_DEBUG_USER("oa_network_get_signal_level = %d", ret);
	//oa_uart_send_at_cmd_req("AT+REG?\r\n", oa_strlen("AT+REG?\r\n"));
	oa_timer_start(OA_APP_SCHEDULER_ID, oa_app_plat_link, NULL, OA_APP_SCHEDULER_PERIOD);
	#endif
}
/*********************************************************
*Function:     oa_app_plat_data()
*Description:  entry function for normal scheduler and prompting     
*Return:		void
*Others:         
*********************************************************/
void oa_app_plat_data(void *param)
{
	static oa_bool task_runed = OA_TRUE;
	static oa_uint32 acc_counter = 0;
	static oa_uint32 hbeat_counter = 0;
	oa_uint16 build_ret;
	oa_uint16 soc_ret;
	static oa_bool send_before_close = OA_TRUE;
	if (OA_TRUE == task_runed){
		OA_DEBUG_USER("<<<<<<<<<<<<<task %s is running......>>>>>>>>>>>>>", __func__);
		task_runed = OA_FALSE;
	}
	
	if (acc_status == ACC_OFF){//do something when acc is off
		acc_counter++;

		if (hbeat_counter > 0){
			hbeat_counter = 0;
		}
		
		if (dev_running.plat_status == ONLINE && acc_short_conning == OA_FALSE){
			//send the last packet before closing soc!!!
			if (send_before_close){
				build_ret = DevReq2ServPackag_build(REPORT_LOCATION);//build a location packet
				if (build_ret > 0){
					soc_ret = oa_soc_send_req();//check datas in buffer & send, concern it send ok.
					if (soc_ret == build_ret){
						Trace("(%s:%s:%d): send the last packet before closing soc!!!", __FILE__, __func__, __LINE__);
						send_before_close = OA_FALSE;
					}
					else{
						Trace("(%s:%s:%d): send data is not equal!", __FILE__, __func__, __LINE__);
					}
				}
				else{
					Trace("(%s:%s:%d): build packet err!", __FILE__, __func__, __LINE__);
				}

			}
			
			if (send_before_close == OA_FALSE){
				//close socket
				if (oa_soc_close_req()){//after testing it doesn't cause "oa_soc_notify_ind_user_callback" 
					Trace("(%s:%s:%d):I close soc by myself for acc is off",__FILE__, __func__, __LINE__);
					dev_running.plat_status = OFFLINE;
					send_before_close = OA_TRUE;
				}
			}
			
		}
		else if (dev_running.plat_status == ONLINE && acc_short_conning == OA_TRUE){
			build_ret = DevReq2ServPackag_build(REPORT_LOCATION);//build a location packet
			
			if (build_ret > 0){
				soc_ret = oa_soc_send_req();//check datas in buffer & send, concern it send ok.
				if (soc_ret == build_ret){
					acc_short_conning = OA_FALSE;
				}
				else{
					Trace("(%s:%s:%d): send data is not equal!", __FILE__, __func__, __LINE__);
				}
			}
			else{
				Trace("(%s:%s:%d): build packet err!", __FILE__, __func__, __LINE__);
			}
			
		}
		
		//acc off time > sleep_reporttime & dev is offline
		//Trace("(%s:%s:%d):sleep_reporttime:%u", __FILE__, __func__, __LINE__,dev_now_params.sleep_reporttime);
		if (dev_now_params.sleep_reporttime >=  PLAT_DATA_SECOND){//avoid sleep_reporttime is too short
			if (acc_counter * PLAT_DATA_SECOND >= dev_now_params.sleep_reporttime && 
				dev_running.plat_status == OFFLINE){
				acc_counter = 0;
				Trace("(%s:%s:%d):I'm doing soc connect for a period", __FILE__, __func__, __LINE__);
				oa_soc_state_check();//check & connect = A lengthy process of asynchronous, so we can not wait here
				acc_short_conning = OA_TRUE;
			}
			
		}
		else{
			OA_DEBUG_USER("(%s:%s:%d): sleep_reporttime is too short!", __FILE__, __func__, __LINE__);
		}
	}
	else if (acc_status == ACC_ON){
		//do something when acc is on
		hbeat_counter++;
		
		if (acc_counter > 0){
			acc_counter = 0;
		}

		if (dev_running.plat_status == ONLINE){
			if (hbeat_counter * PLAT_DATA_SECOND >= dev_now_params.heartbeat_interval){
				build_ret = DevReq2ServPackag_build(HEART_BEAT);//build heartbeats packets & fill buffer with it
				if (!build_ret){
					Trace("(%s:%s:%d): build heartbeat packet err!", __FILE__, __func__, __LINE__);
				}
				else{
					soc_ret = oa_soc_send_req();
					if (build_ret == soc_ret){
						hbeat_counter = 0;
						Trace("(%s:%s:%d): send one heartbeat packet", __FILE__, __func__, __LINE__);
					}
				}
			}

			oa_soc_send_req();//check datas in buffer & send	
		}
		
	}

	oa_timer_start(OA_TIMER_ID_4, oa_app_plat_data, NULL, OA_APP_PLAT_DATA);
	return;
}



/****************************************************************
*---user's app initalize and callback functions register
*****************************************************************/
void oa_app_init(void)
{

	/*Setting parameters*/
	oa_soc_setting_init();

	/*socket init*/
	oa_soc_init_fast();

}
/*********************************************************
*Function:     oa_app_hw_init()
*Description:  Power on hardware init for customer, before openat app runing
*Return:		void
*Others:         
*********************************************************/
 void oa_app_hw_init(void)
{
	oa_bool ret = OA_FALSE;
	//��ʼ��debug��:����һ
#ifndef AT_PS
	ret = oa_uart_init(OA_UART1);
	if (OA_TRUE == ret)
	{
		OA_DEBUG_USER("UART1 init OK!");
	}
	else
	{
		OA_DEBUG_USER("UART1 init ERR!");
	}
#endif
#ifdef USE_UART2
	//��ʼ�����ڶ�
	ret = oa_uart_init(OA_UART2);
	if (OA_TRUE == ret)
	{
		OA_DEBUG_USER("UART2 init OK!");
	}
	else
	{
		OA_DEBUG_USER("UART2 init ERR!");
	}
#endif
#ifdef USE_UART3
	//��ʼ��������
	ret = oa_uart_init(OA_UART3);
	if (OA_TRUE == ret)
	{
		OA_DEBUG_USER("UART3 init OK!");
	}
	else
	{
		OA_DEBUG_USER("UART3 init ERR!");
	}
#endif
	//GPIO init
	oa_gpio_set();
	OA_DEBUG_USER("Hardware Init End");
	OA_DEBUG_USER(OA_HW_VERSION_NO);
	return;
}
/*********************************************************
*Function:     oa_app_main()
*Description:  application entry     
*Return:		void
*Others:         
*********************************************************/
void oa_app_main(void)
{
	static oa_bool first_run = OA_TRUE;
	oa_bool dev_is_locked;
	
	if (OA_TRUE == first_run)
	{
		//OA_DEBUG_USER(OA_SW_VERSION_NO);
		OA_DEBUG_USER("<<<<<<<<<<<<<task %s is running......>>>>>>>>>>>>>", __func__);
		//device params initial
		dev_params_init();
		//just print key params
		print_key_dev_params();
		//fill soc with device params
		params_to_soc_set();//change 'soc_cs' value
		//callback function register
		callback_func_reg();
		first_run = OA_FALSE;
	}
	
	//Determines whether the device is locked?
	use_lock();
	dev_is_locked = use_is_lock();
	if (OA_FALSE == dev_is_locked) //device is unlock
	{
		//application initial, mainly about socket
		oa_app_init();
		//watchdog task
//		oa_timer_start(OA_TIMER_ID_1, oa_app_wdt, NULL, OA_WDT_SCHEDULER_PERIOD);
		//acc status detect
		oa_timer_start(OA_TIMER_ID_6, acc_status_detect, NULL, OA_ACC_RUN);
		//platform link task
		oa_timer_start(OA_APP_SCHEDULER_ID, oa_app_plat_link, NULL, OA_APP_PLAT_LINK_1ST);
		//platform data task
		oa_timer_start(OA_TIMER_ID_4, oa_app_plat_data, NULL, OA_APP_PLAT_DATA);
		//gps initial task:this task maybe is useless because the gps module desn't need initial
//		oa_timer_start(OA_TIMER_ID_2, oa_app_gps_init, NULL, OA_GPS_SCHEDULER_1ST);
		//just for test,when test ok ,comment it
		oa_timer_start(OA_TIMER_ID_5, oa_app_gps, NULL, OA_GPS_SCHEDULER_1ST);//run per 500 millisecond
		//blinddata task
		oa_timer_start(OA_TIMER_ID_7, oa_app_blinddata, NULL, OA_APP_BLINDDATA);
		//schedule screen task, send mainly ,transplant from lbs@wjn
		oa_timer_start(OA_TIMER_ID_8, App_TaskSScrnSendManage, NULL, SCHD_SCRN_TIME);
	}
	else if (OA_TRUE == dev_is_locked)  //device is lock
	{
		OA_DEBUG_USER("device is locked!Please activate it.");
		//app main restart
		oa_timer_start(OA_TIMER_ID_3, oa_app_main, NULL, OA_MAIN_SCHEDULER_PERIOD);
	}

}


/*Warning: NEVER, NEVER, NEVER Modify the compile options!!!*/
#pragma arm section code = "OA_USER_ENTRY"
oa_bool oa_user_dll_entry_dispatch_to_sys(oa_user_dll_dispatch_sys_struct *param)
{
    param->verSr = OA_VERSION_NO;
    param->main_entry = oa_app_main;
    param->hw_init = oa_app_hw_init;
    
    /*if do not want to check, put zero(0x00) here*/
    param->api_check_sum = (oa_uint32)oa_api_begin + (oa_uint32)oa_api_end;

    param->trace_level = g_oa_trace_level;
    param->trace_port = g_oa_trace_port;
    param->ps_port = g_oa_ps_port;
    param->tst_port = g_oa_tst_port;
}
#pragma arm section code
