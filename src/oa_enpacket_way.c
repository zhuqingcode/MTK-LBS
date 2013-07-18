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
*   oa_enpacket_way.c
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

/*********************************************************
*Function:       BCDToDec(u8 * bcd,u8 len) 
*Description:    将BCD码转换为整形
*Calls:          无
*Called By:      
*Input:          bcd  数据数组
				 len  bcd码数组长度
*Output:         
*Return:         成功返回0,
*Others:         
				 
*********************************************************/
oa_uint8  BCDToDec(oa_uint8 BCD) 					//bcd码转为二进制
{
 	return(((BCD>> 4) & 0x0F) * 10 + (BCD & 0x0F));
} 
//-----------------------------------------
/*********************************************************
*Function:       DecToBCD(unsigned char Dec)
*Description:    整数转BCD码
*Calls:          无
*Called By:      无
*Input:          Dec:整数值
				  
*Output:         无
*Return:         0:参数获取成功1:参数获取失败
 
*Others:         只能对100以内的整数转为半字节BCD码值
*********************************************************/
oa_uint8 DecToBCD(oa_uint8 Dec)
{
	if(Dec<100)
	{ 
    	return((Dec / 10) << 4) + (( Dec % 10) & 0x0F); 
	}
    else return 0; 
}
/*********************************************************
*Function:       XOR_Check
*Description:    对Str开始的StrLen长度个数进行异或校验
*Calls:          无
*Called By:      无 
*Input:          无
*Output:         Check 校验结果
*Return:         0：成功 其他：失败
*Others:         
*********************************************************/
u8 XOR_Check(const u8 *Str,const u16 StrLen,u8 *Check)
{
	u16 i;
	const u8 *PStr=Str;
	*Check=0;
	if((StrLen==0)||(Str==NULL))
	{
		return 1;
	}
	for(i=0;i<StrLen;i++)
	{
		if(PStr)
		{
			*Check^=*PStr++;
		}
		else
		{
			return 1;
		}
	}
	return 0;
}
/*-------------------------------------------------------------------
*数制转换函数
--------------------------------------------------------------------*/
void  int_to_char (u8 *p_arg, u32 data)
{
    p_arg[0] = (data >>24)&0xff;
    p_arg[1] = (data >>16)&0xff;
    p_arg[2] = (data >>8)&0xff;
    p_arg[3] = data &0xff;
}
void  short_to_char (u8 *p_arg, u16 data)
{
    p_arg[0] = (data >>8)&0xff;
    p_arg[1] = data &0xff;
}

void  char_to_int (u8 *p_arg, u32 *data)
{
    *data = (p_arg[0]<<24) | (p_arg[1]<<16) |(p_arg[2]<<8) |(p_arg[3]);
}

void  char_to_short (u8 *p_arg, u16 *data)
{
   *data = (p_arg[0]<<8) | (p_arg[1]);
}