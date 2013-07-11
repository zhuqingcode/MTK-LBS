/*
*********************************************************************************************************
*	                                  
*	模块名称 : 调度屏BSP层
*	文件名称 : SchedulScrn.c
*	版    本 : V1.0
*	说    明 : 调度屏控制和数据解析
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2012-1-18        魏江南  ST固件库V3.5.0版本。
*
*	Copyright (C), 2010-2011, 天朗科技
*
*********************************************************************************************************
*/
/*#include "lib_inc.h"*/
#include "SchedulScrn.h"
#include "oa_type.h"
#include "oa_api.h"
#include "oa_debug.h"
/*#include "Hardware.h"*/
/*****************************************/
extern uart_struct uart_contain;
/*****************************************/
/*变量设置*/
#define Schedul_RecvBuf_Num	 300//接收BUF大小
#define SEND_DATA_INTVL  30 //设定应答超时时间节拍数 D5协议应答时间为300ms  不可为0
#define WAIT_TIME_OUT  300 //设定数据等待超时时间节拍数 为0则永远等待

#define MAX_MSG_LEN 250

#define CheckParam1(param1) ((param1 == NULL))
#define CheckParam2(param1,param2) ((param1 == NULL)||(param2 == NULL))
#define CheckParam3(param1,param2,param3) ((param1 == NULL)||(param2== NULL)||(param3 == NULL))
#define CheckParam4(param1,param2,param3,param4) ((param1 == NULL)||(param2== NULL)||(param3 == NULL)||(param4 == NULL))

//static OS_EVENT * SchedulRcvSem;//收到调度屏数据通知信号量
//static OS_EVENT *Schedul_UsartSendSem;//串口保护信号量
//static OS_EVENT *SchedulAckSem=0;//收到应答信号量
//static OS_EVENT *SchedulProtect=0;//调度屏内部参数的保护信号量
//static DataQueue SchedulRecvQue;  //接收队列
static u8 SchedulRecvBuf[Schedul_RecvBuf_Num];//调度屏接收缓区
static u8 IntRecvBuf[300];//调度屏接收缓区,作为中断中接收
STRUCT_SScrn_Result SScrn_Result;
//static USART_TypeDef *SSCRN_COM=0;
/*函数声明,供内部调用*/
static void SchedulScrn_RcvPhase(u8 Com_Data);//调度屏数据接收
static void SchedulScrn_USART_ISR(void);//串口中断

u8 SScrn_Result_Send(u8 result);
static u8 Ack_Package(u8 *Str,u8 mode,u8 Cmd,u8 Result);//简单数据包组包
static u8 Protocol_Check(const u8 *PStr,const u16 PStr_Length);//校验计算
static u8 Protocol_Check_2para(const u8 *PStr1,const u16 PStr_Length1,const u8 *PStr2,const u16 PStr_Length2);//校验计算  两段数据
oa_bool SchedulScrn_SendStr(u8 *Str,u16 Len);//串口发送函数	
static u8 Host_Send_Packag(u8 Cmd, u8 *Value,const u8 ValuLen);//打包发送
u8 SScrn_UpResult(u8 *Str,u8 Cmd);
static u8 SScrn_Ack_Send(void); /*发送数据接收应答*/
#if 0
/*********************************************************
*Function:       SScrn_HW_Init()
*Description:     调度屏相关硬件初始化

*Calls:          
*Called By:       
*Input:          无
*Output:        无
*Return:        无
*Others:
*********************************************************/
void SScrn_HW_Init(USART_TypeDef *COM)
{
	SSCRN_COM=COM;
	if(SSCRN_COM==GENERAL_COM4)
	{
		GENERAL_COM4_LowLevel_Init(Schedul_Baud);
		USART_ITConfig(GENERAL_COM4,USART_IT_RXNE,ENABLE);
		BSP_IntVectSet(GENERAL_COM4_INT, SchedulScrn_USART_ISR);
		BSP_IntEn(GENERAL_COM4_INT);
	}
	else if(SSCRN_COM==GENERAL_COM5)
	{
		GENERAL_COM5_LowLevel_Init(Schedul_Baud);
		USART_ITConfig(GENERAL_COM5,USART_IT_RXNE,ENABLE);
		BSP_IntVectSet(GENERAL_COM5_INT, SchedulScrn_USART_ISR);
		BSP_IntEn(GENERAL_COM5_INT);
	}
	else if(SSCRN_COM == RS485_COM)
	{
		RS485_COM_LowLevel_Init(Schedul_Baud);
		USART_ITConfig(RS485_COM,USART_IT_RXNE,ENABLE);
		BSP_IntVectSet(RS485_COM_INT, SchedulScrn_USART_ISR);
		BSP_IntEn(RS485_COM_INT);
	}
	else
	{
		SSCRN_COM=0;
	}
}
#endif
/*********************************************************
*Function:       SScrn_SW_Init()
*Description:     调度屏相关软件相关初始化

*Calls:          
*Called By:       
*Input:          无
*Output:        无
*Return:        无
*Others:
*********************************************************/
void SScrn_SW_Init()
{
#if 0
	if(SchedulAckSem==0)
	{
		SchedulAckSem = OSSemCreate(0);//收到应答通知信号量 
	}
	if(SchedulRcvSem==0)
	{
		SchedulRcvSem = OSMboxCreate(0);//收到数据通知信号邮箱
	}
	if(Schedul_UsartSendSem==0)
	{
		Schedul_UsartSendSem = OSSemCreate(1);//串口资源保护信号量
	}
	if(SchedulProtect==0)
	{
		SchedulProtect = OSSemCreate(1);//数据读写保护信号量
	}
	QueueCreate(SchedulRecvBuf,Schedul_RecvBuf_Num,&SchedulRecvQue);//初始化接收队列
#endif
	memset(&SScrn_Result,0,sizeof(SScrn_Result));
}
/*********************************************************
*Function:       SScrn_HW_Init()
*Description:     调度屏相关硬件初始化

*Calls:          
*Called By:       
*Input:          无
*Output:        无
*Return:        无
*Others:
*********************************************************/
//void SScrn_N990_HW_Init(USART_TypeDef *COM)
//{
//	SSCRN_COM=COM;
//	if(SSCRN_COM==GENERAL_COM4)
//	{
//		GENERAL_COM4_LowLevel_Init(SchedulN990_Baud);
//		USART_ITConfig(GENERAL_COM4,USART_IT_RXNE,ENABLE);
//		BSP_IntVectSet(GENERAL_COM4_INT, SchedulScrn_USART_ISR);
//		BSP_IntEn(GENERAL_COM4_INT);
//	}
//	else if(SSCRN_COM==GENERAL_COM5)
//	{
//		GENERAL_COM5_LowLevel_Init(SchedulN990_Baud);
//		USART_ITConfig(GENERAL_COM5,USART_IT_RXNE,ENABLE);
//		BSP_IntVectSet(GENERAL_COM5_INT, SchedulScrn_USART_ISR);
//		BSP_IntEn(GENERAL_COM5_INT);
//	}
//	else if(SSCRN_COM == RS485_COM)
//	{
//		RS485_COM_LowLevel_Init(SchedulN990_Baud);
//		USART_ITConfig(RS485_COM,USART_IT_RXNE,ENABLE);
//		BSP_IntVectSet(RS485_COM_INT, SchedulScrn_USART_ISR);
//		BSP_IntEn(RS485_COM_INT);
//	}
//	else
//	{
//		SSCRN_COM=0;
//	}
//}
#if 0
/*********************************************************
*Function:       SScrn_SW_Init()
*Description:     调度屏相关软件相关初始化

*Calls:          
*Called By:       
*Input:          无
*Output:        无
*Return:        无
*Others:
*********************************************************/
void SScrn_N990_SW_Init()
{
	if(SchedulAckSem==0)
	{
		SchedulAckSem = OSSemCreate(0);//收到应答通知信号量 
	}
	if(SchedulRcvSem==0)
	{
		SchedulRcvSem = OSMboxCreate(0);//收到数据通知信号邮箱
	}
	if(Schedul_UsartSendSem==0)
	{
		Schedul_UsartSendSem = OSSemCreate(1);//串口资源保护信号量
	}
	if(SchedulProtect==0)
	{
		SchedulProtect = OSSemCreate(1);//数据读写保护信号量
	}
	QueueCreate(SchedulRecvBuf,Schedul_RecvBuf_Num,&SchedulRecvQue);//初始化接收队列
	memset(&SScrn_Result,0,sizeof(SScrn_Result));
}

