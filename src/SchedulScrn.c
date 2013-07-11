/*
*********************************************************************************************************
*	                                  
*	ģ������ : ������BSP��
*	�ļ����� : SchedulScrn.c
*	��    �� : V1.0
*	˵    �� : ���������ƺ����ݽ���
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2012-1-18        κ����  ST�̼���V3.5.0�汾��
*
*	Copyright (C), 2010-2011, ���ʿƼ�
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
/*��������*/
#define Schedul_RecvBuf_Num	 300//����BUF��С
#define SEND_DATA_INTVL  30 //�趨Ӧ��ʱʱ������� D5Э��Ӧ��ʱ��Ϊ300ms  ����Ϊ0
#define WAIT_TIME_OUT  300 //�趨���ݵȴ���ʱʱ������� Ϊ0����Զ�ȴ�

#define MAX_MSG_LEN 250

#define CheckParam1(param1) ((param1 == NULL))
#define CheckParam2(param1,param2) ((param1 == NULL)||(param2 == NULL))
#define CheckParam3(param1,param2,param3) ((param1 == NULL)||(param2== NULL)||(param3 == NULL))
#define CheckParam4(param1,param2,param3,param4) ((param1 == NULL)||(param2== NULL)||(param3 == NULL)||(param4 == NULL))

//static OS_EVENT * SchedulRcvSem;//�յ�����������֪ͨ�ź���
//static OS_EVENT *Schedul_UsartSendSem;//���ڱ����ź���
//static OS_EVENT *SchedulAckSem=0;//�յ�Ӧ���ź���
//static OS_EVENT *SchedulProtect=0;//�������ڲ������ı����ź���
//static DataQueue SchedulRecvQue;  //���ն���
static u8 SchedulRecvBuf[Schedul_RecvBuf_Num];//���������ջ���
static u8 IntRecvBuf[300];//���������ջ���,��Ϊ�ж��н���
STRUCT_SScrn_Result SScrn_Result;
//static USART_TypeDef *SSCRN_COM=0;
/*��������,���ڲ�����*/
static void SchedulScrn_RcvPhase(u8 Com_Data);//���������ݽ���
static void SchedulScrn_USART_ISR(void);//�����ж�

u8 SScrn_Result_Send(u8 result);
static u8 Ack_Package(u8 *Str,u8 mode,u8 Cmd,u8 Result);//�����ݰ����
static u8 Protocol_Check(const u8 *PStr,const u16 PStr_Length);//У�����
static u8 Protocol_Check_2para(const u8 *PStr1,const u16 PStr_Length1,const u8 *PStr2,const u16 PStr_Length2);//У�����  ��������
oa_bool SchedulScrn_SendStr(u8 *Str,u16 Len);//���ڷ��ͺ���	
static u8 Host_Send_Packag(u8 Cmd, u8 *Value,const u8 ValuLen);//�������
u8 SScrn_UpResult(u8 *Str,u8 Cmd);
static u8 SScrn_Ack_Send(void); /*�������ݽ���Ӧ��*/
#if 0
/*********************************************************
*Function:       SScrn_HW_Init()
*Description:     ���������Ӳ����ʼ��

*Calls:          
*Called By:       
*Input:          ��
*Output:        ��
*Return:        ��
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
*Description:     ��������������س�ʼ��

*Calls:          
*Called By:       
*Input:          ��
*Output:        ��
*Return:        ��
*Others:
*********************************************************/
void SScrn_SW_Init()
{
#if 0
	if(SchedulAckSem==0)
	{
		SchedulAckSem = OSSemCreate(0);//�յ�Ӧ��֪ͨ�ź��� 
	}
	if(SchedulRcvSem==0)
	{
		SchedulRcvSem = OSMboxCreate(0);//�յ�����֪ͨ�ź�����
	}
	if(Schedul_UsartSendSem==0)
	{
		Schedul_UsartSendSem = OSSemCreate(1);//������Դ�����ź���
	}
	if(SchedulProtect==0)
	{
		SchedulProtect = OSSemCreate(1);//���ݶ�д�����ź���
	}
	QueueCreate(SchedulRecvBuf,Schedul_RecvBuf_Num,&SchedulRecvQue);//��ʼ�����ն���
#endif
	memset(&SScrn_Result,0,sizeof(SScrn_Result));
}
/*********************************************************
*Function:       SScrn_HW_Init()
*Description:     ���������Ӳ����ʼ��

*Calls:          
*Called By:       
*Input:          ��
*Output:        ��
*Return:        ��
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
*Description:     ��������������س�ʼ��

*Calls:          
*Called By:       
*Input:          ��
*Output:        ��
*Return:        ��
*Others:
*********************************************************/
void SScrn_N990_SW_Init()
{
	if(SchedulAckSem==0)
	{
		SchedulAckSem = OSSemCreate(0);//�յ�Ӧ��֪ͨ�ź��� 
	}
	if(SchedulRcvSem==0)
	{
		SchedulRcvSem = OSMboxCreate(0);//�յ�����֪ͨ�ź�����
	}
	if(Schedul_UsartSendSem==0)
	{
		Schedul_UsartSendSem = OSSemCreate(1);//������Դ�����ź���
	}
	if(SchedulProtect==0)
	{
		SchedulProtect = OSSemCreate(1);//���ݶ�д�����ź���
	}
	QueueCreate(SchedulRecvBuf,Schedul_RecvBuf_Num,&SchedulRecvQue);//��ʼ�����ն���
	memset(&SScrn_Result,0,sizeof(SScrn_Result));
}

