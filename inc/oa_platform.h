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
 *   oa_platform.h
 *
 * Project:
 * --------
 *   OPEN AT project
 *
 * Description:
 * ------------
 *   This Module gives a sample app on openat framework.
 *
 * Author:
 * -------
 *   None!
 *
 ****************************************************************************/
#ifndef _OA_PLATFORM_H_
#define _OA_PLATFORM_H_
#include "oa_type.h"
#include "oa_api.h"
#include "oa_setting.h"
#define OA_APP_PLAT_LINK_1ST 10000
#define AUTHEN_FILE L"authen_file.ini"

#define OA_APP_PLAT_LINK 1000// 1s
#define OA_APP_PLAT_DATA 3000//30s
#define PLAT_DATA_SECOND (OA_APP_PLAT_DATA / 1000)
#define AUTHEN_CODE_MAX_LEN 127
#define AUTHEN_ERR_MAX_TIMES 10
typedef struct
{
	oa_uint8 authen_code_len;
	oa_bool authen_status;//OA_FALSE:没有注册;OA_TRUE:已注册
	oa_uint8 authen_code[AUTHEN_CODE_MAX_LEN];
}AUTHEN_STRUCT;

typedef enum
{
	PLAT_SOC_INIT = 0x0,
	PLAT_DEV_REG,
	PLAT_DEV_LOGIN,
	PLAT_DEV_UNREG,
	PLAT_RECONN,
	PLAT_ONLINE_CHECK = 0xf,
}PLAT_STEP;

typedef enum
{
	OFFLINE = 0x0,
	ONLINE,
	SHORT_CONNING,
}DEV_NOW_STATUS;

typedef enum
{
	reg,
	authen,
	unreg,
	do_none = 0xf,
}dev_doing;

typedef struct
{
	PLAT_STEP next_step;					 //register,authentication,login steps
	DEV_NOW_STATUS plat_status;				//platform status:online/offline CONNING:just used for send a location data when acc is off
	oa_bool plat_switch;						//Avoid duplication operation OA_FALSE:不允许注册流程运行;OA_TRUE:允许注册流程运行
	dev_doing doing_what;                               //means whether device is authening or not OA_FALSE:not OA_TRUE:yes
	oa_uint8 authen_err_time;					//record authen times in case of my authen code is wrong
	oa_bool plat_check_value;                          //OA_TRUE:check , OA_FALSE:do not check
}DEV_PLAT_PARAS;
#endif
