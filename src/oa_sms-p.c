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
 *   oa_sms.c
 *
 * Project:
 * --------
 *   OPEN AT project
 *
 * Description:
 * ------------
 *   This Module gives apis for sms operation.
 *
 * Author:
 * -------
 *   Castle!
 *
 ****************************************************************************/
//#include "oa_global.h"
#include "oa_type.h"
#include "oa_api.h"
#include "oa_llist.h"
#include "oa_at-p.h"
#include "oa_sms-p.h"
#include "oa_debug.h"
//#include "oa_protocol.h"

#define SMS_DEBUG 2
/*oa_trace_on_uart OR oa_trace_on_at*/
#if (SMS_DEBUG == 2)
//#define SMS_DEBUG_L2(...)
#define SMS_DEBUG_L2 //DEBUG
#define SMS_DEBUG_L1 //DEBUG//OA_DEBUG_USER
#elif(SMS_DEBUG == 1)
#define SMS_DEBUG_L2(...)
#define SMS_DEBUG_L1(...)
#else
#define SMS_DEBUG_L2(...)
#define SMS_DEBUG_L1(...)
#endif


/*sms context pend list */
llist_head g_at_sms_send_pend_list;
llist_head g_at_sms_recv_pend_list;
oa_bool g_at_sms_pend_toggle;/*sms send-recv toogle*/
oa_int16 sms_pend_count;
oa_bool g_isLong;/**/
oa_bool g_sms_init_ok=OA_FALSE;

static oa_sms_context *g_current_sms = NULL;

/*for oa_sms_initial_sms_context*/
oa_init_sms_param_in g_sms_init_param={0};

oa_sms_pend_state g_current_pend_state = OA_SMS_PEND_IDLE;
oa_sms_state g_cur_sms_recv_state;

/*for encode & decode & str-octet convert buffer*/
oa_uint16 g_store_str_len=0;
oa_char g_buffer_store_str[512]={0};
oa_uint16 g_store_octet_len=0;
oa_uint8 g_buffer_store_octet[512]={0};

/*protocol support*/
oa_char sms_protocol_toa[20]={0};
oa_char sms_protocol_sca[20]={0};
oa_buffer *g_sms_pro_rx;
oa_uint8 sms_protocol_buf[256];

/*Default Array for 7bit encode*/
const oa_uint8 OaAsciiToDefaultArray[] = 
{
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    10, 32, 32, 13, 32, 32, 16, 32, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 32, 32,
    32, 32, 32, 33, 34, 35, 2, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 0, 65, 66, 67, 68, 69,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
    90, 32, 32, 32, 32, 17, 32, 97, 98, 99,
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 64, 32, 1, 36, 3, 32, 95, 32, 32,
    32, 32, 32, 64, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 96, 32, 32, 32, 32, 91, 14, 28, 9,
    31, 32, 32, 32, 32, 32, 32, 32, 32, 93,
    32, 32, 32, 32, 92, 32, 11, 32, 32, 32,
    94, 32, 32, 30, 127, 32, 32, 32, 123, 15,
    29, 32, 4, 5, 32, 32, 7, 32, 32, 32,
    32, 125, 8, 32, 32, 32, 124, 32, 12, 6,
    32, 32, 126, 32, 32, 32
};

/*Default Array for 7bit encode*/
const oa_uint8 OaDefaultToAsciiArray[128] = 
{
    64, 163, 36, 165, 232, 233, 249, 236, 242, 199,
    10, 216, 248, 13, 197, 229, 16, 95, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 198, 230,
    223, 200, 32, 33, 34, 35, 164, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 161, 65, 66, 67, 68, 69,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
    90, 196, 214, 209, 220, 167, 191, 97, 98, 99,
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 228, 246, 241, 252, 224
};

const oa_uint8 OaExtendedAsciiToDefaultArray[] = 
{
    0, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 27, 32, 32,
    32, 32, 10, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 60, 47, 62, 20, 32, 32, 32, 32, 32,
    32, 101, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 40, 64, 41, 61, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 101, 32, 32, 32, 32, 32, 32, 32,    /* Changed character at 162 to 101 (old value was 32) */
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32
};

//dummy cmd frame begin
#if 0
oa_int16 oa_at_dummy_queue_counter =0;
void oa_sms_timeout_dummy_callback(void *param)
{
    oa_at_cmd *cmd = (oa_at_cmd *)param;
    
    if(!cmd) return;
    oa_at_finish_cmd(cmd);
    SMS_DEBUG_L2("oa_sms_timeout_dummy_callback called! %s\r\n", cmd->buf);
}
void oa_sms_dummy_callback(void *param)
{
    oa_at_cmd *cmd = (oa_at_cmd *)param;
    oa_int32 ret;
    
    if(!cmd) return;
    
    ret = cmd->ret;
    SMS_DEBUG_L2("oa_sms_dummy_callback called!\r\n");    

    /*addition process: we should wake up 
     * oa_wait_on_msg for MSG_ID_OPENAT_INIT_FINISHED_IND.
     * and confirm that the cmd should be the last at-cmd of initialization.
     */
     if(!strncmp(cmd->buf, "at+csca?\r\n", strlen("at+csca?\r\n"))){
         g_sms_init_ok = OA_TRUE;
         //oa_timer_stop(OA_TIMER_ONLY_FOR_RESTART);
         SMS_DEBUG_L2("Openat initialization complete");
     }
     if(oa_at_dummy_queue_counter>0)
        oa_at_dummy_queue_counter--;
}

//just pend cmd
oa_at_errno oa_sms_cmd_dummy(oa_char *cmdStr, ...)
{
    oa_at_cmd *cmd = NULL;
    oa_char cmdString[64]= {0};
    va_list list;
    
    if(oa_at_dummy_queue_counter>16){
        SMS_DEBUG_L2("oa_at_dummy_queue_counter too many! %d", oa_at_dummy_queue_counter);
        return OA_AT_ERR_NONE;
    }

    va_start(list, cmdStr);
    _vsnprintf(cmdString, 64, cmdStr, list);
    va_end(list);

    cmd = oa_at_create_cmd(cmdString);
    if(!cmd)
        goto MFL_fail;
    cmd->timeout = 20000;
    cmd->timeout_cb = oa_sms_timeout_dummy_callback;
    cmd->cb = oa_sms_dummy_callback;
    oa_at_pend_cmd(cmd);
    oa_at_dummy_queue_counter++;

    return OA_AT_ERR_NONE;

    MFL_fail:
        SMS_DEBUG_L2("oa_sms_cmd_dummy fail!\r\n");
        return OA_AT_ERR_BUSY;
}
//submition
oa_at_errno oa_sms_cmd_submit()
{
    oa_at_errno err;
    
    err = oa_at_submit();
    if(err != OA_AT_ERR_NONE)
        goto MFL_fail;

    return OA_AT_ERR_NONE;
    
    MFL_fail:
        SMS_DEBUG_L2("oa_sms_cmd_submit fail!\r\n");
        return OA_AT_ERR_BUSY;
}
//dummy cmd frame end
#endif

oa_sms_context *oa_sms_create_sms()
{
    oa_sms_context *buf;
    
    buf = (oa_sms_context *)oa_malloc(sizeof(oa_sms_context));
    return buf;
}
void oa_sms_delete_sms(oa_sms_context *sms)
{
	if(OA_SMS_TYPE_PDU == sms->sms_type)
	{
    	oa_free(sms->current_pdu.TP_UD);
    	oa_free(sms);
	}
}