/*********************************************************
*Function:       SchedulScrn_USART_ISR()
*Description:    �����жϷ����������ƽ̨���ݡ��绰������
*Calls:
*Called By:
*Input:          ��
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
*Description:    ���ݽ���״̬��
*Calls:
*Called By:
*Input:            ��
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
//			if(Com_Data==0x0F) //ͷ
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
//*Description:    ��RecvBuf�����ݽ������������ݽ��в���������
//*                ����
//*Calls:          
//*Called By:      
//*Input:          
//*Output:
//*Return:         �յ���ָ�����ö��
//*Others:
//*********************************************************/
//u8 SScrn_DataAnaly(void) 
//{
//	u8 err;
//	u8 RcvBuf[300];//���ջ���
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
//	SSCRN_Print("�յ���������Ϣ,��Ϣ����:%d\r\n",MsgLen);
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
////			case UNICODE_SMS_CMD://���Ͷ���
////			case SET_UDP_CMD: //���ƽ̨IP��UDP�趨
////			case SET_TCP_CMD: //���ƽ̨IP��TCP�趨
////			case SET_LOCAL_TEL_CMD://���ñ��غ���
////			case CALL_DIALUP_CMD://�绰����
////			case SET_SPK_CMD://����ͨ������
////			case SET_MIC_CMD://����MIC����
////			#if DRIVE_SIGN_EN
////			case DRIVER_SIGN_CMD://˾����¼
////			#endif
////			#if CUSTOM_CMD
////			case COM_JTT808_CMD:   //jtt808��Ϣ��
////			#endif
////			case UPDATA_PARA_CMD:
////			case UPDATA_DATA_CMD:
////		#if PROTCL_HX54		
////			case STATION_DOWNLOAD_CMD:
////		#endif
////				SScrn_Ack_Send();/*Ӧ��*/
////				SScrn_UpResult(pStr,Cmd);
////				SScrn_Result.CMD_TYPE=Cmd;
////			break;
////			case CALL_HANGOFF_CMD: //����Ч��������
////				SScrn_Ack_Send();/*Ӧ��*/
////				SSCRN_Print("�������趨�һ�\r\n");
////				SScrn_Result.CMD_TYPE=CALL_HANGOFF_CMD;	
////			break;
////			case CALL_HANGON_CMD://��ͨ�绰
////				SScrn_Ack_Send();/*Ӧ��*/
////				SSCRN_Print("�������趨��ͨ�绰\r\n");
////				SScrn_Result.CMD_TYPE=CALL_HANGON_CMD;	
////			break;
////			case UPDATA_REQ_CMD://��������
////				SScrn_Ack_Send();/*Ӧ��*/
////				SSCRN_Print("PC��������\r\n");
////				SScrn_Result.CMD_TYPE=UPDATA_REQ_CMD;	
////			break;
////			case CALL_SPKSWITCH_CMD://�л�����ͨ��
////				SScrn_Ack_Send();/*Ӧ��*/
////				SSCRN_Print("�������趨�л�����ͨ��\r\n");
////				SScrn_Result.CMD_TYPE=CALL_SPKSWITCH_CMD;
////			break;
////		#if PROTCL_HX54
////			case STATION_DOWNLOAD_START_CMD:
////				SScrn_Ack_Send();/*Ӧ��*/
////				SSCRN_Print("���ڵ�����·��ʼ......\r\n");
////				SScrn_Result.CMD_TYPE=STATION_DOWNLOAD_START_CMD;
////			break;
////		#endif
//			default:
//			//	SScrn_Result.CMD_TYPE=SCCRN_UNEXP_CMD;
//				SSCRN_Print("�������յ�δ������Ϣ:\r\n");
//				SSCRN_Print("ָ����Ϊ:%02x\r\n",Cmd);
//				return 	SCCRN_UNEXP_CMD;
//			break;
//		}
//	}														 
//	return SScrn_Result.CMD_TYPE;
//}
/*********************************************************
*Function:       BSP_XOR_Check
*Description:    ��Str��ʼ��StrLen���ȸ����������У��
*Calls:          ��
*Called By:      �� 
*Input:          ��
*Output:         Check У����
*Return:         0���ɹ� ������ʧ��
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
*Description:    �豸�·�������
*Calls:
*Called By:
*Input:            ��
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
*Description:    �豸�·���������
*Calls:
*Called By:
*Input:            ��
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
	Mem_Copy(pStr,ID,idlen); //����10λ��������
	pStr+=idlen;

	pbuf_Tail[0]=Protocol_Check_2para(pbuf_head+1,13,data,datalen);  //У��������  �ֱ�Ӱ�ͷ  �����ݽ���У��
	pbuf_Tail[1]=0xF0;
	pbuf_Tail[2]=0xFF;

	SSCRN_Print("��������������𶩵�\r\n");
	OSSemPend(Schedul_UsartSendSem,0,&err);
		SchedulScrn_SendStr(pbuf_head,14);			//�ȷ���ͷ
		SchedulScrn_SendStr(data,datalen);			//�ٷ�����
		SchedulScrn_SendStr(pbuf_Tail,3);			//��󷢰�β
	OSSemPost(Schedul_UsartSendSem);
	return 0;
}

/*********************************************************
*Function:       SchedulScrn_CallforcarInfoSend()
*Description:    �豸�·���������
*Calls:
*Called By:
*Input:            ��
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
	Mem_Copy(pStr,ID,idlen); //����10λ��������
	pStr+=idlen;
	*pStr++=Tellen;
	Mem_Copy(pStr,Tel,Tellen);
	pStr+=Tellen;

	pbuf_Tail[0]=Protocol_Check_2para(pbuf_head+1,14+Tellen,data,datalen);  //У��������  �ֱ�Ӱ�ͷ  �����ݽ���У��
	pbuf_Tail[1]=0xF0;
	pbuf_Tail[2]=0xFF;

	SSCRN_Print("�������������ϸ����\r\n");
	for(Len=0;Len<(15+Tellen);Len++)
		SSCRN_Print("0x%02x ",pbuf_head[Len]);
	SSCRN_Print("\r\n");
	OSSemPend(Schedul_UsartSendSem,0,&err);
		SchedulScrn_SendStr(pbuf_head,15+Tellen);			//�ȷ���ͷ
		SchedulScrn_SendStr(data,datalen);			//�ٷ�����
		SchedulScrn_SendStr(pbuf_Tail,3);			//��󷢰�β
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
//	Mem_Copy(pStr,ID,idlen); //����10λ��������
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
//	SSCRN_Print("��������������𶩵�\r\n");
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
*Description:    �豸�·�ȡ���������ĵ���
*Calls:
*Called By:
*Input:            ��
*Output:
*Return:
*Others:
*********************************************************/
u8 SchedulScrn_CallforcarCancelSend(u8 *Sendbuf,u16 Sendbuflen,u8* ID,u8 idlen)
{
	//����δ�� -------------------
//	u8 temp=1;
//	u8 *pStr=Sendbuf;
//	u8 Len=0;
//	*pStr++=0x0F;
//	*pStr++=;
//	Len=(11+Tellen+datalen);
//	*pStr++=(Len>>4)>9?((Len>>4)+0x37):((Len>>4)+'0');
//	*pStr++=(Len&0x0F)>9?((Len&0x0F)+0x37):((Len&0x0F)+'0');
//	Mem_Copy(pStr,ID,idlen); //����10λ��������
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
//	SSCRN_Print("��������������𶩵�\r\n");
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
*Description:    �豸�·����ŵ���������������Ϣ
*Calls:
*Called By:
*Input: Tel �ֻ��� ������ϢʱΪ��������           
		Time BCD��ʽ��ʱ��YYMMDDHHMMSS
		cmd 	   'C'����, 'E'Ӣ�ģ�'G'����
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

	SSCRN_Print("����������Ͷ���\r\n");
	pbuf_Tail[0]=Protocol_Check_2para(pbuf_head+1,3+21+13+1,SMS,smslen);  //У��������  �ֱ�Ӱ�ͷ  �����ݽ���У��
	pbuf_Tail[1]=0xF0;
	pbuf_Tail[2]=0xFF;

	OSSemPend(Schedul_UsartSendSem,0,&err);
		SchedulScrn_SendStr(pbuf_head,39);			//�ȷ���ͷ
		SchedulScrn_SendStr(SMS,smslen);			//�ٷ�����
		SchedulScrn_SendStr(pbuf_Tail,3);			//��󷢰�β
	OSSemPost(Schedul_UsartSendSem);
	return 0;
}
//return 0�ɹ�  1ʧ��
u8 SScrn_GetResult(u8 *Buf,u8 BufSize,u8 *ReadSize,u8 Cmd,u8 SubField)
{
	u8 datalen=0;
	u8 err;
	u8 result=0;	

	if(CheckParam1(Buf)||(Cmd!=SScrn_Result.CMD_TYPE))
	{
		SSCRN_Print("��ȡ��������,��������!\r\n");
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
		SSCRN_Print("��ȡ��������,��������!\r\n");
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
//	SSCRN_Print("�յ���������Ϣ,���ݳ���:%d\r\n",datalen);
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
#elif 1//SCREEN_HD5>0 //D5������
#if 0
/*********************************************************
*Function:       SchedulScrn_RcvPhase()
*Description:    ���ݽ���״̬��
*Calls:
*Called By:
*Input:            ��
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
			if(Com_Data==0x24) //ͷ
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
					
//					Ack_Package(Buf,Normal_Ack,ACK_CMD,0);//���ж��м�ʱ�ظ�Ӧ��
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
//*Description:    ��ʼ����������ͨ�ſںʹ�����Ҫ�õ�����ر���
//*                
//*Calls:          
//*Called By:      
//*Input:          
//*Output:
//*Return:         �յ���ָ�����ö��
//*Others:
//*********************************************************/
//u8 SScrn_Init (void) 
//{
//	SScrn_HW_Init();
//	SScrn_SW_Init();
//	SSCRN_Print("��������ʼ�����\r\n");
//	return 0;
//}
/*********************************************************
*Function:       SScrn_DataAnaly()
*Description:    ��RecvBuf�����ݽ������������ݽ��в���������
*                ����
*Calls:          
*Called By:      
*Input:          
*Output:
*Return:         �յ���ָ�����ö��
*Others:
*********************************************************/
u8 SScrn_DataAnaly(void) 
{
	u8 err;
	u8 RcvBuf[256] = {0x0};//���ջ���
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
	//DEBUG("�յ���������Ϣ,��Ϣ����:%d",MsgLen);
	pStr=RcvBuf;
	if(*pStr++==0x24)
	{
		Cmd=*pStr;
		switch(*pStr++)
		{
			case CENTER_SMS_CMD: //��������������ѯ��������ͬ
			case UNICODE_SMS_CMD://���Ͷ���
			case SET_UDP_CMD: //���ƽ̨IP��UDP�趨
			case SET_TCP_CMD: //���ƽ̨IP��TCP�趨
			case SET_LOCAL_TEL_CMD://���ñ��غ���
			case CALL_DIALUP_CMD://�绰����
			case SET_SPK_CMD://����ͨ������
			case SET_MIC_CMD://����MIC����
			#if DRIVE_SIGN_EN
			case DRIVER_SIGN_CMD://˾����¼
			#endif
			#if CUSTOM_CMD
			case COM_JTT808_CMD:   //jtt808��Ϣ��
			#endif
			case UPDATA_PARA_CMD:
			case UPDATA_DATA_CMD:
			case STATION_DOWNLOAD_CMD:
			case SCR_SMS_DOWNLOAD_CMD:
				SScrn_Ack_Send();/*Ӧ��*/
				SScrn_UpResult(pStr,Cmd);//copy data to 'SScrn_Result'
				SScrn_Result.CMD_TYPE=Cmd;
			break;
			case CALL_HANGOFF_CMD: //����Ч��������
				SScrn_Ack_Send();/*Ӧ��*/
				SSCRN_Print("�������趨�һ�\r\n");
				SScrn_Result.CMD_TYPE=CALL_HANGOFF_CMD;	
			break;
			case CALL_HANGON_CMD://��ͨ�绰
				SScrn_Ack_Send();/*Ӧ��*/
				SSCRN_Print("�������趨��ͨ�绰\r\n");
				SScrn_Result.CMD_TYPE=CALL_HANGON_CMD;	
			break;
			case UPDATA_REQ_CMD://��������
				SScrn_Ack_Send();/*Ӧ��*/
				SSCRN_Print("PC��������\r\n");
				SScrn_Result.CMD_TYPE=UPDATA_REQ_CMD;	
			break;
			case CALL_SPKSWITCH_CMD://�л�����ͨ��
				SScrn_Ack_Send();/*Ӧ��*/
				SSCRN_Print("�������趨�л�����ͨ��\r\n");
				SScrn_Result.CMD_TYPE=CALL_SPKSWITCH_CMD;
			break;
			case STATION_DOWNLOAD_START_CMD://
				SScrn_Ack_Send();/*Ӧ��*/
				SSCRN_Print("���ڵ�����·��ʼ......\r\n");
				SScrn_Result.CMD_TYPE=STATION_DOWNLOAD_START_CMD;
			break;
			case ACK_CMD:
				//DEBUG("�յ�������Ӧ��");
				return 	SCCRN_UNEXP_CMD;
			default:
			//	SScrn_Result.CMD_TYPE=SCCRN_UNEXP_CMD;
				//DEBUG("(%s %s %d):�������յ�δ������Ϣ", __FILE__, __func__, __LINE__);
				//DEBUG("(%s %s %d):ָ����Ϊ:%02x",__FILE__, __func__, __LINE__, Cmd);
				return 	SCCRN_UNEXP_CMD;
//			break;
		}
	}														 
	return SScrn_Result.CMD_TYPE;
}

/*********************************************************
*Function:      SScrn_GetResult(u8 *Buf,u8 BufSize,u8 *ReadSize,u8 Cmd,u8 SubField)
*Description:   ��ȡ�ӵ���������������ֵ

*Calls:          
*Called By:       
*Input:         Cmd:��Ӧ��������������
				Field:��ȡ�������
*Output:        Str:����д��ָ��
*Return:        0:��ȡ�ɹ� 1:��ȡʧ��
*Others:
*********************************************************/
u8 SScrn_GetResult(u8 *Buf,u8 BufSize,u8 *ReadSize,u8 Cmd,u8 SubField)
{
	u8 datalen=0;
	u8 err;
	u8 result=0;	

	if(CheckParam1(Buf)||(Cmd!=SScrn_Result.CMD_TYPE))
	{
		SSCRN_Print("��ȡ��������,��������!\r\n");
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
*Description:    ����ӵ���������������ֵ

*Calls:          
*Called By:       
*Input:         Cmd:��Ӧ��������ָ����
*Output:        Str:����д��ָ��
*Return:        �������ݳ���,��Ϊ0ʱ��,��ʾ������
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
		SSCRN_Print("��ȡ��������,��������!\r\n");
		return 0;
	}
//	OSSemPend(SchedulProtect,0,&err);
	switch(Cmd)
	{
		case CENTER_SMS_CMD:
			datalen=*pStr++-2;
			Mem_Copy(SScrn_Result.result.SMS_Data.SMSDat,pStr,datalen);  //�����Ķ�����Ϣ����
			SScrn_Result.result.SMS_Data.SMSLen=datalen;
			SScrn_Result.result.SMS_Data.SMSDat[datalen]='\0';
			SSCRN_Print("���Ķ�������:%s",SScrn_Result.result.SMS_Data.SMSDat);
			SScrn_Result.CMD_TYPE=CENTER_SMS_CMD;
		break;
		case UNICODE_SMS_CMD:
		#if (HAND_DEVICE_OR_SCRN==0)  //ʹ���ֱ�
			pStr+=7;							
			datalen=*pStr-2;
			pStr+=3;
			i=1;
			while(i<=(datalen/2)*2)//ȡ���ź���
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
			SSCRN_Print("UNICODE���ź���:%s\r\n",SScrn_Result.result.SMS_Data.TelNum);
			pStr+=3;
			datalen=*pStr;	 //��ȡ���ų���
			pStr++;
//			datalen=(datalen<<4)+ *pStr-0x30;
//			pStr++;
			i=0;
			while(i<datalen)//������Ϣ����
			{
				SScrn_Result.result.SMS_Data.SMSDat[i++]=*pStr++;
			}
			//SScrn_Result.result.SMS_Data.SMSDat[i]='\0';																		
			SScrn_Result.result.SMS_Data.SMSLen=datalen;	
			//SSCRN_Print("UNICODE��������:%s\r\n",SScrn_Result.result.SMS_Data.SMSDat);
			SScrn_Result.CMD_TYPE=Cmd;	
		#elif (HAND_DEVICE_OR_SCRN==1)	//ʹ�õ�����
		{
			u8 total_len=(*pStr++)-3;//��¼�ܳ���=���볤��+���ų���,������ 
			datalen=*pStr++;//���볤��
			for(i=0;i<datalen;i++)//ȡ���ź���
			{
				SScrn_Result.result.SMS_Data.TelNum[i]= (*pStr++);
			}
			SScrn_Result.result.SMS_Data.TelNum[datalen]='\0';
			SSCRN_Print("UNICODE���ź���:%s\r\n",SScrn_Result.result.SMS_Data.TelNum);
			
			datalen=total_len-datalen;
			i=0;
			for(i=0;i<datalen;i++)
			{
				SScrn_Result.result.SMS_Data.SMSDat[i]=*pStr++;
			}
			SScrn_Result.result.SMS_Data.SMSLen=datalen;	
			//SSCRN_Print("UNICODE��������:%s\r\n",SScrn_Result.result.SMS_Data.SMSDat);
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
				err=0;//�����־
				while(i<15)//ȡIP,���Ϸ���
				{
					if(*(pStr+i)=='.')temp++;//�����ַ�'.' 
					else if(*(pStr+i)>='0'&&*(pStr+i)<='9')//���ֵ�Ƿ���ȷ
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
					SSCRN_Print("����������IP����,��������\r\n");
					SScrn_Result.CMD_TYPE=PARA_ERROR_CMD;//��������
					SScrn_Result_Send(1);
				}
				else//�������,ȡ��������
				{
					u8 ip_temp[4]={0};
					u32 ip_port_temp;
					memset(ip_temp,0,4);
					ip_temp[3]=0;
					err=0;
					memset(SScrn_Result.result.IP_Port.Ip,0,sizeof(SScrn_Result.result.IP_Port.Ip));
					for(i=0;i<4;i++)//����ipֵ
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
							SSCRN_Print("����������IP����,��������\r\n");
							err=1;
							break;
						}
					}
					if(err==0)
					{
						SScrn_Result.result.IP_Port.Ip[15]='\0';
						pStr+=17;				   
						ip_port_temp=0;
						do	//ȡPORT
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
							SSCRN_Print("����������PORT����,��������\r\n");
						}
						else
						{
							SScrn_Result.result.IP_Port.port=ip_port_temp;
							SScrn_Result.CMD_TYPE=Cmd;
							SSCRN_Print("����������IP��ȷ%s\r\n",
										SScrn_Result.result.IP_Port.Ip
										);
							SSCRN_Print("����������PORT��ȷ:%d\r\n",ip_port_temp);
						}
					}	
				}
			}
		break;
		case SET_LOCAL_TEL_CMD:
			datalen=*pStr-2-4; //ȡsim���ų���
			pStr+=5;
			i=0;
			while(i<datalen)
			{
				SScrn_Result.result.telnum[i++]=*pStr++;
			}
			SScrn_Result.result.telnum[datalen]='\0';
			SScrn_Result.CMD_TYPE=SET_LOCAL_TEL_CMD;	
			SSCRN_Print("���������ñ��غ�����ȷ:%s\r\n",SScrn_Result.result.telnum);
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
			SSCRN_Print("�������������:%s\r\n",SScrn_Result.result.telnum);
		break;
		case SET_SPK_CMD:
			pStr++;
			if(*pStr=='1')//1Ϊ����
			{
				SScrn_Result.result.value=0;
			}
			else
			{
				SScrn_Result.result.value=*pStr-'0';
			}
			SScrn_Result.CMD_TYPE=SET_SPK_CMD;
			SSCRN_Print("�������趨����������:%d\r\n",SScrn_Result.result.value);
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
			SSCRN_Print("�������趨��˷�����:%d\r\n",SScrn_Result.result.value);
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
				SSCRN_Print("˾����¼��Ϣ:%d ����:%d\r\n",
							SScrn_Result.result.driver_inf.SignFlag,
							SScrn_Result.result.driver_inf.DriverID);
			}
		break;
		#endif
		#if CUSTOM_CMD
		case COM_JTT808_CMD:   //jtt808��Ϣ��
			datalen=(*pStr++)-2;
			SScrn_Result.result.JTT_MSG.datalen=datalen	;
			Mem_Copy(SScrn_Result.result.JTT_MSG.databuf,pStr,datalen);
			SScrn_Result.CMD_TYPE=COM_JTT808_CMD;
			SSCRN_Print("�������յ�JTT808Э����Ϣ��:\r\n");
		break;
		#endif