/*********************************************************
*Function:       SchedulScrn_USART_ISR()
*Description:    串口中断服务函数，检测平台数据、电话、短信
*Calls:
*Called By:
*Input:          无
*Output:
*Return:
*Others:
*********************************************************/
static void SchedulScrn_USART_ISR(void)
{
	u8 ComData =0;
	if(USART_GetITStatus(SSCRN_COM,USART_IT_RXNE)==SET)
	{
		USART_ClearITPendingBit(SSCRN_COM,USART_IT_RXNE);
		ComData=USART_ReceiveData(SSCRN_COM);
		SchedulScrn_RcvPhase(ComData);
	}
}
#endif
#if SCREEN_N990>0
/*********************************************************
*Function:       SchedulScrn_RcvPhase()
*Description:    数据接收状态机
*Calls:
*Called By:
*Input:            无
*Output:
*Return:
*Others:
*********************************************************/
//static void SchedulScrn_RcvPhase(u8 Com_Data)
//{
//	static u16 Rx_Couter=0; 
//	static u16 Content_Length=0;
//	static u16 BufCnt=0;
//	static u8 Check_Sum=0;
//	static u8 Step	 =0;
//	static u8 ack_flag;
//	switch (Step)
//	{
//		case 0:
//			if(Com_Data==0x0F) //头
//			{
//				IntRecvBuf[BufCnt++]=Com_Data;
//			    Step++;
//			}	
//			else
//			{
//				Check_Sum=0;
//				Step=0;
//				BufCnt=0;
//			}
//		break;
//		case 1: //cmd
//			Check_Sum=Com_Data;	
//			IntRecvBuf[BufCnt++]=Com_Data;
//			Step++;
//		break;
//		case 2:	//len
//			if((Com_Data>='0'&&Com_Data<='9')||(Com_Data>='A'&&Com_Data<='F')||(Com_Data>='a'&&Com_Data<='f'))
//			{
//				if(Com_Data<='9')
//					Content_Length=Com_Data-'0';
//				else if(Com_Data<='F')
//					Content_Length=Com_Data-0x37;
//				else
//					Content_Length=Com_Data-87;
//
//				Check_Sum^=Com_Data;
//				IntRecvBuf[BufCnt++]=Com_Data;
//				Rx_Couter=0;
//			 	Step++;
//			}
//			else
//			{
//				Step=0;
//				BufCnt=0;
//			}
//		break;
//		case 3:	//len
//			if((Com_Data>='0'&&Com_Data<='9')||(Com_Data>='A'&&Com_Data<='F')||(Com_Data>='a'&&Com_Data<='f'))
//			{
//				if(Com_Data<='9')
//					Content_Length=(Content_Length<<4)+Com_Data-'0';
//				else if(Com_Data<='F')
//					Content_Length=(Content_Length<<4)+Com_Data-0x37;
//				else
//					Content_Length=(Content_Length<<4)+Com_Data-87;
//				Check_Sum^=Com_Data;
//				IntRecvBuf[BufCnt++]=Com_Data;
//				Rx_Couter=0;
//				//if(Content_Length>=1)
//				{
//				 	Step++;
//				}
//			}
//			else
//			{
//				Step=0;
//				BufCnt=0;
//			}
//		break;
//		case 4:	//content
//			if(Rx_Couter<Content_Length)
//			{
//				Check_Sum^=Com_Data;
//				IntRecvBuf[BufCnt++]=Com_Data;
//				Rx_Couter++;
//			}
//			else
//			{
//				if(Check_Sum!=Com_Data)
//				{
//					Step=0;
//					BufCnt=0;
//				}
//				else
//				{
//					IntRecvBuf[BufCnt++]=Com_Data;
//					Step++;
//				}
//			}
//		break;
//		case 5:
//			if(Com_Data==0xF0)
//			{
//				Step=0;
//				IntRecvBuf[BufCnt++]=Com_Data;
//				{
//					if(OSMboxAccept(SchedulRcvSem)==0)
//					{
//						QueueFlush(&SchedulRecvQue);
//					}
//					QueueBufWrite(IntRecvBuf,BufCnt,&SchedulRecvQue,Normal_Write);
//					OSMboxPost(SchedulRcvSem,(void *)BufCnt);
//				}
//				BufCnt=0;
//			}
//			else
//			{
//				Step=0;
//				BufCnt=0;
//			}
//		break;
//		default:
//			Step=0;
//			BufCnt=0;
//		break;
//	}
//}
///*********************************************************
//*Function:       SScrn_DataAnaly()
//*Description:    对RecvBuf中数据解析，根据内容进行参数设置与
//*                处理
//*Calls:          
//*Called By:      
//*Input:          
//*Output:
//*Return:         收到的指令类别枚举
//*Others:
//*********************************************************/
//u8 SScrn_DataAnaly(void) 
//{
//	u8 err;
//	u8 RcvBuf[300];//接收缓存
//	u16 i=0;
//	u16 MsgLen=0;
//	u16 datalen=0;
//	u8 *pStr;
//	u16 *Flag=NULL;
//	u8 Cmd=0;
//	static u8 Swtchflag=0;
//			
//	MsgLen=(u16)(OSMboxPend(SchedulRcvSem,WAIT_TIME_OUT,&err));
////	if(err==OS_ERR_NONE)
////	{
////	//	OSMboxPost(SchedulRcvSem,(void *)1);
////	}
////	else 
//	if (err==OS_ERR_TIMEOUT)
//	{
//		return SCCRN_TIME_OUT;
//	}
//	else if(err!=OS_ERR_NONE)
//	{
//		return SCCRN_ERROR_RETURN;
//	}
//	pStr=RcvBuf;
//	for (i=0;i<MsgLen;i++)
//	{
//		QueueRead(pStr++,&SchedulRecvQue);
//	}
////	OSMboxPend(SchedulRcvSem,WAIT_TIME_OUT,&err);
//	SSCRN_Print("收到调度屏消息,消息长度:%d\r\n",MsgLen);
//	pStr=RcvBuf;
//	{
//		u8 test=0;
//		for (test=0;test<MsgLen;test++)
//		{
//	SSCRN_Print("0x%02x ",*(pStr+test));
//		}
//	SSCRN_Print("\r\n");
//	}
//	
//	if(*pStr++!=0x0F)
//	{}
//	else
//	{
//		Cmd=*pStr++;
//		switch(Cmd)
//		{
//			case N990_ONEBYTE_CMD:
//			case N990_SENDSMS_CMD:
//			case CALL_CAR_RSP_CMD:
//			case PICKUP_PASSEGER_CMD:
//			case PICKUP_PASSEGER_FAILED_CMD:
//			case REACH_DESTI_CMD:
//				SScrn_UpResult(pStr,Cmd);
//				SScrn_Result.CMD_TYPE=Cmd;
//			break;
////			case UNICODE_SMS_CMD://发送短信
////			case SET_UDP_CMD: //监控平台IP及UDP设定
////			case SET_TCP_CMD: //监控平台IP及TCP设定
////			case SET_LOCAL_TEL_CMD://设置本地号码
////			case CALL_DIALUP_CMD://电话拨出
////			case SET_SPK_CMD://设置通话音量
////			case SET_MIC_CMD://调节MIC音量
////			#if DRIVE_SIGN_EN
////			case DRIVER_SIGN_CMD://司机登录
////			#endif
////			#if CUSTOM_CMD
////			case COM_JTT808_CMD:   //jtt808消息包
////			#endif
////			case UPDATA_PARA_CMD:
////			case UPDATA_DATA_CMD:
////		#if PROTCL_HX54		
////			case STATION_DOWNLOAD_CMD:
////		#endif
////				SScrn_Ack_Send();/*应答*/
////				SScrn_UpResult(pStr,Cmd);
////				SScrn_Result.CMD_TYPE=Cmd;
////			break;
////			case CALL_HANGOFF_CMD: //无有效数据内容
////				SScrn_Ack_Send();/*应答*/
////				SSCRN_Print("调度屏设定挂机\r\n");
////				SScrn_Result.CMD_TYPE=CALL_HANGOFF_CMD;	
////			break;
////			case CALL_HANGON_CMD://接通电话
////				SScrn_Ack_Send();/*应答*/
////				SSCRN_Print("调度屏设定接通电话\r\n");
////				SScrn_Result.CMD_TYPE=CALL_HANGON_CMD;	
////			break;
////			case UPDATA_REQ_CMD://请求升级
////				SScrn_Ack_Send();/*应答*/
////				SSCRN_Print("PC请求升级\r\n");
////				SScrn_Result.CMD_TYPE=UPDATA_REQ_CMD;	
////			break;
////			case CALL_SPKSWITCH_CMD://切换语音通道
////				SScrn_Ack_Send();/*应答*/
////				SSCRN_Print("调度屏设定切换语音通道\r\n");
////				SScrn_Result.CMD_TYPE=CALL_SPKSWITCH_CMD;
////			break;
////		#if PROTCL_HX54
////			case STATION_DOWNLOAD_START_CMD:
////				SScrn_Ack_Send();/*应答*/
////				SSCRN_Print("串口导入线路开始......\r\n");
////				SScrn_Result.CMD_TYPE=STATION_DOWNLOAD_START_CMD;
////			break;
////		#endif
//			default:
//			//	SScrn_Result.CMD_TYPE=SCCRN_UNEXP_CMD;
//				SSCRN_Print("调度屏收到未定义消息:\r\n");
//				SSCRN_Print("指令码为:%02x\r\n",Cmd);
//				return 	SCCRN_UNEXP_CMD;
//			break;
//		}
//	}														 
//	return SScrn_Result.CMD_TYPE;
//}
/*********************************************************
*Function:       BSP_XOR_Check
*Description:    对Str开始的StrLen长度个数进行异或校验
*Calls:          无
*Called By:      无 
*Input:          无
*Output:         Check 校验结果
*Return:         0：成功 其他：失败
*Others:         
*********************************************************/
u8 BSP_XOR_Check(const u8 *Str,const u16 StrLen,u8 *Check)
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

/*********************************************************
*Function:       SchedulScrn_heartbeatSend()
*Description:    设备下发心跳包
*Calls:
*Called By:
*Input:            无
*Output:
*Return:	   0 OK
*Others:
*********************************************************/
u8 SchedulScrn_heartbeatSend()
{
	u8 err=1;
	u8 Buf[]={0x0F, 0x0D, 0x30, 0x31, 0x00, 0x0C, 0xF0, 0xFF};

//	OSSemPend(Schedul_UsartSendSem,0,&err);
	SchedulScrn_SendStr(Buf,8);
//	OSSemPost(Schedul_UsartSendSem);

	return 0;
}
#if 0
/*********************************************************
*Function:       SchedulScrn_CallforcarSend()
*Description:    设备下发电召抢单
*Calls:
*Called By:
*Input:            无
*Output:
*Return:
*Others:
*********************************************************/
u8 SchedulScrn_CallforcarSend(u8* ID,u8 idlen,u8* data,u16 datalen)
{
	u8 err=1;
	u8 pbuf_head[14]={0};
	u8 pbuf_Tail[3]={0};
	u8 *pStr=pbuf_head;
	u8 Len=0;

	*pStr++=0x0F;
	*pStr++=CALL_CAR_CMD;
	Len=(10+datalen);
	*pStr++=(Len>>4)>9?((Len>>4)+0x37):((Len>>4)+'0');
	*pStr++=(Len&0x0F)>9?((Len&0x0F)+0x37):((Len&0x0F)+'0');
	Mem_Copy(pStr,ID,idlen); //不足10位？？？？
	pStr+=idlen;

	pbuf_Tail[0]=Protocol_Check_2para(pbuf_head+1,13,data,datalen);  //校验码生成  分别从包头  和内容进行校验
	pbuf_Tail[1]=0xF0;
	pbuf_Tail[2]=0xFF;

	SSCRN_Print("向调度屏发送抢答订单\r\n");
	OSSemPend(Schedul_UsartSendSem,0,&err);
		SchedulScrn_SendStr(pbuf_head,14);			//先发包头
		SchedulScrn_SendStr(data,datalen);			//再发内容
		SchedulScrn_SendStr(pbuf_Tail,3);			//最后发包尾
	OSSemPost(Schedul_UsartSendSem);
	return 0;
}

/*********************************************************
*Function:       SchedulScrn_CallforcarInfoSend()
*Description:    设备下发电召抢单
*Calls:
*Called By:
*Input:            无
*Output:
*Return:
*Others:
*********************************************************/
u8 SchedulScrn_CallforcarInfoSend(u8* ID,u8 idlen,u8* Tel,u8 Tellen, u8* data,u16 datalen)
{
	u8 err=1;
	u8 pbuf_head[30]={0};
	u8 pbuf_Tail[3]={0};
	u8 *pStr=pbuf_head;
	u8 Len=0;

	if(Tellen>15 || idlen!=10)
		return 1;
	*pStr++=0x0F;
	*pStr++=CALL_CAR_INFO_CMD;
	Len=(11+Tellen+datalen);
	*pStr++=(Len>>4)>9?((Len>>4)+0x37):((Len>>4)+'0');
	*pStr++=(Len&0x0F)>9?((Len&0x0F)+0x37):((Len&0x0F)+'0');
	Mem_Copy(pStr,ID,idlen); //不足10位？？？？
	pStr+=idlen;
	*pStr++=Tellen;
	Mem_Copy(pStr,Tel,Tellen);
	pStr+=Tellen;

	pbuf_Tail[0]=Protocol_Check_2para(pbuf_head+1,14+Tellen,data,datalen);  //校验码生成  分别从包头  和内容进行校验
	pbuf_Tail[1]=0xF0;
	pbuf_Tail[2]=0xFF;

	SSCRN_Print("向调度屏发送详细订单\r\n");
	for(Len=0;Len<(15+Tellen);Len++)
		SSCRN_Print("0x%02x ",pbuf_head[Len]);
	SSCRN_Print("\r\n");
	OSSemPend(Schedul_UsartSendSem,0,&err);
		SchedulScrn_SendStr(pbuf_head,15+Tellen);			//先发包头
		SchedulScrn_SendStr(data,datalen);			//再发内容
		SchedulScrn_SendStr(pbuf_Tail,3);			//最后发包尾
	OSSemPost(Schedul_UsartSendSem);
	return 0;


//	u8 temp=1;
//	u8 *pStr=Sendbuf;
//	u8 Len=0;
//	*pStr++=0x0F;
//	*pStr++=CALL_CAR_INFO_CMD;
//	Len=(11+Tellen+datalen);
//	*pStr++=(Len>>4)>9?((Len>>4)+0x37):((Len>>4)+'0');
//	*pStr++=(Len&0x0F)>9?((Len&0x0F)+0x37):((Len&0x0F)+'0');
//	Mem_Copy(pStr,ID,idlen); //不足10位？？？？
//	pStr+=idlen;
//	*pStr++=Tellen;
//	Mem_Copy(pStr,Tel,Tellen);
//	pStr+=Tellen;
//	Mem_Copy(pStr,data,datalen);
//	pStr+=datalen;
//	BSP_XOR_Check(Sendbuf+1,4+idlen+Tellen+datalen,pStr);
//	pStr++;
//	*pStr++=0xF0;
//	*pStr++=0xFF;
//	SSCRN_Print("向调度屏发送抢答订单\r\n");
//	for(Len=0;Len<(8+idlen+Tellen+datalen);Len++)
//		SSCRN_Print("0x%02x ",*(Sendbuf+Len));
//	SSCRN_Print("\r\n");
//
//	OSSemPend(Schedul_UsartSendSem,0,&temp);
//	SchedulScrn_SendStr(Sendbuf,8+idlen+Tellen+datalen);
//	OSSemPost(Schedul_UsartSendSem);
//	return 0;
}

