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
 *   oa_setting.c
 *
 * Project:
 * --------
 *   OPEN AT project
 *
 * Description:
 * ------------
 *   This module contains setting routines for OPENAT.
 *
 * Author:
 * -------
 *   .
 *
 ****************************************************************************/
#include "oa_type.h"
#include "oa_api.h"
#include "oa_setting.h"
#include "oa_debug.h"


#define OA_SOC_SET_NULL_SIGN "@@@"


oa_soc_set_parameter soc_cs=
{
    //{"XMYN12345678913774650324\r\n"},    //dtuid
    {"0.0.0.0"},   //serve_ipaddr 218.82.31.206 
    {"cmt.3322.org"},   //serve_host_name
    2012, //port:21
    {"cmnet"},{"root"},{"root"},//apninfo
    OA_SOCKET_TCP,//connct_type
	//OA_SOCKET_UDP,//hx:just for test
    {0x00, 0x00}, //checktail
};

oa_soc_set_parameter g_soc_param={0};

void oa_soc_setting_init(void)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	   
	handle = oa_fopen(OA_SOC_SETTING_FILE);
	if (handle < 0)
	{
		/* create new file for setting. */
		handle = oa_fcreate(OA_SOC_SETTING_FILE);
		/* hope never return here. */
		if (handle < 0)
		{
			DEBUG("Create soc setting file failed!");
			return;
		}

	      ret = oa_fwrite(handle, &soc_cs, sizeof(soc_cs), &dummy_write);//soc_cs has been changed in 'params_to_soc_set'
		if((ret < 0) || (dummy_write != sizeof(soc_cs)))
		{
			DEBUG("Init soc setting file failed!");
			return;
		}

		DEBUG("Create soc setting file ok!");    

		ret = oa_fseek(handle, 0, OA_FILE_BEGIN);
	}

	/* read setting to g_soc_param. */
	ret = oa_fread(handle, &g_soc_param, sizeof(g_soc_param), &dummy_read);
	if((ret < 0) || (dummy_read != sizeof(g_soc_param))) goto fail;

	ret = oa_fclose(handle);
	return;
  
fail:
	//need delete the file and reset here..
	oa_fclose(handle);
	oa_fdelete(OA_SOC_SETTING_FILE);
	return;
}

void oa_soc_setting_save(void)
{
	oa_int32 handle_get, ret_get;
	oa_uint32 dummy_read_get,dummy_write_get;

	handle_get = oa_fopen(OA_SOC_SETTING_FILE);
	if (handle_get < 0){
		Trace("(%s:%s:%d): open err!", __FILE__, __func__, __LINE__);
		return;
	}
	oa_fseek(handle_get, 0, OA_FILE_BEGIN);
	ret_get = oa_fwrite(handle_get, &soc_cs, sizeof(soc_cs), &dummy_write_get);
	if((ret_get < 0) || (dummy_write_get != sizeof(soc_cs))){
		Trace("(%s:%s:%d): save soc err!", __FILE__, __func__, __LINE__);
		goto fail;
	}
	ret_get = oa_fclose(handle_get);
	return;
fail:
	oa_fclose(handle_get);
	oa_fdelete(OA_SOC_SETTING_FILE);
}

