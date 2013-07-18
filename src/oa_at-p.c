
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   oa_at.c
 *
 * Project:
 * --------
 *   OPEN AT project
 *
 * Description:
 * ------------
 *   This Module defines OPEN AT's AT related function with uart..
 *
 * Author:
 * -------
 *   LiangQuan.
 *
 ****************************************************************************/ 
//#include "oa_global.h"
#include "oa_llist.h"
#include "oa_type.h" 
#include "oa_api.h"
#include "oa_at-p.h"

#include "oa_sms-p.h"

#define AT_DEBUG_L2(...)
#define AT_DEBUG_L1(...)

#define OA_TIMER_ID_CMD_TIMEOUT OA_TIMER_ID_19

llparser g_llp;
static oa_at_cmd *g_cur_at_cmd = NULL;
static oa_at_cmd g_dummy_at_cmd = {NULL, 0, 0, OA_AT_CMD_INIT, NULL, NULL, NULL};
static llist_head g_at_cmd_list;

void oa_at_set_current_cmd(oa_at_cmd *cmd)
{
    g_cur_at_cmd = cmd;
}

oa_at_cmd *oa_at_get_current_cmd()
{
    return g_cur_at_cmd;
}

/* we basically implement a parse that can deal with
 * - receiving and queueing commands from higher level of libgmsd
 * - optionally combining them into one larger command (; appending)
 * - sending those commands to the TA, receiving and parsing responses
 * - calling back application on completion, or waiting synchronously
 *   for a response
 * - dealing with intermediate and unsolicited resultcodes by calling
 *   back into the application / higher levels
 */


static oa_int32 llparse_append(llparser *llp, oa_char byte)
{
    if (llp->cur < llp->buf + llp->len) {
        *(llp->cur++) = byte;
        return 0;
    } else {
        AT_DEBUG_L1("llp->cur too big!!!\n");
        return -1;
    }
}

static void llparse_endline(llparser *llp)
{
    /* re-set cursor to start of buffer */
    llp->cur = llp->buf;
    llp->state = LLPARSE_STATE_IDLE;
    oa_memset(llp->buf, 0, LLPARSE_BUF_SIZE);
}


static oa_int32 llparse_byte_cc(llparser *llp, oa_char byte)
{
    oa_int32 ret = 0;

    //oa_trace_on_file("b=%02X, %d, %d, %c", byte, llp->state, llp->raw_data_count, byte);
    
    switch (llp->state) 
    {
    case LLPARSE_STATE_IDLE:
        if (byte == '\r')
            llp->state = LLPARSE_STATE_IDLE_CR;
        else if (byte == '>')
            llp->state = LLPARSE_STATE_PROMPT;
        else {
            llp->state = LLPARSE_STATE_RESULT;
            ret = llparse_append(llp, byte);
        }
        break;
		
    case LLPARSE_STATE_IDLE_CR:
        if (byte == '\n')
            llp->state = LLPARSE_STATE_RESULT_LF;
        else if(byte == '\r')
            llp->state = LLPARSE_STATE_IDLE_CR;
        else
            llp->state = LLPARSE_STATE_RESULT;
        break;
		
    case LLPARSE_STATE_RESULT_LF:
        llparse_endline(llp);
        if(llp->raw_data_count)
        {
            ret = llparse_append(llp, byte);
            llp->raw_data_count --;
            llp->state = LLPARSE_STATE_RAWDATA;
        }
        else
        {
            if(byte == '\r')
                llp->state = LLPARSE_STATE_IDLE_CR;
            else if(byte == '>')
                llp->state = LLPARSE_STATE_PROMPT;
            else{
                llp->state = LLPARSE_STATE_RESULT;
                ret = llparse_append(llp, byte);
            }
        }
        break;
		
    case LLPARSE_STATE_RESULT:
        if (byte == '\r')
            llp->state = LLPARSE_STATE_IDLE_CR;
        else
            ret = llparse_append(llp, byte);
        break;

    case LLPARSE_STATE_RAWDATA:
        if(llp->raw_data_count==0)
        {
            llparse_endline(llp);
            if(byte == '\r')
                llp->state = LLPARSE_STATE_IDLE_CR;
            else if(byte == '>')
                llp->state = LLPARSE_STATE_PROMPT;
            else
                ret = llparse_append(llp, byte);
        }
        else
        {
            ret = llparse_append(llp, byte);
            llp->raw_data_count --;
        }
        break;
	
    case LLPARSE_STATE_PROMPT:
        llparse_endline(llp);
        if (byte == '\r')
            llp->state = LLPARSE_STATE_IDLE_CR;
        else {
            llp->state = LLPARSE_STATE_RESULT;
            ret = llparse_append(llp, byte);
        }
        break;

    default:
        /*fatal error!*/
        break;
    }

    return ret;
}