/*initialize the sms context
* param: UD shuold be the string data to be send directly.
* so we shuold encode or convert to string outside.
* param: UDL shuold be the string length before 
* encoding or converting (string length user want to send)
*/
void oa_sms_initial_sms_context(oa_init_sms_param_in *initParam)
{
    oa_sms_context *p = initParam->sms;
    oa_char *SCA_num = sms_protocol_sca;
    oa_int32 len = 0;
    oa_char buff[16]={0};
    oa_pdu_type pdu_type={0};
    
    if(initParam->sms == NULL) return;/*better to assert*/

    oa_memset(&(initParam->sms->current_pdu), 0, sizeof(oa_sms_pdu_send));
#ifdef OA_SMS_TEXT 
    oa_memset(&(initParam->sms->current_text), 0, sizeof(oa_sms_text_send));
#endif
    /*we can begin from the middle state*/
    p->sms_state = OA_SMS_SET_FORMAT;
    //p->sms_type = OA_SMS_TYPE_PDU;

    /* these are test initializations
     *  i don't know how to provid so many interfaces for these 
     *  so many parameters such as TP-PID & TP-DCS & TP-VP
     */
    if(p->sms_type==OA_SMS_TYPE_PDU)
    {
        oa_sms_pdu_send *p_pdu = &(p->current_pdu);
        
        //SCA
        if(oa_strlen(SCA_num) != 0)
        {
            if(SCA_num[0] ==  '+'){
                oa_strncpy(p_pdu->SCA_type, "91", oa_strlen("91"));
                SCA_num++;
            }
            else{
                oa_strncpy(p_pdu->SCA_type, "00", oa_strlen("00"));
            }
            len = oa_strlen(SCA_num);
            oa_strncpy(p_pdu->SCA_value, SCA_num, len);
            len = oa_sms_pdu_num_reverse(p_pdu->SCA_value, 'F');
            len/=2;
            oa_sprintf(buff, "%02X", len+1);
            oa_strncpy(p_pdu->SCA_length, buff, 2);
            p_pdu->SCA_size = len+2;
        }
        else
        {
            p_pdu->SCA_size = 0;
        }
        //PDU type & MR (we shuold calculate pdu_length from here!)
        //TP-RP TP-UDHI TP-SRR TP-VPF TP-RD TP-MTI
        //   0          0           0          00         0         00
        pdu_type.TP_MTI = 0x01;/*submit*/
        pdu_type.TP_VPF = 0x02;/**/
        if(initParam->isLongMsg==OA_TRUE){
            //SMS_DEBUG_L2("init long sms %d", isLongMsg);
            pdu_type.TP_UDHI = 0x01;
        }
        oa_sprintf(p_pdu->PDU_TYPE, "%02X", *(oa_uint8 *)&pdu_type);
        oa_strncpy(p_pdu->MR, "00", sizeof("00"));
        
        //DA
        if(initParam->TPOA_num[0] == '+'){
            oa_strncpy(p_pdu->TPDA_type, "91", oa_strlen("91"));
            initParam->TPOA_num++;
        }
        else{
            oa_strncpy(p_pdu->TPDA_type, "00", oa_strlen("00"));
        }
        len = oa_strlen(initParam->TPOA_num);
        oa_strncpy(p_pdu->TPDA_value, initParam->TPOA_num, len);
        oa_sprintf(buff, "%02X", len);
        oa_strncpy(p_pdu->TPDA_length, buff, 2);
        len = oa_sms_pdu_num_reverse(p_pdu->TPDA_value, 'F');
        len/=2;
        p_pdu->TPDA_size = len+2;
        
        //TP-PID
        oa_strncpy(p_pdu->TP_PID, "00", oa_strlen("00"));

        //TP-DCS
        oa_sprintf(p_pdu->TP_DCS, "%02X", initParam->inDCS);
        //strncpy(p_pdu->TP_DCS, "00", strlen("00"));

        //TP-VP
        oa_strncpy(p_pdu->TP_VP, "00", oa_strlen("00"));

        //TP UD
        //len = strlen(UD);
        len = initParam->UDL;
        if(((initParam->inDCS&OA_SMS_DCS_8BIT)==OA_SMS_DCS_8BIT)
                ||((initParam->inDCS&OA_SMS_DCS_USC2)==OA_SMS_DCS_USC2)){
            oa_sprintf(buff, "%02X", len);
            oa_strncpy(p_pdu->TP_UDL, buff, 2);
            
            p_pdu->TP_UD = (oa_char *)oa_malloc(oa_strlen(initParam->UD)+1);/*free by system*/
            oa_memset(p_pdu->TP_UD, 0x00, oa_strlen(initParam->UD)+1);
            oa_memcpy(p_pdu->TP_UD, initParam->UD, oa_strlen(initParam->UD));

            p_pdu->UD_size = oa_strlen(initParam->UD)/2+1;
        }
        else if((initParam->inDCS&OA_SMS_DCS_DFALP)==OA_SMS_DCS_DFALP){
            oa_sprintf(buff, "%02X", len);
            oa_strncpy(p_pdu->TP_UDL, buff, 2);
            
            p_pdu->TP_UD = (oa_char *)oa_malloc(oa_strlen(initParam->UD)+1);/*free by system*/
            oa_memset(p_pdu->TP_UD, 0x00, oa_strlen(initParam->UD)+1);
            oa_memcpy(p_pdu->TP_UD, initParam->UD, oa_strlen(initParam->UD));
            //strncpy(p_pdu->TP_UD, UD, len*2);
            
            p_pdu->UD_size = oa_strlen(initParam->UD)/2+1;
        }
        else{
            //error
            SMS_DEBUG_L2("[OA]SMS: initial sms inDCS error! %02X", initParam->inDCS);
        }
        /*pdu_length for at+cmgs=?  5 = PDU_type+MR+TP_PID+TP_DCS+TP_VP*/
        p_pdu->pdu_length = p_pdu ->TPDA_size + p_pdu->UD_size + 5;
    }
    else if(p->sms_type == OA_SMS_TYPE_TEXT)
    {
#ifdef OA_SMS_TEXT 
        oa_sms_text_send *p_text = &(p->current_text);
        oa_strncpy(p_text->data_number, initParam->TPOA_num, oa_strlen(initParam->TPOA_num));
        oa_strncpy(p_text->data_text, initParam->UD, oa_strlen(initParam->UD));
#endif
    }
    //else
    {
        //error
    }

    //SMS_DEBUG_L2("[OA]SMS: sms initialed OK!");
    return;
}

//pend sms to the sms-list, only mount it, no duplication
oa_sms_err_ret oa_sms_pend_sms(oa_sms_context *sms)
{
    oa_at_errno ret = OA_SMS_RET_OK;
    llist_head *head = &g_at_sms_send_pend_list;

    sms->index = sms_pend_count++;
    //mount the sms
    llist_add_tail((llist_head *)sms, head);

    SMS_DEBUG_L2("[OA]SMS: sms pend OK! %X", sms);
    return ret;
}

//operation on switch g_current_sms
void oa_sms_set_current_sms(oa_sms_context *current_sms)
{
    g_current_sms = current_sms;
}
oa_sms_context *oa_sms_get_current_sms(void)
{
    return g_current_sms;
}
oa_bool oa_sms_current_sms_empty(void)
{
    if(g_current_sms)
        return OA_FALSE;
    else
        return OA_TRUE;
}

/*initialization before any sms operations*/
void oa_sms_initial(void)
{
    /*shuold be coverd be at-response*/
    //strcpy(sms_protocol_toa, "+8613122135713");
    //strcpy(sms_protocol_sca, "+8613800210500");

    /*sms send-recv pend init*/
    INIT_LLIST_HEAD(&g_at_sms_send_pend_list);
    INIT_LLIST_HEAD(&g_at_sms_recv_pend_list);
    sms_pend_count = 0;

    /*buffer for protocol*/
    g_sms_pro_rx = oa_create_param_buffer(sms_protocol_buf, sizeof(sms_protocol_buf));
    oa_memset(g_sms_pro_rx->buf, 0x00, 256);

    /*toggle for sms handler*/
    g_at_sms_pend_toggle = OA_TRUE;
    
    oa_sms_set_current_sms(NULL);
    SMS_DEBUG_L2("oa_sms_initial OK!");
    return;
}

/*initialization after getting SIM card info*/
void oa_sms_initial_by_at_cmd(void)
{
    /*init setting throught at*/
    //oa_at_cmd_demo("ATE0\r");
    oa_at_cmd_demo("at+cpms=\"ME\",\"ME\",\"ME\"\r\n");
    //oa_at_cmd_demo("at+cnmi=0,1,0,0,1\r\n");
    oa_at_cmd_demo("at+cnmi=2,2,0,0,1\r\n");
    
    oa_at_cmd_demo("at+csca?\r\n");/*get SCA number*/
    /*if the last init-at-cmd respond, we start a timer to run our test func to send sms*/
    oa_at_cmd_demo_submit();
}

//handler function for scheduler
void oa_sms_handler(void)
{
	
	llist_head *sendHead = &g_at_sms_send_pend_list;
    llist_head *revcHead = &g_at_sms_recv_pend_list;
    
    SMS_DEBUG_L2("[OA]SCH: goto oa_sms_handler.");
    SMS_DEBUG_L2("[OA]SMS: check list %d %d %d \n", oa_at_busy(), llist_empty(sendHead), g_current_pend_state);

    if(oa_at_busy() || oa_is_in_call()) return;

    //shuold we receive sms before send sms?
    if(OA_TOGGLE_IT(g_at_sms_pend_toggle)){
        if(!llist_empty(revcHead)){
            /*receive one sms and protocol it and...*/
            SMS_DEBUG_L2("[OA]SMS: Let's go to sms-recv machine! %d\r\n", ((oa_sms_recv_index *)revcHead->next)->index);
            oa_recv_sms_state_machine(NULL);
        }
    }
    else{
        /*OK! Let's go to the sms-machine to send sms*/
        if(oa_sms_current_sms_empty() && !llist_empty(sendHead))
        {
            g_current_pend_state = OA_SMS_PEND_SENDING;
            oa_sms_set_current_sms((oa_sms_context *)sendHead->next);
            SMS_DEBUG_L2("[OA]SMS: Let's go to sms-send machine! %X\r\n", (oa_sms_context *)sendHead->next);
            oa_send_sms_state_machine(NULL);
        }
    }

}

