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
 *   oa_dev_params.c
 *
 * Project:
 * --------
 *   MTK-LBS project
 *
 * Description:
 * ------------
 *   This Module declares OPEN AT's api.
 	define device's parameters for protocol 808
 *
 * Author:
 * -------
 *   zhuqing.
 *
 ****************************************************************************/
#include "oa_type.h"
#include "oa_api.h"
#include "oa_sw.h"
#include "oa_hw.h"
#include "oa_setting.h"
#include "oa_dev_params.h"
#include "oa_blinddata.h"
#include "oa_setting.h"
#include "oa_gps.h"
#include "oa_use_lock.h"
#include "oa_platform.h"
#include "oa_sms.h"
#include "oa_sw.h"
#include "oa_hw.h"
#include "oa_debug.h"
#include "oa_area.h"
#define FS_PATH_NOT_FOUND -19
extern oa_soc_set_parameter soc_cs;

DEVICE_PARAMS dev_def_params =
{
	60,							//heartbeat_interval
	20,							//tcp_ack_timeout
	3,							//tcp_retrans_times
	20,							//udp_ack_timeout
	3,							//udp_retrans_times
	20,							//sms_ack_timeout
	3,							//sms_retrans_times
	{"cmnet"},					//m_apn_name
	{"root"},						//m_apn_username
	{"root"},						//m_apn_password
	{"61.155.107.41"},				//m_server_ip
	//{"202.102.108.23"},			//hx:just for test
	{"talentvideo.com.cn"},			//m_server_dn
	{"cmnet"},					//b_apn_name
	{"root"},						//b_apn_username
	{"root"},						//b_apn_password
	{"112.4.133.86"},				//b_server_ip
	{"talentvideo.com.cn"},			//b_server_dn
	11114,						//server_tcp_port
	11115,						//server_udp_port
	//9992,						//hx:just for test
	//......
	0,							//report_strategy
	0,							//report_type
	60,							//unlogin_reporttime
	//......
	300,							//sleep_reporttime
	10,							//urgent_reporttime
	30,							//default_reporttime
	//......
	200,							//default_reportdistance
	300,							//unlogin_reportdistance
	500,							//sleep_reportdistance
	10,							//urgent_reportdistance
	15,							//corner_reportangle
	10,
	{"00000000000"},				//moniter platform num
	{"00000000000"},				//reset_num
	{"00000000000"},				//restore_factory_settings_num
	{"00000000000"},
	{"00000000000"},				//terminal_sms_num
	//......
	0xffffffff,					//alarm_mask
	0,							//alarm_send_sms_mask
	120,							//max_speed
	10,							//speed_duration
	14400,							//continuous_drive_time_threshold
	57600,						//day_add_drive_time_threshold
	1200,						//min_rest_time
	57600,						//max_park_time
	//......
	0,							//vehicle_odometer
	32,							//vehicle_province_id
	100,							//vehicle_city_id
	{"0000000"},					//vehicle_license : carID
	2,							//plate_color
	//user definition
	{"70555"},					//man id
	{"T100G"},					//term model
	{"0000000"},					//term id
	{"00000000000"},				//LclTEL
	{"61.155.107.41"},				//ftp ip
	21,							//port
	{"talent"},					//user name
	{"talent"},					//pwd
	{"mtk-lbs v1.0.0"},				//program name
};

