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
 *   oa_type.h
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
 *   LiangQuan.
 *
 ****************************************************************************/
#ifndef __OA_TYPE_H__
#define __OA_TYPE_H__
/*--------BEG: Customer code----------*/
typedef unsigned int u32;
typedef unsigned short int u16;
typedef unsigned char u8;
typedef float FP32;
typedef signed int s32;
typedef signed short int s16;
#define Trace OA_DEBUG_USER
#define Mem_Copy oa_memcpy
#define Str_Copy oa_strcpy
#define Mem_Set oa_memset
#define Str_Char oa_strchr
#define Str_Cmp_N oa_strncpy
#define SSCRN_Print DEBUG
#define Str_Cat oa_strcat
typedef enum
{
	RESET,
	SET
}flag_status;
/*--------END: Customer code----------*/
typedef unsigned char oa_uint8;
typedef signed char oa_int8;
typedef char oa_char;
typedef unsigned short oa_wchar;
typedef unsigned short int oa_uint16;
typedef signed short int oa_int16;
typedef unsigned int oa_uint32;
typedef signed int oa_int32;
typedef unsigned long long oa_uint64;
typedef signed long long oa_int64;
typedef double oa_double;
typedef oa_uint32 oa_size_t;
typedef int oa_fs_handle;
typedef float oa_float;
//typedef unsigned int oa_evshed_id;
typedef struct oa_lcd_dll_node oa_lcd_dll_node;
struct oa_lcd_dll_node
{
   void            *data;
   oa_lcd_dll_node    *prev;
   oa_lcd_dll_node    *next;
} ;
typedef oa_lcd_dll_node *oa_evshed_id;

typedef oa_uint32 oa_hw_spi_handle;
typedef oa_int32 oa_hw_i2c_handle;

#if 0
/************************************************************/
/************Support variable argument macros and functions********/
/************************************************************/
#ifndef _ARMABI
#define _ARMABI __declspec(__nothrow)

#if defined(__cplusplus) || !defined(__STRICT_ANSI__) || !defined(__size_t)
 /* always defined in C++ and non-strict C for consistency of debug info */
  typedef unsigned int size_t;   /* see <stddef.h> */
  #if !defined(__cplusplus) && defined(__STRICT_ANSI__)
    #define __size_t 1
  #endif
#endif

/* ANSI forbids va_list to be defined here */
/* keep in step with <stdarg.h> and <stdio.h> */
#if defined(__cplusplus) || !defined(__STRICT_ANSI__) || !defined(__va_list_defined)
/* always defined in C++ and non-strict C for consistency of debug info */
  typedef struct __va_list __va_list;
  #if !defined(__cplusplus) && defined(__STRICT_ANSI__)
    #define __va_list_defined 1
  #endif
#endif

/*
 * stdarg.h declares a type and defines macros for advancing through a
 * list of arguments whose number and types are not known to the called
 * function when it is translated. A function may be called with a variable
 * number of arguments of differing types. Its parameter list contains one or
 * more parameters. The rightmost parameter plays a special role in the access
 * mechanism, and will be called parmN in this description.
 */

/* N.B. <stdio.h> is required to declare vfprintf() without defining      */
/* va_list.  Clearly the type __va_list there must keep in step.          */
typedef struct __va_list { void *__ap; } va_list;

#endif

typedef va_list oa_va_list;
#endif

#undef NULL
#define NULL 0

typedef enum 
{
    OA_FALSE,
    OA_TRUE
} oa_bool;

typedef enum 
{
    OA_SOCKET_TCP,
    OA_SOCKET_UDP
} OA_SOCKET_TYPE;

typedef void (* oa_func_ptr)(void *);
typedef void (* oa_func_n_ptr)(void);
typedef void (* oa_uart_rx_cb_fptr)(void *, oa_uint32);
typedef void (* oa_uart_tx_cb_fptr)(void *);
typedef oa_bool (* oa_func_req_tone_ptr)(oa_uint16 playtone);
typedef oa_bool (* oa_func_cust_cmd_ptr)(oa_char*, oa_uint16);
typedef void (* oa_ftp_rsp_cb_fptr)(oa_char *cb_str);
typedef void (* oa_func_at_rsp_ptr)(oa_uint16 len, oa_uint8 *pStr);
typedef void (* oa_func_gps_recv_data_ptr)(oa_uint8 *pStr, oa_uint16 len);
typedef void (* oa_adc_data_cb_fptr)(oa_int32 volt_result, oa_double adc_result);


typedef enum
{
    OA_KEY_DOWN = 0,
    OA_KEY_UP,
    OA_KEY_LONGPRESS,
    OA_KEY_REPEAT,
    OA_KEY_MAX_MODE
} oa_key_mode;

typedef enum
{
    OA_KEY_R0_C0 = 0,
    OA_KEY_R0_C1,
    OA_KEY_R0_C2,
    OA_KEY_R0_C3,
    OA_KEY_R0_C4,
    OA_KEY_R0_C5,
    OA_KEY_R0_C6,
    OA_KEY_R0_C7,
    
    OA_KEY_R1_C0,//8
    OA_KEY_R1_C1,
    OA_KEY_R1_C2,
    OA_KEY_R1_C3,
    OA_KEY_R1_C4,
    OA_KEY_R1_C5,
    OA_KEY_R1_C6,
    OA_KEY_R1_C7,
    
    OA_KEY_R2_C0,//16
    OA_KEY_R2_C1,
    OA_KEY_R2_C2,
    OA_KEY_R2_C3,
    OA_KEY_R2_C4,
    OA_KEY_R2_C5,
    OA_KEY_R2_C6,
    OA_KEY_R2_C7,
    
    OA_KEY_R3_C0,//24
    OA_KEY_R3_C1,
    OA_KEY_R3_C2,
    OA_KEY_R3_C3,
    OA_KEY_R3_C4,
    OA_KEY_R3_C5,
    OA_KEY_R3_C6,
    OA_KEY_R3_C7,
    
    OA_KEY_R4_C0,//32
    OA_KEY_R4_C1,
    OA_KEY_R4_C2,
    OA_KEY_R4_C3,
    OA_KEY_R4_C4,
    OA_KEY_R4_C5,
    OA_KEY_R4_C6,
    OA_KEY_R4_C7,

    OA_KEY_R5_C0,//40
    OA_KEY_R5_C1,
    OA_KEY_R5_C2,
    OA_KEY_R5_C3,
    OA_KEY_R5_C4,
    OA_KEY_R5_C5,
    OA_KEY_R5_C6,
    OA_KEY_R5_C7,
 
    OA_KEY_R6_C0,//48
    OA_KEY_R6_C1,
    OA_KEY_R6_C2,
    OA_KEY_R6_C3,
    OA_KEY_R6_C4,
    OA_KEY_R6_C5,
    OA_KEY_R6_C6,
    OA_KEY_R6_C7,

    OA_KEY_R7_C0,//56
    OA_KEY_R7_C1,
    OA_KEY_R7_C2,
    OA_KEY_R7_C3,
    OA_KEY_R7_C4,
    OA_KEY_R7_C5,
    OA_KEY_R7_C6,
    OA_KEY_R7_C7,
    
    OA_KEY_POWER, //a const with 64

    /*all the keycodes need defined below*/

    OA_KEY_MAX, 
}oa_key;

#define OA_UART_BUFFER_SIZE 512//1024
#define UART3_MAX_SIZE 256
typedef struct
{
	oa_char buf[UART3_MAX_SIZE];
	oa_uint8 len;
}uart_struct;

typedef oa_uint32 oa_uart_baudrate;

typedef enum {
   OA_UART_NULL = 99,
   OA_UART1 = 0,
   OA_UART2,
   OA_UART3,
   OA_UART_AT,  /*virtual port for AT command*/
   OA_UART_USB = 0x05, /*Note : now only use for g_oa_tst_port!!!*/
   OA_UART_NUM
   
} oa_uart_enum;

typedef enum {
      oa_uart_len_5=5,
      oa_uart_len_6,
    oa_uart_len_7,
    oa_uart_len_8
} oa_uart_bits_per_char;

typedef enum {
    oa_uart_sb_1=1,
    oa_uart_sb_2,
    oa_uart_sb_1_5
} oa_uart_stop_bits;

typedef enum {
    oa_uart_pa_none=0,
    oa_uart_pa_odd,
    oa_uart_pa_even,
    oa_uart_pa_space
} oa_uart_parity;

typedef enum {
    oa_uart_fc_none=1,
    oa_uart_fc_hw,
    oa_uart_fc_sw
} oa_uart_flow_ctrl_mode;

typedef struct
{
    oa_uart_baudrate                 baud; 
    oa_uart_bits_per_char         dataBits;
    oa_uart_stop_bits                 stopBits;
    oa_uart_parity                   parity;
    oa_uart_flow_ctrl_mode             flowControl;
      oa_uint8                     xonChar;
      oa_uint8                     xoffChar;
      oa_bool                      DSRCheck;
} oa_uart_struct;

