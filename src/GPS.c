/*******************************************************************************************************
*	                                  
*	ģ������ : (%s:%s:%d):ģ������
*	�ļ����� : GPS.c
*	��    �� : V1.0
*	˵    �� : GPS��Ӳ����ʼ�������ݽ��������̼���
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2011-1-12 κ����  ST�̼���V3.5.0�汾��
*
*	Copyright (C), 2010-2012, ���ʿƼ�
*
*******************************************************************************************************/
//#include "gps.h"
//#include "lib_inc.h"
//#include "Hardware.h"
//--------------add by zhuqing--------------
#include "oa_type.h"
#include "oa_gps.h"
#include "oa_api.h"
#include "oa_debug.h"
#include <math.h>
extern gps_data_struct gps_data;
#define CheckParam1(param1) ((param1) == NULL)
#define CheckParam2(param1,param2) (((param1) == NULL)||((param2) == NULL))
#define CheckParam3(param1,param2,param3) (((param1) == NULL)||((param2)== NULL)||((param3) == NULL))
#define CheckParam4(param1,param2,param3,param4) (((param1) == NULL)||((param2) == NULL)||((param3) == NULL)||((param4) == NULL))

u32 powme(u8 a, u8 b)
{
	u8 i;
	u32 result = 1;
	for (i = 0; i < b; i++){
		result *= a;	
	}
	return result;
}
//-----------------------------------------
#ifdef NO_UBLOX
const u8 HotStart[]="$PSRF104,0,0,0,96000,237759,922,12,00*2B\r\n"; //����������
#endif

/*��������*/
#define POS_AREA  8										/*�ڰ�ʱ��*/
#define R 6371.004
#define Pi 3.1415926
/*DMA���սṹ��*/
//static u8 DMA_ReadATBuf[GPS_DMA_RCV_BUF_SIZE]; //DMA���ݽ��ջ���
//static RCV_DMA_QUEUE GPS_Rcv_Buf;/*DMA ���մ�����*/

/*�������ݽṹ����*/	
STRUCT_RMC Pos_Inf={0};/*GPS��λ��Ϣ�洢�ṹ��*/
static GPS_Handle GPSHandle={0};/*GPSģ���ڲ�����״̬*/

//static OS_EVENT *GPS_SendSem=0;	/*GPS���ڷ����ź���������ǰҪ��ȡ*/
//static OS_EVENT *GPS_DataSem=0;	/*GPS���ݸ����ź�������ֹʹ��ʱ������*/

/*�ڲ����ú���*/
#ifdef UBLOX
 u8 GPS_Wait_Return(u32 Response,u32 UnResponse,u8 Timecnt);
 void GPS_CFG_RST(u8 Start_Mode,u8 Rst_Mode);/*GPSģ��Ĵ�������*/
 void GPS_CFG_PRT(u32 USART_Baud,u16 In_Proto,u16 Out_Proto);/*GPSģ��Ĵ�������*/
 void GPS_CFG_MSG(u8 Msg_Class,u8 Msg_Id,u8 Msg_Rate);/*GPSģ��Ĵ�������*/
//static void GPS_CFG_ANT_SET(void);
#endif
#ifdef NO_UBLOX
static void GPS_HotReset_En(void);
static void GPS_OutPut_En(u8 Type,u8 ONOFF);
#endif


static void GPS_Send_Data( u8 *Str,u8 Length);/*�����ַ�*/
static u8 UBX_Check( u8 *Str,u8 *pCheck);/*UBXЭ��У��ֵд��*/
static void UpdatePos(STRUCT_RMC *GpsInfor,u8 UpdateMode);/*����GPS����*/
void GPS_DMA_RcvIsr(void);	/*GPS���ݽ����ж�*/
u16 GPS_GET_DATA_LEN(void);	/*GPS���Ƚ���*/
u8 GPS_GET_char(u8 *comdata);/*�ӽ��ջ����л�ȡһ�ֽڳ���*/
static void UTC_To_RTC(STRUCT_RMC *POS_Inf);
void GPS_CFG_NAV_SET(void);
static u8 HowManyDays(u8 year ,u8 month);
static s32 Get_Space_Time(u8 *Now_Time,u8 *Pass_Time);
float GPS_Local_Change(u8 *Lon2Lat,u8 Mode, float *res);
static int diff_day (unsigned char* date1,unsigned char *date2 ); /*��������*/
static int count_day ( int year, int month, int day, int flag );  /*��������*/
#define leap(year) ((year%4==0&&year%100!=0)||(year%400==0))  /*�����ж�*/