DEVICE_PARAMS dev_now_params;
/*********************************************************
*Function:      clr_miles()
*Description:   clr miliage
*Return:         oa_bool
*Others:         
*********************************************************/
oa_bool clear_miles(void)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	TOTAL_MILE mileage;
	
	handle = oa_fopen(MILEAGE_FILE);
	if (handle < 0){
		return OA_TRUE;
	}
	
	mileage.total_mileage = 0;
	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fwrite(handle, &mileage, sizeof(TOTAL_MILE), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(TOTAL_MILE))){
		DEBUG("Init mileage file failed!");
		return OA_FALSE;
	}
	ret = oa_fclose(handle);
	if (ret < 0){
		DEBUG("close file err!");
		oa_fdelete(MILEAGE_FILE);
		return OA_FALSE;
	}
	return OA_TRUE;
}
/*********************************************************
*Function:      del_miles()
*Description:   delete authen code
*Return:         oa_bool
*Others:         
*********************************************************/
oa_bool del_authcode(void)
{
	oa_int32 ret, handle;

	handle = oa_fopen(AUTHEN_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about miles
		ret = oa_fdelete(AUTHEN_FILE);
		if (ret < 0){
			DEBUG("delete file:authen_file.ini err!");
			return OA_FALSE;
		}
	}
	return OA_TRUE;
}
/*********************************************************
*Function:      del_miles()
*Description:   delete authen code
*Return:         oa_bool
*Others:         
*********************************************************/
oa_bool del_blinddata(void)
{
	oa_int32 ret, handle;

	handle = oa_fopen(BLINDDATA_DIR_CONF);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about blinddata
		ret = oa_fdelete(BLINDDATA_DIR_CONF);
		if (ret < 0){
			DEBUG("delete file:blinddata_dir_conf.ini err!");
			return OA_FALSE;
		}
	}
		
	handle = oa_fopen(DATANAME_1FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about blinddata
		ret = oa_fdelete(DATANAME_1FILE);
		if (ret < 0){
			DEBUG("delete file:c:\\blinddata.dir\\data err!");
			return OA_FALSE;
		}
	}

	handle = oa_fopen(MANAGE_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about blinddata
		ret = oa_fdelete(MANAGE_FILE);
		if (ret < 0){
			DEBUG("delete file:c:\\blinddata.dir\\manage_file.ini err!");
			return OA_FALSE;
		}
	}

	
	ret = oa_fremoveDir(BLINDDATA_DIRNAME);
	if (ret < 0){
		DEBUG("delete file:blinddata.dir err!");
		return OA_FALSE;
	}

	return OA_TRUE;
	
}
/*********************************************************
*Function:      del_miles()
*Description:   delete authen code
*Return:         oa_bool
*Others:         
*********************************************************/
oa_bool del_areadata(void)
{
	oa_int32 ret, handle;

	handle = oa_fopen(CIRCLE_AREA_DATA);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about blinddata
		ret = oa_fdelete(CIRCLE_AREA_DATA);
		if (ret < 0){
			DEBUG("delete file:circle_area_data err!");
			return OA_FALSE;
		}
	}
		
	handle = oa_fopen(RECT_AREA_DATA);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about blinddata
		ret = oa_fdelete(RECT_AREA_DATA);
		if (ret < 0){
			DEBUG("delete file:rect_area_data err!");
			return OA_FALSE;
		}
	}

	handle = oa_fopen(POLY_AREA_DATA);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about blinddata
		ret = oa_fdelete(POLY_AREA_DATA);
		if (ret < 0){
			DEBUG("delete file:poly_area_data err!");
			return OA_FALSE;
		}
	}

	return OA_TRUE;
	
}