typedef enum {
   OA_TRACE_NULL = 0,       /*no trace*/
   OA_TRACE_USER = 1,       /*OA_DEBUG_USER valid, for oa_app used*/
   OA_TRACE_L1       = 2,       /*OA_DEBUG_L1 valid, event/kernel function call is L1*/
   OA_TRACE_L2       = 4,       /*OA_DEBUG_L2 valid, */
   OA_TRACE_L3       = 8,       /*OA_DEBUG_L3 valid, ftp upgrade log is L3*/
   OA_TRACE_ALL   =0xff
   
} oa_trace_level_enum;

typedef enum {
   OA_TRACE_PORT_NULL = 99, /*no trace port*/
   OA_TRACE_PORT_1 = 0,        /*trace to uart0*/
   OA_TRACE_PORT_2,              /*trace to uart1*/
   OA_TRACE_PORT_3,              /*trace to uart2*/
   OA_TRACE_PORT_FILE,         /*trace to FILE*/
   OA_TRACE_PORT_CATCHER, /*trace to tool catcher*/
   OA_TRACE_PORT_MAX
   
} oa_trace_port_enum;


/*note :oa timer id,user can define 60 timer id at most*/
typedef enum{
    OA_APP_SCHEDULER_ID = 0, 
    OA_TIMER_ID_1,
    OA_TIMER_ID_2,
    OA_TIMER_ID_3,
    OA_TIMER_ID_4,
    OA_TIMER_ID_5,
    OA_TIMER_ID_6,
    OA_TIMER_ID_7,
    OA_TIMER_ID_8,
    OA_TIMER_ID_9,
    OA_TIMER_ID_10,
    OA_TIMER_ID_11,
    OA_TIMER_ID_12,
    OA_TIMER_ID_13,
    OA_TIMER_ID_14,
    OA_TIMER_ID_15,
    OA_TIMER_ID_16,        
    OA_TIMER_ID_17,
    OA_TIMER_ID_18,
    OA_TIMER_ID_19,
    OA_TIMER_ID_20,
#if 0    
    OA_TIMER_ID_21,
    OA_TIMER_ID_22,
    OA_TIMER_ID_23,
    OA_TIMER_ID_24,
    OA_TIMER_ID_25,
    OA_TIMER_ID_26,
    OA_TIMER_ID_27,
    OA_TIMER_ID_28,
    OA_TIMER_ID_29,
    OA_TIMER_ID_30,
    OA_TIMER_ID_31,
    OA_TIMER_ID_32,
    OA_TIMER_ID_33,
    OA_TIMER_ID_34,
    OA_TIMER_ID_35,
    OA_TIMER_ID_36,
    OA_TIMER_ID_37,
    OA_TIMER_ID_38,
    OA_TIMER_ID_39,
    OA_TIMER_ID_40,
    OA_TIMER_ID_41,
    OA_TIMER_ID_42,
    OA_TIMER_ID_43,
    OA_TIMER_ID_44,
    OA_TIMER_ID_45,
    OA_TIMER_ID_46,
    OA_TIMER_ID_47,
    OA_TIMER_ID_48,
    OA_TIMER_ID_49,
    OA_TIMER_ID_50,
    OA_TIMER_ID_51,
    OA_TIMER_ID_52,
    OA_TIMER_ID_53,
    OA_TIMER_ID_54,
    OA_TIMER_ID_55,      
	OA_TIMER_ID_SOC_RECONNECT, 
	
    OA_TIMER_ID_UART1_READ_DELAY,//=91 ,    //reserved  
    OA_TIMER_ID_UART2_READ_DELAY,//=92 ,    //reserved  
    OA_TIMER_ID_UART3_READ_DELAY,//=93 ,    //reserved  
    OA_TIMER_ID_CMD_TIMEOUT,// = 94, //OA_APP_ID_TIMER_MAX is 94,  //reserved
#endif    
    OA_MAX_TIMER
}oa_timer_id;

typedef void* oa_eventid;

typedef enum{
    OA_EVSHED_ID_0 = 0,
    OA_EVSHED_ID_1,
    OA_EVSHED_ID_2,
    OA_EVSHED_ID_3,
    OA_EVSHED_ID_4,
    OA_EVSHED_ID_5,
    OA_EVSHED_ID_6,
    OA_EVSHED_ID_7,
#if 0
    OA_EVSHED_ID_8,
    OA_EVSHED_ID_9,
    OA_EVSHED_ID_10,
    OA_EVSHED_ID_11,
    OA_EVSHED_ID_12,
    OA_EVSHED_ID_13,
    OA_EVSHED_ID_14,
    OA_EVSHED_ID_15,
    OA_EVSHED_ID_16,        
    OA_EVSHED_ID_17,
    OA_EVSHED_ID_18,
    OA_EVSHED_ID_19,
    OA_EVSHED_ID_20,
#endif    
    OA_EVSHED_TIMER
}oa_evshed_enum;

typedef struct
{
    oa_uint8 unused;
} oa_event_scheduler;

typedef struct
{
    oa_event_scheduler *evshedId;
    oa_evshed_enum index;
}oa_evshed_struct;

/*oa buffer*/
typedef struct { 
  oa_uint8 *buf;
  oa_uint32 len;
  oa_uint32 offset;
  oa_uint32 maxLength;
}oa_buffer;

typedef enum
{
   OA_FILE_BEGIN,
   OA_FILE_CURRENT,
   OA_FILE_END
}OA_FILE_SEEK_POS;

/*define timer related */
typedef struct
{
    oa_uint16 nYear;
    oa_uint8 nMonth;
    oa_uint8 nDay;
    oa_uint8 nHour;
    oa_uint8 nMin;
    oa_uint8 nSec;
    oa_uint8 DayIndex; /* 0=Sunday */
}oa_time_struct;

#ifndef ALL_TONE_ENUM
#define ALL_TONE_ENUM
#endif