oa_int32 llparse_string_cc(llparser *llp, oa_char *buf, oa_uint32 len)
{
    oa_buffer *b;
    oa_at_cmd *cmd = oa_at_get_current_cmd();
    while (len--) {
        oa_int32 rc = llparse_byte_cc(llp, *(buf++));
        if (rc < 0)
            return rc;

        /* if _after_ parsing the current byte we have finished,
         * let the caller know that there is something to handle */
        if (llp->state == LLPARSE_STATE_RESULT_LF && llp->cb) 
        {
            if(cmd)
            {
                cmd->rx = llp->buf;
                llp->cb(cmd);
                cmd->rx = NULL;
            }
            else
            {
                g_dummy_at_cmd.rx = llp->buf;
                llp->cb(&g_dummy_at_cmd);
                g_dummy_at_cmd.rx = NULL;
            }
        }

        if (llp->state == LLPARSE_STATE_RAWDATA && llp->raw_data_count==0)
        {
            if(llp->raw_data_len && llp->raw_data_cb)
            {
                b = oa_create_param_buffer((oa_uint8 *)llp->buf, llp->raw_data_len);
                if(b)
                {
                    llp->raw_data_cb(b);
                    oa_free(b);
                }
                
                llp->raw_data_len = 0;
                llp->raw_data_cb = NULL;
            }
        }

        /* LQ: temporaryly comment out */
        /* if a full SMS-style prompt was received, poke the select */
        if (llp->state == LLPARSE_STATE_PROMPT)
        {
            if(cmd && cmd->prompt_cb)
            {
                cmd->prompt_cb(NULL);
            }
        }
    }

    return 0;
}

static oa_int32 llparse_init(llparser *llp)
{
    llp->state = LLPARSE_STATE_IDLE;
    llp->cur = llp->buf;
    llp->len = sizeof(llp->buf);
    llp->raw_data_count = 0;
    llp->raw_data_len = 0;
    llp->raw_data_cb = NULL;
    llp->cb = NULL;
    llp->flags = LGSM_ATCMD_F_EXTENDED;
    
    return 0;
}

oa_bool oa_at_register_response_cb(oa_func_ptr func)
{
    g_llp.cb = func;

    return OA_TRUE;
}


oa_bool oa_at_send_raw_data(oa_uint8 *data, oa_uint16 len)
{
    oa_bool  ret;
  
    //oa_trace_on_at("oa_at_send_raw_data: %d", len);
    ret = oa_uart_send_at_cmd_req(data,len);

    return ret;
}

oa_bool oa_at_register_read_raw_data_cb(oa_func_ptr cb, oa_uint16 len)
{
    g_llp.raw_data_count = len;
    g_llp.raw_data_len = len;
    g_llp.raw_data_cb = cb;
  
    return OA_TRUE;
}

oa_at_cmd *oa_at_create_cmd(const oa_char *buf)
{
    oa_uint32 buflen = oa_strlen(buf);
    oa_at_cmd *cmd;

    cmd = oa_malloc(sizeof(oa_at_cmd));
    if (!cmd) return NULL;

    cmd->buf = oa_malloc(buflen + 1);
    if(!cmd->buf) 
    {
        oa_free(cmd);
        return NULL;
    }

    cmd->ret = 0;
    cmd->state = OA_AT_CMD_INIT;
    cmd->resp = NULL;
    cmd->timeout = 10000;
    cmd->cb = NULL;
    cmd->prompt_cb = NULL;
    cmd->timeout_cb = NULL;
	
    oa_strcpy(cmd->buf, buf);

    return cmd;
}

