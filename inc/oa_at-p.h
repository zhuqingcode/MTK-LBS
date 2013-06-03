#ifndef _OA_AT_H_
#define _OA_AT_H_

#include "oa_type.h"
//#include "linux_list.h"
#include "oa_llist.h"

typedef enum
{
    OA_AT_ERR_NONE = 0,
    OA_AT_ERR_TIMEOUT,
    OA_AT_ERR_BUSY,
    OA_AT_ERR_UART,
    OA_AT_ERR_NO_MEM
}oa_at_errno;

typedef enum
{
    OA_AT_CMD_INIT = 0,
    OA_AT_CMD_EXECUTE,
    OA_AT_CMD_DONE
}oa_at_cmd_state;

typedef struct { 
    llist_head list;
    oa_at_cmd_state state;           /* state of this cmd */
    oa_int32 ret;                          /* return value */
    oa_uint32 timeout;                 /* timeout */
    oa_func_ptr cb;                       /* callback, actually you can implement this in your parsing of rx. */
    oa_func_ptr timeout_cb;        /* timeout callback, only for async cmds */
    oa_func_ptr prompt_cb;        /* prompt callback when ">" comes(sms, gprs data sending), you need to send raw data then */
    oa_char *buf;          /* the cmd sent */
    oa_char *resp;        /* if multiline, need to combine every rx domain here. */
    oa_char *rx;            /*every single line received when cmd's being executed..*/
}oa_at_cmd;


/* Refer to 3GPP TS 07.07 v 7.8.0, Chapter 4.1 */
#define LGSM_ATCMD_F_EXTENDED	0x01	/* as opposed to basic */
#define LGSM_ATCMD_F_PARAM	0x02	/* as opposed to action */
#define LGSM_ATCMD_F_LFCR	0x04	/* accept LFCR as a line terminator */

typedef enum {
	LLPARSE_STATE_IDLE,		/* idle, not parsing a response */
	LLPARSE_STATE_IDLE_CR,		/* CR before response (V1) */
	LLPARSE_STATE_IDLE_LF,		/* LF before response (V1) */
	LLPARSE_STATE_RESULT,		/* within result payload */
	LLPARSE_STATE_RESULT_CR,	/* CR after result */
	LLPARSE_STATE_RESULT_LF,	/* LF after result */
	LLPARSE_STATE_PROMPT,		/* within a "> " prompt */
//	LLPARSE_STATE_PROMPT_SPC,	/* a complete "> " prompt */
	LLPARSE_STATE_ERROR,		/* something went wrong */
	LLPARSE_STATE_RAWDATA  /*for read rawdata from uart*/
					/* ... idle again */
}llparse_state;

/* we can't take any _single_ response bigger than this: */
#define LLPARSE_BUF_SIZE	20480
/* we can't pare a mutiline response biger than this: */
#define MLPARSE_BUF_SIZE	65535

#define OPENAT_AT_PORT uart_port3

typedef struct {
	llparse_state state;
	oa_uint32 len;
	oa_uint32 flags;
	oa_func_ptr cb;
	oa_char *cur;
	oa_char buf[LLPARSE_BUF_SIZE];
	oa_func_ptr raw_data_cb;
	oa_uint32 raw_data_len;
	oa_uint32 raw_data_count;
}llparser;

extern llparser g_llp;

void oa_at_init(void);
oa_at_cmd *oa_at_create_cmd(const oa_char *buf);
oa_at_errno oa_at_pend_cmd(oa_at_cmd *cmd);
void oa_at_finish_cmd(oa_at_cmd *cmd);
oa_bool oa_at_busy(void);
oa_at_errno oa_at_submit(void);
void oa_at_response_handler(oa_uint8 *buf, oa_uint32 len);

/*demo codes*/
oa_at_errno oa_at_cmd_demo(oa_char *cmdStr);
oa_at_errno oa_at_cmd_demo_submit(void);

/*Buffer type*/
/************************************************************/
/*************************** oa buffer*************************/
/************************************************************/
/*function to handle buffers */
oa_buffer *oa_create_ring_buffer(oa_uint8 *rawBuf, oa_uint16 maxLength);
oa_buffer *oa_create_param_buffer(oa_uint8 *rawBuf, oa_uint16 len);
void oa_free_buffer(oa_buffer *buf);
oa_uint16 oa_write_buffer(oa_buffer *buf, oa_uint8 *data, oa_uint16 len);
oa_uint16 oa_query_buffer(oa_buffer *buf, oa_uint8 *data, oa_uint16 len);
oa_uint16 oa_read_buffer(oa_buffer *buf, oa_uint8 *data, oa_uint16 len);
oa_uint16 oa_write_buffer_force(oa_buffer *buf, oa_uint8 *data, oa_uint16 len);




#endif /* __OA_AT_H__ */