/*********************************************************
*Function:      del_rec()
*Description:   delete miliage & anthcode
*Return:         oa_bool
*Others:         
*********************************************************/
oa_bool del_rec(void)
{
	oa_int32 ret, handle;

	handle = oa_fopen(MILEAGE_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about miles
		ret = oa_fdelete(MILEAGE_FILE);
		if (ret < 0){
			DEBUG("delete file:total_miles.ini err!");
			return OA_FALSE;
		}
	}
	
	handle = oa_fopen(AUTHEN_FILE);
	if (handle >= 0){
		oa_fclose(handle);
		//delete files about auther code
		ret = oa_fdelete(AUTHEN_FILE);
		if (ret < 0){
			DEBUG("delete file:authen_file.ini err!");
			return OA_FALSE;
		}
	}

	return OA_TRUE;
}
/*********************************************************
*Function:      del_some_files()
*Description:   delete some files in order to work well
*Return:         oa_bool
*Others:         
*********************************************************/
oa_bool del_some_files(void)
{
	oa_int32 ret, handle;

	handle = oa_fopen(BLINDDATA_DIR_CONF);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about blinddata
		ret = oa_fdelete(BLINDDATA_DIR_CONF);
		if (ret < 0){
			DEBUG("delete file:blinddata_dir_conf.ini err!");
			return OA_FALSE;
		}
	}
		
	handle = oa_fopen(DATANAME_1FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about blinddata
		ret = oa_fdelete(DATANAME_1FILE);
		if (ret < 0){
			DEBUG("delete file:c:\\blinddata.dir\\data err!");
			return OA_FALSE;
		}
	}

	handle = oa_fopen(MANAGE_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about blinddata
		ret = oa_fdelete(MANAGE_FILE);
		if (ret < 0){
			DEBUG("delete file:c:\\blinddata.dir\\manage_file.ini err!");
			return OA_FALSE;
		}
	}

	
	ret = oa_fremoveDir(BLINDDATA_DIRNAME);
	if (ret < 0){
		if (ret != FS_PATH_NOT_FOUND){//means doesn't exist
			DEBUG("delete file:blinddata.dir err! maybe it doesn't exist! ret:%d", ret);
			return OA_FALSE;
		}
	}

	handle = oa_fopen(DEV_PARAMS_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about device parameters
		ret = oa_fdelete(DEV_PARAMS_FILE);
		if (ret < 0){
			DEBUG("delete file:dev_paras.ini err!");
			return OA_FALSE;
		}
	}

	handle = oa_fopen(OA_SOC_SETTING_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about device parameters
		ret = oa_fdelete(OA_SOC_SETTING_FILE);
		if (ret < 0){
			DEBUG("delete file:soc_setting.ini err!");
			return OA_FALSE;
		}
	}

	handle = oa_fopen(MILEAGE_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about miles
		ret = oa_fdelete(MILEAGE_FILE);
		if (ret < 0){
			DEBUG("delete file:total_miles.ini err!");
			return OA_FALSE;
		}
	}

	handle = oa_fopen(USE_LOCK_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about use bock
		ret = oa_fdelete(USE_LOCK_FILE);
		if (ret < 0){
			DEBUG("delete file:use_lock.ini err!");
			return OA_FALSE;
		}
	}

	handle = oa_fopen(AUTHEN_FILE);
	if (handle >= 0){
		oa_fclose(handle);
		//delete files about auther code
		ret = oa_fdelete(AUTHEN_FILE);
		if (ret < 0){
			DEBUG("delete file:authen_file.ini err!");
			return OA_FALSE;
		}
	}

	handle = oa_fopen(CIRCLE_AREA_DATA);
	if (handle >= 0){
		oa_fclose(handle);
		//delete files about auther code
		ret = oa_fdelete(CIRCLE_AREA_DATA);
		if (ret < 0){
			DEBUG("delete file:circle_area_data err!");
			return OA_FALSE;
		}
	}

	handle = oa_fopen(RECT_AREA_DATA);
	if (handle >= 0){
		oa_fclose(handle);
		//delete files about auther code
		ret = oa_fdelete(RECT_AREA_DATA);
		if (ret < 0){
			DEBUG("delete file:rect_area_data err!");
			return OA_FALSE;
		}
	}

	handle = oa_fopen(POLY_AREA_DATA);
	if (handle >= 0){
		oa_fclose(handle);
		//delete files about auther code
		ret = oa_fdelete(POLY_AREA_DATA);
		if (ret < 0){
			DEBUG("delete file:poly_area_data err!");
			return OA_FALSE;
		}
	}
#if 0
	handle = oa_fopen(RESTART_FILE);
	if (handle >= 0){
		oa_fclose(handle);
		//delete files about restart flag
		ret = oa_fdelete(RESTART_FILE);
		if (ret < 0){
			DEBUG("delete file:RESTART_FILE err!");
			return OA_FALSE;
		}
	}
#endif	
	
	return OA_TRUE;
}

