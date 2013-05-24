/*
*********************************************************************************************************
*	                                  
*	ģ������ : ���򱨾�ģ��
*	�ļ����� : Area_Judge.c
*	��    �� : V1.0
*	˵    �� : ������Ϣ��ȡ���ж�
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2013-4-1  κ����  ST�̼���V3.5.0�汾�������д��δ��������ڹر�ģ�飬�ر�GNSS��λ�������ڷǷ�����δ��
*		v1.2    2013-4-22 κ����  ���Ӹ�����Ϣ���� ���洢��ɾ��ʱ�������������жϣ�����debug��ӡ
*	Copyright (C), 2010-2011, ���ʿƼ�
*
**********************************************************************************************************/
//#include "includes.h"
#include <time.h>
#include "oa_area.h"
#include "oa_debug.h"
#define min(x,y) (((x)>(y))?(y):(x))
#define max(x,y) (((x)<(y))?(y):(x))
#define CheckParam1(param1) ((param1) == NULL)
#define CheckParam2(param1,param2) (((param1) == NULL)||((param2) == NULL))
#define CheckParam3(param1,param2,param3) (((param1) == NULL)||((param2)== NULL)||((param3) == NULL))
#define CheckParam4(param1,param2,param3,param4) (((param1) == NULL)||((param2) == NULL)||((param3) == NULL)||((param4) == NULL))

/*��������*/
//extern OS_FLAG_GRP* Alarm_Driver_Event;
//static Area_Manage_struct area_manage;
//static Area_attr_struct  area_attr_struct;
//static u8 AreaJudgeSta[MAX_AREA_SUM]; //�����ж�״̬��
//static u32 AreaSpeedOverSta; //�����ж�״̬λ�飬һλ��ʾһ�����ٱ������ϱ�
//static u32 AreaDoorOpenSta; //�ſ����ж�״̬λ�飬һλ��ʾһ������ķǷ����ű������ϱ�
//static u32 SpeedOverStartTime[MAX_AREA_SUM];//�����ж���ʼʱ����

//static OS_EVENT *Area_Protect_Sem = 0;

//Area_Desc area_desc;
//static u8 W_R_buf[1024];/*��д����*/

/*�ڲ������ӿ�����*/
static void Reset_Area_ALL(void);
static u8 CheckSameID(u32 NewID,u8 area_type);
static u8 CheckFreeID(void);
static u8 SaveAreaDatatoFlash(u8 *pbuf,u8 SaveNum,u8 area_type,u16 *readlen);
static u8 DelAreaDataData(u8 save_num);
u8 CompareTime(u8 *start_time,u8 *stop_time,u8* now_time,u8 index);
static u8 Area_Judge_Process(u32 now_lon,u32 now_lat,Area_Desc *area_desc);
u8 Circular_Judge(u32 now_lon,u32 now_lat,Cir_Area_Desc *point_data);
u8 poly_Judge(u32 lon,u32 lat,Poly_Area_Desc *point_data,u16 polySides);
static Area_Desc * GetAreaData(u8 save_num,u8 *now_time);
u32 GetUnixTime(u8 *TimeStr);


