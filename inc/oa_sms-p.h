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
 *   oa_sms.h
 *
 * Project:
 * --------
 *   OPEN AT project
 *
 * Description:
 * ------------
 *   This Module declares sms api in OPEN AT.
 *
 * Author:
 * -------
 *   Castle.
 *
 ****************************************************************************/
#ifndef _OA_SMS_H_
#define _OA_SMS_H_
//#include "oa_global.h"

//#define OA_SMS_TEXT

/*
* Normally, the total sms length is the sum of 
* length(returned by +cmgl for example) and the length of SCA.
*/
#define OA_SMS_PDU_SCA_LEN (9)

#define OA_SMS_CK_PEND_PERIOD (2000)
#define OA_SMS_UD_MAXLEN (280)

/*for test long message, it's a measurement of byte for pure data*/
#define OA_SMS_UD_BOUNDARY_8BIT (134)
#define OA_SMS_UD_BOUNDARY_7BIT (153)

#define OA_TOGGLE_IT(A) ((A)=((A)?(OA_FALSE):(OA_TRUE)))==OA_TRUE
#define OA_IS_DAA(A) ((A)>='0'&&(A)<='9')||((A)>='a'&&(A)<='z')||((A)>='A'&&(A)<='Z')

/*number type*/
#define OA_SMS_NUM_TYPE_UNKOW 0x00
#define OA_SMS_NUM_TYPE_INTERN 0x10/*international*/
#define OA_SMS_NUM_TYPE_DOME 0x20/*domestic*/
#define OA_SMS_NUM_TYPE_RESV 0x70/*reserve*/
/* TP-DCS octet */
#define FULL_BIT7 0x7F
#define OA_SMS_DCS_DFALP 0x00/*Default alphabet(7bit-encode)*/
#define OA_SMS_DCS_8BIT 0x04/*8 bit data*/
#define OA_SMS_DCS_USC2 0x08/*USC2(16bit)*/
/* PDU type octet*/
#define OA_PDU_TYPE_RP 0x80
#define OA_PDU_TYPE_UDHI 0x40
#define OA_PDU_TYPE_SRR 0x20
#define OA_PDU_TYPE_RD 0x02
#define OA_PDU_TYPE_MTI_SM 0x01/*Submit*/
#define OA_PDU_TYPE_MTI_DL 0x00/*Deliver*/

#define OA_PDU_TYPE_VPF_NONE 0x00
#define OA_PDU_TYPE_VPF_RSV 0x08
#define OA_PDU_TYPE_VPF_REL 0x10/*relative*/
#define OA_PDU_TYPE_VPF_ABS 0x18/*absolute*/

/*pdu octet flags*/


//#define OA_SMS_RETRY_MAX 3

/*******************macro*************************/
//create a sms context by user while delete it by system
//cause user don't know when to delete it
/*
#define oa_sms_create_sms(A) (oa_sms_context *)oa_malloc(sizeof(oa_sms_context))
#define oa_sms_delete_sms(A) oa_free(A)
*/
/*******************macro*************************/


/*******************enum*************************/
typedef enum {
    OA_SMS_SET_RECV_MODE=0,/*it's the beginning*/
    OA_SMS_SET_FORMAT,
    OA_SMS_SEND_PDU_DATA,
    OA_SMS_SET_CHAR_SET,
    OA_SMS_SEND_TEXT,
    OA_SMS_SEND_FINISH,

    OA_SMS_RECV_INIT,
    OA_SMS_RECV_READ,
    OA_SMS_RECV_STORE,   /*Store and delete if necessary*/
    OA_SMS_RECV_FINISH,

    OA_SMS_SEND_NONE    /*nothing to do*/
}oa_sms_state;

typedef enum{   
    /*warning: this enum shuold not be modified*/
    OA_SMS_REC_UNREAD=0,
    OA_SMS_REC_READ,
    OA_SMS_STO_UNSENT,
    OA_SMS_STO_SENT,
    OA_SMS_ALL
}oa_sms_data_state;

typedef enum{
    OA_SMS_TYPE_PDU=0,
    OA_SMS_TYPE_TEXT
}oa_sms_type;

typedef enum{
    OA_SMS_PEND_IDLE=0,   /*sms-queue is free*/
    OA_SMS_PEND_SENDING /*sms-queue is busy*/
}oa_sms_pend_state;

typedef enum{
    OA_SMS_RET_OK=0,
    OA_SMS_RET_BUSY,
    OA_SMS_RET_ERROR
}oa_sms_err_ret;
/*******************enum*************************/


/*******************struct*************************/
typedef struct{
    oa_char retName[16];
    oa_int32 index;
    oa_sms_data_state state;
    oa_uint16 alpha;
    oa_int32 length;
}oa_sms_list_entry;

