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
 *   oa_hw.h
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
#ifndef __OA_ZFZ_H__
#define __OA_ZFZ_H__

#include "oa_type.h"
#include "oa_api.h"
typedef enum {
	tz = 0x0,
	zz = 0x01,
	fz = 0x03,
}zfz_enum;

#define TZ_SHRESHOLD_TIME (60 * 5)
#define ZZ_SHRESHOLD_TIMES 5
#define FZ_SHRESHOLD_TIMES 5

#endif

