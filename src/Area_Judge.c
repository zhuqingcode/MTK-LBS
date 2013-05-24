/*
*********************************************************************************************************
*	                                  
*	模块名称 : 区域报警模块
*	文件名称 : Area_Judge.c
*	版    本 : V1.0
*	说    明 : 区域信息存取及判断
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2013-4-1  魏江南  ST固件库V3.5.0版本。代码编写，未完成区域内关闭模块，关闭GNSS定位，区域内非法开门未测
*		v1.2    2013-4-22 魏江南  增加附加信息生成 ，存储和删除时增加区域类型判断，增加debug打印
*	Copyright (C), 2010-2011, 天朗科技
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

/*变量声明*/
//extern OS_FLAG_GRP* Alarm_Driver_Event;
//static Area_Manage_struct area_manage;
//static Area_attr_struct  area_attr_struct;
//static u8 AreaJudgeSta[MAX_AREA_SUM]; //区域判断状态组
//static u32 AreaSpeedOverSta; //超速判断状态位组，一位表示一个超速报警已上报
//static u32 AreaDoorOpenSta; //门开关判断状态位组，一位表示一个区域的非法开门报警已上报
//static u32 SpeedOverStartTime[MAX_AREA_SUM];//超速判断起始时间组

//static OS_EVENT *Area_Protect_Sem = 0;

//Area_Desc area_desc;
//static u8 W_R_buf[1024];/*读写缓区*/

/*内部函数接口声明*/
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
*Description:    保存区域数值
*                
*Calls:          
*Called By:      
*Input:          pbuf：区域数据
				 area_type：区域类型 圆形 矩形 多边形
				 point_num：点数  当为多边形区域时有效
				 option：写入类型  0：更新区域 1:追加区域 2:修改区域