//send raw data
oa_char g_check_udp[512]={0};
void oa_sms_send_sms_raw_data(oa_sms_context *sms)
{
    oa_sms_context *p = sms;
    oa_char data[2] = {0};
    oa_uint32 offset=0;
    
    SMS_DEBUG_L2("oa_sms_send_sms_raw_data: %X", sms);

    if(sms == NULL) return;
    oa_memset(g_check_udp, 0x00, 512);

    if(p->sms_type==OA_SMS_TYPE_PDU)
    {
        oa_sms_pdu_send *p_pdu = &(p->current_pdu);
        
        //we send sms raw data together(or by sections)
    SMS_DEBUG_L2("oa_sms_send_sms_raw_data 0:p_pdu->SCA_size=%d", p_pdu->SCA_size);
        
        if(p_pdu->SCA_size != 0){
            oa_at_send_raw_data((oa_uint8 *)p_pdu->SCA_length, (p_pdu->SCA_size)*2);
            //offset += snprintf(g_check_udp+offset, (p_pdu->SCA_size)*2, "%s", p_pdu->SCA_length);
        }
        else{
            oa_at_send_raw_data((oa_uint8 *)"00", oa_strlen("00"));
            //offset += snprintf(g_check_udp+offset, strlen("00"), "%s", "00");
        }

    SMS_DEBUG_L2("oa_sms_send_sms_raw_data 1", sms);
        
        oa_at_send_raw_data((oa_uint8 *)p_pdu->PDU_TYPE, (p_pdu->TPDA_size+2)*2);
        //offset += snprintf(g_check_udp+offset, (p_pdu->TPDA_size+2)*2, "%s", p_pdu->PDU_TYPE);
    SMS_DEBUG_L2("oa_sms_send_sms_raw_data 2", sms);
        
        oa_at_send_raw_data((oa_uint8 *)p_pdu->TP_PID, (3+1)*2);// 1 for UDL
        //offset += snprintf(g_check_udp+offset, (3+1)*2, "%s", p_pdu->TP_PID);
    SMS_DEBUG_L2("oa_sms_send_sms_raw_data 3", sms);
        
        oa_at_send_raw_data((oa_uint8 *)p_pdu->TP_UD, p_pdu->UD_size*2);
        //offset += snprintf(g_check_udp+offset, p_pdu->UD_size*2, "%s", p_pdu->TP_UD);

    SMS_DEBUG_L2("oa_sms_send_sms_raw_data 4", sms);

        data[0] = 0x1A;
        oa_at_send_raw_data((oa_uint8 *)data, 1);
        
        SMS_DEBUG_L2("[OA]SMS: sms pdu raw data sended OK! %d, %d, %d, %d", 
                                   p_pdu->SCA_size, p_pdu->TPDA_size, p_pdu->UD_size, p_pdu->pdu_length);
                                   
        SMS_DEBUG_L2("oa_sms_send_sms_raw_data: %d, %s", p->current_pdu.pdu_length, g_check_udp);
        SMS_DEBUG_L2("oa_sms_send_sms_raw_data->%d, %d, %d", (p_pdu->SCA_size)*2, (p_pdu->TPDA_size+2)*2, p_pdu->UD_size*2);
        //SMS_DEBUG_L2("oa_sms_send_sms_raw_data-> %d, %s", p->current_pdu.pdu_length, look_pdu_string);
    }
    else if(p->sms_type == OA_SMS_TYPE_TEXT)
    {
#ifdef OA_SMS_TEXT 
        oa_sms_text_send *p_text = &(p->current_text);
        oa_at_send_raw_data((oa_uint8 *)p_text->data_text, oa_strlen(p_text->data_text));
        data[0] = 0x1A;
        oa_at_send_raw_data((oa_uint8 *)data, 1);
#endif        
        //SMS_DEBUG_L2("[OA]SMS: sms text raw data sended OK!");
    }
    else
    {
        //error
    }
}

void __oa_sms_send_raw_data_prompt_tx(void *dummy)
{
    SMS_DEBUG_L2("[OA]SMS: called __oa_sms_send_raw_data_prompt_tx");
    /*get current sms context and send it*/
    oa_sms_send_sms_raw_data(oa_sms_get_current_sms());
    
    //oa_sleep(3000);
    //oa_trace_on_file("__oa_sms_send_prompt_tx called!  %d\r\n", g_sms_state);
}

/*****************************************************************************
* state machine depends on sms type
* for text mode:
*   OA_SMS_SET_RECV_MODE-->OA_SMS_SET_FORMAT-->OA_SMS_SET_CHAR_SET
*   -->OA_SMS_SEND_TEXT-->OA_SMS_SEND_FINISH-->[OA_SMS_SEND_NONE]?
* 
* for pdu mode:
*   OA_SMS_SET_RECV_MODE-->OA_SMS_SET_FORMAT-->OA_SMS_SEND_PDU_DATA
*   -->OA_SMS_SEND_FINISH-->[OA_SMS_SEND_NONE]?
*****************************************************************************/
void oa_send_sms_state_machine(void *param)
{
    oa_at_cmd *cmd = NULL;
    oa_at_errno err;
    oa_char cmdString[30]= {0};
    llist_head *head = &g_at_sms_send_pend_list;
    oa_sms_context *current_sms = oa_sms_get_current_sms();

    //OA_DEBUG_USER("[OA]SMS: Entry sms machine: %d  %X\r\n", current_sms->sms_state, current_sms);

    //oa_timer_stop(OA_SMS_BUSY_ID);
    if(current_sms == NULL) return;
    if(current_sms->sms_state == OA_SMS_SEND_NONE) return;

    switch(current_sms->sms_state)
    {
        case OA_SMS_SET_RECV_MODE:
            oa_sprintf(cmdString, "at+cnmi=2,2,0,0,1\r\n");/*(1,1,0,1,1)*/
            cmd = oa_at_create_cmd(cmdString);
            if(!cmd)
                goto MF_fail;
            cmd->timeout = 30000;
            cmd->timeout_cb = oa_send_sms_timeout_callback;
            cmd->cb = oa_send_sms_callback;
            oa_at_pend_cmd(cmd);
            err = oa_at_submit();
            //current_sms->sms_state = OA_SMS_SET_FORMAT;
            break;
        case OA_SMS_SET_FORMAT:
            oa_sprintf(cmdString, "at+cmgf=%d\r\n", current_sms->sms_type);
            cmd = oa_at_create_cmd(cmdString);
            if(!cmd)
                goto MF_fail;
            cmd->timeout = 30000;
            cmd->timeout_cb = oa_send_sms_timeout_callback;
            cmd->cb = oa_send_sms_callback;
            oa_at_pend_cmd(cmd);
            err = oa_at_submit();
            /*
            if(current_sms->sms_type==OA_SMS_TYPE_PDU)
                current_sms->sms_state = OA_SMS_SEND_PDU_DATA;
            else
                current_sms->sms_state = OA_SMS_SET_CHAR_SET;
                */
            break;
            
        case OA_SMS_SEND_PDU_DATA:
            oa_sprintf(cmdString, "at+cmgs=%d\r", current_sms->current_pdu.pdu_length); //here shuold be '\r'
            cmd = oa_at_create_cmd(cmdString);
            if(!cmd)
                goto MF_fail;
            cmd->timeout = 30000;
            cmd->timeout_cb = oa_send_sms_timeout_callback;
            cmd->cb = oa_send_sms_callback;
            cmd->prompt_cb = __oa_sms_send_raw_data_prompt_tx;
            oa_at_pend_cmd(cmd);
            err = oa_at_submit();
            //current_sms->sms_state = OA_SMS_SEND_FINISH;
            break;
            
#ifdef OA_SMS_TEXT
        case OA_SMS_SET_CHAR_SET:
            oa_sprintf(cmdString, "at+cscs=\"GSM\"\r\n");
            cmd = oa_at_create_cmd(cmdString);
            if(!cmd)
                goto MF_fail;
            cmd->timeout = 10000;
            cmd->timeout_cb = oa_send_sms_timeout_callback;
            cmd->cb = oa_send_sms_callback;
            oa_at_pend_cmd(cmd);
            err = oa_at_submit();
            /*
            if(err != OA_AT_ERR_NONE)
                goto MF_fail;
                */
            //current_sms->sms_state = OA_SMS_SEND_TEXT;
            break;
        case OA_SMS_SEND_TEXT:
            oa_sprintf(cmdString, "at+cmgs=\"%s\"\r", current_sms->current_text.data_number);
            cmd = oa_at_create_cmd(cmdString);
            if(!cmd)
                goto MF_fail;
            cmd->timeout = 20000;
            cmd->timeout_cb = oa_send_sms_timeout_callback;
            cmd->cb = oa_send_sms_callback;
            cmd->prompt_cb = __oa_sms_send_raw_data_prompt_tx;
            oa_at_pend_cmd(cmd);
            err = oa_at_submit();
            //current_sms->sms_state = OA_SMS_SEND_FINISH;
            break;
 #endif

        case OA_SMS_SEND_FINISH:
            /*shuold we delete the sms-list node here?*/
            SMS_DEBUG_L2("[OA]SMS: finished sms sending & delete sms-list node! %d\r\n", sms_pend_count);
            llist_del(head->next);
            sms_pend_count--;
            oa_sms_delete_sms(current_sms);
            oa_sms_set_current_sms(NULL);
            g_current_pend_state = OA_SMS_PEND_IDLE;

			/*switch back to default for receiving SMS in PDU mode.*/
            //oa_at_cmd_demo("at+cmgf=0\r\n");
            //oa_at_cmd_demo_submit();
			
            break;
            
        default:
            break;
    }

    return;

    MF_fail:
        //oa_trace_on_file("sms send fail!  %d", g_sms_state);
        SMS_DEBUG_L2("[OA]SMS: sms send-machine failed! %d", current_sms->sms_state);
}