//		case COM_UPLOAD_REQACK_CMD://��������Ӧ�� �Զ���ָ��
//				if(*pStr==0x05)
//				{
//					pStr++;
//					if(*pStr==0x00) //��������
//					{
//						pStr++;
//						SScrn_Result.result.value16=*pStr<<8;
//						pStr++;
//						SScrn_Result.result.value16+=*pStr;			
//					}
//					else if(*pStr==0x01) //�������°汾
//					{
//						SScrn_Result.result.value16=0;
//						SScrn_Result.CMD_TYPE=COM_UPLOAD_REQACK_CMD;//���ò���					
//					}
//					else
//					{
//						SScrn_Result.CMD_TYPE=SCCRN_ERROR_RETURN;//ack;
//					}
//				}
//				else
//				{
//					SScrn_Result.CMD_TYPE=SCCRN_ERROR_RETURN;	//������
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
//				 	SScrn_Result.CMD_TYPE=SCCRN_ERROR_RETURN;//������
//				 }
//		break;
		case UPDATA_PARA_CMD:
			datalen=(*pStr++)-2;
			SScrn_Result.result.UP_DATA_PARA.PacketTotalNum=*((u32 *)pStr);
			pStr+=4;
			SScrn_Result.result.UP_DATA_PARA.DataSize=*((u32 *)pStr);
			SScrn_Result.CMD_TYPE=UPDATA_PARA_CMD;
			SSCRN_Print("�������յ���������:\r\n");			
		break;
		case UPDATA_DATA_CMD:
		    datalen=(*pStr++)-2;
			SScrn_Result.result.UP_DATA_STORE.datalen=datalen;
			Mem_Copy(SScrn_Result.result.UP_DATA_STORE.UP_DATA,pStr,datalen);
			SScrn_Result.CMD_TYPE=UPDATA_DATA_CMD;
			SSCRN_Print("�������յ���������:\r\n");	
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
			SSCRN_Print("�������յ�վ����Ϣ:\r\n");	
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
			SSCRN_Print("�������յ�����ģ����Ϣ:\r\n");
		break;
		default: 
		break;
	}