/*********************************************************
*Function:       SchedulScrn_CallforcarCancelSend()
*Description:    设备下发取消已抢到的电召
*Calls:
*Called By:
*Input:            无
*Output:
*Return:
*Others:
*********************************************************/
u8 SchedulScrn_CallforcarCancelSend(u8 *Sendbuf,u16 Sendbuflen,u8* ID,u8 idlen)
{
	//内容未定 -------------------
//	u8 temp=1;
//	u8 *pStr=Sendbuf;
//	u8 Len=0;
//	*pStr++=0x0F;
//	*pStr++=;
//	Len=(11+Tellen+datalen);
//	*pStr++=(Len>>4)>9?((Len>>4)+0x37):((Len>>4)+'0');
//	*pStr++=(Len&0x0F)>9?((Len&0x0F)+0x37):((Len&0x0F)+'0');
//	Mem_Copy(pStr,ID,idlen); //不足10位？？？？
//	pStr+=idlen;
//	*pStr++=Tellen;
//	Mem_Copy(pStr,Tel,Tellen);
//	pStr+=Tellen;
//	Mem_Copy(pStr,data,datalen);
//	pStr+=datalen;
//	BSP_XOR_Check(Sendbuf+1,4+idlen+Tellen+datalen,pStr);
//	pStr++;
//	*pStr++=0xF0;
//	*pStr++=0xFF;
//	SSCRN_Print("向调度屏发送抢答订单\r\n");
//	for(Len=0;Len<(8+idlen+Tellen+datalen);Len++)
//		SSCRN_Print("0x%02x ",*(Sendbuf+Len));
//	SSCRN_Print("\r\n");
//
//	OSSemPend(Schedul_UsartSendSem,0,&temp);
//	SchedulScrn_SendStr(Sendbuf,8+idlen+Tellen+datalen);
//	OSSemPost(Schedul_UsartSendSem);
	return 0;
}
/*********************************************************
*Function:       SchedulScrn_CallforcarInfoSend()
*Description:    设备下发短信到屏，包括抢答信息
*Calls:
*Called By:
*Input: Tel 手机号 抢答信息时为？？？？           
		Time BCD格式的时间YYMMDDHHMMSS
		cmd 	   'C'中文, 'E'英文，'G'国标
*Output:
*Return:
*Others:
*********************************************************/
u8 SScrn_SMS_Send(u8 cmd,u8 *Tel,u8 Tellen,u8 *Time,u8 *SMS,u8 smslen)
{
	u8 err=1;
	u8 pbuf_head[40]={0};
	u8 pbuf_Tail[3]={0};
	u8 *pStr=pbuf_head;
	u8 Len=0;

	if((Tellen>20)
	||(cmd!='C'&&cmd!='E'&&cmd!='G'))
		return 1;
	*pStr++=0x0F;
	*pStr++=TEXT_INFO_CMD;
	Len=(21+13+1+smslen);
	*pStr++=(Len>>4)>9?((Len>>4)+0x37):((Len>>4)+'0');
	*pStr++=(Len&0x0F)>9?((Len&0x0F)+0x37):((Len&0x0F)+'0');
	*pStr++='N'; //21B
	Mem_Copy(pStr,Tel,Tellen);
	if(Tellen<20)
		Mem_Set(pStr+Tellen,0xFF,20-Tellen);
	pStr+=20;
	*pStr++='T';   //13B
	*pStr++=((Time[0]&0x0F) +'0');
	*pStr++=((Time[0]>>4) +'0');
	*pStr++=((Time[1]&0x0F) +'0');
	*pStr++=((Time[1]>>4) +'0');
	*pStr++=((Time[2]&0x0F) +'0');
	*pStr++=((Time[2]>>4) +'0');
	*pStr++=((Time[3]&0x0F) +'0');
	*pStr++=((Time[3]>>4) +'0');
	*pStr++=((Time[4]&0x0F) +'0');
	*pStr++=((Time[4]>>4) +'0');
	*pStr++=((Time[5]&0x0F) +'0');
	*pStr++=((Time[5]>>4) +'0');
	*pStr++=cmd;  //1B

	SSCRN_Print("向调度屏发送短信\r\n");
	pbuf_Tail[0]=Protocol_Check_2para(pbuf_head+1,3+21+13+1,SMS,smslen);  //校验码生成  分别从包头  和内容进行校验
	pbuf_Tail[1]=0xF0;
	pbuf_Tail[2]=0xFF;

	OSSemPend(Schedul_UsartSendSem,0,&err);
		SchedulScrn_SendStr(pbuf_head,39);			//先发包头
		SchedulScrn_SendStr(SMS,smslen);			//再发内容
		SchedulScrn_SendStr(pbuf_Tail,3);			//最后发包尾
	OSSemPost(Schedul_UsartSendSem);
	return 0;
}
//return 0成功  1失败
u8 SScrn_GetResult(u8 *Buf,u8 BufSize,u8 *ReadSize,u8 Cmd,u8 SubField)
{
	u8 datalen=0;
	u8 err;
	u8 result=0;	

	if(CheckParam1(Buf)||(Cmd!=SScrn_Result.CMD_TYPE))
	{
		SSCRN_Print("获取参数错误,错误输入!\r\n");
		return 1;
	}
	OSSemPend(SchedulProtect,0,&err);
	switch(Cmd)
	{
		case N990_ONEBYTE_CMD:
			datalen=1;
			if(datalen>BufSize)
			{
				result=1;
			}
			else
			{
				*Buf=SScrn_Result.result.SUB_CMD_TYPE;
				*ReadSize=datalen;
			}
		break;
		case N990_SENDSMS_CMD:
			switch(SubField)
			{
				case TEL_NUM_ENUM:
					datalen=strlen(SScrn_Result.result.SMS_Data.TelNum);
					if(datalen>BufSize)
					{
						result=1;
					}
					else
					{
						memcpy(Buf,SScrn_Result.result.SMS_Data.TelNum,datalen);
						*ReadSize=datalen;
					}
				break;
				case SMS_DAT_ENUM:
					datalen=SScrn_Result.result.SMS_Data.SMSLen;
					if(datalen>BufSize)
					{
						result=1;
					}
					else
					{
						memcpy(Buf,SScrn_Result.result.SMS_Data.SMSDat,datalen);
						*ReadSize=datalen;
					}
				break;
				default:
					result=1;
				break;
			}
		break;
		case CALL_CAR_RSP_CMD: 
		case PICKUP_PASSEGER_CMD: 
			*ReadSize=SScrn_Result.result.IDLen;
			Mem_Copy(Buf,SScrn_Result.result.ID,SScrn_Result.result.IDLen);
		break;
		case PICKUP_PASSEGER_FAILED_CMD:
			if(0==SubField)
			{ 
				*ReadSize=SScrn_Result.result.IDLen;
				Mem_Copy(Buf,SScrn_Result.result.ID,SScrn_Result.result.IDLen);
			}
			else
			{
				*ReadSize=SScrn_Result.result.failmsgLen;
				Mem_Copy(Buf,SScrn_Result.result.FailedMsg,SScrn_Result.result.failmsgLen);
			}
		break;
		default:
		break;
	}
	OSSemPost(SchedulProtect);
	return result;
}
#endif
static u8 SScrn_UpResult(u8 *Str,u8 Cmd)
{
	u8 err=1;
	u8 U8Temp=0;
	u8 *pStr=Str;
	u16 datalen=0;

	if(CheckParam1(Str))
	{
		SSCRN_Print("获取参数错误,错误输入!\r\n");
		return 0;
	}
	if(*pStr<='9')
		datalen=*pStr-'0';
	else if(*pStr<='F')
		datalen=*pStr-0x37;
	else
		datalen=*pStr-87;
	datalen=(datalen<<4);
	pStr++;
	if(*pStr<='9')
		datalen+=*pStr-'0';
	else if(*pStr<='F')
		datalen+=*pStr-0x37;
	else
		datalen+=*pStr-87;
	pStr++;
//	SSCRN_Print("收到调度屏消息,数据长度:%d\r\n",datalen);
	OSSemPend(SchedulProtect,0,&err);
	switch(Cmd)
	{
	case N990_ONEBYTE_CMD:
		SScrn_Result.result.SUB_CMD_TYPE=*pStr;
	break;
	case N990_SENDSMS_CMD:
		if('N'!=*pStr ||'C'!=*(pStr+21))
			Mem_Set(&SScrn_Result.result.SMS_Data,0,sizeof(SScrn_Result.result.SMS_Data));
		else
		{
			pStr++;
			for(U8Temp=0;U8Temp<20;U8Temp++)
			{	if(*(pStr+U8Temp)==0xff)
				{	SScrn_Result.result.SMS_Data.TelNum[U8Temp]='\0';
					break;
				}
				else
					SScrn_Result.result.SMS_Data.TelNum[U8Temp]=*(pStr+U8Temp);
			}
			pStr+=20;pStr++;
			SScrn_Result.result.SMS_Data.SMSLen=datalen-22;
			Mem_Copy(&SScrn_Result.result.SMS_Data.SMSDat,pStr,datalen-22);
		}
	break;
	case CALL_CAR_RSP_CMD: 
	case PICKUP_PASSEGER_CMD: 
		SScrn_Result.result.IDLen=datalen;
		Mem_Copy(SScrn_Result.result.ID,pStr,datalen);
	break;
	case PICKUP_PASSEGER_FAILED_CMD: 
		SScrn_Result.result.IDLen=10;
		Mem_Copy(SScrn_Result.result.ID,pStr,10);
		pStr+=10;
		SScrn_Result.result.failmsgLen=datalen-10;
		Mem_Copy(SScrn_Result.result.FailedMsg,pStr,SScrn_Result.result.failmsgLen);
	break;
	default:
	break;
	}
	OSSemPost(SchedulProtect);

}
#elif 1//SCREEN_HD5>0 //D5调度屏
#if 0
/*********************************************************
*Function:       SchedulScrn_RcvPhase()
*Description:    数据接收状态机
*Calls:
*Called By:
*Input:            无
*Output:
*Return:
*Others:
*********************************************************/
static void SchedulScrn_RcvPhase(u8 Com_Data)
{
	static u8 Rx_Couter=0; 
	static u8 Content_Length=0;
	static u16 BufCnt=0;
	static u8 Check_Sum=0;
	static u8 Step	 =0;
	static u8 ack_flag;
	switch (Step)
	{
		case 0:
			if(Com_Data==0x24) //头
			{
				Check_Sum=0x24;
				IntRecvBuf[BufCnt++]=Com_Data;
//				QueueWrite(Com_Data,&SchedulRecvQue);
			    Step++;
			}	
			else
			{
				Check_Sum=0;
				Step=0;
				BufCnt=0;
			}
		break;
		case 1:
			Check_Sum^=Com_Data;	
//			QueueWrite(Com_Data,&SchedulRecvQue);
			IntRecvBuf[BufCnt++]=Com_Data;
			Step++;
			if (Com_Data==ACK_CMD||Com_Data==0x0a)
			{
				ack_flag=1;
			}
			else
			{
				ack_flag=0;
			}
		break;
		case 2:
			Content_Length=Com_Data;
			Check_Sum^=Com_Data;
//			QueueWrite(Com_Data,&SchedulRecvQue);
			IntRecvBuf[BufCnt++]=Com_Data;
			Rx_Couter=0;
			if(Content_Length>1)
			{
			 	Step++;
			}
			else
			{
				Step=0;
				BufCnt=0;
//				QueueDelData(&SchedulRecvQue,MSG_Len);
			}
		break;
		case 3:
			if(Rx_Couter<Content_Length-2)
			{
				Check_Sum^=Com_Data;
//				QueueWrite(Com_Data,&SchedulRecvQue);
				IntRecvBuf[BufCnt++]=Com_Data;
				Rx_Couter++;
			}
			else
			{
				if(Check_Sum!=Com_Data)
				{
					Step=0;
					BufCnt=0;
//					QueueDelData(&SchedulRecvQue,MSG_Len);
				}
				else
				{
//					QueueWrite(Com_Data,&SchedulRecvQue);
					IntRecvBuf[BufCnt++]=Com_Data;
					Step++;
				}
			}
		break;
		case 4:
			if(Com_Data==0xFF)
			{
				Step=0;
//				QueueWrite(Com_Data,&SchedulRecvQue);
				IntRecvBuf[BufCnt++]=Com_Data;
				if (ack_flag)
				{
					OSSemPost(SchedulAckSem);
//					QueueDelData(&SchedulRecvQue,MSG_Len);
				}
				else
				{
					if(OSMboxAccept(SchedulRcvSem)==0)
					{
						QueueFlush(&SchedulRecvQue);
					}
					QueueBufWrite(IntRecvBuf,BufCnt,&SchedulRecvQue,Normal_Write);
					
					OSMboxPost(SchedulRcvSem,(void *)BufCnt);
					
//					Ack_Package(Buf,Normal_Ack,ACK_CMD,0);//在中断中及时回复应答
				}
				BufCnt=0;
			}
			else
			{
				Step=0;
				BufCnt=0;
			}
		break;
		default:
			Step=0;
			BufCnt=0;
		break;
	}
}
#endif
///*********************************************************
//*Function:       SScrn_Init()
//*Description:    初始化调度屏的通信口和处理中要用到的相关变量
//*                
//*Calls:          
//*Called By:      
//*Input:          
//*Output:
//*Return:         收到的指令类别枚举
//*Others:
//*********************************************************/
//u8 SScrn_Init (void) 
//{
//	SScrn_HW_Init();
//	SScrn_SW_Init();
//	SSCRN_Print("调度屏初始化完成\r\n");
//	return 0;
//}
/*********************************************************
*Function:       SScrn_DataAnaly()
*Description:    对RecvBuf中数据解析，根据内容进行参数设置与
*                处理
*Calls:          
*Called By:      
*Input:          
*Output:
*Return:         收到的指令类别枚举
*Others:
*********************************************************/
u8 SScrn_DataAnaly(void) 
{
	u8 err;
	u8 RcvBuf[256] = {0x0};//接收缓存
	u8 i=0;
	u8 MsgLen=0;
//	u8 datalen=0;
	u8 *pStr;
//	u16 *Flag=NULL;
	u8 Cmd=0;
//	static u8 Swtchflag=0;
#if 0			
	MsgLen=(u8)(OSMboxPend(SchedulRcvSem,WAIT_TIME_OUT,&err));
	if(err==OS_ERR_NONE)
	{
		OSMboxPost(SchedulRcvSem,(void *)1);
	}
	else if (err==OS_ERR_TIMEOUT)
	{
		return SCCRN_TIME_OUT;
	}
	else
	{
		return SCCRN_ERROR_RETURN;
	}
	pStr=RcvBuf;
	for (i=0;i<MsgLen;i++)
	{
		QueueRead(pStr++,&SchedulRecvQue);
	}
	OSMboxPend(SchedulRcvSem,WAIT_TIME_OUT,&err);
#endif
	oa_memcpy(RcvBuf, uart_contain.buf, uart_contain.len);
	MsgLen = uart_contain.len;
	//DEBUG("收到调度屏消息,消息长度:%d",MsgLen);
	pStr=RcvBuf;
	if(*pStr++==0x24)
	{
		Cmd=*pStr;
		switch(*pStr++)
		{
			case CENTER_SMS_CMD: //与短信任务参数查询与设置相同
			case UNICODE_SMS_CMD://发送短信
			case SET_UDP_CMD: //监控平台IP及UDP设定
			case SET_TCP_CMD: //监控平台IP及TCP设定
			case SET_LOCAL_TEL_CMD://设置本地号码
			case CALL_DIALUP_CMD://电话拨出
			case SET_SPK_CMD://设置通话音量
			case SET_MIC_CMD://调节MIC音量
			#if DRIVE_SIGN_EN
			case DRIVER_SIGN_CMD://司机登录
			#endif
			#if CUSTOM_CMD
			case COM_JTT808_CMD:   //jtt808消息包
			#endif
			case UPDATA_PARA_CMD:
			case UPDATA_DATA_CMD:
			case STATION_DOWNLOAD_CMD:
			case SCR_SMS_DOWNLOAD_CMD:
				SScrn_Ack_Send();/*应答*/
				SScrn_UpResult(pStr,Cmd);//copy data to 'SScrn_Result'
				SScrn_Result.CMD_TYPE=Cmd;
			break;
			case CALL_HANGOFF_CMD: //无有效数据内容
				SScrn_Ack_Send();/*应答*/
				SSCRN_Print("调度屏设定挂机\r\n");
				SScrn_Result.CMD_TYPE=CALL_HANGOFF_CMD;	
			break;
			case CALL_HANGON_CMD://接通电话
				SScrn_Ack_Send();/*应答*/
				SSCRN_Print("调度屏设定接通电话\r\n");
				SScrn_Result.CMD_TYPE=CALL_HANGON_CMD;	
			break;
			case UPDATA_REQ_CMD://请求升级
				SScrn_Ack_Send();/*应答*/
				SSCRN_Print("PC请求升级\r\n");
				SScrn_Result.CMD_TYPE=UPDATA_REQ_CMD;	
			break;
			case CALL_SPKSWITCH_CMD://切换语音通道
				SScrn_Ack_Send();/*应答*/
				SSCRN_Print("调度屏设定切换语音通道\r\n");
				SScrn_Result.CMD_TYPE=CALL_SPKSWITCH_CMD;
			break;
			case STATION_DOWNLOAD_START_CMD://
				SScrn_Ack_Send();/*应答*/
				SSCRN_Print("串口导入线路开始......\r\n");
				SScrn_Result.CMD_TYPE=STATION_DOWNLOAD_START_CMD;
			break;
			case ACK_CMD:
				//DEBUG("收到调度屏应答");
				return 	SCCRN_UNEXP_CMD;
			default:
			//	SScrn_Result.CMD_TYPE=SCCRN_UNEXP_CMD;
				//DEBUG("(%s %s %d):调度屏收到未定义消息", __FILE__, __func__, __LINE__);
				//DEBUG("(%s %s %d):指令码为:%02x",__FILE__, __func__, __LINE__, Cmd);
				return 	SCCRN_UNEXP_CMD;
//			break;
		}
	}														 
	return SScrn_Result.CMD_TYPE;
}

