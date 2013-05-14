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
#ifndef __OA_USE_LOCK_H__
#define __OA_USE_LOCK_H__
#include "oa_type.h"
#include "oa_api.h"

#define USE_LOCK_FILE L"use_lock.ini"
typedef struct
{
	oa_bool lock;//OA_TRUE : lock,OA_FALSE:unlock
}USE_LOCK;

extern oa_bool use_lock(void);
extern oa_bool use_unlock(void);
extern oa_bool use_is_lock(void);
#endif