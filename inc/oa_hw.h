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

#define OA_HW_VERSION_NO  "v1.2"

#define EDGE_SENSITIVE OA_TRUE
#define LEVEL_SENSITIVE OA_FALSE
#define OA_CUR_TEST_EINT_NO 0/**/
#define OA_CUR_TEST_EINT_NO1 5/**/
//#define ACC_GPIO 19 //gpio19
#define ACC_GPIO 4 //gpio4
#define GPSANA_GPIO 3//gpio3
#define KEY_GPIO 7
#define IN0_GPIO 17
#define IN1_GPIO 12
#define OUT_GPIO 11
#define SCRN_POWER 5
#define GPIO_INPUT 0
#define GPIO_OUTPUT 1
#define GPIO_MODE 0
#define POWER_HIGH 15
#define ACC_ON 1
#define ACC_OFF 0
#define OA_ACC_RUN 1000
#define OA_ACC_RUN_1ST 20000
#define OA_ACC_RUN_SECOND (OA_ACC_RUN/1000)
#define USE_ZFZ_SENSOR
#define LEVEL_HIGH 1
#define LEVEL_LOW 0
#if 0
official hw definition
#define GPS_AADET 3//gpio3
#define GPS_EN 18//gpio18
#define KEY 7//gpio7
#define ACC 4//gpio4
#define ALARM0 17//gpio17
#define ALARM1 12//gpio12
#define ALARM_OUT//gpio11
#endif
extern void acc_status_detect(void *param);
#endif
