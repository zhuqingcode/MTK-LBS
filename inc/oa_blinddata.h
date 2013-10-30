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
*   oa_blinddata.h
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
#ifndef __OA_BLIND_H__
#define __OA_BLIND_H__
#include "oa_type.h"
#include "oa_api.h"
#define MANAGE_FILE L"c:\\blinddata.dir\\manage_file.ini"
#define DATA_MAX_LEN 98 //198data+2len of data
#define MAXLEN_1BLINDDATA (DATA_MAX_LEN+2)
#define OA_APP_BLINDDATA_1TIME 60000
#define OA_APP_BLINDDATA 250//1000
#define BLINDDATA_DIRNAME L"blinddata.dir"
#define BLINDDATA_DIR_CONF L"blinddata_dir_conf.ini"
#define DATA_NUM_MAX 10000
#define WRITE_INDEX 0
#define READ_INDEX 1
#define DATA_NAME_MAX_LEN 32
#define W_NAME_MAX_LEN 2 * DATA_NAME_MAX_LEN
#define DATANAME_1FILE L"c:\\blinddata.dir\\data"
typedef struct
{
	u8 data_buf[DATA_MAX_LEN];//98
	u16 data_len;// 2
}blinddata_struct;

typedef struct
{
	oa_bool dir_exist;
}dir_struct;

typedef struct
{
	u16 write_index;
	u16 read_index;
	oa_bool write_overflow;//OA_TURE:overflow
}manage_struct;

extern oa_bool oa_app_blinddata(void);
#endif