/*********************************************************
*Function:      SScrn_GetResult(u8 *Buf,u8 BufSize,u8 *ReadSize,u8 Cmd,u8 SubField)
*Description:   提取从调度屏发来的数据值

*Calls:          
*Called By:       
*Input:         Cmd:对应调度屏处理命令
				Field:读取数据类别
*Output:        Str:数据写入指针
*Return:        0:提取成功 1:提取失败
*Others:
*********************************************************/
u8 SScrn_GetResult(u8 *Buf,u8 BufSize,u8 *ReadSize,u8 Cmd,u8 SubField)
{
	u8 datalen=0;
	u8 err;
	u8 result=0;	

	if(CheckParam1(Buf)||(Cmd!=SScrn_Result.CMD_TYPE))
	{
		SSCRN_Print("获取参数错误,错误输入!\r\n");
		return 1;
	}
//	OSSemPend(SchedulProtect,0,&err);
	switch(Cmd)
	{
		case CENTER_SMS_CMD:
			datalen=SScrn_Result.result.SMS_Data.SMSLen;//strlen(SScrn_Result.result.SMS_Data.SMSDat);
			if(datalen>BufSize)
			{
				result=1;
			}
			else
			{
				memcpy(Buf,SScrn_Result.result.SMS_Data.SMSDat,datalen);
				*ReadSize=datalen;
			}
		break;
		case UNICODE_SMS_CMD:
			switch(SubField)
			{
				case TEL_NUM_ENUM:
					datalen=strlen(SScrn_Result.result.SMS_Data.TelNum);
					if(datalen>BufSize)
					{
						result=1;
					}
					else
					{
						memcpy(Buf,SScrn_Result.result.SMS_Data.TelNum,datalen);
						*ReadSize=datalen;
					}
				break;
				case SMS_DAT_ENUM:
					datalen=SScrn_Result.result.SMS_Data.SMSLen;
					if(datalen>BufSize)
					{
						result=1;
					}
					else
					{
						memcpy(Buf,SScrn_Result.result.SMS_Data.SMSDat,datalen);
						*ReadSize=datalen;
					}
				break;
				default:
				break;
			}
			
		break;
		case SET_UDP_CMD:
		case SET_TCP_CMD:
			switch(SubField)
			{
				case IP_ENUM:
					datalen=16;
					if(datalen>BufSize)
					{
						result=1;
					}
					else
					{
						memcpy(Buf,SScrn_Result.result.IP_Port.Ip,15);
						Buf[15]='\0';
						*ReadSize=datalen-1;
					}	
				break;
				case PORT_ENUM:
					datalen=2;
					if(datalen>BufSize)
					{
						result=1;
					}
					else
					{
						*(u16*)Buf =	SScrn_Result.result.IP_Port.port;
						*ReadSize=datalen;
					}
				break;
				default:
				break;
			}
		break;
		case SET_LOCAL_TEL_CMD:
		case CALL_DIALUP_CMD:	
			datalen=strlen(SScrn_Result.result.telnum);
			if(datalen>BufSize)
			{
				result=1;
			}
			else
			{
				memcpy(Buf,SScrn_Result.result.telnum,datalen);
				*ReadSize=datalen;
			}
		break;
		case SET_SPK_CMD:	
			datalen=1;
			if(datalen>BufSize)
			{
				result=1;
			}
			else
			{
				*Buf=SScrn_Result.result.value;
				*ReadSize=datalen;
			}
		break;
		case SET_MIC_CMD:
			datalen=1;
			if(datalen>BufSize)
			{
				result=1;
			}
			else
			{
				*Buf=SScrn_Result.result.value;
				*ReadSize=datalen;
			}
		break;
		#if DRIVE_SIGN_EN
		case DRIVER_SIGN_CMD:
			switch(SubField)
			{
				case SIGN_FLAG_ENUM:
					datalen=1;
					if(datalen>BufSize)
					{
						result=1;
					}
					else
					{
						*Buf=SScrn_Result.result.driver_inf.SignFlag;
						*ReadSize=datalen;
					}	
				break;
				case DRIVER_ID_ENUM:
					datalen=2;
					if(datalen>BufSize)
					{
						result=1;
					}
					else
					{
						memcpy(Buf,&SScrn_Result.result.driver_inf.DriverID,2);
						*ReadSize=datalen;
					}		
				break;
				default:
				break; 
			}
			break;
		#endif 
		#if CUSTOM_CMD
		case COM_JTT808_CMD:
			datalen=SScrn_Result.result.JTT_MSG.datalen;
			if(datalen>BufSize)
			{
				result=1;
			}
			else
			{
				memcpy(Buf,SScrn_Result.result.JTT_MSG.databuf,datalen);
				*ReadSize=datalen;
			}
		#endif	
		break;

		case UPDATA_PARA_CMD:
			switch(SubField)
			{
				case PACKETS_ENUM:
					datalen=4;
					if(datalen>BufSize)
					{
						result=1;
					}
					else
					{
						memcpy(Buf,(u8 *)&SScrn_Result.result.UP_DATA_PARA.PacketTotalNum,4);
						*ReadSize=datalen;
					}	
				break;
				case DATA_SIZE_ENUM:
					datalen=4;
					if(datalen>BufSize)
					{
						result=1;
					}
					else
					{
						memcpy(Buf,(u8 *)&SScrn_Result.result.UP_DATA_PARA.DataSize,4);
						*ReadSize=datalen;
					}		
				break;
				default:
					result=1;	
				break; 
			}		
		break;
		case UPDATA_DATA_CMD:
			datalen=SScrn_Result.result.UP_DATA_STORE.datalen;
			if(datalen>BufSize)
			{
				result=1;
			}
			else
			{
				memcpy(Buf,SScrn_Result.result.UP_DATA_STORE.UP_DATA,datalen);
				*ReadSize=datalen;
			}
		break;
		case STATION_DOWNLOAD_CMD:
			datalen=sizeof(STRUCT_STATION_DATA);
			if(BufSize>=datalen)
			{
				memcpy(Buf,&SScrn_Result.result.STATION_DATA,datalen);
				*ReadSize=datalen;
			}
			else
				result=1;
		break;
		case SCR_SMS_DOWNLOAD_CMD:
			datalen=sizeof(STRUCT_SCR_SMS_TEMPLET);
			if(BufSize>=datalen)
			{
				memcpy(Buf,&SScrn_Result.result.SCR_SMS_TEMPLET,datalen);
				*ReadSize=datalen;
			}
			else
				result=1;
		break;
		default: 
			result=1;
		break;
	}
//	OSSemPost(SchedulProtect);
	return result;
}