void oa_recv_sms_state_machine(void *param)
{
    llist_head *revcHead = &g_at_sms_recv_pend_list;
    oa_sms_recv_index *cur_index = (oa_sms_recv_index *)(revcHead->next);
    oa_at_cmd *cmd = NULL;
    oa_at_errno err;
    oa_char cmdString[30]= {0};
    
    switch(cur_index->state)
    {
        case OA_SMS_RECV_INIT:
        case OA_SMS_RECV_READ:
            /*just send at cmd to read sms*/
            //if(oa_at_busy()) return;
            oa_sprintf(cmdString, "at+cmgr=%d\r\n", cur_index->index);
            cmd = oa_at_create_cmd(cmdString);
            if(!cmd)
                goto MF_fail;
            cmd->timeout = 30000;
            cmd->timeout_cb = oa_recv_sms_timeout_callback;
            cmd->cb = oa_recv_sms_callback;
            oa_at_pend_cmd(cmd);
            err = oa_at_submit();
            break;
            
        case OA_SMS_RECV_STORE:
            /*restore the sms-context and delete sms if necessary*/
            //if(oa_at_busy()) return;
            oa_sprintf(cmdString, "at+cmgd=%d\r\n", cur_index->index);
            cmd = oa_at_create_cmd(cmdString);
            if(!cmd)
                goto MF_fail;
            cmd->timeout = 30000;
            cmd->timeout_cb = oa_recv_sms_timeout_callback;
            cmd->cb = oa_recv_sms_callback;
            oa_at_pend_cmd(cmd);
            err = oa_at_submit();
            break;

        case OA_SMS_RECV_FINISH:
            //delete the recv-index node
            SMS_DEBUG_L2("delete the recv-index node %d", cur_index->index);
            llist_del(revcHead->next);
            oa_free(cur_index);
            break;
        default:
            break;
    }

    return;

    MF_fail:
        SMS_DEBUG_L2("[OA]SMS: sms recv-machine failed! %d", cur_index->state);
}

void oa_send_sms_timeout_callback(void *param)
{
    oa_at_cmd *cmd = (oa_at_cmd *)param;
    oa_sms_context *current_sms = oa_sms_get_current_sms();
    
    if(!cmd) return;
    SMS_DEBUG_L2("[OA]SMS: sms machine timeout! %d\r\n", current_sms->index);

    cmd->ret = 1;   //let the callback know current cmd was timeout(return error).
    oa_at_finish_cmd(cmd);
}
void oa_send_sms_callback(void *param)
{
    oa_at_cmd *cmd = (oa_at_cmd *)param;
    oa_int32 ret;
    oa_sms_context *current_sms = oa_sms_get_current_sms();
    
    if(!cmd) return;
    
    ret = cmd->ret;
    
    SMS_DEBUG_L2("oa_send_sms_callback called! %d, %s\r\n", g_current_sms->sms_state, cmd->buf);

    //should we switch the STATE here? OR in the machine.
    switch(current_sms->sms_state)
    {
        case OA_SMS_SET_RECV_MODE:
            current_sms->sms_state = OA_SMS_SET_FORMAT;
            break;
        case OA_SMS_SET_FORMAT:
            if(current_sms->sms_type==OA_SMS_TYPE_PDU)
                current_sms->sms_state = OA_SMS_SEND_PDU_DATA;
            else
                current_sms->sms_state = OA_SMS_SET_CHAR_SET;
            break;
        case OA_SMS_SEND_PDU_DATA:
            {
                current_sms->sms_state = OA_SMS_SEND_FINISH;
            }
            break;
        case OA_SMS_SET_CHAR_SET:
            current_sms->sms_state = OA_SMS_SEND_TEXT;
            break;
        case OA_SMS_SEND_TEXT:
            current_sms->sms_state = OA_SMS_SEND_FINISH;
            break;
        case OA_SMS_SEND_FINISH:
            break;
        default:
            break;
    }
    
    if(ret)
        current_sms->sms_state = OA_SMS_SEND_FINISH;
    oa_send_sms_state_machine(NULL);
}
void oa_recv_sms_timeout_callback(void *param)
{
    oa_at_cmd *cmd = (oa_at_cmd *)param;
    
    if(!cmd) return;
    SMS_DEBUG_L2("[OA]SMS: sms recv-machine timeout! %d\r\n");

    cmd->ret = 1;   //let the callback know current cmd was timeout(return error).
    oa_at_finish_cmd(cmd);
}
void oa_recv_sms_callback(void *param)
{
    oa_at_cmd *cmd = (oa_at_cmd *)param;
    oa_int32 ret;
    llist_head *revcHead = &g_at_sms_recv_pend_list;
    oa_sms_recv_index *cur_index = (oa_sms_recv_index *)(revcHead->next);
    
    if(!cmd) return;
    //if(!llist_empty(revcHead)) return;
    
    ret = cmd->ret;
    
    //SMS_DEBUG_L2("oa_send_sms_callback called! %d\r\n", g_current_sms->sms_state);

    //should we switch the STATE here? OR in the machine.
    //SMS_DEBUG_L2("recv cb state:%d", cur_index->state);
    switch(cur_index->state)
    {
        case OA_SMS_RECV_READ:
            if(!ret){
                cur_index->state = OA_SMS_RECV_STORE;
            }
            break;
            
        case OA_SMS_RECV_STORE:
            if(!ret){
                //delete the recv-index node
                llist_del(revcHead->next);
                oa_free(cur_index);
                cur_index->state = OA_SMS_RECV_FINISH;
                //SMS_DEBUG_L2("delete sms-recv node OK");
            }
            break;
            
        default:
            break;
    }

    if(!ret)
        oa_recv_sms_state_machine(NULL);
}

#define OA_ONE_SEGMENT_MAX 512
void oa_sms_send_long_msg_beta(oa_char *TPOA_num, oa_uint8 *Msg_data, oa_int32 Msg_data_len, oa_uint8 inDCS)
{
    oa_int32 totalSeg, i, toSendUDL, currentBoundary;
    oa_uint8 *buf_ptr = Msg_data;
    oa_char *UDH_BUF=NULL, modulo_str[16]={0};
    oa_bool isLong;
    oa_user_data_header hdr = {{'0','5'},{'0','0'},{'0','3'},{'1','F'},{'0','0'},{'0','0'}};

	oa_sprintf(modulo_str,"%02X", 0x00);
	hdr.IEDa[0] = modulo_str[0];
	hdr.IEDa[1] = modulo_str[1];

	OA_DEBUG_USER("%s called: modulo=%s|%c:%c", __func__, modulo_str, hdr.IEDa[0], hdr.IEDa[1]);
	
    /*totalSeg shuold not be larger than 0xFF*/
    if(((inDCS&OA_SMS_DCS_8BIT)==OA_SMS_DCS_8BIT)
            ||((inDCS&OA_SMS_DCS_USC2)==OA_SMS_DCS_USC2)){
        currentBoundary = OA_SMS_UD_BOUNDARY_8BIT;
    }
    else if((inDCS&OA_SMS_DCS_DFALP)==OA_SMS_DCS_DFALP){
        currentBoundary = OA_SMS_UD_BOUNDARY_7BIT;
        /*should convert Msg_data to default alphabet? or outside*/
    }
    else{
        //error
        SMS_DEBUG_L2("[OA]SMS: initial sms inDCS error! %02X", inDCS);
        return;
    }
    totalSeg = (Msg_data_len/currentBoundary)
            +(Msg_data_len%currentBoundary ? 1 : 0);
    //shuold be terminated by '\0'
    UDH_BUF = (oa_char *)oa_malloc(OA_ONE_SEGMENT_MAX);
    
    if(totalSeg > 1){
        isLong = OA_TRUE;
    }
    else{
        /*not a long sms, plz send it in normal mode :)*/
        isLong = OA_FALSE;
        return;
    }
    
    SMS_DEBUG_L2("[oa]smsl: %d, %d", totalSeg, isLong);
    
    for(i=0; i<totalSeg; i++)
    {
        oa_sms_context *p_sms;
        oa_char buf[2+1]={0};
        oa_memset(UDH_BUF, 0x00, OA_ONE_SEGMENT_MAX);

        oa_sprintf(buf, "%02X", totalSeg);
        oa_memcpy(hdr.sumMsg, buf, 2);
        oa_sprintf(buf, "%02X", i+1);
        oa_memcpy(hdr.indexMsg, buf, 2);
        oa_memcpy(UDH_BUF, &hdr, sizeof(hdr));
        
        p_sms = oa_sms_create_sms();
        
        SMS_DEBUG_L2("We will send:isLong=%d, i=%d, totalSeg=%d", isLong, i, totalSeg);

        if(((inDCS&OA_SMS_DCS_8BIT)==OA_SMS_DCS_8BIT)
                ||((inDCS&OA_SMS_DCS_USC2)==OA_SMS_DCS_USC2)){
            oa_int32 toEncodeLen=0;

            if(i<totalSeg-1)
                toEncodeLen = currentBoundary;
            else
                toEncodeLen = Msg_data_len%currentBoundary;
            
            oa_sms_octet2str(UDH_BUF+sizeof(hdr), (oa_uint8 *)buf_ptr, toEncodeLen);
            toSendUDL = sizeof(hdr)/2+toEncodeLen;

            /*begin to init sms*/
            SMS_DEBUG_L2("We will send:toEncodeLen=%d, toSendUDL=%d", toEncodeLen, toSendUDL);
            g_sms_init_param.sms = p_sms;
            g_sms_init_param.TPOA_num = TPOA_num;
            g_sms_init_param.UD = UDH_BUF;
            g_sms_init_param.UDL = toSendUDL;
            g_sms_init_param.inDCS = inDCS;
            g_sms_init_param.isLongMsg = isLong;
	        p_sms->sms_type = OA_SMS_TYPE_PDU;
            oa_sms_initial_sms_context(&g_sms_init_param);
        }
        else if((inDCS&OA_SMS_DCS_DFALP)==OA_SMS_DCS_DFALP){
            oa_int32 toEncodeLen=0;

            /*this is the first byte just to be L-shift*/
            oa_sprintf((oa_char*)buf, "%02X", buf_ptr[0]<<1);
            oa_memcpy(UDH_BUF+sizeof(hdr), buf, 2);
            
            if(i<totalSeg-1)
                toEncodeLen = currentBoundary-1;
            else
                toEncodeLen = Msg_data_len%currentBoundary-1;

            //SMS_DEBUG_L2("Goto encode: %s, %d", buf_ptr, toEncodeLen);
            /*others to be normally 7bit encode*/
            oa_memset(g_buffer_store_str, 0x00, 512);
            oa_memset(g_buffer_store_octet, 0x00, 512);

            g_store_octet_len= oa_gsm_7bit_encode(g_buffer_store_octet, (oa_char*)&buf_ptr[1], toEncodeLen);
            g_store_str_len = oa_sms_octet2str(g_buffer_store_str, g_buffer_store_octet, g_store_octet_len);
            oa_memcpy(UDH_BUF+sizeof(hdr)+2, g_buffer_store_str, g_store_str_len);
            /*header + L-shift byte + 7-bit-encoded bytes + 1*/
            toSendUDL = sizeof(hdr)/2+1+toEncodeLen+1;
            
            /*begin to init sms*/
            //SMS_DEBUG_L2("[OA]LSMS: UDH_BUF: %s, toSendUDL: %d", UDH_BUF, toSendUDL);
            g_sms_init_param.sms = p_sms;
            g_sms_init_param.TPOA_num = TPOA_num;
            g_sms_init_param.UD = UDH_BUF;
            g_sms_init_param.UDL = toSendUDL;
            g_sms_init_param.inDCS = inDCS;
            g_sms_init_param.isLongMsg = isLong;
	        p_sms->sms_type = OA_SMS_TYPE_PDU;
            oa_sms_initial_sms_context(&g_sms_init_param);
        }
        else{
            //error
            SMS_DEBUG_L2("[OA]SMS: initial sms inDCS error! %02X", inDCS);
            return;
        }
        
        buf_ptr += currentBoundary;
        //SMS_DEBUG_L2("LongSMS UD=%s UDL=%d", UDH_BUF, toSendUDL);
        oa_sms_pend_sms(p_sms);
    }

    oa_free(UDH_BUF);
    return;
}

