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
#include "oa_jt808.h"
#include "oa_fuel_sensor.h"
extern oa_uint8 KEYWORDS_SIZE;
extern DEVICE_PARAMS dev_now_params;
extern fuel_sensor_struct fuel_sensor_var;
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
	//DEBUG("recv gps data, len=%d", len);
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
		//DEBUG("%s", pBuf);
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
		if (dev_now_params.para1[7] == UART_SCREEN) {
			App_TaskSScrnRcvManage(NULL);//schedule screen
		} else if (dev_now_params.para1[7] == UART_FUEL_SENSOR) {
			oa_app_uart();
		}
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
*Function:      oa_uart_init()
*Description:  initial uarts  
*Return:        oa_bool
*Others:         
*********************************************************/
oa_bool oa_uart_reset(oa_uart_enum port, oa_uart_baudrate baud)
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
		if (oa_uart_close(OA_UART3)) {
			DEBUG("close uart ok!");
		}
		
		g_uart3_port_setting.baud = baud;
	
		if (oa_uart_open(OA_UART3, &g_uart3_port_setting))
		{
        	//oa_uart_register_callback(OA_UART3, oa_app_uart3_recv);
			DEBUG("reset uart ok!");
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
	u8 i;
	u16 real_len;
	u8 check = 0;
	//para check
	if (uart_contain.len == 0) {
		DEBUG("data len err!");
		return;
	}
	//debug
	{
		u16 i;
		DEBUG("revlen:%d data:", uart_contain.len);
		for (i=0; i<uart_contain.len; i++){
			debug_no_n("%02x ", uart_contain.buf[i]);
		}
		DEBUG();
				
	}

	if (uart_contain.buf[0] == 0x7e && uart_contain.buf[uart_contain.len - 1]  == 0x7e) {
		;
	}	else {
		DEBUG("data format err!");
		return;
	}
	//analysis data
	if (JT808_dataChg(0, &uart_contain.buf[1], uart_contain.len - 2, &real_len)) {
		DEBUG("analysis data err!");
		return;
	}
	DEBUG("reallen:%d", real_len);
	//check data
	for (i = 4; i < real_len + 1; i++) {
		check += uart_contain.buf[i];
	}

	DEBUG("check:%02x", check);
	if (check != uart_contain.buf[1]) {
		DEBUG("check data err!");
		return;
	}

	if (Manufacturer_No != uart_contain.buf[4] || Manufacturer_No != uart_contain.buf[5]) {
		DEBUG("manufacturer No. err!");
		return;
	}

	if (Peripheral_Type_No != uart_contain.buf[6]) {
		DEBUG("peripheral type No. err!");
		return;
	}

	if (Fuel_Cmd_Upload == uart_contain.buf[7]) {
		u16 per;
		u16 vol;
		u16 ad_val;
		//status
		if (Fuel_Not_Support != uart_contain.buf[8]) {
			if (Fuel_Status_Normal == uart_contain.buf[8] ||
				 Fuel_Status_Sud_Down == uart_contain.buf[8] ||
				 Fuel_Status_Sud_Up == uart_contain.buf[8]) {
				fuel_sensor_var.fuel_status = uart_contain.buf[8];
				if (Fuel_Status_Sud_Down == uart_contain.buf[8] /*||
					 Fuel_Status_Sud_Up == uart_contain.buf[8]*/) {
					WriteAlarmPara(SET, StaAlarm0, ALARM_OIL_ERR);
				} else {
					WriteAlarmPara(RESET, StaAlarm0, ALARM_OIL_ERR);
				}
			} else {
				fuel_sensor_var.fuel_status = Fuel_Status_Err;
				DEBUG("fuel status err!");
				return;
			}
		}

		//percent
		char_to_short(&uart_contain.buf[9], &per);
		if (Fuel_Not_Support2 != per) {
			fuel_sensor_var.fuel_percent = per;
		}

		//fuel volume
		char_to_short(&uart_contain.buf[11], &vol);
		if (Fuel_Not_Support2 != vol) {
			fuel_sensor_var.fuel_volume = vol;
		}

		//ad value
		char_to_short(&uart_contain.buf[13], &ad_val);
		if (Fuel_Not_Support2 != ad_val) {
			fuel_sensor_var.fuel_volume = ad_val;
		}
	} 
	else if (Fuel_Cmd_Para_Set == uart_contain.buf[7]) {
		for (i = 8; i < real_len; i+=2) {
			if (Fuel_Per_Alarm_Shreshold == uart_contain.buf[i]) {
				if (Fuel_Para_Set_OK == uart_contain.buf[i + 1]) {
					DEBUG("Fuel_Per_Alarm_Shreshold set ok!");
				}
			} else if (Fuel_Vol_Alarm_Shreshold == uart_contain.buf[i]) {
				if (Fuel_Para_Set_OK == uart_contain.buf[i + 1]) {
					DEBUG("Fuel_Vol_Alarm_Shreshold set ok!");
				}
			}	else if (Fuel_Per == uart_contain.buf[i]) {
				if (Fuel_Para_Set_OK == uart_contain.buf[i + 1]) {
					DEBUG("Fuel_Per set ok!");
				}
			} else if (Fuel_Vol == uart_contain.buf[i]) {
				if (Fuel_Para_Set_OK == uart_contain.buf[i + 1]) {
					DEBUG("Fuel_Vol set ok!");
					oa_timer_stop(OA_TIMER_ID_14);
				}
			} else {
				
			}
		}
	} 
	else {
		DEBUG("fuel cmd err!");
		return;
	}
	
}