/*********************************************************
*Function:       GPS_DataAnaly()
*Description:    GPS���ݽ�������
*Calls:          
*Called By:      
*Input:          ��
*Output:         ��
*Return:         0:�����ɹ�  ����:����ʧ�ܣ�����λ���ж�ʧ��ԭ��
*Others:         
*********************************************************/
u32 GPS_DataAnaly(void)
{
	u16 CurrCnt=0;
	u32 result=0;
	u16 i = 0;
	//CurrCnt=DMA_Q_GetDataLen(&GPS_Rcv_Buf);
	
	//if(CurrCnt)
	if (gps_data.has_new_data)
	{
//		GPSHandle.ModelStatus=0;
		CurrCnt = gps_data.data_len;//add by zq
		while(CurrCnt--)
		{
			//if(DMA_Q_GetByte(&GPS_Rcv_Buf,&GPSHandle.Data))/*�����ݳɹ�*/
			if (i < gps_data.data_len)
			{
				GPSHandle.Data = gps_data.buffer[i++];
				switch(GPSHandle.Step)
				{
					case 0:
					{
						if(GPSHandle.Data=='$')		/*NMEA��־ͷ*/
						{
							GPSHandle.CHK_A=0;
							GPSHandle.Msg_Data_Len=0;
							GPSHandle.Step++;
							GPSHandle.Speed_Temp=0;
							GPSHandle.CntTemp=0;
				 			GPSHandle.dot=0;
						}
						#ifdef UBLOX
						else if(GPSHandle.Data==0xB5)	/*UBX��־ͷ*/
						{
							GPSHandle.CHK_A=0;
							GPSHandle.CHK_B=0;
							GPSHandle.Msg_Data_Len=0;
							GPSHandle.CntTemp	=0;
							GPSHandle.Step=100;
						}
						#endif
					}
					break;
					case 1:
					{
						if(GPSHandle.Data=='G')
						{
							
							GPSHandle.CHK_A='G';
							GPSHandle.Step++;	
						}
						else
						{
							GPSHandle.Step=0;
						}
					}
					break;
					case 2:
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data=='P')
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;
						}
					}
					break;
				    case 3:
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data=='R')
						{
							GPSHandle.Step++;
						}
						/*ȡ���Լ��GPTXT�ֶν���*/
						/*else if(GPSHandle.Data=='T') 	  
						{
							GPSHandle.Step=50;		
						}*/
						else if(GPSHandle.Data=='G') 
						{
							GPSHandle.Step=70;		
						}
						else
						{
							GPSHandle.Step=0;
						}
					}
					break;
					case 4:
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data=='M')
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;
						}
					}
					break;
					case 5:
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data=='C') 	/*GPRMC�ֶ�*/
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;
						}
					}
					break;
					case 6:
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data==',')
						{
							GPSHandle.Step++;
							GPSHandle.CntTemp=0;
						}
						else
						{
							GPSHandle.Step=0;
						}
					}
					break;
					case 7:	//ʱ��
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.CntTemp<6)
			            {
							if(GPSHandle.Data!=',')
							{
								if(GPSHandle.CntTemp%2==0x00)
								{
									GPSHandle.GPS_INF.Time[3+GPSHandle.CntTemp/2]=0x00;
									GPSHandle.GPS_INF.Time[3+GPSHandle.CntTemp/2]=(GPSHandle.Data-'0')<<4;
								}
								else
								{
									GPSHandle.GPS_INF.Time[3+GPSHandle.CntTemp/2]|=GPSHandle.Data-'0';
								}
								GPSHandle.CntTemp++;
							}
							else										/*δ��λʱ�䲻����*/
							{
								GPSHandle.Step++;		
							}
			            }
						else if(GPSHandle.CntTemp<10)
						{
							if(GPSHandle.Data==',')
							{
								GPSHandle.CntTemp=0;
								GPSHandle.Step++;	
							}
							else
							{
								GPSHandle.CntTemp++;		
							}
						}
						else
						{
							GPSHandle.Step=0;	
						}
					}
					break;
					case 8:	 
					{  
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data=='A')//GPS ������Ч
						{
							GPSHandle.Step++;	
							GPSHandle.CntTemp=0;	
							GPSHandle.GPS_INF.Fix_Status=GPS_FIXED;
						}
						else if(GPSHandle.Data=='V')			/*GPS������Ч*/			 
						{
							GPSHandle.CntTemp=0;
							GPSHandle.GPS_INF.Fix_Status=GPS_UNFIXED;
							GPSHandle.Step++;		
						}
						else						/*GPS���ݳ���*/
						{
							GPSHandle.Step=0;	
						}
					}
					break;
					case 9:
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data==',')
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;
						}
					}					 
					break;
					case 10: //ȡγ��ֵ
					{
						GPSHandle.CHK_A^=GPSHandle.Data;		 
						if(GPSHandle.GPS_INF.Fix_Status==GPS_FIXED)	//GPS��λʱ����γ��
						{							
							switch(GPSHandle.CntTemp)
				            {
					            case 0:
					            case 1:
					            case 2:
					            case 3:
				                	GPSHandle.GPS_INF.Latitude[GPSHandle.CntTemp]=GPSHandle.Data;
				                break;
					            case 5:
					            case 6:
					            case 7:
					            case 8:
					            	GPSHandle.GPS_INF.Latitude[GPSHandle.CntTemp-1]=GPSHandle.Data;
					            break;
								case 9:
								break;
								case 10:
								case 11:
								case 12:
									if(GPSHandle.Data==',')
									{
										GPSHandle.Step++;	
										GPSHandle.CntTemp=0;	
									}
									break;
								case 13:
									if(GPSHandle.Data==',')
									{
										GPSHandle.Step++;	
										GPSHandle.CntTemp=0;	
									}
									else
									{
										GPSHandle.Step=0;	
									}
					            default:
								break;
				            }
							GPSHandle.CntTemp++;
						}
						else
						{
							if(GPSHandle.Data==',')
							{
								GPSHandle.Step++;	
								GPSHandle.CntTemp=0;	
							}
							else
							{
								//GPSHandle.Step=0;	
							}	
						}
					}
					break;
					case 11://�ϱ�ά��ʾ
					{	
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data=='N'||GPSHandle.Data=='S')
						{
							GPSHandle.GPS_INF.North_Indicator=GPSHandle.Data;
							GPSHandle.Step++;	
						}
						else  if (GPSHandle.Data==',')
						{
							GPSHandle.Step+=2;	
						}
						else
						{
							GPSHandle.Step=0;	
						}
					}
					break;
					case 12:
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
					    if(GPSHandle.Data==',')
						{
							GPSHandle.CntTemp=0;
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;
						}
					}
					break;
					case 13: //��ȡ����ֵ
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.GPS_INF.Fix_Status==GPS_FIXED)	//gps��λʱ���þ���ֵ
						{							
							switch(GPSHandle.CntTemp)
				            {
					            case 0:
					            case 1:
					            case 2:
					            case 3:
					            case 4:
					            	GPSHandle.GPS_INF.Longitude[GPSHandle.CntTemp]=GPSHandle.Data;
					            break;
					            case 6:
					            case 7:
					            case 8:
					            case 9:  
					            	GPSHandle.GPS_INF.Longitude[GPSHandle.CntTemp-1]=GPSHandle.Data;
					            break;
								case 10:
								break;
								case 11:
								case 12:
								case 13:
									if(GPSHandle.Data==',')
									{
										GPSHandle.Step++;	
										GPSHandle.CntTemp=0;	
									}
									break;
								case 14:
									if(GPSHandle.Data==',')
									{
										GPSHandle.Step++;	
										GPSHandle.CntTemp=0;	
									}
									else
									{
										GPSHandle.Step=0;	
									}
								break;
					            default:break;							
				            }
							GPSHandle.CntTemp++;
						}
						else
						{
							if(GPSHandle.Data==',')
							{
								GPSHandle.Step++;	
								GPSHandle.CntTemp=0;	
							}
							else
							{
							//	GPSHandle.Step=0;	
							}	
						}
					}
					break;
					case 14://������ʾ
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data=='E'||GPSHandle.Data=='W')
						{
							GPSHandle.GPS_INF.East_Indicator=GPSHandle.Data;
							GPSHandle.Step++;	
							GPSHandle.Speed_Temp=0;
							GPSHandle.dot=0;
							GPSHandle.CntTemp=0;
						}
						else  if (GPSHandle.Data==',')
						{
							GPSHandle.Step+=2;	
						}
						else
						{
							GPSHandle.Step=0;	
						}
					}
					break;
					case 15:
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
					    if(GPSHandle.Data==',')
						{
							GPSHandle.Step++;	
						}
						else
						{
							GPSHandle.Step=0;
						}
					}
					break;
					case 16://�ٶ�
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data==',')
						{
							GPSHandle.CntTemp=0;
							GPSHandle.GPS_INF.COG=0;
							GPSHandle.dot=0;
							GPSHandle.Step++;	
						}
						else if(GPSHandle.CntTemp<5)
						{
							if(GPSHandle.Data=='.')
							{
								GPSHandle.dot=4-GPSHandle.CntTemp;//��סС����λ��
							}
							else
							{
								GPSHandle.Speed_Temp=GPSHandle.Speed_Temp*10+GPSHandle.Data-'0';	
							}
							if(GPSHandle.CntTemp==4)
							{
								int i;
								//GPSHandle.GPS_INF.Speed=(u16)((GPSHandle.Speed_Temp*1852)/(1000*( pow(10,GPSHandle.dot)+0.0)));//(GPSHandle.Speed_Temp*1852)/1000/( pow(10,GPSHandle.dot));//(float)
								GPSHandle.GPS_INF.Speed=(u16)((GPSHandle.Speed_Temp*1852)/(1000*( powme(10,GPSHandle.dot)+0.0)));//(GPSHandle.Speed_Temp*1852)/1000/( pow(10,GPSHandle.dot));//(float)
								//DEBUG("Speed_Temp:%u dot:%u pow:%d", GPSHandle.Speed_Temp, GPSHandle.dot, powme(10,GPSHandle.dot));
								
							}
							GPSHandle.CntTemp++;	
						}
						else if(GPSHandle.CntTemp>10)
						{
							GPSHandle.Step=0;	
						}
						else
						{
							GPSHandle.CntTemp++;
						}
					}
					break;
					case 17:	 //��λ COG
					{
						GPSHandle.CHK_A^=GPSHandle.Data;						
						if(GPSHandle.Data==',')
						{
							GPSHandle.CntTemp=0;
							GPSHandle.Speed_Temp=0;
							GPSHandle.dot=0;
							GPSHandle.Step++;	
						}
						else if(GPSHandle.CntTemp<1)	//peijl_0928����
						{
							if(GPSHandle.Data=='.')
							{
								GPSHandle.CntTemp=1;
							}
							else
							{
								GPSHandle.GPS_INF.COG=GPSHandle.GPS_INF.COG*10+GPSHandle.Data-'0';
							}
						}
					}
					break;
					case 18://����
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data==',')
						{
							GPSHandle.Step++;	
							GPSHandle.CntTemp=0;
						}
						else if(GPSHandle.CntTemp<6)
			            {
							if(GPSHandle.CntTemp%2==0x00)
							{
								GPSHandle.GPS_INF.Time[2-GPSHandle.CntTemp/2]=0x00;
								GPSHandle.GPS_INF.Time[2-GPSHandle.CntTemp/2]=(GPSHandle.Data-'0')<<4;
							}
							else
							{
								GPSHandle.GPS_INF.Time[2-GPSHandle.CntTemp/2]|=GPSHandle.Data-'0';
							}
							if(GPSHandle.CntTemp==5)
							{
								UTC_To_RTC( &(GPSHandle.GPS_INF));
							}
							GPSHandle.CntTemp++;
			            }
						else
						{
							GPSHandle.Step=0;	
						}
					}
					break;
					case 19://��ƫ��
					{
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data==',')
						{
							GPSHandle.Step++;	
							GPSHandle.CntTemp=0;	
						}
					}
					break;
					case 20	:	//��ƫ�Ƿ���
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data==',')
						{
							GPSHandle.Step++;	
							GPSHandle.CntTemp=0;	
						}
					break;
					case 21:  //��λģʽ
						GPSHandle.CHK_A^=GPSHandle.Data;
						GPSHandle.Step++;	
					break;
					case 22:
						if(GPSHandle.Data=='*')
						{
							GPSHandle.Step++;	
							GPSHandle.CntTemp=0;
							GPSHandle.Speed_Temp=0;
						}
						else
						{
							GPSHandle.Step=0;
						}
					break;
					case 23:
					{	
						if(GPSHandle.CntTemp<2)
						{	
							if(GPSHandle.Data>'9')
							{
								GPSHandle.Data=GPSHandle.Data-'7';
								GPSHandle.Speed_Temp=(GPSHandle.Speed_Temp<<4)+GPSHandle.Data;		
							}
							else
							{
								GPSHandle.Data=GPSHandle.Data-'0';
								GPSHandle.Speed_Temp=(GPSHandle.Speed_Temp<<4)+GPSHandle.Data;
							}
							if(GPSHandle.CntTemp==1)
							{
								if(GPSHandle.Speed_Temp==GPSHandle.CHK_A)	 //AֵУ��ɹ�
								{
									if(GPSHandle.GPS_INF.Fix_Status==GPS_UNFIXED)
									{
										result|=NMEA_RMC_UNFIXED;
									}
									else
									{
										result|=NMEA_RMC_OK;	  
									}
									GPSHandle.CntTemp=0;
									GPSHandle.Step++;	
								}
								else
								{
									result|= NMEA_CHECK_ERROR  ;	//У��ʧ��
									GPSHandle.Step=0;	
								}
							}
							GPSHandle.CntTemp++;	
						}
					}
					break;
					case 24:	 /*������*/
						if(GPSHandle.Data==0x0D)
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;
					case 25:
						if(GPSHandle.Data==0x0A)
						{
//							GPSHandle.GPSAnalyFlag=GPSAnalyOK;
							if(result & NMEA_RMC_OK  ) //�ж�GPRMC������ȷ�����¶�λ�ṹ��
							{
							/*	if(ReadAlarmPara(Alarm1,STA_ACC_ON))	//ACC ON
								{ */
									if(GPSHandle.GPS_INF.Speed<=Active_Speed)	//����ٶ�С����Сֵ���ٶ���0
									{
										GPSHandle.GPS_INF.Speed=0;
										GPSHandle.GPS_INF.COG=0;	
									}
									UpdatePos(&(GPSHandle.GPS_INF),ALLUPDATE);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
							/*	}
								else	//ACC off
								{
									if(SET==ReadAlarmPara(Alarm1,STA_GPS_FIXED))	//���ϴ��Ƕ�λ״̬,���¶�λ�ṹ�岿�ֲ���
									{										
										if(GPSHandle.GPS_INF.Speed>Active_Speed)
										{
											UpdatePos(&(GPSHandle.GPS_INF),ALLUPDATE);
										}
										else
										{
											GPSHandle.GPS_INF.Speed=0;
											GPSHandle.GPS_INF.COG=0;
											UpdatePos(&(GPSHandle.GPS_INF),ACCOFFUPDATE);
										}										
									}
									else //���ϴ���δ��λ״̬,���¶�λ�ṹ��
									{
										UpdatePos(&(GPSHandle.GPS_INF),ALLUPDATE);
									}
								}	   */
								GPSHandle.Step=0;
							}  
							else if(result&NMEA_RMC_UNFIXED)  //����״̬Ϊδ��λ,ACC��ʱ���¶�λ״̬
							{
							//	if(ReadAlarmPara(Alarm1,STA_ACC_ON))
							//	{
									UpdatePos(NULL,UNFIXEDUPDATE);
							//	} 
								GPSHandle.Step=0;
							}
							else
							{
								GPSHandle.Step=0;	
							}
						}
						else
						{
							GPSHandle.Step=0;
						}
					break ;
