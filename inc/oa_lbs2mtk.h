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
 *   oa_lbs2mtk.c
 *
 * Project:
 * --------
 *   MTK-LBS project
 *
 * Description:
 * ------------
 *   This Module declares OPEN AT's api.
 *	just for transplant some code to mtk-lbs & I think tranplanting is a bad idea to this project and that makes source code
 *	looks like a shit
 * Author:
 * -------
 *   zhuqing.
 *
 ****************************************************************************/
#ifndef _OA_LBS2MTK_H_
#define _OA_LBS2MTK_H_
#include "oa_type.h"
#define SCHD_SCRN_TIME 1000


#define Sms_Ack_Enable		(u16)(1<<0)
#define LBS_Reset_Enable	(u16)(1<<1)
#define WCM_Reset_Enable	(u16)(1<<2)
#define HX_ReLogin_Enable	(u16)(1<<3)
#define HX_PPP_Enable		(u16)(1<<4)
#define TL_Update_Enable	(u16)(1<<5)
#define DEVIC_UNREGIST_Enable	(u16)(1<<6)	 //Éè±¸×¢Ïú
//#define TL_LclUpdate_Enable	(u16)(1<<6)
#define CHINESE_SMS_ENABLE	(u16)(1<<7)
#define Sms_Ack_Force_DISABLE	(u16)(1<<8)
 #endif