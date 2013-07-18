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
 *   oa_platform.c
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
#include "oa_gps.h"
#include "oa_platform.h"
#include "oa_debug.h"

extern DEV_PLAT_PARAS dev_running;
/*********************************************************
*Function:      mile_stat_init()
*Description:  init the mileage statics
*Return:        void
*Others:         
*********************************************************/
oa_bool mile_stat_init(void)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	TOTAL_MILE mileage;
	
	handle = oa_fopen(MILEAGE_FILE);
	if (handle < 0){
		/* create new file for setting. */
		handle = oa_fcreate(MILEAGE_FILE);
		/* hope never return here. */
		if (handle < 0){
			DEBUG("Create mileage file failed!");
			return OA_FALSE;
		}
		mileage.total_mileage = 0;
		ret = oa_fwrite(handle, &mileage, sizeof(TOTAL_MILE), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(TOTAL_MILE))){
			DEBUG("Init mileage file failed!");
			return OA_FALSE;
		}
		DEBUG("Create mileage file ok!");    
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
*Function:      mile_stat_add()
*Description:  calculate the mileage statics
*Return:        void
*Others:         
*********************************************************/
oa_bool mile_stat_add(FP32 p_mile)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	TOTAL_MILE mileage;
	
	handle = oa_fopen(MILEAGE_FILE);
	if (handle < 0){
		#if 0
		/* create new file for setting. */
		handle = oa_fcreate(MILEAGE_FILE);
		/* hope never return here. */
		if (handle < 0){
			DEBUG("Create mileage file failed!", __FILE__,  __func__, __LINE__);
			return OA_FALSE;
		}
		mileage.total_mileage = 0;
		ret = oa_fwrite(handle, &mileage, sizeof(TOTAL_MILE), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(TOTAL_MILE))){
			DEBUG("Init mileage file failed!", __FILE__,  __func__, __LINE__);
			return OA_FALSE;
		}
		DEBUG("Create mileage file ok!", __FILE__,  __func__, __LINE__);
		#endif
		return OA_FALSE;
	}
	oa_fseek(handle, 0, OA_FILE_BEGIN);
	ret = oa_fread(handle, &mileage, sizeof(TOTAL_MILE), &dummy_read);
	//DEBUG("after read total_mileage:%d", mileage.total_mileage);
	if ((ret < 0) || (dummy_read != sizeof(TOTAL_MILE))){
		DEBUG("read mileage err!");
		oa_fclose(handle);
		return OA_FALSE;
	}
	//DEBUG("give to func total_mileage:%f", p_mile);
	mileage.total_mileage += (u32)(p_mile * 10.0);//floor//cut off dot part
	oa_fseek(handle, 0, OA_FILE_BEGIN);//<do not forget!!!>
	//just for debug
	//DEBUG("total_mileage:%d", __FILE__,  __func__, __LINE__, mileage.total_mileage);
	ret = oa_fwrite(handle, &mileage, sizeof(TOTAL_MILE), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(TOTAL_MILE))){
		DEBUG("calculate mileage err!");
		oa_fclose(handle);
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
*Function:     has_reg()
*Description:  judge has registered or not
*Return:		OA_FALSE:has not registered;OA_TRUE:has registered
*Others:         
*********************************************************/
oa_bool has_reg(void)
{
	oa_int32 handle;
	
	handle = oa_fopen(AUTHEN_FILE);//just check has 'AUTHEN_FILE' or not
	if (handle < 0)
	{
		return OA_FALSE;//means has not registered
	}
	else
	{
		oa_fclose(handle);
		return OA_TRUE;
	}
}

 /*********************************************************
*Function:     authen_code_handle()
*Description:  operations about authentication
*Return:		
*Others:         
*********************************************************/
oa_bool authen_code_handle(oa_uint8 *code_buf, oa_uint8 code_len)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	AUTHEN_STRUCT authen;
	
	if (NULL == code_buf || code_len == 0)
	{
		DEBUG(" arg err");
		return OA_FALSE;
	}
	
	handle = oa_fopen(AUTHEN_FILE);
	if (handle < 0)
	{
		/* create new file for setting. */
		handle = oa_fcreate(AUTHEN_FILE);
		/* hope never return here. */
		if (handle < 0)
		{
			DEBUG(" Create authen file failed!");
			return OA_FALSE;
		}
		oa_memset(&authen, 0x0, sizeof(AUTHEN_STRUCT));
		ret = oa_fwrite(handle, &authen, sizeof(AUTHEN_STRUCT), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(AUTHEN_STRUCT)))
		{
			DEBUG(" Init authen file failed!");
			return OA_FALSE;
		}

		DEBUG(" Create authen file ok!");    
	}

	oa_memset(&authen, 0x0, sizeof(AUTHEN_STRUCT));
	oa_fseek(handle, 0, OA_FILE_BEGIN);
	if (code_len < AUTHEN_CODE_MAX_LEN)
	{
		authen.authen_code_len = code_len;
	}
	else
	{
		DEBUG(" code is too long!");
		oa_fclose(handle);
		return OA_FALSE;
	}
	
	oa_memcpy(authen.authen_code, code_buf, code_len);
	authen.authen_status = OA_TRUE;
	ret = oa_fwrite(handle, &authen, sizeof(AUTHEN_STRUCT), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(AUTHEN_STRUCT)))
	{
		DEBUG(" write authen file failed!");
		oa_fclose(handle);
		return OA_FALSE;
	}
	
	oa_fclose(handle);
	return OA_TRUE;
}

 /*********************************************************
*Function:     read_authen_code()
*Description:  read authentication code
*Return:		
*Others:         
*********************************************************/
oa_bool read_authen_code(oa_uint8 *code_buf, oa_uint16 *p_code_len)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	AUTHEN_STRUCT authen;
	
	if (NULL == code_buf || p_code_len == NULL)
	{
		DEBUG(" arg err");
		return OA_FALSE;
	}

	handle = oa_fopen(AUTHEN_FILE);
	if (handle < 0)
	{
		DEBUG(" open err");
		return OA_FALSE;
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	//read Authentication Code
	ret = oa_fread(handle, &authen, sizeof(AUTHEN_STRUCT), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(AUTHEN_STRUCT)))
	{
		DEBUG(" read err");
		oa_fclose(handle);
		return OA_FALSE;
	}

	*p_code_len = authen.authen_code_len;
	oa_memcpy(code_buf, authen.authen_code, *p_code_len);
	oa_fclose(handle);
	return OA_TRUE;
}
  /*********************************************************
*Function:     read_authen_code()
*Description:  save authentication code
*Return:		
*Others:         
*********************************************************/
oa_bool save_authen_code(oa_uint8 *code_buf, oa_uint8 code_len)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	AUTHEN_STRUCT authen;
	
	if (NULL == code_buf || code_len == 0)
	{
		DEBUG(" arg err");
		return OA_FALSE;
	}

	handle = oa_fopen(AUTHEN_FILE);
	if (handle < 0)
	{
		DEBUG(" open err");
		return OA_FALSE;
	}

	oa_memset(&authen, 0x0, sizeof(AUTHEN_STRUCT));
	
	oa_fseek(handle, 0, OA_FILE_BEGIN);
	if (code_len < AUTHEN_CODE_MAX_LEN)
	{
		authen.authen_code_len = code_len;
	}
	else
	{
		DEBUG(" code is too long!");
		oa_fclose(handle);
		return OA_FALSE;
	}
	
	oa_memcpy(authen.authen_code, code_buf, code_len);
	authen.authen_status = OA_TRUE;
	ret = oa_fwrite(handle, &authen, sizeof(AUTHEN_STRUCT), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(AUTHEN_STRUCT)))
	{
		DEBUG(" write authen file failed!");
		oa_fclose(handle);
		return OA_FALSE;
	}
	oa_fclose(handle);
	return OA_TRUE;
}