typedef enum
{
    OA_SRV_PROF_TONE_NONE,                 /* None */
    OA_SRV_PROF_TONE_ERROR,                /* Error tone */
    OA_SRV_PROF_TONE_CONNECT,              /* Connect tone */
    OA_SRV_PROF_TONE_CAMP_ON,              /* Camp on tone */
    OA_SRV_PROF_TONE_WARNING,              /* Warning tone */
    OA_SRV_PROF_TONE_INCOMING_CALL,        /* Incoming call tone */
    OA_SRV_PROF_TONE_INCOMING_CALL_CARD2,  /* Incoming call tone - card2 */
    OA_SRV_PROF_TONE_INCOMING_CALL_CARD3,  /* Incoming call tone - card3 */
    OA_SRV_PROF_TONE_INCOMING_CALL_CARD4,  /* Incoming call tone - card4 */
    OA_SRV_PROF_TONE_VIDEO_CALL,           /* Video call tone */
    OA_SRV_PROF_TONE_VIDEO_CALL_CARD2,     /* Video call tone - card2 */

                                        /*11*/
    OA_SRV_PROF_TONE_VIDEO_CALL_CARD3,     /* Video call tone - card3 */
    OA_SRV_PROF_TONE_VIDEO_CALL_CARD4,     /* Video call tone - card4 */
    OA_SRV_PROF_TONE_SMS,                  /* SMS tone */    
    OA_SRV_PROF_TONE_SMS_CARD2,            /* SMS tone - card2 */
    OA_SRV_PROF_TONE_SMS_CARD3,            /* SMS tone - card3 */
    OA_SRV_PROF_TONE_SMS_CARD4,            /* SMS tone - card4 */
    OA_SRV_PROF_TONE_MMS,                  /* MMS tone */
    OA_SRV_PROF_TONE_MMS_CARD2,            /* MMS tone - card2 */
    OA_SRV_PROF_TONE_MMS_CARD3,            /* MMS tone - card3 */
    OA_SRV_PROF_TONE_MMS_CARD4,            /* MMS tone - card4 */

                                        /*21*/
    OA_SRV_PROF_TONE_EMAIL,                /* Email tone */
    OA_SRV_PROF_TONE_EMAIL_CARD2,          /* Email tone - card2 */
    OA_SRV_PROF_TONE_EMAIL_CARD3,          /* Email tone - card3 */
    OA_SRV_PROF_TONE_EMAIL_CARD4,          /* Email tone - card4 */
    OA_SRV_PROF_TONE_VOICE,                /* Voice tone */
    OA_SRV_PROF_TONE_VOICE_CARD2,          /* Voice tone - card2 */
    OA_SRV_PROF_TONE_VOICE_CARD3,          /* Voice tone - card3 */
    OA_SRV_PROF_TONE_VOICE_CARD4,          /* Voice tone - card4 */
    OA_SRV_PROF_TONE_ALARM,                /* Alarm tone */
    OA_SRV_PROF_TONE_POWER_ON,             /* Power on tone */

                                        /*31*/
    OA_SRV_PROF_TONE_POWER_OFF,            /* Power off tone */
    OA_SRV_PROF_TONE_COVER_OPEN,           /* Cover open tone */
    OA_SRV_PROF_TONE_COVER_CLOSE,          /* Cover close tone */
    OA_SRV_PROF_TONE_SUCCESS,              /* Success tone */
    OA_SRV_PROF_TONE_SAVE,                 /* Save tone */
    OA_SRV_PROF_TONE_EMPTY_LIST,           /* Empty tone */
    OA_SRV_PROF_TONE_GENERAL_TONE,         /* General tone */
    OA_SRV_PROF_TONE_AUX,                  /* AUX tone */
    OA_SRV_PROF_TONE_BATTERY_LOW,          /* Battery low tone */
    OA_SRV_PROF_TONE_BATTERY_WARNING,      /* Battery warning tone */

                                        /*41*/
    OA_SRV_PROF_TONE_CALL_REMINDER,        /* Call reminder tone */
    OA_SRV_PROF_TONE_CCBS,                 /* CCBS tone */
    OA_SRV_PROF_TONE_CONGESTION,           /* Congestion tone */
    OA_SRV_PROF_TONE_AUTH_FAIL,            /* Authentication fail tone */
    OA_SRV_PROF_TONE_NUM_UNOBTAIN,         /* Number un-obtained tone */
    OA_SRV_PROF_TONE_CALL_DROP,            /* Call drop tone */
    OA_SRV_PROF_TONE_SMS_INCALL,           /* SMS in call tone */
    OA_SRV_PROF_TONE_WARNING_INCALL,       /* Warning in call tone */
    OA_SRV_PROF_TONE_ERROR_INCALL,         /* Error in call tone */
    OA_SRV_PROF_TONE_CONNECT_INCALL,       /* Connect in call tone */

                                        /*51*/
    OA_SRV_PROF_TONE_SUCCESS_INCALL,       /* Success in call tone */
    OA_SRV_PROF_TONE_IMPS_CONTACT_ONLINE,          /* IMPS tone - contact online */
    OA_SRV_PROF_TONE_IMPS_NEW_MESSAGE,             /* IMPS tone - new message */
    OA_SRV_PROF_TONE_IMPS_CONTACT_INVITATION,      /* IMPS tone - contact invitation */
    OA_SRV_PROF_TONE_IMPS_CHATROOM_NOTIFICATION,   /* IMPS tone - chat room notification */
    OA_SRV_PROF_TONE_FILE_ARRIVAL,         /* File arrival tone */
    OA_SRV_PROF_TONE_SENT,                 /* Sent tone */
    OA_SRV_PROF_TONE_DELETED,              /* Deleted tone */
    OA_SRV_PROF_TONE_PROBLEM,              /* Problem tone */
    OA_SRV_PROF_TONE_CONFIRM,              /* Confirm tone */

                                        /*61*/
    OA_SRV_PROF_TONE_EXPLICITLY_SAVE,      /* Explicitly save tone */
    OA_SRV_PROF_TONE_NOT_AVAILABLE,        /* Not available tone */
    OA_SRV_PROF_TONE_ANS_PHONE,            /* Answer phone tone */
    OA_SRV_PROF_TONE_WAP_PUSH,             /* WAP push tone */
    OA_SRV_PROF_TONE_REMIND,               /* Remind tone */
    OA_SRV_PROF_TONE_KEYPAD,               /* Key pad tone */
    OA_SRV_PROF_TONE_EM,                   /* Engineering mode tone */
    OA_SRV_PROF_TONE_FM,                   /* Factory mode tone */
    OA_SRV_PROF_TONE_PHONEBOOK,            /* Phonebook tone */
    OA_SRV_PROF_TONE_POC,                  /* PoC tone */

                                        /*71*/
    OA_SRV_PROF_TONE_VOIP,                 /* Voip tone */
    OA_SRV_PROF_TONE_SAT,                  /* SAT tone */
    OA_SRV_PROF_TONE_GSM_BUSY,             /* GSM busy */
    OA_SRV_PROF_TONE_WAITING_CALL,         /* Waiting call */
    OA_SRV_PROF_TONE_ALARM_IN_CALL,        /* Alarm in call */

    OA_NONE_TONE = OA_SRV_PROF_TONE_NONE,
    OA_ERROR_TONE = OA_SRV_PROF_TONE_ERROR,
    OA_CONNECT_TONE = OA_SRV_PROF_TONE_CONNECT,
    OA_CAMP_ON_TONE = OA_SRV_PROF_TONE_CAMP_ON,
    OA_WARNING_TONE = OA_SRV_PROF_TONE_WARNING,
    OA_INCOMING_CALL_TONE = OA_SRV_PROF_TONE_INCOMING_CALL,
    OA_ALARM_TONE = OA_SRV_PROF_TONE_ALARM,
    OA_POWER_ON_TONE = OA_SRV_PROF_TONE_POWER_ON,
    OA_POWER_OFF_TONE = OA_SRV_PROF_TONE_POWER_OFF,
    OA_COVER_OPEN_TONE = OA_SRV_PROF_TONE_COVER_OPEN,
    OA_COVER_CLOSE_TONE = OA_SRV_PROF_TONE_COVER_CLOSE,
    OA_MESSAGE_TONE = OA_SRV_PROF_TONE_SMS,
    OA_KEYPAD_PLAY_TONE = OA_SRV_PROF_TONE_KEYPAD,
    OA_SUCCESS_TONE = OA_SRV_PROF_TONE_SUCCESS,
    OA_SAVE_TONE = OA_SRV_PROF_TONE_SAVE,
    OA_EMPTY_LIST_TONE = OA_SRV_PROF_TONE_EMPTY_LIST,
    OA_GENERAL_TONE = OA_SRV_PROF_TONE_GENERAL_TONE,
    OA_SMS_IN_CALL_TONE = OA_SRV_PROF_TONE_SMS_INCALL,
    OA_AUX_TONE = OA_SRV_PROF_TONE_AUX,
    OA_WARNING_TONE_IN_CALL = OA_SRV_PROF_TONE_WARNING_INCALL,
    OA_ERROR_TONE_IN_CALL = OA_SRV_PROF_TONE_ERROR_INCALL,
    OA_CONNECT_TONE_IN_CALL = OA_SRV_PROF_TONE_CONNECT_INCALL,
    OA_SUCCESS_TONE_IN_CALL = OA_SRV_PROF_TONE_SUCCESS_INCALL,
    OA_BATTERY_LOW_TONE = OA_SRV_PROF_TONE_BATTERY_LOW,
    OA_BATTERY_WARNING_TONE = OA_SRV_PROF_TONE_BATTERY_WARNING,
    OA_CALL_REMINDER_TONE = OA_SRV_PROF_TONE_CALL_REMINDER,
    OA_CCBS_TONE = OA_SRV_PROF_TONE_CCBS,
    OA_CONGESTION_TONE = OA_SRV_PROF_TONE_CONGESTION,
    OA_AUTH_FAIL_TONE = OA_SRV_PROF_TONE_AUTH_FAIL,
    OA_NUM_UNOBTAIN_TONE = OA_SRV_PROF_TONE_NUM_UNOBTAIN,
    OA_CALL_DROP_TONE = OA_SRV_PROF_TONE_CALL_DROP,
    OA_IMPS_CONTACT_ONLINE_TONE = OA_SRV_PROF_TONE_IMPS_CONTACT_ONLINE,
    OA_IMPS_NEW_MESSAGE_TONE = OA_SRV_PROF_TONE_IMPS_NEW_MESSAGE,
    OA_IMPS_CONTACT_INVITATION_TONE = OA_SRV_PROF_TONE_IMPS_CONTACT_INVITATION,
    OA_IMPS_CHATROOM_NOTIFICATION_TONE = OA_SRV_PROF_TONE_IMPS_CHATROOM_NOTIFICATION,
    OA_CARD2_INCOMING_CALL_TONE = OA_SRV_PROF_TONE_INCOMING_CALL_CARD2,
    OA_CARD2_MESSAGE_TONE = OA_SRV_PROF_TONE_SMS_CARD2,
    OA_FILE_ARRIVAL_TONE = OA_SRV_PROF_TONE_FILE_ARRIVAL,

    /*for op11*/
    OA_SENT_TONE = OA_SRV_PROF_TONE_SENT,
    OA_DELETED_TONE = OA_SRV_PROF_TONE_DELETED,
    OA_PROBLEM_TONE = OA_SRV_PROF_TONE_PROBLEM,
    OA_NETWORK_PROBLEM_TONE = OA_SRV_PROF_TONE_PROBLEM,
    OA_CONFIRM_TONE = OA_SRV_PROF_TONE_CONFIRM,
    OA_EXPLICITLY_SAVE_TONE = OA_SRV_PROF_TONE_EXPLICITLY_SAVE,
    OA_NOT_AVAILABLE_TONE = OA_SRV_PROF_TONE_NOT_AVAILABLE,        /*Oops tone*/
    OA_ANS_PHONE_TONE = OA_SRV_PROF_TONE_ANS_PHONE,
    OA_SMS_TONE = OA_SRV_PROF_TONE_SMS,
    OA_MMS_TONE = OA_SRV_PROF_TONE_MMS,
    OA_EMAIL_TONE = OA_SRV_PROF_TONE_EMAIL,
    OA_VOICE_TONE = OA_SRV_PROF_TONE_VOICE,
    OA_WAP_PUSH_TONE = OA_SRV_PROF_TONE_WAP_PUSH,
    OA_REMIND_TONE = OA_SRV_PROF_TONE_REMIND,

    OA_SRV_PROF_TONE_END_OF_ENUM           /* End of enum */
}oa_srv_prof_tone_enum;