/*					case 50://GPTXT
						if(GPSHandle.Data=='X')
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;	   
					case 51:
						if(GPSHandle.Data=='T')	 //GPTXT
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;
					case 52:
						if(GPSHandle.Data==',')
						{
							GPSHandle.dot++;
							if(GPSHandle.dot==4)
							{
								GPSHandle.Step++;
							}
						}
					break;
					case 53:
						if(GPSHandle.Data=='A')
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break; 
					case 54:
						if(GPSHandle.Data=='N')
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;
					case 55:
						if(GPSHandle.Data=='T')
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;
					case 56:
						if(GPSHandle.Data=='S')
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;
					case 57:
						if(GPSHandle.Data=='T')
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;
					case 58:
						if(GPSHandle.Data=='A')
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;
					case 59:
						if(GPSHandle.Data=='T')
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;
					case 60:
						if(GPSHandle.Data=='U')
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;
					case 61:
						if(GPSHandle.Data=='S')	 	//����״̬�־�
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;
					case 62:
						if(GPSHandle.Data=='=')
						{
							GPSHandle.CntTemp=0;
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;
					case 63:
						if (GPSHandle.CntTemp<9)
						{
							if(GPSHandle.Data!='*')
							{
								GPSHandle.GPTXT[GPSHandle.CntTemp]	 =GPSHandle.Data;
								GPSHandle.CntTemp++;
							}
							else
							{
								if(GPSHandle.CntTemp==2)//OK
								{
									if(strncmp(GPSHandle.GPTXT,"OK",GPSHandle.CntTemp)==0)		//��������
									{
										GPSHandle.GPS_INF.Ant_Status=GPS_ANT_OK;
										UpdatePos(&(GPSHandle.GPS_INF),ANTUPDATE);
									}
									else
									{
										GPSHandle.Step=0;
									}
								}
								else if(GPSHandle.CntTemp==4)//OPEN
								{
									if(strncmp(GPSHandle.GPTXT,"OPEN",GPSHandle.CntTemp)==0)		//���߿�·
									{
										GPSHandle.GPS_INF.Ant_Status=GPS_ANT_OPEN;
										UpdatePos(&(GPSHandle.GPS_INF),ANTUPDATE);
														
									}
									else 
									{
										GPSHandle.Step=0;		
									}		
								}
								else if(GPSHandle.CntTemp==5)//SHORT
								{
									if(strncmp(GPSHandle.GPTXT,"SHORT",GPSHandle.CntTemp)==0)	//���߶�·
									{
										GPSHandle.GPS_INF.Ant_Status=GPS_ANT_SHORT;
										UpdatePos(&(GPSHandle.GPS_INF),ANTUPDATE);		
									}
									else
									{
										GPSHandle.Step=0;	
									}
								}
								else
								{
									GPSHandle.Step=0;	
								}
							}
						}
						else
						{
							GPSHandle.Step=0;		
						}
					break;	*/
					case 70://GPGGA
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data=='G')
						{
							GPSHandle.Step++;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;	   
					case 71:
						GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data=='A')
						{
							GPSHandle.Step++;
							GPSHandle.GPS_INF.Height=0;
							GPSHandle.CntTemp=0;//'.'����
						}
						else
						{
							GPSHandle.Step=0;	
						}
					break;	   
					case 72:
						//GPSHandle.CHK_A^=GPSHandle.Data;
						if(GPSHandle.Data==',')
						{
							GPSHandle.CntTemp++;
							if(GPSHandle.CntTemp==9)
							{
								GPSHandle.GGA_P_N_Flag=0;//������־��λ
							}	
							else if(GPSHandle.CntTemp==10)//�ж�������־λ,�趨��ǰֵ
							{
								if (GPSHandle.GGA_P_N_Flag==1)//������־��λ
								{
									GPSHandle.GPS_INF.Height=0-GPSHandle.GPS_INF.Height;
								}
							}						
						}
						else if(GPSHandle.Data=='*')
						{
							GPSHandle.Step++;	
						}
						else
						{
							if(GPSHandle.CntTemp==9)//�߶�
							{
								if(GPSHandle.Data=='.')
								{
									GPSHandle.CntTemp++;
								}
								else if(GPSHandle.Data=='-')
								{
									GPSHandle.GGA_P_N_Flag=1;
								}
								else
								{
									GPSHandle.GPS_INF.Height=GPSHandle.GPS_INF.Height*10+GPSHandle.Data-'0';	
								} 	
							}
							else if(GPSHandle.CntTemp==6)//�߶�
							{
								if((GPSHandle.Data=='2')||(GPSHandle.Data=='1'))
								{
									GPSHandle.GGAFIXFlag=1;	  //�߶�ֵ��λ
								}
								else
								{
									GPSHandle.GGAFIXFlag=0;	  //δ��λ
								} 	 	
							}
							else if(GPSHandle.CntTemp>11)
							{
								GPSHandle.Step++;				
							}	
						}
					break;
					case 73:
						if(GPSHandle.Data==0x0D)
						{
							GPSHandle.Step++;	
						}
					break;
					case 74:
						if(GPSHandle.Data==0x0A)
						{
							//GPSHandle.GPSAnalyFlag=GPSAnalyOK;
							if (GPSHandle.GGAFIXFlag)
							{
								result|=NMEA_GGA_OK;
								UpdatePos(&(GPSHandle.GPS_INF),HEIGHTUPDATE);
							}
							else
							{
								result|=NMEA_GGA_UNFIXED;
							}
							GPSHandle.Step=0;
							
						}
						else
						{
							GPSHandle.Step=0;
						}
					break;	
					#ifdef UBLOX				
					case 100:				/*UBX����*/
					{
						if(GPSHandle.Data==0x62)
						{
							GPSHandle.Step++;	
						}	
						else
						{
							GPSHandle.Step=0;
						}
					}	
					break;
					case 101:
					{
						switch (GPSHandle.Data)
						{
//							case 0x0A:	  /*MON�ֶ�*/	/*����ʹ��MON�ֶ���Ϣ�ж�����״̬*/
//								GPSHandle.CHK_A+=GPSHandle.Data;
//								GPSHandle.CHK_B+=GPSHandle.CHK_A;
//								GPSHandle.Step=200;
//								
//							break;
							case 0x05:	  /*ACK�ֶ�*/	
								GPSHandle.CHK_A+=GPSHandle.Data;
								GPSHandle.CHK_B+=GPSHandle.CHK_A;
								GPSHandle.Step++;	
							break;
							default:
								GPSHandle.Step=0;
							break;
						}
					}
					break;
					case 102:
					{
						GPSHandle.Msg_Data_Len=0;
						switch (GPSHandle.Data)
						{
							case 0x00:		 	/*Ӧ��ʧ��*/
								GPSHandle.CHK_A+=GPSHandle.Data;
								GPSHandle.CHK_B+=GPSHandle.CHK_A;
								GPSHandle.Step=150;
							break;
							case 0x01:			/*Ӧ��ɹ�*/
								GPSHandle.CHK_A+=GPSHandle.Data;
								GPSHandle.CHK_B+=GPSHandle.CHK_A;
								GPSHandle.Step=103;
							break;								  
							default:
								GPSHandle.Step=0;
							break;
						}
					}
					break;
					case 103:
					{
						GPSHandle.CHK_A+=GPSHandle.Data;
						GPSHandle.CHK_B+=GPSHandle.CHK_A;	
						GPSHandle.Step++;
					}
					break;
					case 104:
					{
						GPSHandle.CHK_A+=GPSHandle.Data;
						GPSHandle.CHK_B+=GPSHandle.CHK_A;		
						GPSHandle.Step++;
					}
					break;
					case 105:
					{
						GPSHandle.CHK_A+=GPSHandle.Data;
						GPSHandle.CHK_B+=GPSHandle.CHK_A;	
						switch(GPSHandle.Data)
						{
							case 0x06:			/*CFG ACK*/
								GPSHandle.Step++;
							break;
							default:
								GPSHandle.Step=0;
							break;	
						}	
					}
					break;
					case 106:
					{
						
						GPSHandle.CHK_A+=GPSHandle.Data;
						GPSHandle.CHK_B+=GPSHandle.CHK_A;
						switch(GPSHandle.Data)
						{
							case 0x01:			/*CFG MSG ACK*/
							result|=UBX_CFG_MSG_OK  ;
							GPSHandle.Step++;
							break;
							case 0x04:			/*CFG RST ACK*/
							result|= UBX_CFG_RST_OK  ;
							GPSHandle.Step++;
							break;
//							case 0x13:			/*CFG ANT ACK*/
//							result|= UBX_CFG_ANT_OK ;
//							GPSHandle.Step++;
//							break;
							case 0x24:			/*NAV����*/
							result|= UBX_NAV_MSG_OK  ;
							GPSHandle.Step++;
							break;
							default:
								GPSHandle.Step=0;
							break;	
						}
					}
					break;
					case 107:
					{
						if(GPSHandle.CHK_A==GPSHandle.Data)	 			/*ƥ��У��ֵ*/
						{
							GPSHandle.Step++;	
						}
						else
						{
							result&=~UBX_CFG_RST_OK;
							result&=~UBX_CFG_RST_ERROR;
							result&=~UBX_CFG_MSG_OK;
							result&=~UBX_CFG_MSG_ERROR;
							result&=~UBX_NAV_MSG_OK;
							result&=~UBX_NAV_MSG_ERROR;
							result|=UBX_CHECK_ERROR  ;	
							GPSHandle.Step++;
						}
					}
					break;
					case 108:
					{
						if(result&UBX_CHECK_ERROR)	
						{
							GPSHandle.Step=0;
						}
						else  if(GPSHandle.CHK_B==GPSHandle.Data)
						{
							GPSHandle.Step=0;
//							GPSHandle.GPSAnalyFlag=GPSAnalyOK;		
						}
						else
						{
							result&=~UBX_CFG_RST_OK;
							result&=~UBX_CFG_RST_ERROR;
							result&=~UBX_CFG_MSG_OK;
							result&=~UBX_CFG_MSG_ERROR;
							result&=~UBX_NAV_MSG_OK;
							result&=~UBX_NAV_MSG_ERROR;
							result|=UBX_CHECK_ERROR;
							GPSHandle.Step=0;		 
						}
					}
					break;
					case 150:
					{
						GPSHandle.CHK_A+=GPSHandle.Data;
						GPSHandle.CHK_B+=GPSHandle.CHK_A;	
						GPSHandle.Step++;
					}
					break;
					case 151:
					{
						GPSHandle.CHK_A+=GPSHandle.Data;
						GPSHandle.CHK_B+=GPSHandle.CHK_A;		
						GPSHandle.Step++;
					}
					break;
					case 152:
					{
						GPSHandle.CHK_A+=GPSHandle.Data;
						GPSHandle.CHK_B+=GPSHandle.CHK_A;	
						switch(GPSHandle.Data)
						{
							case 0x06:			/*CFG ACK*/
							GPSHandle.Step++;
							break;
							default:
							GPSHandle.Step=0;
							break;	
						}	
					}
					break;
					case 153:
					{
						GPSHandle.CHK_A+=GPSHandle.Data;
						GPSHandle.CHK_B+=GPSHandle.CHK_A;	
						switch(GPSHandle.Data)
						{
							case 0x01:			/*CFG MSG ACK*/
								result|= UBX_CFG_MSG_ERROR  ;
								GPSHandle.Step++;
							break;
							case 0x04:			/*CFG RST ACK*/
								result|= UBX_CFG_RST_ERROR  ;
								GPSHandle.Step++;
							break;
							case 0x24:			/*CFG RST ACK*/
								result|= UBX_NAV_MSG_ERROR  ;
								GPSHandle.Step++;
							break ;

//							case 0x13:			/*CFG ANT ACK*/		   
//								result|= UBX_CFG_ANT_ERROR  ;
//								GPSHandle.Step++;
//							break;
							default:
								GPSHandle.Step=0;
							break;	
						}
					}
					break;
					case 154:
					{
						if(GPSHandle.CHK_A==GPSHandle.Data)	 			/*ƥ��У��ֵ*/
						{
							GPSHandle.Step++;	
						}
						else
						{
							result&=~UBX_CFG_RST_OK;
							result&=~UBX_CFG_RST_ERROR;
							result&=~UBX_CFG_MSG_OK;
							result&=~UBX_CFG_MSG_ERROR;
							result&=~UBX_NAV_MSG_OK;
							result&=~UBX_NAV_MSG_ERROR;
							result|=UBX_CHECK_ERROR;	
							GPSHandle.Step++;
						}
					}
					break;
					case 155:
					{
						if(result&UBX_CHECK_ERROR)	
						{
							GPSHandle.Step=0;
						}
						else  if(GPSHandle.CHK_B==GPSHandle.Data)
						{
							GPSHandle.Step=0;
							//GPSHandle.GPSAnalyFlag=GPSAnalyOK;		
						}
						else
						{
							result&=~UBX_CFG_RST_OK;
							result&=~UBX_CFG_RST_ERROR;
							result&=~UBX_CFG_MSG_OK;
							result&=~UBX_CFG_MSG_ERROR;
							result&=~UBX_NAV_MSG_OK;
							result&=~UBX_NAV_MSG_ERROR;
							result|=UBX_CHECK_ERROR;
							GPSHandle.Step=0;		 
						}
					}
					break;
					/*case 200:
					{
						if(GPSHandle.Data==0x09)					//�鿴����״̬��
						{
							GPSHandle.CHK_A+=GPSHandle.Data;
							GPSHandle.CHK_B+=GPSHandle.CHK_A;
							GPSHandle.Step++;
							GPSHandle.Msg_Data_Len=0;
						}
						else
						{
							GPSHandle.Step=0;	
						}
					}
					break;
					case 201:
					{
						GPSHandle.CHK_A+=GPSHandle.Data;
						GPSHandle.CHK_B+=GPSHandle.CHK_A;
						GPSHandle.Msg_Data_Len=GPSHandle.Data;
						GPSHandle.Step++;	
					}
					break;
					case 202:
					{
						GPSHandle.CHK_A+=GPSHandle.Data;
						GPSHandle.CHK_B+=GPSHandle.CHK_A;
						GPSHandle.Msg_Data_Len=GPSHandle.Msg_Data_Len+(GPSHandle.Data<<8);
						GPSHandle.CntTemp=0;
						GPSHandle.Step++;	
					}
					break;
					case 203:
					{
						GPSHandle.CHK_A+=GPSHandle.Data;
						GPSHandle.CHK_B+=GPSHandle.CHK_A;
						GPSHandle.CntTemp++;
						if(GPSHandle.CntTemp>=GPSHandle.Msg_Data_Len)	 //�յ����һ���ֽ�
						{
							GPSHandle.Step++;	
						}
						else if(GPSHandle.CntTemp==21)		//����״̬�ֽ�
						{
							GPSHandle.GPSDataType= GPSHandle.Data;		
						}
					}
					break;
					case 204:
					{
						if(GPSHandle.CHK_A==GPSHandle.Data)	 			//ƥ��У��ֵ
						{
							GPSHandle.Step++;	
						}
						else
						{
							GPSHandle.GPSDataType= UBX_CHECK_ERROR  ;	
							GPSHandle.Step++;
						}
					}
					break;
					case 205:
					{
						if(GPSHandle.GPSDataType ==UBX_CHECK_ERROR)	
						{
							GPSHandle.Step=0;
							return 	GPSHandle.GPSDataType;
						}
						else  if(GPSHandle.CHK_B==GPSHandle.Data)
						{
							GPSHandle.Step=0;
							switch(GPSHandle.GPSDataType)
							{
								case 0x00:			//��ʼ��
									GPSHandle.GPS_INF.Ant_Status=GPS_ANT_INIT;
									UpdatePos(&(GPSHandle.GPS_INF),ACCOFFUPDATE);
								break;
								case 0x01:
									GPSHandle.GPS_INF.Ant_Status=GPS_ANT_UNKNOW;
								break;
								case 0x02:
									GPSHandle.GPS_INF.Ant_Status=GPS_ANT_OK;
								break;
								case 0x03:
									GPSHandle.GPS_INF.Ant_Status=GPS_ANT_SHORT;
								break;
								case 0x04:
									GPSHandle.GPS_INF.Ant_Status=GPS_ANT_OPEN;
								break;
								default:
								break;	
							}
							UpdatePos(&(GPSHandle.GPS_INF),ANTUPDATE);
							GPSHandle.GPSDataType=UBX_MON_ANT_OK;
							GPSHandle.GPSAnalyFlag=GPSAnalyOK;	
						}
						else
						{
							GPSHandle.Step=0;		 
						}
					}
					break; */
					#endif
					default:
						GPSHandle.Step=0;
					break;	
				}		
			}
			else
			{
				
			}