oa_char g_sms_ftp_str[128] = {0};
void oa_sms_delay_to_start_ftp(void *param)
{
	oa_start_ftp_upgrade((oa_char*)g_sms_ftp_str);
}

extern void oa_sms_test_dfalp(oa_char* contx, oa_char* tpoa);
void oa_sms_read_raw_data_rx(void *dummy)
{
    oa_buffer *buf_p = (oa_buffer *)dummy;
    oa_sms_pdu_receive pduRecv={0};
    oa_int32 len, i, userDataLen;
    
    static oa_char strBuf[160+1]={0};
    
    oa_sms_pdu_recv_parse((oa_char *)buf_p->buf, &pduRecv, buf_p->len);

    /* decode process:
     * "C6A20D47C301"-->0xC6 0xA2 0x0D 0x47 0xC3 0x01
     * -->"FE6848"-->0xFE 0x68 0x48-->parse_package
     */

    //SMS_DEBUG_L1("[OA]SMS: raw buflen-->%d", buf_p->len);//bytes
    //SMS_DEBUG_L1("[OA]SMS: raw udl-->%s", pduRecv.TP_UDL);
    //SMS_DEBUG_L1("[OA]SMS: raw ud-->%s", pduRecv.TP_UD);
    
    /*TP_UDL is the length of decoded string length, not the TP_UD length*/
    len = oa_strtol(pduRecv.TP_UDL, NULL, 16);

    if(oa_sms_check_octet_flag(pduRecv.TP_DCS, OA_SMS_DCS_8BIT))
    {
        /*add codes to handle sms context*/
    }
    else if(oa_sms_check_octet_flag(pduRecv.TP_DCS, OA_SMS_DCS_USC2))
    {
        /*add codes to handle sms context*/
        oa_memset(g_buffer_store_octet, 0x00, sizeof(g_buffer_store_octet));
        g_store_octet_len = oa_sms_str2octet(g_buffer_store_octet, pduRecv.TP_UD, len*2);
        oa_sms_octe_reverse(g_buffer_store_octet, g_store_octet_len);
        oa_chset_convert(OA_CHSET_UCS2, OA_CHSET_GB2312, (oa_char*)g_buffer_store_octet, (oa_char *)g_buffer_store_octet, g_store_octet_len);
        SMS_DEBUG_L1("USC2 TEST: %s", (oa_char*)g_buffer_store_octet);
    }
    else if(oa_sms_check_octet_flag(pduRecv.TP_DCS, OA_SMS_DCS_DFALP))
    {
		oa_char* ftp_needle = NULL;
        g_store_octet_len = oa_sms_str2octet(g_buffer_store_octet, pduRecv.TP_UD, oa_strlen(pduRecv.TP_UD));
        //store the recived sms oa_num for next sending
        oa_memcpy(sms_protocol_toa, pduRecv.TOA_value, 20);

		oa_memset(strBuf, 0x00, 160);
        g_store_str_len = oa_gsm_7bit_decode(g_buffer_store_str, g_buffer_store_octet, g_store_octet_len);
        for(i=0; i<len; i++){
            strBuf[i] = OaDefaultToAsciiArray[g_buffer_store_str[i]];
        }
        SMS_DEBUG_L1("[OA]SMS: decoded-->%s", strBuf);

		if((ftp_needle = oa_strstr((oa_char*)strBuf, "ftp:")) != NULL)
		{
			SMS_DEBUG_L1("[OA]SMS: FTP launch-->%s", ftp_needle);
			/*problem here: need to delay sometimes to call oa_start_ftp_upgrade,
			* cause the protocol stack is busy right now
			*/
			//oa_start_ftp_upgrade((oa_char*)ftp_needle);
			oa_memset(g_sms_ftp_str, 0x00, sizeof(g_sms_ftp_str));
			oa_strcpy(g_sms_ftp_str, ftp_needle);
		}
    }

    return;
}

