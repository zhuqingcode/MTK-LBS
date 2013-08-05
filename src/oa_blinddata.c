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
*   oa_blinddata.c
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
#include "oa_blinddata.h"
#include "oa_platform.h"
#include "oa_debug.h"
#include "oa_jt808.h"
#include "oa_app.h"
#include <stdio.h>
#include <stdlib.h>
extern DEV_PLAT_PARAS dev_running;
extern timeout_struct timeout_var;
extern oa_soc_context g_soc_context;
u16 total_write;
u16 total_read;
oa_bool has_blinddata_dir(void);
#if 0
 /*********************************************************
*Function:     has_blinddata_manage_file()
*Description:  if system desn't have blinddata manage file,create it
*Return:		
*Others:         
*********************************************************/
oa_bool has_blinddata_manage_file(manage_struct *p_m_s)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	manage_struct manage_data;
	
	handle = oa_fopen(MANAGE_FILE);
	if (handle < 0){
		/* create new file for setting. */
		handle = oa_fcreate(MANAGE_FILE);
		//DEBUG("handle:%d", __FILE__,  __func__, __LINE__, handle);
		/* hope never return here. */
		if (handle < 0){
			DEBUG("Create manage file failed!");
			return OA_FALSE;
		}

		oa_memset(&manage_data, 0x0, sizeof(manage_struct));
		ret = oa_fwrite(handle, &manage_data, sizeof(manage_struct), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(manage_struct)))
		{
			DEBUG("Init manage file failed!");
			oa_fclose(handle);
			return OA_FALSE;
		}
		DEBUG("Create manage file ok!");

		
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fread(handle, &manage_data, sizeof(manage_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(manage_struct))){
		DEBUG("read manage_struct err!");
		oa_fclose(handle); 
		return OA_FALSE;
	}
	oa_memcpy(p_m_s, &manage_data, sizeof(manage_struct));
	oa_fclose(handle);    
	return OA_TRUE;

}
#endif
  /*********************************************************
*Function:     has_blinddata_manage_file()
*Description:  if system desn't have blinddata manage file,create it
*Return:		
*Others:         
*********************************************************/
oa_bool has_blinddata_manage_1file(manage_struct *p_m_s)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	manage_struct manage_data;
	
	handle = oa_fopen(MANAGE_FILE);
	if (handle < 0){
		/* create new file for setting. */
		handle = oa_fcreate(MANAGE_FILE);
		//DEBUG("handle:%d", __FILE__,  __func__, __LINE__, handle);
		/* hope never return here. */
		if (handle < 0){
			DEBUG("Create manage file failed!");
			return OA_FALSE;
		}

		oa_memset(&manage_data, 0x0, sizeof(manage_struct));
		ret = oa_fwrite(handle, &manage_data, sizeof(manage_struct), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(manage_struct)))
		{
			DEBUG("Init manage file failed!");
			oa_fclose(handle);
			return OA_FALSE;
		}
		DEBUG("Create manage file ok!");

		
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fread(handle, &manage_data, sizeof(manage_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(manage_struct))){
		DEBUG("read manage_struct err!");
		oa_fclose(handle); 
		return OA_FALSE;
	}
	
	oa_memcpy(p_m_s, &manage_data, sizeof(manage_struct));
	oa_fclose(handle);    
	return OA_TRUE;

}
#if 0
 /*********************************************************
*Function:     save_to_manage_file()
*Description:  sace w/r index to manage file
*Return:		
*Others:         
*********************************************************/
oa_bool save_to_manage_file(u16 index, u8 w_r_index)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	manage_struct manage_data;
	if ((w_r_index != WRITE_INDEX) && (w_r_index != READ_INDEX)){
		DEBUG("params err!");
		return OA_FALSE;
	}

	handle = oa_fopen(MANAGE_FILE);
	if (handle < 0){
		DEBUG("open err!");
		return OA_FALSE;
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fread(handle, &manage_data, sizeof(manage_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(manage_struct))){
		DEBUG("read dir_struct err!");
		oa_fclose(handle); 
		return OA_FALSE;
	}

	
	
	if (w_r_index == WRITE_INDEX){//write
		if (manage_data.write_overflow == OA_TRUE && index >= manage_data.read_index){//overflow handle
			manage_data.read_index++;
			if (manage_data.read_index > DATA_NUM_MAX){//overflow
				manage_data.read_index = 0;
				manage_data.write_overflow = OA_FALSE;
			}
		}
		
		if (index > DATA_NUM_MAX){
			index = 0;
			manage_data.write_overflow = OA_TRUE;
		}
		manage_data.write_index = index;
	}
	else{//read
		if (index > DATA_NUM_MAX){
			index = 0;
			if (manage_data.write_overflow = OA_TRUE){
				manage_data.write_overflow = OA_FALSE;//cancel overflow
			}
		}
		manage_data.read_index = index;
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fwrite(handle, &manage_data, sizeof(manage_struct), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(manage_struct)))
	{
		DEBUG("r/w err!");
		oa_fclose(handle);
		return OA_FALSE;
	}

	oa_fclose(handle);
	return OA_TRUE;
}
#endif
 /*********************************************************
*Function:     save_to_manage_file()
*Description:  sace w/r index to manage file
*Return:		
*Others:         
*********************************************************/
oa_bool save_to_manage_1file(u16 index, u8 w_r_index)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	manage_struct manage_data;
	if ((w_r_index != WRITE_INDEX) && (w_r_index != READ_INDEX)){
		DEBUG("params err!");
		return OA_FALSE;
	}

	handle = oa_fopen(MANAGE_FILE);
	if (handle < 0){
		DEBUG("open err!");
		return OA_FALSE;
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fread(handle, &manage_data, sizeof(manage_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(manage_struct))){
		DEBUG("read dir_struct err!");
		oa_fclose(handle); 
		return OA_FALSE;
	}

	
	
	if (w_r_index == WRITE_INDEX){//write
		if (manage_data.write_overflow == OA_TRUE && index >= manage_data.read_index){//overflow handle
			manage_data.read_index++;
			if (manage_data.read_index > DATA_NUM_MAX){//overflow
				manage_data.read_index = 0;
				manage_data.write_overflow = OA_FALSE;
			}
		}
		
		if (index > DATA_NUM_MAX){
			index = 0;
			manage_data.write_overflow = OA_TRUE;
		}
		manage_data.write_index = index;
	}
	else{//read
		if (index > DATA_NUM_MAX){
			index = 0;
			if (manage_data.write_overflow = OA_TRUE){
				manage_data.write_overflow = OA_FALSE;//cancel overflow
			}
		}
		manage_data.read_index = index;
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fwrite(handle, &manage_data, sizeof(manage_struct), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(manage_struct)))
	{
		DEBUG("r/w err!");
		oa_fclose(handle);
		return OA_FALSE;
	}

	oa_fclose(handle);
	return OA_TRUE;
}
#if 0
 /*********************************************************
*Function:     write_blinddata()
*Description:  write blind data
*Return:		
*Others:         
*********************************************************/
oa_bool write_blinddata(u8 *buf, u16 len)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	manage_struct manage_data;
	blinddata_struct blind_data;
	//oa_char data_name[32] = "c:\\blinddata.dir\\data";//max:"data255"
	oa_char data_name[DATA_NAME_MAX_LEN] = {0};
	oa_char w_name[W_NAME_MAX_LEN]={0};
	if (NULL == buf || len == 0){
		OA_DEBUG_USER(" buf/len err");
		return OA_FALSE;
	}

	ret = has_blinddata_dir();
	if (!ret){//doesn't exist dir
		DEBUG("doesn't exist blinddata dir!");
		return OA_FALSE;
	}
	//create blind data manage file
	ret = has_blinddata_manage_file(&manage_data);
	if (!ret){
		DEBUG("handle manage file err!");
		return OA_FALSE;
	}

	sprintf(data_name, "c:\\blinddata.dir\\data%d"/*very important here*/, manage_data.write_index);
	//DEBUG("write_index:%d", manage_data.write_index);
	//DEBUG("data_name:%s", data_name);
	oa_chset_convert(OA_CHSET_ASCII, OA_CHSET_UCS2, data_name, w_name, W_NAME_MAX_LEN);
