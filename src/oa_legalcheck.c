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
 *   oa_legalcheck.c
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
 *   zhuqing.
 *
 ****************************************************************************/
 #include "oa_type.h"
 /*********************************************************
*Function:     digit_check()
*Description:  digit_check
*Return:		void
*Others:         
*********************************************************/
 oa_bool digit_check(oa_char ch)
 {
	if ((ch >= '0') && (ch <= '9')) return OA_TRUE;
	else return OA_FALSE;
	
 }
  /*********************************************************
*Function:     digit01_check()
*Description:  digit_check
*Return:		void
*Others:         
*********************************************************/
 oa_bool digit01_check(oa_char ch)
 {
	if ((ch >= '0') && (ch <= '1')) return OA_TRUE;
	else return OA_FALSE;
	
 }
/*********************************************************
*Function:     digit01_check()
*Description:  digit check only for '0' '1' 
*Return:		void
*Others:         
*********************************************************/
 oa_bool digit01_string_check(oa_char *src, oa_uint8 len)
 {
	oa_uint8 i;
	if (NULL == src || len == 0){
		Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
		return OA_FALSE;
	}

	for(i = 0;i < len; i++){
		if (digit01_check(src[i])){
			continue;
		}
		else{
			return OA_FALSE;
		}
	}
	return OA_TRUE;
	
 }
/*********************************************************
*Function:     digit_check()
*Description:  digit_check
*Return:		void
*Others:         
*********************************************************/
 oa_bool alpha_check(oa_char ch)
 {
	if (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'))) return OA_TRUE;
	else return OA_FALSE;
 }
/*********************************************************
*Function:     digit_check()
*Description:  digit_check
*Return:		void
*Others:         
*********************************************************/
 oa_bool cap_alpha_check(oa_char ch)
 {
	if ((ch >= 'A') && (ch <= 'Z')) return OA_TRUE;
	else return OA_FALSE;
 }
 /*********************************************************
*Function:     digit_alpha_check()
*Description:  digit_alpha_check
*Return:		void
*Others:         
*********************************************************/
 oa_bool digit_alpha_check(oa_char *src, oa_uint8 len)
 {
	oa_uint8 i;
	if (NULL == src || len == 0){
		Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
		return OA_FALSE;
	}

	for (i = 0;i < len; i++){
		if (digit_check(src[i]) || alpha_check(src[i])){
			continue;
		}
		else{
			return OA_FALSE;
		}
	}
	return OA_TRUE;
 }
  /*********************************************************
*Function:     digit_alpha_check()
*Description:  digit_alpha_check
*Return:		void
*Others:         
*********************************************************/
 oa_bool cap_digit_alpha_check(oa_char *src, oa_uint8 len)
 {
	oa_uint8 i;
	if (NULL == src || len == 0){
		Trace("(%s:%s:%d): paras err!", __FILE__, __func__, __LINE__);
		return OA_FALSE;
	}

	for (i = 0;i < len; i++){
		if (digit_check(src[i]) || cap_alpha_check(src[i])){
			continue;
		}
		else{
			return OA_FALSE;
		}
	}
	return OA_TRUE;
 }
  /*********************************************************
*Function:     myatoi()
*Description:  myatoi for solving mtk atoi's probrem : atoi("90") = 0;
*Return:		void
*Others:         
*********************************************************/
oa_int32 myatoi(const char *src)
{
	int total = 0;
	while (*src){
		total = total*10 + (int)(*src - '0');
		src++;
	}
	return total;
}
/*********************************************************
*Function:     atobi()
*Description:  myatoi for solving mtk atoi's probrem : atoi("90") = 0;
*Return:		void
*Others:         
*********************************************************/
oa_uint32 atobi(const char *src, oa_uint8 len)
{
	oa_uint32 bi_tmp = 0;
	oa_uint8 i;
	if (src == NULL || len == 0 || len > 32){//max 32 bit
		return 0;
	}
	for(i = 0;i < len; i++){
		if (src[len-i-1] == '1'){
			bi_tmp |= (0x1 << i);
		}
	}
	return bi_tmp;
}