//for oa_at_register_response_cb(...)
void oa_sms_at_response_parse(void *param)
{
    oa_at_cmd *cmd = (oa_at_cmd *)param;
    oa_int32 cme_error = 0;
    oa_int32 cms_error = 0;
    oa_char *buf = cmd->rx;
    //oa_sms_context *current_sms = oa_sms_get_current_sms();

    //SMS_DEBUG_L2("[OA]SMS: Entry oa_sms_at_response_parse.");

    if (cmd && !oa_strcmp(buf, cmd->buf)) {
        //ignore echo.
        return;
    }
	
	/* we have to differentiate between the following cases:
	 *
	 * A) an information response ("+whatever: ...")
	 *    we just pass it up the callback
	 * B) an unsolicited message ("+whateverelse: ... ")
	 *    we call the unsolicited.c handlers
	 * C) a final response ("OK", "+CME ERROR", ...)
	 *    in this case, we need to check whether we already sent some
	 *    previous data to the callback (information response).  If yes,
	 *    we do nothing.  If no, we need to call the callback.
	 * D) an intermediate response ("CONNECTED", "BUSY", "NO DIALTONE")
	 *    TBD
	 */

     if (buf[0] == '+') {
        /* an extended response */
        oa_char *colon = oa_strchr(buf, ':');
        if (!colon) {
            SMS_DEBUG_L2("[OA] AT: no extchar in a externded response!");
            return;
        }
        if (!oa_strncmp(buf+1, "CME ERROR", 9)) {
            /* Part of Case 'C' */
            oa_uint32 err_nr;
            err_nr = oa_strtoul(colon+1, NULL, 10);
            SMS_DEBUG_L2("[OA] AT: error number %lu\n", err_nr);
            if (cmd && (cmd->state == OA_AT_CMD_EXECUTE)){
                cmd->ret = err_nr;
                oa_at_finish_cmd(cmd);
            }
            cme_error = 1;
            goto final_cb;
        }
        if (!oa_strncmp(buf+1, "CMS ERROR", 9)) {
            /* Part of Case 'C' */
            oa_uint32 err_nr;
            err_nr = oa_strtoul(colon+1, NULL, 10);
            //SMS_DEBUG_L2("[OA] AT: error number %lu  %d\n", err_nr, current_sms->index);
            if (cmd && (cmd->state == OA_AT_CMD_EXECUTE)){
                cmd->ret = err_nr;
                oa_at_finish_cmd(cmd);
            }  
            cms_error = 1;
            goto final_cb;
        }

        if(!oa_strncmp(buf+1, "CMGL:", 5)){
            oa_sms_list_entry currentEntry={0};
            oa_sms_list_parse(buf, &currentEntry);
            SMS_DEBUG_L2("[OA] AT: we go into +CMGL:--> %d, %d, %d\n", currentEntry.index, currentEntry.state, currentEntry.length);

            /*
            oa_at_register_read_raw_data_cb(oa_sms_read_raw_data_rx, 
                                                                        (currentEntry.length+OA_SMS_PDU_SCA_LEN)*2);
            */
            /*dispatch the sms entry with different states*/
            switch(currentEntry.state)
            {
                case OA_SMS_REC_UNREAD:
                case OA_SMS_REC_READ:
                case OA_SMS_STO_UNSENT:
                case OA_SMS_STO_SENT:
                    break;

                default:
                    break;
            }

            goto final_cb;
        }

        /*at+cnmi unsolicited message (a new sms received)*/
        if(!oa_strncmp(buf+1, "CMTI:", 5)){
            oa_sms_recv_index *recvIndex;
            oa_sms_unsolParam tempUnsol={0};
            oa_unsolicited_parse(buf, &tempUnsol);

            recvIndex = (oa_sms_recv_index *)oa_malloc(sizeof(oa_sms_recv_index));
            recvIndex->index = oa_atoi(tempUnsol.param[1]);//if parse has problem, we shuold catch the exception?
            llist_add_tail((llist_head *)recvIndex, &g_at_sms_recv_pend_list);

            SMS_DEBUG_L2("[OA]SMS: we go into +CMTI: index=%d\r\n", recvIndex->index);
            goto final_cb;
        }
        if(!oa_strncmp(buf+1, "CMT:", 4)){
            SMS_DEBUG_L2("[OA]SMS: we go into +CMT.\r\n");
            goto final_cb;
        }

        /*at+cmgr unsolicited message to read sms*/
        if(!oa_strncmp(buf+1, "CMGR:", 5)){
            SMS_DEBUG_L2("[OA]SMS: we go into +CMGR: pioneer");
            {
                oa_int32 smsLength;
                oa_sms_unsolParam tempUnsol={0};
                oa_unsolicited_parse(buf, &tempUnsol);
                smsLength= oa_atoi(tempUnsol.param[2]);
                SMS_DEBUG_L2("[OA]SMS: we go into +CMGR: len=%d", smsLength);
                oa_at_register_read_raw_data_cb(oa_sms_read_raw_data_rx, (smsLength+OA_SMS_PDU_SCA_LEN)*2);
            }

            goto final_cb;
        }
        
        if(!oa_strncmp(buf+1, "CMGS:", 5)){
            /*sms send count.*/

            goto final_cb;
        }
        
        /*sms-state-report*/
        if(!oa_strncmp(buf+1, "CDS:", 4)){
            /*register to read report raw data here.*/
            //SMS_DEBUG_L2("[OA]SMS: sms-state-report received.\r\n");
            goto final_cb;
        }
     } 
     else 
        {
            /* most normal resp for a cmd.. */
            if (!oa_strcmp(buf, "ERROR")){
                /* Part of Case 'C' */
                if (cmd && (cmd->state == OA_AT_CMD_EXECUTE)){
                    cmd->ret = 4;
                    oa_at_finish_cmd(cmd);
                }
                goto final_cb;
            }
            if (!oa_strcmp(buf, "OK")) {
                /* Part of Case 'C' */
                if (cmd && (cmd->state == OA_AT_CMD_EXECUTE)){
                    cmd->ret = 0;
                    oa_at_finish_cmd(cmd);
                    SMS_DEBUG_L1("[OA] AT: Current cmd finished OK!\n");
                }
                goto final_cb;
            }

            if (!oa_strcmp(buf, "RING")) {
                /* this is the only non-extended unsolicited return
                  * code, part of Case 'B' */
                //return unsolicited_parse(g, buf, len, NULL);
            }

            if (!oa_strcmp(buf, "NO CARRIER")) {
                /* Part of Case 'D' */
                goto final_cb;
            }

            if (!oa_strncmp(buf, "BUSY", 4)) {
                /* Part of Case 'D' */
                goto final_cb;
            }
     }
  
final_cb:
    //SMS_DEBUG_L2("[OA]SMS: Leave oa_sms_at_response_parse.");
    //none now.
    ;
}
//SMS end

//assistant function: back strchr
const char *strchr_back(const char *head, const char *tail, char ref)
{
	const char *p = tail;
	while ((p!=head)&&(*p!='\0'))
	{
		if(*p == ref)
			return p;
		p--;
	}
	return NULL;
}

//before calling, we shuold confirm that buf is an unsolicited message
void oa_unsolicited_parse(const char *buf, oa_sms_unsolParam *up)
{
    const oa_char *head, *tail;
    oa_size_t len;
    oa_int32 row=0, col=0;

    //initialization
    head = buf;
    tail = oa_strchr(head, ':');
    if(!tail)
        return;

    //parse cmd text
    len = (oa_size_t)(tail-head);
    oa_strncpy(up->cmd, head, len);

    head = tail+1;
    while (*head!=0x00)
    {
        if(OA_IS_DAA(*head)){
            up->param[row][col] = *head;
            head++;
            col++;
        }
        else if (*head==','){
            head++;
            col=0;
            row++;
        }
        else{
            head++;
        }
    }

    up->paraLen = row+1;
    return;
}

void oa_sms_list_parse(const oa_char *buf, oa_sms_list_entry *entry)
{
    oa_sms_unsolParam unsol_p;
    
    oa_unsolicited_parse(buf, &unsol_p);
    SMS_DEBUG_L2("[OA]SMS: oa_sms_list_parse: %d", unsol_p.paraLen);

    //something like "+CMGL: 2, 1,, 35" has 4 parameters
    //if((unsol_p.paraLen!=4)||(strncmp(unsol_p.cmd, "+CMGL", 5))) {
    if(unsol_p.paraLen!=4){
        SMS_DEBUG_L2("[OA]SMS: oa_sms_list_parse: wrong param!");
        return;
    }
    
    entry->index = oa_atoi(unsol_p.param[0]);
    entry->state = oa_atoi(unsol_p.param[1]);
    entry->length = oa_atoi(unsol_p.param[3]);

    SMS_DEBUG_L2("[OA]SMS: oa_sms_list_parse: %d, %d, %d", entry->index, entry->state, entry->length);
    return;
}

/* "8613800210500F" --> "683108200105F0" and filled with fillChar if not even :) */
oa_size_t oa_sms_pdu_num_reverse(oa_char *buf, char fillChar)
{
    oa_char *p, *q;
    oa_size_t len = oa_strlen(buf);
    
    p = buf;
    q = buf+1;
    if(len%2 != 0)
    {
        buf[len] = fillChar;
        len++;
    }
    while((*p!='\0')&&(*q!='\0'))
    {
        oa_char temp;
        temp = *p;
        *p = *q;
        *q = temp;
        p+=2;
        q+=2;
    }
    return len;
}

oa_size_t oa_sms_octe_reverse(oa_uint8 *buf, oa_int16 len)
{
    oa_uint8 *p, *q;
    oa_int16 relLen;
    
    p = buf;
    q = buf+1;
    if(len%2 != 0)
    {
        relLen = --len;
    }
    
    while(len>0)
    {
        oa_uint8 temp;
        temp = *p;
        *p = *q;
        *q = temp;
        p+=2;
        q+=2;

        len-=2;
    }
    return relLen;
}


/*fuction to parse a received-pdu sms*/
void oa_sms_pdu_recv_parse(const oa_char *buf, oa_sms_pdu_receive *entry, oa_int16 len)
{
    oa_char *p, *head;
    oa_char lenStr[16]={0};
    oa_uint16 lenBuf;
    oa_char *indx = NULL;

    head = (oa_char *)oa_malloc(len+1);
    oa_memset(head, 0x00, len+1);
    oa_memcpy(head, buf, len);
    p = head;
    
    //SCA
    oa_strncpy(lenStr, p, 2);
    lenBuf = oa_strtoul(lenStr, NULL, 16);
    oa_strncpy(entry->SCA_length, p, 2);
    p+=2;
    oa_strncpy(entry->SCA_type, p, 2);
    p+=2;
    //if(!strncmp(entry->SCA_type, "91", 2)){
    if(oa_sms_check_octet_flag(entry->SCA_type, OA_SMS_NUM_TYPE_INTERN)){
        entry->SCA_value[0] = '+';
        oa_strncpy(&(entry->SCA_value[1]), p, (lenBuf-1)*2);
        oa_sms_pdu_num_reverse(&(entry->SCA_value[1]), 'F');
        //delete 'F' if exist
        indx = oa_strchr(entry->SCA_value, 'F');
        if(indx)
            *indx = 0x00;
    }
    else{
        oa_strncpy(entry->SCA_value, p, (lenBuf-1)*2);
        oa_sms_pdu_num_reverse(entry->SCA_value, 'F');
        //delete 'F' if exist
        indx = oa_strchr(entry->SCA_value, 'F');
        if(indx)
            *indx = 0x00;
    }
    p+=(lenBuf-1)*2;

    //PDU-Type
    oa_strncpy(entry->PDU_TYPE, p, 2);
    p+=2;

    //OA
    oa_strncpy(lenStr, p, 2);
    lenBuf = oa_strtoul(lenStr, NULL, 16);
    if(lenBuf%2==1)
        lenBuf++;//there is an 'F' additionally if odd
    oa_strncpy(entry->TOA_length, p, 2);
    p+=2;
    oa_strncpy(entry->TOA_type, p, 2);
    p+=2;
    //if(!strncmp(entry->TOA_type, "91", 2)){
    if(oa_sms_check_octet_flag(entry->TOA_type, OA_SMS_NUM_TYPE_INTERN)){
        entry->TOA_value[0] = '+';
        oa_strncpy(&(entry->TOA_value[1]), p, lenBuf);
        oa_sms_pdu_num_reverse(&(entry->TOA_value[1]), 'F');
        //delete 'F' if exist
        indx = oa_strchr(entry->TOA_value, 'F');
        if(indx)
            *indx = 0x00;
    }
    else{
        oa_strncpy(entry->TOA_value, p, lenBuf);
        oa_sms_pdu_num_reverse(entry->TOA_value, 'F');
        //delete 'F' if exist
        indx = oa_strchr(entry->TOA_value, 'F');
        if(indx)
            *indx = 0x00;
    }
    p+=lenBuf;

    //PID
    oa_strncpy(entry->TP_PID, p, 2);
    p+=2;
    //DCS
    oa_strncpy(entry->TP_DCS, p, 2);
    p+=2;
    //SCTS
    oa_strncpy(entry->SCTS, p, 7*2);
    p+=7*2;
    //UDL
    /*TP_UDL is the length of decoded string length, not the TP_UD length*/
    oa_strncpy(entry->TP_UDL, p, 2);
    p+=2;
    //UD
    oa_strcpy(entry->TP_UD, p);

    oa_free(head);

    return;
}

