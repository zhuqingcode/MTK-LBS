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
 *   oa_api.h
 *
 * Project:
 * --------
 *   OPEN AT project
 *
 * Description:
 * ------------
 *   This Module declares OPEN AT's api.
 *
 * Author:
 * -------
 *   LiangQuan.
 *
 ****************************************************************************/
#ifndef __OA_API_H__
#define __OA_API_H__

#define oa_milli_secs_to_ticks(x)  (x)*10/46
#define oa_ticks_to_milli_secs(x)   (x)*46/10

#define oa_is_alpha(c)      (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
#define oa_is_digit(c)        ((c >= '0') && (c <= '9'))
#define oa_is_alphadigit(c)    (oa_is_alpha(c) || oa_is_digit(c))

/*for dll check address*/
void oa_api_begin(void);
void oa_api_end(void);

/************************************************************/
/**************String APIs***********************************/
/************************************************************/

/**
 * strlen - Find the length of a string
 * @s: The string to be sized
 *
 * returns the length of @s
 */
extern oa_size_t oa_strlen(const oa_char *s);

/**
 * strcpy - Copy a %NUL terminated string
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 *
 * returns a pointer to @dest
 */
extern oa_char *oa_strcpy(oa_char *dest, const oa_char *src);

/**
 * strlcpy - Copy a %NUL terminated string into a sized buffer
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @size: size of destination buffer
 *
 * Compatible with *BSD: the result is always a valid
 * NUL-terminated string that fits in the buffer (unless,
 * of course, the buffer size is zero). It does not pad
 * out the result like strncpy() does.
 */
extern oa_size_t oa_strlcpy(oa_char *dest, const oa_char *src, oa_size_t size);

/**
 * strncpy - Copy a length-limited, %NUL-terminated string
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @n: The maximum number of bytes to copy
 *
 * The result is not %NUL-terminated if the source exceeds
 * @n bytes.
 */
extern oa_char *oa_strncpy(oa_char *dest, const oa_char *src, oa_size_t n);

/**
 * strcat - Append one %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 *
 * returns a pointer to @dest
 */
extern oa_char *oa_strcat(oa_char *dest, const oa_char *src);

/**
 * strlcat - Append a length-limited, %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 * @n: The size of the destination buffer.
 */
extern oa_size_t oa_strlcat(oa_char *dest, const oa_char *src, oa_size_t n);

/**
 * strncat - Append a length-limited, %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 * @n: The maximum numbers of bytes to copy
 *
 * returns a pointer to @dest
 *
 * Note that in contrast to strncpy, strncat ensures the result is
 * terminated.
 */
extern oa_char *oa_strncat(oa_char *dest, const oa_char *src, oa_size_t n);

/**
 * strcmp - Compare two strings
 * @cs: One string
 * @ct: Another string
 *
 * returns   0 if @cs and @ct are equal,
 *         < 0 if @cs is less than @ct
 *         > 0 if @cs is greater than @ct
 */
extern oa_int32 oa_strcmp(const oa_char *cs, const oa_char *ct);

/**
 * strcmp - Compare two strings
 * @cs: One string
 * @ct: Another string
 * @sz: Length to compare
 *
 * returns   0 if @cs and @ct are equal,
 *         < 0 if @cs is less than @ct
 *         > 0 if @cs is greater than @ct
 */
extern oa_int32 oa_strncmp(const oa_char *cs, const oa_char *ct, oa_size_t sz);

/**
 * strrchr - Find the last occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 */
extern oa_char * oa_strrchr(const oa_char * s, oa_int32 c);

/**
 * strstr - Find the first substring in a %NUL terminated string
 * @s1: The string to be searched
 * @s2: The string to search for
 */
extern oa_char * oa_strstr(const oa_char * s1,const oa_char * s2);

/**
 * strchr - Find the first occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 */
extern oa_char*	oa_strchr (const oa_char* s, oa_int32 c);

extern oa_int32 oa_strtol (const oa_char* s, oa_char** tail, oa_int32 dec);

extern oa_uint32 oa_strtoul (const oa_char* s, oa_char** tail, oa_int32 dec);

/**
 * Convert a string to all-lower-case string
 * @s: The string to be Convert
 */
extern oa_char *oa_strtolower(oa_char *s);

/**
 * oa_is_digit_str - check string all characters are digit or not. ('0'~'9')
 * @sz: ascii string
 * @len: string length
 *
 * returns   OA_TRUE:all characters are digit,('0'~'9')
                 OA_FALSE: invalid digit string
 */
oa_bool oa_is_digit_str (const char *sz, int len);

/**
 * oa_is_digit_str - check string are valid phone number address or not. ('0'~'9','+')
 * @sz: ascii string
 * @len: string length
 *
 * returns   OA_TRUE:is valid phone number address string,('0'~'9','+')
                 OA_FALSE: invalid phone number address string
 */
oa_bool oa_is_phone_addr_str (const char *sz, int len);

/*********************************************************************/
/*************************openat  unicode string api***********************/
/*********************************************************************/
extern oa_int32 oa_wstrlen(const oa_wchar *wstr);
extern oa_wchar *oa_wstrcpy(oa_wchar *to, const oa_wchar *from);
extern oa_wchar *oa_wstrncpy(oa_wchar *to, const oa_wchar *from, oa_int32 n);
extern oa_int32 oa_wstrcmp(const oa_wchar *s1, const oa_wchar *s2);
extern oa_int32 oa_wstrncmp(const oa_wchar *s1, const oa_wchar *s2, oa_int32 n);
extern oa_wchar *oa_wstrcat(oa_wchar *s1, const oa_wchar *s2);
extern oa_wchar *oa_wstrncat(oa_wchar *s1, const oa_wchar *s2, oa_int32 n);
extern oa_wchar *oa_wstrchr(const oa_wchar *s, oa_int32 c);
extern oa_wchar *oa_wstrrchr(const oa_wchar *str, oa_int32 ch);
extern oa_uint16 oa_ascii2unicode(oa_int8 *pOutBuffer, oa_int8 *pInBuffer);
extern oa_uint16 oa_ascii2unicodeN(oa_int8 *pOutBuffer, oa_int8 *pInBuffer,oa_uint32 len);
extern oa_uint16 oa_unicode2ascii(oa_int8 *pOutBuffer, oa_int8 *pInBuffer);
extern oa_uint16 oa_unicode2asciiN(oa_int8 *pOutBuffer, oa_int8 *pInBuffer,oa_uint32 len);

/*****************************************************************************
 * FUNCTION
 *  oa_chset_convert
 * DESCRIPTION
 *  Convert string between 2 character sets. (will add the terminate character)
 * PARAMETERS
 *  src_type        [IN]        Charset type of source
 *  dest_type       [IN]        Charset type of destination
 *  src_buff        [IN]        Buffer stores source string
 *  dest_buff       [OUT]       Buffer stores destination string
 *  dest_size       [IN]        Size of destination buffer (bytes)
 * RETURNS
 *  Length of destination string, including null terminator. (bytes)
 *****************************************************************************/
extern oa_int32 oa_chset_convert(oa_chset_enum src_type,
                                                    oa_chset_enum dest_type,
                                                    oa_char *src_buff,
                                                    oa_char *dest_buff,
                                                    oa_int32 dest_size); 

/* stream op */
extern oa_int32 oa_sprintf (oa_char* s, const oa_char* format, ...);

/* size max is 2048*/
extern oa_int32 oa_snprintf (oa_char* s, oa_uint32 size, const oa_char* format, ...);

//extern void oa_vsnprintf(oa_char* s, oa_uint32 size, const oa_char* format,oa_va_list list);


/************************************************************/
/**************Memory APIs***********************************/
/************************************************************/
extern void *oa_malloc(oa_uint32 size);
extern void oa_free(void *ptr);

/**
 * memchr - Find a character in an area of memory.
 * @s: The memory area
 * @c: The byte to search for
 * @n: The size of the area.
 *
 * returns the address of the first occurrence of @c, or %NULL
 * if @c is not found
 */
extern void *oa_memchr(const void *s, oa_int32 c, oa_size_t n);

/**
 * memcmp - Compare two areas of memory
 * @cs: One area of memory
 * @ct: Another area of memory
 * @count: The size of the area.
 */
extern oa_int32 oa_memcmp(const void *cs, const void *ct, oa_size_t n);