typedef enum{
	OA_PWRKEYPWRON = 0,
	OA_CHRPWRON	= 1, /* Charger power on */
	OA_RTCPWRON = 2,
	OA_CHRPWROFF = 3,
	OA_WDTRESET = 4, /*NORMAL*/
	OA_ABNRESET = 5,  /*ABNORMAL RESET*/
	OA_USBPWRON = 6,  
	OA_USBPWRON_WDT = 7,
	OA_PRECHRPWRON = 8,    /* Precharge Power On */
	OA_UNKNOWN_PWRON = 0xF9	
}oa_power_on_enum;

//oa_nw_signal_level, range(0-31,99)
typedef enum
{
   OA_SIGNAL_STRENGTH_NONE=0,
   OA_SIGNAL_STRENGTH_LEVEL1=6,
   OA_SIGNAL_STRENGTH_LEVEL2=9,
   OA_SIGNAL_STRENGTH_LEVEL3=12,
   OA_SIGNAL_STRENGTH_LEVEL4=15,
   OA_SIGNAL_STRENGTH_INVAILD=99
}oa_nw_signal_level;

/*
 * DESCRIPTION
 *  Location information.
 */
typedef struct
{
    oa_char plmn[6 + 1];    /* Network PLMN */
    oa_uint16 lac;                            /* LAC */
    oa_uint16 cell_id;                        /* Cell ID. */
} oa_nw_info_location_info_struct;


typedef enum{
   OA_FTP_FAIL = 0,   //"+FTP:Fail"                           FTP upgrade failed
   OA_FTP_START,     //"+FTP:Start"                         FTP upgrade is start
   OA_FTP_NOT_START,       //"+FTP:Not Start"                   FTP upgrade isn't start
   OA_FTP_PARAM_ERR,       //"+FTP:Param Error"              FTP upgrade string parameter is error
   OA_FTP_TIMER_OUT,       //"+FTP:Timer out"                  FTP upgrade timer out
   OA_FTP_FINISHED,       //"+FTP:Finished"                    FTP upgrade finished   
   OA_FTP_RESTART,      //"+FTP:Restart"                     FTP upgrade finished auto restart    
	OA_FTP_MAX = 0xFF

 }oa_ftp_state_enum;

typedef enum
{
	OA_FTP_FLAG_RSET = 0x00,   //ftp  finished, module auto restart, for ftp upgrade
	OA_FTP_FLAG_NO_RSET = 0x01, //ftp  finished, module don't auto restart, for ftp upgrade
	OA_FTP_FLAG_NOT_DLL = 0x02,  //ftp get file
	//OA_FTP_FLAG_NOT_CRC = 0x04,
	//OA_FTP_FLAG_EN_PCT = 0x08, //enable download percentage
	/*ADD other flag*/
	OA_FTP_FLAG_MAX = 0xFF
}oa_ftp_flag_enum; //ftp flag 

/*For dtmf detect*/
typedef void (* oa_dtmf_callback_fptr)(oa_int16);

#ifndef OPENAT_SOCKET_2
#define OPENAT_SOCKET_2 // support TCP/IP socket api
#endif

#ifdef OPENAT_SOCKET_2

#define OA_MAX_TCPIP_APN_LEN 100
#define OA_MAX_TCPIP_USER_NAME_LEN 32
#define OA_MAX_TCPIP_PASSWORD_LEN 32

#define OA_MAX_SOCK_ADDR_LEN           (64)

typedef void (* oa_gprs_attach_rsp_fptr)(oa_bool);
typedef void (* oa_gprs_detach_rsp_fptr)(oa_bool);

//apn set state, error cause
typedef void (* oa_soc_set_apn_rsp_fptr)(oa_bool);
//result, addr len, addr value, error_cause
typedef void (* oa_soc_gethostbyname_fptr)(oa_bool, oa_uint8, oa_uint8*, oa_int8);

typedef void (* oa_soc_notify_ind_fptr)(void *);

/* <GROUP CallBackFunctions>
   FUNCTION
   SPI_CALLBACK
   DESCRIPTION
   SPI callback function
   SPI callback function is called when SPI interrupt has arrived. It indicates SPI has finished the last data transfer.
   SPI callback function is called in the context of HISR.
*/
typedef void (*oa_spi_callback)(void);

typedef enum
{
    OA_SOC_READ    = 0x01,  /* Notify for read */
    OA_SOC_WRITE   = 0x02,  /* Notify for write */
    OA_SOC_ACCEPT  = 0x04,  /* Notify for accept */
    OA_SOC_CONNECT = 0x08,  /* Notify for connect */
    OA_SOC_CLOSE   = 0x10   /* Notify for close */
} oa_soc_event_enum;

typedef enum
{
  OA_IP_ADDRESS,
  OA_DOMAIN_NAME,
  OA_NUM_OF_ADDR_TYPE
  
}oa_socket_addr_enum;

/* Socket Type */
/*refer to soc_consts.h*/
typedef enum
{
    OA_SOCK_STREAM = 0,  /* stream socket, TCP */
    OA_SOCK_DGRAM,       /* datagram socket, UDP */
    OA_SOCK_SMS,         /* SMS bearer */
    OA_SOCK_RAW          /* raw socket */
} oa_socket_type_enum;


typedef struct
{
   oa_uint8    ref_count;
   oa_uint16   msg_len;	
   oa_int8     socket_id;    /* socket ID */
   oa_uint8    event_type;   /* soc_event_enum */
   oa_bool     result;
   oa_int8     error_cause;  /* used only when EVENT is close/connect refer oa_soc_error_enum*/
   oa_int32    detail_cause; /* refer to ps_cause_enum if error_cause is
                               * SOC_BEARER_FAIL */
} oa_app_soc_notify_ind_struct;

typedef struct 
{
    oa_socket_type_enum	sock_type;
    oa_int16	addr_len;
    oa_uint8	addr[OA_MAX_SOCK_ADDR_LEN];
    oa_uint16	port;
} oa_sockaddr_struct;

//network apn info struct
typedef struct 
{
    oa_uint8 apn[OA_MAX_TCPIP_APN_LEN];
    oa_uint8 userName[OA_MAX_TCPIP_USER_NAME_LEN];
    oa_uint8 password[OA_MAX_TCPIP_PASSWORD_LEN];
    oa_bool use_password;
}oa_soc_apn_info_struct;


/*refer to soc_consts.h*/
typedef enum
{
    OA_SOC_SUCCESS           = 0,
    OA_SOC_ERROR             = -1,
    OA_SOC_WOULDBLOCK        = -2,
    OA_SOC_LIMIT_RESOURCE    = -3,    /* limited resource */
    OA_SOC_INVALID_SOCKET    = -4,    /* invalid socket */
    OA_SOC_INVALID_ACCOUNT   = -5,    /* invalid account id */
    OA_SOC_NAMETOOLONG       = -6,    /* address too long */
    OA_SOC_ALREADY           = -7,    /* operation already in progress */
    OA_SOC_OPNOTSUPP         = -8,    /* operation not support */
    OA_SOC_CONNABORTED       = -9,    /* Software caused connection abort */
    OA_SOC_INVAL             = -10,   /* invalid argument */
    OA_SOC_PIPE              = -11,   /* broken pipe */
    OA_SOC_NOTCONN           = -12,   /* socket is not connected */
    OA_SOC_MSGSIZE           = -13,   /* msg is too long */
    OA_SOC_BEARER_FAIL       = -14,   /* bearer is broken */
    OA_SOC_CONNRESET         = -15,   /* TCP half-write close, i.e., FINED */
    OA_SOC_DHCP_ERROR        = -16,
    OA_SOC_IP_CHANGED        = -17,
    OA_SOC_ADDRINUSE         = -18,
    OA_SOC_CANCEL_ACT_BEARER = -19    /* cancel the activation of bearer */
} oa_soc_error_enum;
#endif/*OPENAT_SOCKET_2*/

//typedef nvram_ef_oa_reserve_struct oa_reserve_struct;
typedef struct
{
  oa_uint8   buf[2000];  /*buf size according NVRAM_MAX_OA_RESERVE_SIZE*/
} oa_reserve_struct;


//typedef oa_sram_noinit_struct point to SRAM buffer no initialize for openat user;
typedef struct
{
  oa_uint8   buf[1024];  /*buf size 1024*/
} oa_sram_noinit_struct;

typedef enum
{
    OA_CHSET_BASE = 0x0,//CHSET_BASE,
    OA_CHSET_BIG5 = 0x15,//CHSET_BIG5,
    OA_CHSET_GB2312 = 0x16,//CHSET_GB2312,
    OA_CHSET_UTF8 = 0x1e,
    OA_CHSET_UCS2 = 0x1f,//CHSET_UCS2,
    OA_CHSET_ASCII =0x20,
    OA_CHSET_TOTAL = 0x21
} oa_chset_enum;

typedef enum {
    OA_AUD_PATH_NORMAL   =0x0 , /* Normal path */
    //oa_AUD_PATH_HEADSET     , /* HeadSet (Earphone) path */
    OA_AUD_PATH_LOUDSPK = 0x02    , /* Loudspeaker path */
    OA_MAX_AUD_PATH_NUM     
}oa_aud_path_enum;