/*********************************************************
*Function:       SScrn_UpResult(u8 *Str,u8 Cmd)
*Description:    保存从调度屏发来的数据值

*Calls:          
*Called By:       
*Input:         Cmd:对应调度屏的指令码
*Output:        Str:数据写入指针
*Return:        返回数据长度,当为0时候,表示无数据
*Others:
*********************************************************/
static u8 SScrn_UpResult(u8 *Str,u8 Cmd)
{
	u8 datalen=0;
	u8 *pStr=Str;
	u8 err;
	u8 i;

	if(CheckParam1(Str))
	{
		SSCRN_Print("获取参数错误,错误输入!\r\n");
		return 0;
	}
//	OSSemPend(SchedulProtect,0,&err);
	switch(Cmd)
	{
		case CENTER_SMS_CMD:
			datalen=*pStr++-2;
			Mem_Copy(SScrn_Result.result.SMS_Data.SMSDat,pStr,datalen);  //将中心短信信息保存
			SScrn_Result.result.SMS_Data.SMSLen=datalen;
			SScrn_Result.result.SMS_Data.SMSDat[datalen]='\0';
			SSCRN_Print("中心短信内容:%s",SScrn_Result.result.SMS_Data.SMSDat);
			SScrn_Result.CMD_TYPE=CENTER_SMS_CMD;
		break;
		case UNICODE_SMS_CMD:
		#if (HAND_DEVICE_OR_SCRN==0)  //使用手柄
			pStr+=7;							
			datalen=*pStr-2;
			pStr+=3;
			i=1;
			while(i<=(datalen/2)*2)//取短信号码
			{
				SScrn_Result.result.SMS_Data.TelNum[i-1]= (*pStr&0x0f)+'0';
				SScrn_Result.result.SMS_Data.TelNum[i]=((*pStr++&0xf0)>>4)+'0';
				i+=2;
			}
			if (i<datalen+1)
			{
				SScrn_Result.result.SMS_Data.TelNum[i-1]= (*pStr++&0x0f)+'0';
			}
			SScrn_Result.result.SMS_Data.TelNum[datalen]='\0';
			SSCRN_Print("UNICODE短信号码:%s\r\n",SScrn_Result.result.SMS_Data.TelNum);
			pStr+=3;
			datalen=*pStr;	 //获取短信长度
			pStr++;
//			datalen=(datalen<<4)+ *pStr-0x30;
//			pStr++;
			i=0;
			while(i<datalen)//拷贝短息内容
			{
				SScrn_Result.result.SMS_Data.SMSDat[i++]=*pStr++;
			}
			//SScrn_Result.result.SMS_Data.SMSDat[i]='\0';																		
			SScrn_Result.result.SMS_Data.SMSLen=datalen;	
			//SSCRN_Print("UNICODE短信内容:%s\r\n",SScrn_Result.result.SMS_Data.SMSDat);
			SScrn_Result.CMD_TYPE=Cmd;	
		#elif (HAND_DEVICE_OR_SCRN==1)	//使用调度屏
		{
			u8 total_len=(*pStr++)-3;//记录总长度=号码长度+短信长度,计算用 
			datalen=*pStr++;//号码长度
			for(i=0;i<datalen;i++)//取短信号码
			{
				SScrn_Result.result.SMS_Data.TelNum[i]= (*pStr++);
			}
			SScrn_Result.result.SMS_Data.TelNum[datalen]='\0';
			SSCRN_Print("UNICODE短信号码:%s\r\n",SScrn_Result.result.SMS_Data.TelNum);
			
			datalen=total_len-datalen;
			i=0;
			for(i=0;i<datalen;i++)
			{
				SScrn_Result.result.SMS_Data.SMSDat[i]=*pStr++;
			}
			SScrn_Result.result.SMS_Data.SMSLen=datalen;	
			//SSCRN_Print("UNICODE短信内容:%s\r\n",SScrn_Result.result.SMS_Data.SMSDat);
			SScrn_Result.CMD_TYPE=Cmd;
		}
		#endif
		break;
		case SET_UDP_CMD:
		case SET_TCP_CMD:
			{
				u8 temp=0;
				i=0;
				pStr+=5;
				i=0;
				err=0;//错误标志
				while(i<15)//取IP,检测合法性
				{
					if(*(pStr+i)=='.')temp++;//计数字符'.' 
					else if(*(pStr+i)>='0'&&*(pStr+i)<='9')//检测值是否正确
					{}
					else
					{
						err=1;
						break;
					}

					if(temp>3)
					{
						err=1;
						break;
					}
					i++;
				}
				if(err==1)
				{
					SSCRN_Print("调度屏设置IP出错,参数错误\r\n");
					SScrn_Result.CMD_TYPE=PARA_ERROR_CMD;//参数错误
					SScrn_Result_Send(1);
				}
				else//检测无误,取出并保存
				{
					u8 ip_temp[4]={0};
					u32 ip_port_temp;
					memset(ip_temp,0,4);
					ip_temp[3]=0;
					err=0;
					memset(SScrn_Result.result.IP_Port.Ip,0,sizeof(SScrn_Result.result.IP_Port.Ip));
					for(i=0;i<4;i++)//保存ip值
					{
						strncpy(ip_temp,pStr+4*i,3);
						ip_port_temp=atoi(ip_temp);
						if(ip_port_temp<256)
						{
							Str_Cat(SScrn_Result.result.IP_Port.Ip,ip_temp);
							if(i<3)
							{
								Str_Cat(SScrn_Result.result.IP_Port.Ip,".");
							}
						}
						else
						{
							SScrn_Result.CMD_TYPE=PARA_ERROR_CMD;
							SScrn_Result_Send(1);
							SSCRN_Print("调度屏设置IP出错,参数错误\r\n");
							err=1;
							break;
						}
					}
					if(err==0)
					{
						SScrn_Result.result.IP_Port.Ip[15]='\0';
						pStr+=17;				   
						ip_port_temp=0;
						do	//取PORT
						{
							if(*pStr>='0'&&*pStr<='9')
								ip_port_temp=ip_port_temp*10+*pStr-'0';
							else
							{
								err=1;
								break;
							}
							pStr++;
						}while(*pStr!='>');
						if(err==1 ||ip_port_temp==0||ip_port_temp>65535) 
						{
							SScrn_Result.CMD_TYPE=PARA_ERROR_CMD;
							SScrn_Result_Send(1);
							SSCRN_Print("调度屏设置PORT出错,参数错误\r\n");
						}
						else
						{
							SScrn_Result.result.IP_Port.port=ip_port_temp;
							SScrn_Result.CMD_TYPE=Cmd;
							SSCRN_Print("调度屏设置IP正确%s\r\n",
										SScrn_Result.result.IP_Port.Ip
										);
							SSCRN_Print("调度屏设置PORT正确:%d\r\n",ip_port_temp);
						}
					}	
				}
			}
		break;
		case SET_LOCAL_TEL_CMD:
			datalen=*pStr-2-4; //取sim卡号长度
			pStr+=5;
			i=0;
			while(i<datalen)
			{
				SScrn_Result.result.telnum[i++]=*pStr++;
			}
			SScrn_Result.result.telnum[datalen]='\0';
			SScrn_Result.CMD_TYPE=SET_LOCAL_TEL_CMD;	
			SSCRN_Print("调度屏设置本地号码正确:%s\r\n",SScrn_Result.result.telnum);
		break;
		case CALL_DIALUP_CMD:
			datalen=*pStr++;
			i=0;
			while(i<(datalen-2))
			{
				SScrn_Result.result.telnum[i++]=*pStr++;
			}
			SScrn_Result.result.telnum[datalen-2]='\0';
			SScrn_Result.CMD_TYPE=CALL_DIALUP_CMD;	
			SSCRN_Print("调度屏拨打号码:%s\r\n",SScrn_Result.result.telnum);
		break;
		case SET_SPK_CMD:
			pStr++;
			if(*pStr=='1')//1为静音
			{
				SScrn_Result.result.value=0;
			}
			else
			{
				SScrn_Result.result.value=*pStr-'0';
			}
			SScrn_Result.CMD_TYPE=SET_SPK_CMD;
			SSCRN_Print("调度屏设定扬声器音量:%d\r\n",SScrn_Result.result.value);
		break;
		case SET_MIC_CMD:	
			pStr++;
			if(*pStr=='1' || *pStr=='2')
			{
				SScrn_Result.result.value=1;
			}
			else if(*pStr=='3' || *pStr=='4')
			{
				SScrn_Result.result.value=2;
			}
			else if(*pStr=='5' || *pStr=='6')
			{
				SScrn_Result.result.value=3;
			}
			else
			SScrn_Result.result.value=4;
			SScrn_Result.CMD_TYPE=SET_MIC_CMD;
			SSCRN_Print("调度屏设定麦克风音量:%d\r\n",SScrn_Result.result.value);
		break;
		#if DRIVE_SIGN_EN
		case DRIVER_SIGN_CMD:
			pStr++;
			if(*pStr==0x00 ||*pStr==0x01)
			{
				SScrn_Result.result.driver_inf.SignFlag= (DRIVER_SIGN_ENUM)(*pStr++);
				SScrn_Result.result.driver_inf.DriverID=*pStr<<8;
				pStr++;
				SScrn_Result.result.driver_inf.DriverID+=*pStr;
				SScrn_Result.CMD_TYPE=DRIVER_SIGN_CMD;
				SSCRN_Print("司机登录信息:%d 工号:%d\r\n",
							SScrn_Result.result.driver_inf.SignFlag,
							SScrn_Result.result.driver_inf.DriverID);
			}
		break;
		#endif
		#if CUSTOM_CMD
		case COM_JTT808_CMD:   //jtt808消息包
			datalen=(*pStr++)-2;
			SScrn_Result.result.JTT_MSG.datalen=datalen	;
			Mem_Copy(SScrn_Result.result.JTT_MSG.databuf,pStr,datalen);
			SScrn_Result.CMD_TYPE=COM_JTT808_CMD;
			SSCRN_Print("调度屏收到JTT808协议消息包:\r\n");
		break;
		#endif
//		case COM_UPLOAD_REQACK_CMD://请求升级应答 自定义指令
//				if(*pStr==0x05)
//				{
//					pStr++;
//					if(*pStr==0x00) //可以升级
//					{
//						pStr++;
//						SScrn_Result.result.value16=*pStr<<8;
//						pStr++;
//						SScrn_Result.result.value16+=*pStr;			
//					}
//					else if(*pStr==0x01) //已是最新版本
//					{
//						SScrn_Result.result.value16=0;
//						SScrn_Result.CMD_TYPE=COM_UPLOAD_REQACK_CMD;//不用操作					
//					}
//					else
//					{
//						SScrn_Result.CMD_TYPE=SCCRN_ERROR_RETURN;//ack;
//					}
//				}
//				else
//				{
//					SScrn_Result.CMD_TYPE=SCCRN_ERROR_RETURN;	//包错误
//				}
//		break;
//		case COM_UPLOAD_DATAACK_CMD:
//				 if(*pStr>0x07)
//				 {
//					pStr++;
//					SScrn_Result.result.value16=*pStr<<8;
//					pStr++;
//					SScrn_Result.result.value16+=*pStr++;
//					SScrn_Result.CMD_TYPE=COM_UPLOAD_DATAACK_CMD;
//				 }
//				 else
//				 {
//				 	SScrn_Result.CMD_TYPE=SCCRN_ERROR_RETURN;//包错误
//				 }
//		break;
		case UPDATA_PARA_CMD:
			datalen=(*pStr++)-2;
			SScrn_Result.result.UP_DATA_PARA.PacketTotalNum=*((u32 *)pStr);
			pStr+=4;
			SScrn_Result.result.UP_DATA_PARA.DataSize=*((u32 *)pStr);
			SScrn_Result.CMD_TYPE=UPDATA_PARA_CMD;
			SSCRN_Print("调度屏收到升级请求:\r\n");			
		break;
		case UPDATA_DATA_CMD:
		    datalen=(*pStr++)-2;
			SScrn_Result.result.UP_DATA_STORE.datalen=datalen;
			Mem_Copy(SScrn_Result.result.UP_DATA_STORE.UP_DATA,pStr,datalen);
			SScrn_Result.CMD_TYPE=UPDATA_DATA_CMD;
			SSCRN_Print("调度屏收到升级数据:\r\n");	
		break;
		case STATION_DOWNLOAD_CMD:
		    datalen=(*pStr++)-2;
				Mem_Copy((u8*)&SScrn_Result.result.STATION_DATA.ROAD_NUM,pStr,4);
				pStr+=4;
				SScrn_Result.result.STATION_DATA.STA_DIRECTION=*pStr++;
				SScrn_Result.result.STATION_DATA.STA_TOTAL_NUM=*pStr++;
				SScrn_Result.result.STATION_DATA.ROAD_VER=*pStr++;
				//if(SScrn_Result.result.STATION_DATA.STA_NUM*22!=(datalen-8)) err
				SScrn_Result.result.STATION_DATA.STA_DATALEN=22*(*pStr++);
				Mem_Copy((u8*)&SScrn_Result.result.STATION_DATA.STA_DATA,pStr,datalen-8);	 
				SScrn_Result.CMD_TYPE=STATION_DOWNLOAD_CMD;
			SSCRN_Print("调度屏收到站点信息:\r\n");	
		break;
		case SCR_SMS_DOWNLOAD_CMD:
			datalen=(*pStr++)-2;
				SScrn_Result.result.SCR_SMS_TEMPLET.STATE=*pStr++;
				SScrn_Result.result.SCR_SMS_TEMPLET.F_OR_A=*pStr++;
				SScrn_Result.result.SCR_SMS_TEMPLET.SMS_LEN=*pStr++;
				if(SScrn_Result.result.SCR_SMS_TEMPLET.SMS_LEN ==(datalen-3))
				{
					Mem_Copy((u8*)&SScrn_Result.result.SCR_SMS_TEMPLET.SMS_DATA,pStr,SScrn_Result.result.SCR_SMS_TEMPLET.SMS_LEN);
				}
				else
				{
					SScrn_Result.result.SCR_SMS_TEMPLET.SMS_LEN = 0;
				}
				SScrn_Result.CMD_TYPE=SCR_SMS_DOWNLOAD_CMD;
			SSCRN_Print("调度屏收到短信模板信息:\r\n");
		break;
		default: 
		break;
	}
//	OSSemPost(SchedulProtect);
	return 0;
}
/*********************************************************
*Function:       SScrn_UniSMS_Send(u8 *Tel,u8 Telen,u8 *Time,u8* SMSStr,u8 Strlen)
*Description:     向调度屏发送UNICODE编码的短消息

*Calls:          
*Called By:       
*Input:     	  	Tel:电话号码
					Tellen:号码长度
					Time:6字节BCD码时间
					SMSStr:短信内容指针
					SMSlen:短信长度
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:
*********************************************************/
//u8 SScrn_UniSMS_Send(u8 *Tel,u8 Telen,u8 *Time,u8* SMSStr,u8 SMSlen)
//{	
//	if((CheckParam3(Tel,Time,SMSStr)))
//	{
//		SSCRN_Print("向调度屏发送PDU短信出错,错误输入参数\r\n"); 
//		return 1;
//	}
//
//	SSCRN_Print("向调度屏发送PDU短信\r\n");
//	return(SScrn_SMS_Send(TELSMS_PDU_CMD,Tel,Telen,Time,SMSStr,SMSlen));
//}
/*********************************************************
*Function:       SScrn_AscSMS_Send(u8 *Tel,u8 Telen,u8 *Time,u8* SMSStr,u8 SMSlen)
*Description:     向调度屏发送ASCII编码的短消息

*Calls:          
*Called By:       
*Input: 		Tel:电话号码
				Tellen:号码长度
				Time:6字节BCD码时间
				SMSStr:短信内容指针
				SMSlen:短信长度
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:
*********************************************************/
//u8 SScrn_AscSMS_Send(u8 *Tel,u8 Telen,u8 *Time,u8* SMSStr,u8 SMSlen)
//{
//	if((CheckParam3(Tel,Time,SMSStr)))
//	{
//		SSCRN_Print("向调度屏发送TXT短信出错,错误输入参数\r\n"); 
//		return 1;
//	}
//	SSCRN_Print("向调度屏发送TXT短信\r\n");
//	return(SScrn_SMS_Send(TELSMS_TXT_CMD,Tel,Telen,Time,SMSStr,SMSlen));
//}
/*********************************************************
*Function:       SScrn_SMS_Send(u8 cmd,u8 *Tel,u8 Telen,u8 *Time,u8 *SMS,u8 len)
*Description:     向调度屏上发短消息

*Calls:          
*Called By:       
*Input: 		cmd:所用的发送命令  TELSMS_PDU_CMD pdu短信  TELSMS_TXT_CMD: TXT短信		
				Tel:短信号码
				Telen:号码长度
				Time:短信时间
				SMS:短信内容
				Str:发送的内容指针
				Len:发送数据长度
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:
*********************************************************/
u8 SScrn_SMS_Send(u8 cmd,u8 *Tel,u8 Tellen,u8 *Time,u8 *SMS,u8 smslen)
{
//	u8 *Pstr;
	u8 pbuf_head[3];		//3字节数组，存放  包头 指令码 长度	
	u8 TimeStr[20];			//20字节时间  存储字符串
	u8 pbuf_Tail[2];		//2字节数组，存放 校验值 结束符
	u8 err;
	u8 cnt=3;
	u8 result=0;
	u8 Check=0;
	u8 i;
	u16 paklen;

	if((CheckParam3(Tel,Time,SMS)))
	{
		return 1;						 //错误输入,返回发送失败
	}
	
	pbuf_head[0]=0x24;	//包头
	Check=pbuf_head[0];						   //组包
	pbuf_head[1]=cmd;    //指令码
	Check^=pbuf_head[1];
	paklen=(25+Tellen)+smslen+2;  //20=时间字符串长度+所有引号长度=2*7+5+6=25  时间格式  "2012-02-16 09:04:20"
	if (paklen>MAX_MSG_LEN)
	{
		return 1;//超过最大长度,直接返回失败
	}
	pbuf_head[2]=paklen;
	Check^=pbuf_head[2];
	Check^='\"';
	for(i=0;i<Tellen;i++)
	{
		Check^=Tel[i];
	}
	Check^='\"';
	sprintf(TimeStr,"20%02d-%02d-%02d %02d:%02d:%02d",
		   BCDToDec(Time[0]),BCDToDec(Time[1]),BCDToDec(Time[2]),
		   BCDToDec(Time[3]),BCDToDec(Time[4]),BCDToDec(Time[5])
			);
	Check^='\"';
	for(i=0;i<19;i++)
	{
		Check^=TimeStr[i];
	}
	Check^='\"';
	Check^='\"';
	for(i=0;i<smslen;i++)
	{
		Check^=SMS[i];
	}
	Check^='\"';
	pbuf_Tail[0]=Check;  //校验码生成  分别从包头  和内容进行校验
	pbuf_Tail[1]=0x0a;				//结束符 

//	OSSemPend(Schedul_UsartSendSem,0,&err);
	while(cnt--)
	{
		SchedulScrn_SendStr(pbuf_head,3);				//先发包头
		SchedulScrn_SendStr("\"",1);			  		//再发内容
		SchedulScrn_SendStr(Tel,Tellen);			  	//再发号码
		SchedulScrn_SendStr("\"",1);
		SchedulScrn_SendStr("\"",1);
		SchedulScrn_SendStr(TimeStr,19);
		SchedulScrn_SendStr("\"",1);
		SchedulScrn_SendStr("\"",1);
		SchedulScrn_SendStr(SMS,smslen);
		SchedulScrn_SendStr("\"",1);
		SchedulScrn_SendStr(pbuf_Tail,2);//最后发包尾
		break;
//		OSSemPend(SchedulAckSem,SEND_DATA_INTVL,&err);	//等待300ms时间
#if 0
		if(err!=OS_ERR_NONE)
		{
			result=1;
			SSCRN_Print("向调度屏发送失败,次数:%d\r\n",3-cnt);
		}
		else
		{
			SSCRN_Print("向调度屏发送成功\r\n");
			result=0;
			break;
		}
#endif
	}

//	OSSemPost(Schedul_UsartSendSem);
	return result;
}