/**
 * memcpy - Copy one area of memory to another
 * @dest: Where to copy to
 * @src: Where to copy from
 * @n: The size of the area.
 *
 * returns a pointer to @dest
 */
extern void *oa_memcpy(void *dest, const void *src, oa_size_t n);

/**
 * memset - Fill a region of memory with the given value
 * @s: Pointer to the start of the area.
 * @c: The byte to fill the area with
 * @n: The size of the area.
 *
 * returns a pointer to @s
 */
extern void *oa_memset(void *s, oa_int32 c, oa_size_t n);

/************************************************************/
/**************File System APIs********************************/
/************************************************************/
extern oa_int32 oa_fcreate(const oa_wchar * filename);
extern oa_int32 oa_fseek(oa_fs_handle handle, oa_int32 offset, OA_FILE_SEEK_POS pos);
extern oa_int32 oa_fopen(const oa_wchar * filename);
extern oa_int32 oa_fopen_immediate_write(const oa_wchar * filename);
extern oa_int32 oa_fclose(oa_fs_handle handle);
extern oa_int32 oa_fread(oa_fs_handle handle, void * DataPtr, oa_uint32 Length, oa_uint32 * Read);
extern oa_int32 oa_fwrite(oa_fs_handle handle, void * DataPtr, oa_uint32 Length, oa_uint32 * Written);
extern oa_int32 oa_fdelete(const oa_wchar * filename);
extern oa_int32 oa_fsize(oa_fs_handle handle, oa_uint32 *size);
extern oa_int32 oa_frename(const oa_wchar * filename, const oa_wchar * newname);
extern oa_int32 oa_fposition(oa_fs_handle handle, oa_uint32 * position);
extern oa_int32 oa_fxdelete(const oa_wchar * foldername);
extern oa_int32 oa_fcreateDir(const oa_wchar * foldername);
extern oa_int32 oa_fremoveDir(const oa_wchar * foldername);

/************************************************************/
/********************MATH APIs*******************************/
/************************************************************/
extern oa_int32 oa_atoi (const oa_char* s);
extern oa_double oa_atof(const oa_char* s);
extern oa_char* oa_itoa (oa_int32 c, oa_char* s, oa_int32 len);
extern void oa_srand(oa_int32 s);
extern oa_int32	oa_rand	(void);
extern oa_int32	oa_abs(oa_int32 s);
extern oa_double oa_sin (oa_double d);
extern oa_double oa_cos (oa_double d);
extern oa_double oa_tan (oa_double d);
extern oa_double oa_sinh (oa_double d);
extern oa_double oa_cosh (oa_double d);
extern oa_double oa_tanh (oa_double d);
extern oa_double oa_asin (oa_double d);
extern oa_double oa_acos (oa_double d);
extern oa_double oa_atan (oa_double d);
extern oa_double oa_atan2 (oa_double d1 , oa_double d2 );
extern oa_double oa_exp (oa_double d);
extern oa_double oa_log (oa_double d);
extern oa_double oa_log10 (oa_double d);
extern oa_double oa_pow (oa_double d1 , oa_double d2);
extern oa_double oa_sqrt (oa_double d);
extern oa_double oa_fabs (oa_double d);
extern oa_double oa_ldexp (oa_double d1 , int d2);
extern oa_double oa_frexp (oa_double d1 , int* d2 );
extern oa_double oa_modf (oa_double d1 , oa_double* d2);
extern oa_double oa_fmod (oa_double d1 , oa_double d2);
extern oa_int32 oa_sprintf (oa_char* s, const oa_char* format, ...);
extern oa_int32 oa_snprintf (oa_char* s, oa_uint32 size, const oa_char* format, ...);

/************************************************************/
/**************Speech&Audio APIs******************************/
/************************************************************/

/*****************************************************************************
 * FUNCTION
 *  oa_set_speech_volume
 * DESCRIPTION
 *  set speech volume in call
 * PARAMETERS
 *  oa_level: 0~6
 * RETURNS
 *  true or false
*****************************************************************************/
extern oa_bool oa_set_speech_volume(oa_uint8 oa_level);

/*****************************************************************************
 * FUNCTION
 *  oa_audio_set_aud_path
 * DESCRIPTION
 *  set audio path
 * PARAMETERS
 *  oa_path: OA_AUD_PATH_NORMAL or OA_AUD_PATH_LOUDSPK
 * RETURNS
 *  true or false
*****************************************************************************/
extern oa_bool oa_audio_set_aud_path(oa_aud_path_enum oa_path);


/*****************************************************************************
 * FUNCTION
 *  oa_audio_cfg_mic_path
 * DESCRIPTION
 *  configure diff audio path use mic path
 * PARAMETERS
 *  oa_path: OA_AUD_PATH_NORMAL or OA_AUD_PATH_LOUDSPK
 * RETURNS
 *  true or false
*****************************************************************************/
extern oa_bool oa_audio_cfg_mic_path(oa_aud_path_enum oa_path, oa_audio_input_path_enum mic_path);

/*****************************************************************************
 * FUNCTION
 *  oa_audio_play_id
 * DESCRIPTION
 *  play an exist audio id request
 * PARAMETERS
 *  id: 
     play_style: oa_mdi_play_style
     callback:  oa_audio_play_file callback function
 * RETURNS
 *  void
*****************************************************************************/
extern void oa_audio_play_id(oa_uint32 id, oa_mdi_play_style play_style, oa_mdi_callback callback);

/*****************************************************************************
 * FUNCTION
 *  oa_audio_stop_id
 * DESCRIPTION
 *  stop audio play id
 * PARAMETERS
     callback:  oa_audio_stop_id callback function
 * RETURNS
 *  void
*****************************************************************************/
extern void oa_audio_stop_id(oa_uint32 id, oa_mdi_callback callback);

/*****************************************************************************
 * FUNCTION
 *  oa_audio_play_file
 * DESCRIPTION
 *  play an exist audio file request
 * PARAMETERS
 *  file_name: file name include directory. such as: L"C:\\Audio\\0101000000.amr"  UCS2 code
     play_style: oa_mdi_play_style
     callback:  oa_audio_play_file callback function
 * RETURNS
 *  void
*****************************************************************************/
extern void oa_audio_play_file(const oa_wchar *file_name, oa_mdi_play_style play_style, oa_mdi_callback callback);

/*****************************************************************************
 * FUNCTION
 *  oa_audio_stop_file
 * DESCRIPTION
 *  stop audio play
 * PARAMETERS
     callback:  oa_audio_stop_file callback function
 * RETURNS
 *  void
*****************************************************************************/
extern void oa_audio_stop_file(oa_mdi_callback callback);

/*****************************************************************************
 * FUNCTION
 *  oa_audio_pause
 * DESCRIPTION
 *  pause audio play
 * PARAMETERS
     callback:  callback function
 * RETURNS
 *  void
*****************************************************************************/
extern void oa_audio_pause(oa_mdi_callback callback);

/*****************************************************************************
 * FUNCTION
 *  oa_audio_resume
 * DESCRIPTION
 *  resume audio play
 * PARAMETERS
     callback:  callback function
 * RETURNS
 *  void
*****************************************************************************/
extern void oa_audio_resume(oa_mdi_callback callback);

/*****************************************************************************
 * FUNCTION
 *  oa_audio_stop_all
 * DESCRIPTION
 *  stop all audio play
 * PARAMETERS
     callback:  oa_audio_stop_all callback function
 * RETURNS
 *  void
*****************************************************************************/
extern void oa_audio_stop_all(oa_mdi_callback callback);

/*****************************************************************************
 * FUNCTION
 *  oa_tts_stream_play
 * DESCRIPTION
 *  tts steam play
 * PARAMETERS
     callback:  oa_tts_stream_play callback function
 * RETURNS
 *  void
*****************************************************************************/
extern void oa_tts_stream_play(oa_wchar *tts_stream, oa_mdi_callback callback);