/* Define the audio device type */
typedef enum {
    OA_AUDIO_DEVICE_MIC1 = 0 ,    /* ME */
    OA_AUDIO_DEVICE_MIC2     ,    /* Headset */
    OA_AUDIO_DEVICE_FMRR          /* FM Radio */
}oa_audio_input_path_enum;

typedef enum
{
    //OA_ALERT_NONE,
    OA_RING = 0x01,
    //OA_VIBRATION_ONLY,
    //OA_VIBRATION_AND_RING,
    //OA_VIBRATION_THEN_RING,
    OA_SILENT = 0x05
} oa_alert_enum;


typedef enum
{
    OA_MDI_AUDIO_PLAY_CRESCENDO = 0,  /* Play sound for crescendo. */
    OA_MDI_AUDIO_PLAY_INFINITE,             /* Play sound for infinite. */
    OA_MDI_AUDIO_PLAY_ONCE,                   /* Play sound for once. */
    OA_MDI_AUDIO_PLAY_DESCENDO,           /* Play sound for descendo. */
    OA_MDI_NO_OF_PLAY_STYLE
}oa_mdi_play_style;

/* The enum should be the same as "med_main.h" */
#define OA_MDI_AUDIO_SUCCESS               0
#define OA_MDI_AUDIO_FAIL                  -9001
#define OA_MDI_AUDIO_BUSY                  -9002
#define OA_MDI_AUDIO_DISC_FULL             -9003
#define OA_MDI_AUDIO_OPEN_FILE_FAIL        -9004
#define OA_MDI_AUDIO_END_OF_FILE           -9005
#define OA_MDI_AUDIO_TERMINATED            -9006   /* Only used in MMI */
#define OA_MDI_AUDIO_BAD_FORMAT            -9007   /* Error from aud driver */
#define OA_MDI_AUDIO_INVALID_FORMAT        -9008   /* Error from MED */
#define OA_MDI_AUDIO_ERROR                 -9009
#define OA_MDI_AUDIO_NO_DISC               -9010
#define OA_MDI_AUDIO_NO_SPACE              -9011   /* MED audio not used */
#define OA_MDI_AUDIO_INVALID_HANDLE        -9012   /* MMI not used */
#define OA_MDI_AUDIO_NO_HANDLE             -9013   /* MMI not used */
#define OA_MDI_AUDIO_RESUME                -9014   /* Only used in MMI */
#define OA_MDI_AUDIO_BLOCKED               -9015   /* Only used in MMI */
#define OA_MDI_AUDIO_MEM_INSUFFICIENT      -9016
#define OA_MDI_AUDIO_BUFFER_INSUFFICIENT   -9017
#define OA_MDI_AUDIO_FILE_EXIST            -9018   /* MMI not used */
#define OA_MDI_AUDIO_WRITE_PROTECTION      -9019
#define OA_MDI_AUDIO_PARAM_ERROR           -9020
#define OA_MDI_AUDIO_UNSUPPORTED_CHANNEL   -9021
#define OA_MDI_AUDIO_UNSUPPORTED_FREQ      -9022
#define OA_MDI_AUDIO_UNSUPPORTED_TYPE      -9023
#define OA_MDI_AUDIO_UNSUPPORTED_OPERATION -9024
#define OA_MDI_AUDIO_PARSER_ERROR          -9025

#define OA_MDI_AUDIO_AUDIO_ERROR           -9027
#define OA_MDI_AUDIO_MP4_NO_AUDIO_TRACK    -9032
#define OA_MDI_AUDIO_STOP_FM_RECORD        -9065   /* Only used in MMI */
#define OA_MDI_AUDIO_UNSUPPORTED_SPEED     -9066
#define OA_MDI_AUDIO_DECODER_NOT_SUPPORT   -9101
#define OA_MDI_AUDIO_DEMO_END              -9116
#define OA_MDI_AUDIO_HFP_SCO_CONNECTED     -9200   /* Only used in MMI */
#define OA_MDI_AUDIO_DRM_PROHIBIT          -9201   /* Only used in MMI */
#define OA_MDI_AUDIO_DRM_TIMEOUT           -9202   /* Only used in MMI */

//media play callback function, result refer OA_MDI_AUDIO_xxx
typedef void (*oa_mdi_callback) (oa_int32 result);

/*
 *	TTS player ERROR CODES
 */
typedef oa_uint16 oa_ttserr_id;

#define OA_TTS_ERR_OK					0x0000					/* success */
#define OA_TTS_ERR_FAILED				0xFFFF					/* failed */
#define OA_TTS_STATE_BASE				0x0100					/* state base */
#define OA_TTS_STATE_INVALID_DATA		OA_TTS_STATE_BASE + 2	/* invalid data */
#define OA_TTS_STATE_TTS_STOP			OA_TTS_STATE_BASE + 3	/* TTS stop */
#define OA_TTS_ERR_BASE					0x8000					/* error number base */
#define OA_TTS_ERR_UNIMPEMENTED			OA_TTS_ERR_BASE + 0		/* unimplemented function */
#define OA_TTS_ERR_UNSUPPORTED			OA_TTS_ERR_BASE + 1		/* unsupported on this platform */
#define OA_TTS_ERR_INVALID_HANDLE		OA_TTS_ERR_BASE + 2		/* invalid handle */
#define OA_TTS_ERR_INVALID_PARAMETER		OA_TTS_ERR_BASE + 3		/* invalid parameter(s) */
#define OA_TTS_ERR_INSUFFICIENT_HEAP		OA_TTS_ERR_BASE + 4		/* insufficient heap size  */
#define OA_TTS_ERR_STATE_REFUSE			OA_TTS_ERR_BASE + 5		/* refuse to do in current state  */
#define OA_TTS_ERR_INVALID_PARAM_ID		OA_TTS_ERR_BASE + 6		/* invalid parameter ID */
#define OA_TTS_ERR_INVALID_PARAM_VALUE	OA_TTS_ERR_BASE + 7		/* invalid parameter value */
#define OA_TTS_ERR_SIZE_EXCEED_BUFFER	OA_TTS_ERR_BASE + 8		/* get data size exceed the data buffer */
#define OA_TTS_ERR_END_OF_INPUT			OA_TTS_ERR_BASE + 9		/* end of input stream */

/*oa event id*/
typedef enum{
    OA_EVENT_ID_0 = 0,
    OA_EVENT_ID_1,
    OA_EVENT_ID_2,
    OA_EVENT_ID_3,   
    OA_EVENT_ID_4,
    OA_EVENT_ID_5,
    OA_EVENT_ID_6,
    OA_EVENT_ID_7,
    OA_EVENT_ID_8,
    OA_EVENT_ID_9,
    OA_EVENT_ID_10,
    OA_EVENT_ID_11,
    OA_EVENT_ID_12,
    OA_EVENT_ID_13,
    OA_EVENT_ID_14,
    OA_EVENT_ID_15,

    OA_EVENT_MAX_NUM
}oa_event_id;

/************************************************************/
/**********sms return result********************/
/************************************************************/
typedef enum
{
    OA_SMS_OK          = 0,

    /* Reference TS 27.005 Clause 3.2.5, For AT Command and MMI */
    OA_SMS_ME_FAILURE        = 300,      /* ME failure */
    OA_SMS_OP_NOT_ALLOWED    = 302,      /* Operation not allowed */
    OA_SMS_OP_NOT_SUPPORTED  = 303,      /* Operation not supported */
    OA_SMS_INVALID_PDU_PARA  = 304,      /* Invalid PDU mode parameter */
    OA_SMS_INVALID_TEXT_PARA = 305,      /* Invalid text mode parameter */
    OA_SMS_SIM_NOT_INSERTED  = 310,      /* (U)SIM not inserted */
    OA_SMS_SIM_FAILURE       = 313,      /* (U)SIM failure */
    OA_SMS_MEMORY_FAILURE    = 320,      /* Memory failure */
    OA_SMS_INVALID_MEM_INDEX = 321,      /* Invalid memory index */
    OA_SMS_MEMFULL          = 322,      /* Memory full */
    OA_SMS_SCA_UNKNOWN       = 330,      /* SMSC address unknown */

    /* MMI Defined Error Codes */
    OA_SMS_NOTREADY         = 341,      /* SMS servicer not ready */
    OA_SMS_SEND_BUSY,                    /* SMS servicer busy for sending SMS */
    OA_SMS_SEND_ABORT,                   /* Send abort */
    OA_SMS_MEMINSUFFICIENT,             /* Memory insufficient */
    OA_SMS_DATAINVALID,                 /* SMS content invalid*/
    OA_SMS_DATAEXCEED,                  /* SMS content exceed*/
    OA_SMS_NUMEMPTY,                 /* SMS number empty*/
    OA_SMS_NUMINVALID,               /* SMS number invalid */
    OA_SMS_SC_EMPTY,                     /* SMS service center is empty*/
    OA_SMS_SC_INVALID,                   /* SMS service center invalid*/
    OA_SMS_FDL_FAIL,                     /* FDN check failed */
    OA_SMS_AT_CONFLICT,                  /* AT commond conflict */
    OA_SMS_OP_CONFLICT,                  /* Operation conflict */
    OA_SMS_EMAIL_NO_SPACE,               /* No space to handle email SMS */
    OA_SMS_FS_ERROR,                     /* File system error */
    OA_SMS_IN_USB_MODE,                  /* USB mode error */
    OA_SMS_ARCH_FILE_NOT_EXIST,          /* Archive file not exist */
    OA_SMS_ABORT,                        /* Abort an action */

    OA_SMS_CB_NOT_READY,                 /* Cell Broadcast service not ready */
    OA_SMS_CB_CHNL_DUPLICATED,           /* Duplicated channel */
    OA_SMS_CB_CHNL_NOT_EXISTED,          /* Channel not exist */
    OA_SMS_CB_CHNL_FULL,                 /* Channel full */
    OA_SMS_CB_LANG_NOT_EXISTED,          /* Language not exist */

    OA_SMS_ERROR     = 500       /* Unknown error */
    
} os_sms_result;

