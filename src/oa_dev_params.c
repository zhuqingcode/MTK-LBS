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
extern oa_soc_set_parameter soc_cs;

DEVICE_PARAMS dev_def_params =
{
	60,							//heartbeat_interval
	10,							//tcp_ack_timeout
	3,							//tcp_retrans_times
	30,							//udp_ack_timeout
	3,							//udp_retrans_times
	30,							//sms_ack_timeout
	3,							//sms_retrans_times
	{"cmnet"},					//m_apn_name
	{"root"},						//m_apn_username
	{"root"},						//m_apn_password
	{"112.4.133.86"},				//m_server_ip
	//{"202.102.108.23"},			//hx:just for test
	{"talentvideo.com.cn"},			//m_server_dn
	{"cmnet"},					//b_apn_name
	{"root"},						//b_apn_username
	{"root"},						//b_apn_password
	{"112.4.133.86"},				//b_server_ip
	{"talentvideo.com.cn"},			//b_server_dn
	11119,						//server_tcp_port
	9994,						//server_udp_port
	//9992,						//hx:just for test
	//......
	0,							//report_strategy
	0,							//report_type
	60,							//unlogin_reporttime
	//......
	300,							//sleep_reporttime
	30,							//urgent_reporttime
	30,							//default_reporttime
	//......
	200,							//default_reportdistance
	300,							//unlogin_reportdistance
	500,							//sleep_reportdistance
	10,							//urgent_reportdistance
	15,							//corner_reportangle
	10,
	{"13814120415"},				//moniter platform num
	{"13814120415"},				//reset_num
	{"13814120415"},				//restore_factory_settings_num
	{"13814120415"},
	{"13814120415"},				//terminal_sms_num
	//......
	0x00000000,					//alarm_mask
	0,							//alarm_send_sms_mask
	120,							//max_speed
	10,							//speed_duration
	120,							//continuous_drive_time_threshold
	57600,						//day_add_drive_time_threshold
	1200,						//min_rest_time
	60000,						//max_park_time
	//......
	3,							//vehicle_odometer
	32,							//vehicle_province_id
	100,							//vehicle_city_id
	{"NJ001"},					//vehicle_license : carID
	2,							//plate_color
	//user definition
	{"70555"},					//man id
	{"T100G"},					//term model
	{"3000015"},					//term id
	{"13814120415"},				//LclTEL
	{"114.221.34.95"},				//ftp ip
	21,							//port
	{"talent"},					//user name
	{"talent"},					//pwd
	{"mtk-lbs-beta v1.0.0"},				//program name
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
		OA_DEBUG_USER("(%s:%s:%d):Init mileage file failed!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}
	ret = oa_fclose(handle);
	if (ret < 0){
		OA_DEBUG_USER("(%s:%s:%d):close file err!", __FILE__,  __func__, __LINE__);
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
			Trace("(%s:%s:%d): delete file:authen_file.ini err!", __FILE__, __func__, __LINE__);
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
			Trace("(%s:%s:%d): delete file:blinddata_dir_conf.ini err!", __FILE__, __func__, __LINE__);
			return OA_FALSE;
		}
	}
		
	handle = oa_fopen(DATANAME_1FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about blinddata
		ret = oa_fdelete(DATANAME_1FILE);
		if (ret < 0){
			Trace("(%s:%s:%d): delete file:c:\\blinddata.dir\\data err!", __FILE__, __func__, __LINE__);
			return OA_FALSE;
		}
	}

	handle = oa_fopen(MANAGE_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about blinddata
		ret = oa_fdelete(MANAGE_FILE);
		if (ret < 0){
			Trace("(%s:%s:%d): delete file:c:\\blinddata.dir\\manage_file.ini err!", __FILE__, __func__, __LINE__);
			return OA_FALSE;
		}
	}

	
	ret = oa_fremoveDir(BLINDDATA_DIRNAME);
	if (ret < 0){
		Trace("(%s:%s:%d): delete file:blinddata.dir err!", __FILE__, __func__, __LINE__);
		return OA_FALSE;
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
			Trace("(%s:%s:%d): delete file:total_miles.ini err!", __FILE__, __func__, __LINE__);
			return OA_FALSE;
		}
	}
	
	handle = oa_fopen(AUTHEN_FILE);
	if (handle >= 0){
		oa_fclose(handle);
		//delete files about auther code
		ret = oa_fdelete(AUTHEN_FILE);
		if (ret < 0){
			Trace("(%s:%s:%d): delete file:authen_file.ini err!", __FILE__, __func__, __LINE__);
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
			Trace("(%s:%s:%d): delete file:blinddata_dir_conf.ini err!", __FILE__, __func__, __LINE__);
			return OA_FALSE;
		}
	}
		
	handle = oa_fopen(DATANAME_1FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about blinddata
		ret = oa_fdelete(DATANAME_1FILE);
		if (ret < 0){
			Trace("(%s:%s:%d): delete file:c:\\blinddata.dir\\data err!", __FILE__, __func__, __LINE__);
			return OA_FALSE;
		}
	}

	handle = oa_fopen(MANAGE_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about blinddata
		ret = oa_fdelete(MANAGE_FILE);
		if (ret < 0){
			Trace("(%s:%s:%d): delete file:c:\\blinddata.dir\\manage_file.ini err!", __FILE__, __func__, __LINE__);
			return OA_FALSE;
		}
	}

	
	ret = oa_fremoveDir(BLINDDATA_DIRNAME);
	if (ret < 0){
		Trace("(%s:%s:%d): delete file:blinddata.dir err!", __FILE__, __func__, __LINE__);
		return OA_FALSE;
	}

	handle = oa_fopen(DEV_PARAMS_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about device parameters
		ret = oa_fdelete(DEV_PARAMS_FILE);
		if (ret < 0){
			Trace("(%s:%s:%d): delete file:dev_paras.ini err!", __FILE__, __func__, __LINE__);
			return OA_FALSE;
		}
	}

	handle = oa_fopen(OA_SOC_SETTING_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about device parameters
		ret = oa_fdelete(OA_SOC_SETTING_FILE);
		if (ret < 0){
			Trace("(%s:%s:%d): delete file:soc_setting.ini err!", __FILE__, __func__, __LINE__);
			return OA_FALSE;
		}
	}

	handle = oa_fopen(MILEAGE_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about miles
		ret = oa_fdelete(MILEAGE_FILE);
		if (ret < 0){
			Trace("(%s:%s:%d): delete file:total_miles.ini err!", __FILE__, __func__, __LINE__);
			return OA_FALSE;
		}
	}

	handle = oa_fopen(USE_LOCK_FILE);
	if (handle >= 0){//exist
		oa_fclose(handle);
		//delete files about use bock
		ret = oa_fdelete(USE_LOCK_FILE);
		if (ret < 0){
			Trace("(%s:%s:%d): delete file:use_lock.ini err!", __FILE__, __func__, __LINE__);
			return OA_FALSE;
		}
	}

	handle = oa_fopen(AUTHEN_FILE);
	if (handle >= 0){
		oa_fclose(handle);
		//delete files about auther code
		ret = oa_fdelete(AUTHEN_FILE);
		if (ret < 0){
			Trace("(%s:%s:%d): delete file:authen_file.ini err!", __FILE__, __func__, __LINE__);
			return OA_FALSE;
		}
	}
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
		OA_DEBUG_USER("factory setting failed!");
		return OA_FALSE;
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	
	ret = oa_fwrite(handle, &dev_def_params, sizeof(dev_def_params), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(dev_def_params)))
	{
		OA_DEBUG_USER("factory setting write err!");
		return OA_FALSE;
	}

	oa_fclose(handle);
