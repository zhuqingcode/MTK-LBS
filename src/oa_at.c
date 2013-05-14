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
 *   oa_at.c
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
#include "oa_type.h"
#include "oa_api.h"
#include "oa_at.h"
#include "oa_platform.h"
#include "oa_dev_params.h"
extern DEVICE_PARAMS dev_now_params;
extern void do_reset(void);
extern void do_factory_set(void);
oa_uint8 at_fedbak_buf[AT_FEDBAK_MAX_LEN];
/*********************************************************
*Function:     is_specific_num()
*Description:  Determine whether the strange numbers     
*Return:		call_kind
*Others:         
*********************************************************/
call_kind is_specific_num(void)
{
	oa_char *p = NULL;
	oa_char *p_QUOTES = NULL;
	oa_char *p_QUOTES_NEXT = NULL;
	oa_char ring_num[RING_NUM_MAX_LEN] = {0};
	oa_uint8 len = 0;

	p = oa_strstr((oa_char *)at_fedbak_buf, RING_CLIP);
	if (NULL != p) {//extract ring number
		p_QUOTES = oa_strstr(p, QUOTES);
		if (NULL != p){
			p_QUOTES_NEXT = oa_strstr(p_QUOTES+1, QUOTES);
			if (NULL != p_QUOTES_NEXT){
				len = p_QUOTES_NEXT - p_QUOTES-1;
				if (len <= RING_NUM_MAX_LEN && len > 0){
					oa_memcpy(ring_num, p_QUOTES+1, len);
				}
				else{
					Trace("(%s:%s:%d): err ring number is too long!", __FILE__, __func__, __LINE__);
					return call_err;
				}
			}
			else{
				Trace("(%s:%s:%d): err in extract ring number!", __FILE__, __func__, __LINE__);
				return call_err;
			}
		}
		else{
			Trace("(%s:%s:%d): err in extract ring number!", __FILE__, __func__, __LINE__);
			return call_err;
		}

		if (!oa_is_phone_addr_str(ring_num, len)){//maybe it is 025-88888888-8888, it destn't work
			Trace("(%s:%s:%d): invalid ring number!", __FILE__, __func__, __LINE__);
			return call_err;
		}

		if (!oa_strncmp(ring_num, dev_now_params.reset_num, oa_strlen(ring_num))){
			Trace("(%s:%s:%d): reset ring!", __FILE__, __func__, __LINE__);
			return call_reset;
		}	
		else if (!oa_strncmp(ring_num, dev_now_params.restore_factory_settings_num, oa_strlen(ring_num)))	return call_factory_set;
		else{
			return call_err;
		}
	}
	else{
		return call_err;
	}
}

/*********************************************************
*Function:      oa_app_at()
*Description:  handle at command feedback  
*Return:        void
*Others:         
*********************************************************/
void oa_app_at(void)
{
	call_kind ret = call_err;
#ifdef AT_FEDBAK_PRINT
	OA_DEBUG_USER("%s called", __FILE__, __func__);
	OA_DEBUG_USER("%s", at_fedbak_buf);
#endif
	//handle at feedback
	if (NULL != oa_strstr((oa_char *)at_fedbak_buf, RING_CLIP)){
		oa_hang_up_calls(NULL);//hang up the calling whatever it is
		ret = is_specific_num();
		switch (ret){
			#if 0
			oa_hang_up_calls(NULL);
			OA_DEBUG_USER("Strange numbers is calling me......");
			OA_DEBUG_USER("so I will restart myself, goodbye......");
			oa_module_restart(NULL);
			#endif
			case call_reset:{
				do_reset();
			}break;
			case call_factory_set:{
				do_factory_set();
			}break;
			default:break;
		}
	}

	return;
}