#if 0
	mbtowc(w_name, data_name, sizeof(data_name));
	{
		u8 i;
		for (i=0; i<32; i++){
			printf ("[%lc]", w_name);
		}
	}
#endif
	handle = oa_fopen((oa_wchar *)w_name);
	if (handle < 0){
		/* create new file for setting. */
		handle = oa_fcreate((oa_wchar *)w_name);
		/* hope never return here. */
		if (handle < 0){
			DEBUG("Create blind file failed!");
			return OA_FALSE;
		} 
	}

	if (len < DATA_MAX_LEN){
		oa_memset(blind_data.data_buf, 0x0, DATA_MAX_LEN);
		oa_memcpy(blind_data.data_buf, buf, len);
		blind_data.data_len = len;
		//DEBUG("len:%d", len);
	}
	else{
		DEBUG("blind data is too long!");
		return OA_FALSE;
	}

	ret = oa_fwrite(handle, &blind_data, sizeof(blind_data), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(blind_data))){
		DEBUG("write blind data err!");
		oa_fclose(handle);
		return OA_FALSE;
	}
	
	manage_data.write_index++;
	ret = save_to_manage_file(manage_data.write_index, WRITE_INDEX);
	if (ret == OA_FALSE){
		oa_fclose(handle);
		return OA_FALSE;
	}
	
	oa_fclose(handle);
	return OA_TRUE;
	
}
#endif
  /*********************************************************
*Function:     write_blinddata()
*Description:  write blind data
*Return:		
*Others:         
*********************************************************/
oa_bool write_blinddata_to_1file(u8 *buf, u16 len)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	manage_struct manage_data;
	blinddata_struct blind_data;
	//oa_wchar data_name[32] = L"c:\\blinddata.dir\\data";
	oa_int32 offset;
	if (NULL == buf || len == 0){
		OA_DEBUG_USER(" buf/len err");
		return OA_FALSE;
	}

	ret = has_blinddata_dir();
	if (!ret){//doesn't exist dir
		DEBUG("doesn't exist blinddata dir!");
		return OA_FALSE;
	}
	//create blind data manage file
	ret = has_blinddata_manage_1file(&manage_data);
	if (!ret){
		DEBUG("handle manage file err!");
		return OA_FALSE;
	}
	//debuf info
	total_write = manage_data.write_index+1;
	
	handle = oa_fopen(DATANAME_1FILE);
	if (handle < 0){
		/* create new file for setting. */
		handle = oa_fcreate(DATANAME_1FILE);
		/* hope never return here. */
		if (handle < 0){
			DEBUG("Create blind file failed!");
			return OA_FALSE;
		} 
	}

	if (len < DATA_MAX_LEN){
		oa_memset(blind_data.data_buf, 0x0, DATA_MAX_LEN);
		oa_memcpy(blind_data.data_buf, buf, len);
		blind_data.data_len = len;
		//DEBUG("len:%d", len);
	}
	else{
		DEBUG("blind data is too long!");
		return OA_FALSE;
	}

	offset = manage_data.write_index * MAXLEN_1BLINDDATA;//locate
	oa_fseek(handle, offset, OA_FILE_BEGIN);
	ret = oa_fwrite(handle, &blind_data, sizeof(blind_data), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(blind_data))){
		DEBUG("write blind data err, maybe has no space!");
		oa_fclose(handle);
		return OA_FALSE;
	}
	
	manage_data.write_index++;//next index to write
	ret = save_to_manage_1file(manage_data.write_index, WRITE_INDEX);
	if (ret == OA_FALSE){
		oa_fclose(handle);
		return OA_FALSE;
	}
	
	oa_fclose(handle);
	return OA_TRUE;
	
}
#if 0
 /*********************************************************
*Function:     read_blinddata()
*Description:  read blind data to buf
*Return:		
*Others:         
*********************************************************/
oa_bool read_blinddata(u8 *buf, u16 *p_len)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	manage_struct manage_data;
	blinddata_struct blind_data;
	oa_char data_name[DATA_NAME_MAX_LEN] = {0};
	oa_char w_name[W_NAME_MAX_LEN]={0};
	if (NULL == buf){
		OA_DEBUG_USER(" buf err");
		return OA_FALSE;
	}

	ret = has_blinddata_dir();
	if (!ret){//doesn't exist dir
		DEBUG("doesn't exist blinddata dir!");
		return OA_FALSE;
	}
	//create blind data manage file
	ret = has_blinddata_manage_file(&manage_data);//copy manage info to 'manage_data'
	if (!ret){
		DEBUG("handle manage file err!");
		return OA_FALSE;
	}

	if (manage_data.read_index > manage_data.write_index){//read overflow:means no data
		if (manage_data.write_overflow == OA_FALSE){//case:|.....w_p.....r_p.......|
			return OA_FALSE;
		}
		
	}
	
	//oa_sprintf(data_name+22, "%d", manage_data.read_index);
	sprintf(data_name, "c:\\blinddata.dir\\data%d"/*very important here*/, manage_data.read_index);
	oa_chset_convert(OA_CHSET_ASCII, OA_CHSET_UCS2, data_name, w_name, W_NAME_MAX_LEN);
	handle = oa_fopen((oa_wchar *)w_name);
	if (handle < 0){
		//DEBUG("has no this blind data!");
		return OA_FALSE;
	}

	ret = oa_fread(handle, &blind_data, sizeof(blinddata_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(blinddata_struct))){
		DEBUG("read err!");
		oa_fclose(handle); 
		return OA_FALSE;
	}

	oa_memcpy(buf, blind_data.data_buf, blind_data.data_len);
	*p_len = blind_data.data_len;
	manage_data.read_index++;//next
	ret = save_to_manage_file(manage_data.read_index, READ_INDEX);
	if (ret == OA_FALSE){
		oa_fclose(handle);
		return OA_FALSE;
	}
	
	oa_fclose(handle);
	return OA_TRUE;
	
}
#endif
 /*********************************************************
*Function:     read_blinddata()
*Description:  read blind data to buf
*Return:		
*Others:         
*********************************************************/
oa_bool read_blinddata_from_1file(u8 *buf, u16 *p_len)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	manage_struct manage_data;
	blinddata_struct blind_data;
	oa_uint32 offset;
	
	if (NULL == buf){
		OA_DEBUG_USER(" buf err");
		return OA_FALSE;
	}

	ret = has_blinddata_dir();
	if (!ret){//doesn't exist dir
		DEBUG("doesn't exist blinddata dir!");
		return OA_FALSE;
	}
	//create blind data manage file
	ret = has_blinddata_manage_1file(&manage_data);//copy manage info to 'manage_data'
	if (!ret){
		DEBUG("handle manage file err!");
		return OA_FALSE;
	}
	//debuf info
	total_read = manage_data.read_index+1;
	//DEBUG("w_index:%d,r_index:%d", __FILE__,  __func__, __LINE__, manage_data.write_index,manage_data.read_index);
	if (manage_data.read_index >= manage_data.write_index){//read overflow:means no data
		if (manage_data.write_overflow == OA_FALSE){//case:|.....w_p.....r_p.......|
			return OA_FALSE;
		}
	}
	
	handle = oa_fopen(DATANAME_1FILE);
	if (handle < 0){
		//DEBUG("has no this blind data!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}

	offset = manage_data.read_index * MAXLEN_1BLINDDATA;//locate
	oa_fseek(handle, offset, OA_FILE_BEGIN);
	ret = oa_fread(handle, &blind_data, sizeof(blinddata_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(blinddata_struct))){
		DEBUG("read err!");
		oa_fclose(handle); 
		return OA_FALSE;
	}

	oa_memcpy(buf, blind_data.data_buf, blind_data.data_len);
	*p_len = blind_data.data_len;
	manage_data.read_index++;//next
	ret = save_to_manage_1file(manage_data.read_index, READ_INDEX);
	if (ret == OA_FALSE){
		oa_fclose(handle);
		return OA_FALSE;
	}
	
	oa_fclose(handle);
	return OA_TRUE;
	
}

 /*********************************************************
*Function:     has_blinddata_dir()
*Description:  if system desn't have blinddata directory,create it 
*Return:		
*Others:         
*********************************************************/
oa_bool has_blinddata_dir(void)
{	
	oa_int32 handle, ret,dir_handle;
	dir_struct dir;
	oa_uint32 dummy_read, dummy_write;
	
	handle = oa_fopen(BLINDDATA_DIR_CONF);
	if (handle < 0){
		/* create new file for setting. */
		handle = oa_fcreate(BLINDDATA_DIR_CONF);
		/* hope never return here. */
		if (handle < 0){
			DEBUG("Create blinddata config file failed!");
			goto fail;
		}
		dir.dir_exist = OA_FALSE;
		ret = oa_fwrite(handle, &dir, sizeof(dir_struct), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(dir_struct))){
			OA_DEBUG_USER("Init blinddata config file failed!");
			goto fail;
		}

		DEBUG("Create blinddata config file ok!");    
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fread(handle, &dir, sizeof(dir_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(dir_struct))){
		DEBUG("read dir_struct err!");
		goto fail;
	}
	//-----------------------------------------------------------------------------
	if (dir.dir_exist == OA_FALSE){
		dir_handle = oa_fcreateDir(BLINDDATA_DIRNAME);
		if (dir_handle < 0){
			DEBUG(" create blinddata dir err!");
			goto fail;
		}
		dir.dir_exist = OA_TRUE;
		oa_fseek(handle, 0, OA_FILE_BEGIN);
		ret = oa_fwrite(handle, &dir, sizeof(dir_struct), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(dir_struct))){
			DEBUG("Init blinddata config file failed!");
			goto fail;
		}
		oa_fclose(handle);
		return OA_TRUE;
	}
	else{
		oa_fclose(handle);
		return OA_TRUE;
	}