/***************************************************************/
/****************************UART APIs**************************/
/***************************************************************/
//uart received callback function:oa_app_uartxx_recv(),max length is OA_UART_BUFFER_SIZE, 1024
extern oa_bool oa_uart_register_callback(oa_uart_enum port,oa_uart_rx_cb_fptr cb);
extern oa_bool oa_uart_open(oa_uart_enum port, oa_uart_struct *dcb);
extern oa_bool oa_uart_close(oa_uart_enum port);
//block write to uart, Max Length is 65535
extern oa_bool oa_uart_write(oa_uart_enum port,oa_uint8 *data, oa_uint16 len);
//not blocking write to uart, return the bytes push in uart buffer ,Max Length is 65535
extern oa_uint32 oa_uart_write_nb(oa_uart_enum port,oa_uint8 *data, oa_uint16 len, oa_uart_tx_cb_fptr cb);
extern oa_bool oa_uart_set_read_delay(oa_uart_enum port, oa_uint16 delay);
//extern oa_bool oa_uart_purge(oa_uart_enum port, oa_bool tx);
extern oa_bool oa_uart_switch_at_port(oa_uart_enum port);
extern oa_bool oa_uart_switch_tst_port(oa_uart_enum port);

/*Directly send AT command to Protocol*/
extern oa_bool oa_uart_send_at_cmd_req( oa_uint8*pData, oa_uint32 len);

/*register the AT response callback*/
extern void oa_atf_rsp_callback_register(oa_func_at_rsp_ptr func);

/************************************************************/
/*********************GPIO APIs******************************/
/************************************************************/

/*
* FUNCTION                                                            
*	oa_gpio_init
*
* DESCRIPTION                                                           
*   	This function is to initialize one GPIO pin as INPUT or OUTPUT
*
* CALLS  
*	Initialize one GPIO pin as INPUT or OUTPUT
*
* PARAMETERS
*	direction: 0-INPUT and 1-OUTPUT
*	pin: pin gpio no.
*	
* RETURNS
*	None
*
* GLOBALS AFFECTED
*   external_global
*/
extern void     oa_gpio_init(oa_uint8 direction, oa_uint8 pin);

/*
* FUNCTION                                                            
*	oa_gpio_pull_selhigh
*
* DESCRIPTION                                                           
*   	This function is to select pull up/down of the related GPIO pin.
*
* PARAMETERS
*	pin : gpio pin number range from 0~54 (MT6219)
*	enable: enable the pull up/down
*	
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*/
extern void     oa_gpio_pull_selhigh(oa_uint8 pin, oa_bool enable);

/*
* FUNCTION                                                            
*	oa_gpio_pull_setup
*
* DESCRIPTION                                                           
*   	This function is to enable or disable the pull up/down of the related GPIO pin.
*		You can not decide to pull up or down, it is set inside the chip.
*		And, the setting is different from pin by pin.
*
* PARAMETERS
*	pin : gpio pin number range from 0~54 (MT6219)
*	enable: enable the pull up/down
*	
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*/
extern void     oa_gpio_pull_setup(oa_uint8 pin, oa_bool enable);

/*
* FUNCTION                                                            
*	oa_gpio_mode_setup
*
* DESCRIPTION                                                           
*   	This function is to MD251 pin's mode. The mode is defined in MD251 datasheet.
*
* PARAMETERS
*	pin : gpio pin number
*	conf_data: 0      : GPIO_MODE
                   1,2,3 : Other mode, refer the datasheet
*	
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*/
extern void     oa_gpio_mode_setup(oa_uint8 pin, oa_uint8 conf_data);

/*
* FUNCTION                                                            
*	oa_gpio_read
*
* DESCRIPTION                                                           
*   	This function is to read data from one GPIO pin
*
* CALLS  
*	Read data from one GPIO pin
*
* PARAMETERS
*	port: pin number
*	
* RETURNS
*	1 or 0
*
* GLOBALS AFFECTED
*   external_global
*/
extern oa_uint8 oa_gpio_read(oa_uint8 port);

/*
* FUNCTION                                                            
*	oa_gpio_write
*
* DESCRIPTION                                                           
*   	This function is to write data to one GPIO port
*
* CALLS  
*	Write data to one GPIO port
*
* PARAMETERS
*	data: 1-HIGH LEVEL or 0-LOW LEVEL
*	port: gpio pin number
*	
* RETURNS
*	None
*
* GLOBALS AFFECTED
*   external_global        
*/
extern void     oa_gpio_write(oa_uint8 data, oa_uint8 port);

/************************************************************/
/*********************Interrupt APIs******************************/
/************************************************************/

/*************************************************************************
* FUNCTION
*  oa_eint_registration
*
* DESCRIPTION
*   This function implements registration of external interrupts
*
* CALLS
*
* PARAMETERS
*  eintno            -     External interrupt vector number, MD251 eintno:0,1,4,5,6
*  Dbounce_En        -     Debounce control enable
*  ACT_Polarity      -     Interrupt polarity
*  reg_hisr          -     Registered hisr
*  auto_umask        -     A flag instruct the systerm to do unmask after
*                          invoking the registered HISR
*
* RETURNS
*  No return
*
* GLOBALS AFFECTED
*
*************************************************************************/
extern void      oa_eint_registration(oa_uint8 eintno, oa_bool Dbounce_En, oa_bool ACT_Polarity, \
                                     void (reg_hisr)(void), oa_bool auto_umask);


/*************************************************************************
* FUNCTION
*  oa_eint_set_debounce
*
* DESCRIPTION
*   This function dedicates to set the hardware debounce time of external 
*   interrupts.
*
* CALLS
*
* PARAMETERS
*   eintno   -   External interrupt vector number, MD251 eintno:0,1,4,5,6
*   ms       -   hardware debounce time (in milli second)
*
* RETURNS
*   No return
*
* GLOBALS AFFECTED
*
*************************************************************************/
extern void oa_eint_set_debounce(oa_uint8 eintno, oa_uint32 ms);


/*************************************************************************
* FUNCTION
*  oa_eint_set_polarity
*
* DESCRIPTION
*   This function sets the polarity of external interrupts
*
* CALLS
*
* PARAMETERS
*  eintno            -     External interrupt vector number,MD251 eintno:0,1,4,5,6
*  ACT_Polarity      -     Interrupt polarity
*
* RETURNS
*  No return
*
* GLOBALS AFFECTED
*
*************************************************************************/
extern void oa_eint_set_polarity(oa_uint8 eintno, oa_bool ACT_Polarity);


/*************************************************************************
* FUNCTION
*  oa_eint_set_sensitivity
*
* DESCRIPTION
*   This function dedicates to set the sensitivity of external 
*   interrupts.
*
* CALLS
*
* PARAMETERS
*   eintno   -   External interrupt vector number,MD251 eintno:0,1,4,5,6
*   sens     -   EDGE_SENSITIVE:1
                     LEVEL_SENSITIVE:0
*
* RETURNS
*   0 for success; -1 for failure
*
* GLOBALS AFFECTED
*
*************************************************************************/
extern oa_uint32 oa_eint_set_sensitivity(oa_uint8 eintno, oa_bool sens);

/*************************************************************************
* FUNCTION
*  oa_eint_enable
*
* DESCRIPTION
*   Enable the extern interrupt
*
* CALLS
*
* PARAMETERS
*   eintno   -   External interrupt vector number,MD251 eintno:0,1,4,5,6
*
* RETURNS
*
* GLOBALS AFFECTED
*
*************************************************************************/
extern void oa_eint_enable(oa_uint8 eintno);

/*************************************************************************
* FUNCTION
*  oa_eint_disable
*
* DESCRIPTION
*   Disable the extern interrupt
*
* CALLS
*
* PARAMETERS
*   eintno   -   External interrupt vector number,MD251 eintno:0,1,4,5,6
*
* RETURNS
*
* GLOBALS AFFECTED
*
*************************************************************************/
extern void oa_eint_disable(oa_uint8 eintno);

/************************************************************/
/***********************SW IIC control APIs************************/
/************************************************************/
/*************************************************************************
* FUNCTION
*	oa_iic_init
* DESCRIPTION
*	This function initializes serial interface.
*
* PARAMETERS
*  scl_pin :  default GPIO_1
*  sda_pin : default GPIO_2,if sda_pin is define 0xff,we will use HW I2C,else we used SW I2C
*  freq :     if SW I2C,0 default
* RETURNS
*	None
*
*************************************************************************/
extern void oa_iic_init(  oa_uint8 scl_pin, oa_uint8 sda_pin , oa_uint32 freq);
// iic_start , send start bit for SW I2C
extern void oa_iic_start(void);  
// iic_stop , send stop bit for SW I2C
extern void oa_iic_stop(void);  
extern oa_bool oa_iic_write_byte( oa_uint8 wdata );    // iic_write_byte
extern oa_uint8 oa_iic_read_byte( oa_bool re_data );  // iic_read_byte re_data -- whether send ack bit