//to convert from string to octet( in_"1F89"---> out_ 0x1F 0x89 )
oa_uint16 oa_sms_str2octet(oa_uint8 *out, oa_char *in, oa_uint16 strLen)
{
    oa_char *p = in;
    oa_uint8 temp = 0;
    oa_int32 out_index=0;
    oa_char buff[2+1]={0};
    
    if(strLen%2 != 0){//length of string shuold be even!
        SMS_DEBUG_L2("oa_sms_str2octet fatal error!");
        //oa_add_fatal_time();
        strLen--;
        //return;
    }

    strLen/=2;
    for(out_index=0; out_index<strLen; out_index++)
    {
        oa_strncpy(buff, p, 2);
        temp = oa_strtoul(buff, NULL, 16);
        //[out] is a param oa_buffer, so we can access it directly.
        out[out_index] = temp;
        p+=2;
    }

    return strLen;
}

//to convert from octet to string( in_ 0x1F 0x89 ---> out_ "1F89" )
oa_uint16 oa_sms_octet2str(oa_char *out, oa_uint8 *in, oa_uint16 octetLen)
{
    oa_int32 out_index = 0;
    oa_char buff[2+1]={0};
    oa_char *p = out;

    for(out_index=0; out_index<octetLen; out_index++)
    {
        oa_sprintf(buff, "%02X", in[out_index]);
        oa_memcpy(p, buff, 2);
        p+=2;
    }
    *p=0x00;
    
    return octetLen*2;
}

oa_int32 oa_gsm_7bit_encode(oa_uint8 *outBuf, oa_char *inBuf, oa_int32 size)
{
    oa_int32 i, j, LS=0, RS;
    oa_uint8 buf=0x00;
    oa_char operatBuf[256]={0};

    if(size>=256){
        SMS_DEBUG_L2("[OA]SMS: oa_gsm_7bit_encode size too large %d", size);
        return 0;
    }

    oa_memcpy(operatBuf, inBuf, size);
    for(i=0, j=0; i<size; i++)
    {
        RS = (i%8);
        LS = 8-(i%8);

        if((i%8)==0){
            buf = operatBuf[i];
            continue;
        }

        outBuf[j] = ((operatBuf[i]<<LS)|buf);
        buf = operatBuf[i]>>RS;
        j++;
    }
    //last data
    if(size%8!=0){
        outBuf[j] = ((operatBuf[i]<<LS)|buf);
        return ++j;
    }
    else{
        return j;
    }
}

oa_int32 oa_gsm_7bit_decode(oa_char *outBuf, oa_uint8 *inBuf, oa_int32 size)
{
    oa_int32 i, j, LS, RS;
    oa_uint16 container = 0x0000;

    for (i=0,j=0; i<size; i++, j++)
    {
        LS = (i%7);
        RS = 8-(i%7);

        container = inBuf[i];
        container = container << LS;
        container = container|(inBuf[i-1]>>RS);//overflow here!
        outBuf[j] = container&FULL_BIT7;

        if ((i+1)%7==0){
            j++;
            outBuf[j] = container>>7;
        }
    }
    return j;
}

/*check the flags */
oa_bool oa_sms_check_octet_flag(oa_char *input, oa_uint8 flag)
{
    oa_uint8 octetBuf = 0x00;
    oa_char charBuf[3]={0};

    oa_strncpy(charBuf, input, 2);
    octetBuf = oa_strtoul(charBuf, NULL, 16);

    if((octetBuf & flag)==flag)
        return OA_TRUE;
    else
        return OA_FALSE;
}

void oa_sms_set_octet_flag(oa_char *output, oa_uint8 flag)
{
    
    return;
}

oa_char g_sms_send_buf[256] = {0};/**/

/****************just for test sms*******************/
oa_char test_ttt[]={"This is a test sms!@@@@ There is no doubt that Emacs is the most powerful text editor available! Unfortunately Emacs has quite a reputation.Unfortunately EMACS!"};
//oa_char test_ttt[]={"This is a test sms!@@@!!!"};
void oa_sms_test_dfalp(oa_char* contx, oa_char* tpoa)
{
    oa_int32 k;
    oa_sms_context *p_sms;
    p_sms = oa_sms_create_sms();

    if(p_sms!=NULL)
    {
        oa_memset(g_buffer_store_str, 0x00, 512);
        oa_memset(g_buffer_store_octet, 0x00, 512);

        for(k=0; k<oa_strlen(contx); k++){
            /*may encounter a letter called 0x00*/
            g_buffer_store_str[k] = OaAsciiToDefaultArray[contx[k]];
            //g_buffer_store_str[k] = contx[k];
        }
        g_store_octet_len= oa_gsm_7bit_encode(g_buffer_store_octet, g_buffer_store_str, oa_strlen(contx));
        oa_memset(g_buffer_store_str, 0x00, 512);
        g_store_str_len = oa_sms_octet2str(g_buffer_store_str, (oa_uint8 *)g_buffer_store_octet, g_store_octet_len);
        /*begin to init sms*/
        //SMS_DEBUG_L2("UD: %s", g_buffer_store_str);
        g_sms_init_param.sms = p_sms;
        g_sms_init_param.TPOA_num = tpoa;
        g_sms_init_param.UD = g_buffer_store_str;
        g_sms_init_param.UDL = oa_strlen(contx);
        g_sms_init_param.inDCS = OA_SMS_DCS_DFALP;//OA_SMS_DCS_DFALP
        g_sms_init_param.isLongMsg = OA_FALSE;
		g_sms_init_param.sms->sms_type = OA_SMS_TYPE_PDU;

        oa_sms_initial_sms_context(&g_sms_init_param);
        //pend sms into list
        oa_sms_pend_sms(p_sms);
        //OA_DEBUG_L2("[OA]SMS: pend sms OK!");
    }
    else{
        //fatal error!no memory space?
        OA_DEBUG_USER("[OA]SMS: create sms Failed!");
        //break;
    }
}

void oa_sms_test_ucs2(oa_uint8* contx, oa_char* tpoa, oa_uint16 len)
{
    oa_sms_context *p_sms;
    p_sms = oa_sms_create_sms();

    if(p_sms!=NULL)
    {
        oa_memset(g_buffer_store_str, 0x00, 512);
        oa_memset(g_buffer_store_octet, 0x00, 512);

        g_store_str_len = oa_sms_octet2str(g_buffer_store_str, (oa_uint8 *)contx, len);

        /*begin to init sms*/
        //SMS_DEBUG_L2("UD: %s", g_buffer_store_str);
        g_sms_init_param.sms = p_sms;
        g_sms_init_param.TPOA_num = tpoa;
        g_sms_init_param.UD = g_buffer_store_str;
        g_sms_init_param.UDL = len;
        g_sms_init_param.inDCS = OA_SMS_DCS_USC2;
        g_sms_init_param.isLongMsg = OA_FALSE;
		g_sms_init_param.sms->sms_type = OA_SMS_TYPE_PDU;
		
        oa_sms_initial_sms_context(&g_sms_init_param);
        //pend sms into list
        oa_sms_pend_sms(p_sms);
        //OA_DEBUG_L2("[OA]SMS: pend sms OK!");
    }
    else{
        //fatal error!no memory space?
        //OA_DEBUG_L2("[OA]SMS: create sms Failed!");
        //break;
    }
}

void oa_sms_test_textmode(oa_char* contx, oa_char* tpoa)
{
    oa_int32 i, k;
    oa_sms_context *p_sms;
    p_sms = oa_sms_create_sms();

    if(p_sms!=NULL)
    {
        /*begin to init sms*/
        //SMS_DEBUG_L2("UD: %s", g_buffer_store_str);

		memset(&g_sms_init_param, 0x00, sizeof(g_sms_init_param));
		memset(p_sms, 0x00, sizeof(oa_sms_context));
		
        g_sms_init_param.sms = p_sms;
        g_sms_init_param.TPOA_num = tpoa;
        g_sms_init_param.UD = contx;
		g_sms_init_param.sms->sms_type = OA_SMS_TYPE_TEXT;

        oa_sms_initial_sms_context(&g_sms_init_param);
        //pend sms into list
		
        oa_sms_pend_sms(p_sms);
        //OA_DEBUG_L2("[OA]SMS: pend sms OK!");
    }
    else{
        //fatal error!no memory space?
        //OA_DEBUG_L2("[OA]SMS: create sms Failed!");
        //break;
    }
}

