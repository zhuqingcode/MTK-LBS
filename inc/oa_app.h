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
*   oa_app.h
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
#ifndef __OA_APP_H__
#define __OA_APP_H__
#include "oa_type.h"
#include "oa_soc.h"
typedef struct{
	oa_uint8 data[OA_APP_GPRS_SENDING_SIZE];
	oa_uint16 len;
}soc_bak_context;

typedef struct{
	oa_bool timeout_en;
	oa_bool do_timeout;
	oa_uint16 timeout_times;
}timeout_struct;
#endif