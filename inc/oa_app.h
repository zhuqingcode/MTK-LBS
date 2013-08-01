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
#include "oa_blinddata.h"

typedef struct{
	u8 data_buf[DATA_MAX_LEN];//98
	u16 data_len;// 2
}timeout_data_struct;

typedef struct{
	oa_bool timeout_en;
	oa_bool do_timeout;
	oa_uint16 timeout_times;
}timeout_struct;
#endif