oa_bool oa_at_delete_cmd(oa_at_cmd *cmd)
{
    if(cmd->resp) oa_free(cmd->resp);
    if(cmd->buf) oa_free(cmd->buf);
    if(cmd) oa_free(cmd);
   
    return OA_TRUE;
}

void oa_at_finish_cmd(oa_at_cmd *cmd)
{
    llist_head *head = &g_at_cmd_list;
    oa_int32 ret = cmd->ret;

    oa_timer_stop(OA_TIMER_ID_CMD_TIMEOUT);
    
    cmd->state = OA_AT_CMD_DONE;
    oa_at_set_current_cmd(NULL);
    
    if(head->next == &cmd->list)
        llist_del(&cmd->list);

    oa_at_submit();
    
    if(cmd && cmd->cb) 
        cmd->cb(cmd);

    oa_at_delete_cmd(cmd);
}

oa_bool oa_at_busy(void)
{
  if(oa_at_get_current_cmd())
    return OA_TRUE;
  else
    return OA_FALSE;
}

void oa_at_set_ret(oa_at_cmd *cmd, oa_int32 ret)
{
    cmd->ret = ret;
}

void oa_at_cmd_timeout_callback(void *param)
{
    oa_at_cmd *cur_cmd = (oa_at_cmd *)param;
    static oa_int8 oa_at_cmd_timeout_counter = 0;
    
    AT_DEBUG_L2("[AT] oa_at_cmd_timeout_callback called!cur_cmd=%s, cur_cmd->timeout_cb=%x",cur_cmd->buf,cur_cmd->timeout_cb);

    if(cur_cmd == NULL){
        AT_DEBUG_L2("[AT] oa_at_cmd_timeout_callback NULL cur_cmd");
        return;
    }
/*    
    if(3 < oa_at_cmd_timeout_counter++){
        extern UART_PORT PS_UART_PORT;
        uart_clear_at_buffer(PS_UART_PORT);

        oa_at_cmd_timeout_counter = 0;
    }
*/
    if(cur_cmd->timeout_cb)
        cur_cmd->timeout_cb(cur_cmd);
}

oa_at_errno oa_at_send_curr_cmd(oa_at_cmd *cmd)
{
    oa_at_errno ret = OA_AT_ERR_NONE;
    
    AT_DEBUG_L2("[AT] send cmd no wait = '%s', %d", cmd->buf, oa_strlen((oa_char*)cmd->buf));
    
    //AT_DEBUG_L2("AT_FEEDBACK: %s", cmd->buf);
    if(oa_uart_send_at_cmd_req((oa_uint8*)cmd->buf, oa_strlen(cmd->buf)))
    {
        oa_at_set_current_cmd(cmd);
    
        cmd->state = OA_AT_CMD_EXECUTE;

        if(cmd->timeout_cb && cmd->timeout)
            oa_timer_start(OA_TIMER_ID_CMD_TIMEOUT, oa_at_cmd_timeout_callback, cmd, cmd->timeout);
    }
    else
    {
        ret = OA_AT_ERR_UART;
        //TODO: Fatal error. Need RESET!!
    }

    AT_DEBUG_L2("[AT] send cmd ret = %d.", ret);
  
    return ret;
}

oa_at_errno oa_at_pend_cmd(oa_at_cmd *cmd)
{
    oa_at_errno ret = OA_AT_ERR_NONE;
    llist_head *head = &g_at_cmd_list;
    
    AT_DEBUG_L2("[AT] pend cmd '%s'", cmd->buf);

    llist_add_tail((llist_head *)cmd, head);
    return ret;
}

/* user star submit, then controled by at system to continue submitting */
oa_at_errno oa_at_submit(void)
{
    oa_at_errno ret = OA_AT_ERR_NONE;
    llist_head *head = &g_at_cmd_list;
    oa_at_cmd *cmd;
    
    AT_DEBUG_L2("[AT] submit %d %d", oa_at_busy(), llist_empty(head));
	//if(oa_sms_get_current_sms()) return OA_AT_ERR_BUSY;
    if(oa_at_busy()) return OA_AT_ERR_BUSY;
    if(llist_empty(head)) return ret;
    
    cmd = (oa_at_cmd *)(head->next);
    
    ret = oa_at_send_curr_cmd(cmd);
    return ret;
}