/*			if(GPSHandle.GPSAnalyFlag==GPSAnalyOK)
			{
				return GPSHandle.GPSAnalyFlag;
			} */
		}
//		return GPSHandle.GPSAnalyFlag;			/*���ؽ���δ���*/
//		return result;
	//add by zq
	gps_data.has_new_data = OA_FALSE;
	}
	else
	{
			result |= GPS_NO_DATA;	
	}
	
	return result;
}

/*********************************************************
*Function:       GPS_Init()
*Description:    ��ʼ��ͨ�ſڣ����GPIO�ڣ���GPS�������߼����趨������GPS
*Calls:          
*Called By:      
*Input:          ��
*Output:         ��
*Return:         ��
*Others:         
*********************************************************/
//u8 GPS_Init(void)
//{
//	u8 status;
//
//	status=GPS_HW_Init();
//	if (status)
//	{
//		return status;
//	}
//	status=GPS_SW_Init();	
//	return status;
//}

/*********************************************************
*Function:       GPS_GetPosition()
*Description:    ��ȡλ����Ϣ
*Calls:          
*Called By:      
*Input:          ��
*Output:         Latitude:   γ��ֵд��ָ��
				 North_Indicator���ϱ�γָ��  'S'����'N'
				 Longitude ����д��ָ��
				 East_Indicator ������ֵд�� 'E' ���� 'W'	
*Return:         0����ȡ�ɹ� 1����ȡʧ�ܣ�һ������Ϊδ��λ
*Others:         γ�ȣ�8�ֽ��ַ���  ���ȣ�9�ֽ��ַ���
*********************************************************/
u8 GPS_GetPosition(STRUCT_RMC *gps_info)
{
	u8 err;
	if (CheckParam1(gps_info))
	{
		#if GPS_PRINT
		//DEBUG("((%s:%s:%d):): params err!", __FILE__, __func__, __LINE__);	
		#endif
		return 1;
	}
	
	////OSSemPend(GPS_DataSem,0,&err);
	oa_memcpy(gps_info,&Pos_Inf,sizeof(STRUCT_RMC));
	////OSSemPost(GPS_DataSem) ;
	return 0;
} 
/*********************************************************
*Function:       GPS_GetTime(u8 *TimeStr)
*Description:    ��ȡʱ��ֵ��BCD��ֵ
*Calls:          
*Called By:      
*Input:         ��
*Output:        TimeStr  д��ʱ���ָ��
*Return:        �ַ�����
*Others:        ����6�ֽ�BCD�룬����ʱ�� 
*********************************************************/
u8 GPS_GetTime(u8 *TimeStr)
{
	u8 err;

	if (CheckParam1(TimeStr))
	{
		#if GPS_PRINT
		//DEBUG("((%s:%s:%d):): params err!", __FILE__, __func__, __LINE__);	
		#endif
		return 1;
	}
	////OSSemPend(GPS_DataSem,0,&err);
	oa_memcpy(TimeStr,Pos_Inf.Time,6);
	////OSSemPost(GPS_DataSem);
	return 0;

}

