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
#include <stdio.h>
#include <stdlib.h>
extern DEV_PLAT_PARAS dev_running;
u16 total_write;
u16 total_read;
oa_bool has_blinddata_dir(void);
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
		//Trace("(%s:%s:%d):handle:%d", __FILE__,  __func__, __LINE__, handle);
		/* hope never return here. */
		if (handle < 0){
			Trace("(%s:%s:%d):Create manage file failed!", __FILE__,  __func__, __LINE__);
			return OA_FALSE;
		}

		oa_memset(&manage_data, 0x0, sizeof(manage_struct));
		ret = oa_fwrite(handle, &manage_data, sizeof(manage_struct), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(manage_struct)))
		{
			Trace("(%s:%s:%d):Init manage file failed!", __FILE__,  __func__, __LINE__);
			return OA_FALSE;
		}
		Trace("(%s:%s:%d):Create manage file ok!", __FILE__,  __func__, __LINE__);

		
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fread(handle, &manage_data, sizeof(manage_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(manage_struct))){
		Trace("(%s:%s:%d):read manage_struct err!", __FILE__,  __func__, __LINE__);
		oa_fclose(handle); 
		return OA_FALSE;
	}
	oa_memcpy(p_m_s, &manage_data, sizeof(manage_struct));
	oa_fclose(handle);    
	return OA_TRUE;

}
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
		//Trace("(%s:%s:%d):handle:%d", __FILE__,  __func__, __LINE__, handle);
		/* hope never return here. */
		if (handle < 0){
			Trace("(%s:%s:%d):Create manage file failed!", __FILE__,  __func__, __LINE__);
			return OA_FALSE;
		}

		oa_memset(&manage_data, 0x0, sizeof(manage_struct));
		ret = oa_fwrite(handle, &manage_data, sizeof(manage_struct), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(manage_struct)))
		{
			Trace("(%s:%s:%d):Init manage file failed!", __FILE__,  __func__, __LINE__);
			return OA_FALSE;
		}
		Trace("(%s:%s:%d):Create manage file ok!", __FILE__,  __func__, __LINE__);

		
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fread(handle, &manage_data, sizeof(manage_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(manage_struct))){
		Trace("(%s:%s:%d):read manage_struct err!", __FILE__,  __func__, __LINE__);
		oa_fclose(handle); 
		return OA_FALSE;
	}
	
	oa_memcpy(p_m_s, &manage_data, sizeof(manage_struct));
	oa_fclose(handle);    
	return OA_TRUE;

}

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
		Trace("(%s:%s:%d):params err!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}

	handle = oa_fopen(MANAGE_FILE);
	if (handle < 0){
		Trace("(%s:%s:%d):open err!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fread(handle, &manage_data, sizeof(manage_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(manage_struct))){
		Trace("(%s:%s:%d):read dir_struct err!", __FILE__,  __func__, __LINE__);
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
		Trace("(%s:%s:%d):r/w err!", __FILE__,  __func__, __LINE__);
		oa_fclose(handle);
		return OA_FALSE;
	}

	oa_fclose(handle);
	return OA_TRUE;
}
 
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
		Trace("(%s:%s:%d):params err!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}

	handle = oa_fopen(MANAGE_FILE);
	if (handle < 0){
		Trace("(%s:%s:%d):open err!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fread(handle, &manage_data, sizeof(manage_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(manage_struct))){
		Trace("(%s:%s:%d):read dir_struct err!", __FILE__,  __func__, __LINE__);
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
		Trace("(%s:%s:%d):r/w err!", __FILE__,  __func__, __LINE__);
		oa_fclose(handle);
		return OA_FALSE;
	}

	oa_fclose(handle);
	return OA_TRUE;
}

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
		OA_DEBUG_USER("(%s:%s:%d): buf/len err", __FILE__, __func__, __LINE__);
		return OA_FALSE;
	}

	ret = has_blinddata_dir();
	if (!ret){//doesn't exist dir
		Trace("(%s:%s:%d):doesn't exist blinddata dir!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}
	//create blind data manage file
	ret = has_blinddata_manage_file(&manage_data);
	if (!ret){
		Trace("(%s:%s:%d):handle manage file err!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}

	sprintf(data_name, "c:\\blinddata.dir\\data%d"/*very important here*/, manage_data.write_index);
	//Trace("(%s:%s:%d):write_index:%d", __FILE__,  __func__, __LINE__, manage_data.write_index);
	//Trace("(%s:%s:%d):data_name:%s", __FILE__,  __func__, __LINE__, data_name);
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
			Trace("(%s:%s:%d):Create blind file failed!", __FILE__,  __func__, __LINE__);
			return OA_FALSE;
		} 
	}

	if (len < DATA_MAX_LEN){
		oa_memset(blind_data.data_buf, 0x0, DATA_MAX_LEN);
		oa_memcpy(blind_data.data_buf, buf, len);
		blind_data.data_len = len;
		//Trace("(%s:%s:%d):len:%d", __FILE__,  __func__, __LINE__, len);
	}
	else{
		Trace("(%s:%s:%d):blind data is too long!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}

	ret = oa_fwrite(handle, &blind_data, sizeof(blind_data), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(blind_data))){
		Trace("(%s:%s:%d):write blind data err!", __FILE__,  __func__, __LINE__);
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
		OA_DEBUG_USER("(%s:%s:%d): buf/len err", __FILE__, __func__, __LINE__);
		return OA_FALSE;
	}

	ret = has_blinddata_dir();
	if (!ret){//doesn't exist dir
		Trace("(%s:%s:%d):doesn't exist blinddata dir!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}
	//create blind data manage file
	ret = has_blinddata_manage_1file(&manage_data);
	if (!ret){
		Trace("(%s:%s:%d):handle manage file err!", __FILE__,  __func__, __LINE__);
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
			Trace("(%s:%s:%d):Create blind file failed!", __FILE__,  __func__, __LINE__);
			return OA_FALSE;
		} 
	}

	if (len < DATA_MAX_LEN){
		oa_memset(blind_data.data_buf, 0x0, DATA_MAX_LEN);
		oa_memcpy(blind_data.data_buf, buf, len);
		blind_data.data_len = len;
		//Trace("(%s:%s:%d):len:%d", __FILE__,  __func__, __LINE__, len);
	}
	else{
		Trace("(%s:%s:%d):blind data is too long!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}

	offset = manage_data.write_index * MAXLEN_1BLINDDATA;//locate
	oa_fseek(handle, offset, OA_FILE_BEGIN);
	ret = oa_fwrite(handle, &blind_data, sizeof(blind_data), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(blind_data))){
		Trace("(%s:%s:%d):write blind data err, maybe has no space!", __FILE__,  __func__, __LINE__);
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
		OA_DEBUG_USER("(%s:%s:%d): buf err", __FILE__, __func__, __LINE__);
		return OA_FALSE;
	}

	ret = has_blinddata_dir();
	if (!ret){//doesn't exist dir
		Trace("(%s:%s:%d):doesn't exist blinddata dir!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}
	//create blind data manage file
	ret = has_blinddata_manage_file(&manage_data);//copy manage info to 'manage_data'
	if (!ret){
		Trace("(%s:%s:%d):handle manage file err!", __FILE__,  __func__, __LINE__);
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
		//Trace("(%s:%s:%d):has no this blind data!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}

	ret = oa_fread(handle, &blind_data, sizeof(blinddata_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(blinddata_struct))){
		Trace("(%s:%s:%d):read err!", __FILE__,  __func__, __LINE__);
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
		OA_DEBUG_USER("(%s:%s:%d): buf err", __FILE__, __func__, __LINE__);
		return OA_FALSE;
	}

	ret = has_blinddata_dir();
	if (!ret){//doesn't exist dir
		Trace("(%s:%s:%d):doesn't exist blinddata dir!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}
	//create blind data manage file
	ret = has_blinddata_manage_1file(&manage_data);//copy manage info to 'manage_data'
	if (!ret){
		Trace("(%s:%s:%d):handle manage file err!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}
	//debuf info
	total_read = manage_data.read_index+1;
	//Trace("(%s:%s:%d):w_index:%d,r_index:%d", __FILE__,  __func__, __LINE__, manage_data.write_index,manage_data.read_index);
	if (manage_data.read_index >= manage_data.write_index){//read overflow:means no data
		if (manage_data.write_overflow == OA_FALSE){//case:|.....w_p.....r_p.......|
			return OA_FALSE;
		}
	}
	
	handle = oa_fopen(DATANAME_1FILE);
	if (handle < 0){
		//Trace("(%s:%s:%d):has no this blind data!", __FILE__,  __func__, __LINE__);
		return OA_FALSE;
	}

	offset = manage_data.read_index * MAXLEN_1BLINDDATA;//locate
	oa_fseek(handle, offset, OA_FILE_BEGIN);
	ret = oa_fread(handle, &blind_data, sizeof(blinddata_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(blinddata_struct))){
		Trace("(%s:%s:%d):read err!", __FILE__,  __func__, __LINE__);
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
			Trace("(%s:%s:%d):Create blinddata config file failed!", __FILE__,  __func__, __LINE__);
			goto fail;
		}
		dir.dir_exist = OA_FALSE;
		ret = oa_fwrite(handle, &dir, sizeof(dir_struct), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(dir_struct))){
			OA_DEBUG_USER("(%s:%s:%d):Init blinddata config file failed!", __FILE__,  __func__, __LINE__);
			goto fail;
		}

		Trace("(%s:%s:%d):Create blinddata config file ok!",__FILE__, __func__, __LINE__);    
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fread(handle, &dir, sizeof(dir_struct), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(dir_struct))){
		Trace("(%s:%s:%d):read dir_struct err!", __FILE__,  __func__, __LINE__);
		goto fail;
	}
	//-----------------------------------------------------------------------------
	if (dir.dir_exist == OA_FALSE){
		dir_handle = oa_fcreateDir(BLINDDATA_DIRNAME);
		if (dir_handle < 0){
			Trace("(%s:%s:%d): create blinddata dir err!", __FILE__, __func__, __LINE__);
			goto fail;
		}
		dir.dir_exist = OA_TRUE;
		oa_fseek(handle, 0, OA_FILE_BEGIN);
		ret = oa_fwrite(handle, &dir, sizeof(dir_struct), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(dir_struct))){
			Trace("(%s:%s:%d):Init blinddata config file failed!", __FILE__, __func__, __LINE__);
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
	u16 data_len;
	u16 ret_len;
	static oa_bool task_runed = OA_TRUE;
	
	if (OA_TRUE == task_runed){
		OA_DEBUG_USER("<<<<<<<<<<<<<task %s is running......>>>>>>>>>>>>>", __func__);
		task_runed = OA_FALSE;
	}
	
	ret = has_blinddata_dir();
	if (ret == OA_FALSE){
		goto redoit;
	}

	if (dev_running.plat_status == ONLINE){
		//ret = read_blinddata(blind_buf, &data_len);
		ret = read_blinddata_from_1file(blind_buf, &data_len);
		if (ret == OA_FALSE){
			goto redoit;
		}
		//has blinddata
		ret_len = escape_copy_to_send(blind_buf, data_len);
		//Trace("(%s:%s:%d): ret_len:%d!", __FILE__, __func__, __LINE__, ret_len);
		if (ret_len > 0){
			oa_soc_send_req();//check datas in buffer & send
			print_rtc_time();
			Trace("send one blinddata packet!total send num:%d", total_read);
		}
		

	}
	
	

redoit:
	oa_timer_start(OA_TIMER_ID_7, oa_app_blinddata, NULL, OA_APP_BLINDDATA);//
}
