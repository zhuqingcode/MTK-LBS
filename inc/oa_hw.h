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
#ifndef __OA_HW_H__
#define __OA_HW_H__

#include "oa_type.h"
#include "oa_api.h"

//#define EINT				//”≤º˛÷–∂œ
//#define TONE_PLAY               //“Ù∆µ≤•∑≈

#define OA_HW_VERSION_NO  "V 00.00.00"

#define EDGE_SENSITIVE OA_TRUE
#define LEVEL_SENSITIVE OA_FALSE
#define OA_CUR_TEST_EINT_NO 0/**/

#define ACC_GPIO 19 //gpio19
#define GPSANA_GPIO 3//gpio3
#define KEY_GPIO 18
#define IN0_GPIO 7
#define IN1_GPIO 4
#define OUT_GPIO 17

#define GPIO_INPUT 0
#define GPIO_OUTPUT 1
#define GPIO_MODE 0

#define ACC_ON 1
#define ACC_OFF 0
#define OA_ACC_RUN 10000
extern void acc_status_detect(void *param);
#endif