/*Plz refer to GSM_0340
 *number type parameters(1 byte)
 */
typedef struct{
    oa_uint32 num_iden:4; /*numbering plan identification*/
    oa_uint32 num_type:3; /*type of number*/
    oa_uint32 num_resv:1;/*always be 0x01*/
}oa_num_type;

/*Plz refer to GSM_0340
 *TPDU parameters(1 byte)
 */
typedef struct{
    oa_uint32 TP_MTI:2;
    oa_uint32 TP_RD:1;
    oa_uint32 TP_VPF:2;
    oa_uint32 TP_SRR:1;
    oa_uint32 TP_UDHI:1;/*if using oa_user_data_header*/
    oa_uint32 TP_RP:1;
}oa_pdu_type;

/*SMS Data Coding Scheme(1 byte)
  * Plz refer to GSM_0338
  */
typedef struct{
    oa_uint32 MsgClass:2;
    oa_uint32 Alphabet:2;
    oa_uint32 MsgClass_ind:1;
    oa_uint32 isCompress:1;
    oa_uint32 reserve:2;
}oa_pdu_dcs;

/*string members*/
typedef struct{
    /*warning: most of them are raw data,
        so we shuold handle them carefully.*/
    oa_char SCA_length[2];  //SCA
    oa_char SCA_type[2];
    oa_char SCA_value[20]; /*max length*/
    
    oa_char PDU_TYPE[2];    /*PDU_TYPE*/
    oa_char MR[2];  /*MR*/
    oa_char TPDA_length[2];  /*TP-Destination_Address*/
    oa_char TPDA_type[2];
    oa_char TPDA_value[20];  /*the length of value will be flexible*/
    
    oa_char TP_PID[2];  /*TP-PID*/
    oa_char TP_DCS[2];
    oa_char TP_VP[2];/*send-sms only*/
    oa_char TP_UDL[2];/*user data length(including user_data_header)*/
    oa_char *TP_UD;/*dyn user data, maybe it will include a user_data_header for long msg*/
    //oa_char TP_UD[280];/*user data*/
    //raw data end!

    oa_int32 SCA_size;/*length+type+value*/
    oa_int32 TPDA_size;/*length+type+value*/
    oa_int32 UD_size;/*UDL+UD*/
    oa_int32 pdu_length;/*except SCA section*/
    oa_int32 index;/*reserved*/
}oa_sms_pdu_send;

/*pdu user data additional header(6-bytes)
 * Plz refer to GSM_0340 9.2.3.24
 * some field can be dynamic like:IEDa, IEDb,...,IEDn 
 * but i make them aptotic here
 */
typedef struct{
    oa_char UDHL[2];
    oa_char IEDaL[2];
    oa_char rsIEDL[2];
    oa_char IEDa[2];
    oa_char sumMsg[2];/*total message*/
    oa_char indexMsg[2];/*the index in total message*/
}oa_user_data_header;

/*string members*/
typedef struct{
    oa_char SCA_length[2+1];  /*SCA*/
    oa_char SCA_type[2+1];
    oa_char SCA_value[20+1]; /*the length of value shuold be flexible?*/
    oa_char PDU_TYPE[2+1];    /*PDU_TYPE*/
    oa_char TOA_length[2+1];  /*TP-Source-Address*/
    oa_char TOA_type[2+1];
    oa_char TOA_value[20+1];  /*the length of value shuold be flexible?*/
    oa_char TP_PID[2+1];  /*TP*/
    oa_char TP_DCS[2+1];
    oa_char SCTS[14+1];/*Service Center TimeStamp*/
    oa_char TP_UDL[2+1];/*user data length*/
    oa_char TP_UD[280];/*user data*/
    //oa_char *TP_UD;/*dyn user data*/
    oa_int32 index;/*reserved*/
}oa_sms_pdu_receive;

/*struct sms text*/
typedef struct{
    oa_char data_number[20+1];/* 1 for '\0'*/
    oa_char data_text[200];
}oa_sms_text_send;

/*main noumenon present a sms*/
typedef struct{
    llist_head list;/*for sms-list*/
    oa_sms_state sms_state;/*current sms state for state machine*/
    oa_sms_type sms_type;/*current sms type (text or pdu)*/
    oa_sms_pdu_send current_pdu;/*pdu data for sms*/
#ifdef OA_SMS_TEXT 
    oa_sms_text_send current_text;/*text data for sms*/
#endif
    oa_int32 index;
}oa_sms_context;

/*pend an index when reveive a new sms*/
typedef struct{
    llist_head list;/*for sms-list*/
    oa_int32 index;/*store index we want to receive*/
    oa_sms_state state;/*sms-recv machine state*/
}oa_sms_recv_index;