#endif	 ////D5调度屏

/*********************************************************
*Function:       u8 SScrn_Ack_Send()
*Description:     向调度屏/手柄发送中心短消息

*Calls:          
*Called By:      
*Input:          无
*Output:        无
*Return:        0表示发送成功 1:发送失败
*Others:
*********************************************************/
static u8 SScrn_Ack_Send()
{
	u8 buf[5];
	SSCRN_Print("向调度屏发送应答");
	return(Ack_Package(buf,Normal_Ack,ACK_CMD,0));
}
/*********************************************************
*Function:       SScrn_Result_Send(u8 result)
*Description:     返回调度屏设置命令的执行结果

*Calls:          
*Called By:       
*Input:          result:返回执行结果   
						0:表示执行成功
						1:表示执行失败
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:
*********************************************************/
u8 SScrn_Result_Send(u8 result)
{
	u8 buf[6];

	if ((result!=0)&&(result!=1))
	{
		SSCRN_Print("返回调度屏设置结果,错误输入\r\n");
		return 1;
	}
	SSCRN_Print("返回调度屏设置结果\r\n");
	return(Ack_Package(buf,Operate_Ack,RETURN_CMD,result));
	
}
u8 SScrn_CmdResult_Send(u8 Cmd,u8 Ackmode,u8 result)
{
	u8 buf[6];

	if ((result!=0)&&(result!=1))
	{
		SSCRN_Print("返回调度屏设置结果,错误输入\r\n");
		return 1;
	}
	SSCRN_Print("返回调度屏设置结果\r\n");
	return(Ack_Package(buf,Ackmode,Cmd,result));
	
}