typedef enum
{
   OA_SMSAL_DEFAULT_DCS        = 0x00,  /* GSM 7-bit */
   OA_SMSAL_8BIT_DCS           = 0x04,  /* 8-bit */
   OA_SMSAL_UCS2_DCS           = 0x08,  /* UCS2 */  
   OA_SMSAL_RESERVED_DCS       = 0x0c   /* reserved alphabet,
                                        currently, MMI shall display "not support alphabet" 
                                        or "cannot display" when receive dcs indicated this value */ 
} oa_smsal_dcs_enum;

typedef void (* oa_sms_send_req_callback_ptr)(os_sms_result result);
typedef oa_bool (* oa_sms_rcv_ind_callback_ptr)(oa_char * deliver_num, oa_char * timestamp, oa_uint8 * data, oa_uint16 len,oa_smsal_dcs_enum dcs);


/************************************************************/
/**********  Battery              **** **** ************************/
/************************************************************/
typedef enum
{
   OA_VBAT_STATUS,    /* Notify the battery voltage, BMT_VBAT_STATUS */
   OA_CHARGER_IN,     /* Charger plug in, BMT_CHARGER_IN */
   OA_CHARGER_OUT,    /* Charger plug out, BMT_CHARGER_OUT */
   OA_OVERVOLPROTECT,    /* The voltage of battery is too high. BMT_OVERVOLPROTECT */
   OA_OVERBATTEMP,    /* The temperature of battery is too high. BMT_OVERBATTEMP */
   OA_LOWBATTEMP,     /* The temperature of battery is too low. BMT_LOWBATTEMP */
   OA_OVERCHARGECURRENT, /* Charge current is too large. BMT_OVERCHARGECURRENT */
   OA_CHARGE_COMPLETE,   /* Charge is completed. BMT_CHARGE_COMPLETE */
   OA_INVALID_BATTERY,    /* invalid battery  BMT_INVALID_BAT*/
   OA_INVALID_CHARGER,   /* invalid charger BMT_INVALID_CHARGER*/
   OA_CHARGING_TIMEOUT, /* Bad battery BMT_CHARGE_TIMEOUT */ 
   OA_LOWCHARGECURRENT, /* Charge current is too Low. BMT_LOWCHARGECURRENT */
   OA_CHARGE_BAD_CONTACT, /* Charger Bad Contact */
   OA_BATTERY_BAD_CONTACT, /* Battery Bad Contact */
   OA_USB_CHARGER_IN,   /* Usb Charger plug in */
   OA_USB_CHARGER_OUT,   /* Usb Charger plug out */
   OA_USB_NO_CHARGER_IN,
   OA_USB_NO_CHARGER_OUT,
   OA_PMIC_BATTERY_IN,
   OA_PMIC_BATTERY_OUT,
   OA_MAX_BATTERY_STATUS
}oa_battery_status;

typedef enum 
{
   OA_BATTERY_LOW_POWEROFF = 0,
   OA_BATTERY_LOW_TX_PROHIBIT,
   OA_BATTERY_LOW_WARNING,
   OA_BATTERY_LEVEL_0,
   OA_BATTERY_LEVEL_1,
   OA_BATTERY_LEVEL_2,
   OA_BATTERY_LEVEL_3, 
   OA_BATTERY_LEVEL_4 = OA_BATTERY_LEVEL_3, /* OA_BATTERY_LEVEL_4 */
   OA_BATTERY_LEVEL_5 = OA_BATTERY_LEVEL_3, /* OA_BATTERY_LEVEL_5 */
   OA_BATTERY_LEVEL_6 = OA_BATTERY_LEVEL_3, /* OA_BATTERY_LEVEL_6 */
   OA_BATTERY_LEVEL_LAST = 9

} oa_battery_level;

typedef enum
{
    OA_ADC_RAW_DATA = 0,
    OA_ADC_TRANSFORM_INTO_VOLT,
    OA_ADC_TRANSFORM_INTO_CURR,
    OA_ADC_TRANSFORM_INTO_TEMP,
    OA_ADC_TRANSFORM_MAX
}oa_adc_mode;


/************************************************************/
/**********  GPIO Control Register            **** ********************/
/************************************************************/
//#define OA_GPIO_base                (0x80120000)   /* GPIO */
#define OA_GPIO_base                (0x80020000) //52 
#define OA_GPIO_DIR1_SET        (OA_GPIO_base+0x04)    
#define OA_GPIO_DIR1_CLR        (OA_GPIO_base+0x08)  
#define OA_GPIO_DOUT1_SET     (OA_GPIO_base+0x00f0+0x04) 
#define OA_GPIO_DOUT1_CLR     (OA_GPIO_base+0x00f0+0x08)   
#define OA_GPIO_DIN1       	       (OA_GPIO_base+0x0140)


#define OA_GPIO_DIR_OUT(port) (*(volatile oa_uint16 *)(OA_GPIO_DIR1_SET+(0x10*(port/16)))=(oa_uint16)(1<<(port%16)))
#define OA_GPIO_DIR_IN(port)    (*(volatile oa_uint16 *)(OA_GPIO_DIR1_CLR+(0x10*(port/16)))=(oa_uint16)(1<<(port%16)))

#define OA_GPIO_WRITE_HIGH(port)   (*(volatile oa_uint16 *)(OA_GPIO_DOUT1_SET+(0x10*(port/16)))=(oa_uint16)(1<<(port%16)))
#define OA_GPIO_WRITE_LOW(port)    (*(volatile oa_uint16 *)(OA_GPIO_DOUT1_CLR+(0x10*(port/16)))=(oa_uint16)(1<<(port%16)))

#define OA_GPIO_READ(port)   ((*(volatile oa_uint16 *)(OA_GPIO_DIN1+(0x10*(port/16))))&(1 <<(port%16)))>>(port%16)


/************************************************************/
/**********  Eint            ***************************************/
/************************************************************/
#define OA_EINT_NUMBER_MAX 4
typedef enum
{
    OA_EINT_NO_0 = 0,
    OA_EINT_NO_1,
    OA_EINT_NO_2,
    OA_EINT_NO_3,

    OA_EINT_NO_4,
    OA_EINT_NO_5,
    OA_EINT_NO_6,
    OA_EINT_NO_7,

    OA_EINT_NO_MAX
}oa_eint_enum;


typedef enum
{
    OA_CALL_IDLE = 0,
    OA_CALL_OUTGOING = 0x01		/* Outgoing state */,
    OA_CALL_INCOMING = 0x02		/* Incoming state */,
    OA_CALL_ACTIVE = 0x04		/* Active state */,
    OA_CALL_HOLD = 0x08			/* Hold state */,
    OA_CALL_STATE_END_OF_ENUM 	/* End of the enum */
} oa_call_state;


typedef struct 
{
    oa_func_n_ptr main_entry;
    oa_func_n_ptr hw_init;
    oa_trace_level_enum trace_level;
    oa_trace_port_enum trace_port;
    oa_uart_enum ps_port;
    oa_uart_enum tst_port;
    oa_uint32 api_check_sum;
    oa_char* verSr;
    oa_uint32 reserve_a;
    oa_uint32 reserve_b;
    oa_uint32 reserve_c;
    oa_uint32 reserve_d;
}oa_user_dll_dispatch_sys_struct;


/* HW SPI result enum.
*/
typedef enum
{
    /* Indicates no error occurs in the last operation. */
    OA_SPI_RESULT_OK,
    
    /* Indicates some errors have occured in the last operation. */
     OA_SPI_RESULT_ERROR,

    /* Indicates some errors have occured in the last operation. */
     OA_SPI_RESULT_INVALID_ARGUMENT,  
    
    /* Indicates the function is not supported on the current platform. */
     OA_SPI_RESULT_NOT_SUPPORTED
}  oa_spi_result;

/*HW SPI clock polarity definition. */
typedef enum
{
    /* CPOL = 0. */
    OA_SPI_CPOL_B0 = 0,
    /* CPOL = 1. */
    OA_SPI_CPOL_B1 = 1
} oa_spi_cpol_e;

/*HW SPI clock format definition. */
typedef enum
{
    /* CPHA = 0. */
    OA_SPI_CPHA_B0 = 0,
    /* CPHA = 1. */
    OA_SPI_CPHA_B1 = 1
} oa_spi_cpha_e;

