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
 *   oa_callback_func_reg.c
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
#include "oa_at.h"
#include "oa_sms.h"
#include "oa_debug.h"

/*--------BEG: Customer code----------*/
extern oa_sms_context message;
 extern oa_uint8 at_fedbak_buf[AT_FEDBAK_MAX_LEN];
/*--------END: Customer code----------*/
/*****************************************************************/
/*-----------------End for customer define--------------------------*/
/*****************************************************************/
#ifdef CUSTOM_AT
/*****************************************************************/
/* Custom AT command implementation
/* FORMAT: 
          [AT+OPENAT=xxx]
/* EXAMPLE:
     input AT command string: [AT+OPENAT=GPRS:0,www.3322.org,2011#]
     string [GPRS:0,www.3322.org,2011#] will be received in this function
/*****************************************************************/
oa_bool oa_app_execute_custom_at_cmd(oa_char* pStr, oa_uint16 len)
{
    DEBUG("len=%d@@%s", len, pStr);
    /*Example: [AT+OPENAT=GPRS:0,www.3322.org,0.0.0.0,2011#]
    * if use domain, set ip:0.0.0.0, or system will use ip for primitive.
    */
    if(!oa_strncmp(pStr, "GPRS:", 5))
    {
        oa_char *hh=pStr+5, *tt=pStr+6;
        oa_char curIP[20]={0}, curDoma[64]={0}, curPort[8]={0};
        oa_uint16 curType=0, curLen;

        if(*tt == ',')
            curType = oa_atoi(hh);

        hh = oa_strchr(++tt, ',');
        if(!hh)
            return OA_FALSE;
        curLen = (oa_uint16)(hh-tt);
        oa_memcpy(curDoma, tt, curLen);

        tt = oa_strchr(++hh, ',');
        if(!tt)
            return OA_FALSE;
        curLen = (oa_uint16)(tt-hh);
        oa_memcpy(curIP, hh, curLen);

        hh = oa_strchr(++tt, '#');
        if(!hh)
            return OA_FALSE;
        curLen = (oa_uint16)(hh-tt);
        oa_memcpy(curPort, tt, curLen);

        oa_strcpy(g_soc_param.serve_ipaddr, curIP);
        oa_strcpy(g_soc_param.serve_host_name, curDoma);
        g_soc_param.port = oa_atoi(curPort);
        g_soc_param.connct_type = curType;

        oa_soc_setting_save();
        DEBUG("GPRS parameter set: %d, %s, %s, %d", 
                        g_soc_param.connct_type, g_soc_param.serve_host_name,
                        g_soc_param.serve_ipaddr, g_soc_param.port);

        return OA_TRUE;
    }

    return OA_FALSE;
}
#endif

#ifdef TONE_PLAY
// on tone playing callback
oa_bool oa_app_on_tone_play_req(oa_uint16 playtone)
{
	DEBUG("%d", playtone);

	return OA_TRUE;

	switch (playtone)
	{
		case OA_MESSAGE_TONE:   // do not allow message tone
		  return OA_FALSE;

		case OA_SMS_IN_CALL_TONE:  // do not allow message tone during call
		  return OA_FALSE;

		case OA_INCOMING_CALL_TONE: // do not allow incoming call ringing
		  return OA_FALSE;
		  
		default:
		  return OA_TRUE; // allow other tones playing
	}
}
#endif


/*****************************************************************
*         AT cmd response received, after oa_uart_send_at_cmd_req
*****************************************************************/
void oa_app_at_rsp_recv(oa_uint16 len, oa_uint8 *pStr)
{
	
#ifdef AT_RECV_PRINT
	DEBUG("[--------------------");
    	DEBUG("%s", pStr);
	DEBUG("--------------------]");
#endif
	//handle AT feedback
	oa_memset(at_fedbak_buf, 0x0, sizeof(at_fedbak_buf));
	if (len < AT_FEDBAK_MAX_LEN && NULL != pStr)
	{
		oa_memcpy(at_fedbak_buf, pStr, len);
		oa_app_at();
	}
	else
	{
		DEBUG("ERR!");
	}
}

void oa_app_power_shutdown(void* param)
{
	DEBUG("power down! good bye!");
	//oa_soc_close_req();
	oa_power_shutdown(NULL);
}


