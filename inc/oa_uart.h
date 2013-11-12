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
#define PROTOCOL_SCREEN_HEAD 0x24
#define PROTOCOL_JT808_HEAD 0x7e
#define UART3_MAX_SIZE 256
typedef struct
{
	oa_char buf[UART3_MAX_SIZE];
	oa_uint8 len;
}uart_struct;
typedef enum
{
	sms = 0x0,
	uart,//derectly uart
	scrn,//schedule screen
}sms_or_uart;
extern oa_bool oa_uart_init(oa_uart_enum port);
extern void oa_app_uart();
#endif