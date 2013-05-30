//#include "includes.h"
#include "oa_type.h"
#include "oa_api.h"
#include "oa_gps.h"
#include "oa_jt808.h"
#include "oa_platform.h"
#include "oa_hw.h"
#include "app_conf.h"
#include "SchedulScrn.h"
#include "oa_lbs2mtk.h"
#include "oa_debug.h"
extern STRUCT_RMC Pos_Inf;
extern DEV_PLAT_PARAS dev_running;
extern oa_uint8 acc_status;
extern STRUCT_SScrn_Result SScrn_Result;
#if 0
#ifdef EVDO_USE
extern u32 SYS_CHANNEL;  //EVDO分配给监控平台的socket号
#endif
/*升级登录请求应答，数据包请求应答*/ 
//extern OS_EVENT *Upgrade_AckMbox;
extern u8 gGruptelFlag; //集团号码设置 0空闲 1下载 2清除，动作后设为空闲
//static OS_EVENT *ScrnHtbeatAck_Sem; //n990心跳
//static OS_EVENT *SchedulAckSem; //调度屏协议应答
//OS_EVENT *DriverlogSem;//平台对司机签到签退应答通知

//OS_FLAG_GRP *Alarm_Driver_Event = 0;//通知驾驶员事件


//串口升级用
//OS_EVENT *UpdataRcvSem=0;
u8 gComUpflag=0; //com升级标志，从设备收到升级请求到升级完成有效为1

//OS_EVENT *Lineflash_Protct_Sem=0;
typedef struct{
u32 AllPacket; //总包数
u32 AllSize;   //升级包大小
u32 CurPacket; //当前包号
u8  Packetsize;
}STRT_ComUpdata;
static STRT_ComUpdata ComUpdata;

u8 gOrderInOperatflag=0; //0无效/空闲 1订单下发，2已抢单 3抢单成功
u8 gOrderInOperat[50]; //抢单的订单ID
u8 gLastOrder[50]; //平台下发订单ID,下发给屏时转换成1
/********************************************************
*Description:	串口升级标志，SET时进行串口升级	 ???
********************************************************/
//static u8 ComupdateFlag;
#endif
typedef struct StkSchedulHandle
{
	u8 ScrnType; //调度屏类型，对应Perpher_Define的SchedulScrn,LCDN990
//	u8 ScrnSta; //调度屏状态
	u8 Cmdtype;	 //下发协议命令字
	u8 DataBuf[255]; //数据包内容
	u8 DataBuf2[255]; //数据包内容
	u8 len;
	u8 len2;
	u16 len3;
	u8 TelNum[33];
	u8 U8Temp;
	u8 Status;
	u16 U16Temp;
	u16 DevAct;
	u32 U32Temp;
}Stk_Schedul_Handle;
typedef struct StkSchedulSendHandle
{
	u8 U8Temp;
	u8 U8len;
	u8 DataBuf[100];
	u16 U16Temp;
	u16 TxTim_Cnt;	//调度屏GPS更新用计数
	u32 U32Temp;
	STRUCT_RMC GpsInfo;
}Stk_SchedulSend_Handle;

//extern void SetUpgradStarttimeS(void); //设置升级计时开始时间
//* 函数定义
static void app_SScrnRcvtaskinit(Stk_Schedul_Handle *pSchedulScrnHandle);
static void app_SScrnRcvtaskExcute(Stk_Schedul_Handle *pSchedulScrnHandle);
static u8 app_SScrnSendtaskInit(Stk_SchedulSend_Handle *pSScrnSendHandle);
static void app_SScrnSendtaskExecut(Stk_SchedulSend_Handle *pSchedulScrnHandle);

void App_TaskSScrnRcvManage(void *Para)
{
	static oa_bool scrn_init = OA_FALSE;
	static Stk_Schedul_Handle SchedulScrnHandle;
	static Stk_Schedul_Handle *pSchedulScrnHandle = &SchedulScrnHandle;

//	if (OA_FALSE == scrn_init){
//		app_SScrnRcvtaskinit(pSchedulScrnHandle);
//		scrn_init = OA_TRUE;
//	}
	Mem_Set(pSchedulScrnHandle, 0, sizeof( Stk_Schedul_Handle));
	pSchedulScrnHandle->ScrnType = SchedulScrn;
	oa_memset(&SScrn_Result, 0, sizeof(SScrn_Result));
	
	app_SScrnRcvtaskExcute(pSchedulScrnHandle);
}

