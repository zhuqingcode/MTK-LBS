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
*   oa_uart.h
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
#ifndef __OA_UART_H__
#define __OA_UART_H__
#include "oa_type.h"
#include "oa_api.h"
#define AT_PS                 	//AT命令口
#define USE_UART2			//串口二
#define USE_UART3			//串口三
#define UART_SCREEN 0x2
#define UART_FUEL_SENSOR 14
typedef enum
{
	sms = 0x0,
	uart,//derectly uart
	scrn,//schedule screen
}sms_or_uart;
extern oa_bool oa_uart_init(oa_uart_enum port);
extern void oa_app_uart();
#endif