/*********************************************************
*Function:      factory_set()
*Description:   restore factory settings
*Return:         oa_bool
*Others:         after factory setting, please restart.
*********************************************************/
oa_bool factory_set(void)
{
#if 0
	oa_int32 handle, ret;
	oa_uint32 dummy_write;
	
	handle = oa_fopen(DEV_PARAMS_FILE);
	if (handle < 0)
	{
		DEBUG("factory setting failed!");
		return OA_FALSE;
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	
	ret = oa_fwrite(handle, &dev_def_params, sizeof(dev_def_params), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(dev_def_params)))
	{
		DEBUG("factory setting write err!");
		return OA_FALSE;
	}

	oa_fclose(handle);
#endif
	oa_bool ret;
	ret = del_some_files();
	if (ret == OA_TRUE){
		DEBUG("factory setting ok!");
		oa_module_restart(NULL);
	}
	else{
		DEBUG("factory setting err!");
	}
	
	return OA_TRUE;
}
/*********************************************************
*Function:      dev_params_init()
*Description:   device parameters initial
*Return:         oa_bool
*Others:         
*********************************************************/
oa_bool dev_params_init(void)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	
	handle = oa_fopen(DEV_PARAMS_FILE);
	if (handle < 0)
	{
		/* create new file for setting. */
		handle = oa_fcreate(DEV_PARAMS_FILE);
		/* hope never return here. */
		if (handle < 0)
		{
			DEBUG("Create dev params file failed!");
			return OA_FALSE;
		}

		ret = oa_fwrite(handle, &dev_def_params, sizeof(dev_def_params), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(dev_def_params)))
		{
			DEBUG("Init dev params file failed!");
			return OA_FALSE;
		}

		DEBUG("Create dev params file ok!");    

		
	}
	oa_fseek(handle, 0, OA_FILE_BEGIN);
	//read dev params to 'dev_now_params'
	ret = oa_fread(handle, &dev_now_params, sizeof(dev_now_params), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(dev_now_params))) goto fail;

	oa_fclose(handle);
	DEBUG("dev params init ok!");    
	return OA_TRUE;

	fail:
	//need delete the file and reset here..
	oa_fclose(handle);
	oa_fdelete(DEV_PARAMS_FILE);
	DEBUG("dev params init err!");    
	return OA_FALSE;
}
/*********************************************************
*Function:      dev_params_save()
*Description:   save dev_now_params to 'DEV_PARAMS_FILE'
*Return:         oa_bool
*Others:         
*********************************************************/
oa_bool dev_params_save(void)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	
	handle = oa_fopen(DEV_PARAMS_FILE);
	if (handle < 0)
	{
		DEBUG("open err ");
		return OA_FALSE;
	}
	
	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fwrite(handle, &dev_now_params, sizeof(dev_now_params), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(dev_now_params)))
	{
		DEBUG("write dev params file failed!");
		oa_fclose(handle);
		return OA_FALSE;
	}
	oa_fclose(handle);    
	return OA_TRUE;
}
/*********************************************************
*Function:      backup_devid()
*Description:   save dev_now_params to 'DEV_PARAMS_FILE'
*Return:         oa_bool
*Others:         
*********************************************************/
oa_bool backup_devid(oa_uint8 *p_id)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	dev_id_struct dev_id_var;
	
	handle = oa_fopen(DEVID_FILE);
	if (handle < 0)
	{
		/* create new file for setting. */
		handle = oa_fcreate(DEVID_FILE);
		/* hope never return here. */
		if (handle < 0)
		{
			DEBUG("Create dev id file failed!");
			return OA_FALSE;
		}

		oa_memset(&dev_id_var, 0x0, sizeof(dev_id_var));
		oa_strcpy(dev_id_var.dev_id, DEVID_INI);
		
		ret = oa_fwrite(handle, &dev_id_var, sizeof(dev_id_var), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(dev_id_var)))
		{
			DEBUG("Init dev id file failed!");
			return OA_FALSE;
		}

		DEBUG("Create dev id file ok!");    

		
	}
	oa_fseek(handle, 0, OA_FILE_BEGIN);
	oa_memset(&dev_id_var, 0x0, sizeof(dev_id_var));
	oa_strcpy(dev_id_var.dev_id, p_id);
	//write 
	ret = oa_fwrite(handle, &dev_id_var, sizeof(dev_id_var), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(dev_id_var)))
	{
		DEBUG("write dev id file failed!");
		oa_fclose(handle);
		return OA_FALSE;
	}

	oa_fclose(handle);
	return OA_TRUE;
}
/*********************************************************
*Function:      restore_devid()
*Description:   save dev_now_params to 'DEV_PARAMS_FILE'
*Return:         oa_bool
*Others:         
*********************************************************/
oa_bool restore_devid(void)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	dev_id_struct dev_id_var;
	
	handle = oa_fopen(DEVID_FILE);
	if (handle < 0) {
		return OA_FALSE;
	}
	oa_fseek(handle, 0, OA_FILE_BEGIN);
	oa_memset(&dev_id_var, 0x0, sizeof(dev_id_var));
	//read 
	ret = oa_fread(handle, &dev_id_var, sizeof(dev_id_var), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(dev_id_var))) {
		oa_fclose(handle);
		return OA_FALSE;
	}

	oa_strcpy(dev_now_params.term_id, dev_id_var.dev_id);
	oa_fclose(handle);
	return OA_TRUE;
}
/*********************************************************
*Function:      params_to_soc_set()
*Description:   fill soc with device params
*Return:         void
*Others:         
*********************************************************/
void params_to_soc_set(void)
{
	oa_memset(soc_cs.serve_ipaddr, 0x0, sizeof(soc_cs.serve_ipaddr));
	oa_memset(soc_cs.serve_host_name, 0x0, sizeof(soc_cs.serve_host_name));
	oa_memcpy(soc_cs.serve_ipaddr, dev_now_params.m_server_ip, oa_strlen(dev_now_params.m_server_ip)/*sizeof(soc_cs.serve_ipaddr)*/);
	oa_memcpy(soc_cs.serve_host_name, dev_now_params.m_server_dn, oa_strlen(dev_now_params.m_server_dn)/*sizeof(soc_cs.serve_host_name)*/);
	if (soc_cs.connct_type == OA_SOCKET_TCP){
		soc_cs.port = dev_now_params.server_tcp_port;
	}
	else if (soc_cs.connct_type == OA_SOCKET_UDP){
		soc_cs.port = dev_now_params.server_udp_port;
	}
	
	DEBUG("dev params to soc ok!");
}
/*********************************************************
*Function:      params_to_soc_set()
*Description:   fill soc with device params
*Return:         void
*Others:         
*********************************************************/
void print_key_dev_params(void)
{
	oa_char tmp[64] = {0x0};
	
	DEBUG("------------------------------device params-----------------------------");
	DEBUG("Hearttime					:%d", dev_now_params.heartbeat_interval);
	DEBUG("RSP_TCP						:%d", dev_now_params.tcp_ack_timeout);
	DEBUG("RSP_UDP						:%d", dev_now_params.udp_ack_timeout);
	DEBUG("RSP_SMS						:%d", dev_now_params.sms_ack_timeout);
	DEBUG("Retry_TCP					:%d", dev_now_params.tcp_retrans_times);
	DEBUG("Retry_UDP					:%d", dev_now_params.udp_retrans_times);
	DEBUG("Retry_SMS					:%d", dev_now_params.sms_retrans_times);
	DEBUG("IP						:%s", dev_now_params.m_server_ip);
	DEBUG("UDPPORT						:%d", dev_now_params.server_udp_port);
	DEBUG("TCPPORT	 					:%d", dev_now_params.server_tcp_port);
	DEBUG("TEL						:%s", dev_now_params.term_tel_num);
	DEBUG("Rpt_strategy                        		:%d", dev_now_params.report_strategy);
	DEBUG("Rpttime_unlog                       		:%d", dev_now_params.unlogin_reporttime);
	DEBUG("Rpttime_sleep                       		:%d", dev_now_params.sleep_reporttime);
	DEBUG("Rpttime_alarm                      	 	:%d", dev_now_params.urgent_reporttime);
	DEBUG("Rpttime_def                       	 	:%d", dev_now_params.default_reporttime);
	DEBUG("Rptdis_unlog                       	 	:%d", dev_now_params.unlogin_reportdistance);
	DEBUG("Rptdis_sleep                       	 	:%d", dev_now_params.sleep_reportdistance);
	DEBUG("Rptdis_alarm                       	 	:%d", dev_now_params.urgent_reportdistance);
	DEBUG("Rptdis_def                       	 	:%d", dev_now_params.default_reportdistance);
	DEBUG("Rptcog                       	 	 	:%d", dev_now_params.corner_reportangle);
	DEBUG("servertel					:%s", dev_now_params.monitor_platform_num);
	DEBUG("resettel					:%s", dev_now_params.reset_num);
	DEBUG("factorysettel					:%s", dev_now_params.restore_factory_settings_num);
	DEBUG("smstel						:%s", dev_now_params.monitor_platform_sms_num);
	DEBUG("alarmsmstel					:%s", dev_now_params.terminal_sms_num);
	oa_myitoa(dev_now_params.alarm_mask, tmp);
	DEBUG("swh_alarmmask					:%s", tmp);
	oa_memset(tmp, 0x0, sizeof(tmp));
	oa_myitoa(dev_now_params.alarm_send_sms_mask, tmp);
	DEBUG("swh_alarmsms					:%s", tmp);
	DEBUG("overspeed					:%d", dev_now_params.max_speed);
	DEBUG("overspeedtime					:%d", dev_now_params.speed_duration);
	DEBUG("tireddrivetime					:%d", dev_now_params.continuous_drive_time_threshold);
	DEBUG("daydrivetime					:%d", dev_now_params.day_add_drive_time_threshold);
	DEBUG("min_resttime					:%d", dev_now_params.min_rest_time);
	DEBUG("max_parktime					:%d", dev_now_params.max_park_time);
	DEBUG("provincID					:%d", dev_now_params.vehicle_province_id);
	DEBUG("cityID						:%d", dev_now_params.vehicle_city_id);
	DEBUG("carID						:%s", dev_now_params.vehicle_license);
	DEBUG("carcolor					:%d", dev_now_params.plate_color);
	DEBUG("UPIP						:%s", dev_now_params.update_server_ip);
	DEBUG("UPPORT						:%d", dev_now_params.update_server_port);
	DEBUG("ftp_prog_name					:%s", dev_now_params.ftp_prog_name);
	DEBUG("ftpusr						:%s", dev_now_params.ftpusr);
	DEBUG("ftppwd						:%s", dev_now_params.ftppwd);
	
	
	DEBUG("man id						:%s", dev_now_params.manufacturers_id);
	DEBUG("term model					:%s", dev_now_params.term_model);
	DEBUG("term id						:%s", dev_now_params.term_id);
	#if 0
	DEBUG_N("car id						:%s", dev_now_params.vehicle_license);
	DEBUG_N("plate_color					:%d", dev_now_params.plate_color);
	DEBUG_N("continuous driving time			:%d", dev_now_params.continuous_drive_time_threshold);
	DEBUG_N("min rest time					:%d", dev_now_params.min_rest_time);
	DEBUG_N("max part time					:%d", dev_now_params.max_park_time);
	DEBUG_N("default report time				:%d", dev_now_params.default_reporttime);
	DEBUG_N("max speed					:%d", dev_now_params.max_speed);
	oa_itoa(dev_now_params.alarm_mask, tmp, BI);
	DEBUG_N("swh_alarmmask					:%s", tmp);
	oa_itoa(dev_now_params.alarm_send_sms_mask, tmp, BI);
	DEBUG_N("swh_alarmsms					:%s", tmp);
	DEBUG_N("alarmsmstel					:%s", dev_now_params.terminal_sms_num);
	DEBUG_N("UPIP						:%s", dev_now_params.update_server_ip);
	DEBUG_N("UPPORT						:%d", dev_now_params.update_server_port);
	DEBUG_N("ftp_prog_name					:%s", dev_now_params.ftp_prog_name);
	#endif
	DEBUG("software					:%s", OA_SW_VERSION_NO);
	DEBUG("hardware					:%s", OA_HW_VERSION_NO);
	DEBUG("-----------------------------------------------------------------------");
	
}