void App_TaskSScrnSendManage(void *Para)
{
	static oa_bool scrn_s_init = OA_FALSE;
	static Stk_SchedulSend_Handle SScrnSendHandle;
	
	static Stk_SchedulSend_Handle *pSScrnSendHandle = &SScrnSendHandle;
	
	if (OA_FALSE == scrn_s_init){
		app_SScrnSendtaskInit(pSScrnSendHandle);
		scrn_s_init = OA_TRUE;
	}	
	
	app_SScrnSendtaskExecut(pSScrnSendHandle);
	
	oa_timer_start(OA_TIMER_ID_8, App_TaskSScrnSendManage, NULL, SCHD_SCRN_TIME);
}
u8 SMS_EnglishUniToGBK(u8 *SrcStr,u8 Srclen,u8 *DesStr,u8 *Deslen)
{
	u8 *pstr=  SrcStr;
	u8 *pdstr=DesStr;
	u8 i;
	if(SrcStr==NULL||DesStr==NULL||Deslen==NULL)
		return ActionError;
	if(Srclen%2!=0)
		return ActionError;
	for(i=0;i<Srclen;i+=2)
	{
		if(0!=*(SrcStr+i))
			return ActionError;
	}
	*Deslen=Srclen/2;
	for(i=0;i<*Deslen;i++)
	{
		*(pdstr+i)=*(pstr+1);
		pstr+=2;
	}
	return ActionOK;
}
u8 SMS_EnglishGBKToUni(u8 *SrcStr,u8 Srclen,u8 *DesStr,u8 *Deslen)
{
	u8 i;
	u8 *pstr=  SrcStr;
	u8 *pdstr=DesStr;
	if(SrcStr==NULL||DesStr==NULL||Deslen==NULL)
		return ActionError;
	*Deslen=Srclen*2;
	for(i=0;i<Srclen;i++)
	{
		*pdstr=0;
		*(pdstr+1)=*pstr;
		pdstr+=2;
		pstr++;
	}
	return ActionOK;
}
/*********************************************************
*Function:       SchedulScrnSemInit()
*Description:    信号量初始化
*Calls:
*Called By:      GPS_Init
*Input:          无
*Output:         无
*Return:         无
*Others:
*********************************************************/
u8 SchedulScrnSemInit()
{
//	SchedulAckSem = OSSemCreate(0);
//	Lineflash_Protct_Sem= OSSemCreate(0);
	return ActionOK;
}
/*********************************************************
*Function:       app_CentersmsAckAction
*Description:    中心短信辅助参数设置后，终端后续动作
*Calls:          无
*Called By:      无
*Input:          Action	终端动作
				 Sendbuf，buflen 短信回复内容
*Output:         
*Return:         
*Others:
*********************************************************/
void app_CentersmsAckAction(u16 Action,u8 *Sendbuf, u8 buflen)
{
	//if(Action&Sms_Ack_Enable)
	{ //调度屏常短信回复 peijl_2010808
//		OSTimeDly(30); //等待连续两次向调度屏发送时间间隔
		DEBUG("SchedulScrn_Task: Send reply message to schedul screen.");
		if(Action&CHINESE_SMS_ENABLE)  //中文短信
		{
			u8 time[6]={0};
		#if SCREEN_N990>0
			SScrn_SMS_Send('C',"####",4,time,Sendbuf,buflen);
		#else
			if(ActionOK!=SScrn_SMS_Send(TELSMS_PDU_CMD,"0",1,time,Sendbuf,buflen))
				DEBUG("SchedulScrn_Task:Send Unicode Ack-Sms failed!");
			else
				DEBUG("SchedulScrn_Task:Send Unicode Ack-Sms message successfully.");		
		#endif
		}
		else
		{
		#if SCREEN_N990>0
			u8 time[6]={0};
			SScrn_SMS_Send('E',"####",4,time,Sendbuf,buflen);
		#else
			if(ActionOK!=SScrn_CenterSMS_Send(Sendbuf,buflen))
				DEBUG("SchedulScrn_Task:Send Ack-Sms failed!");		
			else
				DEBUG("SchedulScrn_Task:Send Ascall Ack-Sms message successfully.");		
		#endif
		}
	}
#if 0
	if(Action&LBS_Reset_Enable)
	{
		DEBUG(PrintInfo,"SchedulScrn_Task:need to reset LBS.");
		WriteLinkTaskPara(ActionResetLbs,ActionTypePara,SET);						
	}
	if(Action&WCM_Reset_Enable)
	{
		DEBUG(PrintInfo,"SchedulScrn_Task: need to reset Wcm model.");
		WriteLinkTaskPara(ActionResetWCM,ActionTypePara,SET);	
		WriteLinkTaskPara(0xFFFF,NetStatusPara,RESET);	
	}
	if(Action&HX_ReLogin_Enable)
	{
		u16 temp=0;
		DEBUG(PrintInfo,"SchedulScrn_Task:need reLogin MONITER server.");
		if(Action&DEVIC_UNREGIST_Enable) //peijl_130220 添加：车辆基本参数修改时从平台注销
		{
			WriteLinkTaskPara(ActionUnregistServ|ActionLoginHX,ActionTypePara,SET);
		}
		else
		{
			ReadLinkTaskPara(&temp,NetStatusPara);
			if(temp&LinkHXSeverOk)
				WCM_IPCloseLink(SYS_CHANNEL);
			WriteLinkTaskPara(LinkHXSeverOk,NetStatusPara,RESET);
			WriteLinkTaskPara(ActionLoginHX,ActionTypePara,SET);	
		}
	}
	if(Action&HX_PPP_Enable)
	{
		u16 temp=0;
		DEBUG(PrintInfo,"SchedulScrn_Task: need PPP redial.");
		ReadLinkTaskPara(&temp,NetStatusPara);
		if(temp&HXPPPOk)
			WCM_ClosePPPLink(); //升级冲突否	
		WriteLinkTaskPara(HXPPPOk,NetStatusPara,RESET);	
		WriteLinkTaskPara(ActionHXPPP|ActionLoginHX,ActionTypePara,SET);	
	}
	if(Action&TL_Update_Enable)
	{						
	//	if(0==Get_Upgradetype())
		{
			DEBUG(PrintDebug,"SchedulScrn_Task:Com Upgrade Cmd");
			SetUpgradStarttimeS(); //设置升级计时开始时间
			WriteLinkTaskPara(ActionUpdateTL,ActionTypePara,SET);
		//	Set_Upgradetype(2); //???
		}
	}
#endif
}
#ifdef PROTCL_HX54
/*********************************************************
*Function:       app_SScrndriverSign
*Description:    中心短信辅助参数设置后，终端后续动作
*Calls:          无
*Called By:      无
*Input:          Action	终端动作
				 Sendbuf，buflen 短信回复内容
*Output:         
*Return:         
*Others:
*********************************************************/
u8 app_SScrndriverSign(u8 *DataBuf,const u8 buflen)
{
	u8 U8Temp;
	u8 len;
	u16 U16Temp;
	DriversigninfoStrct Signinfo;
	STRUCT_RMC gps_info;
	if(!SScrn_GetResult(&U8Temp,1,&len,DRIVER_SIGN_CMD,SIGN_FLAG_ENUM))
	{	if(!SScrn_GetResult((u8 *)&U16Temp,2,&len,DRIVER_SIGN_CMD,DRIVER_ID_ENUM))
		{
			u8 cnt=0;
			u16 t=10000;
			//取司机ID
			Mem_Set(Signinfo.drverID,0x00,sizeof(Signinfo.drverID));		
			if(U16Temp!=0)
			{
				len=5;//最大5位数			
				while(1)
				{
					if(U16Temp/t!=0)
						break;
					else
					{
						t=t/10;
						len--;
					}
				}
				cnt=0;
				while(len>1)
				{
					Signinfo.drverID[cnt++]=U16Temp/t+'0';
					U16Temp=U16Temp%t;
					t=t/10;
					len--;
				}
				Signinfo.drverID[cnt]=U16Temp%10+'0';
			}
			DEBUG(PrintError,"SchedulScrn_Task:Driver ID  %s!",Signinfo.drverID);
			//司机登录包其他信息
			app_GetGPSStrct(&gps_info);
			ReadDistanceData(Signinfo.mileag);
			ReadAllBitAlarmPara(StaAlarm0,&Signinfo.staflag);
			Mem_Copy(Signinfo.time,gps_info.Time,6);
			Mem_Copy(Signinfo.lon,gps_info.Longitude,9); //东西判断否
			Mem_Copy(Signinfo.lat,gps_info.Latitude,8);  //南北
			Signinfo.speed=(u8)gps_info.Speed;
			Signinfo.cog=gps_info.COG;
			if(U8Temp==SIGN_IN) //登签
				Control_Package(DataBuf,&U16Temp,DriverLogin,(u8 *)&Signinfo,sizeof(DriversigninfoStrct));
			else if(U8Temp==SIGN_OUT) //退签
				Control_Package(DataBuf,&U16Temp,DriverLogout,(u8 *)&Signinfo,sizeof(DriversigninfoStrct));
			
			if(U16Temp>buflen)
			{
			DEBUG(PrintError,"SchedulScrn_Task:When uploading driver-sign infomation,buffer isn't enough!");
				U8Temp=ActionError;			
			}
			else
			{
				U8Temp=HXPackageSend(DataBuf,U16Temp);
				if(ActionOK!=U8Temp)
				{
					WriteflashPack(eDriverSignData,DataBuf,U16Temp);
					DEBUG(PrintDebug,"SchedulScrn_Task:saved Driver signed infomation to flash.");
				}
				else
				{
					OSSemPend(DriverlogSem,3000,&U8Temp);
					if(U8Temp==OS_ERR_NONE)
						DEBUG(PrintDebug,"SchedulScrn_Task:Driver signed successfully!");
					else
					{
						WriteflashPack(eDriverSignData,DataBuf,U16Temp);
						DEBUG(PrintDebug,"SchedulScrn_Task:saved Driver signed infomation to flash.");
						U8Temp=ActionError;
					}
				}
			}
		}
		else
			U8Temp=ActionError;
	}
	else
		U8Temp=ActionError;
	return U8Temp;
}
#endif
static u8 app_ScrnRcvData_Analy(u8 ScrnType) 
{
//	if(ScrnType==LCDN990)
      if(0)
		return LCDN990_RcvDataAnaly();	
	else if(ScrnType==SchedulScrn)
		return SScrn_DataAnaly();
	else
		return 0;

}
/*********************************************************
*Function:       app_SScrnRcvtaskinit
*Description:    初始化任务句柄和所使用的全局变量
*Calls:          无
*Called By:      无
*Input:          *Stk_Schedul_Handle 任务句柄指针
*Output:         
*Return:         
*Others:
*********************************************************/
static void app_SScrnRcvtaskinit(Stk_Schedul_Handle *pSchedulScrnHandle)
{
	Mem_Set(pSchedulScrnHandle,0,sizeof( Stk_Schedul_Handle));
	/*取对应的外设*/
	//ReadPerpherCfg(ePara3,PerpherKindMax,(u8 *)&pSchedulScrnHandle->U32Temp);
	pSchedulScrnHandle->U32Temp = 1<<SchedulScrn;
	if(pSchedulScrnHandle->U32Temp&(1<<SchedulScrn))
	{
		pSchedulScrnHandle->ScrnType=SchedulScrn;
		SScrn_SW_Init();
	}
#if 0
	else if(pSchedulScrnHandle->U32Temp&(1<<LCDN990))
	{
		pSchedulScrnHandle->ScrnType=LCDN990;
		LCDN990_SW_Init();
	}
	else
		pSchedulScrnHandle->ScrnType=0;
	DriverlogSem=OSSemCreate(0);//调度屏司机签到签退应答通知
	UpdataRcvSem=OSSemCreate(0);
#endif
}
/*********************************************************
*Function:       SchedulTaskExecute
*Description:    任务执行
*Calls:          无
*Called By:      无
*Input:          *pSchedulScrnHandle 调用函数的任务句柄指针
*Output:         pSchedulScrnHandle
*Return:         ReturnType类型
*Others:		 
*********************************************************/
static void app_SScrnRcvtaskExcute(Stk_Schedul_Handle *pSchedulScrnHandle)
{

	//串口数据处理
	pSchedulScrnHandle->Cmdtype = app_ScrnRcvData_Analy(pSchedulScrnHandle->ScrnType);

	if (pSchedulScrnHandle->Cmdtype != SCCRN_UNEXP_CMD 
		&& pSchedulScrnHandle->Cmdtype != SCCRN_TIME_OUT
		&& pSchedulScrnHandle->Cmdtype != SCCRN_ERROR_RETURN)
		DEBUG("SchedulScrn_Task:sscreen protocal cmd: 0x%02x", pSchedulScrnHandle->Cmdtype);
	else		return;
	
	if (pSchedulScrnHandle->ScrnType == SchedulScrn){
		switch (pSchedulScrnHandle->Cmdtype){
		case CENTER_SMS_CMD: //调度屏发出中心短信为uni，接收为gbk；手柄均为gbk
			SScrn_SMSResult_Send(0);//ack to sched_scrn
			SScrn_GetResult(pSchedulScrnHandle->DataBuf, 
							sizeof(pSchedulScrnHandle->DataBuf), 
							&pSchedulScrnHandle->len, 
							CENTER_SMS_CMD,0);//copy data
			pSchedulScrnHandle->DataBuf[pSchedulScrnHandle->len]='\0';
			//调度屏时将英文unicode短信转换成gbk
			if (pSchedulScrnHandle->len>1 && pSchedulScrnHandle->DataBuf[0]==0){	
				if (!SMS_EnglishUniToGBK(pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len,
									pSchedulScrnHandle->DataBuf2,&pSchedulScrnHandle->len2)){
					Mem_Copy(pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2);
					pSchedulScrnHandle->len=pSchedulScrnHandle->len2;
				}
				else
					pSchedulScrnHandle->len=0;
			}
			pSchedulScrnHandle->DevAct=0; //peijl_130311 添加：初始值为0
			//here is the transplant point
			pSchedulScrnHandle->Status = /*app_LbsAscSmsHandl(pSchedulScrnHandle->DataBuf,
															pSchedulScrnHandle->len,
															&pSchedulScrnHandle->DevAct,
															pSchedulScrnHandle->DataBuf2,
															&pSchedulScrnHandle->len3);*/
										 sched_scrn_ana_4trans(	pSchedulScrnHandle->DataBuf,
															     	pSchedulScrnHandle->len,
																&pSchedulScrnHandle->DevAct,
																pSchedulScrnHandle->DataBuf2,
																&pSchedulScrnHandle->len3);
			if (pSchedulScrnHandle->Status == UnDefinedSms){//非辅助短信
				//OSTimeDly(30);
				pSchedulScrnHandle->len3 = strlen("非辅助短信！");
				Mem_Copy(pSchedulScrnHandle->DataBuf2, "非辅助短信！", pSchedulScrnHandle->len3);
				if (ActionOK != SScrn_CenterSMS_Send(pSchedulScrnHandle->DataBuf2, pSchedulScrnHandle->len3))
					DEBUG("SchedulScrn_Task:Send Ack-Sms failed", __FILE__, __func__, __LINE__);
			}
			else if(pSchedulScrnHandle->Status == ActionOK)//正确解析
			{
		/*		if(pSchedulScrnHandle->ScrnUniCenterSMS==1) //还原调度屏uni短信
				{
					pSchedulScrnHandle->ScrnUniCenterSMS=0;
					if(!SMS_EnglishGBKToUni(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2,pSchedulScrnHandle->DataBuf,&pSchedulScrnHandle->len))
					{
					Mem_Copy(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len);
					pSchedulScrnHandle->len2=pSchedulScrnHandle->len;
					}
				}	  */
				if(!(pSchedulScrnHandle->DevAct&Sms_Ack_Force_DISABLE))
				{
					//OSTimeDly(300);
					app_CentersmsAckAction(pSchedulScrnHandle->DevAct,pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len3);
				}
			}
		break;
		#if 0
		case UNICODE_SMS_CMD:
			SScrn_SMSResult_Send(0);
			SScrn_GetResult(pSchedulScrnHandle->TelNum,sizeof(pSchedulScrnHandle->TelNum),&pSchedulScrnHandle->len,UNICODE_SMS_CMD,TEL_NUM_ENUM);
			SScrn_GetResult(pSchedulScrnHandle->DataBuf2,sizeof(pSchedulScrnHandle->DataBuf2),&pSchedulScrnHandle->len2,UNICODE_SMS_CMD,SMS_DAT_ENUM);
			if(WCM_OK!=WCM_SendSMS(UNICOD_SMS,pSchedulScrnHandle->TelNum,pSchedulScrnHandle->len,pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2))
				DEBUG(PrintDebug,"SchedulScrn_Task:WCM send message failed");
		break;
		
		case SET_UDP_CMD:
			SScrn_Result_Send(0);
			if(!SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,SET_UDP_CMD,IP_ENUM))
				if(!SScrn_GetResult((u8 *)&pSchedulScrnHandle->U16Temp,2,&pSchedulScrnHandle->len2,SET_UDP_CMD,PORT_ENUM))
			{
				pSchedulScrnHandle->DataBuf[pSchedulScrnHandle->len]='\0';
				DEBUG(PrintInfo,"SchedulScrn_Task:set IP and udp port");
				DEBUG(PrintDebug,"IP  :%s",pSchedulScrnHandle->DataBuf);
				DEBUG(PrintDebug,"PORT:%d",pSchedulScrnHandle->U16Temp);

				ReadLbsCfgPara(eIP,pSchedulScrnHandle->DataBuf2,&pSchedulScrnHandle->U8Temp);
				if((pSchedulScrnHandle->U8Temp!=pSchedulScrnHandle->len) ||0!=(memcmp(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len)))
				{//IP改变 删除注册信息
					pSchedulScrnHandle->U32Temp=pSchedulScrnHandle->U16Temp;
					WriteLbsCfgPara(eIP,pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len,UpdateOnly);
					WriteLbsCfgPara(eUdpPort,(u8 *)&(pSchedulScrnHandle->U32Temp),4,UpdateFlash);

					ReadLinkTaskPara(&pSchedulScrnHandle->U16Temp,NetStatusPara);
					if(pSchedulScrnHandle->U16Temp&LinkHXSeverOk)
						WCM_IPCloseLink(SYS_CHANNEL);
					WriteLinkTaskPara(LinkHXSeverOk,NetStatusPara,RESET);
					WriteLinkTaskPara(ActionLoginHX,ActionTypePara,SET);	
					DEBUG(PrintInfo,"SchedulScrn_Task:ReLogin Moniter server.");
				}
				else
				{
					ReadLbsCfgPara(eNET,&pSchedulScrnHandle->len2,&pSchedulScrnHandle->U8Temp);
					if(pSchedulScrnHandle->len2==eNET_UDP)
					{
						ReadLbsCfgPara(eUdpPort,(u8 *)&pSchedulScrnHandle->U32Temp,&pSchedulScrnHandle->U8Temp);
						if(pSchedulScrnHandle->U32Temp!=(u32)pSchedulScrnHandle->U16Temp)
						{ //端口号改变，则该平台注册状态删除（未注销）
						pSchedulScrnHandle->U32Temp=pSchedulScrnHandle->U16Temp;
						WriteLbsCfgPara(eUdpPort,(u8 *)&pSchedulScrnHandle->U32Temp,4,UpdateFlash);
						ReadLinkTaskPara(&pSchedulScrnHandle->U16Temp,NetStatusPara);
						if(pSchedulScrnHandle->U16Temp&LinkHXSeverOk)
							WCM_IPCloseLink(SYS_CHANNEL);
						WriteLinkTaskPara(LinkHXSeverOk,NetStatusPara,RESET);
						WriteLinkTaskPara(ActionLoginHX,ActionTypePara,SET);	
						DEBUG(PrintInfo,"SchedulScrn_Task:ReLogin Moniter server.");
						}				
					}
				}
			}
		break;
		#endif
		case SET_LOCAL_TEL_CMD:
			SScrn_Result_Send(0);
			if(!SScrn_GetResult(pSchedulScrnHandle->TelNum, sizeof(pSchedulScrnHandle->TelNum), 
												&pSchedulScrnHandle->len, SET_LOCAL_TEL_CMD,0)){
				pSchedulScrnHandle->TelNum[pSchedulScrnHandle->len]='\0';
				DEBUG("SchedulScrn_Task:");
				DEBUG("Set local tel %s", __FILE__, __func__, __LINE__, pSchedulScrnHandle->TelNum);
				
				ReadLbsCfgPara(eLclTEL,pSchedulScrnHandle->DataBuf2,&pSchedulScrnHandle->U8Temp);
				if((pSchedulScrnHandle->U8Temp==pSchedulScrnHandle->len)&&(memcmp(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->TelNum,pSchedulScrnHandle->len)==0))
				{ //号码未变
				}
				else
				{
					WriteLbsCfgPara(eLclTEL,pSchedulScrnHandle->TelNum,pSchedulScrnHandle->len,UpdateFlash);
	//			#ifdef EVDO_USE
	//					DVRLinkParaSet(NULL,0,NULL,pSchedulScrnHandle->TelNum,pSchedulScrnHandle->len);		
	//			#endif
	//					//更换设备后清盲区数据，统计参数，里程
	//					ClearflashPackg(eBlindPosData);
	//					ClearflashPackg(eDriverSignData);
	//					ClearflashPackg(eStaticData);
	//					ClearflashPackg(eDistanc);
	//					ReadLinkTaskPara(&pSchedulScrnHandle->U16Temp,NetStatusPara);
	//					if(pSchedulScrnHandle->U16Temp&LinkHXSeverOk)
	//						WCM_IPCloseLink(SYS_CHANNEL);
	//					WriteLinkTaskPara(LinkHXSeverOk,NetStatusPara,RESET);
	//					WriteLinkTaskPara(ActionLoginHX,ActionTypePara,SET);	
	//					DEBUG(PrintInfo,"SchedulScrn_Task:ReLogin Moniter server.");
				}
			}
		break;
#if 0
		case CALL_DIALUP_CMD:
			ReadTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
			if(pSchedulScrnHandle->U8Temp==NoneAction)
			{
				if(!SScrn_GetResult(pSchedulScrnHandle->TelNum,sizeof(pSchedulScrnHandle->TelNum)-1,&pSchedulScrnHandle->len,CALL_DIALUP_CMD,0))
				{
					pSchedulScrnHandle->TelNum[pSchedulScrnHandle->len]='\0';
					WriteTelPara(pSchedulScrnHandle->TelNum,eTelNumPara);
					pSchedulScrnHandle->U8Temp=0;
					WriteTelPara(&pSchedulScrnHandle->U8Temp,eTelTypePara);
					pSchedulScrnHandle->U8Temp=DailUp;
					WriteTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);

					if(OS_ERR_NONE!=OSTaskResume(CallTaskPrio))	//直到唤醒电话任务或挂断
					{
						SScrn_HandOff_Send(); //peijl_20120918 添加
						pSchedulScrnHandle->U8Temp=NoneAction;
						WriteTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
					}
					DEBUG(PrintInfo,"SchedulScrn_Task: Dial %s",pSchedulScrnHandle->TelNum);
				}
			}
			else //作为第三方电话打出或电话监听过程，禁止电话打出
			{				
				SScrn_HandOff_Send();
			}
		break;
		case CALL_HANGOFF_CMD:
			ReadTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
			pSchedulScrnHandle->U8Temp=DevHangUp;
			WriteTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
			DEBUG(PrintInfo,"SchedulScrn_Task: hang up!");
		break;
		case CALL_HANGON_CMD:
			ReadTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
			if(pSchedulScrnHandle->U8Temp==SchedulRcvCall)
			{
				pSchedulScrnHandle->U8Temp=HandOn;
				WriteTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
				DEBUG(PrintInfo,"SchedulScrn_Task: Pick up the phone");
			}
		break;
		case CALL_SPKSWITCH_CMD:
			WriteTelPara(0,eHandFree);
			DEBUG(PrintInfo,"SchedulScrn_Task: Set handfree-switch!");
		break;
		case ACK_CMD:
			OSSemPost(SchedulAckSem);			
		break;
		case SET_SPK_CMD:
			DEBUG(PrintInfo,"SchedulScrn_Task: Set SPK ");
			if(!SScrn_GetResult(&pSchedulScrnHandle->U8Temp,1,&pSchedulScrnHandle->len,SET_SPK_CMD,0))
			{
				WCM_SetSPKValue(pSchedulScrnHandle->U8Temp);
			}
		break;
		case SET_MIC_CMD:
			DEBUG(PrintInfo,"SchedulScrn_Task: Set MIC!");
			if(!SScrn_GetResult(&pSchedulScrnHandle->U8Temp,1,&pSchedulScrnHandle->len,SET_MIC_CMD,0))
			{
			#ifndef EVDO_USE
				WCM_SetMICValue(pSchedulScrnHandle->U8Temp);
			#endif
			}
		break;
#ifdef PROTCL_HX54
		case DRIVER_SIGN_CMD: //jt808未处理
			SScrn_Result_Send(0);
			pSchedulScrnHandle->U8Temp=app_SScrndriverSign(pSchedulScrnHandle->DataBuf2,sizeof(pSchedulScrnHandle->DataBuf2));
			if(pSchedulScrnHandle->U8Temp!=ActionOK)
				DEBUG(PrintDebug,"SchedulScrn_Task: Driver log error!");
		break;
#endif
#endif
#if 0
		case SET_TCP_CMD:
			SScrn_Result_Send(0);
			if(!SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,SET_TCP_CMD,IP_ENUM))
				if(!SScrn_GetResult((u8 *)&pSchedulScrnHandle->U16Temp,2,&pSchedulScrnHandle->len2,SET_TCP_CMD,PORT_ENUM))
			{
				pSchedulScrnHandle->DataBuf[pSchedulScrnHandle->len]='\0';
				DEBUG(PrintDebug,"SchedulScrn_Task:set IP and udp port");
				DEBUG(PrintDebug,"IP  :%s",pSchedulScrnHandle->DataBuf);
				DEBUG(PrintDebug,"PORT:%d",pSchedulScrnHandle->U16Temp);

				ReadLbsCfgPara(eIP,pSchedulScrnHandle->DataBuf2,&pSchedulScrnHandle->U8Temp);
				if((pSchedulScrnHandle->U8Temp!=pSchedulScrnHandle->len) ||0!=(memcmp(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len)))
				{//IP改变 删除注册信息
					pSchedulScrnHandle->U32Temp=pSchedulScrnHandle->U16Temp;
					WriteLbsCfgPara(eIP,pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len,UpdateOnly);
					WriteLbsCfgPara(eTcpPort,(u8 *)&(pSchedulScrnHandle->U32Temp),4,UpdateFlash);
					ReadLinkTaskPara(&pSchedulScrnHandle->U16Temp,NetStatusPara);
					if(pSchedulScrnHandle->U16Temp&LinkHXSeverOk)
						WCM_IPCloseLink(SYS_CHANNEL);
					WriteLinkTaskPara(LinkHXSeverOk,NetStatusPara,RESET);
					WriteLinkTaskPara(ActionLoginHX,ActionTypePara,SET);	
					DEBUG(PrintInfo,"SchedulScrn_Task:ReLogin Moniter server.");
				}
				else
				{
					ReadLbsCfgPara(eNET,&pSchedulScrnHandle->len2,&pSchedulScrnHandle->U8Temp);
					if(pSchedulScrnHandle->len2==eNET_TCP)
					{
						ReadLbsCfgPara(eTcpPort,(u8 *)&pSchedulScrnHandle->U32Temp,&pSchedulScrnHandle->U8Temp);
						if(pSchedulScrnHandle->U32Temp!=(u32)pSchedulScrnHandle->U16Temp)
						{ //端口号改变，则该平台注册状态删除（未注销）
						pSchedulScrnHandle->U32Temp=pSchedulScrnHandle->U16Temp;
						WriteLbsCfgPara(eTcpPort,(u8 *)&pSchedulScrnHandle->U32Temp,4,UpdateFlash);
						ReadLinkTaskPara(&pSchedulScrnHandle->U16Temp,NetStatusPara);
						if(pSchedulScrnHandle->U16Temp&LinkHXSeverOk)
							WCM_IPCloseLink(SYS_CHANNEL);
						WriteLinkTaskPara(LinkHXSeverOk,NetStatusPara,RESET);
						WriteLinkTaskPara(ActionLoginHX,ActionTypePara,SET);	
						DEBUG(PrintInfo,"SchedulScrn_Task:ReLogin Moniter server.");
						}				
					}
				}
			}
		break;