//	OSSemPost(SchedulProtect);
	return 0;
}
/*********************************************************
*Function:       SScrn_UniSMS_Send(u8 *Tel,u8 Telen,u8 *Time,u8* SMSStr,u8 Strlen)
*Description:     �����������UNICODE����Ķ���Ϣ

*Calls:          
*Called By:       
*Input:     	  	Tel:�绰����
					Tellen:���볤��
					Time:6�ֽ�BCD��ʱ��
					SMSStr:��������ָ��
					SMSlen:���ų���
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
*Others:
*********************************************************/
//u8 SScrn_UniSMS_Send(u8 *Tel,u8 Telen,u8 *Time,u8* SMSStr,u8 SMSlen)
//{	
//	if((CheckParam3(Tel,Time,SMSStr)))
//	{
//		SSCRN_Print("�����������PDU���ų���,�����������\r\n"); 
//		return 1;
//	}
//
//	SSCRN_Print("�����������PDU����\r\n");
//	return(SScrn_SMS_Send(TELSMS_PDU_CMD,Tel,Telen,Time,SMSStr,SMSlen));
//}
/*********************************************************
*Function:       SScrn_AscSMS_Send(u8 *Tel,u8 Telen,u8 *Time,u8* SMSStr,u8 SMSlen)
*Description:     �����������ASCII����Ķ���Ϣ

*Calls:          
*Called By:       
*Input: 		Tel:�绰����
				Tellen:���볤��
				Time:6�ֽ�BCD��ʱ��
				SMSStr:��������ָ��
				SMSlen:���ų���
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
*Others:
*********************************************************/
//u8 SScrn_AscSMS_Send(u8 *Tel,u8 Telen,u8 *Time,u8* SMSStr,u8 SMSlen)
//{
//	if((CheckParam3(Tel,Time,SMSStr)))
//	{
//		SSCRN_Print("�����������TXT���ų���,�����������\r\n"); 
//		return 1;
//	}
//	SSCRN_Print("�����������TXT����\r\n");
//	return(SScrn_SMS_Send(TELSMS_TXT_CMD,Tel,Telen,Time,SMSStr,SMSlen));
//}
/*********************************************************
*Function:       SScrn_SMS_Send(u8 cmd,u8 *Tel,u8 Telen,u8 *Time,u8 *SMS,u8 len)
*Description:     ��������Ϸ�����Ϣ

*Calls:          
*Called By:       
*Input: 		cmd:���õķ�������  TELSMS_PDU_CMD pdu����  TELSMS_TXT_CMD: TXT����		
				Tel:���ź���
				Telen:���볤��
				Time:����ʱ��
				SMS:��������
				Str:���͵�����ָ��
				Len:�������ݳ���
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
*Others:
*********************************************************/
u8 SScrn_SMS_Send(u8 cmd,u8 *Tel,u8 Tellen,u8 *Time,u8 *SMS,u8 smslen)
{
//	u8 *Pstr;
	u8 pbuf_head[3];		//3�ֽ����飬���  ��ͷ ָ���� ����	
	u8 TimeStr[20];			//20�ֽ�ʱ��  �洢�ַ���
	u8 pbuf_Tail[2];		//2�ֽ����飬��� У��ֵ ������
	u8 err;
	u8 cnt=3;
	u8 result=0;
	u8 Check=0;
	u8 i;
	u16 paklen;

	if((CheckParam3(Tel,Time,SMS)))
	{
		return 1;						 //��������,���ط���ʧ��
	}
	
	pbuf_head[0]=0x24;	//��ͷ
	Check=pbuf_head[0];						   //���
	pbuf_head[1]=cmd;    //ָ����
	Check^=pbuf_head[1];
	paklen=(25+Tellen)+smslen+2;  //20=ʱ���ַ�������+�������ų���=2*7+5+6=25  ʱ���ʽ  "2012-02-16 09:04:20"
	if (paklen>MAX_MSG_LEN)
	{
		return 1;//������󳤶�,ֱ�ӷ���ʧ��
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
	pbuf_Tail[0]=Check;  //У��������  �ֱ�Ӱ�ͷ  �����ݽ���У��
	pbuf_Tail[1]=0x0a;				//������ 

//	OSSemPend(Schedul_UsartSendSem,0,&err);
	while(cnt--)
	{
		SchedulScrn_SendStr(pbuf_head,3);				//�ȷ���ͷ
		SchedulScrn_SendStr("\"",1);			  		//�ٷ�����
		SchedulScrn_SendStr(Tel,Tellen);			  	//�ٷ�����
		SchedulScrn_SendStr("\"",1);
		SchedulScrn_SendStr("\"",1);
		SchedulScrn_SendStr(TimeStr,19);
		SchedulScrn_SendStr("\"",1);
		SchedulScrn_SendStr("\"",1);
		SchedulScrn_SendStr(SMS,smslen);
		SchedulScrn_SendStr("\"",1);
		SchedulScrn_SendStr(pbuf_Tail,2);//��󷢰�β
		break;
//		OSSemPend(SchedulAckSem,SEND_DATA_INTVL,&err);	//�ȴ�300msʱ��
#if 0
		if(err!=OS_ERR_NONE)
		{
			result=1;
			SSCRN_Print("�����������ʧ��,����:%d\r\n",3-cnt);
		}
		else
		{
			SSCRN_Print("����������ͳɹ�\r\n");
			result=0;
			break;
		}
#endif
	}

//	OSSemPost(Schedul_UsartSendSem);
	return result;
}

#endif	 ////D5������

/*********************************************************
*Function:       u8 SScrn_Ack_Send()
*Description:     �������/�ֱ��������Ķ���Ϣ

*Calls:          
*Called By:      
*Input:          ��
*Output:        ��
*Return:        0��ʾ���ͳɹ� 1:����ʧ��
*Others:
*********************************************************/
static u8 SScrn_Ack_Send()
{
	u8 buf[5];
	SSCRN_Print("�����������Ӧ��");
	return(Ack_Package(buf,Normal_Ack,ACK_CMD,0));
}
/*********************************************************
*Function:       SScrn_Result_Send(u8 result)
*Description:     ���ص��������������ִ�н��

*Calls:          
*Called By:       
*Input:          result:����ִ�н��   
						0:��ʾִ�гɹ�
						1:��ʾִ��ʧ��
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
*Others:
*********************************************************/
u8 SScrn_Result_Send(u8 result)
{
	u8 buf[6];

	if ((result!=0)&&(result!=1))
	{
		SSCRN_Print("���ص��������ý��,��������\r\n");
		return 1;
	}
	SSCRN_Print("���ص��������ý��\r\n");
	return(Ack_Package(buf,Operate_Ack,RETURN_CMD,result));
	
}
u8 SScrn_CmdResult_Send(u8 Cmd,u8 Ackmode,u8 result)
{
	u8 buf[6];

	if ((result!=0)&&(result!=1))
	{
		SSCRN_Print("���ص��������ý��,��������\r\n");
		return 1;
	}
	SSCRN_Print("���ص��������ý��\r\n");
	return(Ack_Package(buf,Ackmode,Cmd,result));
	
}

/*********************************************************
*Function:       SScrn_SMSResult_Send(u8 result)
*Description:     ���ص��������������ִ�н��

*Calls:          
*Called By:       
*Input:          result:����ִ�н��   
0:��ʾ���ͳɹ�
1:��ʾ����ʧ��.
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
*Others:
*********************************************************/
u8 SScrn_SMSResult_Send(u8 result)
{
	u8 buf[5];

	if ((result!=0)&&(result!=1))
	{
		SSCRN_Print("���ص��������ŷ��ͽ��,��������\r\n");
		return 1;
	}
	SSCRN_Print("���ص��������ŷ��ͽ��");
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
*Description:     �������Ķ�Ϣ

*Calls:          
*Called By:       
*Input:				Str:���͵�����ָ��
				   Len:�������ݳ���
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
*Others:		����Ϊascii�����GB2312��
*********************************************************/
u8 SScrn_CenterSMS_Send(u8* Str,u16 len)	  //�������Ķ���	//�޸�,֧�ֳ������ĵ��ȶ���
{
	u8 res=0;
	u8 *Pstr=Str;
	u16 strlen = len;
	if((CheckParam1(Str)))
	{
		SSCRN_Print("��������������Ķ��ų���,�������");
		return 1;
	}
	//SSCRN_Print("(%s %s %d):��������������Ķ���", __FILE__, __func__, __LINE__);
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
*Description:     ֪ͨ������,�е绰����

*Calls:          
*Called By:       
*Input:          �绰���뼰���볤��
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
*Others:
*********************************************************/
u8 SScrn_CallIn_Send(u8 *telnum,u8 tellen)
{
	if((CheckParam1(telnum))||(tellen>MAX_MSG_LEN))
	{
		SSCRN_Print("���������֪�е绰�������,�����������\r\n");	
		return 1;
	}
	SSCRN_Print("���������֪�е绰����\r\n");
	return(Host_Send_Packag(CALL_IN_CMD,telnum,tellen));
}
/*********************************************************
*Function:       SScrn_HandOff_Send()
*Description:     ֪ͨ������,�绰�Ҷ�

*Calls:          
*Called By:       
*Input:         
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
*Others:
*********************************************************/
u8 SScrn_HandOff_Send()
{
	u8 buf[5];
	SSCRN_Print("����������͵绰�Ҷ�\r\n");
	return(Ack_Package(buf,Normal_Ack,HAND_OFF_CMD,0));
}
/*********************************************************
*Function:       SScrn_HandOn_Send()
*Description:     ֪ͨ������,�绰��ͨ

*Calls:          
*Called By:       
*Input:         
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
*Others:
*********************************************************/
u8 SScrn_HandOn_Send()
{
	u8 buf[5];
	SSCRN_Print("����������͵绰��ͨ\r\n");
	return(Ack_Package(buf,Normal_Ack,HAND_ON_CMD,0));
}
/*********************************************************
*Function:       SScrn_Version_Send()
*Description:     ������PC�����Ͱ汾��

*Calls:          
*Called By:       
*Input:          VersionStr:�汾���ַ���
				  StrLen:�ַ�������
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
*Others:
*********************************************************/
u8 SScrn_Version_Send(u8* VersionStr,u8 StrLen)
{
	if((CheckParam1(VersionStr))||(StrLen>MAX_MSG_LEN))
	{
		SSCRN_Print("��PC���浱ǰ�汾�Ŵ���,�����������\r\n");	
		return 1;
	}
	SSCRN_Print("��PC���浱ǰ�汾��\r\n");
	return(Host_Send_Packag(UPDATA_VERSION_UP_CMD,VersionStr,StrLen));
}
/*********************************************************
*Function:       SScrn_Data_Req()
*Description:    ������PC����������

*Calls:          
*Called By:       
*Input:         PacketNum:��������
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
*Others:
*********************************************************/
u8 SScrn_Data_Req(u32 PacketNum) 
{
//	u8 buf[5];
	SSCRN_Print("��PC������������\r\n");
	return(Host_Send_Packag(UPDATA_REQ_DATA_CMD,(u8 *)&PacketNum,4));
}  
/*********************************************************
*Function:       SScrn_GroupNum_Send(u8* Name,u8 NameLen,u8* TelNum,u8 TelLen)
*Description:     �������ؼ��ź���

*Calls:          
*Called By:       
*Input:			   Name:�������� GB2312��
				   NameLen:���Ƴ���
				   TelNum:����
				   TelLen:���볤��
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
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
		SSCRN_Print("����������ͼ��ź������,�������\r\n");
		return 1;
	}
	SSCRN_Print("����������ͼ��ź���\r\n");
	Buf[0]=0x04;
	memcpy(&Buf[1],Name,NameLen);
	Buf[9]=0x0b;
	memcpy(&Buf[10],TelNum,TelLen);
	res=Host_Send_Packag(GROUP_NUM_SEND_CMD,Buf,30);
	if(res)
	{
		SSCRN_Print("����������ͼ��ź������\r\n");
	}
	else
	{
		SSCRN_Print("����������ͼ��ź���ɹ�\r\n");
	}
	return res;
}
/*********************************************************
*Function:       SScrn_GroupNum_Clr(void)
*Description:     ������ؼ��ź���

*Calls:          
*Called By:       
*Input:			   
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
*Others:		
*********************************************************/
u8 SScrn_GroupNum_Clr(void)	 
{
	u8 buf[5];
	SSCRN_Print("��ռ��ź���\r\n");
	return(Ack_Package(buf,Normal_Ack,GROUP_NUM_CLR_CMD,0));
}
/*********************************************************
*Function:       SScrn_Status_Send(STRUCT_RMC *pGpsInfo,STRUCT_HOST_STATE * Host_State)
*Description:     ���������������״̬

*Calls:          
*Called By:       
*Input: 		time:ϵͳʱ�� 6�ֽ� ѹ��bcd��
				pGpsInfo:gps��Ϣ
				HostType:�������� λb0��1 GPRS��ͨ��0 GPRSû��ͨ
									λb2λb1�������ͣ�00  GPRS�͡�01 GSM/GPRS�͡�10����
									λb6λb5λb4�������ͣ� 000 ���������С�001 �й��ƶ���010 �й���ͨ��110 �����ԭ�ַ�
									λb7 1 ����
				OverSpeed:��������
				CSQ:�ź�ǿ�� ��Χ:0x00-0x1f
				ACC_State:״̬: 0:�ѵ�� 1δ���
				DriveState:����״̬
				TCP_UDP:  '1' ��ʾ��ǰΪTCP���ӣ�'0'��ʾUDP����
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
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
		SSCRN_Print("���������������״̬����,�����������\r\n"); 
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
	if(pGpsInfo->North_Indicator=='S') //��γ��0x80��
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
	if(pGpsInfo->East_Indicator=='W') //��γ��0x80��
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
	//����״̬
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
	*P++=Host_State->TCP_UDP;//UDP��TCPѡ��
	*P++=0x08; //unknown
	memcpy(P,"abcdefgh",8);
	P+=8;
	Len+=21;
	Buf[2]=Len-3+2;
	temp=Protocol_Check(Buf,Len);
	*P++=temp;
	*P=0x0a;
	Len+=2;

//	SSCRN_Print("���������������״̬");
//	OSSemPend(Schedul_UsartSendSem,0,&temp);
	SchedulScrn_SendStr(Buf,Len);
//	OSSemPost(Schedul_UsartSendSem);
	return 0;
}

/*********************************************************
*Function:       Ack_Package()
*Description:    �����ݰ��������
*Calls:
*Called By:
*Input:            ��
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
		//if((Cmd==ACK_CMD))//������͵���Ӧ��������,�򲻵ȴ�
		if (OA_TRUE == ret){
			SSCRN_Print("����������ͳɹ�");
			flag=0;
			break;
		}
#if 0
		OSSemPend(SchedulAckSem,SEND_DATA_INTVL,&err);	//�ȴ�300msʱ��
		if(err!=OS_ERR_NONE)
		{
			SSCRN_Print("�����������ʧ��,����:%d\r\n",3-cnt);
			flag=1;

		}
		else
		{
			SSCRN_Print("����������ͳɹ�!\r\n");
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
*Description:	��������˿ڷ����ַ���
*Calls:			USART_SendData() �����ַ�����
*				USART_GetFlagStatus() ���ڱ�־
*Called By:		��
*Input:			Ҫ���͵��ַ���������
*Output:		��
*Return:		���ͳɹ����
*Others:		SchedulIOMutex ���ڱ���
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
*Description:	����У�����
*Calls:			
*Called By:		��
*Input:			Ҫ���͵��ַ���������
*Output:		��
*Return:		����ɹ�
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
*Description:	����У�����
*Calls:			
*Called By:		��
*Input:			Ҫ���͵��ַ���������
*Output:		��
*Return:		����ɹ�
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
*Description:    �������ֱ��������ݰ�
*Calls:          
*Called By:      ��
*Input:           Cmd  ָ����
				Value  ����ָ��
			  ValuLen  ���ݳ���
*Output:         
*Return:         
*Others:		 ���ŷ�������δ��
*********************************************************/
 u8 Host_Send_Packag(u8 Cmd, u8 *Value,const u8 ValuLen)
{
//	u8 *Pstr = (u8 *)Value;
	u8 pbuf_head[3];		//3�ֽ����飬���  ��ͷ ָ���� ����
	u8 pbuf_Tail[2];		//2�ֽ����飬��� У��ֵ ������
	u8 err;
	u8 cnt=3;
	u8 result=0;

	pbuf_head[0]=0x24;	//��ͷ
	pbuf_head[1]=Cmd;    //ָ����
	if(ValuLen>0)
	{
		pbuf_head[2]=ValuLen+2;			   //д������
	}
	else
	{
		pbuf_head[2]=2;					   //�հ�
	}								  //ָ�����ݰ����һ���ֽڵ���һ���ֽڴ�
	pbuf_Tail[0]=Protocol_Check_2para(pbuf_head,3,Value,ValuLen);  //У��������  �ֱ�Ӱ�ͷ  �����ݽ���У��
	pbuf_Tail[1]=0x0a;				//������ 


//	OSSemPend(Schedul_UsartSendSem,0,&err);
	while(cnt--)
	{
		SchedulScrn_SendStr(pbuf_head,3);				//�ȷ���ͷ
		SchedulScrn_SendStr(Value,ValuLen);			  //�ٷ�����
		SchedulScrn_SendStr(pbuf_Tail,2);				//��󷢰�β
		#if 0
		OSSemPend(SchedulAckSem,SEND_DATA_INTVL*2,&err);	//�ȴ�300msʱ��
		if(err!=OS_ERR_NONE)
		{
			result=1;
			SSCRN_Print("�����������ʧ��,����:%d\r\n",3-cnt);
		}
		#endif
		
		//SSCRN_Print("(%s %s %d):����������ͳɹ�", __FILE__, __func__, __LINE__);
	}
	result=0;
//	OSSemPost(Schedul_UsartSendSem);
	return result;
}
#if PROTCL_HX54
/*********************************************************
*Function:       u8 SScrn_GetStationData(u8 *Direction,u8 * total_num,u8 *sta_num,u8 *sta_type,FP32 * LAT,FP32 *LON)
*Description:     ��ȡվ����Ϣ����

*Calls:          
*Called By:      
*Input:          ��
*Output:        RoadNum      ��·��  4�ֽ�
				Direction	 ������Ϣ  0Ϊ���� 1Ϊ����
				total_num	 �ܹ�վ���� 
				sta_num		 վ���� ��0��ʼ
				sta_type	 վ������ 	 ���վ��0x01
										 �м�ͣ��վ��0x02
										 �յ�վ��0x03
										 ����վ��0x04
										 ͣ��վ��0x05
										 ��ͣ��վ���м�㣺0x06
										 ������0

				LAT			 γ����Ϣ�������ȸ���
				LON			 ������Ϣ�������ȸ���
*Return:        0��ʾ���ͳɹ� 1:����ʧ��
*Others:
*********************************************************/
u8 SScrn_GetStationData(u8 *RoadNum,u8 *Direction,u8 * total_num,u8 *sta_num,u8 *sta_type,FP32 * LAT,FP32 *LON)
{
	u8 datalen=0;
	u8 err;
	u8 result=0;	

	if(CheckParam4(RoadNum,Direction,sta_num,sta_type)||CheckParam3(sta_type,LAT,LON))
	{
		SSCRN_Print("��ȡ��������,��������!\r\n");
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
*Description:    ������PC����������

*Calls:          
*Called By:       
*Input:         PacketNum:��������
*Output:        ��
*Return:        0:��ʾ���ͳɹ�(���յ�������/�ֱ�Ӧ��Ϊ׼)1:����ʧ��
*Others:
*********************************************************/
u8 SScrn_StationDownloadStart_Rsq(void) 
{
	u8 buf[5];
	SSCRN_Print("��ʼ������·��Ϣ����Ӧ��\r\n");
	return(Ack_Package(buf,Normal_Ack,STATION_DOWNLOAD_START_RSP_CMD,0));
}
u8 SScrn_StationData_Rsq(void) 
{
	u8 buf[5];
	SSCRN_Print("�����������վ����Ϣ�������\r\n");
	return(Ack_Package(buf,Normal_Ack,STATION_DOWNLOAD_RSP_CMD,0));
}

#endif