/* only when a cmd returns, you can cancel next cmds */
oa_at_errno oa_at_cancel_submit()
{
    oa_at_errno ret = OA_AT_ERR_NONE;
    llist_head *head = &g_at_cmd_list;
    oa_at_cmd *cmd;
    
    AT_DEBUG_L2("[AT] cancel submit");

    /* delete all pending cmd */
    while(!llist_empty(head))
    {
        cmd = (oa_at_cmd *)head->next;
        if(cmd->state == OA_AT_CMD_EXECUTE)
            return OA_AT_ERR_BUSY;
        
        llist_del(head->next);
        oa_at_delete_cmd(cmd);
    }

    AT_DEBUG_L2("[AT] cancel submit ok!");

    return ret;
}

void oa_at_response_handler(oa_uint8 *buf, oa_uint32 len)
{
    llparse_string_cc(&g_llp, (oa_char *)buf, len);

    /* TODO : add support for user port.*/
}

/*user's response callback*/
void oa_at_response_parse(void *param)
{
    oa_at_cmd *cmd = (oa_at_cmd *)param;
    oa_int32 cme_error = 0;
    oa_int32 cms_error = 0;
    oa_char *buf = cmd->rx;

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
            AT_DEBUG_L1("[OA] AT: no extchar in a externded response!");
            return;
        }

        if (!oa_strncmp(buf+1, "CSQ", 3))
        {
            OA_DEBUG_USER("GOT CSQ response!");
        }

        if (!oa_strncmp(buf+1, "CME ERROR", 9)) {
            /* Part of Case 'C' */
            oa_uint32 err_nr;
            err_nr = oa_strtoul(colon+1, NULL, 10);
            if (cmd && (cmd->state == OA_AT_CMD_EXECUTE))
            {
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
            if (cmd && (cmd->state == OA_AT_CMD_EXECUTE))
            {
                cmd->ret = err_nr;
                oa_at_finish_cmd(cmd);
            }  
            cms_error = 1;
            goto final_cb;
        }
        
        if (!oa_strncmp(buf+1, "CSIM:", 5)) {
            goto final_cb;
        }
        
        if (!oa_strncmp(buf+1, "CSCA:", 5)) {
            goto final_cb;
        }
        
        if(!oa_strncmp(buf+1, "CMGL:", 5)){
            goto final_cb;
        }

        /*at+cnmi unsolicited message (a new sms received)*/
        if(!oa_strncmp(buf+1, "CMTI:", 5)){
            oa_sms_recv_index *recvIndex;
            oa_sms_unsolParam tempUnsol={0};
            oa_unsolicited_parse(buf, &tempUnsol);

            recvIndex = (oa_sms_recv_index *)oa_malloc(sizeof(oa_sms_recv_index));
            recvIndex->index = atoi(tempUnsol.param[1]);//if parse has problem, we shuold catch the exception?
            recvIndex->state = OA_SMS_RECV_READ;
            llist_add_tail((llist_head *)recvIndex, &g_at_sms_recv_pend_list);

            //oa_trace_on_uart("[OA]SMS: we go into +CMTI: index=%d\r\n", recvIndex->index);
            goto final_cb;
        }
        if(!oa_strncmp(buf+1, "CMT:", 4)){
            oa_int32 smsLength;
            oa_sms_unsolParam tempUnsol={0};
            //oa_trace_on_uart("[OA]SMS: we go into +CMT.\r\n");
            
            oa_unsolicited_parse(buf, &tempUnsol);
            smsLength= atoi(tempUnsol.param[1]);
            oa_at_register_read_raw_data_cb(oa_sms_read_raw_data_rx, (smsLength+OA_SMS_PDU_SCA_LEN)*2);
            goto final_cb;
        }

        /*at+cmgr unsolicited message to read sms*/
        if(!oa_strncmp(buf+1, "CMGR:", 5)){
        #if 0
            oa_int32 smsLength;
            oa_sms_unsolParam tempUnsol={0};
            oa_unsolicited_parse(buf, &tempUnsol);
            smsLength= atoi(tempUnsol.param[2]);
            oa_trace_on_uart("[OA]SMS: we go into +CMGR: len=%d", smsLength);
            oa_at_register_read_raw_data_cb(oa_sms_read_raw_data_rx, (smsLength+OA_SMS_PDU_SCA_LEN)*2);
        #endif
            goto final_cb;
        }
        
        if(!oa_strncmp(buf+1, "CMGS:", 5)){
            /*sms send count.*/
            
            goto final_cb;
        }
        
        /*sms-state-report*/
        if(!oa_strncmp(buf+1, "CDS:", 4)){
            /*register to read report raw data here.*/
            goto final_cb;
        }

        /*phone call*/
        if(!oa_strncmp(buf+1, "CLIP:", 5)){
            //ata OR ath
            //goto final_cb;
            goto call;
        }

        /*phone call*/
        if(!oa_strncmp(buf+1, "FTP:", 4)){
            //ata OR ath
            //OA_DEBUG_USER("INTO FTP result: %s", buf+5);
            goto final_cb;
        }
		
    } 
    else 
    {
        /* most normal resp for a cmd.. */
        if (!oa_strcmp(buf, "ERROR")) 
        {
            /* Part of Case 'C' */
            if (cmd && (cmd->state == OA_AT_CMD_EXECUTE))
            {
                cmd->ret = 4;
                oa_at_finish_cmd(cmd);
            }
            goto final_cb;
        }
      
        if (!oa_strcmp(buf, "OK")) 
        {
            /* Part of Case 'C' */
            if (cmd && (cmd->state == OA_AT_CMD_EXECUTE))
            {
                cmd->ret = 0;
                oa_at_finish_cmd(cmd);
            }
            goto final_cb;
        }
      
        if (!oa_strcmp(buf, "RING")) 
        {
            /* this is the only non-extended unsolicited return
             * code, part of Case 'B' */
        }
        
        if (!oa_strcmp(buf, "VOICE")) 
        {
            /* Part of Case 'D' */
            //oa_audio_stop_file();
            goto final_cb;
        }
        
        if (!oa_strcmp(buf, "NO CARRIER")) 
        {
            /* Part of Case 'D' */
            //oa_audio_stop_file();
            goto final_cb;
        }

        if (!oa_strncmp(buf, "BUSY", 4)) 
        {
            /* Part of Case 'D' */
            goto final_cb;
        }

		
    }