#endif
		case SCCRN_TIME_OUT:
		case SCCRN_ERROR_RETURN:
			//DEBUG(PrintInfo,"SScreen Rcvtask:No data");
		default:
		break;
			
#if 0
#if APTS_ENABLE>0
		case STATION_DOWNLOAD_START_CMD: //站点下载启动
			Clr_LineInfoInFlash();
			SScrn_CmdResult_Send(STATION_DOWNLOAD_START_RSP_CMD,Normal_Ack,0);
		break;
		case STATION_DOWNLOAD_CMD: //站点下载
			if(0==SScrn_GetResult(pSchedulScrnHandle->DataBuf2,sizeof(pSchedulScrnHandle->DataBuf2),&pSchedulScrnHandle->len2,STATION_DOWNLOAD_CMD,0))
			{
				u8 index=0;
				STRUCT_STATION_DATA *Pstr=(STRUCT_STATION_DATA *)pSchedulScrnHandle->DataBuf2;
				if(0==LineInfo_save2flash(Pstr->ROAD_NUM,Pstr->STA_DIRECTION,Pstr->STA_TOTAL_NUM,Pstr->ROAD_VER,Pstr->STA_DATALEN,Pstr->STA_DATA))
				{
					SScrn_CmdResult_Send(STATION_DOWNLOAD_RSP_CMD,Normal_Ack,0);
					DEBUG(PrintInfo,"SchedulScrn_Task:Line information was downloaded successfully.");
				}else  //缓存区满
				{	SScrn_CmdResult_Send(STATION_DOWNLOAD_RSP_CMD,Normal_Ack,1);
					DEBUG(PrintInfo,"SchedulScrn_Task:can't download Line information !!!!!");
				}
			}
			else
			SScrn_CmdResult_Send(STATION_DOWNLOAD_RSP_CMD,Normal_Ack,1);
		break;