/*********************************************************
*Function:       SScrn_SMSResult_Send(u8 result)
*Description:     返回调度屏设置命令的执行结果

*Calls:          
*Called By:       
*Input:          result:返回执行结果   
0:表示发送成功
1:表示发送失败.
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:
*********************************************************/
u8 SScrn_SMSResult_Send(u8 result)
{
	u8 buf[5];

	if ((result!=0)&&(result!=1))
	{
		SSCRN_Print("返回调度屏短信发送结果,错误输入\r\n");
		return 1;
	}
	SSCRN_Print("返回调度屏短信发送结果");
	if(result)
	{
		return(Ack_Package(buf,Operate_Ack,SMS_SEND_FAILD_CMD,0));
	}
	else
	{
		return(Ack_Package(buf,Operate_Ack,SMS_SEND_OK_CMD,0));	
	}

}
/*********************************************************
*Function:       SScrn_CenterSMS_Send(u8* Str,u8 len)
*Description:     发送中心短息

*Calls:          
*Called By:       
*Input:				Str:发送的内容指针
				   Len:发送数据长度
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:		内容为ascii码或者GB2312码
*********************************************************/
u8 SScrn_CenterSMS_Send(u8* Str,u16 len)	  //发送中心短信	//修改,支持超长中心调度短信
{
	u8 res=0;
	u8 *Pstr=Str;
	u16 strlen = len;
	if((CheckParam1(Str)))
	{
		SSCRN_Print("向调度屏发送中心短信出错,输入错误");
		return 1;
	}
	//SSCRN_Print("(%s %s %d):向调度屏发送中心短信", __FILE__, __func__, __LINE__);
	while(strlen>MAX_MSG_LEN)
	{
		res=Host_Send_Packag(HOST_CenSMS_CMD,Pstr,MAX_MSG_LEN);
		if(res)
		{
			break;
		}
		strlen-=MAX_MSG_LEN;
		Pstr+=MAX_MSG_LEN;
//		OSTimeDlyHMSM(0,0,1,0);
	}
	if(res)
	{
		return 1;
	}
	else if(strlen>0)
	{
		return(Host_Send_Packag(HOST_CenSMS_CMD,Pstr,strlen));
	}
	else
	{
		return 0;
	}
}
/*********************************************************
*Function:       SScrn_CallIn_Send(u8 *telnum,u8 len)
*Description:     通知调度屏,有电话打入

*Calls:          
*Called By:       
*Input:          电话号码及号码长度
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:
*********************************************************/
u8 SScrn_CallIn_Send(u8 *telnum,u8 tellen)
{
	if((CheckParam1(telnum))||(tellen>MAX_MSG_LEN))
	{
		SSCRN_Print("向调度屏告知有电话打入出错,错误输入参数\r\n");	
		return 1;
	}
	SSCRN_Print("向调度屏告知有电话打入\r\n");
	return(Host_Send_Packag(CALL_IN_CMD,telnum,tellen));
}
/*********************************************************
*Function:       SScrn_HandOff_Send()
*Description:     通知调度屏,电话挂断

*Calls:          
*Called By:       
*Input:         
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:
*********************************************************/
u8 SScrn_HandOff_Send()
{
	u8 buf[5];
	SSCRN_Print("向调度屏发送电话挂断\r\n");
	return(Ack_Package(buf,Normal_Ack,HAND_OFF_CMD,0));
}
/*********************************************************
*Function:       SScrn_HandOn_Send()
*Description:     通知调度屏,电话接通

*Calls:          
*Called By:       
*Input:         
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:
*********************************************************/
u8 SScrn_HandOn_Send()
{
	u8 buf[5];
	SSCRN_Print("向调度屏发送电话接通\r\n");
	return(Ack_Package(buf,Normal_Ack,HAND_ON_CMD,0));
}
/*********************************************************
*Function:       SScrn_Version_Send()
*Description:     向升级PC机发送版本号

*Calls:          
*Called By:       
*Input:          VersionStr:版本号字符串
				  StrLen:字符串长度
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:
*********************************************************/
u8 SScrn_Version_Send(u8* VersionStr,u8 StrLen)
{
	if((CheckParam1(VersionStr))||(StrLen>MAX_MSG_LEN))
	{
		SSCRN_Print("向PC报告当前版本号错误,错误输入参数\r\n");	
		return 1;
	}
	SSCRN_Print("向PC报告当前版本号\r\n");
	return(Host_Send_Packag(UPDATA_VERSION_UP_CMD,VersionStr,StrLen));
}
/*********************************************************
*Function:       SScrn_Data_Req()
*Description:    向升级PC机请求数据

*Calls:          
*Called By:       
*Input:         PacketNum:请求包序号
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:
*********************************************************/
u8 SScrn_Data_Req(u32 PacketNum) 
{
//	u8 buf[5];
	SSCRN_Print("向PC请求升级数据\r\n");
	return(Host_Send_Packag(UPDATA_REQ_DATA_CMD,(u8 *)&PacketNum,4));
}  
/*********************************************************
*Function:       SScrn_GroupNum_Send(u8* Name,u8 NameLen,u8* TelNum,u8 TelLen)
*Description:     更新下载集团号码

*Calls:          
*Called By:       
*Input:			   Name:号码名称 GB2312码
				   NameLen:名称长度
				   TelNum:号码
				   TelLen:号码长度
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:		
*********************************************************/
#define GROUP_NAME_MAX_LEN 8
#define MAX_GROUP_TEL_LEN  20
u8 SScrn_GroupNum_Send(u8* Name,u8 NameLen,u8* TelNum,u8 TelLen)	 
{
	u8 res=0;
	u8 Buf[30]={0};
	if((CheckParam2(Name,TelNum))||(NameLen>GROUP_NAME_MAX_LEN)||(TelLen>MAX_GROUP_TEL_LEN))
	{
		SSCRN_Print("向调度屏发送集团号码出错,输入错误\r\n");
		return 1;
	}
	SSCRN_Print("向调度屏发送集团号码\r\n");
	Buf[0]=0x04;
	memcpy(&Buf[1],Name,NameLen);
	Buf[9]=0x0b;
	memcpy(&Buf[10],TelNum,TelLen);
	res=Host_Send_Packag(GROUP_NUM_SEND_CMD,Buf,30);
	if(res)
	{
		SSCRN_Print("向调度屏发送集团号码出错\r\n");
	}
	else
	{
		SSCRN_Print("向调度屏发送集团号码成功\r\n");
	}
	return res;
}
/*********************************************************
*Function:       SScrn_GroupNum_Clr(void)
*Description:     清空下载集团号码

*Calls:          
*Called By:       
*Input:			   
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:		
*********************************************************/
u8 SScrn_GroupNum_Clr(void)	 
{
	u8 buf[5];
	SSCRN_Print("清空集团号码\r\n");
	return(Ack_Package(buf,Normal_Ack,GROUP_NUM_CLR_CMD,0));
}
/*********************************************************
*Function:       SScrn_Status_Send(STRUCT_RMC *pGpsInfo,STRUCT_HOST_STATE * Host_State)
*Description:     向调度屏发送主机状态

*Calls:          
*Called By:       
*Input: 		time:系统时间 6字节 压缩bcd码
				pGpsInfo:gps信息
				HostType:主机类型 位b0：1 GPRS连通、0 GPRS没连通
									位b2位b1主机类型：00  GPRS型、01 GSM/GPRS型、10其它
									位b6位b5位b4网络类型： 000 网络搜索中、001 中国移动、010 中国联通、110 需解析原字符
									位b7 1 报警
				OverSpeed:超速限制
				CSQ:信号强度 范围:0x00-0x1f
				ACC_State:状态: 0:已点火 1未点火
				DriveState:看车状态
				TCP_UDP:  '1' 表示当前为TCP连接，'0'表示UDP连接
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:
*********************************************************/
u8 SScrn_Status_Send(STRUCT_RMC *pGpsInfo,STRUCT_HOST_STATE * Host_State)
{
	u8 Buf[50];
	u8 *P;
	u8 Len=0;
	u8 temp=0;
//	u32 speed;
//	u8 paralen=0;
	
	if(CheckParam2(Host_State,pGpsInfo))
	{
		SSCRN_Print("向调度屏发送主机状态出错,错误输入参数\r\n"); 
		return 1;
	}
	P=Buf;
	*P++=0x24;
	*P++=0xa0;
	*P++;
	*P++=pGpsInfo->Time[3];
    *P++=pGpsInfo->Time[4];
	*P++=pGpsInfo->Time[5];
	Len=6;
	if(pGpsInfo->North_Indicator=='S') //南纬加0x80表负
	{
		*P++=0x80+pGpsInfo->Latitude[0]-'0';
	}
	else
	{
		*P++=pGpsInfo->Latitude[0]-'0';
	}
	*P++=((pGpsInfo->Latitude[1]-'0')<<4)+(pGpsInfo->Latitude[2]-'0');
	*P++=((pGpsInfo->Latitude[3]-'0')<<4)+(pGpsInfo->Latitude[4]-'0');
	*P++=((pGpsInfo->Latitude[5]-'0')<<4)+(pGpsInfo->Latitude[6]-'0');
	if(pGpsInfo->East_Indicator=='W') //西纬加0x80表负
	{
		*P++=(0x80|((pGpsInfo->Longitude[0]-'0')<<4))+(pGpsInfo->Longitude[1]-'0');
	}
	else
	{
		*P++=((pGpsInfo->Longitude[0]-'0')<<4)+(pGpsInfo->Longitude[1]-'0');
	}
	*P++=((pGpsInfo->Longitude[2]-'0')<<4)+(pGpsInfo->Longitude[3]-'0');
	*P++=((pGpsInfo->Longitude[4]-'0')<<4)+(pGpsInfo->Longitude[5]-'0');
	*P++=((pGpsInfo->Longitude[6]-'0')<<4)+(pGpsInfo->Longitude[7]-'0');
	Len+=8;
	*P++=0x00|((u8)pGpsInfo->Speed/100%10);
	*P++=(((u8)pGpsInfo->Speed/10%10)<<4)+ (u8)pGpsInfo->Speed%10;
	*P++=0x00|(pGpsInfo->COG/100%10);
	*P++=((pGpsInfo->COG/10%10)<<4)+ (pGpsInfo->COG)%10;
	Len+=4;
	if(pGpsInfo->Fix_Status==GPS_FIXED)
	{
		*P++=1;
	}
	else
	{
		*P++=0;
	}
	//类型状态
	*P++=Host_State->HostType;
	*P++=Host_State->OverSpeed;
	*P++=0;
	*P++=0;
	*P++=Host_State->CSQ;
	*P++=Host_State->ACC_State;
	*P++=Host_State->DriveState;
	*P++=pGpsInfo->Time[0];
    *P++=pGpsInfo->Time[1];
	*P++=pGpsInfo->Time[2];
	*P++=Host_State->TCP_UDP;//UDP或TCP选择
	*P++=0x08; //unknown
	memcpy(P,"abcdefgh",8);
	P+=8;
	Len+=21;
	Buf[2]=Len-3+2;
	temp=Protocol_Check(Buf,Len);
	*P++=temp;
	*P=0x0a;
	Len+=2;

//	SSCRN_Print("向调度屏发送主机状态");
//	OSSemPend(Schedul_UsartSendSem,0,&temp);
	SchedulScrn_SendStr(Buf,Len);
//	OSSemPost(Schedul_UsartSendSem);
	return 0;
}