extern oa_bool oa_iic_read_bytes( oa_uint8 iic_addr, oa_uint8 reg_addr, oa_char * rdata, oa_uint8 len);
extern oa_bool oa_iic_write_bytes( oa_uint8 iic_addr, oa_uint8 reg_addr, oa_char * wdata, oa_uint8 len);


/************************************************************/
/************************SW spi interface***********************/
/************************************************************/
/*************************************************************************
* FUNCTION
*	oa_spi_init
* DESCRIPTION
*	This function initializes SPI interface.
* PARAMETERS
* cs_pin:default GPIO_22  
* sck_pin:default GPIO_19  
* mosi_pin:default GPIO_9  ,if define 0xff,we will use HW SPI,else SW SPI
* miso_pin:default GPIO_19 
* RETURNS
*	None
*************************************************************************/
extern void oa_spi_init( oa_uint8 cs_pin, oa_uint8 sck_pin , oa_uint8 mosi_pin, oa_uint8 miso_pin);

/*************************************************************************
* FUNCTION
*	oa_spi_cs_en
* DESCRIPTION
*	This function is enable SPI.only for SW SPI.
*
* PARAMETERS
*  None
* RETURNS
*	None
*************************************************************************/
extern void oa_spi_cs_en(void) ;

/*************************************************************************
* FUNCTION
*	oa_spi_cs_dis
* DESCRIPTION
*	This function is disable SPI.only for SW SPI.
*
* PARAMETERS
*  None
* RETURNS
*	None
*************************************************************************/
extern void oa_spi_cs_dis(void) ;

/*************************************************************************
* FUNCTION
*	oa_spi_read_data
*
* DESCRIPTION
*	This function is to read data via SPI.
*
* PARAMETERS
*  None
*
* RETURNS
*	received data
*
* GLOBALS AFFECTED
*
*************************************************************************/
extern oa_uint8 oa_spi_read_data(void);

/*************************************************************************
* FUNCTION
*	oa_spi_write_data
*
* DESCRIPTION
*	This function is to write data via SPI.
*
* PARAMETERS
*  transmitted data
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
extern void oa_spi_write_data(oa_uint8 data);


/*********************************************************************************/
/**************************start HW I2C Interface*************************************/
/*********************************************************************************/

/*************************************************************************
* FUNCTION
*  oa_hw_i2c_open
*
* DESCRIPTION
*  This function is to open the I2C module and return a handle
*
* PARAMETERS
*
* RETURNS
*  oa_hw_i2c_handle_invalid: Incorrect device ID.
*  oa_hw_i2c_handle_occupied: This user has opened a handle, which is in use.
*  Other value: A valid handle
*
* EXAMPLE
*  <code>
*  oa_hw_i2c_handle handle = NULL;
*  if (NULL == handle)
*  {
*      handle = oa_hw_i2c_open();
*  }
*  </code>
*
*************************************************************************/
extern oa_hw_i2c_handle oa_hw_i2c_open(void);

/*************************************************************************
* FUNCTION
*  oa_hw_i2c_Configure
*
* DESCRIPTION
*  This function is to configure the I2C module.
*
* PARAMETERS
*  handle: the returned handle value of oa_hw_i2c_open
*  configure: a structure which include the I2C configuration.  
*
* RETURNS
*  OA_STATUS_OK: Successfully configure I2C module.
*  OA_STATUS_INVALID_DCL_HANDLE: It's a invalid handle.
*  OA_STATUS_NOT_OPENED: The module has not been opened.
*  OA_STATUS_INVALID_CONFIGURATION: The configuration is invalid.
*  OA_STATUS_DEVICE_NOT_SUPPORT_DMA: User wants to use DMA transfer, but driver do not support.
*
* EXAMPLE
*  <code>
*  oa_i2c_config_t cfg;
*  cfg.eOwner = 0;
*  cfg.fgGetHandleWait = 1;
*  cfg.u1SlaveAddress = OA_SLAVE_ADDRESS_ID;
*  cfg.u1DelayLen = 0;
*  cfg.eTransactionMode = OA_DCL_I2C_TRANSACTION_FAST_MODE;
*  cfg.u4FastModeSpeed = 300;
*  cfg.u4HSModeSpeed = 0;
*  cfg.fgEnableDMA = 0;
*  if (OA_STATUS_OK != oa_hw_i2c_Configure(i2c_handle, (oa_i2c_config_t *)&cfg))
*  {
*      ASSERT(0);
*  }
*  </code>
*
*************************************************************************/
extern oa_hw_i2c_status oa_hw_i2c_Configure(oa_hw_i2c_handle handle, oa_i2c_config_t * pconfigure);

/*************************************************************************
* FUNCTION
*  DclSI2C_Control
*
* DESCRIPTION
*  This function is to send command to control the I2C module.
*
* PARAMETERS
*  handle: The handle value returned from DclSI2C_Open
*  cmd: A control command for I2C module
*          1. OA_I2C_CMD_GET_TRANSACTION_MODE: to get transaction mode
*          2. OA_I2C_CMD_SET_TRANSACTION_SPEED: to set transaction speed
*          3. OA_I2C_CMD_SINGLE_WRITE: to perform a single WRITE
*          4. OA_I2C_CMD_SINGLE_READ: to perform a single READ
*          5. OA_I2C_CMD_CONT_WRITE: to perform a continuous WRITE
*          6. OA_I2C_CMD_CONT_READ: to  perform a continuous READ
*          7. OA_I2C_CMD_WRITE_AND_READ: to perform a specific sequence "WRITE then READ"
*          8. OA_I2C_CMD_BATCH_TRANSACTION: to perform a non-blocking batch transaction
*          9. OA_I2C_CMD_GET_BATCH_TRANSACTION_RESULT: to get batch transaction result
*          10. OA_I2C_CMD_GET_DMA_PROPERTY: to get DMA property
*          11. OA_I2C_CMD_ENABLE_DMA: to enable/disable DMA
*  data: The data of the control command
*          1. OA_I2C_CMD_GET_TRANSACTION_MODE: pointer to a oa_i2c_ctrl_get_transaction_mode_t structure
*          2. OA_I2C_CMD_SET_TRANSACTION_SPEED: pointer to a oa_i2c_ctrl_set_transaction_speed_t structure
*          3. OA_I2C_CMD_SINGLE_WRITE: pointer to a oa_i2c_ctrl_single_write_t structure
*          4. OA_I2C_CMD_SINGLE_READ: pointer to a oa_i2c_ctrl_single_read_t structure
*          5. OA_I2C_CMD_CONT_WRITE: pointer to a oa_i2c_ctrl_cont_write_t structure
*          6. OA_I2C_CMD_CONT_READ: pointer to a oa_i2c_ctrl_cont_read_t structure
*          7. OA_I2C_CMD_WRITE_AND_READ: pointer to a oa_i2c_ctrl_write_and_read_t structure
*          8. OA_I2C_CMD_BATCH_TRANSACTION: pointer to a oa_i2c_ctrl_transaction_data_t structure
*          9. OA_I2C_CMD_GET_BATCH_TRANSACTION_RESULT: pointer to a oa_i2c_ctrl_get_batch_transaction_result_t structure
*          10. OA_I2C_CMD_GET_DMA_PROPERTY: pointer to a oa_i2c_ctrl_get_dma_property_t structure
*          11. OA_I2C_CMD_ENABLE_DMA: pointer to a oa_i2c_ctrl_enable_dma_t structure
*
* RETURNS
*  OA_STATUS_OK: command is executed successfully.
*  OA_STATUS_FAIL: command is failed.
*  OA_STATUS_INVALID_CMD: It's a invalid command.
*  OA_STATUS_INVALID_DCL_HANDLE: It's a invalid handle.
*  OA_STATUS_NOT_OPENED: The module has not been opened.
*  OA_STATUS_DEVICE_IS_BUSY: I2C bus is occupied by others, but this user choose not to wait. Only valid for blocking transaction mode.
*  OA_STATUS_ACKERR: ACK error occured. Only valid for blocking transaction mode.
*  OA_STATUS_HS_NACKERR: Master code nack error occured. Only valid for blocking transaction mode.
*
* EXAMPLE
*  <code>
*  oa_uint8 out_buff[3];
*  out_buff[0] = REG_ADDR >> 8;
*  out_buff[1] = REG_ADDR;
*  out_buff[2] = WRT_DATA;
*  oa_hw_i2c_status rt;
*  oa_hw_i2c_cmd cmd = OA_I2C_CMD_SINGLE_WRITE;
*  oa_i2c_ctrl_single_write_t oa_write_data;
*  oa_write_data.pu1Data = out_buff;
*  oa_write_data.u4DataLen = 3;
*  rt = oa_hw_i2c_control(i2c_handle, cmd, (oa_hw_i2c_ctrl_data_t *)&oa_write_data);
*  if (OA_STATUS_OK != rt)
*  {
*      ASSERT(0);
*  }
*  </code>
*  
*************************************************************************/
extern oa_hw_i2c_status oa_hw_i2c_control(oa_hw_i2c_handle handle, oa_hw_i2c_cmd cmd, oa_hw_i2c_ctrl_data_t * data);
/*************************************************************************
* FUNCTION
*  DclSI2C_Close
*
* DESCRIPTION
*  This function is to close the I2C module.
*
* PARAMETERS
*  handle: the returned handle value of oa_hw_i2c_open
*
* RETURNS
*  OA_STATUS_OK
*
* EXAMPLE
*  <code>
*  if (handle)
*  {
*      oa_hw_i2c_close(handle);
*      handle = NULL;
*  }
*  </code>
*  
*************************************************************************/
extern oa_hw_i2c_status oa_hw_i2c_close(oa_hw_i2c_handle handle);

