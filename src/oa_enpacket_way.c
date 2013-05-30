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
*Description:    ��BCD��ת��Ϊ����
*Calls:          ��
*Called By:      
*Input:          bcd  ��������
				 len  bcd�����鳤��
*Output:         
*Return:         �ɹ�����0,
*Others:         
				 
*********************************************************/
oa_uint8  BCDToDec(oa_uint8 BCD) 					//bcd��תΪ������
{
 	return(((BCD>> 4) & 0x0F) * 10 + (BCD & 0x0F));
} 
//-----------------------------------------
/*********************************************************
*Function:       DecToBCD(unsigned char Dec)
*Description:    ����תBCD��
*Calls:          ��
*Called By:      ��
*Input:          Dec:����ֵ
				  
*Output:         ��
*Return:         0:������ȡ�ɹ�1:������ȡʧ��
 
*Others:         ֻ�ܶ�100���ڵ�����תΪ���ֽ�BCD��ֵ
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
*Description:    ��Str��ʼ��StrLen���ȸ����������У��
*Calls:          ��
*Called By:      �� 
*Input:          ��
*Output:         Check У����
*Return:         0���ɹ� ������ʧ��
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
*����ת������
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