/*for parse unsolicited message */
typedef struct 
{
    oa_char cmd[10];
    oa_int32 paraLen;
    oa_char param[10][36];
}oa_sms_unsolParam;

/*because so many parameters in 
* oa_sms_initial_sms_context ...*/
typedef struct
{
    oa_sms_context *sms;
    oa_char *TPOA_num;
    oa_char *UD;
    oa_int32 UDL;
    oa_uint8 inDCS; 
    oa_bool isLongMsg;
}oa_init_sms_param_in;
/*******************struct*************************/

/*******************function**********************/
void oa_send_sms_state_machine(void *param);
void oa_recv_sms_state_machine(void *param);
void oa_sms_initial(void);
void oa_sms_initial_by_at_cmd(void);

void oa_send_sms_timeout_callback(void *param);
void oa_send_sms_callback(void *param);
void oa_recv_sms_timeout_callback(void *param);
void oa_recv_sms_callback(void *param);
void oa_sms_at_response_parse(void *param);
void oa_sms_read_raw_data_rx(void *dummy);

oa_sms_context *oa_sms_create_sms(void);
void oa_sms_delete_sms(oa_sms_context * sms);

oa_sms_err_ret oa_sms_pend_sms(oa_sms_context *sms);
void oa_sms_start_ck_sms_pend(void *param);
void oa_sms_stop_ck_sms_pend(void);
void oa_sms_handler(void);
void oa_sms_set_current_sms(oa_sms_context *current_sms);
oa_sms_context *oa_sms_get_current_sms(void);
oa_bool oa_sms_current_sms_empty(void);
void oa_sms_initial_sms_context(oa_init_sms_param_in *initParam);
void oa_sms_send_long_msg_beta(oa_char *TPOA_num, oa_uint8 *Msg_data, oa_int32 Msg_data_len, oa_uint8 inDCS);

void oa_sms_list_parse(const oa_char *buf, oa_sms_list_entry *entry);
void oa_sms_pdu_recv_parse(const oa_char *buf, oa_sms_pdu_receive *entry, oa_int16 len);
oa_size_t oa_sms_pdu_num_reverse(oa_char *buf, char fillChar);
oa_size_t oa_sms_octe_reverse(oa_uint8 *buf, oa_int16 len);

void oa_unsolicited_parse(const char *buf, oa_sms_unsolParam *up);
oa_int32 oa_gsm_7bit_encode(oa_uint8 *outBuf, oa_char *inBuf, oa_int32 size);
oa_int32 oa_gsm_7bit_decode(oa_char *outBuf, oa_uint8 *inBuf, oa_int32 size);


oa_uint16 oa_sms_str2octet(oa_uint8 *out, oa_char *in, oa_uint16 strLen);
oa_uint16 oa_sms_octet2str(oa_char *out, oa_uint8 *in, oa_uint16 octetLen);
oa_bool oa_sms_check_octet_flag(oa_char *input, oa_uint8 flag);


void oa_send_dummy_package_start(void* param);
void oa_send_dummy_package_stop(void);
/*******************function**********************/

/*******************export**********************/
extern llist_head g_at_sms_send_pend_list;
extern llist_head g_at_sms_recv_pend_list;

extern const oa_uint8 OaAsciiToDefaultArray[];
extern const oa_uint8 OaDefaultToAsciiArray[]; 

extern oa_char sms_protocol_toa[];
extern oa_char sms_protocol_sca[];

extern oa_bool g_sms_init_ok;

extern oa_uint16 g_store_str_len;
extern oa_char g_buffer_store_str[];
extern oa_uint16 g_store_octet_len;
extern oa_uint8 g_buffer_store_octet[];

extern oa_init_sms_param_in g_sms_init_param;
extern oa_char g_sms_send_buf[256];

/*******************export**********************/

/*******************import**********************/
extern void oa_add_fatal_time(void);
/*******************import**********************/


/*******************API for SMS****************************/
#define OA_SMS_SEND_MAX_LEN 320
#define OA_SMS_RESEND_TIMES 3

typedef struct 
{
  oa_char  send_addr[21];
  oa_char send_buffer[OA_SMS_SEND_MAX_LEN+10];
  oa_uint8 send_cnt ;
  oa_uint16 send_len ;
  oa_smsal_dcs_enum dcs;

}oa_sms_send_buf_struct;

extern void oa_sms_init(void);
extern void oa_sms_send(oa_char * pAddr, oa_char * pData, oa_uint16 len,oa_smsal_dcs_enum dcs);
extern oa_bool oa_sms_rcv_ind_handler(oa_char * deliver_num, oa_char * timestamp, oa_uint8 * data,  oa_uint16 len, oa_smsal_dcs_enum dcs);

#endif /* __OA_SMS_H__ */