/*****************************************************************************/
/**************************end HW I2C Interface*************************************/
/*****************************************************************************/


/*****************************************************************************/
/**************************HW SPI Interface*************************************/
/*****************************************************************************/

/**************************************************************************************************************************************
  * Function
  * oa_hw_spi_open
  * Description
  * Open and get SPI handle.
  * Parameters
  * port :  [IN] SPI port. Zero based port index.only enable SPI1 be used on MD231G
  * For <color Blue>MT6238</color>/<color Blue>MT6268</color>/<color Blue>MT6236</color>, only one port is supported. 
  * Returns
  * Return a SPI handle.
  * Example
  * <code>
  * oa_hw_spi_handle handle;
  * handle = oa_hw_spi_open(1);//open SPI1
  * </code>                                                                                                                            
  **************************************************************************************************************************************/
oa_hw_spi_handle oa_hw_spi_open(oa_uint32 port);

/********************************************************
  * Function
  * oa_hw_spi_close
  * Description
  * Release a SPI handle.
  * Parameters
  * handle :  [IN] SPI handle. Retrieved from oa_hw_spi_open().
  * Returns
  * Return a SPI result code.
  * Example
  * <code>
  * oa_hw_spi_handle handle;
  * handle = oa_hw_spi_close(1);
  * // SPI operations.
  * oa_hw_spi_close(handle);
  * </code>                                              
  ********************************************************/
oa_spi_result oa_hw_spi_close(oa_hw_spi_handle handle);

/***********************************************************************
  * Function
  * oa_hw_spi_configure
  * Description
  * Configure SPI parameters including timing parameters and waveform parameters.
  * SPI parameters must be set before any SPI transmissions.
  * Parameters
  * handle :        [IN] SPI handle. Retrieved from oa_hw_spi_open().
  * pConfigParam :  [IN] SPI parameters. Refer to SPI_CONFIG_PARAM_T for details.
  * Returns
  * Return a SPI result code.
  * Example
  * <code>
  * oa_hw_spi_handle handle;
  * oa_spi_config_param_t param;
  * handle = oa_hw_spi_open(1);
  * param.cs_setup_time = 5;
  * param.cs_hold_time = 5;
  * param.cs_idle_time = 5;
  * param.clk_low_time = 5;
  * param.clk_high_time = 5;
  * param.tx_mlsb = OA_SPI_MSBF_MSB;
  * param.rx_mlsb = OA_SPI_MSBF_MSB;
  * param.clk_polarity = OA_SPI_CPOL_B0;
  * param.clk_fmt = OA_SPI_CPHA_B0;
  * oa_hw_spi_configure(handle, &amp;&param);
  * // ...
  * oa_hw_spi_close(handle);
  * </code>                                                             
  ***********************************************************************/
oa_spi_result oa_hw_spi_configure(oa_hw_spi_handle handle, oa_spi_config_param_t * pConfigParam);

/*******************************************************************************
  * Function
  * oa_hw_spi_write
  * Description
  * Fire a SPI transmission for output.
  * Parameters
  * handle :   [IN] SPI handle. Retrieved from oa_hw_spi_open().
  * pBuffer :  [IN] Data buffer for output.
  * length :   [IN] Data length for one package. Unit in bytes. <color Red><b>If count &gt; 1, length must be 4X.</b></color>
  * count :    [IN] Package count.<b> <color Red>If count &gt; 1, length must be 4X.</b></color>
  * fCB :      [IN] Specifies the callback function when the transmission completes.
  * If fCB is specified, SPI driver uses interrupt mode. This API is asynchronous.
  * If fCB is NULL, SPI drvier uses polling mode. This API becomes synchronous.
  * Returns
  * Return a SPI result code.
  * Example
  * <code>
  * oa_hw_spi_handle handle;
  * oa_spi_config_param_t param;
  * handle = oa_hw_spi_open(1);
  * // ...
  * oa_hw_spi_configure(handle, &amp;param);
  * 
  * // Synchronous call.
  * oa_hw_spi_write(handle, spi_test_buffer1, 1024, 1, NULL);
  * 
  * // Asynchronous call.
  * oa_hw_spi_write(handle, spi_test_buffer1, 1024, 1, spi_test_cb);
  * // Wait for a event which is set in callback function.
  * // ...
  * oa_hw_spi_close(handle);
  * </code>                                                                     
  *******************************************************************************/
oa_spi_result oa_hw_spi_write(oa_hw_spi_handle handle, void* pBuffer, oa_uint32 length, oa_uint32 count, oa_spi_callback fCB);


/*******************************************************************************
  * Function
  * oa_hw_spi_read
  * Description
  * Fire a SPI transmission for input.
  * Parameters
  * handle :   [IN] SPI handle. Retrieved from oa_hw_spi_open().
  * pBuffer :  [OUT] Data buffer for input.
  * length :   [IN] Data length for one package. Unit in bytes. <color Red><b>If count &gt; 1, length must be 4X.</b></color>
  * count :    [IN] Package count. <color Red><b>If count &gt; 1, length must be 4X.</b></color>
  * fCB :      [IN] Specifies the callback function when the transmission completes.
  * If fCB is specified, SPI driver uses interrupt mode. This API is asynchronous.
  * If fCB is NULL, SPI drvier uses polling mode. This API becomes synchronous.
  * Returns
  * Return a SPI result code.
  * Example
  * <code>
  * oa_hw_spi_handle handle;
  * oa_spi_config_param_t param;
  * handle = oa_hw_spi_open(1);
  * // ...
  * oa_hw_spi_configure(handle, &amp;param);
  * 
  * // Synchronous call.
  * oa_hw_spi_read(handle, spi_test_buffer2, 1024, 1, NULL);
  * 
  * // Asynchronous call.
  * oa_hw_spi_read(handle, spi_test_buffer2, 1024, 1, spi_test_cb);
  * // Wait for a event which is set in callback function.
  * // ...
  * oa_hw_spi_close(handle);
  * </code>                                                                     
  *******************************************************************************/
oa_spi_result oa_hw_spi_read(oa_hw_spi_handle handle, void* pBuffer, oa_uint32 length, oa_uint32 count, oa_spi_callback fCB);