fail:
	oa_fclose(handle);
	oa_fdelete(BLINDDATA_DIR_CONF);
	return OA_FALSE;

}
 /*********************************************************
*Function:     oa_app_blinddata()
*Description:  operations about blinddata
*Return:		
*Others:         
*********************************************************/
oa_bool oa_app_blinddata(void)
{
	oa_bool ret;
	u8 blind_buf[DATA_MAX_LEN] = {0};
	oa_int16 data_len;
	oa_int16 ret_len;
	oa_int16 send_len;
	static oa_bool task_runed = OA_TRUE;
	//static u16 ReqMsgId = REPORT_LOCATION;
	if (OA_TRUE == task_runed){
		DEBUG("(:(:(:(:(:(:(:(:task is %s running:):):):):):):):)", __func__);
		task_runed = OA_FALSE;
	}
	
	ret = has_blinddata_dir();
	if (ret == OA_FALSE){
		goto redoit;
	}
	//if device is doing timeout, do not send blind data
	if (dev_running.plat_status == ONLINE && timeout_var.do_timeout == OA_FALSE 
		&& g_soc_context.is_blocksend == OA_FALSE){
		//ret = read_blinddata(blind_buf, &data_len);
		ret = read_blinddata_from_1file(blind_buf, &data_len);
		if (ret == OA_FALSE){
			goto redoit;
		}
		//has blinddata
		ret_len = escape_copy_to_send(blind_buf, data_len);
		//DEBUG(" ret_len:%d!", ret_len);
		if (ret_len > 0){
			print_rtc_time();
			//Write_ProtclHandl(eDevMsgid, (u8 *)&ReqMsgId, 2);
			timeout_var.timeout_en = OA_FALSE;
			send_len = oa_soc_send_req();//check datas in buffer & send
			if (send_len == ret_len){
				DEBUG("^^^send one blinddata packet!total send num:%d", total_read);
			}
			else{
				DEBUG("^^^send err, write back!");
				write_blinddata_to_1file(blind_buf, data_len);
			} 
		}
	}
	
	

redoit:
	oa_timer_start(OA_TIMER_ID_7, oa_app_blinddata, NULL, OA_APP_BLINDDATA);//
}