/*********************************************************
*Function:       GetPosinfASC(u8 *Str,u8 StrSize,u8 *Strlen,u8 Filed)
*Description:    ��ȡ��ز���, ASCII��
*Calls:          
*Called By:      
*Input:         StrSize:������С
				Filed ��ȡ���ݵ�����
*Output:        Str:  д�����ݵ�ָ��
				Strlen: �������ݳ���

*Return:        0:��ȡ�ɹ� 1:��ȡʧ��
*Others:        
*********************************************************/
u8 GetPosinfASC(u8 *Str,u8 StrSize,u8 *Strlen,u8 Filed)
{
	u8 err;

	if (CheckParam2(Str,Strlen))
	{
		#if GPS_PRINT
		//DEBUG("((%s:%s:%d):): params err!", __FILE__, __func__, __LINE__);	
		#endif
		return 1;
	}
	////OSSemPend(GPS_DataSem,0,&err);
	switch(Filed)
	{
		case GPSFixedStatus:
			if(StrSize<1)
			{
				////OSSemPost(GPS_DataSem);
				return 1;
			}
			if(Pos_Inf.Fix_Status==GPS_FIXED)
			{
				*Str='A';	
			}
			else
			{
				*Str='V';	
			}
			*Strlen=1;	
		break;
		case GPSLat:
			if(StrSize<8)
			{
				////OSSemPost(GPS_DataSem);
				return 1;
			}
			oa_memcpy(Str,Pos_Inf.Latitude,8);
			*Strlen=8;
		break;
		case GPSNInd:
			if(StrSize<1)
			{
				////OSSemPost(GPS_DataSem);
				return 1;
			}
			*Str=Pos_Inf.North_Indicator;	
			*Strlen=1;
		break;
		case GPSLon:
			if(StrSize<9)
			{
				////OSSemPost(GPS_DataSem);
				return 1;
			}
			oa_memcpy(Str,Pos_Inf.Longitude,9);
			*Strlen=9;
		break;
		case GPSEInd:
			if(StrSize<1)
			{
				////OSSemPost(GPS_DataSem);
				return 1;
			}
			*Str=Pos_Inf.East_Indicator;	
			*Strlen=1;
		break;
		case GPSCog:
			if(StrSize<3)
			{
				////OSSemPost(GPS_DataSem);
				return 1;
			}
			*Str++=Pos_Inf.COG/100%10+'0';	
			*Str++=Pos_Inf.COG/10%10+'0';	
			*Str++=Pos_Inf.COG%10+'0';	
			*Strlen=3;	
		break;
		case GPSTime:
			if(Pos_Inf.Fix_Status==GPS_FIXED)
			{
				if(StrSize<12)
				{
					////OSSemPost(GPS_DataSem);
					return 1;
				}
				*Str++=((Pos_Inf.Time[0]>>4)&0x0F)+'0';
				*Str++=(Pos_Inf.Time[0]&0x0F)+'0';
				*Str++=((Pos_Inf.Time[1]>>4)&0x0F)+'0';
				*Str++=(Pos_Inf.Time[1]&0x0F)+'0';
				*Str++=((Pos_Inf.Time[2]>>4)&0x0F)+'0';
				*Str++=(Pos_Inf.Time[2]&0x0F)+'0';
				*Str++=((Pos_Inf.Time[3]>>4)&0x0F)+'0';
				*Str++=(Pos_Inf.Time[3]&0x0F)+'0';
				*Str++=((Pos_Inf.Time[4]>>4)&0x0F)+'0';
				*Str++=(Pos_Inf.Time[4]&0x0F)+'0';
				*Str++=((Pos_Inf.Time[5]>>4)&0x0F)+'0';
				*Str++=(Pos_Inf.Time[5]&0x0F)+'0';	
				*Strlen=12;
			}
			else
			{
				*Strlen=0;
			}
		break;
		case GPSAnt:
			if(StrSize<1)
			{
				////OSSemPost(GPS_DataSem);
				return 1;
			}
			if(Pos_Inf.Ant_Status==GPS_ANT_OK)
			{
				*Str='A';
			}
			else
			{
				*Str='V';
			}
			*Strlen=1;
		break;
		case GPSSpeed:
			{
				u8 TempStr[4];
				if(StrSize<3)
				{
					////OSSemPost(GPS_DataSem);
					return 1;
				}
				sprintf(TempStr,"03d",Pos_Inf.Speed);
				oa_memcpy(Str,TempStr,3);
				*Strlen=3;
			}	
		break;
		case GPSHeght:
			{
				u8 TempStr[6];

				if(StrSize<5)
				{
					////OSSemPost(GPS_DataSem);
					return 1;
				}
				sprintf(TempStr,"06d",Pos_Inf.Height);
				oa_memcpy(Str,TempStr,5);
				*Strlen=5;
			}	
		break;	
	}
	////OSSemPost(GPS_DataSem);
	return 0; 
}
/*********************************************************
*Function:       void GPS_IntvlDistanc(void)
*Description:    ��GPS�������߼����趨������GPS
*Calls:          
*Called By:      
*Input:         ��
*Output:        TimeStr  д��ʱ���ָ��
*Return:        0:��ȡʱ��ɹ�. 1:��ȡʧ�ܣ�һ������Ϊδ��λ
*Others:        32λ�޷�������ֵ����λkm 
*********************************************************/
#if GPS_DISTANCE_EN
/*********************************************************
*Function:       void GPS_IntvlDistanc()
*Description:    ��GPS�������߼����趨������GPS
*Calls:          
*Called By:      
*Input:         ��
*Output:        TimeStr  д��ʱ���ָ��
*Return:        0:��ȡʱ��ɹ�. 1:��ȡʧ�ܣ�һ������Ϊδ��λ
*Others:        32λ�޷�������ֵ����λkm 
*********************************************************/
FP32 GPS_IntvlDistanc(STRUCT_RMC *gps_info, float *res)
{
	static FP32  Old_Lon = 0;	/*��һ��γ��*/
	static FP32  Old_Lat = 0;	/*��һ�㾭��*/
	FP32 IntvlDistanc=0;
	static u16   Last_Speed;	/*ǰһ���ٶ� ����һ�ξ���*/
	static u8 Pass_Time[6] = {0};			/*�ϴε�ʱ��*/
	FP32  New_Lon;	/*��γ��*/
	FP32  New_Lat;	/*�¾���*/
//	FP32  Now_Speed;	/*��ǰ�ٶ� ����һ�ξ���*/
//	u32   RtcNowCnt;  /*RTC����*/
	s32		Fixed_Cnt;		/*��λʱ�����*/
//	u8 Time[6];
//	u8 err;
	u8 flag=0;//��ȷ��־
	oa_double temp = 0.0;
//	u8 status=0;
//	app_GetPosinf(&status,GPSFixedStatus,0);
	if(gps_info->Fix_Status == GPS_FIXED)	   //����ڶ�λ״̬
	{
	//	Trace(PrintDebug,"�������!\r\n");
		if((Old_Lon==0)&&(Old_Lat==0))//�״ζ�λ
		{
			/*Old_Lon=*/GPS_Local_Change(gps_info->Longitude,1, &Old_Lon) ;
			/*Old_Lat=*/GPS_Local_Change(gps_info->Latitude,0, &Old_Lat) ;
			Fixed_Cnt=0;
			memcpy(Pass_Time,gps_info->Time,6);
			*res = 0;
			return 0;	
		}
		else
		{
			Fixed_Cnt=Get_Space_Time(gps_info->Time,Pass_Time);  //��ȡʱ����s
			/*New_Lon=*/GPS_Local_Change(gps_info->Longitude,1, &New_Lon) ;
			/*New_Lat=*/GPS_Local_Change(gps_info->Latitude,0, &New_Lat) ;
			if(Fixed_Cnt>=0)
			{
				if(Fixed_Cnt>DISTANCE_FRQ)
				{
				//	Trace(PrintDebug,"ͨ����γ�ȼ������!\r\n");
					GPS2Point_Distance(New_Lon,New_Lat,Old_Lon,Old_Lat, &temp);
					IntvlDistanc = temp;
					//�÷�ʽȡ�õ���̾��ٴ���180km/h��ȡ180
					if(IntvlDistanc>(180*Fixed_Cnt)/3600.0)
					{
						IntvlDistanc=0;	
						flag=1;
					}	 
				}
				else 
				{
					if(gps_info->Speed>180)//�ٶ�����
					{
						IntvlDistanc=0;	
						flag=1;
					}
					else
					{
						IntvlDistanc=((gps_info->Speed+Last_Speed)/2.0)*Fixed_Cnt/3600.0;//����ƽ���ٶȼ���fixed_cnt�����					
					//	Trace(PrintDebug,"FIX_CNT:%d,Distance:%8.6f\r\n",Fixed_Cnt,IntvlDistanc);
					}
				}
			}
			else
			{
				IntvlDistanc=0;//ʱ������
				Old_Lon = 0;
				Old_Lat = 0;
				flag=1;
				DEBUG("GPSʱ������");
			}
			/*��������,���¾�����*/
			if(flag==0)
			{
				Last_Speed=gps_info->Speed;
				Old_Lon=New_Lon;
				Old_Lat=New_Lat;
				memcpy(Pass_Time,gps_info->Time,6);	//����ʱ��
			}
			*res = IntvlDistanc;
			return 	IntvlDistanc;
		}	
	}
	else
	{
//		Trace(PrintDevbug,"δ��λ!��ͳ�����\r\n");
		*res = 0;
		return 0;
	}			
}  
/*********************************************************
*Function:       GPS_GetDistance(u8 *TimeStr)
*Description:    ��GPS�������߼����趨������GPS
*Calls:          
*Called By:      
*Input:          
*Output:        TimeStr  д��ʱ���ָ��
*Return:        0:��ȡʱ��ɹ�. 1:��ȡʧ�ܣ�һ������Ϊδ��λ
*Others:        32λ�޷�������ֵ����λ0.1km 
*********************************************************/
u32 GPS_GetDistance(void)
{
	u32 distance=0; 
	if(GPSHandle.Speed_Distance>MinDistance) //������ʱ������̼��������UPDATE_DISTANC=0.1km
	{
		distance=floor(GPSHandle.Speed_Distance*10.0);
		GPSHandle.Speed_Distance=(GPSHandle.Speed_Distance*10-(float)distance)/10.0;
	}
	return distance;		
}
#endif
/*********************************************************
*Function:       GPS_GetAntaStatus()
*Description:    ��ȡ����״̬
*Calls:          
*Called By:      
*Input:         ��
*Output:        TimeStr  д��ʱ���ָ��
*Return:        0:������������. 1:���߶Ͽ�
*Others:         
*********************************************************/
u8 GPS_GetAntaStatus(void)
{
	//if(GPIO_ReadInputDataBit(GPS_AADET_PORT,GPS_AADET_PIN))
	if(0)
	{
		GPSHandle.GPS_INF.Ant_Status=GPS_ANT_OPEN;
		UpdatePos(&(GPSHandle.GPS_INF),ANTUPDATE);
		#if GPS_PRINT
		//DEBUG(PrintDevDebug,"GPS ANANT OPEN!\r\n");
		#endif
		return 1;	
	}
	else 
	{
		GPSHandle.GPS_INF.Ant_Status=GPS_ANT_OK;
		UpdatePos(&(GPSHandle.GPS_INF),ANTUPDATE);
		return 0;
	}	
}
/*********************************************************
*Function:       GPS_GetFixStatus()
*Description:    ��ȡGPS��λ״̬
*Calls:          
*Called By:      
*Input:         ��
*Output:        ��
*Return:        0:ģ���Ѷ�λ 1:ģ��δ��λ
*Others:         
*********************************************************/
u8 GPS_GetFixStatus(void)
{
	u8 result ;
	u8 err;

	//OSSemPend(GPS_DataSem,0,&err);
	if(Pos_Inf.Fix_Status==GPS_FIXED)
	{
		result=0;
	}
	else
	{
		result=1;
	}
	//OSSemPost(GPS_DataSem);
	return result;

}
/*********************************************************
*Function:       GPS_GetModuleStatus()
*Description:    ��ȡGPSģ��״̬
*Calls:          
*Called By:      
*Input:         ��
*Output:        ��
*Return:        0:ģ������ 1:ģ���쳣
*Others:         ����һ������ָ��,�������޻ظ�
*********************************************************/
//u8 GPS_GetModuleStatus(void)
//{
//    GPS_CFG_MSG(NMEA_GP,GPRMC,0x01);
//	if (GPS_Wait_Return(UBX_CFG_MSG_OK,UBX_CFG_MSG_ERROR,10))
//	{
//		return 1;
//	}	
//	return 0;
//}
/*********************************************************
*Function:       UpdatePos(STRUCT_RMC *GpsInfor , u8 UpdateMode)
*Description:    ���� GPS���ݽṹ��	 Pos_Inf ��ALRAM��GPS�ֶ�
*Calls:          
*Called By:      GPS_Start_Init
*Input:          GpsInfor  GPS����Դ
				 UpdateMode ���·�ʽ
				 ȡֵ��
				 	ALLUPDATE ȫ����
					ACCOFFUPDATE ACC���������
					ALLCLRUPDATE ȫ��ո���
					UNFIXEDUPDATE δ��λ����
					ANTUPDATE ����״̬����

*Output:         ��
*Return:         ��
*Others:         
*********************************************************/
static void UpdatePos(STRUCT_RMC *GpsInfor , u8 UpdateMode)
{
	u8 err;

	//OSSemPend(GPS_DataSem,0,&err);
	switch (UpdateMode)
	{
		case ALLUPDATE:													/*ȫ����*/
			if(GpsInfor !=NULL)
			{
				oa_memcpy(&Pos_Inf,GpsInfor,sizeof(STRUCT_RMC));
			//	WriteAlarmPara(SetAcktion,Alarm1,STA_GPS_FIXED);	/*GPS��λ*/
			//	WriteAlarmPara(ResetAcktion,Alarm0,ALARM_GNSS_ERR);	/*GPSģ�����*/
				
			/*	if(Pos_Inf.North_Indicator=='N')						//�ж��Ƿ��Ǳ�γ
				{
					WriteAlarmPara(ResetAcktion,Alarm1,STA_LAT_SOUTH);
				}
				else
				{
					WriteAlarmPara(SetAcktion,Alarm1,STA_LAT_SOUTH);
				}
				if(Pos_Inf.East_Indicator=='E')							//�ж��Ƿ��Ƕ���
				{
					WriteAlarmPara(ResetAcktion,Alarm0,STA_LON_WEAST);
				}
				else
				{
					WriteAlarmPara(SetAcktion,Alarm0,STA_LON_WEAST);
				} */
			} 
		break;
		case ACCOFFUPDATE:												/*ACC�ظ��£�ֻ����ʱ��*/
			Pos_Inf.Speed=0;
			Pos_Inf.COG=0;
			oa_memcpy(Pos_Inf.Time,GpsInfor->Time,6);					   
		break;
		case ALLCLRUPDATE:												 /*ȫ�����״̬��λ��ʼ״̬*/
			Pos_Inf.Fix_Status=GPS_UNFIXED;
			Pos_Inf.Ant_Status=GPS_ANT_OK;
			Pos_Inf.Speed=0;
			Pos_Inf.COG=0;
		/*	WriteAlarmPara(ResetAcktion,Alarm1,STA_GPS_FIXED);
			WriteAlarmPara(ResetAcktion,Alarm0,ALARM_GNSS_ANTENNA);
			WriteAlarmPara(ResetAcktion,Alarm0,ALARM_GNSS_SHORT_CIRCUIT); */
			memset(Pos_Inf.Time,0,6);
			oa_memcpy(Pos_Inf.Latitude,"00000000",8);
			oa_memcpy(Pos_Inf.Longitude,"000000000",9);
		break;
		case UNFIXEDUPDATE:
			Pos_Inf.Fix_Status=GPS_UNFIXED;
		/*	WriteAlarmPara(ResetAcktion,Alarm1,STA_GPS_FIXED);
			oa_memcpy(Pos_Inf.Latitude,"00000000",8);
			oa_memcpy(Pos_Inf.Longitude,"000000000",9); */
		break;
		case ANTUPDATE:													 /*��������״̬*/
			Pos_Inf.Ant_Status=GpsInfor->Ant_Status;
			/*switch (Pos_Inf.Ant_Status)
			{
				case GPS_ANT_OPEN:
					WriteAlarmPara(SetAcktion,Alarm0,ALARM_GNSS_ANTENNA);
				break;
				case GPS_ANT_SHORT:
					WriteAlarmPara(SetAcktion,Alarm0,ALARM_GNSS_SHORT_CIRCUIT);
				break;
				default:
					WriteAlarmPara(ResetAcktion,Alarm0,ALARM_GNSS_ANTENNA);
					WriteAlarmPara(ResetAcktion,Alarm0,ALARM_GNSS_SHORT_CIRCUIT);
				break; 
			}*/
		break;
		case HEIGHTUPDATE:
			Pos_Inf.Height=GpsInfor->Height;
		break;
		default:
		break;
	}
	//OSSemPost(GPS_DataSem) ;
}
/*********************************************************
*Function:       GetPosinf(u8 *Str,u8 Filed,u8 Mode)
*Description:    ���� GPS���ݽṹ��	 Pos_Inf ��ALRAM��GPS�ֶ�
*Calls:          
*Called By:      GPS_Start_Init
*Input:          GpsInfor  GPS����Դ
				 UpdateMode ���·�ʽ

*Output:         ��
*Return:         ��
*Others:         
*********************************************************/
u8 GetPosinf(u8 *Str,u8 Filed,u8 Mode)
{
	u8 err;
	u8 Result;

	if (Str==NULL)
	{
		return 0;
	}
	//OSSemPend(GPS_DataSem,0,&err);
	switch(Filed)
	{
		case GPSFixedStatus:
			if(Mode==ASC_Code)
			{
				if(Pos_Inf.Fix_Status==GPS_FIXED)
				{
					*Str='A';	
				}
				else
				{
					*Str='V';	
				}		
			}
			else
			{
				*Str=Pos_Inf.Fix_Status;	
			}
			Result=1;
		break;
		case GPSLat:
			if(Mode == 5)
			{
				u32 U32temp = 0;
				float F32temp=0.0;
				u8 cnt;

				for(cnt=0;cnt<2;cnt++) //γ��,�����֮һ��
				{
					U32temp=U32temp*10+Pos_Inf.Latitude[cnt]-'0';
				}
				for(cnt=2;cnt<8;cnt++) //γ��
				{
					F32temp=F32temp*10+Pos_Inf.Latitude[cnt]-'0';
				}
				F32temp=F32temp*10.0/6.0;
				U32temp=U32temp*1000000+(u32)(F32temp);
				oa_memcpy(Str,(u8*)&U32temp,4);
				Result=4;
			}
			else{
				oa_memcpy(Str,Pos_Inf.Latitude,8);
				Result=8;
			}
			
		break;
		case GPSNInd:
			*Str=Pos_Inf.North_Indicator;	
			Result=1;
		break;
		case GPSLon:
			if(Mode == 5)
			{
				u32 U32temp = 0;
				float F32temp=0.0;
				u8 cnt;

				for(cnt=0;cnt<3;cnt++) //γ��,�����֮һ��
				{
					U32temp=U32temp*10+Pos_Inf.Longitude[cnt]-'0';
				}
				for(cnt=3;cnt<9;cnt++) //γ��
				{
					F32temp=F32temp*10+Pos_Inf.Longitude[cnt]-'0';
				}
				F32temp=F32temp*10.0/6.0;
				U32temp=U32temp*1000000+(u32)(F32temp);
				oa_memcpy(Str,(u8*)&U32temp,4);
				Result=4;
			}
			else{
				oa_memcpy(Str,Pos_Inf.Longitude,9);
				Result=9;
			}
			
		break;
		case GPSEInd:
			*Str=Pos_Inf.East_Indicator;	
			Result=1;
		break;
		case GPSCog:
			if(Mode==ASC_Code)
			{
				*Str++=Pos_Inf.COG/100%10+'0';	
			    *Str++=Pos_Inf.COG/10%10+'0';	
				*Str++=Pos_Inf.COG%10+'0';	
				Result=3;	
			}
			else
			{
				*Str=Pos_Inf.COG;	
				Result=1;	
			}
		break;
		case GPSTime:
			if(Pos_Inf.Fix_Status==GPS_FIXED)
			{
				if(Mode==ASC_Code)
				{
					*Str++=((Pos_Inf.Time[0]>>4)&0x0F)+'0';
					*Str++=(Pos_Inf.Time[0]&0x0F)+'0';
					*Str++=((Pos_Inf.Time[1]>>4)&0x0F)+'0';
					*Str++=(Pos_Inf.Time[1]&0x0F)+'0';
					*Str++=((Pos_Inf.Time[2]>>4)&0x0F)+'0';
					*Str++=(Pos_Inf.Time[2]&0x0F)+'0';
					*Str++=((Pos_Inf.Time[3]>>4)&0x0F)+'0';
					*Str++=(Pos_Inf.Time[3]&0x0F)+'0';
					*Str++=((Pos_Inf.Time[4]>>4)&0x0F)+'0';
					*Str++=(Pos_Inf.Time[4]&0x0F)+'0';
					*Str++=((Pos_Inf.Time[5]>>4)&0x0F)+'0';
					*Str++=(Pos_Inf.Time[5]&0x0F)+'0';	
					Result=12;
				}
				else 
				{
					oa_memcpy(Str,Pos_Inf.Time,6);
					Result=6;	
				}
			}
			else
			{
				Result=0;
			}
		break;
		case GPSAnt:
		*Str=Pos_Inf.Ant_Status;
		Result=1;
		break;
		case GPSSpeed:
			if(Mode==ASC_Code)
			{
				Result=ceil( Pos_Inf.Speed);
				*Str++=Result/100%10+'0';
				*Str++=Result/10%10+'0';
				*Str++=Result%10+'0';
				Result=3;	
			}
			else
			{
				oa_memcpy(Str,&Pos_Inf.Speed,/*sizeof(float)*/sizeof(u16));
				Result=/*sizeof(float)*/sizeof(u16);	
			}
		break;	
	}
	//OSSemPost(GPS_DataSem) ;
	return Result; 
}
/*********************************************************
*Function:       GPS_HW_Init()
*Description:    Ӳ����ʼ��
*Calls:          
*Called By:      
*Input:         ��
*Output:        ��
*Return:        ��
*Others:         
*********************************************************/
//u8 GPS_HW_Init(void)
//{
//	DMA_InitTypeDef DMA_InitStructure;