/***************************************************************/
/********************TST API for SMS****************************/
/***************************************************************/
//create SMS sengding buffer
oa_sms_send_buf_struct g_oa_sms_send_buf={0};
/*oa sms init after SIM ready and network registered*/
void oa_sms_init(void)
{
    /*注册接收短消息的回调函数*/
    //oa_sms_rcv_ind_register(oa_sms_rcv_ind_handler);
    //开机删除短消息内存
	//oa_at_cmd_demo("at+cmgd=0,4\r\n");
	//oa_at_cmd_demo_submit();
	    /*shuold be coverd be at-response*/
    //strcpy(sms_protocol_toa, "+8613122135713");
    //strcpy(sms_protocol_sca, "+8613800210500");

    /*sms send-recv pend init*/
    INIT_LLIST_HEAD(&g_at_sms_send_pend_list);
    INIT_LLIST_HEAD(&g_at_sms_recv_pend_list);
    sms_pend_count = 0;

    /*buffer for protocol*/
    g_sms_pro_rx = oa_create_param_buffer(sms_protocol_buf, sizeof(sms_protocol_buf));
    oa_memset(g_sms_pro_rx->buf, 0x00, 256);

    /*toggle for sms handler*/
    g_at_sms_pend_toggle = OA_TRUE;
    
    oa_sms_set_current_sms(NULL);
    SMS_DEBUG_L2("oa_sms_initial OK!");
    return;
}

/*new example to send SMS*/
extern oa_uint8 GB2312Words[];
extern oa_wchar UCS2Words[];
extern void oa_sms_send_req_cb(os_sms_result result);
extern oa_size_t oa_sms_octe_reverse(oa_uint8 *buf, oa_int16 len);
oa_char smsSendData_dlf[]={"TRACE:Ready to set apn info--cmnet, root, !@#$%^&*()_+-={}[]|*~`;:?<>,. gprs--cmt.3322.org, 2011#abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrll;"};
oa_char real_send_sms[256]={0};
extern oa_uint16 g_sms_api_recv_counter;


/* 例子:发送英文短消息"123ABC" 到地址"13812345678" */
void os_sms_send_demo_7bit(void)
{
    oa_sms_send("13812345678", "123ABC", oa_strlen((oa_char *)"123ABC"),OA_SMSAL_DEFAULT_DCS);

}
/* 例子:发送16进制短消息"0x01,0x02,0x03,0xAA,0xBB,0xCC" 到地址"13812345678" */
void os_sms_send_demo_8bit(void)
{
    oa_uint8 smsSendData[]={0x01,0x02,0x03,0xAA,0xBB,0xCC,0x00};
    oa_sms_send("13812345678",  (oa_char *)smsSendData, 6,OA_SMSAL_8BIT_DCS);

}
/* 例子:发送中文短消息"测试ABC" 到地址"13812345678" */
void os_sms_send_demo_ucs2(void)
{
    oa_char sendUcs2Str[100]={0};

    /*首先将 GB2312 编码转换为UCS2编码*/
    oa_chset_convert(OA_CHSET_GB2312,
                                   OA_CHSET_UCS2,
                                   "测试ABC，首航测试短信。",
                                  sendUcs2Str,
                                   100); 

    oa_sms_send("13916706878", sendUcs2Str, oa_wstrlen((oa_wchar *)sendUcs2Str),OA_SMSAL_UCS2_DCS);

}

//发送短消息后的，状态返回回调函数
void oa_sms_send_req_cb(os_sms_result result)
{
    OA_DEBUG_USER("%s: ret=%d", __func__,result);

    if(OA_SMS_OK != result 
      && g_oa_sms_send_buf.send_cnt < OA_SMS_RESEND_TIMES
      && oa_strlen(g_oa_sms_send_buf.send_buffer)>0)
    {
		//如果发送失败，将重发短消息，直到超过设定的最大发送次数，默认为2次

		oa_sleep(200);
		oa_sms_send_req(oa_sms_send_req_cb,g_oa_sms_send_buf.send_addr,
		                        g_oa_sms_send_buf.send_buffer,g_oa_sms_send_buf.send_len,
		                        g_oa_sms_send_buf.dcs);

		g_oa_sms_send_buf.send_cnt ++;

		OA_DEBUG_USER("%s: resend!",__func__);

		return;
    }
    else
    {
		//发送成功，或者失败超过发送次数，清除buffer
		oa_memset(&g_oa_sms_send_buf,0,sizeof(oa_sms_send_buf_struct));
    }
}

void oa_sms_send(oa_char * pAddr, oa_char * pData, oa_uint16 len,oa_smsal_dcs_enum dcs)
{
      //将短消息内容，保存在发送缓存，如果失败将重发
       if(oa_strlen(pAddr)==0 || len ==0)
       {
         OA_DEBUG_USER("%s: invalid param!", __func__);
         return;
       }

       if(!oa_sim_network_is_valid())
       {
         OA_DEBUG_USER("%s: sim or network is not ready!", __func__);
         return;
       }

       //set sms send buffer
       oa_memset(&g_oa_sms_send_buf,0,sizeof(oa_sms_send_buf_struct));
       oa_memcpy(g_oa_sms_send_buf.send_addr,pAddr,oa_strlen(pAddr));

       //UCS2 code
       if(dcs == OA_SMSAL_UCS2_DCS)
       {
          oa_memcpy(g_oa_sms_send_buf.send_buffer,pData,len*2);
       }
       else
       {
         oa_memcpy(g_oa_sms_send_buf.send_buffer,pData,len);
       }
       g_oa_sms_send_buf.send_len=len;
       g_oa_sms_send_buf.dcs=dcs;
       g_oa_sms_send_buf.send_cnt =1;

       oa_sms_send_req(oa_sms_send_req_cb,g_oa_sms_send_buf.send_addr,
                                     g_oa_sms_send_buf.send_buffer, g_oa_sms_send_buf.send_len,
                                     dcs);

       OA_DEBUG_USER("oa_sms_send addr=%s,len=%d,data=%s", g_oa_sms_send_buf.send_addr,g_oa_sms_send_buf.send_len,g_oa_sms_send_buf.send_buffer);

}

oa_uint16 g_sms_api_recv_counter = 0;
#ifdef SMS_PACTH
/*****************************************************************/
/*------------------sms received indication handler refer framework
 PARAM:
 deliver_num: SMS MO address
 timestamp: sms timestamp
 data: sms text data
 len: sms data length
 dcs: refer oa_smsal_dcs_enum
 RETURN:
   OA_TRUE: the sms be handled by openat user, and will be be saved
   OA_FALSE:the sms is not openat user cared
******************************************************************/
oa_bool oa_sms_rcv_ind_handler(oa_char * deliver_num, oa_char * timestamp, oa_uint8 * data, oa_uint16 len, oa_smsal_dcs_enum dcs)
{
	oa_char timestamp_str[36]={0};
    static oa_uint8 rawData[256]={0};
	static oa_char reply_data[1256]={0};
    oa_int32 textLen;
	oa_sprintf( timestamp_str, "%02X-%02X-%02X-%02X-%02X-%02X-%02X", timestamp[0], timestamp[1], 
                timestamp[2], timestamp[3], timestamp[4], timestamp[5], timestamp[6]);
	OA_DEBUG_USER("NEW sms!: deliver_num=%s, ts=%s", deliver_num, timestamp_str);
	if(dcs == OA_SMSAL_DEFAULT_DCS)
	{
		/*handle ascii sms text.*/
		oa_char* ftp_needle = NULL;
        textLen = oa_strlen((oa_char*)data);
        OA_DEBUG_USER("DEFAULT_DCS: len = %d; data = %s", textLen, (oa_char*)data);

        /*maybe it's a download prompting*/
		if((ftp_needle = oa_strstr((oa_char*)data, "ftp:")) != NULL)
		{
			OA_DEBUG_USER("[OA]SMS: FTP launch-->%s", ftp_needle);
			OA_DEBUG_L3("[OA]SMS: FTP launch-->%s", ftp_needle);
			/*Need delay to start upgrade, cause the protocol stack is busy right now*/
			//oa_start_ftp_upgrade((oa_char*)ftp_needle);
			oa_memset(g_sms_ftp_str, 0x00, sizeof(g_sms_ftp_str));
			oa_strcpy(g_sms_ftp_str, ftp_needle);
		}
	}
	else if(dcs == OA_SMSAL_UCS2_DCS)
	{
		/*handle unicode sms text.*/
        textLen = oa_wstrlen((oa_wchar*)data);
		oa_memset(rawData, 0x00, 256);
        oa_chset_convert(OA_CHSET_UCS2, OA_CHSET_GB2312, (oa_char*)data, (oa_char *)rawData, (textLen+1)*2);
		OA_DEBUG_USER("OA_SMSAL_UCS2_DCS, print the real words here: len=%d.\r\nText = %s", textLen, rawData);
	}
	else
	{
		/*handle 8-bit sms text.*/
		OA_DEBUG_USER("SMS with 8-bit text.");
	}

	return OA_TRUE;/*delete current SMS from memory*/
	//return OA_FALSE;/*do not delete this SMS, user can handle it*/
	/*Warning: if user return OA_FALSE to save current sms in memory, 
	he should pay attention to the memory capacity. 
	Please use at+cpms to confirm current memory capacity and, when the memory is full, 
	user will receive AT command response "+OPENAT: SMS FULL" from PS port 
	and system will clean all the SMS in order to receive more SMS.*/
}
/************************/
#endif