*Output:
*Return:         0：保存成功 1：保存失败
*Others:
*********************************************************/
u8 SaveAreaData(u8 *pbuf,u8 area_type,u8 option,u16 *readlen)
{
	u8 res;//循环变量
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
	if(option == 0x00)//更新指令？
	{
		//Reset_Area_ALL();//擦除所有信息	
		//reset_all_area_file(area_type);
		del_area_data(0/*has no effect*/, area_type, all_area);
	}
#if 0
	if(area_manage.manage_vail_flag == 0xaa)//判断管理存储数据有效
	{
		char_to_int(pbuf,&New_Area_ID);//获取新ID
		/*检查是否有相同存储ID的区域,如果有，无论下发指令是否为追加区域，一律对原区域信息做替换*/
		SaveNum = CheckSameID(New_Area_ID,area_type);
		if(SaveNum!=0xff)
		{
			//存入对应位置
			res = SaveAreaDatatoFlash(pbuf,SaveNum,area_type,readlen);//存入对应位置
			//OSSemPost(Area_Protect_Sem);
			return res;
		}

		/*检查空闲的区域,如果有则返回对应位置，如果没有则替换最早存入的区域信息*/
		SaveNum = CheckFreeID();
		if(SaveNum!=0xff)
		{
			//存入对应位置
			res = SaveAreaDatatoFlash(pbuf,SaveNum,area_type,readlen);//存入对应位置
			//OSSemPost(Area_Protect_Sem);
			return res;
		}
	}
	else
	{
		Reset_Area_ALL();//擦除所有信息
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
*Input:          save_num：区域序号
				 now_time：当前时间段
*Output:
*Return:         0：读取成功 1：读取失败
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
			//判断本区域当前是否有效
			if(area_attr_struct.flag==0xaa)
			{
				res = 1;
				if(area_attr_struct.area_para.depend_time)//需要对时间判断
				{
					if((now_time[0]==0x00)&&(now_time[1]==0x00)&&(now_time[2]==0x00))//判断输入时间有效性
					{
						res = 0;
					}
					else
					{
						if((area_attr_struct.start_time[0]==0x00)&&(area_attr_struct.stop_time[0]==0x00))//时间设定为周期时间
						{
							if((area_attr_struct.start_time[1]!=0x00)&&(area_attr_struct.stop_time[1]!=0x00))//年周期时间
							{
								//判断当前时分秒在设定时间段内
								res = CompareTime(area_attr_struct.start_time,area_attr_struct.stop_time,now_time,1);
							}
							else if((area_attr_struct.start_time[2]!=0x00)&&(area_attr_struct.stop_time[2]!=0x00))//月周期时间
							{
								//判断当前时分秒在设定时间段内
								res = CompareTime(area_attr_struct.start_time,area_attr_struct.stop_time,now_time,2);
							}
							else //日周期时间，不做时周期和分周期判断
							{
								//判断当前时分秒在设定时间段内
								res = CompareTime(area_attr_struct.start_time,area_attr_struct.stop_time,now_time,3);
							}
						}
						else//设定非周期时间
						{
							res = CompareTime(area_attr_struct.start_time,area_attr_struct.stop_time,now_time,0);
							if(!res)
							{
								//数据过期失效，删除处理 返回为0
								DelAreaDataData(save_num);
							}
							if(res == 2)
							{
								res = 0; //未到预定时间
							}
						}
					}
				}
				if(res ==0)//不在检测时段内
				{
					return NULL;
				}
				//读出数据，并作简易有效性判断
				Flash_Read_Buffer((u8 *)area_desc,sizeof(Area_Desc),save_num * sizeof(Area_Desc)+VIRTUAL_AERA_DATA_ADDR);
				if((area_desc->poly_area_desc.area_point[0].Lon!=0xffffffff)&&(area_desc->poly_area_desc.area_point[0].Lon!=0x0))// 
				{
					return area_desc;
				}
				else
				{
					//数据失效，删除处理
					DelAreaDataData(save_num);
					return NULL;
				}
			}
			else
			{
				//本区域无效,做删除处理
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
*Description:    删除区域信息
*                
*Calls:          
*Called By:      
*Input:          
*Output:
*Return:         0：删除成功 1：删除失败
*Others:
*********************************************************/
u8 DelAreaData(u8* pbuf,u8 areatype)
{
	u32 New_Area_ID = 0;
	u8 SaveNum,err;
	u8 i, ret;
	
	if(pbuf)   //删除指定区域ID
	{
#if 0
		OSSemPend(Area_Protect_Sem,0,&err);
		if(err != OS_ERR_NONE)
		{
			return 1;
		}
#endif
		char_to_int(pbuf,&New_Area_ID);//获取新ID
		ret = del_area_data(New_Area_ID, areatype, one_area);
		if (ret == 0){
			OA_DEBUG_USER("======delete area ok======");
		}
		else OA_DEBUG_USER("======delete area failed======");
#if 0
		/*检查是否有相同存储ID的区域,如果有，无论下发指令是否为追加区域，一律对原区域信息做替换*/
		SaveNum = CheckSameID(New_Area_ID,areatype);
		if(SaveNum!=0xff)
		{
			//删除对应位置
			DelAreaDataData(SaveNum);//删除对应位置
			OSSemPost(Area_Protect_Sem);
			AREA_Printf("============找到对应区域信息,存储位置:%d,删除成功=============\r\n",SaveNum);
			return 0;
		}
		else
		{
			OSSemPost(Area_Protect_Sem);
			AREA_Printf("============未找到对应区域信息,删除失败=============\r\n");
			return 1;
		}
#endif
	}
	else  //删除所有同一类区域
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
			//遍历同类区域类型
			if(area_manage.area_attr[i]==0xaa)
			{
				if((area_manage.area_type[i]==areatype))
				{
					DelAreaDataData(i);//删除对应位置	
				}
			}
		}
		OSSemPost(Area_Protect_Sem);
		AREA_Printf("============找到类型区域信息,删除类型:%d,删除成功=============\r\n",areatype);
		return 0;	
#endif
	}
}
#if 0
/*********************************************************
*Function:       Area_Judge_Init()
*Description:    区域判断模块初始化
*                
*Calls:          
*Called By:      
*Input:          
*Output:
*Return:         0：读取成功 1：读取失败
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
	if(area_manage.manage_vail_flag!=0xaa) //当前值无效
	{
		Reset_Area_ALL();
		AREA_Printf("============初始化电子围栏失败，复位管理结构体=============\r\n");
	}
	OSSemPost(Area_Protect_Sem);
	return 0;	
}
#endif
#if 0
/*********************************************************
*Function:       Reset_Area_ALL()
*Description:    复位，清空所有已存区域信息
*                
*Calls:          
*Called By:      
*Input:          pbuf：
				 area_num：区域序号
*Output:
*Return:         0：读取成功 1：读取失败
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
*Description:    检查相同区域ID的区域号
*                
*Calls:          
*Called By:      
*Input:          
*Output:
*Return:         0~(MAX_AREA_SUM-1)  相同区域ID存储位置  0xff:无相同ID
*Others:
*********************************************************/
static u8 CheckSameID(u32 NewID,u8 area_type)
{
	u8 res=0xff;
	u8 i;//循环变量

	for(i=0;i<MAX_AREA_SUM;i++)
	{
		if(area_manage.area_attr[i] == 0xaa)//该区域信息有效
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
*Description:    检查空闲存储区域
*                
*Calls:          
*Called By:      
*Input:          
*Output:
*Return:         0~(MAX_AREA_SUM-1)  相同区域ID存储位置  0xff:无相同ID   一般不会返回0xff
*Others:
*********************************************************/

static u8 CheckFreeID(void)
{
	u8 i;//循环变量
	u8 res;
	u32 min_id = 0;

	for(i=0;i<MAX_AREA_SUM;i++)
	{
		if(area_manage.area_attr[i]!= 0xaa)//该区域信息未存储或无效
		{
			res = i;
			return i;
		}
	}
	//无空闲区域，查找最小ID号进行替换
	min_id = area_manage.actual_id[0];
	res = 0;
	for(i=1;i<MAX_AREA_SUM;i++)
	{
		if(area_manage.actual_id[i] == 0xaa)//该区域信息未存储或无效
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
*Description:    将区域信息存入FLASH
*                
*Calls:          
*Called By:      
*Input:          Pbuf  下发信息指针
				 SaveNum 存储位置
				 area_type  区域类型  圆形 矩形 多边形
*Output:
*Return:         0~(MAX_AREA_SUM-1)  相同区域ID存储位置  0xff:无相同ID   一般不会返回0xff
*Others:
*********************************************************/

static u8 SaveAreaDatatoFlash(u8 *pbuf,u8 SaveNum,u8 area_type,u16 *readlen)
{
	u8 i;
	u32 U32Temp;
	u16 U16Temp;
	u8 *p_buf = pbuf;
	if(CheckParam1(pbuf)||SaveNum>=MAX_AREA_SUM)//参数检查
	{
		return 1;
	}
	//数据存储
	/*找到无效区域，准备写入*/
	Flash_Write_Buffer(0,0,VIRTUAL_AERA_TEMP_ADDR,Sector_Erase);//擦除转移扇区
	//改写管理数据
	area_manage.area_attr[SaveNum] = 0xaa;
	area_manage.area_type[SaveNum] = area_type;
	area_manage.actual_id[SaveNum] = area_manage.max_actual_id;
	char_to_int(pbuf,&U32Temp); 
	area_manage.area_id[SaveNum] = U32Temp;
	area_manage.max_actual_id++;
	Flash_Write_Buffer((u8*)&area_manage,sizeof(Area_Manage_struct),VIRTUAL_AERA_TEMP_ADDR,Write_Mode);
	for(i=0;i<SaveNum;i++)//复制有效内容
	{
		Flash_Read_Buffer(W_R_buf,sizeof(Area_attr_struct),VIRTUAL_AERA_MANAGE_ADDR+sizeof(Area_Manage_struct)+sizeof(Area_attr_struct)*i);
		Flash_Write_Buffer(W_R_buf,sizeof(Area_attr_struct),VIRTUAL_AERA_TEMP_ADDR+sizeof(Area_Manage_struct)+sizeof(Area_attr_struct)*i,Write_Mode);
	}
	//写入更新内容
	{
		Area_attr_struct* p_attr = &area_attr_struct;//将读写区域用作Area_attr_struct
		p_attr->area_id = U32Temp;
		p_buf+=4;

		char_to_short(p_buf,&U16Temp);//获取区域属性
		memcpy((u8*)&(p_attr->area_para),(u8*)&U16Temp,2);
		p_buf += 2;

		/*区域类型*/
		if(area_type == Circular_Area)//获取区域类型
		{
			p_attr->area_type = Circular_Area;
			p_buf+=12;
		}
		else if(area_type == Rectangle_Area)//矩形区域当做多边形区域处理
		{
			p_attr->area_type = Rectangle_Area;
			p_buf+=16;
			
		}
		else if(area_type == Poly_Area)
		{
			p_attr->area_type = Poly_Area;
		}

		/*写入附加参数  时间范围 速度等*/
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
		else if(area_type == Rectangle_Area)//获取区域类型
		{
			p_attr->point_num= 4;
			*readlen = p_buf - pbuf;
		}
		else if(area_type == Poly_Area)
		{
			char_to_short(p_buf,(u16 *)&(p_attr->point_num));
			p_buf+=2;
			if(p_attr->point_num>MAX_AREA_POINTS) //超出最大点数
			{
				area_manage.area_attr[SaveNum] = 0x00;
				area_manage.max_actual_id--;  //恢复原先值
				AREA_Printf("============多边形点数过多，存储失败============\r\n");
				return 1;
			}
			*readlen = (p_buf - pbuf)+ p_attr->point_num*8;
		}

		p_attr->flag = 0xaa;

		/*写入FLASH*/
		Flash_Write_Buffer((u8*)p_attr,sizeof(Area_attr_struct),VIRTUAL_AERA_TEMP_ADDR+sizeof(Area_Manage_struct)+sizeof(Area_attr_struct)*SaveNum,Write_Mode);
		
		/*写入其余未修改数据*/
		for(i=SaveNum+1;i<MAX_AREA_SUM;i++)//复制有效内容
		{
			Flash_Read_Buffer(W_R_buf,sizeof(Area_attr_struct),VIRTUAL_AERA_MANAGE_ADDR+sizeof(Area_Manage_struct)+sizeof(Area_attr_struct)*i);
			Flash_Write_Buffer(W_R_buf,sizeof(Area_attr_struct),VIRTUAL_AERA_TEMP_ADDR+sizeof(Area_Manage_struct)+sizeof(Area_attr_struct)*i,Write_Mode);
		}
		/*管理扇区擦除，更新*/
		Flash_Write_Buffer(0,0,VIRTUAL_AERA_MANAGE_ADDR,Sector_Erase);
		for(i=0;i<4;i++ )//正好一个扇区
		{
			Flash_Read_Buffer(W_R_buf,sizeof(W_R_buf),VIRTUAL_AERA_TEMP_ADDR+sizeof(W_R_buf)*i);
			Flash_Write_Buffer(W_R_buf,sizeof(W_R_buf),VIRTUAL_AERA_MANAGE_ADDR+sizeof(W_R_buf)*i,Write_Mode);
		}
		
		
		/*区域信息具体数据写入*/
		Flash_Write_Buffer(0,0,VIRTUAL_AERA_TEMP_ADDR,Sector_Erase);//擦除转移数据区

		U32Temp = SaveNum * sizeof(Area_Desc)+VIRTUAL_AERA_DATA_ADDR;//实际地址
		U32Temp = (U32Temp /SECTOR_SIZE)*SECTOR_SIZE;//所在扇区起始地址
		U16Temp = SaveNum%4;//每段区域信息数据大小正好1024KB  ，一个扇区正好存储 4个，取余数正好得到在该扇区所处位置
		
		/*无关数据转存*/
		for(i=0;i<U16Temp;i++)//读取代写区域前无关数据保存到临时扇区
		{
			Flash_Read_Buffer(W_R_buf,sizeof(W_R_buf),U32Temp+sizeof(W_R_buf)*i);
			Flash_Write_Buffer(W_R_buf,sizeof(W_R_buf),VIRTUAL_AERA_TEMP_ADDR+sizeof(W_R_buf)*i,Write_Mode);
		}
		//写入新数据
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
				
				AREA_Printf("存储圆形区域，区域ID为%d\r\n圆心经纬度为:%d°,%d°\r\n半径为:%dm\r\n",
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
				AREA_Printf("存储矩形区域，区域ID为%d\r\n区域经纬度为:%d°,%d°\r\n%d°,%d°\r\n%d°,%d°\r\n%d°,%d°\r\n",
							p_attr->area_id,
						   area_desc->poly_area_desc.area_point[0].Lat,area_desc->poly_area_desc.area_point[0].Lon,
						   area_desc->poly_area_desc.area_point[1].Lat,area_desc->poly_area_desc.area_point[1].Lon,
						   area_desc->poly_area_desc.area_point[2].Lat,area_desc->poly_area_desc.area_point[2].Lon,
						   area_desc->poly_area_desc.area_point[3].Lat,area_desc->poly_area_desc.area_point[3].Lon
							);
			}
			else if(area_type == Poly_Area)
			{
				AREA_Printf("存储多边形区域，区域ID为%d\r\n",
							p_attr->area_id);
				for(i=0;i<p_attr->point_num;i++)
				{
					char_to_int(p_buf,(u32*)&(area_desc->poly_area_desc.area_point[i].Lat));
					p_buf+=4;
					char_to_int(p_buf,(u32*)&(area_desc->poly_area_desc.area_point[i].Lon));
					p_buf+=4;
					AREA_Printf("顶点%d坐标为:%d°,%d°\r\n",i,
								area_desc->poly_area_desc.area_point[i].Lat,area_desc->poly_area_desc.area_point[i].Lon
								);
				}
			}
			Flash_Write_Buffer(W_R_buf,sizeof(W_R_buf),VIRTUAL_AERA_TEMP_ADDR+sizeof(W_R_buf)*U16Temp,Write_Mode);

			/*无关数据转存*/
			for(i=U16Temp+1;i<4;i++)//读取代写区域前无关数据保存到临时扇区
			{
				Flash_Read_Buffer(W_R_buf,sizeof(W_R_buf),U32Temp+sizeof(W_R_buf)*i);
				Flash_Write_Buffer(W_R_buf,sizeof(W_R_buf),VIRTUAL_AERA_TEMP_ADDR+sizeof(W_R_buf)*i,Write_Mode);
			}

			/*回写数据*/
			Flash_Write_Buffer(0,0,U32Temp,Sector_Erase);
			for(i=0;i<4;i++)//读取代写区域前无关数据保存到临时扇区
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
*Description:   删除错误或已经失效的区域信息
*                
*Calls:          
*Called By:      
*Input:          pbuf：
				 area_num：区域序号
*Output:
*Return:         0：删除成功 1：删除失败
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
		//不必擦除，直接覆盖写入
		Flash_Write_Buffer((u8*)&area_manage,sizeof(Area_Manage_struct),VIRTUAL_AERA_MANAGE_ADDR,Write_Mode);
	}
	else
	{
		//本来就是无效的	，不用去管
		return 0;
	}
	return 0;
}
#endif
/*********************************************************
*Function:       CompareTime(u8 *start_time,u8 *stop_time,u8* now_time)
*Description:   删除错误或已经失效的区域信息
*                
*Calls:          
*Called By:      
*Input:          start_time：起始时间
				 stop_time：终止时间
				 now_time:当前时间
				 index:开始比对位置
*Output:
*Return:         0：不在区域时间内 1：在区域内 0xff:区域时间出错，需要调用删除本段区域信息
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
		case 0://年比较不考虑跨世纪情况
			start_time_unix = GetUnixTime(start_time);
			stop_time_unix = GetUnixTime(stop_time);
			now_time_unix = GetUnixTime(now_time);

			if((start_time_unix<=now_time_unix)&&(now_time_unix<=stop_time_unix))
			{
				return 1;
			}
			else if((now_time_unix<=start_time_unix)&&(now_time_unix<stop_time_unix))
			{
				return 2;//未到预定时间
			}
			else
			{
				return 0;
			}
//			break;
		case 1://月比较
			if(start_time[1] > stop_time[1])//判断是否为跨年时间
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
		case 2://日比较
			if(start_time[2] > stop_time[2])//判断是否为跨月时间
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
		case 3://时比较
			if(start_time[3] > stop_time[3])//判断是否为跨日时间
			{
				start_time[2] = DecToBCD(BCDToDec(now_time[2])-1);
				if(start_time[2]==0x00)
				{
					switch(now_time[1])
					{
						case 0x03:
							if(BCDToDec(now_time[0])%4==0)//只考虑逢4闰年 不考虑逢400闰年
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
		default://其他情况不考虑
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
*Description:    根据当前经纬度，速度值判断是否进出区域，是否超速，并作出相应操作
*                
*Calls:          
*Called By:      
*Input:          lon：经度值
				 lat:纬度值
				 speed：速度值
				 door_state:门状态  0:关门   1:开门
				 now_time：当前时间
*Output:
*Return:         0：无意义 1：位1表示发生超速报警
*Others:
*********************************************************/
u32 Area_Judge(u32 lon,u32 lat,u16 speed,u8 door_state,u8 *now_time)
{
	Area_Desc *area_desc;
	Protocol_Add_Mess * add_mess;
	u8 i,err;//循环变量
	u8 res = 0;
	u32 result = 0;

	u32 AlarmtoPlat = 0;//上报警报，位0表示通知平台出区域，位1表示通知平台进区域，
						  //位2表示通知平台超速 ，位3表示通知平台非法开门
						  //
	u8 AlarmtoDriver = 0;//上报驾驶员，位0表示通知驾驶员出区域 位1表示通知驾驶员进区域

	if((lon == 0x00)||(lat == 0x00)||CheckParam1(now_time))
	{
		return AlarmtoPlat;
	}
	
	for(i = 0;i<MAX_AREA_SUM;i++)
	{
		if(OSSemAccept(Area_Protect_Sem))
		{
			//获取区域信息
			area_desc = GetAreaData(i,now_time);
			//有效则进入判断
			if(area_desc)//有效则进入下一步判断
			{
				res = Area_Judge_Process(lon,lat,area_desc);
				switch(AreaJudgeSta[i])
				{
					case 0://默认在区域之外
						if(res)//进入区域
						{
							AreaJudgeSta[i] = 1;
							//根据属性位要求，判断是否发送通知
							if(area_attr_struct.area_para.alarm_inside_to_driver)
							{
								//通知驾驶员进入区域
								AlarmtoDriver|=0x02;
							}
							if(area_attr_struct.area_para.alarm_inside_to_plat)
							{
								//通知平台进入区域
								AlarmtoPlat |=0x02;
								//生成附加信息
								add_mess =(Protocol_Add_Mess *) GetFreeAddMessBuf();
								if(add_mess)
								{
									add_mess->Mess_ID = Inside_Outside_A_L_enum;
									add_mess->Mess_Len = INSIDE_OUTSIDE_A_L_MESS_LEN;//固定长度
									add_mess->Mess_Data.OutIn_Area_Mess.Area_Line_ID = area_attr_struct.area_id;
									add_mess->Mess_Data.OutIn_Area_Mess.Area_Type = area_attr_struct.area_type + 1;
									add_mess->Mess_Data.OutIn_Area_Mess.In_Or_Out = 0x00;//进区域
									OSQPost(AddMessSem,(void *)add_mess);
								}
								
							}
							if(area_attr_struct.area_para.limit_open_door)
							{
								//门开关判断状态
								AreaDoorOpenSta&=~(1<<i);
								if(door_state)//非法开门
								{
									AlarmtoPlat|=0x08;
									AreaDoorOpenSta|=(1<<i);
								}
							}
							if(area_attr_struct.area_para.close_wcm_inside)
							{
								//进入区域关闭模块
							}
							if(area_attr_struct.area_para.speed_limit)
							{
								AreaSpeedOverSta&=~(1<<i);//清空该区域的报警状态
								SpeedOverStartTime[i] = 0;
							}
							AREA_Printf("============进入区域，区域ID为%d============\r\n",area_attr_struct.area_id);
						}
						else
						{
							//区域外不做动作，保持状态
						}
					break;
					case 1://在区域内
						if(res)
						{
							//仍在区域内
							//根据属性位要求，判断是否发送通知
							
							//非法开门判断
							if(area_attr_struct.area_para.limit_open_door)
							{
								if(AreaDoorOpenSta&(1<<i))//已报警等待报警条件消失
								{
									if(door_state==0)
									{
										AreaDoorOpenSta&=~(1<<i);//门已关闭，清空已报警状态
									}
								}
								else
								{
									if(door_state)
									{
										AlarmtoPlat|=0x08;//在区域内关门，上报该位
										AreaDoorOpenSta|=(1<<i);
									}
								}
							}
	
							//区域内超速判断
							if(area_attr_struct.area_para.speed_limit)
							{
								if(AreaSpeedOverSta&(1<<i))//
								{
									//已发生报警
									if(speed<area_attr_struct.max_speed)
									{
										AreaSpeedOverSta&=~(1<<i);
										SpeedOverStartTime[i] = 0;
									}
									else
									{
										result = 1;
//										AlarmtoPlat|=0x04;//超速报警未消除,该报警位将持续
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
											if((OSTimeGet() - SpeedOverStartTime[i])>area_attr_struct.continue_time*100)//判断超速时间是否超过限制
											{
												AreaSpeedOverSta|=(1<<i);
												SpeedOverStartTime[i] = 0;
												AlarmtoPlat |=0x04;
												result = 1;
												
												//生成附加信息
												add_mess =(Protocol_Add_Mess *) GetFreeAddMessBuf();
												if(add_mess)
												{
													add_mess->Mess_ID = Over_Speed_enum;
													add_mess->Mess_Len = OVER_SPEED_MESS_A_L_LEN;//固定长度
													add_mess->Mess_Data.OverSpeed_Mess.Over_Type = area_attr_struct.area_type + 1;
													add_mess->Mess_Data.OverSpeed_Mess.Area_Line_ID = area_attr_struct.area_id;
												}
												AREA_Printf("============区域内超速，区域ID为%d============\r\n",area_attr_struct.area_id);
											}
										}
									}
									else
									{
										//时间清零
										SpeedOverStartTime[i] = 0;
									}
								}
							}
							
						}
						else
						{
							//离开区域
							AreaJudgeSta[i] = 0;//改变状态
							//根据属性位要求，判断是否发送通知
							if(area_attr_struct.area_para.alarm_inside_to_driver)
							{
								//通知驾驶员离开区域
								AlarmtoDriver|=0x01;
							}
							if(area_attr_struct.area_para.alarm_inside_to_plat)
							{
								//通知平台离开区域
								AlarmtoPlat |=0x01;

								//生成附加信息
								add_mess =(Protocol_Add_Mess *) GetFreeAddMessBuf();
								if(add_mess)
								{
									add_mess->Mess_ID = Inside_Outside_A_L_enum;
									add_mess->Mess_Len = INSIDE_OUTSIDE_A_L_MESS_LEN;//固定长度
									add_mess->Mess_Data.OutIn_Area_Mess.Area_Line_ID = area_attr_struct.area_id;
									add_mess->Mess_Data.OutIn_Area_Mess.Area_Type = area_attr_struct.area_type + 1;
									add_mess->Mess_Data.OutIn_Area_Mess.In_Or_Out = 0x01;//进区域
								}
							}
							AREA_Printf("============离开区域，区域ID为%d============\r\n",area_attr_struct.area_id);
						}
					break;
					default:
						AreaJudgeSta[i] = 0;//改变状态
					break;
				}
			}
			else
			{
				//状态位置零
				AreaJudgeSta[i] = 0;
			}
			OSSemPost(Area_Protect_Sem);
		}
	}	
	if(AlarmtoPlat)//需要上报平台  	  改为在函数外上报
	{
		//通过位置上报信息上报
		if(AlarmtoPlat&0x01)//出区域上报
		{
			WriteAlarmPara(SET,StaAlarm0,ALARM_ENTER_AREA);
		}
		if(AlarmtoPlat&0x02)//进区域上报
		{
			WriteAlarmPara(SET,StaAlarm0,ALARM_ENTER_AREA);//进出区域是同一个报警
		}
		if(AlarmtoPlat&0x04)//超速上报
		{
			WriteAlarmPara(SET,StaAlarm0,ALARM_OVER_SPEED);
		}
		if(AlarmtoPlat&0x08)//非法开门上报
		{	
			WriteAlarmPara(SET,StaAlarm0,ALARM_AREAILLEGAL_OPENDOOR);
		}
		if(AlarmtoPlat)
		{
			DevReq2ServPackag_build(REPORT_LOCATION);
		}
	}

	if(AlarmtoDriver)//需要通知驾驶员
	{
		if(AlarmtoDriver&0x01)//出区域上报
		{
			OSFlagPost(Alarm_Driver_Event,AREA_OUT_FLAG,OS_FLAG_SET,&err);
		}
		if(AlarmtoDriver&0x02)//进区域上报
		{
			OSFlagPost(Alarm_Driver_Event,AREA_INTO_FLAG,OS_FLAG_SET,&err);
		}
	}
	return result;
}

/*********************************************************
*Function:       Circular_Judge()
*Description:    圆形区域判断
*                
*Calls:          
*Called By:      
*Input:          pbuf：
				 area_num：区域序号
*Output:
*Return:         0：在区域外 1：在区域内
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
*Description:    圆形区域判断
*                
*Calls:          
*Called By:      
*Input:          pbuf：
				 area_num：区域序号
*Output:
*Return:         0：在区域外 1：在区域内
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
*Description:    圆形区域判断
*                
*Calls:          
*Called By:      
*Input:          pbuf：
				 area_num：区域序号
*Output:
*Return:         0：在区域外 1：在区域内
*Others:
*********************************************************/
double xz;
u8 poly_Judge(u32 lon,u32 lat,Poly_Area_Desc *point_data,u16 polySides)
{
	

	#if 0 
	/*算法1*/
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
	/*算法2*/
	int  i;
	int res =0;
	for (i = 0; i < polySides; i++)
	{
	u32 p1lon = point_data->area_point[i].Lon; 
	u32 p1lat = point_data->area_point[i].Lat;
	u32 p2lon = point_data->area_point[(i + 1) % polySides].Lon;
	u32 p2lat = point_data->area_point[(i + 1) % polySides].Lat;
	if ( p1lat == p2lat ) // p1p2 与 y=p0.y平行 
	   continue;
	if ( lat < min(p1lat, p2lat) ) // 交点在p1p2延长线上 
	   continue; 
	if ( lat >= max(p1lat, p2lat) ) // 交点在p1p2延长线上 
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



