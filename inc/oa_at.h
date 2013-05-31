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
 *   oa_at.h
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
#ifndef __OA_AT_H__
#define __OA_AT_H__
#include "oa_type.h"
#include "oa_api.h"
#define AT_FEDBAK_MAX_LEN 512
//#define AT_RECV_PRINT
//#define CUSTOM_AT
#define TEST_NUM "138xxxxxxxx" //just for test
#define RING_CLIP "+CLIP: "
#define QUOTES "\""
#define RING_NUM_MAX_LEN 32
//#define FEDBAK

typedef enum{
	call_strange = 0x1,
	call_reset,
	call_factory_set,
	call_err = 0xf,
}call_kind;
//extern void oa_app_at(void);
#endif