#endif
		case SCR_SMS_DOWNLOAD_CMD://短信模板下载
			if(0==SScrn_GetResult(pSchedulScrnHandle->DataBuf2,sizeof(pSchedulScrnHandle->DataBuf2),&pSchedulScrnHandle->len2,SCR_SMS_DOWNLOAD_CMD,0))
			{
	//					u8 index=0;
				STRUCT_SCR_SMS_TEMPLET *Pstr=(STRUCT_SCR_SMS_TEMPLET *)pSchedulScrnHandle->DataBuf2;
				if(Pstr->SMS_LEN>0)
				{
					if((Pstr->STATE==0)&&(Pstr->F_OR_A==0))
					{
						Flash_Write_Buffer(0,0,DEBUS_SMS_DATA_ADDR,Sector_Erase);
					}
					Flash_Write_Buffer((u8*)Pstr->SMS_DATA,128,DEBUS_SMS_DATA_ADDR+Pstr->STATE*sizeof(SCR_SMS_TEMPLET_DATA)+Pstr->F_OR_A*129,Write_Mode);
					Flash_Write_Buffer((u8*)&Pstr->SMS_LEN,1,DEBUS_SMS_DATA_ADDR+Pstr->STATE*sizeof(SCR_SMS_TEMPLET_DATA)+Pstr->F_OR_A*129+1,Write_Mode);
					DEBUG(PrintInfo,"SchedulScrn_Task:config SCR_SMS_TEMPLET successfully!!!!!");
					SScrn_CmdResult_Send(SCR_SMS_DOWNLOAD_RSP_CMD,Normal_Ack,0);
				}
				else
				{
					SScrn_CmdResult_Send(SCR_SMS_DOWNLOAD_RSP_CMD,Normal_Ack,1);
					DEBUG(PrintInfo,"SchedulScrn_Task:config SCR_SMS_TEMPLET failed!!!!!");
				}
			}
			else
			{
				SScrn_CmdResult_Send(SCR_SMS_DOWNLOAD_RSP_CMD,Normal_Ack,1);
				DEBUG(PrintInfo,"SchedulScrn_Task:config SCR_SMS_TEMPLET failed!!!!!");
			}
		break;
		case UPDATA_REQ_CMD: //收到串口升级请求，下发当前版本信息
			DEBUG(PrintInfo,"Requry for version upgrading by com.");
			Flash_Read_Buffer(&pSchedulScrnHandle->U8Temp,1,DEV_HWVER_ADDR);
			Flash_Read_Buffer(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->U8Temp,DEV_HWVER_ADDR+1);
			memcpy(&pSchedulScrnHandle->DataBuf2[pSchedulScrnHandle->U8Temp],SOFTWARE_VER,9);
			memcpy(&pSchedulScrnHandle->DataBuf2[pSchedulScrnHandle->U8Temp+9],".bin",strlen(".bin"));
			pSchedulScrnHandle->len2=pSchedulScrnHandle->U8Temp+9+strlen(".bin");
			SScrn_Version_Send(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2); //
		break;
		case UPDATA_PARA_CMD: //升级版本信息
			if(0==SScrn_GetResult((u8 *)&ComUpdata.AllPacket,sizeof(ComUpdata.AllPacket),&pSchedulScrnHandle->len2,UPDATA_PARA_CMD,PACKETS_ENUM))
			{
			if(0==SScrn_GetResult((u8 *)&ComUpdata.AllSize,sizeof(ComUpdata.AllSize),&pSchedulScrnHandle->len,UPDATA_PARA_CMD,DATA_SIZE_ENUM))
				{
				DEBUG(PrintInfo,"收到串口升级版本信息");
				ComUpdata.CurPacket=0;
				ComUpdata.Packetsize=250;//定长
				gComUpflag=1;
				}
			}
		break;
		case UPDATA_DATA_CMD : //升级程序包
			if(0==SScrn_GetResult(pSchedulScrnHandle->DataBuf2,sizeof(pSchedulScrnHandle->DataBuf2),&pSchedulScrnHandle->len2,UPDATA_DATA_CMD,0))
			{
				static u32 addr;
				if(ComUpdata.CurPacket==0)//第一包时备份版本擦除
				{
					Flash_Read_Buffer((u8 *)&addr,4,OLD_CODE_STORE_ADDR);
					for(pSchedulScrnHandle->U8Temp=0;pSchedulScrnHandle->U8Temp<VER_SECTOR_NUM;pSchedulScrnHandle->U8Temp++)
						Flash_Write_Buffer(NULL,0,addr+SECTOR_SIZE*pSchedulScrnHandle->U8Temp,Sector_Erase);
				}
				//程序存flash
				Flash_Write_Buffer(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2,addr+4+ComUpdata.Packetsize*ComUpdata.CurPacket,Write_Mode);
				while(OSSemAccept(UpdataRcvSem)) ; //防止同一包两次信号通知
				OSSemPost(UpdataRcvSem);
			}
		break;
		default:
			OSTimeDly(5);
		//	DEBUG(PrintInfo,"SScreen Rcvtask:unknown command type 0x%2x",pSchedulScrnHandle->Cmdtype);
		break;
		
#endif
		}
	}
	}