/***********************************************************************************
  * Function
  * oa_hw_spi_readwrite
  * Description
  * Fire a SPI transmission for input and output. This transaction uses
  * duplex mode of SPI.
  * Parameters
  * handle :      [IN] SPI handle. Retrieved from oa_hw_spi_open().
  * pOutBuffer :  [IN] Data buffer for output.
  * pInBuffer :   [OUT] Data buffer for input.
  * length :      [IN] Data length for one package. Unit in bytes. <color Red><b>If count &gt; 1, length must be 4X.</b></color>
  * count :       [IN] Package count. <color Red><b>If count &gt; 1, length must be 4X.</b></color>
  * fCB :         [IN] Specifies the callback function when the transmission completes.
  * If fCB is specified, SPI driver uses interrupt mode. This API is asynchronous.
  * If fCB is NULL, SPI drvier uses polling mode. This API becomes synchronous.
  * Returns
  * Return a SPI result code.
  * Example
  * <code>
  * oa_hw_spi_handle handle;
  * oa_spi_config_param_t param;
  * handle = oa_hw_spi_open(1);
  * // ...
  * oa_hw_spi_configure(handle, &amp;param);
  * 
  * // Synchronous call.
  * oa_hw_spi_readwrite(handle, spi_test_buffer1, spi_test_buffer2, 1024, 1, NULL);
  * 
  * // Asynchronous call.
  * oa_hw_spi_readwrite(handle, spi_test_buffer1, spi_test_buffer2, 1024, 1, spi_test_cb);
  * // Wait for a event which is set in callback function.
  * // ...
  * oa_hw_spi_close(handle);
  * </code>
  ***********************************************************************************/
oa_spi_result oa_hw_spi_readwrite(oa_hw_spi_handle handle, void* pOutBuffer, void* pInBuffer, oa_uint32 length, oa_uint32 count, oa_spi_callback fCB);

/*****************************************************************************
  * Function
  * oa_hw_spi_power_ctrl
  * Description
  * Enable or disable SPI Controller power.
  * <textattr color="Red">This API is currently not implemented. SPI power will be turned on when calling spi_open() and turned off when calling</textattr> oa_hw_spi_close().
  * Parameters
  * handle :    [IN] SPI handle. Retrieved from oa_hw_spi_open().
  * bPowerOn :  [IN] Set OA_TRUE to power on SPI Controller. Set OA_FALSE to power off SPI Controller.
  * Returns
  * Return a SPI result code.
  * Example
  * <code>
  * oa_hw_spi_handle handle;
  * handle = oa_hw_spi_open(1);
  * oa_hw_spi_power_ctrl(handle, KAL_TRUE);
  * // SPI operations.
  * oa_hw_spi_power_ctrl(handle, KAL_FALSE);
  * oa_hw_spi_close(handle);
  * </code>                                                                   
  *****************************************************************************/
oa_spi_result oa_hw_spi_power_ctrl(oa_hw_spi_handle handle, oa_bool bPowerOn);

/*****************************************************************************
  * Function
  * oa_hw_spi_ioctl
  * Description
  * IO Control for SPI driver.
  * A series of SPI control code is defined for extension usage. Refer to oa_spi_ioctl_code.
  * Parameters
  * handle :  [IN] SPI handle. Retrieved from oa_hw_spi_open().
  * code :    [IN] SPI control code. Refer to oa_spi_ioctl_code.
  * pParam :  [IN/OUT] Parameter for SPI IO Control. The definition depends on the specified control code. Refer to oa_spi_ioctl_code.
  * Returns
  * Return a SPI result code.
  * The return value is OA_SPI_RESULT_NOT_SUPPORTED if the current platform does not support the sepcific control code.
  * Example
  * <code>
  * oa_hw_spi_handle handle;
  * oa_spi_hal_mode oa_SetMode;
  * handle = oa_hw_spi_open(1);
  * // Test for deassert mode.
  * oa_SetMode.mode = OA_SPI_MODE_DEASSERT;
  * oa_SetMode.bEnable = OA_TRUE;
  * oa_hw_spi_ioctl(handle, OA_SPI_IOCTL_SET_MODE, &amp;oa_SetMode);
  * oa_hw_spi_readwrite(handle, spi_test_buffer1, spi_test_buffer2, 1024, 1, NULL);
  * oa_SetMode.bEnable = OA_FALSE;
  * oa_hw_spi_ioctl(handle, OA_SPI_IOCTL_SET_MODE, &amp;oa_SetMode);
  * oa_hw_spi_close(handle);
  * </code>                                                                   
  *****************************************************************************/
oa_spi_result oa_hw_spi_ioctl(oa_hw_spi_handle handle, oa_spi_ioctl_code code, void* pParam);

/*****************************************************************************
  * Function
  * oa_hw_spi_wr_buf_reset
 * DESCRIPTION
      Reset the hw spi write buffer to 0x00, default size is 1024bytes
 * PARAMETERS
     void.
 * RETURNS
 *  void
*****************************************************************************/
void oa_hw_spi_wr_buf_reset(void);

/*****************************************************************************
  * Function
  * oa_hw_spi_wr_buf_reset
 * DESCRIPTION
      Reset the hw spi read buffer to 0x00, default size is 1024bytes
 * PARAMETERS
     void.
 * RETURNS
 *  void
*****************************************************************************/
void oa_hw_spi_rd_buf_reset(void);

/*****************************************************************************
  * Function
  * oa_get_hw_spi_wr_buf
 * DESCRIPTION
      Get the pointer to hw spi write buffer.
      default size is 1024 bytes.
 * PARAMETERS
     void.
 * RETURNS
 *  oa_uint8 *: struct pointer to the  SRAM buffer address.
*****************************************************************************/
oa_uint8 * oa_get_hw_spi_wr_buf(void);

/*****************************************************************************
  * Function
  * oa_get_hw_spi_rd_buf
 * DESCRIPTION
      Get the pointer to hw spi read buffer.
      default size is 1024 bytes.
 * PARAMETERS
     void.
 * RETURNS
 *  oa_uint8 *: struct pointer to the  SRAM buffer address.
*****************************************************************************/
oa_uint8 * oa_get_hw_spi_rd_buf(void);

/*****************************************************************************
  * Function
  * oa_get_hw_spi_wr_buf_size
 * DESCRIPTION
      Get current hw spi write buffer size.  default size is 1024bytes
 * PARAMETERS
     void.
 * RETURNS
 *  oa_int32
****************************************************************************/
oa_int32 oa_get_hw_spi_wr_buf_size(void);

/*****************************************************************************
  * Function
  * oa_get_hw_spi_rd_buf_size
 * DESCRIPTION
      Get current hw spi read buffer size.  default size is 1024bytes
 * PARAMETERS
     void.
 * RETURNS
 *  oa_int32
****************************************************************************/
oa_int32 oa_get_hw_spi_rd_buf_size(void);

/****************************************************************************/

/* **************************NVRAM buffer APIs********************************/
/* It's for keep keep record data                                                                     **********/
/****************************************************************************/

/*****************************************************************************
 * DESCRIPTION
 *  read the reserve NVRAM buffer through recordidx
 PARAMETERS
 *  recordidx:range (0~9)
 * RETURNS
 *  oa_reserve_struct *: A pointer to 2000 bytes NVRAM buffer 
 *****************************************************************************/
extern oa_reserve_struct *oa_nvram_read_reserve(oa_uint8 recordidx);
/*****************************************************************************
 * DESCRIPTION
 *  write data to the reserve NVRAM buffer through recordidx
 PARAMETERS
 *  param*: pointer data will write to NVRAM;
 *  size     : data length will write to NVRAM
 *  recordidx:range (0~9)
 * RETURNS
 *  oa_bool: OA_TRUE/OA_FALSE 
 *****************************************************************************/
extern oa_bool oa_nvram_write_reserve(oa_uint8 *param, oa_uint32 size, oa_uint8 recordidx);


/****************************************************************************/
/****************************SRAM buffer APIs**********************************/
/* SRAM buffer is a RAM data area that will not be initialized when system power on.           */
/* It's for keep needn't initialize data in RAM                                                                     */
/****************************************************************************/

/*****************************************************************************
 * DESCRIPTION
      Reset the SRAM buffer to 0x00, default size is 1024bytes
 * PARAMETERS
     void.
 * RETURNS
 *  void
*****************************************************************************/
extern void oa_sram_noinit_reset(void);

/*****************************************************************************
 * DESCRIPTION
      Get current SRAM buffer size.  default size is 1024bytes
 * PARAMETERS
     void.
 * RETURNS
 *  oa_int32
****************************************************************************/
extern oa_int32 oa_sram_noinit_get_size(void);

/*****************************************************************************
 * DESCRIPTION
      Get the pointer to SRAM buffer that no initialize.
      User can keep it's temporary ram data in the buffer that will not be initialized if target power reset.
      default size is 1024 bytes.
 * PARAMETERS
     void.
 * RETURNS
 *  oa_sram_noinit_struct *: struct pointer to the  SRAM buffer address.
*****************************************************************************/
extern oa_sram_noinit_struct * oa_sram_noinit_get_buf(void);