/*****************************************************************/
 /*------------------sms received indication handler refer framework
 PARAM:
 deliver_num: SMS MO address
 timestamp: sms timestamp
                  timestamp[0]= nYear;
                  timestamp[1]= nMonth;
                  timestamp[2]= nDay;
                  timestamp[3]= nHour;
                  timestamp[4]= nMin;
                  timestamp[5]= nSec;

 data: sms text data
 len: sms data length,
        if dcs=7-bit or 8-bit, is the bytes length,
        if dcs=UCS2,is the unicode length
 dcs: refer oa_smsal_dcs_enum
 RETURN:
   OA_TRUE: the sms be handled by openat user, and will be be saved
   OA_FALSE:the sms is not openat user cared
******************************************************************/
oa_bool oa_sms_rcv_ind_handler(oa_char * deliver_num, oa_char * timestamp, oa_uint8 * data, oa_uint16 len, oa_smsal_dcs_enum dcs)
{

#ifdef DBG_SMS
	DEBUG("SMS received:deliver_num=%s,timestamp=%d-%d-%d-%d:%d:%d,dcs=%d,len=%d,data=%s",\
	deliver_num,*(timestamp+0),*(timestamp+1),*(timestamp+2),*(timestamp+3),*(timestamp+4),*(timestamp+5),dcs,len,data);
#endif
	oa_memset(&message, 0x0, sizeof(message));//reset message
	if(dcs == OA_SMSAL_DEFAULT_DCS){
		/*handle ascii sms text.*/
		//DEBUG("SMS TXT: %s", data);
		
	}
	else if(dcs == OA_SMSAL_UCS2_DCS){
		/*handle unicode sms text.*/
		/*chinese sms*/
	#if 0
		oa_uint16 i;
		oa_uint8 temp;
		len*=2;
		if (NULL != data && len <= SMS_DATA_MAX_LEN){
			for(i=0; i<len-1; i+=2){//exchange high byte & low byte
				temp = data[i];
				data[i] = data[i+1];
				data[i+1] = temp;
			}
		}
	#endif
		Trace("unicode not support!");
		return OA_TRUE; //do not handle & delete it
	}
	else{
		/*handle 8-bit sms text.*/
		//彩信
		DEBUG("8-bit sms text!!!");
		return OA_TRUE; //do not handle & delete it
	}
	//copy message 
	message.dcs = dcs;
	if (NULL != deliver_num && NULL != data){
		if (oa_is_phone_addr_str(deliver_num, oa_strlen(deliver_num))
			&& oa_strlen(deliver_num) <= SMS_NUM_MAX_LEN){//check number valid
			oa_strcpy(message.deliver_num, deliver_num);
		}
		else{
			DEBUG("ERR : deliver number");
			return OA_TRUE;//do not handle & delete it
		}
		

		if (len <= SMS_DATA_MAX_LEN){//check len
			oa_memcpy(message.data, data, len);
			message.len = len;
		}
		else{
			DEBUG("ERR : len > SMS_DATA_MAX_LEN");
			return OA_TRUE;//do not handle & delete it
		}
		
		//oa_evshed_start(OA_EVSHED_ID_0, oa_app_sms, (void *)&message, SMS_HANDLE_DELAY);
		oa_app_sms();//handle the sms
	}
	else{
		DEBUG("ERR!");
	}
	return OA_TRUE;/*delete current SMS from memory*/
	//return OA_FALSE;/*do not delete this SMS, user can handle it*/
	/*Warning: if user return OA_FALSE to save current sms in memory, 
	he should pay attention to the memory capacity. 
	Please use at+cpms to confirm current memory capacity and, when the memory is full, 
	user will receive AT command response "+OPENAT: SMS FULL" from PS port 
	and system will clean all the SMS in order to receive more SMS.*/
}

 void callback_func_reg(void)
 {
	/*AT command callback*/
	oa_atf_rsp_callback_register(oa_app_at_rsp_recv);

	//执行特殊定制的AT指令
#ifdef CUSTOM_AT
	oa_cust_cmd_register(oa_app_execute_custom_at_cmd);
#endif

	oa_key_register(OA_KEY_POWER, OA_KEY_LONGPRESS, oa_app_power_shutdown);   

#ifdef SMS_CALLBACK
	//register sms receive callback function
	oa_sms_rcv_ind_register(oa_sms_rcv_ind_handler);
#endif
 }