//	DMA_Q_StructCreate(&GPS_Rcv_Buf,GPS_DMA_RX_CHANNEL,DMA_ReadATBuf,GPS_DMA_RCV_BUF_SIZE);//����DMA���չ���ṹ��
//	GPS_LowLevel_DeInit();
	
	/*��ʼ��UART DMAͨ�� IO����*/
//	GPS_LowLevel_Init(DMA_ReadATBuf,GPS_DMA_RCV_BUF_SIZE);

	/*����DMA�����ж�*/
//    DMA_ITConfig(GPS_DMA_RX_CHANNEL, GPS_DMA_IT_TC_RX, ENABLE);
//    BSP_IntVectSet(GPS_INT_ID_DMA_RX_CH, GPS_DMA_RcvIsr);
//    BSP_IntEn(GPS_INT_ID_DMA_RX_CH);
						   
//	return 0;
//}
/*********************************************************
*Function:       GPS_DMA_RcvIsr()
*Description:    GPS DMA�����жϷ������
*Calls:          
*Called By:      
*Input:         ��
*Output:        ��
*Return:        ��
*Others:         
*********************************************************/
//void GPS_DMA_RcvIsr(void)
//{
//	CPU_SR cpu_sr;

//	CPU_CRITICAL_ENTER();
//	OSIntNesting++;
//	CPU_CRITICAL_EXIT();
//	if(DMA_GetITStatus(GPS_DMA_IT_TC_RX)) //ͨ����������ж�TC
//    {
//    	DMA_ClearITPendingBit(GPS_DMA_IT_GL_RX);    //���ȫ���жϱ�־
//    	DMA_Int_Call(&GPS_Rcv_Buf);
//	}
//	OSIntExit();
//}
/*********************************************************
*Function:       GPS_SW_Init()
*Description:    ����ģ��״̬
*Calls:          
*Called By:      
*Input:         ��
*Output:        ��
*Return:        ��
*Others:         
*********************************************************/
#ifdef UBLOX
u8 GPS_SW_Init(void)
{
	u32 i;
	u32 j;
	u8  status,err;
	u32 result;
	
	

	#if GPS_PRINT
	//DEBUG(PrintError,"GPSͣ��\r\n"); 
	#endif
//	if (GPS_SendSem==0)
//	{
//		GPS_SendSem=OSSemCreate(1);
//	}
//	if (GPS_DataSem==0)
//	{
//		GPS_DataSem=OSSemCreate(1);
//	}

	//OSSemPend(GPS_DataSem,0,&err);
	Pos_Inf.Fix_Status=GPS_UNFIXED;
	oa_strncpy(Pos_Inf.Latitude,"00000000",sizeof(Pos_Inf.Latitude));
	oa_strncpy(Pos_Inf.Longitude,"000000000",sizeof(Pos_Inf.Longitude));
	Pos_Inf.East_Indicator='E';
	Pos_Inf.North_Indicator='N';
	//OSSemPost(GPS_DataSem);
	return 0;
#if 0
	GPS_CFG_RST(Hot_Start,GPS_Stop);
	if (GPS_Wait_Return(UBX_CFG_RST_OK,UBX_CFG_RST_ERROR,10))	  
	{
		#if GPS_PRINT
		//DEBUG(PrintError,"GPSģ���ʼ��ʧ��!\r\n");
		#endif
		return 1;
	}
	/*���ιر������ֶ����*/
    for( i=0;i<GPDTM+1;i++)
    {
    	#if GPS_PRINT
		//DEBUG(PrintDevDebug,"GPS�ر�NMEA���\r\n");
		#endif
		GPS_CFG_MSG(NMEA_GP,i,0x00);	   //0x40
		if (GPS_Wait_Return(UBX_CFG_MSG_OK,UBX_CFG_MSG_ERROR,10))
		{
			#if GPS_PRINT
			//DEBUG(PrintError,"GPSģ���ʼ��ʧ��!\r\n");
			#endif
			return 1;
		}
    }
/*	#if GPS_PRINT
	//DEBUG(PrintInfo,"GPS�ر�NMEA���\r\n");
	#endif
	GPS_CFG_MSG(NMEA_GP,GPGPQ,0x00);
	j=10;								   //0x02
	while(j--)
	{
		result=GPS_DataAnaly();
		OSTimeDly(50);
	}
	#if GPS_PRINT
	//DEBUG(PrintInfo,"GPS�ر�NMEA���\r\n");
	#endif
	GPS_CFG_MSG(NMEA_GP,GPTXT,0x00);
	if (GPS_Wait_Return(UBX_CFG_MSG_OK,UBX_CFG_MSG_ERROR,10))
	{
		#if GPS_PRINT
		//DEBUG(PrintInfo,"GPSģ���ʼ��ʧ��!\r\n");
		#endif
		return 1;
	}
   */
	/*��GPRMC�ֶκ�GPGGA�ֶ����*/
	#if GPS_PRINT
	//DEBUG(PrintDevDebug,"GPS����NMEA���\r\n");
	#endif
    GPS_CFG_MSG(NMEA_GP,GPRMC,0x01);
	if (GPS_Wait_Return(UBX_CFG_MSG_OK,UBX_CFG_MSG_ERROR,10))
	{
		#if GPS_PRINT
		//DEBUG(PrintError,"GPSģ���ʼ��ʧ��!\r\n");
		#endif
		return 1;
	}
	#if GPS_PRINT
	//DEBUG(PrintDevDebug,"GPS����NMEA���\r\n");
	#endif
    GPS_CFG_MSG(NMEA_GP,GPGGA,0x01);
	if (GPS_Wait_Return(UBX_CFG_MSG_OK,UBX_CFG_MSG_ERROR,10))
	{
		#if GPS_PRINT
		//DEBUG(PrintError,"GPSģ���ʼ��ʧ��!\r\n");
		#endif
		return 1;
	}
	#if GPS_PRINT
	//DEBUG(PrintDevDebug,"GPS ����\r\n");
	#endif
	GPS_CFG_NAV_SET();	//�����ڲ��Ĵ���  ���÷�ֹƯ�Ʋ�����
	if (GPS_Wait_Return(UBX_NAV_MSG_OK,UBX_CFG_RST_ERROR,10))
	{
		#if GPS_PRINT
		//DEBUG(PrintDevDebug,"GPSģ���ʼ��ʧ��!\r\n");
		#endif
		return 1;
	}
    GPS_CFG_RST(Hot_Start,GPS_Start);
	if (GPS_Wait_Return(UBX_CFG_RST_OK,UBX_CFG_RST_ERROR,10))
	{
		#if GPS_PRINT
		//DEBUG(PrintDevDebug,"GPSģ���ʼ��ʧ��!\r\n");
		#endif
		return 1;
	}
	//OSTimeDly(30);     /*��ʱ���ȴ�GPS����*/
	#if GPS_PRINT
	//DEBUG(PrintDevDebug,"GPS NAV����\r\n");
	#endif
#endif
	return 0;
}
/*********************************************************
*Function:       GPS_Wait_Return()
*Description:    �ȴ��ظ� 
*Calls:          ��
*Called By:      
*Input:          
*Output:         ��
*Return:         0:�յ�GPSģ��ظ�  1δ�յ�GPSģ��ظ�
*Others:          
*********************************************************/

