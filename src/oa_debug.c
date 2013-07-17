/*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  MobileTech(Shanghai) Co., Ltd.
 *
 *****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   oa_debug.h
 *
 * Project:
 * --------
 *   OPEN AT project
 *
 * Description:
 * ------------
 *   This Module defines OPEN AT's basic types.
 *
 * Author:
 * -------
 *   zhuqing.
 *
 ****************************************************************************/
#include <stdio.h>  
#include <stdarg.h>
#include "oa_type.h"
#include "oa_api.h"
#define OA_DEBUG_BUF_MAXLEN 256

void debug_no_n(oa_char *fmt, ...)
{
	oa_char tmp_buffer[256] = {0};
	u8 cnt;
	va_list list;
	va_start(list, fmt);
	cnt = _vsnprintf(tmp_buffer, OA_DEBUG_BUF_MAXLEN, fmt, list);
	va_end(list);
	oa_uart_write(OA_UART1,(oa_uint8 *)tmp_buffer, cnt);
}
#if 0
int debug_no_n(const char *fmt, ...)
{
	va_list args;
	int i;
	oa_char printbuf[256] = {0};
	va_start(args, fmt);
	oa_uart_write(OA_UART1, printbuf, i=vsprintf(printbuf, fmt, args));
	va_end(args);
	return i;
}
#endif