#endif
	oa_bool ret;
	ret = del_some_files();
	if (ret == OA_TRUE){
		OA_DEBUG_USER("factory setting ok!");
		oa_module_restart(NULL);
	}
	else{
		OA_DEBUG_USER("factory setting err!");
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
			OA_DEBUG_USER("Create dev params file failed!");
			return OA_FALSE;
		}

		ret = oa_fwrite(handle, &dev_def_params, sizeof(dev_def_params), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(dev_def_params)))
		{
			OA_DEBUG_USER("Init dev params file failed!");
			return OA_FALSE;
		}

		OA_DEBUG_USER("Create dev params file ok!");    

		
	}
	oa_fseek(handle, 0, OA_FILE_BEGIN);
	//read dev params to 'dev_now_params'
	ret = oa_fread(handle, &dev_now_params, sizeof(dev_now_params), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(dev_now_params))) goto fail;

	oa_fclose(handle);
	OA_DEBUG_USER("dev params init ok!");    
	return OA_TRUE;

	fail:
	//need delete the file and reset here..
	oa_fclose(handle);
	oa_fdelete(DEV_PARAMS_FILE);
	OA_DEBUG_USER("dev params init err!");    
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
		OA_DEBUG_USER("open err (%s:%s:%d):", __FILE__, __func__, __LINE__);
		return OA_FALSE;
	}
	
	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fwrite(handle, &dev_now_params, sizeof(dev_now_params), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(dev_now_params)))
	{
		OA_DEBUG_USER("write dev params file failed!");
		oa_fclose(handle);
		return OA_FALSE;
	}
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
	
	OA_DEBUG_USER("dev params to soc ok!");
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
	
	Trace("------------------------------device params-----------------------------");
	Trace("Hearttime					:%d", dev_now_params.heartbeat_interval);
	Trace("RSP_TCP					:%d", dev_now_params.tcp_ack_timeout);
	Trace("RSP_UDP					:%d", dev_now_params.udp_ack_timeout);
	Trace("RSP_SMS					:%d", dev_now_params.sms_ack_timeout);
	Trace("Retry_TCP					:%d", dev_now_params.tcp_retrans_times);
	Trace("Retry_UDP					:%d", dev_now_params.udp_retrans_times);
	Trace("Retry_SMS					:%d", dev_now_params.sms_retrans_times);
	Trace("IP						:%s", dev_now_params.m_server_ip);
	Trace("UDPPORT					:%d", dev_now_params.server_udp_port);
	Trace("TCPPORT	 				:%d", dev_now_params.server_tcp_port);
	Trace("TEL						:%s", dev_now_params.term_tel_num);
	Trace("Rpt_strategy                        		:%d", dev_now_params.report_strategy);
	Trace("Rpttime_unlog                       		:%d", dev_now_params.unlogin_reporttime);
	Trace("Rpttime_sleep                       		:%d", dev_now_params.sleep_reporttime);
	Trace("Rpttime_alarm                      	 		:%d", dev_now_params.urgent_reporttime);
	Trace("Rpttime_def                       	 		:%d", dev_now_params.default_reporttime);
	Trace("Rptdis_unlog                       	 		:%d", dev_now_params.unlogin_reportdistance);
	Trace("Rptdis_sleep                       	 		:%d", dev_now_params.default_reportdistance);
	Trace("Rptdis_alarm                       	 		:%d", dev_now_params.urgent_reportdistance);
	Trace("Rptdis_def                       	 		:%d", dev_now_params.default_reportdistance);
	Trace("Rptcog                       	 	 		:%d", dev_now_params.corner_reportangle);
	Trace("servertel					:%s", dev_now_params.monitor_platform_num);
	Trace("resettel					:%s", dev_now_params.reset_num);
	Trace("factorysettel					:%s", dev_now_params.restore_factory_settings_num);
	Trace("smstel						:%s", dev_now_params.monitor_platform_sms_num);
	Trace("alarmsmstel					:%s", dev_now_params.terminal_sms_num);
	oa_itoa(dev_now_params.alarm_mask, tmp, BI);
	Trace("swh_alarmmask					:%s", tmp);
	oa_itoa(dev_now_params.alarm_send_sms_mask, tmp, BI);
	Trace("swh_alarmsms					:%s", tmp);
	Trace("overspeed					:%d", dev_now_params.max_speed);
	Trace("overspeedtime					:%d", dev_now_params.speed_duration);
	Trace("tireddrivetime					:%d", dev_now_params.continuous_drive_time_threshold);
	Trace("min_resttime					:%d", dev_now_params.min_rest_time);
	Trace("max_parktime					:%d", dev_now_params.max_park_time);
	Trace("provincID					:%d", dev_now_params.vehicle_province_id);
	Trace("cityID						:%d", dev_now_params.vehicle_city_id);
	Trace("carID						:%d", dev_now_params.vehicle_license);
	Trace("carcolor					:%d", dev_now_params.plate_color);
	Trace("UPIP						:%s", dev_now_params.update_server_ip);
	Trace("UPPORT						:%d", dev_now_params.update_server_port);
	Trace("ftp_prog_name					:%s", dev_now_params.ftp_prog_name);
	
	
	Trace("man id						:%s", dev_now_params.manufacturers_id);
	Trace("term model					:%s", dev_now_params.term_model);
	Trace("term id					:%s", dev_now_params.term_id);
	#if 0
	Trace("car id						:%s", dev_now_params.vehicle_license);
	Trace("plate_color					:%d", dev_now_params.plate_color);
	Trace("continuous driving time			:%d", dev_now_params.continuous_drive_time_threshold);
	Trace("min rest time					:%d", dev_now_params.min_rest_time);
	Trace("max part time					:%d", dev_now_params.max_park_time);
	Trace("default report time				:%d", dev_now_params.default_reporttime);
	Trace("max speed					:%d", dev_now_params.max_speed);
	oa_itoa(dev_now_params.alarm_mask, tmp, BI);
	Trace("swh_alarmmask					:%s", tmp);
	oa_itoa(dev_now_params.alarm_send_sms_mask, tmp, BI);
	Trace("swh_alarmsms					:%s", tmp);
	Trace("alarmsmstel					:%s", dev_now_params.terminal_sms_num);
	Trace("UPIP						:%s", dev_now_params.update_server_ip);
	Trace("UPPORT						:%d", dev_now_params.update_server_port);
	Trace("ftp_prog_name					:%s", dev_now_params.ftp_prog_name);
<<<<<<< HEAD
	#endif
	Trace("software					:%s", OA_SW_VERSION_NO);
	Trace("hardware					:%s", OA_HW_VERSION_NO);
	Trace("-----------------------------------------------------------------------");
	
}