call:
	oa_app_at();  	
final_cb:
    ;  //none now.
}

/* init at cmd parser */
void oa_app_at_rsp_recv(oa_uint16 len, oa_uint8 *pStr);
void oa_at_init(void)
{
    llist_head *head = &g_at_cmd_list;
    
    AT_DEBUG_L2("[OA] AT: oa_at_init");

    INIT_LLIST_HEAD(head);
    llparse_init (&g_llp);
    oa_at_register_response_cb(oa_at_response_parse);

	oa_atf_rsp_callback_register(oa_app_at_rsp_recv);
}


/*Demo codes for run AT-cmd begin*/
void oa_at_timeout_demo_callback(void *param)
{
    oa_at_cmd *cmd = (oa_at_cmd *)param;
    
    if(!cmd) return;
    oa_at_finish_cmd(cmd);
    AT_DEBUG_L2("%s called! %s\r\n", __func__, cmd->buf);
}
void oa_at_demo_callback(void *param)
{
    oa_at_cmd *cmd = (oa_at_cmd *)param;
    oa_int32 ret;
    
    if(!cmd) return;
    
    ret = cmd->ret;
    AT_DEBUG_L2("%s called!\r\n", __func__);

    /*addition process: we should wake up 
     * oa_wait_on_msg for MSG_ID_OPENAT_INIT_FINISHED_IND.
     * and confirm that the cmd should be the last at-cmd of initialization.
     */
}
//just pend cmd
oa_at_errno oa_at_cmd_demo(oa_char *cmdStr)
{
    oa_at_cmd *cmd = NULL;
    oa_at_errno err;
    
    cmd = oa_at_create_cmd(cmdStr);
    if(!cmd)
        goto MFL_fail;
    cmd->timeout = 20000;
    cmd->timeout_cb = oa_at_timeout_demo_callback;
    cmd->cb = oa_at_demo_callback;
    oa_at_pend_cmd(cmd);

    return OA_AT_ERR_NONE;

    MFL_fail:
        AT_DEBUG_L2("%s fail!\r\n", __func__);
        return OA_AT_ERR_BUSY;
}
//submition
oa_at_errno oa_at_cmd_demo_submit(void)
{
    oa_at_errno err;
    
    err = oa_at_submit();
    if(err != OA_AT_ERR_NONE)
        goto MFL_fail;

    return OA_AT_ERR_NONE;
    
    MFL_fail:
        AT_DEBUG_L2("%s fail! %d", __func__, err);
        return err;
}
/*Demo codes for run AT-cmd begin*/

