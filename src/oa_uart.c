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
*   oa_uart.c
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
#include "oa_type.h"
#include "oa_api.h"
#include "oa_uart.h"
#include "oa_gps.h"
#include "oa_sms.h"
#include "oa_debug.h"

extern oa_uint8 KEYWORDS_SIZE;
/*****************************************************************/
/*-----------------For uart port configure----------------------------*/
/*****************************************************************/  
oa_uart_struct g_uart_port_setting = 
{
	115200,//9600,   /* baudrate */
	oa_uart_len_8,              /* dataBits; */
	oa_uart_sb_1,               /* stopBits; */
	oa_uart_pa_none,            /* parity; */
	oa_uart_fc_none,            /* flow control */
	0x11,               /* xonChar; */
	0x13,               /* xoffChar; */
	OA_FALSE
};

oa_uart_struct g_uart2_port_setting = 
{
	9600,//9600,   /* baudrate */
	oa_uart_len_8,              /* dataBits; */
	oa_uart_sb_1,               /* stopBits; */
	oa_uart_pa_none,            /* parity; */
	oa_uart_fc_none,            /* flow control */
	0x11,               /* xonChar; */
	0x13,               /* xoffChar; */
	OA_FALSE
};

oa_uart_struct g_uart3_port_setting = 
{
	19200,//9600,   /* baudrate */
	oa_uart_len_8,              /* dataBits; */
	oa_uart_sb_1,               /* stopBits; */
	oa_uart_pa_none,            /* parity; */
	oa_uart_fc_none,            /* flow control */
	0x11,               /* xonChar; */
	0x13,               /* xoffChar; */
	OA_FALSE
};
void oa_app_uart();
//oa_char uart_buffer[OA_UART_BUFFER_SIZE] = {0};
uart_struct uart_contain;

gps_data_struct gps_data = {
	{0x0},
	0,
	OA_FALSE,
};
/*****************************************************************
*------oa_app_uart1_recv  
*          need oa_uart_open() and oa_uart_register_callback() firstly,
*          len max is OA_UART_BUFFER_SIZE, 1024
*****************************************************************/
void oa_app_uart1_recv( void * param, oa_uint32 len)
{
	char * pBuf = (char *)param;
	//DEBUG("%s:len=%d,pBuf=%s", __FILE__, __func__,len, pBuf );
}

/*****************************************************************
*------oa_app_uart2_recv
*          need oa_uart_open() and oa_uart_register_callback() firstly, 
*          len max is OA_UART_BUFFER_SIZE, 1024
*****************************************************************/
void oa_app_uart2_recv( void * param, oa_uint32 len)
{
	oa_uint8 * pBuf = (oa_uint8 *)param;
	//DEBUG("%s:%d->%s", __FILE__, __func__, len, (oa_char*)pBuf);
	//oa_write_buffer_force_noinit(g_soc_context.gprs_tx, pBuf, len);
	//handle gps data
	//......
	//DEBUG("(%s:%s:%d):len=%d", __FILE__, __func__, __LINE__, len);
	//DEBUG("%s\n", (oa_char *)pBuf);
	if (len >= OA_UART_BUFFER_SIZE)
	{
		//DEBUG("(%s:%s:%d):gps data is too long", __FILE__, __func__, __LINE__);
		oa_memset(gps_data.buffer, 0x0, OA_UART_BUFFER_SIZE);
		oa_memcpy(gps_data.buffer, pBuf, OA_UART_BUFFER_SIZE);
		gps_data.data_len = OA_UART_BUFFER_SIZE;
		gps_data.has_new_data = OA_TRUE;
		return;
	}
	else
	{
		oa_memset(gps_data.buffer, 0x0, OA_UART_BUFFER_SIZE);
		oa_memcpy(gps_data.buffer, pBuf, len);
		gps_data.data_len = len;
		gps_data.has_new_data = OA_TRUE;
		//DEBUG("%s", gps_data.buffer);
		//oa_app_gps();
	}
#if 0
	DEBUG("%s",  (oa_char *)pBuf);
	DEBUG("len : %d", len);
#endif
}

/*****************************************************************
*------oa_app_uart3_recv
*          need oa_uart_open() and oa_uart_register_callback() firstly,  
*          len max is OA_UART_BUFFER_SIZE, 1024
*****************************************************************/
void oa_app_uart3_recv( void * param, oa_uint32 len)
{
	char * pBuf = (char *)param;
	
	if (len > UART3_MAX_SIZE)
	{
		DEBUG("uart data is too long");
		return;
	}
	else
	{
		oa_memset(uart_contain.buf, 0x0, UART3_MAX_SIZE);
		oa_memcpy(uart_contain.buf, pBuf, len);
		uart_contain.len = len;
		#ifdef SCHE_SCRN
		App_TaskSScrnRcvManage(NULL);//schedule screen
		#else
		oa_app_uart();
		#endif
	}
}

/*********************************************************
*Function:      oa_uart_init()
*Description:  initial uarts  
*Return:        oa_bool
*Others:         
*********************************************************/
oa_bool oa_uart_init(oa_uart_enum port)
{
	if (OA_UART1 == port)
	{
		if (oa_uart_open(OA_UART1, &g_uart_port_setting))
		{
        		oa_uart_register_callback(OA_UART1, oa_app_uart1_recv);
			return OA_TRUE;
		}
		else
		{
			return OA_FALSE;
		}
	}
	else if (OA_UART2 == port)
	{
		if (oa_uart_open(OA_UART2, &g_uart2_port_setting))
		{
        		oa_uart_register_callback(OA_UART2, oa_app_uart2_recv);
			return OA_TRUE;
		}
		else
		{
			return OA_FALSE;
		}
	}
	else if (OA_UART3 == port)
	{
		if (oa_uart_open(OA_UART3, &g_uart3_port_setting))// 115200
		{
        		oa_uart_register_callback(OA_UART3, oa_app_uart3_recv);
			return OA_TRUE;
		}
		else
		{
			return OA_FALSE;
		}
	}
	else
	{
		return OA_FALSE;
	}
}

/*********************************************************
*Function:      oa_app_uart()
*Description:  handle uart's feedback
*Return:        void
*Others:         
*********************************************************/
void oa_app_uart(void)
{
#if 0
	//analysis the uart data
	//do about sms
	e_keyword key_ret = e_none;
	keyword_context set = {0x0};
	oa_uint8 e_i;
	
	//do not support multiple sms
	//Trace("(%s:%s:%d):KEYWORDS_SIZE:%d", __FILE__, __func__, __LINE__,KEYWORDS_SIZE);
	for (e_i = 0;e_i < KEYWORDS_SIZE;e_i++){
		key_ret = lookfor_keywords_loop(NULL, 0, &set, e_i, uart);
		if (e_none == key_ret){
			//Trace("(%s:%s:%d): not support!", __FILE__, __func__, __LINE__);
			continue;
		}
		handle_keyword(NULL, NULL, NULL, key_ret, &set, uart);
		oa_memset(&set, 0x0, sizeof(set));
	}
	return;
#endif
}