/*********************************************************
*Function:       Ack_Package()
*Description:    简单数据包组包发送
*Calls:
*Called By:
*Input:            无
*Output:
*Return:
*Others:
*********************************************************/

static u8 Ack_Package(u8 *Str,u8 mode,u8 Cmd,u8 Result)
{
	u8 *PStr=Str;
	u8 Check_Sum=0;
	u8 err;
	u8 cnt=3;
	u8 flag=0;
	u8 datalen=0;
	
	if(NULL==Str)
	{
		return 1;
	}

//	OSSemPend(Schedul_UsartSendSem,0,&err);
	if(Normal_Ack==mode)
	{
		*PStr++=0x24;
		*PStr++=Cmd;
	    *PStr++=0x02;
		Check_Sum=Protocol_Check(Str,3);
		*PStr++=Check_Sum;
		*PStr++=0x0a;
		datalen=5;
	}
	else if(Operate_Ack==mode)
	{
		*PStr++=0x24;
		*PStr++=Cmd;
	    *PStr++=0x03;
		*PStr++=Result;
		Check_Sum=Protocol_Check(Str,4);
		*PStr++=Check_Sum;
		*PStr++=0x0a;
		datalen=6;
	}
	while(cnt--)
	{
		oa_bool ret;
		ret = SchedulScrn_SendStr(Str,datalen);
		//if((Cmd==ACK_CMD))//如果发送的是应答命令字,则不等待
		if (OA_TRUE == ret){
			SSCRN_Print("向调度屏发送成功");
			flag=0;
			break;
		}
#if 0
		OSSemPend(SchedulAckSem,SEND_DATA_INTVL,&err);	//等待300ms时间
		if(err!=OS_ERR_NONE)
		{
			SSCRN_Print("向调度屏发送失败,次数:%d\r\n",3-cnt);
			flag=1;

		}
		else
		{
			SSCRN_Print("向调度屏发送成功!\r\n");
			flag=0;
			break;
		}
	}
	OSSemPost(Schedul_UsartSendSem);
#endif
	}
	return flag;
}

/*********************************************************
*Function:		SchedulScrn_SendStr
*Description:	向调度屏端口发送字符串
*Calls:			USART_SendData() 串口字符发送
*				USART_GetFlagStatus() 串口标志
*Called By:		无
*Input:			要发送的字符串及长度
*Output:		无
*Return:		发送成功与否
*Others:		SchedulIOMutex 串口保护
*********************************************************/
oa_bool SchedulScrn_SendStr(u8 *Str,u16 Len)
{
#if 0
	u16 i=0;
//	u8 err;
	if(SSCRN_COM)
	{
		for(i=0;i<Len; i++)
		{
			USART_SendData(SSCRN_COM,*Str++);
			while(!USART_GetFlagStatus(SSCRN_COM,USART_FLAG_TXE));
		}
		return 0;
	}
#endif
	return oa_uart_write(OA_UART3, Str, Len);
	return 1;
}
/*********************************************************
*Function:		Protocol_Check
*Description:	数据校验计算
*Calls:			
*Called By:		无
*Input:			要发送的字符串及长度
*Output:		无
*Return:		计算成功
*Others:		
*********************************************************/
static u8 Protocol_Check(const u8 *PStr,const u16 PStr_Length)
{
  u8 Check=*PStr++;
  u16 i=0;
  for( ;i<PStr_Length-1&&PStr;i++)
  {
    Check^=*PStr++;
  }
  return Check;
}
/*********************************************************
*Function:		Protocol_Check
*Description:	数据校验计算
*Calls:			
*Called By:		无
*Input:			要发送的字符串及长度
*Output:		无
*Return:		计算成功
*Others:		
*********************************************************/
static u8 Protocol_Check_2para(const u8 *PStr1,const u16 PStr_Length1,const u8 *PStr2,const u16 PStr_Length2)
{
  u8 Check=*PStr1++;
  u16 i=0;

  for(i=0;i<PStr_Length1-1&&PStr1;i++)
  {
    Check^=*PStr1++;
  }
  for(i=0;i<PStr_Length2&&PStr2;i++)
  {
    Check^=*PStr2++;
  }
  return Check;
}

/*********************************************************
*Function:       Host_Send_Packag(u8 Cmd,const u8 *Value,const u8 ValuLen)
*Description:    主机向手柄发送数据包
*Calls:          
*Called By:      无
*Input:           Cmd  指令码
				Value  数据指针
			  ValuLen  数据长度
*Output:         
*Return:         
*Others:		 短信分条发送未做
*********************************************************/
 u8 Host_Send_Packag(u8 Cmd, u8 *Value,const u8 ValuLen)
{
//	u8 *Pstr = (u8 *)Value;
	u8 pbuf_head[3];		//3字节数组，存放  包头 指令码 长度
	u8 pbuf_Tail[2];		//2字节数组，存放 校验值 结束符
	u8 err;
	u8 cnt=3;
	u8 result=0;

	pbuf_head[0]=0x24;	//包头
	pbuf_head[1]=Cmd;    //指令码
	if(ValuLen>0)
	{
		pbuf_head[2]=ValuLen+2;			   //写包长度
	}
	else
	{
		pbuf_head[2]=2;					   //空包
	}								  //指向数据包最后一个字节的下一个字节处
	pbuf_Tail[0]=Protocol_Check_2para(pbuf_head,3,Value,ValuLen);  //校验码生成  分别从包头  和内容进行校验
	pbuf_Tail[1]=0x0a;				//结束符 


//	OSSemPend(Schedul_UsartSendSem,0,&err);
	while(cnt--)
	{
		SchedulScrn_SendStr(pbuf_head,3);				//先发包头
		SchedulScrn_SendStr(Value,ValuLen);			  //再发内容
		SchedulScrn_SendStr(pbuf_Tail,2);				//最后发包尾
		#if 0
		OSSemPend(SchedulAckSem,SEND_DATA_INTVL*2,&err);	//等待300ms时间
		if(err!=OS_ERR_NONE)
		{
			result=1;
			SSCRN_Print("向调度屏发送失败,次数:%d\r\n",3-cnt);
		}
		#endif
		
		//SSCRN_Print("(%s %s %d):向调度屏发送成功", __FILE__, __func__, __LINE__);
	}
	result=0;
//	OSSemPost(Schedul_UsartSendSem);
	return result;
}
#if PROTCL_HX54
/*********************************************************
*Function:       u8 SScrn_GetStationData(u8 *Direction,u8 * total_num,u8 *sta_num,u8 *sta_type,FP32 * LAT,FP32 *LON)
*Description:     获取站点信息数据

*Calls:          
*Called By:      
*Input:          无
*Output:        RoadNum      线路号  4字节
				Direction	 方向信息  0为上行 1为下行
				total_num	 总共站点数 
				sta_num		 站点编号 从0开始
				sta_type	 站点类型 	 起点站：0x01
										 中间停靠站：0x02
										 终点站：0x03
										 加油站：0x04
										 停车站：0x05
										 非停靠站的中间点：0x06
										 其他：0

				LAT			 纬度信息，单精度浮点
				LON			 经度信息，单精度浮点
*Return:        0表示发送成功 1:发送失败
*Others:
*********************************************************/
u8 SScrn_GetStationData(u8 *RoadNum,u8 *Direction,u8 * total_num,u8 *sta_num,u8 *sta_type,FP32 * LAT,FP32 *LON)
{
	u8 datalen=0;
	u8 err;
	u8 result=0;	

	if(CheckParam4(RoadNum,Direction,sta_num,sta_type)||CheckParam3(sta_type,LAT,LON))
	{
		SSCRN_Print("获取参数错误,错误输入!\r\n");
		return 1;
	}
	OSSemPend(SchedulProtect,0,&err);
	memcpy((u8*)RoadNum,(u8*)&SScrn_Result.result.STATION_DATA.ROAD_NUM,4);
	*Direction=SScrn_Result.result.STATION_DATA.STA_DIRECTION;
	*total_num=SScrn_Result.result.STATION_DATA.STA_TOTAL_NUM;
	*sta_num=SScrn_Result.result.STATION_DATA.STA_NUM;
	*sta_type=SScrn_Result.result.STATION_DATA.STA_TYPE;
	memcpy((u8*)LAT,(u8*)&SScrn_Result.result.STATION_DATA.LAT,4);
	memcpy((u8*)LON,(u8*)&SScrn_Result.result.STATION_DATA.LON,4);
	OSSemPost(SchedulProtect);
	return 0;
}
/*********************************************************
*Function:       SScrn_StationData_Rsq()
*Description:    向升级PC机请求数据

*Calls:          
*Called By:       
*Input:         PacketNum:请求包序号
*Output:        无
*Return:        0:表示发送成功(以收到调度屏/手柄应答为准)1:发送失败
*Others:
*********************************************************/
u8 SScrn_StationDownloadStart_Rsq(void) 
{
	u8 buf[5];
	SSCRN_Print("开始下载线路信息命令应答\r\n");
	return(Ack_Package(buf,Normal_Ack,STATION_DOWNLOAD_START_RSP_CMD,0));
}
u8 SScrn_StationData_Rsq(void) 
{
	u8 buf[5];
	SSCRN_Print("向调度屏发送站点信息接收完毕\r\n");
	return(Ack_Package(buf,Normal_Ack,STATION_DOWNLOAD_RSP_CMD,0));
}

#endif