#if 0
	else if(pSchedulScrnHandle->ScrnType==LCDN990)
	{
		switch(pSchedulScrnHandle->Cmdtype)
		{
	//			case N990_ONEBYTE_CMD:
	//				SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,N990_ONEBYTE_CMD,0);
	//				switch(pSchedulScrnHandle->DataBuf[0])
	//				{
	//				case HEARTBEAT_ACK_CMD:
	//					DEBUG(PrintDebug,"调度屏心跳应答-------------------------------");
	//					DEBUG(PrintDebug,"SchedulScrn_Task:Heartbeat response.");
	//					OSSemPost(ScrnHtbeatAck_Sem);
	//				break;
	//				case CALL_HANGON_CMD: //接电话
	//					ReadTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//					if(pSchedulScrnHandle->U8Temp==SchedulRcvCall)
	//					{
	//						pSchedulScrnHandle->U8Temp=HandOn;
	//						WriteTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//						DEBUG(PrintInfo,"SchedulScrn_Task: Pick up the phone");
	//					}
	//				break;
	//				case CALL_HANGOFF_CMD: //挂机
	//					ReadTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//					pSchedulScrnHandle->U8Temp=DevHangUp;
	//					WriteTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//					DEBUG(PrintInfo,"SchedulScrn_Task: hang up!");
	//				break;
	//				case GET_SMS_FORMAT_CMD: //查询短信格式
	//					DEBUG(PrintInfo,"SchedulScrn_Task: get SMS format of WCM.");
	//	
	//				break;
	//				case CALL_CENTER_CMD: //呼叫中心
	//				break;
	//				case SETSPK_UP_CMD: //加音量
	//				break;
	//				case SETSPK_DOWN_CMD: //减音量
	//				break;
	//				case GET_SPKVAL_CMD: //查询音量
	//				break;
	//				default:
	//				break;
	//				}
	//			break;
	//			case N990_SENDSMS_CMD: //uni
	//				if(0==SScrn_GetResult(pSchedulScrnHandle->TelNum,sizeof(pSchedulScrnHandle->TelNum),&pSchedulScrnHandle->len2,N990_SENDSMS_CMD,TEL_NUM_ENUM))
	//				if(0==SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,N990_SENDSMS_CMD,SMS_DAT_ENUM))
	//				{	u8 t=0;
	//					pSchedulScrnHandle->TelNum[pSchedulScrnHandle->len2]='\0';
	//					DEBUG(PrintInfo,"SchedulScrn_Task: 手机号-%s",pSchedulScrnHandle->TelNum);
	//					for(t=0;t<pSchedulScrnHandle->len;t++)
	//	 				DEBUG(PrintInfo,"0x%02x ",pSchedulScrnHandle->DataBuf[t]);
	//					DEBUG(PrintInfo,"");
	//	
	//				if(0==memcmp(pSchedulScrnHandle->TelNum,"####",pSchedulScrnHandle->len2))  //中心短信
	//				{//调度屏时将英文unicode短信转换成gbk
	//					if(pSchedulScrnHandle->len>1 && pSchedulScrnHandle->DataBuf[0]==0)
	//					{	if(!SMS_EnglishUniToGBK(pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len,pSchedulScrnHandle->DataBuf2,&pSchedulScrnHandle->len2))
	//						{u8 t;
	//							Mem_Copy(pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2);
	//							pSchedulScrnHandle->len=pSchedulScrnHandle->len2;
	//	
	//					DEBUG(PrintInfo,"英文短信---------");
	//					for(t=0;t<pSchedulScrnHandle->len;t++)
	//	 				DEBUG(PrintInfo,"0x%02x ",pSchedulScrnHandle->DataBuf[t]);
	//					DEBUG(PrintInfo,"");
	//	
	//							pSchedulScrnHandle->DevAct=0; //peijl_130311 添加：初始值为0
	//							pSchedulScrnHandle->Status=app_LbsAscSmsHandl(pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len,&pSchedulScrnHandle->DevAct,pSchedulScrnHandle->DataBuf2,&pSchedulScrnHandle->len2);
	//							if(pSchedulScrnHandle->Status==UnDefinedSms)
	//							{//非辅助短信
	//								pSchedulScrnHandle->len2=strlen("非辅助短信！");
	//								Mem_Copy(pSchedulScrnHandle->DataBuf2,"非辅助短信！",pSchedulScrnHandle->len2);
	//								app_GetPosinf(pSchedulScrnHandle->DataBuf,GPSTime,BCD_Code);
	//								SScrn_SMS_Send('E',"####",4,pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2);
	//							}
	//							else if(pSchedulScrnHandle->Status==ActionOK)//正确解析
	//							{
	//								app_CentersmsAckAction(pSchedulScrnHandle->DevAct,pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2);
	//							}
	//							break;
	//						}
	//					}
	//					//是否发送到中心？？
	//				}
	//					//发unicode短信
	//					WCM_SendSMS(UNICOD_SMS,pSchedulScrnHandle->TelNum,strlen(pSchedulScrnHandle->TelNum),pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len);
	//				}
	//			break;
	//			case CALL_CAR_RSP_CMD: //抢单
	//				Mem_Copy(gOrderInOperat,gLastOrder,50);
	//				DEBUG(PrintInfo,"SchedulScrn_Task: Request the order-%s",gOrderInOperat);
	//	//			if(gOrderInOperatflag==1)
	//				{
	//				SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,CALL_CAR_RSP_CMD,0);
	//				pSchedulScrnHandle->DataBuf[pSchedulScrnHandle->len]='\0';
	//				DEBUG(PrintInfo,"please wait for result......");
	//				JT808_SendMsg(ORDER_REQUEST,1,0,pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf));
	//				//gOrderRequestTime; peijl_test 计数开始，1m无应答，抢答取消
	//				}
	//	//			else
	//	//			{//抢答超时
	//	//			DEBUG(PrintInfo,"Order-Request is forbidden!");
	//	//			}
	//			break;
	//			case PICKUP_PASSEGER_CMD: //接人
	//				SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,PICKUP_PASSEGER_CMD,0);
	//				pSchedulScrnHandle->DataBuf[pSchedulScrnHandle->len]='\0';	
	//				DEBUG(PrintInfo,"SchedulScrn_Task: Pick up the passegers,order %s is successful.",pSchedulScrnHandle->DataBuf);
	//				pSchedulScrnHandle->U8Temp=0;//接到人
	//				Write_ProtclHandl(e_CallcarResult,&pSchedulScrnHandle->U8Temp,1);
	//				JT808_SendMsg(ORDER_DONE,1,0,pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf));
	//				gOrderInOperatflag=0;
	//			break;
	//			case PICKUP_PASSEGER_FAILED_CMD: //未接人
	//				SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,PICKUP_PASSEGER_FAILED_CMD,0);
	//				SScrn_GetResult(pSchedulScrnHandle->DataBuf2,sizeof(pSchedulScrnHandle->DataBuf2),&pSchedulScrnHandle->len2,PICKUP_PASSEGER_FAILED_CMD,1);
	//				pSchedulScrnHandle->DataBuf[pSchedulScrnHandle->len]='\0';	
	//				pSchedulScrnHandle->DataBuf2[pSchedulScrnHandle->len2]='\0';	
	//				DEBUG(PrintInfo,"SchedulScrn_Task: Order %s was failed,reason was %s",pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->DataBuf2);
	//				pSchedulScrnHandle->U8Temp=1;//未接到人
	//				Write_ProtclHandl(e_CallcarResult,&pSchedulScrnHandle->U8Temp,1);
	//				JT808_SendMsg(ORDER_DONE,1,1,pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf));
	//				gOrderInOperatflag=0;
	//			break;
		default:
		break;
		}
	}
	else
	{
		DEBUG(PrintInfo,"SchedulScrn_Task:没有对应的外设屏");
		OSTimeDlyHMSM(0,1,0,0);
	}

	//		switch(pSchedulScrnHandle->Cmdtype)
	//		{
	//	//#if SCREEN_N990>0
	//	if(pSchedulScrnHandle->ScrnType&LCDN990)
	//	{
	//		case N990_ONEBYTE_CMD:
	//			SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,N990_ONEBYTE_CMD,0);
	//			switch(pSchedulScrnHandle->DataBuf[0])
	//			{
	//			case HEARTBEAT_ACK_CMD:
	//				DEBUG(PrintDebug,"调度屏心跳应答-------------------------------");
	//				DEBUG(PrintDebug,"SchedulScrn_Task:Heartbeat response.");
	//				OSSemPost(ScrnHtbeatAck_Sem);
	//			break;
	//			case CALL_HANGON_CMD: //接电话
	//				ReadTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//				if(pSchedulScrnHandle->U8Temp==SchedulRcvCall)
	//				{
	//					pSchedulScrnHandle->U8Temp=HandOn;
	//					WriteTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//					DEBUG(PrintInfo,"SchedulScrn_Task: Pick up the phone");
	//				}
	//			break;
	//			case CALL_HANGOFF_CMD: //挂机
	//				ReadTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//				pSchedulScrnHandle->U8Temp=DevHangUp;
	//				WriteTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//				DEBUG(PrintInfo,"SchedulScrn_Task: hang up!");
	//			break;
	//			case GET_SMS_FORMAT_CMD: //查询短信格式
	//				DEBUG(PrintInfo,"SchedulScrn_Task: get SMS format of WCM.");
	//
	//			break;
	//			case CALL_CENTER_CMD: //呼叫中心
	//			break;
	//			case SETSPK_UP_CMD: //加音量
	//			break;
	//			case SETSPK_DOWN_CMD: //减音量
	//			break;
	//			case GET_SPKVAL_CMD: //查询音量
	//			break;
	//			default:
	//			break;
	//			}
	//		break;
	//		case N990_SENDSMS_CMD: //uni
	//			if(0==SScrn_GetResult(pSchedulScrnHandle->TelNum,sizeof(pSchedulScrnHandle->TelNum),&pSchedulScrnHandle->len2,N990_SENDSMS_CMD,TEL_NUM_ENUM))
	//			if(0==SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,N990_SENDSMS_CMD,SMS_DAT_ENUM))
	//			{	u8 t=0;
	//				pSchedulScrnHandle->TelNum[pSchedulScrnHandle->len2]='\0';
	//				DEBUG(PrintInfo,"SchedulScrn_Task: 手机号-%s",pSchedulScrnHandle->TelNum);
	//				for(t=0;t<pSchedulScrnHandle->len;t++)
	// 				DEBUG(PrintInfo,"0x%02x ",pSchedulScrnHandle->DataBuf[t]);
	//				DEBUG(PrintInfo,"");
	//
	//			if(0==memcmp(pSchedulScrnHandle->TelNum,"####",pSchedulScrnHandle->len2))  //中心短信
	//			{//调度屏时将英文unicode短信转换成gbk
	//				if(pSchedulScrnHandle->len>1 && pSchedulScrnHandle->DataBuf[0]==0)
	//				{	if(!SMS_EnglishUniToGBK(pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len,pSchedulScrnHandle->DataBuf2,&pSchedulScrnHandle->len2))
	//					{u8 t;
	//						Mem_Copy(pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2);
	//						pSchedulScrnHandle->len=pSchedulScrnHandle->len2;
	//
	//				DEBUG(PrintInfo,"英文短信---------");
	//				for(t=0;t<pSchedulScrnHandle->len;t++)
	// 				DEBUG(PrintInfo,"0x%02x ",pSchedulScrnHandle->DataBuf[t]);
	//				DEBUG(PrintInfo,"");
	//
	//						pSchedulScrnHandle->DevAct=0; //peijl_130311 添加：初始值为0
	//						pSchedulScrnHandle->Status=app_LbsAscSmsHandl(pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len,&pSchedulScrnHandle->DevAct,pSchedulScrnHandle->DataBuf2,&pSchedulScrnHandle->len2);
	//						if(pSchedulScrnHandle->Status==UnDefinedSms)
	//						{//非辅助短信
	//							pSchedulScrnHandle->len2=strlen("非辅助短信！");
	//							Mem_Copy(pSchedulScrnHandle->DataBuf2,"非辅助短信！",pSchedulScrnHandle->len2);
	//							app_GetPosinf(pSchedulScrnHandle->DataBuf,GPSTime,BCD_Code);
	//							SScrn_SMS_Send('E',"####",4,pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2);
	//						}
	//						else if(pSchedulScrnHandle->Status==ActionOK)//正确解析
	//						{
	//							app_CentersmsAckAction(pSchedulScrnHandle->DevAct,pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2);
	//						}
	//						break;
	//					}
	//				}
	//				//是否发送到中心？？
	//			}
	//				//发unicode短信
	//				WCM_SendSMS(UNICOD_SMS,pSchedulScrnHandle->TelNum,strlen(pSchedulScrnHandle->TelNum),pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len);
	//			}
	//		break;
	//		case CALL_CAR_RSP_CMD: //抢单
	//			Mem_Copy(gOrderInOperat,gLastOrder,50);
	//			DEBUG(PrintInfo,"SchedulScrn_Task: Request the order-%s",gOrderInOperat);
	////			if(gOrderInOperatflag==1)
	//			{
	//			SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,CALL_CAR_RSP_CMD,0);
	//			pSchedulScrnHandle->DataBuf[pSchedulScrnHandle->len]='\0';
	//			DEBUG(PrintInfo,"please wait for result......");
	//			JT808_SendMsg(ORDER_REQUEST,1,0,pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf));
	//			//gOrderRequestTime; peijl_test 计数开始，1m无应答，抢答取消
	//			}
	////			else
	////			{//抢答超时
	////			DEBUG(PrintInfo,"Order-Request is forbidden!");
	////			}
	//		break;
	//		case PICKUP_PASSEGER_CMD: //接人
	//			SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,PICKUP_PASSEGER_CMD,0);
	//			pSchedulScrnHandle->DataBuf[pSchedulScrnHandle->len]='\0';	
	//			DEBUG(PrintInfo,"SchedulScrn_Task: Pick up the passegers,order %s is successful.",pSchedulScrnHandle->DataBuf);
	//			pSchedulScrnHandle->U8Temp=0;//接到人
	//			Write_ProtclHandl(e_CallcarResult,&pSchedulScrnHandle->U8Temp,1);
	//			JT808_SendMsg(ORDER_DONE,1,0,pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf));
	//			gOrderInOperatflag=0;
	//		break;
	//		case PICKUP_PASSEGER_FAILED_CMD: //未接人
	//			SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,PICKUP_PASSEGER_FAILED_CMD,0);
	//			SScrn_GetResult(pSchedulScrnHandle->DataBuf2,sizeof(pSchedulScrnHandle->DataBuf2),&pSchedulScrnHandle->len2,PICKUP_PASSEGER_FAILED_CMD,1);
	//			pSchedulScrnHandle->DataBuf[pSchedulScrnHandle->len]='\0';	
	//			pSchedulScrnHandle->DataBuf2[pSchedulScrnHandle->len2]='\0';	
	//			DEBUG(PrintInfo,"SchedulScrn_Task: Order %s was failed,reason was %s",pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->DataBuf2);
	//			pSchedulScrnHandle->U8Temp=1;//未接到人
	//			Write_ProtclHandl(e_CallcarResult,&pSchedulScrnHandle->U8Temp,1);
	//			JT808_SendMsg(ORDER_DONE,1,1,pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf));
	//			gOrderInOperatflag=0;
	//		break;
	//	}
	//	#else
	//	else if(pSchedulScrnHandle->U32Temp&SchedulScrn)
	//	{
	//		case CENTER_SMS_CMD: //调度屏发出中心短信为uni，接收为gbk；手柄均为gbk
	//			SScrn_SMSResult_Send(0);
	//			SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,CENTER_SMS_CMD,0);
	//			pSchedulScrnHandle->DataBuf[pSchedulScrnHandle->len]='\0';
	//			//调度屏时将英文unicode短信转换成gbk
	//			if(pSchedulScrnHandle->len>1 && pSchedulScrnHandle->DataBuf[0]==0)
	//			{	
	//				if(!SMS_EnglishUniToGBK(pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len,pSchedulScrnHandle->DataBuf2,&pSchedulScrnHandle->len2))
	//				{
	//				Mem_Copy(pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2);
	//				pSchedulScrnHandle->len=pSchedulScrnHandle->len2;
	//				}
	//				else
	//					pSchedulScrnHandle->len=0;
	//			}
	//			pSchedulScrnHandle->DevAct=0; //peijl_130311 添加：初始值为0
	//			pSchedulScrnHandle->Status=app_LbsAscSmsHandl(pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len,&pSchedulScrnHandle->DevAct,pSchedulScrnHandle->DataBuf2,&pSchedulScrnHandle->len2);
	//			if(pSchedulScrnHandle->Status==UnDefinedSms)
	//			{//非辅助短信
	//				OSTimeDly(30);
	//				pSchedulScrnHandle->len2=strlen("非辅助短信！");
	//				Mem_Copy(pSchedulScrnHandle->DataBuf2,"非辅助短信！",pSchedulScrnHandle->len2);
	//				if(ActionOK!=SScrn_CenterSMS_Send(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2))
	//					DEBUG(PrintDebug,"SchedulScrn_Task:Send Ack-Sms failed");
	//			}
	//			else if(pSchedulScrnHandle->Status==ActionOK)//正确解析
	//			{
	//		/*		if(pSchedulScrnHandle->ScrnUniCenterSMS==1) //还原调度屏uni短信
	//				{
	//					pSchedulScrnHandle->ScrnUniCenterSMS=0;
	//					if(!SMS_EnglishGBKToUni(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2,pSchedulScrnHandle->DataBuf,&pSchedulScrnHandle->len))
	//					{
	//					Mem_Copy(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len);
	//					pSchedulScrnHandle->len2=pSchedulScrnHandle->len;
	//					}
	//				}	  */
	//				OSTimeDly(300);
	//				app_CentersmsAckAction(pSchedulScrnHandle->DevAct,pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2);
	//			}
	//		break;
	//		case UNICODE_SMS_CMD:
	//			SScrn_SMSResult_Send(0);
	//			SScrn_GetResult(pSchedulScrnHandle->TelNum,sizeof(pSchedulScrnHandle->TelNum),&pSchedulScrnHandle->len,UNICODE_SMS_CMD,TEL_NUM_ENUM);
	//			SScrn_GetResult(pSchedulScrnHandle->DataBuf2,sizeof(pSchedulScrnHandle->DataBuf2),&pSchedulScrnHandle->len2,UNICODE_SMS_CMD,SMS_DAT_ENUM);
	//			if(WCM_OK!=WCM_SendSMS(UNICOD_SMS,pSchedulScrnHandle->TelNum,pSchedulScrnHandle->len,pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2))
	//				DEBUG(PrintDebug,"SchedulScrn_Task:WCM send message failed");
	//		break;
	//		case SET_UDP_CMD:
	//			SScrn_Result_Send(0);
	//			if(!SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,SET_UDP_CMD,IP_ENUM))
	//				if(!SScrn_GetResult((u8 *)&pSchedulScrnHandle->U16Temp,2,&pSchedulScrnHandle->len2,SET_UDP_CMD,PORT_ENUM))
	//			{
	//				pSchedulScrnHandle->DataBuf[pSchedulScrnHandle->len]='\0';
	//				DEBUG(PrintInfo,"SchedulScrn_Task:set IP and udp port");
	//				DEBUG(PrintDebug,"IP  :%s",pSchedulScrnHandle->DataBuf);
	//				DEBUG(PrintDebug,"PORT:%d",pSchedulScrnHandle->U16Temp);
	//
	//				ReadLbsCfgPara(eIP,pSchedulScrnHandle->DataBuf2,&pSchedulScrnHandle->U8Temp);
	//				if((pSchedulScrnHandle->U8Temp!=pSchedulScrnHandle->len) ||0!=(memcmp(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len)))
	//				{//IP改变 删除注册信息
	//					pSchedulScrnHandle->U32Temp=pSchedulScrnHandle->U16Temp;
	//					WriteLbsCfgPara(eIP,pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len,UpdateOnly);
	//					WriteLbsCfgPara(eUdpPort,(u8 *)&(pSchedulScrnHandle->U32Temp),4,UpdateFlash);
	//
	//					ReadLinkTaskPara(&pSchedulScrnHandle->U16Temp,NetStatusPara);
	//					if(pSchedulScrnHandle->U16Temp&LinkHXSeverOk)
	//						WCM_IPCloseLink(SYS_CHANNEL);
	//					WriteLinkTaskPara(LinkHXSeverOk,NetStatusPara,RESET);
	//					WriteLinkTaskPara(ActionLoginHX,ActionTypePara,SET);	
	//					DEBUG(PrintInfo,"SchedulScrn_Task:ReLogin Moniter server.");
	//				}
	//				else
	//				{
	//					ReadLbsCfgPara(eNET,&pSchedulScrnHandle->len2,&pSchedulScrnHandle->U8Temp);
	//					if(pSchedulScrnHandle->len2==eNET_UDP)
	//					{
	//						ReadLbsCfgPara(eUdpPort,(u8 *)&pSchedulScrnHandle->U32Temp,&pSchedulScrnHandle->U8Temp);
	//						if(pSchedulScrnHandle->U32Temp!=(u32)pSchedulScrnHandle->U16Temp)
	//						{ //端口号改变，则该平台注册状态删除（未注销）
	//						pSchedulScrnHandle->U32Temp=pSchedulScrnHandle->U16Temp;
	//						WriteLbsCfgPara(eUdpPort,(u8 *)&pSchedulScrnHandle->U32Temp,4,UpdateFlash);
	//						ReadLinkTaskPara(&pSchedulScrnHandle->U16Temp,NetStatusPara);
	//						if(pSchedulScrnHandle->U16Temp&LinkHXSeverOk)
	//							WCM_IPCloseLink(SYS_CHANNEL);
	//						WriteLinkTaskPara(LinkHXSeverOk,NetStatusPara,RESET);
	//						WriteLinkTaskPara(ActionLoginHX,ActionTypePara,SET);	
	//						DEBUG(PrintInfo,"SchedulScrn_Task:ReLogin Moniter server.");
	//						}				
	//					}
	//				}
	//			}
	//		break;
	//		case SET_LOCAL_TEL_CMD:
	//			SScrn_Result_Send(0);
	//			if(!SScrn_GetResult(pSchedulScrnHandle->TelNum,sizeof(pSchedulScrnHandle->TelNum),&pSchedulScrnHandle->len,SET_LOCAL_TEL_CMD,0))
	//			{
	//				pSchedulScrnHandle->TelNum[pSchedulScrnHandle->len]='\0';
	//				DEBUG(PrintDebug,"SchedulScrn_Task:");
	//				DEBUG(PrintDebug,"Set local tel %s",pSchedulScrnHandle->TelNum);
	//				
	//				ReadLbsCfgPara(eLclTEL,pSchedulScrnHandle->DataBuf2,&pSchedulScrnHandle->U8Temp);
	//				if((pSchedulScrnHandle->U8Temp==pSchedulScrnHandle->len)&&(memcmp(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->TelNum,pSchedulScrnHandle->len)==0))
	//				{ //号码未变
	//				}
	//				else
	//				{
	//				WriteLbsCfgPara(eLclTEL,pSchedulScrnHandle->TelNum,pSchedulScrnHandle->len,UpdateFlash);
	//		#ifdef EVDO_USE
	//				DVRLinkParaSet(NULL,0,NULL,pSchedulScrnHandle->TelNum,pSchedulScrnHandle->len);		
	//		#endif
	//				//更换设备后清盲区数据，统计参数，里程
	//				ClearflashPackg(eBlindPosData);
	//				ClearflashPackg(eDriverSignData);
	//				ClearflashPackg(eStaticData);
	//				ClearflashPackg(eDistanc);
	//				ReadLinkTaskPara(&pSchedulScrnHandle->U16Temp,NetStatusPara);
	//				if(pSchedulScrnHandle->U16Temp&LinkHXSeverOk)
	//					WCM_IPCloseLink(SYS_CHANNEL);
	//				WriteLinkTaskPara(LinkHXSeverOk,NetStatusPara,RESET);
	//				WriteLinkTaskPara(ActionLoginHX,ActionTypePara,SET);	
	//				DEBUG(PrintInfo,"SchedulScrn_Task:ReLogin Moniter server.");
	//				}
	//			}
	//		break;
	//		case CALL_DIALUP_CMD:
	//			ReadTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//			if(pSchedulScrnHandle->U8Temp==NoneAction)
	//			{
	//				if(!SScrn_GetResult(pSchedulScrnHandle->TelNum,sizeof(pSchedulScrnHandle->TelNum)-1,&pSchedulScrnHandle->len,CALL_DIALUP_CMD,0))
	//				{
	//					pSchedulScrnHandle->TelNum[pSchedulScrnHandle->len]='\0';
	//					WriteTelPara(pSchedulScrnHandle->TelNum,eTelNumPara);
	//					pSchedulScrnHandle->U8Temp=0;
	//					WriteTelPara(&pSchedulScrnHandle->U8Temp,eTelTypePara);
	//					pSchedulScrnHandle->U8Temp=DailUp;
	//					WriteTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//
	//					if(OS_ERR_NONE!=OSTaskResume(CallTaskPrio))	//直到唤醒电话任务或挂断
	//					{
	//						SScrn_HandOff_Send(); //peijl_20120918 添加
	//						pSchedulScrnHandle->U8Temp=NoneAction;
	//						WriteTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//					}
	//					DEBUG(PrintInfo,"SchedulScrn_Task: Dial %s",pSchedulScrnHandle->TelNum);
	//				}
	//			}
	//			else //作为第三方电话打出或电话监听过程，禁止电话打出
	//			{				
	//				SScrn_HandOff_Send();
	//			}
	//		break;
	//		case CALL_HANGOFF_CMD:
	//			ReadTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//			pSchedulScrnHandle->U8Temp=DevHangUp;
	//			WriteTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//			DEBUG(PrintInfo,"SchedulScrn_Task: hang up!");
	//		break;
	//		case CALL_HANGON_CMD:
	//			ReadTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//			if(pSchedulScrnHandle->U8Temp==SchedulRcvCall)
	//			{
	//				pSchedulScrnHandle->U8Temp=HandOn;
	//				WriteTelPara(&pSchedulScrnHandle->U8Temp,eCallAction);
	//				DEBUG(PrintInfo,"SchedulScrn_Task: Pick up the phone");
	//			}
	//		break;
	//		case CALL_SPKSWITCH_CMD:
	//			WriteTelPara(0,eHandFree);
	//			DEBUG(PrintInfo,"SchedulScrn_Task: Set handfree-switch!");
	//		break;
	//		case ACK_CMD:
	//			OSSemPost(SchedulAckSem);			
	//		break;
	//		case SET_SPK_CMD:
	//			DEBUG(PrintInfo,"SchedulScrn_Task: Set SPK ");
	//			if(!SScrn_GetResult(&pSchedulScrnHandle->U8Temp,1,&pSchedulScrnHandle->len,SET_SPK_CMD,0))
	//			{
	//				WCM_SetSPKValue(pSchedulScrnHandle->U8Temp);
	//			}
	//		break;
	//		case SET_MIC_CMD:
	//			DEBUG(PrintInfo,"SchedulScrn_Task: Set MIC!");
	//			if(!SScrn_GetResult(&pSchedulScrnHandle->U8Temp,1,&pSchedulScrnHandle->len,SET_MIC_CMD,0))
	//			{
	//			#ifndef EVDO_USE
	//				WCM_SetMICValue(pSchedulScrnHandle->U8Temp);
	//			#endif
	//			}
	//		break;
	//	#ifdef PROTCL_HX54
	//		case DRIVER_SIGN_CMD: //jt808未处理
	//			SScrn_Result_Send(0);
	//			pSchedulScrnHandle->U8Temp=app_SScrndriverSign(pSchedulScrnHandle->DataBuf2,sizeof(pSchedulScrnHandle->DataBuf2));
	//			if(pSchedulScrnHandle->U8Temp!=ActionOK)
	//				DEBUG(PrintDebug,"SchedulScrn_Task: Driver log error!");
	//		break;
	//	#endif
	//		case SET_TCP_CMD:
	//			SScrn_Result_Send(0);
	//			if(!SScrn_GetResult(pSchedulScrnHandle->DataBuf,sizeof(pSchedulScrnHandle->DataBuf),&pSchedulScrnHandle->len,SET_TCP_CMD,IP_ENUM))
	//				if(!SScrn_GetResult((u8 *)&pSchedulScrnHandle->U16Temp,2,&pSchedulScrnHandle->len2,SET_TCP_CMD,PORT_ENUM))
	//			{
	//				pSchedulScrnHandle->DataBuf[pSchedulScrnHandle->len]='\0';
	//				DEBUG(PrintDebug,"SchedulScrn_Task:set IP and udp port");
	//				DEBUG(PrintDebug,"IP  :%s",pSchedulScrnHandle->DataBuf);
	//				DEBUG(PrintDebug,"PORT:%d",pSchedulScrnHandle->U16Temp);
	//
	//				ReadLbsCfgPara(eIP,pSchedulScrnHandle->DataBuf2,&pSchedulScrnHandle->U8Temp);
	//				if((pSchedulScrnHandle->U8Temp!=pSchedulScrnHandle->len) ||0!=(memcmp(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len)))
	//				{//IP改变 删除注册信息
	//					pSchedulScrnHandle->U32Temp=pSchedulScrnHandle->U16Temp;
	//					WriteLbsCfgPara(eIP,pSchedulScrnHandle->DataBuf,pSchedulScrnHandle->len,UpdateOnly);
	//					WriteLbsCfgPara(eTcpPort,(u8 *)&(pSchedulScrnHandle->U32Temp),4,UpdateFlash);
	//					ReadLinkTaskPara(&pSchedulScrnHandle->U16Temp,NetStatusPara);
	//					if(pSchedulScrnHandle->U16Temp&LinkHXSeverOk)
	//						WCM_IPCloseLink(SYS_CHANNEL);
	//					WriteLinkTaskPara(LinkHXSeverOk,NetStatusPara,RESET);
	//					WriteLinkTaskPara(ActionLoginHX,ActionTypePara,SET);	
	//					DEBUG(PrintInfo,"SchedulScrn_Task:ReLogin Moniter server.");
	//				}
	//				else
	//				{
	//					ReadLbsCfgPara(eNET,&pSchedulScrnHandle->len2,&pSchedulScrnHandle->U8Temp);
	//					if(pSchedulScrnHandle->len2==eNET_TCP)
	//					{
	//						ReadLbsCfgPara(eTcpPort,(u8 *)&pSchedulScrnHandle->U32Temp,&pSchedulScrnHandle->U8Temp);
	//						if(pSchedulScrnHandle->U32Temp!=(u32)pSchedulScrnHandle->U16Temp)
	//						{ //端口号改变，则该平台注册状态删除（未注销）
	//						pSchedulScrnHandle->U32Temp=pSchedulScrnHandle->U16Temp;
	//						WriteLbsCfgPara(eTcpPort,(u8 *)&pSchedulScrnHandle->U32Temp,4,UpdateFlash);
	//						ReadLinkTaskPara(&pSchedulScrnHandle->U16Temp,NetStatusPara);
	//						if(pSchedulScrnHandle->U16Temp&LinkHXSeverOk)
	//							WCM_IPCloseLink(SYS_CHANNEL);
	//						WriteLinkTaskPara(LinkHXSeverOk,NetStatusPara,RESET);
	//						WriteLinkTaskPara(ActionLoginHX,ActionTypePara,SET);	
	//						DEBUG(PrintInfo,"SchedulScrn_Task:ReLogin Moniter server.");
	//						}				
	//					}
	//				}
	//			}
	//		break;
	//		case SCCRN_TIME_OUT:
	//		case SCCRN_ERROR_RETURN:
	//			//DEBUG(PrintInfo,"SScreen Rcvtask:No data");
	//		break;
	//		case STATION_DOWNLOAD_START_CMD: //站点下载启动
	//			Clr_LineInfoInFlash();
	//			SScrn_CmdResult_Send(STATION_DOWNLOAD_START_RSP_CMD,Normal_Ack,0);
	//		break;
	//		case STATION_DOWNLOAD_CMD: //站点下载
	//			if(0==SScrn_GetResult(pSchedulScrnHandle->DataBuf2,sizeof(pSchedulScrnHandle->DataBuf2),&pSchedulScrnHandle->len2,STATION_DOWNLOAD_CMD,0))
	//			{
	//				u8 index=0;
	//				STRUCT_STATION_DATA *Pstr=(STRUCT_STATION_DATA *)pSchedulScrnHandle->DataBuf2;
	//				if(0==LineInfo_save2flash(Pstr->ROAD_NUM,Pstr->STA_DIRECTION,Pstr->STA_TOTAL_NUM,Pstr->ROAD_VER,Pstr->STA_DATALEN,Pstr->STA_DATA))
	//				{
	//					SScrn_CmdResult_Send(STATION_DOWNLOAD_RSP_CMD,Normal_Ack,0);
	//					DEBUG(PrintInfo,"SchedulScrn_Task:Line information was downloaded successfully.");
	//				}else  //缓存区满
	//				{	SScrn_CmdResult_Send(STATION_DOWNLOAD_RSP_CMD,Normal_Ack,1);
	//					DEBUG(PrintInfo,"SchedulScrn_Task:can't download Line information !!!!!");
	//				}
	//			}
	//			else
	//			SScrn_CmdResult_Send(STATION_DOWNLOAD_RSP_CMD,Normal_Ack,1);
	//		break;
	//	}
	////	#endif
	//		case UPDATA_REQ_CMD: //收到串口升级请求，下发当前版本信息
	//			DEBUG(PrintInfo,"Requry for version upgrading by com.");
	//			Flash_Read_Buffer(&pSchedulScrnHandle->U8Temp,1,DEV_HWVER_ADDR);
	//			Flash_Read_Buffer(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->U8Temp,DEV_HWVER_ADDR+1);
	//			memcpy(&pSchedulScrnHandle->DataBuf2[pSchedulScrnHandle->U8Temp],SOFTWARE_VER,9);
	//			memcpy(&pSchedulScrnHandle->DataBuf2[pSchedulScrnHandle->U8Temp+9],".bin",strlen(".bin"));
	//			pSchedulScrnHandle->len2=pSchedulScrnHandle->U8Temp+9+strlen(".bin");
	//			SScrn_Version_Send(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2); //
	//		break;
	//		case UPDATA_PARA_CMD: //升级版本信息
	//			if(0==SScrn_GetResult((u8 *)&ComUpdata.AllPacket,sizeof(ComUpdata.AllPacket),&pSchedulScrnHandle->len2,UPDATA_PARA_CMD,PACKETS_ENUM))
	//			{
	//			if(0==SScrn_GetResult((u8 *)&ComUpdata.AllSize,sizeof(ComUpdata.AllSize),&pSchedulScrnHandle->len,UPDATA_PARA_CMD,DATA_SIZE_ENUM))
	//				{
	//				DEBUG(PrintInfo,"收到串口升级版本信息");
	//				ComUpdata.CurPacket=0;
	//				ComUpdata.Packetsize=250;//定长
	//				gComUpflag=1;
	//				}
	//			}
	//		break;
	//		case UPDATA_DATA_CMD : //升级程序包
	//			if(0==SScrn_GetResult(pSchedulScrnHandle->DataBuf2,sizeof(pSchedulScrnHandle->DataBuf2),&pSchedulScrnHandle->len2,UPDATA_DATA_CMD,0))
	//			{
	//				static u32 addr;
	//				if(ComUpdata.CurPacket==0)//第一包时备份版本擦除
	//				{
	//					Flash_Read_Buffer((u8 *)&addr,4,OLD_CODE_STORE_ADDR);
	//					for(pSchedulScrnHandle->U8Temp=0;pSchedulScrnHandle->U8Temp<VER_SECTOR_NUM;pSchedulScrnHandle->U8Temp++)
	//						Flash_Write_Buffer(NULL,0,addr+SECTOR_SIZE*pSchedulScrnHandle->U8Temp,Sector_Erase);
	//				}
	//				//程序存flash
	//				Flash_Write_Buffer(pSchedulScrnHandle->DataBuf2,pSchedulScrnHandle->len2,addr+4+ComUpdata.Packetsize*ComUpdata.CurPacket,Write_Mode);
	//				while(OSSemAccept(UpdataRcvSem)) ; //防止同一包两次信号通知
	//				OSSemPost(UpdataRcvSem);
	//			}
	//		break;
	//		default:
	//			OSTimeDly(5);
	//		//	DEBUG(PrintInfo,"SScreen Rcvtask:unknown command type 0x%2x",pSchedulScrnHandle->Cmdtype);
	//		break;	
	//		}
	}
	}
