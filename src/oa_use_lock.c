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
*   oa_lock_use.c
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
#include "oa_use_lock.h"
#include "oa_debug.h"

USE_LOCK def_use_lock = 
{
	//OA_TRUE,// lock
	OA_FALSE,// unlock
};
USE_LOCK now_use_lock;

/*********************************************************
*Function:      use_unlock()
*Description:   unlock the device
*Return:         oa_bool
*Others:         after use_unlocking, please restart.
*********************************************************/
oa_bool use_unlock(void)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_write;
	
	handle = oa_fopen(USE_LOCK_FILE);
	if (handle < 0)
	{
		DEBUG("use unlock failed!");
		return OA_FALSE;
	}

	oa_fseek(handle, 0, OA_FILE_BEGIN);
	now_use_lock.lock = OA_FALSE;
	ret = oa_fwrite(handle, &now_use_lock, sizeof(now_use_lock), &dummy_write);
	if ((ret < 0) || (dummy_write != sizeof(now_use_lock)))
	{
		DEBUG("use unlock write err!");
		return OA_FALSE;
	}

	oa_fclose(handle);
	DEBUG("use unlock ok!");
	return OA_TRUE;
}
/*********************************************************
*Function:      use_is_lock()
*Description:   Determines whether the device is locked?
*Return:         oa_bool
*Others:         
*********************************************************/
oa_bool use_is_lock(void)
{
	if (now_use_lock.lock == OA_TRUE)
	{
		return OA_TRUE;
	}
	else if (now_use_lock.lock == OA_FALSE)
	{
		return OA_FALSE;
	}

}
/*********************************************************
*Function:      use_lock()
*Description:   lock device before using
*Return:         oa_bool
*Others:         
*********************************************************/
oa_bool use_lock(void)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	
	handle = oa_fopen(USE_LOCK_FILE);
	if (handle < 0)
	{
		/* create new file for setting. */
		handle = oa_fcreate(USE_LOCK_FILE);
		/* hope never return here. */
		if (handle < 0)
		{
			DEBUG("Create use lock file failed!");
			return OA_FALSE;
		}

		ret = oa_fwrite(handle, &def_use_lock, sizeof(def_use_lock), &dummy_write);
		if ((ret < 0) || (dummy_write != sizeof(def_use_lock)))
		{
			DEBUG("Init use lock file failed!");
			return OA_FALSE;
		}

		DEBUG("Create use lock file ok!");    

		oa_fseek(handle, 0, OA_FILE_BEGIN);
	}
	//read lock status to 'now_use_lock'
	ret = oa_fread(handle, &now_use_lock, sizeof(now_use_lock), &dummy_read);
	if ((ret < 0) || (dummy_read != sizeof(now_use_lock))) goto fail;

	oa_fclose(handle); 
	return OA_TRUE;

	fail:
	//need delete the file and reset here..
	oa_fclose(handle);
	oa_fdelete(USE_LOCK_FILE);
	return OA_FALSE;
}