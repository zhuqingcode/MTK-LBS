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
 *   oa_mutex.h
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
#ifndef __OA_MUTEX_H__
#define __OA_MUTEX_H__

typedef enum
{
	MUTEX_BUSY = 0,
	MUTEX_AVAILABLE
}OA_MUTEX;

typedef enum
{
	GET_MUTEX_ERR = 0,
	GET_MUTEX_OK
}OA_MUTEX_GET_STATUS;

extern OA_MUTEX_GET_STATUS get_mutex(OA_MUTEX mutex);
extern void free_mutex(OA_MUTEX mutex);
#endif