/***********************************************************************/
/*****************************RTC time/alarm APIs*************************/
/***********************************************************************/
extern void oa_sleep(oa_uint32 delay);
extern void oa_get_time(oa_time_struct *t);
extern void oa_set_time(oa_time_struct *t);
/*****************************************************************************
 * DESCRIPTION
    This function is to used to set SPOF data. For auto power on/off
 * PARAMETERS
 * index: 0~3
 * type: 0 for POWERON, 1 for POWEROFF
 * status: 0 for DISABLE, 1 for ENABLE
 * hour: 0~23
 * min:  0~59
 * RETURNS
 *  void
****************************************************************************/
extern void oa_set_spof_data(oa_uint8 index, oa_uint8 type, oa_uint8 status, oa_uint8 hour, oa_uint8 min);

/*****************************************************************************
 * DESCRIPTION
    This function dedicates to get current time stamp..
 * PARAMETERS
 *  void
 * RETURNS
 *  void
****************************************************************************/
extern oa_uint32 oa_get_time_stamp(void);

/*****************************************************************************
 * DESCRIPTION
    This function is to used to compute UTC time to RTC time.
 * PARAMETERS
 * diff_tz [IN]:  Time zone 
 * currTime  [IN]:  Utc time 
 * resultTime  [OUT]:  Rtc time 
 * RETURNS
 *  void
****************************************************************************/
extern void oa_date_utc_to_rtc(float diff_tz, oa_time_struct *currTime, oa_time_struct *resultTime);

/*************************************************************************/
/*************************** Other APIs************************************/
/*************************************************************************/
extern oa_char* oa_version( void );


/*************************************************************************
* FUNCTION
*  oa_adc_get_vbat_voltage
*
* DESCRIPTION
*   This function gets the vbat voltage via ADC
*
* CALLS
*
* PARAMETERS
*  NONE
*
* RETURNS
*  VBat value in voltage, Unit: uV
*
* GLOBALS AFFECTED
*
*************************************************************************/
extern oa_int32 oa_adc_get_vbat_voltage(void);

/*************************************************************************
* FUNCTION
*  oa_adc_get_data
* DESCRIPTION
*    This function gets the adc_channel value
*    adc_channel 4 is for user used.
*    other adc_channel is for system used.
* RETURNS
*  adc data
*************************************************************************/
extern oa_uint32 oa_adc_get_data( oa_uint8 adc_channel, oa_adc_mode mode, oa_adc_data_cb_fptr callback);


extern oa_uint8 oa_battery_is_charger_connected(void);

/*************************************************************************
* FUNCTION
*  oa_enable_low_power_sleep
*
* DESCRIPTION
*   This function enable low power sleep mode
*
* CALLS
*
* PARAMETERS
*  enable            -     enable sleep or not
*
* RETURNS
*  NONE
*
* GLOBALS AFFECTED
*
*************************************************************************/
extern void oa_enable_low_power_sleep(oa_bool enable);

extern oa_bool oa_sim_network_is_valid(void);

extern oa_bool oa_inital_csim_is_sms_ready(void);

extern void oa_make_call(oa_char* number);

extern void oa_answer_calls(void* param);

extern void oa_hang_up_calls(void* param);

extern oa_bool oa_is_in_call(void);

extern oa_call_state oa_get_current_call_state(void);

extern oa_int32 oa_dtmf_detect_register(oa_dtmf_callback_fptr cb);

extern void oa_power_shutdown(void* param);

extern void oa_module_restart(void *param);


/*****************************************************************************
 * FUNCTION
 *  oa_open_network_led
 * DESCRIPTION
       ForNetwork LED flash Blink control, Default MD251 network led pin19(gpio 0)
       
Refer MD251 module
State        MD251 function    
64ms On/ 800ms Off  MD251 does not find the network
64ms On/ 3000ms Off  MD251 find the network
64ms On/ 300ms Off  GPRS communication   

 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void oa_open_network_led(void);

extern void oa_close_network_led(void);
/*************************************************************************
* DESCRIPTION
*   This function set the network led gpio port,
* PARAMETERS
* gpio: mapping to gpio no,Default MD251 network led pin19(gpio 0)
* RETURNS
*  oa_bool
*************************************************************************/
extern oa_bool oa_set_network_led_gpio(oa_uint8 gpio);

/*************************************************************************
* DESCRIPTION
*   This function set the trace output level and port,
* PARAMETERS
* level: refer oa_trace_level_enum
* port: oa_trace_port_enum
* RETURNS
*  oa_bool
*************************************************************************/
oa_bool oa_set_trace_level_and_port(oa_trace_level_enum level, oa_trace_port_enum port);

/*************************************************************************
* FUNCTION
*  oa_cust_cmd_register
*
* DESCRIPTION
*   1.User can define it's self AT command with format "AT+OPENAT=<cmd>";
*   2.This function is register the callback function for handle <cmd>;
*   3.Callback function will received <cmd> string and <cmd> length. 
     EXAMPLE:
     If user send AT command "AT+OPENAT=DTUID:1380013800#"
     command "DTUID:1380013800#" will be received by the callback function.
*
* PARAMETERS
*   func: callback function;
*            prototype  oa_bool (* oa_func_cust_cmd_ptr)(oa_char*, oa_uint16);
*           oa_char*  : <cmd> string;
*           oa_uint16 : <cmd> length;
* RETURNS
*
*************************************************************************/
extern void oa_cust_cmd_register(oa_func_cust_cmd_ptr func);

extern void  oa_play_req_tone_register(oa_func_req_tone_ptr func);

extern void oa_set_alm_data(oa_uint8 index, 
                          oa_uint8 state, 
                          oa_uint8 hour, 
                          oa_uint8 min, 
                          oa_uint8 snooze,
                          oa_uint8 days,
                          oa_uint8 freq);


/************************************************************/
/**************Key  APIs ***********************/
/************************************************************/
void oa_key_register(oa_key key, oa_key_mode mode, oa_func_ptr func);
void oa_key_unregister(oa_key key, oa_key_mode mode);

/************************************************************/
/**************Timer  APIs ***********************/
/************************************************************/
/*Start a timer. When the timer expires, the handle will be invoked.
oa_uint8 id: timer id OA_TIMER_ID_X,refer enum oa_timer_id
oa_func_ptr handler: handle function pointer
void* param" parameter for handle func
oa_uint32 delay: timer delay scaled in MS
*/
void oa_timer_start(oa_uint8 id,
                    oa_func_ptr handler, 
                    void* param, 
                    oa_uint32 delay);
void oa_timer_stop(oa_uint8 id);



oa_eventid oa_evshed_start(oa_evshed_enum index, oa_func_ptr ptr, void* param, oa_uint32 elapse_time);
void oa_evshed_stop(oa_evshed_enum index, oa_eventid* eid);


/************************************************************/
/********************SMS send/receive APIs*********************/
/************************************************************/

 /*****************************************************************************
 * DESCRIPTION
      send sms through api
 * PARAMETERS
      func: oa_sms_send_req_callback_ptr, send sms request callback, return send result
      pAddr: sms mt number, max len is 21
      pData: sms content string, 
      len - sms content length
              dcs=SMSAL_DEFAULT_DCS,max len is 160*4,
              dcs=SMSAL_UCS2_DCS,max len is 70*4,
              dcs=SMSAL_8BIT_DCS, max len is 280,
      dcs:  refer oa_smsal_dcs_enum   
 * RETURNS
 *  void
*****************************************************************************/
extern void oa_sms_send_req(oa_sms_send_req_callback_ptr func, oa_char * pAddr, oa_char * pData, oa_uint16 len,oa_smsal_dcs_enum dcs);

 /*****************************************************************************
 * DESCRIPTION
      register received sms callback function,
 * PARAMETERS
      func: oa_sms_rcv_ind_callback_ptr , received sms callback function.
 * RETURNS
 *  void
*****************************************************************************/
extern void  oa_sms_rcv_ind_register(oa_sms_rcv_ind_callback_ptr func );


