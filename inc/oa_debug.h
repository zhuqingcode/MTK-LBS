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
 *   oa_debug.h
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
 *   zhuqing.
 *
 ****************************************************************************/
#ifndef __OA_DEBUG_H__
#define __OA_DEBUG_H__
#define __DEBUG__ 

#ifdef __DEBUG__  
#define DEBUG(format,...) OA_DEBUG_USER(" "__FILE__" %d : "format"", __LINE__, ##__VA_ARGS__)
#else  
#define DEBUG(format,...)  
#endif  
#endif