/*********************************************************
*Function:       SaveAreaData()
*Description:    ����������ֵ
*                
*Calls:          
*Called By:      
*Input:          pbuf����������
				 area_type���������� Բ�� ���� �����
				 point_num������  ��Ϊ���������ʱ��Ч
				 option��д������  0���������� 1:׷������ 2:�޸�����
*Output:
*Return:         0������ɹ� 1������ʧ��
*Others:
*********************************************************/
u8 SaveAreaData(u8 *pbuf,u8 area_type,u8 option,u16 *readlen)
{
	u8 res;//ѭ������
	u32 New_Area_ID = 0;
	u8 SaveNum,err;
	if(CheckParam1(pbuf))
	{
		return 1;
	}
#if 0
	OSSemPend(Area_Protect_Sem,0,&err);
	if(err != OS_ERR_NONE)
	{
		return 1;
	}
#endif
	if(option == 0x00)//����ָ�
	{
		//Reset_Area_ALL();//����������Ϣ	
		//reset_all_area_file(area_type);
		del_area_data(0/*has no effect*/, area_type, all_area);
	}
#if 0
	if(area_manage.manage_vail_flag == 0xaa)//�жϹ���洢������Ч
	{
		char_to_int(pbuf,&New_Area_ID);//��ȡ��ID
		/*����Ƿ�����ͬ�洢ID������,����У������·�ָ���Ƿ�Ϊ׷������һ�ɶ�ԭ������Ϣ���滻*/
		SaveNum = CheckSameID(New_Area_ID,area_type);
		if(SaveNum!=0xff)
		{
			//�����Ӧλ��
			res = SaveAreaDatatoFlash(pbuf,SaveNum,area_type,readlen);//�����Ӧλ��
			//OSSemPost(Area_Protect_Sem);
			return res;
		}

		/*�����е�����,������򷵻ض�Ӧλ�ã����û�����滻��������������Ϣ*/
		SaveNum = CheckFreeID();
		if(SaveNum!=0xff)
		{
			//�����Ӧλ��
			res = SaveAreaDatatoFlash(pbuf,SaveNum,area_type,readlen);//�����Ӧλ��
			//OSSemPost(Area_Protect_Sem);
			return res;
		}
	}
	else
	{
		Reset_Area_ALL();//����������Ϣ
		SaveNum = 0;
		SaveAreaDatatoFlash(pbuf,SaveNum,area_type,readlen);
	}
	AreaJudgeSta[SaveNum] = 0;
	AreaSpeedOverSta&=~(1<<SaveNum) ;
	AreaDoorOpenSta&=~(1<<SaveNum) ;
	SpeedOverStartTime[SaveNum]= 0;
	//OSSemPost(Area_Protect_Sem);
	return 0;	
#endif
	write_area_data(pbuf, area_type, readlen);
}
#if 0
/*********************************************************
*Function:       GetAreaData()
*Description:    
*                
*Calls:          
*Called By:      
*Input:          save_num���������
				 now_time����ǰʱ���
*Output:
*Return:         0����ȡ�ɹ� 1����ȡʧ��
*Others:
*********************************************************/
static Area_Desc * GetAreaData(u8 save_num,u8 *now_time)
{
	Area_Desc *area_desc =(Area_Desc *)W_R_buf;
	u8 res;

	if((CheckParam1(now_time))||(save_num>MAX_AREA_SUM))
	{
		return NULL;
	}
	if(area_manage.manage_vail_flag == 0xaa)
	{
		if(area_manage.area_attr[save_num] == 0XAA)
		{
			Flash_Read_Buffer((u8*)&area_attr_struct,sizeof(Area_attr_struct),VIRTUAL_AERA_MANAGE_ADDR+sizeof(Area_Manage_struct)+sizeof(Area_attr_struct)*save_num);	
			//�жϱ�����ǰ�Ƿ���Ч
			if(area_attr_struct.flag==0xaa)
			{
				res = 1;
				if(area_attr_struct.area_para.depend_time)//��Ҫ��ʱ���ж�
				{
					if((now_time[0]==0x00)&&(now_time[1]==0x00)&&(now_time[2]==0x00))//�ж�����ʱ����Ч��
					{
						res = 0;
					}
					else
					{
						if((area_attr_struct.start_time[0]==0x00)&&(area_attr_struct.stop_time[0]==0x00))//ʱ���趨Ϊ����ʱ��
						{
							if((area_attr_struct.start_time[1]!=0x00)&&(area_attr_struct.stop_time[1]!=0x00))//������ʱ��
							{
								//�жϵ�ǰʱ�������趨ʱ�����
								res = CompareTime(area_attr_struct.start_time,area_attr_struct.stop_time,now_time,1);
							}
							else if((area_attr_struct.start_time[2]!=0x00)&&(area_attr_struct.stop_time[2]!=0x00))//������ʱ��
							{
								//�жϵ�ǰʱ�������趨ʱ�����
								res = CompareTime(area_attr_struct.start_time,area_attr_struct.stop_time,now_time,2);
							}
							else //������ʱ�䣬����ʱ���ںͷ������ж�
							{
								//�жϵ�ǰʱ�������趨ʱ�����
								res = CompareTime(area_attr_struct.start_time,area_attr_struct.stop_time,now_time,3);
							}
						}
						else//�趨������ʱ��
						{
							res = CompareTime(area_attr_struct.start_time,area_attr_struct.stop_time,now_time,0);
							if(!res)
							{
								//���ݹ���ʧЧ��ɾ������ ����Ϊ0
								DelAreaDataData(save_num);
							}
							if(res == 2)
							{
								res = 0; //δ��Ԥ��ʱ��
							}
						}
					}
				}
				if(res ==0)//���ڼ��ʱ����
				{
					return NULL;
				}
				//�������ݣ�����������Ч���ж�
				Flash_Read_Buffer((u8 *)area_desc,sizeof(Area_Desc),save_num * sizeof(Area_Desc)+VIRTUAL_AERA_DATA_ADDR);
				if((area_desc->poly_area_desc.area_point[0].Lon!=0xffffffff)&&(area_desc->poly_area_desc.area_point[0].Lon!=0x0))// 
				{
					return area_desc;
				}
				else
				{
					//����ʧЧ��ɾ������
					DelAreaDataData(save_num);
					return NULL;
				}
			}
			else
			{
				//��������Ч,��ɾ������
				DelAreaDataData(save_num);
			}
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		Reset_Area_ALL();
		return NULL;
	}
	return NULL;
}
#endif
/*********************************************************
*Function:       DelAreaData()
*Description:    ɾ��������Ϣ
*                
*Calls:          
*Called By:      
*Input:          
*Output:
*Return:         0��ɾ���ɹ� 1��ɾ��ʧ��
*Others:
*********************************************************/
u8 DelAreaData(u8* pbuf,u8 areatype)
{
	u32 New_Area_ID = 0;
	u8 SaveNum,err;
	u8 i, ret;
	
	if(pbuf)   //ɾ��ָ������ID
	{
#if 0
		OSSemPend(Area_Protect_Sem,0,&err);
		if(err != OS_ERR_NONE)
		{
			return 1;
		}
#endif
		char_to_int(pbuf,&New_Area_ID);//��ȡ��ID
		ret = del_area_data(New_Area_ID, areatype, one_area);
		if (ret == 0){
			OA_DEBUG_USER("======delete area ok======");
		}
		else OA_DEBUG_USER("======delete area failed======");
#if 0
		/*����Ƿ�����ͬ�洢ID������,����У������·�ָ���Ƿ�Ϊ׷������һ�ɶ�ԭ������Ϣ���滻*/
		SaveNum = CheckSameID(New_Area_ID,areatype);
		if(SaveNum!=0xff)
		{
			//ɾ����Ӧλ��
			DelAreaDataData(SaveNum);//ɾ����Ӧλ��
			OSSemPost(Area_Protect_Sem);
			AREA_Printf("============�ҵ���Ӧ������Ϣ,�洢λ��:%d,ɾ���ɹ�=============\r\n",SaveNum);
			return 0;
		}
		else
		{
			OSSemPost(Area_Protect_Sem);
			AREA_Printf("============δ�ҵ���Ӧ������Ϣ,ɾ��ʧ��=============\r\n");
			return 1;
		}
#endif
	}
	else  //ɾ������ͬһ������
	{
		ret = del_area_data(0/*has no effects*/, areatype, all_area);
		if (ret == 0){
			OA_DEBUG_USER("======delete area ok======");
		}
		else OA_DEBUG_USER("======delete area failed======");
#if 0
		OSSemPend(Area_Protect_Sem,0,&err);
		if(err != OS_ERR_NONE)
		{
			return 1;
		}
		for(i=0;i<MAX_AREA_SUM;i++)
		{
			//����ͬ����������
			if(area_manage.area_attr[i]==0xaa)
			{
				if((area_manage.area_type[i]==areatype))
				{
					DelAreaDataData(i);//ɾ����Ӧλ��	
				}
			}
		}
		OSSemPost(Area_Protect_Sem);
		AREA_Printf("============�ҵ�����������Ϣ,ɾ������:%d,ɾ���ɹ�=============\r\n",areatype);
		return 0;	
#endif
	}
}
#if 0
/*********************************************************
*Function:       Area_Judge_Init()
*Description:    �����ж�ģ���ʼ��
*                
*Calls:          
*Called By:      
*Input:          
*Output:
*Return:         0����ȡ�ɹ� 1����ȡʧ��
*Others:
*********************************************************/
u8 Area_Judge_Init(void)
{
	u8 err;

	if(Area_Protect_Sem == NULL)
	{
		Area_Protect_Sem = OSSemCreate(1);
	}
	OSSemPend(Area_Protect_Sem,0,&err);
	Flash_Read_Buffer((u8*)&area_manage,sizeof(Area_Manage_struct),VIRTUAL_AERA_MANAGE_ADDR);
//	Reset_Area_ALL();
	if(area_manage.manage_vail_flag!=0xaa) //��ǰֵ��Ч
	{
		Reset_Area_ALL();
		AREA_Printf("============��ʼ������Χ��ʧ�ܣ���λ����ṹ��=============\r\n");
	}
	OSSemPost(Area_Protect_Sem);
	return 0;	
}
#endif
#if 0
/*********************************************************
*Function:       Reset_Area_ALL()
*Description:    ��λ����������Ѵ�������Ϣ
*                
*Calls:          
*Called By:      
*Input:          pbuf��
				 area_num���������
*Output:
*Return:         0����ȡ�ɹ� 1����ȡʧ��
*Others:
*********************************************************/
static void Reset_Area_ALL(void)
{
	area_manage.manage_vail_flag = 0xaa;
	memset(area_manage.area_attr,0x00,sizeof(area_manage.area_attr));
	memset(area_manage.area_id,0x00,sizeof(area_manage.area_id));
	memset(area_manage.actual_id,0x00,sizeof(area_manage.actual_id));
	memset(area_manage.reserve_data,0x00,sizeof(area_manage.reserve_data));
	area_manage.max_actual_id = 0;
	Flash_Write_Buffer((u8*)&area_manage,sizeof(Area_Manage_struct),VIRTUAL_AERA_MANAGE_ADDR,Sector_Erase);
	Flash_Write_Buffer((u8*)&area_manage,sizeof(Area_Manage_struct),VIRTUAL_AERA_MANAGE_ADDR,Write_Mode);

	memset((u8*)AreaJudgeSta,0,sizeof(AreaJudgeSta));
	AreaSpeedOverSta = 0;
	AreaDoorOpenSta = 0;
	memset((u8*)SpeedOverStartTime,0,sizeof(SpeedOverStartTime));
}

/*********************************************************
*Function:       CheckSameID()
*Description:    �����ͬ����ID�������
*                
*Calls:          
*Called By:      
*Input:          
*Output:
*Return:         0~(MAX_AREA_SUM-1)  ��ͬ����ID�洢λ��  0xff:����ͬID
*Others:
*********************************************************/
static u8 CheckSameID(u32 NewID,u8 area_type)
{
	u8 res=0xff;
	u8 i;//ѭ������

	for(i=0;i<MAX_AREA_SUM;i++)
	{
		if(area_manage.area_attr[i] == 0xaa)//��������Ϣ��Ч
		{
			if((area_manage.area_id[i]==NewID)&&(area_manage.area_type[i]==area_type))
			{
				res = i;
				return i;
			}
		}
	}
	return res;
}

/*********************************************************
*Function:       CheckFreeID()
*Description:    �����д洢����
*                
*Calls:          
*Called By:      
*Input:          
*Output:
*Return:         0~(MAX_AREA_SUM-1)  ��ͬ����ID�洢λ��  0xff:����ͬID   һ�㲻�᷵��0xff
*Others:
*********************************************************/

static u8 CheckFreeID(void)
{
	u8 i;//ѭ������
	u8 res;
	u32 min_id = 0;

	for(i=0;i<MAX_AREA_SUM;i++)
	{
		if(area_manage.area_attr[i]!= 0xaa)//��������Ϣδ�洢����Ч
		{
			res = i;
			return i;
		}
	}
	//�޿������򣬲�����СID�Ž����滻
	min_id = area_manage.actual_id[0];
	res = 0;
	for(i=1;i<MAX_AREA_SUM;i++)
	{
		if(area_manage.actual_id[i] == 0xaa)//��������Ϣδ�洢����Ч
		{
			if(min_id > area_manage.actual_id[i])
			{
				min_id = area_manage.actual_id[i];
				res = i;
			}
		}
	}
	return res;
}
/*********************************************************
*Function:       SaveAreaDatatoFlash()
*Description:    ��������Ϣ����FLASH
*                
*Calls:          
*Called By:      
*Input:          Pbuf  �·���Ϣָ��
				 SaveNum �洢λ��
				 area_type  ��������  Բ�� ���� �����
*Output:
*Return:         0~(MAX_AREA_SUM-1)  ��ͬ����ID�洢λ��  0xff:����ͬID   һ�㲻�᷵��0xff
*Others:
*********************************************************/

static u8 SaveAreaDatatoFlash(u8 *pbuf,u8 SaveNum,u8 area_type,u16 *readlen)
{
	u8 i;
	u32 U32Temp;
	u16 U16Temp;
	u8 *p_buf = pbuf;
	if(CheckParam1(pbuf)||SaveNum>=MAX_AREA_SUM)//�������
	{
		return 1;
	}
	//���ݴ洢
	/*�ҵ���Ч����׼��д��*/
	Flash_Write_Buffer(0,0,VIRTUAL_AERA_TEMP_ADDR,Sector_Erase);//����ת������
	//��д��������
	area_manage.area_attr[SaveNum] = 0xaa;
	area_manage.area_type[SaveNum] = area_type;
	area_manage.actual_id[SaveNum] = area_manage.max_actual_id;
	char_to_int(pbuf,&U32Temp); 
	area_manage.area_id[SaveNum] = U32Temp;
	area_manage.max_actual_id++;
	Flash_Write_Buffer((u8*)&area_manage,sizeof(Area_Manage_struct),VIRTUAL_AERA_TEMP_ADDR,Write_Mode);
	for(i=0;i<SaveNum;i++)//������Ч����
	{
		Flash_Read_Buffer(W_R_buf,sizeof(Area_attr_struct),VIRTUAL_AERA_MANAGE_ADDR+sizeof(Area_Manage_struct)+sizeof(Area_attr_struct)*i);
		Flash_Write_Buffer(W_R_buf,sizeof(Area_attr_struct),VIRTUAL_AERA_TEMP_ADDR+sizeof(Area_Manage_struct)+sizeof(Area_attr_struct)*i,Write_Mode);
	}
	//д���������
	{
		Area_attr_struct* p_attr = &area_attr_struct;//����д��������Area_attr_struct
		p_attr->area_id = U32Temp;
		p_buf+=4;

		char_to_short(p_buf,&U16Temp);//��ȡ��������
		memcpy((u8*)&(p_attr->area_para),(u8*)&U16Temp,2);
		p_buf += 2;

		/*��������*/
		if(area_type == Circular_Area)//��ȡ��������
		{
			p_attr->area_type = Circular_Area;
			p_buf+=12;
		}
		else if(area_type == Rectangle_Area)//���������������������
		{
			p_attr->area_type = Rectangle_Area;
			p_buf+=16;
			
		}
		else if(area_type == Poly_Area)
		{
			p_attr->area_type = Poly_Area;
		}

		/*д�븽�Ӳ���  ʱ�䷶Χ �ٶȵ�*/
		if(p_attr->area_para.depend_time)/**/
		{
			memcpy(p_attr->start_time,p_buf,6);
			memcpy(p_attr->stop_time,p_buf+6,6);
			p_buf+=12;
		}
		else
		{
			memset(p_attr->start_time,0,6);
			memset(p_attr->stop_time,0,6);
		}

		if(p_attr->area_para.speed_limit)
		{
			char_to_short(p_buf,(u16 *)&(p_attr->max_speed));
			p_attr->continue_time = *(p_buf+2);
			p_buf+=3;
		}
		else
		{
			p_attr->max_speed = 0;
			p_attr->continue_time = 0;
		}
		if(area_type == Circular_Area)
		{
			*readlen = p_buf - pbuf;
			p_attr->point_num= 0;	
		}
		else if(area_type == Rectangle_Area)//��ȡ��������
		{
			p_attr->point_num= 4;
			*readlen = p_buf - pbuf;
		}
		else if(area_type == Poly_Area)
		{
			char_to_short(p_buf,(u16 *)&(p_attr->point_num));
			p_buf+=2;
			if(p_attr->point_num>MAX_AREA_POINTS) //����������
			{
				area_manage.area_attr[SaveNum] = 0x00;
				area_manage.max_actual_id--;  //�ָ�ԭ��ֵ
				AREA_Printf("============����ε������࣬�洢ʧ��============\r\n");
				return 1;
			}
			*readlen = (p_buf - pbuf)+ p_attr->point_num*8;
		}

		p_attr->flag = 0xaa;

		/*д��FLASH*/
		Flash_Write_Buffer((u8*)p_attr,sizeof(Area_attr_struct),VIRTUAL_AERA_TEMP_ADDR+sizeof(Area_Manage_struct)+sizeof(Area_attr_struct)*SaveNum,Write_Mode);
		
		/*д������δ�޸�����*/
		for(i=SaveNum+1;i<MAX_AREA_SUM;i++)//������Ч����
		{
			Flash_Read_Buffer(W_R_buf,sizeof(Area_attr_struct),VIRTUAL_AERA_MANAGE_ADDR+sizeof(Area_Manage_struct)+sizeof(Area_attr_struct)*i);
			Flash_Write_Buffer(W_R_buf,sizeof(Area_attr_struct),VIRTUAL_AERA_TEMP_ADDR+sizeof(Area_Manage_struct)+sizeof(Area_attr_struct)*i,Write_Mode);
		}
		/*������������������*/
		Flash_Write_Buffer(0,0,VIRTUAL_AERA_MANAGE_ADDR,Sector_Erase);
		for(i=0;i<4;i++ )//����һ������
		{
			Flash_Read_Buffer(W_R_buf,sizeof(W_R_buf),VIRTUAL_AERA_TEMP_ADDR+sizeof(W_R_buf)*i);
			Flash_Write_Buffer(W_R_buf,sizeof(W_R_buf),VIRTUAL_AERA_MANAGE_ADDR+sizeof(W_R_buf)*i,Write_Mode);
		}
		
		
		/*������Ϣ��������д��*/
		Flash_Write_Buffer(0,0,VIRTUAL_AERA_TEMP_ADDR,Sector_Erase);//����ת��������

		U32Temp = SaveNum * sizeof(Area_Desc)+VIRTUAL_AERA_DATA_ADDR;//ʵ�ʵ�ַ
		U32Temp = (U32Temp /SECTOR_SIZE)*SECTOR_SIZE;//����������ʼ��ַ
		U16Temp = SaveNum%4;//ÿ��������Ϣ���ݴ�С����1024KB  ��һ���������ô洢 4����ȡ�������õõ��ڸ���������λ��
		
		/*�޹�����ת��*/
		for(i=0;i<U16Temp;i++)//��ȡ��д����ǰ�޹����ݱ��浽��ʱ����
		{
			Flash_Read_Buffer(W_R_buf,sizeof(W_R_buf),U32Temp+sizeof(W_R_buf)*i);
			Flash_Write_Buffer(W_R_buf,sizeof(W_R_buf),VIRTUAL_AERA_TEMP_ADDR+sizeof(W_R_buf)*i,Write_Mode);
		}
		//д��������
		{
			Area_Desc *area_desc = (Area_Desc *)W_R_buf;
	
			if(area_type == Circular_Area)
			{
				p_buf=pbuf+6;
				char_to_int(p_buf,(u32*)&(area_desc->cir_area_desc.lat));
				p_buf+=4;
				char_to_int(p_buf,(u32*)&(area_desc->cir_area_desc.lon));
				p_buf+=4;
				char_to_int(p_buf,(u32*)&(area_desc->cir_area_desc.rad));
				p_buf+=4;
				
				AREA_Printf("�洢Բ����������IDΪ%d\r\nԲ�ľ�γ��Ϊ:%d��,%d��\r\n�뾶Ϊ:%dm\r\n",
							p_attr->area_id,	
						   area_desc->cir_area_desc.lat,area_desc->cir_area_desc.lon,
						   area_desc->cir_area_desc.rad
							);
			}
			else if(area_type == Rectangle_Area)
			{
				p_buf=pbuf+6;
				char_to_int(p_buf,(u32*)&(area_desc->poly_area_desc.area_point[0].Lat));
				char_to_int(p_buf,(u32*)&(area_desc->poly_area_desc.area_point[3].Lat));
				p_buf+=4;
				char_to_int(p_buf,(u32*)&(area_desc->poly_area_desc.area_point[0].Lon));
				char_to_int(p_buf,(u32*)&(area_desc->poly_area_desc.area_point[1].Lon));
				p_buf+=4;
				char_to_int(p_buf,(u32*)&(area_desc->poly_area_desc.area_point[1].Lat));
				char_to_int(p_buf,(u32*)&(area_desc->poly_area_desc.area_point[2].Lat));
				p_buf+=4;
				char_to_int(p_buf,(u32*)&(area_desc->poly_area_desc.area_point[2].Lon));
				char_to_int(p_buf,(u32*)&(area_desc->poly_area_desc.area_point[3].Lon));
				p_buf+=4;
				AREA_Printf("�洢������������IDΪ%d\r\n����γ��Ϊ:%d��,%d��\r\n%d��,%d��\r\n%d��,%d��\r\n%d��,%d��\r\n",
							p_attr->area_id,
						   area_desc->poly_area_desc.area_point[0].Lat,area_desc->poly_area_desc.area_point[0].Lon,
						   area_desc->poly_area_desc.area_point[1].Lat,area_desc->poly_area_desc.area_point[1].Lon,
						   area_desc->poly_area_desc.area_point[2].Lat,area_desc->poly_area_desc.area_point[2].Lon,
						   area_desc->poly_area_desc.area_point[3].Lat,area_desc->poly_area_desc.area_point[3].Lon
							);
			}
			else if(area_type == Poly_Area)
			{
				AREA_Printf("�洢�������������IDΪ%d\r\n",
							p_attr->area_id);
				for(i=0;i<p_attr->point_num;i++)
				{
					char_to_int(p_buf,(u32*)&(area_desc->poly_area_desc.area_point[i].Lat));
					p_buf+=4;
					char_to_int(p_buf,(u32*)&(area_desc->poly_area_desc.area_point[i].Lon));
					p_buf+=4;
					AREA_Printf("����%d����Ϊ:%d��,%d��\r\n",i,
								area_desc->poly_area_desc.area_point[i].Lat,area_desc->poly_area_desc.area_point[i].Lon
								);
				}
			}
			Flash_Write_Buffer(W_R_buf,sizeof(W_R_buf),VIRTUAL_AERA_TEMP_ADDR+sizeof(W_R_buf)*U16Temp,Write_Mode);

			/*�޹�����ת��*/
			for(i=U16Temp+1;i<4;i++)//��ȡ��д����ǰ�޹����ݱ��浽��ʱ����
			{
				Flash_Read_Buffer(W_R_buf,sizeof(W_R_buf),U32Temp+sizeof(W_R_buf)*i);
				Flash_Write_Buffer(W_R_buf,sizeof(W_R_buf),VIRTUAL_AERA_TEMP_ADDR+sizeof(W_R_buf)*i,Write_Mode);
			}

			/*��д����*/
			Flash_Write_Buffer(0,0,U32Temp,Sector_Erase);
			for(i=0;i<4;i++)//��ȡ��д����ǰ�޹����ݱ��浽��ʱ����
			{
				Flash_Read_Buffer(W_R_buf,sizeof(W_R_buf),VIRTUAL_AERA_TEMP_ADDR+sizeof(W_R_buf)*i);
				Flash_Write_Buffer(W_R_buf,sizeof(W_R_buf),U32Temp+sizeof(W_R_buf)*i,Write_Mode);
			}
		}
	}
	return 0;
}


/*********************************************************
*Function:       DelAreaDataData()
*Description:   ɾ��������Ѿ�ʧЧ��������Ϣ
*                
*Calls:          
*Called By:      
*Input:          pbuf��
				 area_num���������
*Output:
*Return:         0��ɾ���ɹ� 1��ɾ��ʧ��
*Others:
*********************************************************/
static u8 DelAreaDataData(u8 save_num)
{
	if(save_num>=MAX_AREA_SUM)
	{
		return 0;
	}
	if(area_manage.area_attr[save_num]==0xaa)
	{
		area_manage.area_attr[save_num]=0x00;
		//���ز�����ֱ�Ӹ���д��
		Flash_Write_Buffer((u8*)&area_manage,sizeof(Area_Manage_struct),VIRTUAL_AERA_MANAGE_ADDR,Write_Mode);
	}
	else
	{
		//����������Ч��	������ȥ��
		return 0;
	}
	return 0;
}
#endif
/*********************************************************
*Function:       CompareTime(u8 *start_time,u8 *stop_time,u8* now_time)
*Description:   ɾ��������Ѿ�ʧЧ��������Ϣ
*                
*Calls:          
*Called By:      
*Input:          start_time����ʼʱ��
				 stop_time����ֹʱ��
				 now_time:��ǰʱ��
				 index:��ʼ�ȶ�λ��
*Output:
*Return:         0����������ʱ���� 1���������� 0xff:����ʱ�������Ҫ����ɾ������������Ϣ
*Others:
*********************************************************/
u8 CompareTime(u8 *start_time,u8 *stop_time,u8* now_time,u8 index)
{
	u8 i;
	u32 start_time_unix=0;
	u32 stop_time_unix=0;
	u32 now_time_unix=0;
	
	if(CheckParam3(start_time,stop_time,now_time)||(index>5))
	{
		return 1;
	}

	switch(index)
	{
		case 0://��Ƚϲ����ǿ��������
			start_time_unix = GetUnixTime(start_time);
			stop_time_unix = GetUnixTime(stop_time);
			now_time_unix = GetUnixTime(now_time);

			if((start_time_unix<=now_time_unix)&&(now_time_unix<=stop_time_unix))
			{
				return 1;
			}
			else if((now_time_unix<=start_time_unix)&&(now_time_unix<stop_time_unix))
			{
				return 2;//δ��Ԥ��ʱ��
			}
			else
			{
				return 0;
			}
//			break;
		case 1://�±Ƚ�
			if(start_time[1] > stop_time[1])//�ж��Ƿ�Ϊ����ʱ��
			{
				start_time[0] = DecToBCD(BCDToDec(now_time[0])-1);
			}
			else
			{
				start_time[0] = now_time[0]; 	
			}
			stop_time[0] = now_time[0];
			start_time_unix = GetUnixTime(start_time);
			stop_time_unix = GetUnixTime(stop_time);
			now_time_unix = GetUnixTime(now_time);

			if((start_time_unix<=now_time_unix)&&(now_time_unix<=stop_time_unix))
			{
				return 1;
			}
			else
			{
				return 0;
			}
//			break;
		case 2://�ձȽ�
			if(start_time[2] > stop_time[2])//�ж��Ƿ�Ϊ����ʱ��
			{
				start_time[1] = DecToBCD(BCDToDec(now_time[1])-1);
				if(start_time[1]==0x00)
				{
					start_time[1] = 0x12;
					start_time[0] = DecToBCD(BCDToDec(now_time[0])-1);
				}
				else
				{
					start_time[0] = now_time[0];
				}
			}
			else
			{
				start_time[0] = now_time[0]; 
				start_time[1] = now_time[1];	
			}
			stop_time[0] = now_time[0];
			stop_time[1] = now_time[1];
			start_time_unix = GetUnixTime(start_time);
			stop_time_unix = GetUnixTime(stop_time);
			now_time_unix = GetUnixTime(now_time);

			if((start_time_unix<=now_time_unix)&&(now_time_unix<=stop_time_unix))
			{
				return 1;
			}
			else
			{
				return 0;
			}
//			break;
		case 3://ʱ�Ƚ�
			if(start_time[3] > stop_time[3])//�ж��Ƿ�Ϊ����ʱ��
			{
				start_time[2] = DecToBCD(BCDToDec(now_time[2])-1);
				if(start_time[2]==0x00)
				{
					switch(now_time[1])
					{
						case 0x03:
							if(BCDToDec(now_time[0])%4==0)//ֻ���Ƿ�4���� �����Ƿ�400����
							{
								start_time[2] = 0x29;
							}
							else
							{
								start_time[2] = 0x28;
							}
							start_time[1] = DecToBCD(BCDToDec(now_time[1])-1);
							start_time[0] = now_time[0];
						break;
						case 0x05:
						case 0x07:
						case 0x10:
						case 0x12:
							start_time[2] = 0x30;
							start_time[1] = DecToBCD(BCDToDec(now_time[1])-1);
							start_time[0] = now_time[0];
						break;
						case 0x02:
						case 0x04:
						case 0x06:
						case 0x08:
						case 0x09:
						case 0x11:
							start_time[2] = 0x31;
							start_time[1] = DecToBCD(BCDToDec(now_time[1])-1);
							start_time[0] = now_time[0];
						break;
						case 0x01:
							start_time[2] = 0x31;
							start_time[1] = 0x12;
							start_time[0] = DecToBCD(BCDToDec(now_time[0])-1);;
						break;
					}
				}
			}
			else
			{
				start_time[2] = now_time[2];
				start_time[1] = now_time[1];
				start_time[0] = now_time[0];
			}

			stop_time[0] = now_time[0];
			stop_time[1] = now_time[1];
			stop_time[2] = now_time[2];
			start_time_unix = GetUnixTime(start_time);
			stop_time_unix = GetUnixTime(stop_time);
			now_time_unix = GetUnixTime(now_time);

			if((start_time_unix<=now_time_unix)&&(now_time_unix<=stop_time_unix))
			{
				return 1;
			}
			else
			{
				return 0;
			}
//			break;
		default://�������������
			return 1;
	}
}

static u32 GetUnixTime(u8 *TimeStr)
{
	struct tm TimeNow;
	
	if(CheckParam1(TimeStr))
	{
		return 1;
	}

	TimeNow.tm_year=100+ ((TimeStr[0]>>4)&0x0F)*10+ (TimeStr[0]&0x0F);
	TimeNow.tm_mon=((TimeStr[1]>>4)&0x0F)*10+ (TimeStr[1]&0x0F)-1;
	TimeNow.tm_mday=((TimeStr[2]>>4)&0x0F)*10+ (TimeStr[2]&0x0F);
	TimeNow.tm_hour=((TimeStr[3]>>4)&0x0F)*10+ (TimeStr[3]&0x0F);
	TimeNow.tm_min=((TimeStr[4]>>4)&0x0F)*10+ (TimeStr[4]&0x0F);
	TimeNow.tm_sec=((TimeStr[5]>>4)&0x0F)*10+ (TimeStr[5]&0x0F);

	return mktime(&TimeNow);
}
#if 0
/*********************************************************
*Function:       Area_Judge()
*Description:    ���ݵ�ǰ��γ�ȣ��ٶ�ֵ�ж��Ƿ���������Ƿ��٣���������Ӧ����
*                
*Calls:          
*Called By:      
*Input:          lon������ֵ
				 lat:γ��ֵ
				 speed���ٶ�ֵ
				 door_state:��״̬  0:����   1:����
				 now_time����ǰʱ��
*Output:
*Return:         0�������� 1��λ1��ʾ�������ٱ���
*Others:
*********************************************************/
u32 Area_Judge(u32 lon,u32 lat,u16 speed,u8 door_state,u8 *now_time)
{
	Area_Desc *area_desc;
	Protocol_Add_Mess * add_mess;
	u8 i,err;//ѭ������
	u8 res = 0;
	u32 result = 0;

	u32 AlarmtoPlat = 0;//�ϱ�������λ0��ʾ֪ͨƽ̨������λ1��ʾ֪ͨƽ̨������
						  //λ2��ʾ֪ͨƽ̨���� ��λ3��ʾ֪ͨƽ̨�Ƿ�����
						  //
	u8 AlarmtoDriver = 0;//�ϱ���ʻԱ��λ0��ʾ֪ͨ��ʻԱ������ λ1��ʾ֪ͨ��ʻԱ������

	if((lon == 0x00)||(lat == 0x00)||CheckParam1(now_time))
	{
		return AlarmtoPlat;
	}
	
	for(i = 0;i<MAX_AREA_SUM;i++)
	{
		if(OSSemAccept(Area_Protect_Sem))
		{
			//��ȡ������Ϣ
			area_desc = GetAreaData(i,now_time);
			//��Ч������ж�
			if(area_desc)//��Ч�������һ���ж�
			{
				res = Area_Judge_Process(lon,lat,area_desc);
				switch(AreaJudgeSta[i])
				{
					case 0://Ĭ��������֮��
						if(res)//��������
						{
							AreaJudgeSta[i] = 1;
							//��������λҪ���ж��Ƿ���֪ͨ
							if(area_attr_struct.area_para.alarm_inside_to_driver)
							{
								//֪ͨ��ʻԱ��������
								AlarmtoDriver|=0x02;
							}
							if(area_attr_struct.area_para.alarm_inside_to_plat)
							{
								//֪ͨƽ̨��������
								AlarmtoPlat |=0x02;
								//���ɸ�����Ϣ
								add_mess =(Protocol_Add_Mess *) GetFreeAddMessBuf();
								if(add_mess)
								{
									add_mess->Mess_ID = Inside_Outside_A_L_enum;
									add_mess->Mess_Len = INSIDE_OUTSIDE_A_L_MESS_LEN;//�̶�����
									add_mess->Mess_Data.OutIn_Area_Mess.Area_Line_ID = area_attr_struct.area_id;
									add_mess->Mess_Data.OutIn_Area_Mess.Area_Type = area_attr_struct.area_type + 1;
									add_mess->Mess_Data.OutIn_Area_Mess.In_Or_Out = 0x00;//������
									OSQPost(AddMessSem,(void *)add_mess);
								}
								
							}
							if(area_attr_struct.area_para.limit_open_door)
							{
								//�ſ����ж�״̬
								AreaDoorOpenSta&=~(1<<i);
								if(door_state)//�Ƿ�����
								{
									AlarmtoPlat|=0x08;
									AreaDoorOpenSta|=(1<<i);
								}
							}
							if(area_attr_struct.area_para.close_wcm_inside)
							{
								//��������ر�ģ��
							}
							if(area_attr_struct.area_para.speed_limit)
							{
								AreaSpeedOverSta&=~(1<<i);//��ո�����ı���״̬
								SpeedOverStartTime[i] = 0;
							}
							AREA_Printf("============������������IDΪ%d============\r\n",area_attr_struct.area_id);
						}
						else
						{
							//�����ⲻ������������״̬
						}
					break;
					case 1://��������
						if(res)
						{
							//����������
							//��������λҪ���ж��Ƿ���֪ͨ
							
							//�Ƿ������ж�
							if(area_attr_struct.area_para.limit_open_door)
							{
								if(AreaDoorOpenSta&(1<<i))//�ѱ����ȴ�����������ʧ
								{
									if(door_state==0)
									{
										AreaDoorOpenSta&=~(1<<i);//���ѹرգ�����ѱ���״̬
									}
								}
								else
								{
									if(door_state)
									{
										AlarmtoPlat|=0x08;//�������ڹ��ţ��ϱ���λ
										AreaDoorOpenSta|=(1<<i);
									}
								}
							}
	
							//�����ڳ����ж�
							if(area_attr_struct.area_para.speed_limit)
							{
								if(AreaSpeedOverSta&(1<<i))//
								{
									//�ѷ�������
									if(speed<area_attr_struct.max_speed)
									{
										AreaSpeedOverSta&=~(1<<i);
										SpeedOverStartTime[i] = 0;
									}
									else
									{
										result = 1;
//										AlarmtoPlat|=0x04;//���ٱ���δ����,�ñ���λ������
									}
								}
								else
								{
									if(speed>area_attr_struct.max_speed)
									{
										AreaSpeedOverSta&=~(1<<i);
										if(SpeedOverStartTime[i] == 0)
										{
											SpeedOverStartTime[i] = OSTimeGet();
										}
										else
										{
											if((OSTimeGet() - SpeedOverStartTime[i])>area_attr_struct.continue_time*100)//�жϳ���ʱ���Ƿ񳬹�����
											{
												AreaSpeedOverSta|=(1<<i);
												SpeedOverStartTime[i] = 0;
												AlarmtoPlat |=0x04;
												result = 1;
												
												//���ɸ�����Ϣ
												add_mess =(Protocol_Add_Mess *) GetFreeAddMessBuf();
												if(add_mess)
												{
													add_mess->Mess_ID = Over_Speed_enum;
													add_mess->Mess_Len = OVER_SPEED_MESS_A_L_LEN;//�̶�����
													add_mess->Mess_Data.OverSpeed_Mess.Over_Type = area_attr_struct.area_type + 1;
													add_mess->Mess_Data.OverSpeed_Mess.Area_Line_ID = area_attr_struct.area_id;
												}
												AREA_Printf("============�����ڳ��٣�����IDΪ%d============\r\n",area_attr_struct.area_id);
											}
										}
									}
									else
									{
										//ʱ������
										SpeedOverStartTime[i] = 0;
									}
								}
							}
							
						}
						else
						{
							//�뿪����
							AreaJudgeSta[i] = 0;//�ı�״̬
							//��������λҪ���ж��Ƿ���֪ͨ
							if(area_attr_struct.area_para.alarm_inside_to_driver)
							{
								//֪ͨ��ʻԱ�뿪����
								AlarmtoDriver|=0x01;
							}
							if(area_attr_struct.area_para.alarm_inside_to_plat)
							{
								//֪ͨƽ̨�뿪����
								AlarmtoPlat |=0x01;

								//���ɸ�����Ϣ
								add_mess =(Protocol_Add_Mess *) GetFreeAddMessBuf();
								if(add_mess)
								{
									add_mess->Mess_ID = Inside_Outside_A_L_enum;
									add_mess->Mess_Len = INSIDE_OUTSIDE_A_L_MESS_LEN;//�̶�����
									add_mess->Mess_Data.OutIn_Area_Mess.Area_Line_ID = area_attr_struct.area_id;
									add_mess->Mess_Data.OutIn_Area_Mess.Area_Type = area_attr_struct.area_type + 1;
									add_mess->Mess_Data.OutIn_Area_Mess.In_Or_Out = 0x01;//������
								}
							}
							AREA_Printf("============�뿪��������IDΪ%d============\r\n",area_attr_struct.area_id);
						}
					break;
					default:
						AreaJudgeSta[i] = 0;//�ı�״̬
					break;
				}
			}
			else
			{
				//״̬λ����
				AreaJudgeSta[i] = 0;
			}
			OSSemPost(Area_Protect_Sem);
		}
	}	
	if(AlarmtoPlat)//��Ҫ�ϱ�ƽ̨  	  ��Ϊ�ں������ϱ�
	{
		//ͨ��λ���ϱ���Ϣ�ϱ�
		if(AlarmtoPlat&0x01)//�������ϱ�
		{
			WriteAlarmPara(SET,StaAlarm0,ALARM_ENTER_AREA);
		}
		if(AlarmtoPlat&0x02)//�������ϱ�
		{
			WriteAlarmPara(SET,StaAlarm0,ALARM_ENTER_AREA);//����������ͬһ������
		}
		if(AlarmtoPlat&0x04)//�����ϱ�
		{
			WriteAlarmPara(SET,StaAlarm0,ALARM_OVER_SPEED);
		}
		if(AlarmtoPlat&0x08)//�Ƿ������ϱ�
		{	
			WriteAlarmPara(SET,StaAlarm0,ALARM_AREAILLEGAL_OPENDOOR);
		}
		if(AlarmtoPlat)
		{
			DevReq2ServPackag_build(REPORT_LOCATION);
		}
	}

	if(AlarmtoDriver)//��Ҫ֪ͨ��ʻԱ
	{
		if(AlarmtoDriver&0x01)//�������ϱ�
		{
			OSFlagPost(Alarm_Driver_Event,AREA_OUT_FLAG,OS_FLAG_SET,&err);
		}
		if(AlarmtoDriver&0x02)//�������ϱ�
		{
			OSFlagPost(Alarm_Driver_Event,AREA_INTO_FLAG,OS_FLAG_SET,&err);
		}
	}
	return result;
}

/*********************************************************
*Function:       Circular_Judge()
*Description:    Բ�������ж�
*                
*Calls:          
*Called By:      
*Input:          pbuf��
				 area_num���������
*Output:
*Return:         0���������� 1����������
*Others:
*********************************************************/
static u8 Area_Judge_Process(u32 now_lon,u32 now_lat,Area_Desc *area_desc)
{
	if((now_lon == 0x00)||(now_lat == 0x00)||CheckParam1(area_desc))
	{
		return 0;
	}
	if(area_attr_struct.area_type == Circular_Area)	
	{
		return Circular_Judge(now_lon,now_lat,&(area_desc->cir_area_desc));
	}
	else
	{
		return poly_Judge(now_lon,now_lat,&(area_desc->poly_area_desc),area_attr_struct.point_num);
	}
}
#endif
/*********************************************************
*Function:       Circular_Judge()
*Description:    Բ�������ж�
*                
*Calls:          
*Called By:      
*Input:          pbuf��
				 area_num���������
*Output:
*Return:         0���������� 1����������
*Others:
*********************************************************/
u8 Circular_Judge(u32 now_lon,u32 now_lat,Cir_Area_Desc *point_data)
{
	FP32 now_lon_f,now_lat_f,Cent_lon_f,Cent_lat_f;

	now_lon_f = (FP32)now_lon/1000000;
	now_lat_f =(FP32)now_lat/1000000;
	Cent_lon_f = (FP32)point_data->lon/1000000;
	Cent_lat_f = (FP32)point_data->lat/1000000;

	if(GPS2Point_Distance(now_lon_f,now_lat_f,Cent_lon_f,Cent_lat_f)*1000>point_data->rad)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*********************************************************
*Function:       poly_Judge()
*Description:    Բ�������ж�
*                
*Calls:          
*Called By:      
*Input:          pbuf��
				 area_num���������
*Output:
*Return:         0���������� 1����������
*Others:
*********************************************************/
double xz;
u8 poly_Judge(u32 lon,u32 lat,Poly_Area_Desc *point_data,u16 polySides)
{
	

	#if 0 
	/*�㷨1*/
	u16 i,j;
	u8 res = 0;
	for (i=0; i<polySides; i++)
	{
	    j++; 
	    if (j==polySides) j=0;
	    if (point_data->area_point[i].Lat<lat && point_data->area_point[j].Lat>=lat
	    ||  point_data->area_point[j].Lat<lat && point_data->area_point[i].Lat>=lat)
	     {
	      if (point_data->area_point[i].Lon+(lat-point_data->area_point[i].Lat)/(point_data->area_point[j].Lat-point_data->area_point[i].Lat)*(point_data->area_point[j].Lon-point_data->area_point[i].Lon)<lon) 
	          {
	              res=!res; 
	           
	           }
	      }
	}
	return res;

	#else
	/*�㷨2*/
	int  i;
	int res =0;
	for (i = 0; i < polySides; i++)
	{
	u32 p1lon = point_data->area_point[i].Lon; 
	u32 p1lat = point_data->area_point[i].Lat;
	u32 p2lon = point_data->area_point[(i + 1) % polySides].Lon;
	u32 p2lat = point_data->area_point[(i + 1) % polySides].Lat;
	if ( p1lat == p2lat ) // p1p2 �� y=p0.yƽ�� 
	   continue;
	if ( lat < min(p1lat, p2lat) ) // ������p1p2�ӳ����� 
	   continue; 
	if ( lat >= max(p1lat, p2lat) ) // ������p1p2�ӳ����� 
	   continue;
	xz = (((double)lat - (double)p1lat) *(((double)p2lon - (double)p1lon) / ((double)p2lat - (double)p1lat))) + (double)p1lon;
//	xz = ((double)lat - (double)p1lat);
//	xz *= ((double)p2lon - (double)p1lon);
//	xz /= ((double)p2lat - (double)p1lat);
//	xz +=  (double)p1lon;
	if ( xz > lon ) 
	  res++;
	}
	return (res % 2 == 1); 
	#endif
}



