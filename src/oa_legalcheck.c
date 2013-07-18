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
 #include "oa_debug.h"
 #include <math.h>
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
		DEBUG(" paras err!");
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
		DEBUG(" paras err!");
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
		DEBUG(" paras err!");
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
*Function:     myatoi()
*Description:  myatoi for solving mtk atoi's probrem : atoi("90") = 0;
*Return:		void
*Others:         
*********************************************************/
void oa_myitoa(oa_uint32 uint, oa_char *dst)
{
	oa_uint8 i;
	if (dst == NULL){
		DEBUG("params err!");
		return;
	}

	for (i = 0;i < 32; i++){
		if (uint & (0x1 << i)) dst[32-i-1] = '1';
		else dst[32-i-1] = '0';
	}
	
}
/*********************************************************
*Function:     atobi()
*Description:  myatoi for solving mtk atoi's probrem : atoi("90") = 0;
*Return:		void
*Others:         
*********************************************************/
oa_uint32 atobi(const char *src, oa_uint8 len)
{
	oa_uint64 bi_tmp = 0;
	oa_uint8 i;
	if (src == NULL || len == 0 || len > 32){//max 32 bit
		DEBUG("params err!");
		return 0;
	}
	for(i = 0;i < len; i++){
		if (src[len-i-1] == '1'){
			bi_tmp |= (0x1 << i);
		}
	}
	return (oa_uint32)bi_tmp;
}
/*********************************************************
*Function:     myhypot()
*Description:  myatoi for solving mtk atoi's probrem : atoi("90") = 0;
*Return:		void
*Others:         
*********************************************************/
oa_double myhypot(oa_double f0, oa_double f1)
{
	OA_DEBUG_USER("f0:%lf f1:%lf sqrt():%lf", f0, f1, oa_sqrt(f0*f0 + f1*f1));
	return oa_sqrt(f0*f0 + f1*f1);
}
/*********************************************************
*Function:     asc2uc()
*Description: acs 2 uc
*Return:		void
*Others:         
*********************************************************/
oa_uint16 asc2uc(char *dst, char *src, oa_uint8 len)
{
	oa_uint8 i;
	if (src == NULL || dst == NULL || len == 0){
		return 0;
	}
	
	for(i = 0;i < len; i++){
		dst[2*i] = 0x0;
		dst[2*i+1] = src[i];
	}
	return len * 2;
}