/************************************************************/
/**************SOCKET APIs for gprs rx/tx***********************/
/************************************************************/
/*OPENAT GPRS NETWORK APIs*/
extern void oa_gprs_apn_init(oa_soc_apn_info_struct* cidInfo);
extern void oa_gprs_set_apn_req(oa_soc_apn_info_struct* apnInfo, oa_soc_set_apn_rsp_fptr cb);
extern void oa_gprs_detach_req(oa_gprs_detach_rsp_fptr cb);
extern void oa_gprs_attach_req(oa_gprs_attach_rsp_fptr cb);

/*OPENAT SOCKET APIs*/
extern void  oa_soc_notify_ind_register(oa_soc_notify_ind_fptr cb);
extern oa_int8 oa_soc_create(oa_socket_type_enum  sockType, oa_uint16 bind_port);
extern oa_int8 oa_soc_connect( oa_int8 sockId, oa_socket_addr_enum addrType , oa_sockaddr_struct *addr );
extern oa_int8 oa_soc_close(oa_int8 sockId);
/*oa_soc_send, len max is 1400*/
extern oa_int32 oa_soc_send(oa_int8 sockId, oa_uint8* pBuff, oa_uint16 len);
extern oa_int32 oa_soc_sendto(oa_int8	sockId,oa_uint8 *pBuf,oa_int32	len,oa_uint8 flags, oa_sockaddr_struct *addr);
extern oa_int32 oa_soc_recv(oa_int8  sockId, oa_uint8 *pBuff,oa_int32 len,oa_uint8 flags);
extern oa_int8 oa_soc_bind(oa_int8 sockId, oa_sockaddr_struct *addr);
extern oa_int8 oa_soc_listen(oa_int8 sockId, oa_uint8 backlog);
extern oa_int8 oa_soc_accept(oa_int8 sockId, oa_sockaddr_struct *addr);
extern oa_int8 oa_soc_shutdown(oa_int8 sockId, oa_uint8 how);
/*check the ascii ip address is validity or not.If ascii addr is validity will convert the digit ip address.*/
extern oa_bool oa_soc_ip_check(oa_char *asci_addr,oa_uint8 *ip_addr,oa_bool *ip_validity);
/*provided a method to monitor the socket data retransmission times, in order to check if any errors in sending.*/
extern oa_int32 oa_soc_tcp_retransmit_times(oa_int8 socket_id);

extern oa_int8 oa_soc_getlocalip(oa_uint8 *local_ip);
extern oa_int8 oa_soc_gethostbyname(oa_char* addr, oa_soc_gethostbyname_fptr cb);
extern oa_int8 oa_soc_getsockaddr(oa_int8 sockId, oa_bool is_local, oa_sockaddr_struct *addr);


/************************************************************/
/**************Debug APIs for output trace***********************/
/************************************************************/
/**************************************************************
* DESCRIPTION
   for debug trace output.
   oa_trace_level_enum g_oa_trace_level:  //Debug information output level.
   oa_trace_port_enum g_oa_trace_port:  //Debug information output port.

*NOTE
   #define OA_DEBUG_BUF_MAXLEN 1024

* EXAMPLE:
   a) g_oa_trace_level = OA_TRACE_USER; g_oa_trace_port = OA_UART1; (default)
       OA_DEBUG_USER() trace will output to OA_UART1;

   b) g_oa_trace_level = OA_TRACE_USER|OA_TRACE_L1; g_oa_trace_port = OA_UART1;
       OA_DEBUG_USER() and OA_DEBUG_L1() trace will output to OA_UART1;

   c) g_oa_trace_level = OA_TRACE_ALL; g_oa_trace_port = OA_UART2;
       All OA_DEBUG_xxx() trace will output to OA_UART2;

***************************************************************/
extern void OA_DEBUG(oa_char *str, ...); // not control by g_oa_trace_level
extern void OA_DEBUG_USER(oa_char *str, ...);             // |OA_TRACE_USER will valid
extern void OA_DEBUG_L1(oa_char *str, ...);                    // |OA_TRACE_L1 will valid
extern void OA_DEBUG_L2(oa_char *str, ...);                    // |OA_TRACE_L2 will valid
extern void OA_DEBUG_L3(oa_char *str, ...);                    // |OA_TRACE_L3 will valid

/*****************************************************************************
 * DESCRIPTION
   1.For openat user: start ftp remote upgrade or get files
   2.Ftp upgrade state string will output to AT cmd port; callback function oa_app_at_rsp_recv() can received this AT string.   
          string                                    comment
        "+FTP:Start"                         FTP upgrade is start
        "+FTP:Not Start"                   FTP upgrade isn't start
        "+FTP:Param Error"              FTP upgrade string parameter is error
        "+FTP:Timer out"                  FTP upgrade timer out
        "+FTP:Fail"                           FTP upgrade failed
        "+FTP:Finished"                    FTP upgrade finished   
        "+FTP:Restart"                     FTP upgrade finished auto restart             
   3.If ftp_rsp_register  register callback function, can received the ftp state
   4.If g_oa_trace_level =|OA_TRACE_L3; Ftp upgrade trace log will ouput to oa_trace_port. default not ouput.

 *PARAMETERS:
     pFtpData String Format:
     "ftp:<ipAddress>,<port>,<usrName>,<pwd>"
     "ftp:<ipAddress>,<port>,<usrName>,<pwd>,<openatxxx.dll>"
     "ftp:<ipAddress>,<port>,<usrName>,<pwd>,<openatxxx.dll>,<noAutoRestart>"
     
     pFtpData String Description:
     <ipAddress>,<port>,<usrName>,<pwd>: is necessary.
     <openatxxx.dll>: is optional, if not specify will used the default "openatdll.dll".
     <ftp_flag>: refer oa_ftp_flag_enum, 
                    default is OA_FTP_FLAG_RSET, will auto restart.

 *EXAMPLE :
      1. oa_start_ftp_upgrade("ftp:218.82.3.214,21,simon,123456") //start upgrade, default "openatdll.dll", finished auto restart.
      2. oa_start_ftp_upgrade("ftp:218.82.3.214,21,simon,123456,openatxxx.dll") //specify "openatxxx.dll" as upgrade file.
      3. oa_start_ftp_upgrade("ftp:218.82.3.214,21,simon,123456,,1");//ftp upgrade finished will not auto restart

*NOTE:
    1. ftp gprs connect use default APN(cmnet,,), if need change APN, use function oa_gprs_set_apn_req

*RETURNS
   TRUE:  start ftp upgrade
   FALSE: not start ftp upgrade                                                    
**************************************************************************************/
extern oa_bool oa_start_ftp_upgrade(oa_char *pFtpData);
extern void oa_ftp_rsp_register(oa_ftp_rsp_cb_fptr cb);

extern oa_char* oa_get_imei(void);
extern oa_uint8* oa_get_chip_id(void);

/* Get the GSM signal level ,rang(0-31,99).. : refer oa_nw_signal_level.*/
extern oa_uint8 oa_network_get_signal_level(void);

//Get the GSM plmn lac cell_id information
extern oa_bool oa_network_get_location_info(oa_nw_info_location_info_struct *loc_info);

void * oa_reserve_dummy0(void * p);
void * oa_reserve_dummy1(void * p);
void * oa_reserve_dummy2(void * p);
void * oa_reserve_dummy3(void * p);
void * oa_reserve_dummy4(void * p);
void * oa_reserve_dummy5(void * p);
void * oa_reserve_dummy6(void * p);
void * oa_reserve_dummy7(void * p);
void * oa_reserve_dummy8(void * p);
void * oa_reserve_dummy9(void * p);
void * oa_reserve_dummy10(void * p);
void * oa_reserve_dummy11(void * p);
void * oa_reserve_dummy12(void * p);
void * oa_reserve_dummy13(void * p);
void * oa_reserve_dummy14(void * p);
void * oa_reserve_dummy15(void * p);
void * oa_reserve_dummy16(void * p);
void * oa_reserve_dummy17(void * p);
void * oa_reserve_dummy18(void * p);
void * oa_reserve_dummy19(void * p);

typedef struct
{
    oa_uint8 index;
    oa_char *param1;
    oa_char *param2;
    oa_char *param3;
    oa_char *param4;
    void *param5;
    oa_int32 *param6;
    oa_int32 *param7;    
    oa_int32 num1;
    oa_double num2;
    oa_char num3;
    oa_size_t num4;    
}oa_api_param;

#endif /* __OA_API_H__ */