/*HW SPI transimssion bit order definition. */
typedef enum
{
    /* Transfer LSB first. */
    OA_SPI_MSBF_LSB = 0,
    /* Transfer MSB first. */
    OA_SPI_MSBF_MSB = 1
} oa_spi_msbf_e;


/* HW SPI data transfer byte order definition. */
typedef enum
{
    /* Use little endian. */
    OA_SPI_ENDIAN_LITTLE = 0,
    /* Use big endian. */
    OA_SPI_ENDIAN_BIG = 1
} oa_spi_endian_e;

/************************************
  *HW SPI configuration parameters.
  * 
  * Remarks
  * <img name="spi_timing_diagram" />
  ************************************/
typedef struct
{
    /* CS setup time. Unit in count of SPI base clock. Range(0-255).
       The chip select setup time = (cs_setup_time+1)*CLK_PERIOD, where CLK_PERIOD is the cycle time of the clock the SPI engine adopts. */
    oa_uint32 cs_setup_time;
    /* CS hold time. Unit in count of SPI base clock. Range(0-255).
       The chip select hold time = (cs_hold_time+1)*CLK_PERIOD. */
    oa_uint32 cs_hold_time;
    /* CS idle time. Unit in count of SPI base clock. Range(0-255).
       The chip select idle time between consecutive transaction = (cs_idle_time+1)*CLK_PERIOD. */
    oa_uint32 cs_idle_time;
    /* SCK clock low time. Unit in count of SPI base clock. Range(0-255).
       The SCK clock low time = (clk_low_time+1)*CLK_PERIOD. */
    oa_uint32 clk_low_time;
    /* SCK clock high time. Unit in count of SPI base clock. Range(0-255).
       The SCK clock high time = (clk_high_time+1)*CLK_PERIOD. */
    oa_uint32 clk_high_time;
    /* Bit order setting for SPI output. */
    oa_spi_msbf_e tx_msbf;
    /* Bit order setting for SPI input. */
    oa_spi_msbf_e rx_msbf;
    /* Byte order setting for SPI output. */
    oa_spi_endian_e tx_endian;
    /* Byte order setting for SPI input. */
    oa_spi_endian_e rx_endian;
    /* SPI clock polarity. */
    oa_spi_cpol_e clk_polarity;
    /* SPI clock format. */
    oa_spi_cpha_e clk_fmt;
} oa_spi_config_param_t;

/*************************************************************************************************************************************
  * SPI IOCTL code definition.
  * For detail description and usage of SPI control codes, please refer to <xref target="SPI HAL IOCTL Code" text="SPI IOCTL Code" />.
  *************************************************************************************************************************************/
typedef enum
{
    /****************************************************************************************************************
      * Get the current SPI configuration parameters.
      * For detail description and usage of this control code, please refer to <xref target="SPI_IOCTL_GET_PARAM" />.
      ****************************************************************************************************************/
    OA_SPI_IOCTL_GET_PARAM,
    /****************************************************************************************************************
      * Get the capability of current SPI hardware.
      * For detail description and usage of this control code, please refer to <xref target="SPI_IOCTL_QUERY_CAPABILITY" />.
      ****************************************************************************************************************/
    OA_SPI_IOCTL_QUERY_CAPABILITY,
    /*************************************************************************************************************************
      * Query supported SPI modes.
      * For detail description and usage of this control code, please refer to <xref target="SPI_IOCTL_QUERY_MODE_SUPPORT" />.
      *************************************************************************************************************************/
    OA_SPI_IOCTL_QUERY_MODE_SUPPORT,
    /******************************************************************************************************************
      * Query SPI base clock.
      * For detail description and usage of this control code, please refer to <xref target="SPI_IOCTL_QUERY_CLOCK" />.
      ******************************************************************************************************************/
    OA_SPI_IOCTL_QUERY_CLOCK,
    /***************************************************************************************************************
      * Set and configure SPI mode.
      * For detail description and usage of this control code, please refer to <xref target="SPI_IOCTL_SET_MODE" />.
      ***************************************************************************************************************/
    OA_SPI_IOCTL_SET_MODE,
    /***************************************************************************************************************
      * Get SPI mode settings.
      * For detail description and usage of this control code, please refer to <xref target="SPI_IOCTL_GET_MODE" />.
      ***************************************************************************************************************/
    OA_SPI_IOCTL_GET_MODE,
    /***************************************************************************************************************
      * Get SPI driving current.
      * For detail description and usage of this control code, please refer to <xref target="SPI_IOCTL_GET_DRIVING_CURRENT" />.
      ***************************************************************************************************************/
    OA_SPI_IOCTL_GET_DRIVING_CURRENT,
    /***************************************************************************************************************
      * Set SPI driving current.
      * For detail description and usage of this control code, please refer to <xref target="SPI_IOCTL_SET_DRIVING_CURRENT" />.
      ***************************************************************************************************************/
    OA_SPI_IOCTL_SET_DRIVING_CURRENT
} oa_spi_ioctl_code;

/*HW SPI mode. Including special modes provided by SPI controller.
*/
typedef enum
{
    /* Deassert mode. SPI CS pin will be pulled low and high for each byte during transmission. */
    OA_SPI_MODE_DEASSERT,
    /* Pause mode. SPI CS pin is pulled low and keeps until specific amount of transfers have been finished. */
    OA_SPI_MODE_PAUSE,
    /* Ultra high mode. Raise DMA priority during DMA transmission. */
    OA_SPI_MODE_ULTRA_HIGH,
    /* Slow down mode. Slow down SPI DMA speed during DMA transmission. */
    OA_SPI_MODE_SLOW_DOWN,
    /* Get tick delay mode. This mode is used to tuning SPI timing. */
    OA_SPI_MODE_GET_TICK
} oa_spi_hal_mode;

/********************************************************************
  * This structure is used as the parameter of OA_SPI_IOCTL_SET_MODE and OA_SPI_IOCTL_GET_MODE.
  ********************************************************************/
typedef struct
{
    /* [IN] Specify a SPI mode. */
    oa_spi_hal_mode mode;
    /* [IN/OUT] Specify whether the mode is enabled.
       For SPI_IOCTL_SET_MODE, it is an input parameter.
       For SPI_IOCTL_GET_MODE, it is an output parameter.
    */
    oa_bool bEnable;
    /* [IN/OUT] The parameter for the specific mode.
       The meaning of this parameter depends on the mode to be set/get.
    */
    oa_uint32 Param;
} oa_spi_mode_t;


#define oa_hw_i2c_handle_none    (0)
#define oa_hw_i2c_handle_invalid    (-1)
#define oa_hw_i2c_handle_occupied    (-2)

typedef oa_int32 oa_hw_i2c_status;
typedef enum
{
   OA_STATUS_OK = 0,
   OA_STATUS_FAIL = -1,
   OA_STATUS_INVALID_CMD = -2,
   OA_STATUS_UNSUPPORTED = -3,
   OA_STATUS_NOT_OPENED = -4,
   OA_STATUS_INVALID_EVENT = -5,
   OA_STATUS_INVALID_DCL_HANDLE = -6,
   OA_STATUS_INVALID_CTRL_DATA = -7,
   OA_STATUS_INVALID_CONFIGURATION = -8,
   OA_STATUS_INVALID_ARGUMENT = -9,
   OA_STATUS_ERROR_TIMEOUT = -10,
   OA_STATUS_ERROR_CRCERROR = -11,
   OA_STATUS_ERROR_READONLY = -12,
   OA_STATUS_ERROR_WRONG_STATE = -13,
   OA_STATUS_INVALID_DEVICE = -14,
   OA_STATUS_ALREADY_OPENED = -15,
   OA_STATUS_SET_VFIFO_FAIL = -16,
   OA_STATUS_INVALID_OPERATION = -17,
   OA_STATUS_DEVICE_NOT_EXIST = -18,

   //*/ I2C DCL added /*//
   OA_STATUS_DEVICE_NOT_SUPPORT_DMA = -19,
   OA_STATUS_DEVICE_IS_BUSY = -20,
   OA_STATUS_ACKERR = -21, 
   OA_STATUS_HS_NACKERR = -22,

   OA_STATUS_BUFFER_EMPTY = 1
} oa_dcl_status_t;

 /*HW I2C Transaction Mode*/
typedef enum
{
   OA_DCL_I2C_TRANSACTION_FAST_MODE, /* Fast Mode: < 400kbps */
   OA_DCL_I2C_TRANSACTION_HIGH_SPEED_MODE /* Hign Speed Mode: > 400kbps */
}oa_dcl_i2c_transaction_mode;

/*HW DCL I2C configure structure */
typedef struct
{
   oa_uint8 eOwner; /* The owner of I2C, defined in DCL_I2C_OWNER */
   oa_int32 fgGetHandleWait; /* 1: wait until I2C is idle; 0: do not wait */
   oa_uint8 u1SlaveAddress; /* Slave address */
   oa_uint8 u1DelayLen; /* Wait delay between consecutive transfers (the unit is half pulse width) */
   oa_dcl_i2c_transaction_mode eTransactionMode; /* Fast mode or high speed mode */
   oa_uint32 u4FastModeSpeed; /* The transfer speed under fast mode. But even under high speed mode, you should alse configure this parameter */
   oa_uint32 u4HSModeSpeed; /* The transfer speed under high speed mode */
   oa_int32 fgEnableDMA; /* 1: use DMA to transfer data; 0: do not use DMA. Note: Make sure driver support DMA mode before you use it. */
} oa_i2c_config_t;