/*Buffer type*/
/************************************************************/
/*************************** oa buffer*************************/
/************************************************************/
/*function to handle buffers */
oa_buffer *oa_create_ring_buffer(oa_uint8 *rawBuf, oa_uint16 maxLength)
{
    oa_buffer *temp_buf=NULL;

    temp_buf = (oa_buffer *)oa_malloc(sizeof(oa_buffer));
    if(!temp_buf)
        return temp_buf;

    temp_buf->buf = rawBuf;
    temp_buf->len = 0;
    temp_buf->offset = 0;
    temp_buf->maxLength = maxLength;
    
    return temp_buf;
}
oa_buffer *oa_create_param_buffer(oa_uint8 *rawBuf, oa_uint16 len)
{
    oa_buffer *temp_buf=NULL;

    temp_buf = (oa_buffer *)oa_malloc(sizeof(oa_buffer));
    if(!temp_buf)
        return temp_buf;

    temp_buf->buf = rawBuf;
    temp_buf->len = len;
    temp_buf->offset = 0;
    temp_buf->maxLength = len;
    
    return temp_buf;
}

void oa_free_buffer(oa_buffer *buf)
{
    oa_free(buf);
}

oa_uint16 oa_write_buffer(oa_buffer *buf, oa_uint8 *data, oa_uint16 len)
{
    oa_uint16 i, p;
  
    if(buf->len + len > buf->maxLength)
    {
        len = buf->maxLength - buf->len;
    }

    for(i = 0, p = buf->offset + buf->len; i < len; i++, p++)
    {
        if(p >= buf->maxLength)
        {
            p -= buf->maxLength;
        }

        buf->buf[p] = data[i];
    }
  
    buf->len += len;
  
    return len;
}

oa_uint16 oa_query_buffer(oa_buffer *buf, oa_uint8 *data, oa_uint16 len)
{
    oa_uint16 i, p;

    if(buf->len < len)
    {
        len = buf->len;
    }
  
    for(i = 0, p = buf->offset; i < len; i++, p++)
    {
        if(p >= buf->maxLength)
        {
            p -= buf->maxLength;
        }

        data[i] = buf->buf[p];
    }

    //for query, don't update buffer control struct.
    //buf->len -= len;
    //buf->offset = p;  

    return len;
}

oa_uint16 oa_read_buffer(oa_buffer *buf, oa_uint8 *data, oa_uint16 len)
{
    oa_uint16 l;

    l = oa_query_buffer(buf, data, len);
  
    buf->len -= l;
    buf->offset += l;
    if(buf->offset > buf->maxLength)
        buf->offset -= buf->maxLength;

    return l;
}

oa_uint16 oa_write_buffer_force(oa_buffer *buf, oa_uint8 *data, oa_uint16 len)
{
    oa_uint16 i, p, cut_off=0;
  
    cut_off = buf->maxLength - buf->len;
    if(cut_off >= len)
        cut_off = 0;
    else
        cut_off =len - cut_off;

    for(i = 0, p = buf->offset + buf->len; i < len; i++, p++)
    {
        if(p >= buf->maxLength)
        {
            p -= buf->maxLength;
        }
        buf->buf[p] = data[i];
    }

    buf->offset += cut_off;
    if(buf->offset >= buf->maxLength)
        buf->offset -= buf->maxLength;
  
    buf->len += len;
    if(buf->len > buf->maxLength)
        buf->len = buf->maxLength;
  
    return len;
}