#endif
/*********************************************************
*Function:       SchedulScrnTaskInit
*Description:    初始化任务句柄和所使用的全局变量
*Calls:          无
*Called By:      无
*Input:          *pLinkHandle 调用函数的任务句柄指针
*Output:         pLinkHandle
*Return:         ReturnType类型
*Others:
*********************************************************/
static u8 app_SScrnSendtaskInit(Stk_SchedulSend_Handle *pSScrnSendHandle)
{
#if 0
	u8 err;

	if(ScrnHtbeatAck_Sem==0)
		ScrnHtbeatAck_Sem=OSSemCreate(0);
	if(Alarm_Driver_Event == 0)
	{
		Alarm_Driver_Event = OSFlagCreate(0, &err);
	}
#endif
	Mem_Set(pSScrnSendHandle, 0, sizeof(Stk_SchedulSend_Handle));
	return 0;
}
//extern OS_FLAG_GRP *Area_Flag;
static void app_SScrnSendtaskExecut(Stk_SchedulSend_Handle *pSScrnSendHandle)
{
	u8 err;
	#if SCREEN_N990>0
	u8 ScrnHtbeatCnt=0;
	#endif
	u16 U16Temp2=0;
//	OS_FLAGS flags = 0;

//	OSTimeDly(500);
//	while(1)
	{
//		OSTimeDly(10);
#if 0
		flags=OSFlagAccept(Alarm_Driver_Event,(OS_FLAGS)0xffffffff,OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME,&err);
		if(flags)
		{
			if(flags&AREA_INTO_FLAG)
			{
				SScrn_CenterSMS_Send("已经驶入区域",strlen("已经驶入区域"));
			}
			if(flags&AREA_OUT_FLAG)
			{
				SScrn_CenterSMS_Send("已经驶出区域",strlen("已经驶出区域"));
			}
			if(flags&AREA_SPEED_OVER)
			{
				SScrn_CenterSMS_Send("区域内超速",strlen("区域内超速"));
			}
			if(flags&SCHOOL_GET_ON_BUS)
			{
				SScrn_CenterSMS_Send("上学登车刷卡状态",strlen("上学登车刷卡状态"));
			}
			if(flags&SCHOOL_GET_OFF_BUS)
			{
				SScrn_CenterSMS_Send("上学下车刷卡状态",strlen("上学下车刷卡状态"));
			}
			if(flags&HOME_GET_ON_BUS)
			{
				SScrn_CenterSMS_Send("放学上车刷卡状态",strlen("放学上车刷卡状态"));
			}
			if(flags&HOME_GET_OFF_BUS)
			{
				SScrn_CenterSMS_Send("放学下车刷卡状态",strlen("放学下车刷卡状态"));
			}
		}
#endif
	#if SCREEN_N990>0
		if(++pSScrnSendHandle->TxTim_Cnt>300)  //10s心跳
		{
			SchedulScrn_heartbeatSend();
			pSScrnSendHandle->U8Temp=1;
			OSSemPend(ScrnHtbeatAck_Sem,20,&pSScrnSendHandle->U8Temp);
			if(pSScrnSendHandle->U8Temp==OS_ERR_NONE)
			{	ScrnHtbeatCnt=0;
				if(ReadAlarmPara(StaAlarm0,ALARM_DISPLAY_ERR)==SET)
				{
					WriteAlarmPara(RESET,StaAlarm0,ALARM_DISPLAY_ERR);
					DEBUG(PrintInfo,"Cancel Schedul screen alarm!");
				#if PROTCL_JT808
					DevReq2ServPackag_build(REPORT_LOCATION); //告警产生一包位置汇报
				#endif
				}
			}
			else if(ScrnHtbeatCnt++>10) //10*(TxTim_Cnts=10)s不回,故障
			{	ScrnHtbeatCnt=0;
				DEBUG(PrintInfo,"SScrnSendtask:Screen N990 error!");		
				if(ReadAlarmPara(StaAlarm0,ALARM_DISPLAY_ERR)==RESET)
				{
				#ifdef PROTCL_JT808
					AlarmHandle(ALARM_DISPLAY_ERR,pSScrnSendHandle->DataBuf,StaAlarm0);
				#elif defined PROTCL_HX54
					WriteAlarmPara(SET,StaAlarm0,ALARM_DISPLAY_ERR);
					DEBUG(PrintError,"Alarm:Schedul screen error!");
				#endif
				}
			}
			pSScrnSendHandle->TxTim_Cnt=0;
		}
	#else
		if(++pSScrnSendHandle->TxTim_Cnt > 3)  //3s上报
		{
			//app_GetGPSStrct(&pSScrnSendHandle->GpsInfo);
			oa_memcpy(&pSScrnSendHandle->GpsInfo, &Pos_Inf, sizeof(STRUCT_RMC));
			//ReadLinkTaskPara(&pSScrnSendHandle->U16Temp,NetStatusPara);
			#if 0
			if(pSScrnSendHandle->U16Temp&LinkHXSeverOk)
				pSScrnSendHandle->DataBuf[0]=0x25; //peijl_120607 GPRS 中国联通 00100101
			else
				pSScrnSendHandle->DataBuf[0]=0;
			#endif
			if (dev_running.plat_status == ONLINE) 	pSScrnSendHandle->DataBuf[0]=0x25;
			else 	pSScrnSendHandle->DataBuf[0]=0;
			ReadLbsCfgPara(eOverspeed,&pSScrnSendHandle->DataBuf[1],&pSScrnSendHandle->U8len);
			//GetCSQ(&pSScrnSendHandle->DataBuf[2]);
			pSScrnSendHandle->DataBuf[2] = oa_network_get_signal_level();
			#if 0
			if(SET==ReadAlarmPara(StaSector1,STA_ACC_ON))
				pSScrnSendHandle->DataBuf[3]=0;
			else
				pSScrnSendHandle->DataBuf[3]=1;
			#endif
			if (acc_status == ACC_ON)		pSScrnSendHandle->DataBuf[3] = 0;
			else if (acc_status == ACC_OFF)	pSScrnSendHandle->DataBuf[3] = 1;	
			pSScrnSendHandle->DataBuf[4]=0;//未设看车
			#if 0
			ReadLbsCfgPara(eNET, &pSScrnSendHandle->U8Temp, &pSScrnSendHandle->U8len);
			if(pSScrnSendHandle->U8Temp==eNET_TCP)
				pSScrnSendHandle->DataBuf[5]='1';
			else
				pSScrnSendHandle->DataBuf[5]='0';
			#endif
			pSScrnSendHandle->DataBuf[5] = '1';
			SScrn_Status_Send(&pSScrnSendHandle->GpsInfo,(STRUCT_HOST_STATE *)pSScrnSendHandle->DataBuf);
			pSScrnSendHandle->TxTim_Cnt = 0;
		}
	#endif
#if 0
		#ifdef PROTCL_HX54
		if(gGruptelFlag==1) //下载集团号码
		{
			DEBUG(PrintInfo,"Schedul-screen send task:Download group-tel.");
			SScrn_GroupNum_Clr(); //先清除再下载所有号码
			pSScrnSendHandle->U16Temp=0;
			memcpy(pSScrnSendHandle->Name,"集团",4);
			pSScrnSendHandle->NameLen=4;//集团号码名字空
			while(ActionOK==Get_GrupTel(pSScrnSendHandle->TelNum,&pSScrnSendHandle->TelLen,pSScrnSendHandle->U16Temp))
			{
				if(0!=pSScrnSendHandle->TelLen)
					SScrn_GroupNum_Send(pSScrnSendHandle->Name,pSScrnSendHandle->NameLen,pSScrnSendHandle->TelNum,pSScrnSendHandle->TelLen);
				pSScrnSendHandle->U16Temp++;
				OSTimeDly(30); //调度屏处理最小间隔	
			}
			gGruptelFlag=0; //下载完成置无效标示
		}
		else if(gGruptelFlag==2)//删除集团号码
		{
			if(ActionOK==SScrn_GroupNum_Clr())
			{
				gGruptelFlag=0;
				DEBUG(PrintInfo,"Schedul-screen send task:Clear group-tel.");
			}
		}
		#endif
	#endif
#if 0
		//串口升级
		if(gComUpflag==1)
		{
			DEBUG(PrintInfo,"串口升级开始");
			DEBUG(PrintInfo,"串口升级包大小 %luB",ComUpdata.AllSize);
			DEBUG(PrintInfo,"串口升级总包数 %lu",ComUpdata.AllPacket);
			ReadLinkTaskPara(&pSScrnSendHandle->U16Temp,ActionTypePara);
			DEBUG(PrintInfo,"等待关闭在线升级.");
			do
			{
				OSTimeDly(5000);
				ReadLinkTaskPara(&pSScrnSendHandle->U16Temp,ActionTypePara);							
				ReadLinkTaskPara(&U16Temp2,NetStatusPara);
			}while((pSScrnSendHandle->U16Temp)&ActionUpdateTL ||(U16Temp2&LinkTLSeverOk));
			DEBUG(PrintInfo,"在线升级已关闭.");
			
			ComUpdata.CurPacket=0;
			while(ComUpdata.CurPacket<ComUpdata.AllPacket)
			{
				pSScrnSendHandle->U8len=0;
				while(pSScrnSendHandle->U8len<3)
				{
					if(0==SScrn_Data_Req(ComUpdata.CurPacket))
					{
						OSSemPend(UpdataRcvSem,100,&pSScrnSendHandle->U8Temp);
						if(pSScrnSendHandle->U8Temp==OS_ERR_NONE)
						{
							DEBUG(PrintDebug,"串口升级请求包号 %lu",ComUpdata.CurPacket);
							ComUpdata.CurPacket++;
							break;
						}
						else
							pSScrnSendHandle->U8len++;	
					}
				}
				if(pSScrnSendHandle->U8len>2) //升级失败
					break;
			}
			if(ComUpdata.CurPacket==ComUpdata.AllPacket) //升级成功
			{
				u32 t;
				//保存版本size
				Flash_Read_Buffer((u8 *)&pSScrnSendHandle->U32Temp,4,OLD_CODE_STORE_ADDR);
				Flash_Write_Buffer((u8 *)&ComUpdata.AllSize,sizeof(ComUpdata.AllSize),pSScrnSendHandle->U32Temp,Write_Mode);
				//将当前升级程序存储地址赋值为新版本
				Flash_Read_Buffer((u8 *)&t,4,NEW_CODE_STORE_ADDR);
				Flash_Write_Buffer(NULL,0,NEW_CODE_STORE_ADDR,Sector_Erase);
				Flash_Write_Buffer((u8 *)&pSScrnSendHandle->U32Temp,4,NEW_CODE_STORE_ADDR,Write_Mode);
				Flash_Write_Buffer((u8 *)&t,4,OLD_CODE_STORE_ADDR,Write_Mode);
				pSScrnSendHandle->U8Temp=0;
				Flash_Write_Buffer(&pSScrnSendHandle->U8Temp,1,UPDATE_FLAG_ADDR,Write_Mode);//写版本更新标志
				DEBUG(PrintInfo,"串口升级结束，设备重启。。。");
				WriteLinkTaskPara(ActionResetLbs,ActionTypePara,SET);
			}
			gComUpflag=0;
		}
#endif
	}
}
#if 0
void SchedulScrn_Factoryset()
{
//	u8 cnt=0;
#if SCREEN_N990>0
#else
//	for(cnt=0;cnt<BLIND_DRIVER_STORE_SIZE;cnt++)   //清登退签盲区数据  //JT808删除
//		Flash_Write_Buffer(NULL,0,BLIND_DRIVER_STORE_ADDR+SECTOR_SIZE*cnt,Sector_Erase);
//	for(cnt=0;cnt<RESTRICTTEL_STORE_SIZE;cnt++) //呼入呼出限制
//		Flash_Write_Buffer(NULL,0,RESTRICTTEL_NUM_ADDR+SECTOR_SIZE*cnt,Sector_Erase);
//	//集团号码
//	Clr_GrupTel();
//	SScrn_GroupNum_Clr();
#endif

}
#endif