static u8 GPS_Wait_Return(u32 Response,u32 UnResponse,u8 Timecnt)
{
	u8 cnt = Timecnt;
	u32 result;

	//OSTimeDly(10);
	while(cnt--)
	{
		result=GPS_DataAnaly();
		if (result&Response)
		{
			return 0; /*�յ�ȷ���ظ� */
		}
		else if (result&UnResponse)
		{
			return 1;/*�յ�����ظ�*/
		}
		//OSTimeDly(50);
	}
	return 1;
}

/*********************************************************
*Function:       GPS_CFG_RST()
*Description:    ����GPSģ���ڲ��Ĵ��� 
*Calls:          ��
*Called By:      
*Input:          
*Output:         ��
*Return:         ��
*Others:          
*********************************************************/
void GPS_CFG_RST(u8 Start_Mode,u8 Rst_Mode)
{
	u8 SendStr[RST_CMD_LEN]={0};
	SendStr[0]=0xB5;
	SendStr[1]=0x62;
	SendStr[2]=0x06;
	SendStr[3]=0x04;							   
	SendStr[4]=0x04;
	SendStr[5]=0x00;
	switch(Start_Mode)
	{
		case Hot_Start:
		SendStr[6]=0x00;
		SendStr[7]=0x00;
		break;
		case Warm_Start:
		SendStr[6]=0x01;
		SendStr[7]=0x00;
		break;
		case Cold_Start:
		SendStr[6]=0xFF;
		SendStr[7]=0xFF;
		break;
		default:
		SendStr[6]=0xFF;
		SendStr[7]=0xFF;
		break;
	}
	switch(Rst_Mode)
	{
		case HardWare_Reset:
		SendStr[8]=0x00;
		break;
		case SoftWare_Reset:
		SendStr[8]=0x01;
		break;
		case GPS_SoftWare_Reset:
		SendStr[8]=0x02;
		break;
		case GPS_Stop:
		SendStr[8]=0x08;
		break;
		case GPS_Start:
		SendStr[8]=0x09;
		break;
		default:
		SendStr[8]=0x00;
		break;
	}
	SendStr[9]=0x00;
	UBX_Check(SendStr,&SendStr[10]);
	GPS_Send_Data(SendStr,RST_CMD_LEN);
}
/*********************************************************
*Function:       GPS_CFG_PRT()
*Description:    ����GPSģ���ڲ��Ĵ��� 
*Calls:          ��
*Called By:      
*Input:          
*Output:         ��
*Return:         ��
*Others:          
*********************************************************/
static void GPS_CFG_PRT(u32 USART_Baud,u16 In_Proto,u16 Out_Proto)
{
	u8 Prt_Str[PRT_CMD_LEN]={0};

	u8 *P=Prt_Str;
	*P++=0xB5;
	*P++=0x62;
	*P++=0x06;
	*P++=0x00;
	*P++=0x14;
	*P++=0x00;
	*P++=GPS_USE_USART1;
	*P++=0x00;
	*P++=0x00;
	*P++=0x00;
	*P++=USART_Set&0xFF;
	*P++=(USART_Set>>8)&0xFF;
	*P++=(USART_Set>>16)&0xFF;
	*P++=(USART_Set>>24)&0xFF;
	*P++=USART_Baud&0xFF;
	*P++=(USART_Baud>>8)&0xFF;
	*P++=(USART_Baud>>16)&0xFF;
	*P++=(USART_Baud>>24)&0xFF;
	*P++=In_Proto&0xFF;
	*P++=(In_Proto>>8)&0xFF;
	*P++=Out_Proto&0xFF;
	*P++=(Out_Proto>>8)&0xFF;
	*P++=0x00;
	*P++=0x00;
	*P++=0x00;
	*P++=0x00;
	UBX_Check(Prt_Str,P);
	GPS_Send_Data(Prt_Str,PRT_CMD_LEN);
}
/*********************************************************
*Function:       GPS_CFG_MSG()
*Description:    ����GPSģ���ڲ��Ĵ��� 
*Calls:          ��
*Called By:      
*Input:          
*Output:         ��
*Return:         ��
*Others:          
*********************************************************/
void GPS_CFG_MSG(u8 Msg_Class,u8 Msg_Id,u8 Msg_Rate)
{
	u8 SendStr[MSG_CMD_LEN];

	SendStr[0]=0xB5;
	SendStr[1]=0x62;
	SendStr[2]=0x06;
	SendStr[3]=0x01;
	SendStr[4]=0x03;
	SendStr[5]=0x00;
	SendStr[6]=Msg_Class;
	SendStr[7]=Msg_Id;
	SendStr[8]=Msg_Rate;
	UBX_Check(SendStr,&SendStr[9]);
	GPS_Send_Data(SendStr,MSG_CMD_LEN);
}
/*********************************************************
*Function:       GPS_CFG_ANT_SET()
*Description:    �������߼��
*Calls:          UBX_Check
                 GPS_Send_Data
                 Ack_Status
				 Wait_UBX_Sem
*Called By:      
*Input:          ��
*Output:         ��
*Return:         0x00 �ɹ�
                 0x01 ʧ��
*Others:         
*********************************************************/
//static void GPS_CFG_ANT_SET()
//{
//	u8 SendStr[ANT_CMD_LEN];
//	u8 *P=SendStr;
//
//	*P++=0xB5;
//	*P++=0x62;
//	*P++=0x06;
//	*P++=0x13;
//	*P++=0x04;
//	*P++=0x00;
//	*P++=0x17;
//	*P++=0x00;
//	*P++=0x0F;
//	*P++=0x64;
//	UBX_Check(SendStr,P);
//	GPS_Send_Data(SendStr,ANT_CMD_LEN);
//}
/*********************************************************
*Function:       GPS_CFG_NAV_SET()
*Description:    
*Calls:          UBX_Check
                 GPS_Send_Data
                 Ack_Status
				 Wait_UBX_Sem
*Called By:      
*Input:          ��
*Output:         ��
*Return:         0x00 �ɹ�
                 0x01 ʧ��
*Others:         
*********************************************************/
void GPS_CFG_NAV_SET(void)
{
	u8 SendStr[44];
	u8 *PStr=SendStr;

	*PStr++=0xB5;
	*PStr++=0x62;
	*PStr++=0x06;
	*PStr++=0x24;
	*PStr++=0x24;
	*PStr++=0x00;
	*PStr++=0xFF;	
	*PStr++=0xFF;
	*PStr++=0x04;
	*PStr++=0x02;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x10;
	*PStr++=0x27;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x05;
	*PStr++=0x00;
	*PStr++=0x64;
	*PStr++=0x00;
	*PStr++=0x64;
	*PStr++=0x00;
	*PStr++=0x32;
	*PStr++=0x00;
	*PStr++=0x32;
	*PStr++=0x00;
	*PStr++=0x64;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x00;
	*PStr++=0x00;
	UBX_Check(SendStr,PStr);
	GPS_Send_Data(SendStr,44);
}
/*********************************************************
*Function:       UBX_Check()
*Description:    ����UBXָ���У��ֵ��
*Calls:          ��
*Called By:      Ack_Status
                 GPS_CFG_RST
                 GPS_CFG_PRT
                 GPS_CFG_MSG
*Input:          Str ָ������ָ��
                 pCheck ���ڴ洢У������ָ��
*Output:         pCheck
*Return:         0x00 �ɹ�
                 0x01 ʧ��
*Others:         
*********************************************************/
u8 UBX_Check( u8 *Str,u8 *pCheck)
{
  u16 Check_Lenth=0;
  u8 *P=Str;
  u8 CK_A = 0;
  u8 CK_B = 0;
  if(*P==0xb5&&*(P+1)==0x62)
  {
    u16 i;
    P+=2;
    CK_A+=*P++;
    CK_B+=CK_A;
    Check_Lenth=*(P+1)+(*(P+2)<<4);
    for(i=0;i<Check_Lenth+3;i++)
    {
      CK_A+=*P++;
      CK_B+=CK_A;
    }
    *pCheck++=CK_A;
    *pCheck++=CK_B;
//	*pCheck='\0';
    return 0x00;
  }
  else
  {
  	*P='\0';
    return 0x01;
  }
}
#endif