typedef oa_uint32 oa_hw_i2c_cmd;
typedef enum
{
   OA_I2C_CMDS_START = 0x900,
   OA_I2C_CMD_GET_TRANSACTION_MODE, /* Get transaction mode */
   OA_I2C_CMD_SET_TRANSACTION_SPEED, /* Set transaction speed */
   OA_I2C_CMD_SINGLE_WRITE, /* Single write of blocking mode */
   OA_I2C_CMD_SINGLE_READ, /* Single read of blocking mode */
   OA_I2C_CMD_CONT_WRITE, /* Continue write of blocking mode */
   OA_I2C_CMD_CONT_READ, /* Continue read of blocking mode */
   OA_I2C_CMD_WRITE_AND_READ, /* Write and read of blocking mode */
   OA_I2C_CMD_BATCH_TRANSACTION, /* Batch transaction which support non-blocking mode */
   OA_I2C_CMD_GET_BATCH_TRANSACTION_RESULT, /* Query the prior batch transaction result */
   OA_I2C_CMD_GET_DMA_PROPERTY, /* Get the DMA property which driver support */
   OA_I2C_CMD_ENABLE_DMA, /* Use DMA to transfer data. Note: Before you do this, please make sure that driver support DMA mode */
   OA_I2C_CMD_SET_SLAVE_ADDRESS /* Set the slave address of sensor */
}oa_dcl_i2c_ctrl_cmd_t;

/* DMA property that driver support, for user query */
typedef enum
{
   OA_DCL_I2C_DMA_NOT_SUPPORT, /* Driver do not support DMA */
   OA_DCL_I2C_DMA_DUAL_CHANNEL, /* Driver support dual channel DMA mode */
   OA_DCL_I2C_DMA_SINGLE_CHANNEL /* Driver support single channel DMA mode, not support WRITE_AND_READ when DMA is enabled */
}oa_dcl_i2c_dma_property;

/* Transaction Type, for batch transaction use*/
typedef enum
{
	OA_DCL_I2C_TRANSACTION_WRITE, /* Single write */
	OA_DCL_I2C_TRANSACTION_READ, /* Single read */
	OA_DCL_I2C_TRANSACTION_CONT_WRITE, /* Continue write */
	OA_DCL_I2C_TRANSACTION_CONT_READ, /* Continue read */
	OA_DCL_I2C_TRANSACTION_WRITE_AND_READ /* Write and read. Note: This command can not be used, when using DMA to transfer but driver can only support OA_DCL_I2C_DMA_SINGLE_CHANNEL. */
}oa_dcl_i2c_transaction_type;

/* Transaction result, for batch transaction query */
typedef enum
{
   OA_DCL_I2C_BATCH_TRANSACTION_SUCCESS, /* The prior batch transaction is successfully transmitted */
   OA_DCL_I2C_BATCH_TRANSACTION_FAIL, /* Something wrong was happened during the prior batch transaction */
   OA_DCL_I2C_BATCH_TRANSACTION_IS_BUSY /* I2C bus is occupied now */
}oa_dcl_i2c_batch_transaction_result;

/* For I2C_CMD_GET_TRANSACTION_MODE command. */
typedef struct
{
   oa_dcl_i2c_transaction_mode eTransactionMode; /* Transaction mode */
}oa_i2c_ctrl_get_transaction_mode_t;
/* For OA_I2C_CMD_SET_TRANSACTION_SPEED command. */
typedef struct
{
   oa_dcl_i2c_transaction_mode eTransactionMode; /* Transaction mode */
   oa_uint32 u4FastModeSpeed; /* Fast mode speed */
   oa_uint32 u4HSModeSpeed; /* High speed mode speed */
}oa_i2c_ctrl_set_transaction_speed_t;
/* For OA_I2C_CMD_SINGLE_WRITE, OA_I2C_CMD_SINGLE_READ command. */
typedef struct
{
   oa_uint8 *pu1Data; /* Pointer to the buffer of data */
   oa_uint32 u4DataLen; /* Data length */
}oa_i2c_ctrl_single_write_t,oa_i2c_ctrl_single_read_t;
/* For OA_I2C_CMD_CONT_WRITE, OA_I2C_CMD_CONT_READ command. */
typedef struct
{
   oa_uint8 *pu1Data; /* Pointer to the buffer of data */
   oa_uint32 u4DataLen; /* Data length of each transfer */
   oa_uint32 u4TransferNum; /* Transfer number */
}oa_i2c_ctrl_cont_write_t,oa_i2c_ctrl_cont_read_t;
/* For OA_I2C_CMD_WRITE_AND_READ command. */
typedef struct
{
   oa_uint8 *pu1InData; /* Pointer to the read data */
   oa_uint32 u4InDataLen; /* Read data length */
   oa_uint8 *pu1OutData; /* Pointer to the write data */
   oa_uint32 u4OutDataLen; /* Write data length */
}oa_i2c_ctrl_write_and_read_t;
/* For oa_i2c_ctrl_batch_data_t structure. */
typedef union
{
  oa_i2c_ctrl_single_write_t rSingleWrite; /* Single write */
  oa_i2c_ctrl_single_read_t rSingleRead; /* Single read */
  oa_i2c_ctrl_cont_write_t rContWrite; /* Continue write */
  oa_i2c_ctrl_cont_read_t rContRead; /* Continue read */
  oa_i2c_ctrl_write_and_read_t rWriteAndRead; /* Write and read. Note: this command can not be used, when using DMA to transfer but driver can only support OA_DCL_I2C_DMA_SINGLE_CHANNEL. */
}oa_i2c_ctrl_transaction_data_t;
/* For oa_i2c_ctrl_batch_transaction_t structure. */
typedef struct
{
   oa_dcl_i2c_transaction_type eTransactionType; /* Transaction type */
   oa_i2c_ctrl_transaction_data_t rTansactionData; /* Batch transaction data of corresponding transaction type */
}oa_i2c_ctrl_batch_data_t;
/* For OA_I2C_CMD_BATCH_TRANSACTION command. */
typedef struct
{
   oa_uint32 u4BatchNum; /* Number of batches of this batch transaction */
   oa_i2c_ctrl_batch_data_t *prBatchData; /* Pointer to the buffer where stored the batches data*/
}oa_i2c_ctrl_batch_transaction_t;
/* For OA_I2C_CMD_GET_BATCH_TRANSACTION_RESULT command. */
typedef struct
{
   oa_i2c_ctrl_batch_transaction_t eBatchTransactionResult; /* Batch transaction result */
   oa_uint32 u4BatchNum;  /* The number of Batches have been transferred successfully */
}oa_i2c_ctrl_get_batch_transaction_result_t;
/* For OA_I2C_CMD_GET_DMA_PROPERTY command. */
typedef struct
{
   oa_dcl_i2c_dma_property eDMAProperty; /* DMA property */
}oa_i2c_ctrl_get_dma_property_t;
/* For I2C_CMD_ENABLE_DMA command. */
typedef struct
{
   oa_int32 fgEnableDMA; /* 1: use DMA to transfer data; 0: do not use DMA. Note: Make sure driver support DMA mode before you use it. */
}oa_i2c_ctrl_enable_dma_t;
/* For OA_I2C_CMD_SET_SLAVE_ADDRESS command. */
typedef struct
{
   oa_uint8 u1SlaveAddress; /* Slave address */
}oa_i2c_ctrl_set_slave_address_t;

typedef union
{
   oa_i2c_ctrl_get_transaction_mode_t rGetTransactionMode; /* Get transaction mode */
   oa_i2c_ctrl_set_transaction_speed_t rSetTransactionSpeed; /* Set transaction speed */
   oa_i2c_ctrl_single_write_t; /* Single write of blocking mode */
   oa_i2c_ctrl_single_read_t rSingleRead; /* Single read of blocking mode */
   oa_i2c_ctrl_cont_write_t rContWrite; /* Continue write of blocking mode */
   oa_i2c_ctrl_cont_read_t rContRead; /* Continue read of blocking mode */
   oa_i2c_ctrl_write_and_read_t rWriteAndRead; /* Write and read of blocking mode */
   oa_i2c_ctrl_batch_transaction_t rBatchTransaction; /* Batch transaction which support non-blocking mode */
   oa_i2c_ctrl_get_batch_transaction_result_t rGetBatchTransactionResult; /*Query the prior batch transaction result */
   oa_i2c_ctrl_get_dma_property_t rGetDMAProperty; /* Get the DMA property which driver support */
   oa_i2c_ctrl_enable_dma_t rEnableDMA; /* Use DMA to transfer data. Note: Before you do this, please make sure that driver support DMA mode */
   oa_i2c_ctrl_set_slave_address_t rSetSlaveAddress; /* Set slave address of sensor */
}oa_hw_i2c_ctrl_data_t;

#endif /* __OA_TYPE_H__ */
