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
#define DEBUG_K 0
#define DEBUG_U 1
#define DEBUG_N 2

#define DEBUG_LEVEL DEBUG_U

#if (DEBUG_K == DEBUG_LEVEL)
#define DEBUG(format,...) debug_no_n("("__FILE__" %s %d):"format"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define DEBUG_N(format,...) debug_no_n(""format"", ##__VA_ARGS__)
#define DEBUG_F(format,...) OA_DEBUG_USER("("__FILE__" %s %d):"format"", __func__, __LINE__, ##__VA_ARGS__)
#elif (DEBUG_U == DEBUG_LEVEL)
#define DEBUG(format,...) debug_no_n(""format"\r\n", ##__VA_ARGS__)
#define DEBUG_N(format,...) debug_no_n(""format"", ##__VA_ARGS__)
#define DEBUG_F(format,...) OA_DEBUG_USER(""format"", ##__VA_ARGS__)
#elif (DEBUG_N == DEBUG_LEVEL)
#define DEBUG(format,...)
#define DEBUG_N(format,...)
#endif
#endif