/*********************************************************
*Function:       GPS_SW_Init()
*Description:    ��ʼ��
*Calls:          
*Called By:      
*Input:          ��
*Output:         ��
*Return:         0x00 �ɹ�
                 0x01 ʧ��
*Others:         
*********************************************************/
#if 0
u8 GPS_SW_Init(void)
{
	u8 err;
	u8 i;

	if (GPS_SendSem==0)
	{
		GPS_SendSem=OSSemCreate(1);
	}
	if (GPS_DataSem==0)
	{
		GPS_DataSem=OSSemCreate(1);
	}

	//OSSemPend(GPS_DataSem,0,&err);
	Pos_Inf.Fix_Status=GPS_UNFIXED;
	Str_Copy_N(Pos_Inf.Latitude,"00000000",sizeof(Pos_Inf.Latitude));
	Str_Copy_N(Pos_Inf.Longitude,"000000000",sizeof(Pos_Inf.Longitude));
	Pos_Inf.East_Indicator='E';
	Pos_Inf.North_Indicator='S';
	//OSSemPost(GPS_DataSem);

	/*���ιر������ֶ����*/
    for( i=0;i<GPZDA+1;i++)
    {
		GPS_OutPut_En(i,0x00);	   //0x40
		OSTimeDlyHMSM(0,0,0,200);
    }
	GPS_OutPut_En(GPRMC,1);
	GPS_OutPut_En(GPGGA,1);	 
	GPS_HotReset_En();
}
/*********************************************************
*Function:       GPS_OutPut_En()
*Description:    ʹ��NMEA�ֶ����
*Calls:         
*Called By:      
*Input:          Type:�ֶ�����
				 ONOFF:�رջ��ߴ�  0:�ر� 1:��
*Output:         ��
*Return:         
*Others:         
*********************************************************/
static void GPS_OutPut_En(u8 Type,u8 ONOFF)
{
	u8 SendStr[44];
	u8 *PStr=SendStr;

	if ((Type>8)||(ONOFF>1))
	{
		return;
	}
	sprintf(SendStr,"$PSRF103,%02d,00,%02d,00*25\r\n",Type,ONOFF);
	GPS_Send_Data(SendStr,strlen(SendStr));
}
/*********************************************************
*Function:       GPS_HotReset_En()
*Description:    ����ģ��������
*Calls:         
*Called By:      
*Input:          
*Output:         ��
*Return:         
*Others:         
*********************************************************/
static void GPS_HotReset_En(void)
{
	GPS_Send_Data((u8 *)HotStart,strlen(HotStart));
}
#endif
/*********************************************************
*Function:       GPS_Send_Data()
*Description:    ��GPS������������ 
*Calls:          ��
*Called By:      GPS_CFG_RST
                 GPS_CFG_PRT
                 GPS_CFG_MSG
*Input:          Str  ����ָ��
                 Lenth  ���ݳ���
*Output:         ��
*Return:         ��
*Others:          
*********************************************************/
void GPS_Send_Data( u8 *Str,u8 Length)
{
#if 0
	u8 err ;
	OSSemPend(GPS_SendSem,0,&err);
	while(Length-->0)
	{
		USART_SendData(GPS_COM,*Str++);
		while(!USART_GetFlagStatus(GPS_COM,USART_FLAG_TXE));
	}
	OSSemPost(GPS_SendSem) ;
#endif
	if (NULL == Str || Length == 0)
	{
		DEBUG("paras err");
		return;
	}
	if (OA_FALSE == oa_uart_write(GPS_COM, Str, Length))
	{
		DEBUG("paras err");
		return;
	}
}
/*********************************************************
*Function:       UTC_To_RTC()
*Description:    ��GPS��UTCʱ��ת��Ϊ����ʱ��
*Calls:          ��
*Called By:      GPS_USART_ISR
*Input:          STRUCT_RMC ��ǰ��ȡ��GPS���ݽṹ��ָ��
*Output:         ��STRUCT_RMCʱ�����ݽ����޸�
*Return:         ��
*Others:         
*********************************************************/
static void UTC_To_RTC(STRUCT_RMC *POS_Inf)
{
  u8 i;
  for( i=0;i<6;i++)
  {
    POS_Inf->Time[i]=POS_Inf->Time[i]/16*10+POS_Inf->Time[i]%16;
  }
  POS_Inf->Time[3]+=POS_AREA;
  if(POS_Inf->Time[3]>23)//����24��
  {
    POS_Inf->Time[3]-=24;
	if(POS_Inf->Time[2]+1>HowManyDays(POS_Inf->Time[0],POS_Inf->Time[1]))//Ҫ���·�
    {
       POS_Inf->Time[2]=1;
       if(POS_Inf->Time[1]+1>12)//Ҫ�����
       {
         POS_Inf->Time[1]=1;
         POS_Inf->Time[0]+=1;
         if(POS_Inf->Time[0]>=100)
         {
           POS_Inf->Time[0]-=100;
         }
       }
       else
       {
         POS_Inf->Time[1]+=1;
       }
    }
    else
    {
      POS_Inf->Time[2]+=1;
    }
  }
  for( i=0;i<6;i++)
  {
    POS_Inf->Time[i]=POS_Inf->Time[i]/10*16+POS_Inf->Time[i]%10;
  }
}
/*********************************************************
*Function:       HowManyDays()
*Description:    ���ڼ���ѡ�����µ��·ݵ�����
*Calls:          ��
*Called By:      UTC_To_RTC
*Input:          year ��� 0-100
                 month �·� 1-12
*Output:         ��
*Return:         ѡ���·ݵ�����
*Others:         
*********************************************************/
static u8 HowManyDays(u8 year ,u8 month)                  
{
    int days;
    if(month==1||month==3||month==5||month==7||month==8||month==10||month==12)
        return days=31;
    else if(month==2){
        if(year%4==0)//�豸Ӧ���ò���һ������
            return days=29;
        else return days=28;
    }
    else 
        return    days=30;
}
/*********************************************************
*Function:       u32 Get_Space_Time(STRUCT_RMC  GPS_INF,u8 *Pass_Time)
*Description:    ��ȡʱ����  Pass_Time �ĸ�ʽӦ��	  GPS_INF ��ʱ�����ͬ
*Calls:          ��
*Called By:      ��
*Input:          GPS_INF ��ǰ��λ��������Ϣ
				 Pass_Time �洢�Ĺ�ȥʱ����Ϣ 
*Output:         ��
*Return:        ʱ��������
*Others:          
*********************************************************/
s32 Get_Space_Time(u8 *Now_Time,u8 *Pass_Time)
{
	u32 GPS_Second_Cnt=0;
	u32	Pass_Time_Sencond_Cnt=0;
	u32 pass_day;
	pass_day=diff_day(Pass_Time,Now_Time);
	if(pass_day>=(u32)0)
	{
		GPS_Second_Cnt=(((Now_Time[3]&0xF0)>>4)*10+(Now_Time[3]&0x0F))*3600+(((Now_Time[4]&0xF0)>>4)*10+(Now_Time[4]&0x0F))*60+(((Now_Time[5]&0xF0)>>4)*10+(Now_Time[5]&0x0F))	 ;
		Pass_Time_Sencond_Cnt=(((Pass_Time[3]&0xF0)>>4)*10+(Pass_Time[3]&0x0F))*3600+(((Pass_Time[4]&0xF0)>>4)*10+(Pass_Time[4]&0x0F))*60+(((Pass_Time[5]&0xF0)>>4)*10+(Pass_Time[5]&0x0F))	 ;
	
		if((Pass_Time_Sencond_Cnt>GPS_Second_Cnt)&&(pass_day>0))//��������
		{
			GPS_Second_Cnt+=24*3600;
			return 	  GPS_Second_Cnt- Pass_Time_Sencond_Cnt+(pass_day-1)*24*3600;
		}
		else if((Pass_Time_Sencond_Cnt<=GPS_Second_Cnt)&&(pass_day==0))
		{
			return 	  GPS_Second_Cnt- Pass_Time_Sencond_Cnt;	
		}
		else if((Pass_Time_Sencond_Cnt<GPS_Second_Cnt)&&(pass_day>0))
		{
			return 	  GPS_Second_Cnt- Pass_Time_Sencond_Cnt+(pass_day)*24*3600;	
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
	
}

static int diff_day (unsigned char* date1,unsigned char *date2 ) 
{ 
    long year, day, day1, day2; 
    int s_year, s_month, s_day, e_year, e_month, e_day;//sΪ��ʼ�����ڣ�eΪ��ֹ����

	s_year=2000+BCDToDec(date1[0]);
	s_month=BCDToDec(date1[1]);
	s_day=BCDToDec(date1[2]);

	e_year=2000+BCDToDec(date2[0]);
	e_month=BCDToDec(date2[1]);
	e_day=BCDToDec(date2[2]);

	//��ͬ���ʱ���ж�// 
    if ( s_year < e_year ) 
    { 
        day = count_day ( s_year, s_month, s_day , 0 );
		if (day<0)
		{
			return -1;
		}
        for (year=s_year+1;year<e_year; year++ )//����ۼ�
        if (leap(year))//��������
            day += 366L; 
        else   //ƽ��
            day += 365L; 
        day1 = count_day (e_year, e_month, e_day, 1);
		if (day1<0)
		{
			return -1;
		}
		day +=day1;

                
    } 
        //ͬһ���ʱ���ж�
    else if ( s_year == e_year ) 
            { 
                    day1 = count_day ( s_year, s_month, s_day, 1 ); 
					if (day1<0)
					{
						return -1;
					}
                    day2 = count_day ( e_year, e_month, e_day, 1 ); 
					if (day2<0)
					{
						return -1;
					}
                    day = day2-day1;//��ı���������
                } 
            else 
                day = -1; //��ʶ
                return day;

} 
 //���������ĺ���// 
static int count_day ( int year, int month, int day, int flag ) 
 { 
   static int day_tab[2][12] = {{31,28,31,30,31,30,31,31,30,31,30,31},{31,29,31,30,31,30,31,31,30,31,30,31}};
   //��ά����������
   int p, i, s;
		
        if (leap(year))                                                                                //����Ϊ1ѡ��������
            p = 1; 
        else   //ƽ��Ϊ0ѡ��ǰ�����
           p = 0; 
		if ((day<=0)||(day>day_tab[p][month-1]))
		{
			return -1;
		}
        if (flag) 
        { 
          s = day; 
          for ( i = 1; i < month; i++ ) 
            s += day_tab[p][i-1];//��ѯ�����ۼ�����
         } 
        else 
        { 
           s = day_tab[p][month-1] - day; 
           for ( i = month + 1; i <= 12; i++ ) 
             s += day_tab[p][i-1]; 
        } 
        return ( s );//��������
 } 



/*********************************************************
*Function:       GPS2Point_Distance()
*Description:    �����������
*Calls:          ��
*Called By:      ��
*Input:          New_Point_Lon���µ�ľ���
				 New_Point_Lat���µ��γ��
				 Old_Point_Lon���ɵ�ľ���
				 Old_Pont_Lat���ɵ��γ��
*Output:         ��
*Return:         ������루km��
*Others:          
*********************************************************/
void GPS2Point_Distance(oa_double New_Point_Lon,oa_double New_Point_Lat,oa_double Old_Point_Lon,oa_double Old_Pont_Lat, oa_double *res )
{
	oa_double    Temp;
	oa_double    x,y;
	x=(New_Point_Lon-Old_Point_Lon)*Pi*R*oa_cos( ((New_Point_Lat+Old_Pont_Lat)/2) *Pi/180)/180;  
	y=(New_Point_Lat-Old_Pont_Lat)*Pi*R/180;  
	//Temp=myhypot(x, y);
	Temp = hypot(x, y);
	OA_DEBUG_USER("x:%lf y:%lf Temp:%lf myhypot:%lf", x, y, Temp, hypot(x,y));
	*res = Temp;
}
/*********************************************************
*Function:       GPS_Lon_Change()
*Description:    ����õ�GPS��γ��ת��Ϊ��
*Calls:          ��
*Called By:      ��
*Input:          STRUCT_RMC��GPS�ṹ��
  				 Mode ��1���� 0γ��
*Output:         ��
*Return:         ת�������
*Others:          
*********************************************************/
float GPS_Local_Change(u8 *Lon2Lat,u8 Mode, float *res)
{
	float    Temp=0;
	u8 i=0;
	u8 Degrees=0 ;
	u32 Minutes=0;
	if(Mode)							/*����lon*/
	{
		for(;i<3;i++)
		{
			Degrees	=Degrees*10+(*(Lon2Lat+i))-0x30;
		}
		for(;i<9;i++)
		{
			Minutes	=Minutes*10+(*(Lon2Lat+i))-0x30;	
		}
		Temp= Degrees+ ((Minutes+0.0)/powme(10,4))/60;
		*res = Temp;
		return Temp;
	}
	else						   		/*γ��*/
	{
		for(;i<2;i++)
		{
			Degrees	=Degrees*10+(*(Lon2Lat+i))-0x30;
		}
		for(;i<8;i++)
		{
			Minutes	=Minutes*10+(*(Lon2Lat+i))-0x30;	
		}
		Temp= Degrees+ ((Minutes+0.0)/powme(10,4))/60;
		*res = Temp;
		return Temp;
	}
}

