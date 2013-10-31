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
 *   oa_jt808.c
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
#include "oa_soc.h"
#include "oa_jt808.h"
#include "oa_enpacket_way.h"
#include "oa_dev_params.h"
#include "oa_debug.h"
#include "oa_gps.h"
#include "oa_platform.h"
#include "oa_blinddata.h"
#include "SchedulScrn.h"
#include "Area_Judge.h"
#include "oa_debug.h"
#include "oa_app.h"
#include "oa_sms.h"
#include "oa_alarm.h"
#define PRINT_SAMEPARA	DEBUG(" this parameter is same as the original, so I do nothing...")
#define PORT_MAX 65535
extern DEVICE_PARAMS dev_now_params;
extern oa_soc_context g_soc_context;
extern timeout_struct timeout_var;
extern timeout_data_struct timeout_data;
extern os_struct overspeed_var;
extern area_alarm_addition_struct area_alarm_addition_var;
extern STRUCT_RMC Pos_Inf;
dev_control_type control_type = none;
upgrade_paras up_paras;
action_kind plat_paraset = 0x0;
static u32 AlarmFlag[StatusNum]={0};

//#define UPDATA_BUFNUM		12 //上传数据队列的缓存区个数
//#define UPDATA_BUFLEN_MAX	200
//#define BUFFER_MAX_LEN 198
//#define BUFFER_MAX_LEN DATA_MAX_LEN

extern DEV_PLAT_PARAS dev_running;
ProtocolHandle sProtclHandl = {0};

u16 escape_copy_to_send(u8 *buf, u16 len);
u16 DevReq2ServPackag_build(u16 ReqMsgId);
u8 JT808Msg_Build(u16 DevMsgId,u16 totalPackt,u16 SubPackt,u8 *Sendbuf,u16 Sendbuflen,u16 *Senddatalen);
//=====================================从LBS上移植======================================
u8 Read_ProtclHandl(Protocol_Handle_e type, u8 *pbuf, u16 *pbuflen)
{
	u8 err= 0;
	if(NULL==pbuf||NULL==pbuflen||type>=e_ProHandlMAX)
		return 1;
	
//	OSSemPend(JT808_PrtclHandPrtctSem, 0, &err);
	switch(type)
	{
		case e_APTSStationinfo:
			*pbuflen=sizeof(sProtclHandl.stationInfo);
			Mem_Copy(pbuf,&sProtclHandl.stationInfo,*pbuflen);
		break;
		case eDevSeqid: //终端发送流水号
			*pbuflen=sizeof(sProtclHandl.DevSeqId);
			Mem_Copy(pbuf,sProtclHandl.DevSeqId,sizeof(sProtclHandl.DevSeqId));
	#if 0
			{
				u16 id;
				char_to_short(sProtclHandl.DevSeqId,&id);
				if(id==0xffff)
					id=0;
				else
					id++;
				short_to_char(sProtclHandl.DevSeqId,id);
			}
	#endif
		break;
		case eDevMsgid:{
			*pbuflen=sizeof(sProtclHandl.DevMsgId);
			Mem_Copy(pbuf,sProtclHandl.DevMsgId,sizeof(sProtclHandl.DevMsgId));
		}break;
		case eServSeqid: //流水号
			*pbuflen=sizeof(sProtclHandl.ServSeqId);
			Mem_Copy(pbuf,sProtclHandl.ServSeqId,sizeof(sProtclHandl.ServSeqId));
		break;
		case eServMsgid: //平台请求消息id
			*pbuflen=sizeof(sProtclHandl.ServMsgId);
			Mem_Copy(pbuf,sProtclHandl.ServMsgId,sizeof(sProtclHandl.ServMsgId));
		break;
		case eRsp2ServSeq: //平台请求应答结果
			*pbuflen=1;
			*pbuf=sProtclHandl.Rsp2Serv;
		break;
//		case eParaReq:
//			*pbuflen=sizeof(LBS_GetDevParamrsp);
//			Mem_Copy(pbuf,&sProtclHandl.GetDevParamrsp,*pbuflen);
//		break;
#if 0 //zq comment for mtk-lbs desn't support
		case e_Authen:
			Str_Copy(pbuf,sProtclHandl.Authen);
			*pbuflen=strlen(sProtclHandl.Authen);
		break;
		case eMediaInfo:  //多媒体信息上传
			*pbuflen=sizeof(sProtclHandl.MediaEventinfo);
			Mem_Copy(pbuf,&sProtclHandl.MediaEventinfo,*pbuflen);
		break;
		case eSpclServPara:	//监管平台
			*pbuflen=sizeof(sProtclHandl.spclServ);
			Mem_Copy(pbuf,&sProtclHandl.spclServ,*pbuflen);
		break;
		case eRecordPara: //录音参数
			*pbuflen=sizeof(sProtclHandl.StartRecordreq);
			Mem_Copy(pbuf,&sProtclHandl.StartRecordreq,*pbuflen);
		break;
#endif
		case eRsp2DevReq: //监管平台应答
			*pbuflen=sizeof(sProtclHandl.PlatComrsp);
			Mem_Copy(pbuf,&sProtclHandl.PlatComrsp,*pbuflen);
		break;
#if 0//zq comment for mtk-lbs desn't support
		case e_ReportEventreq:
		{
			*pbuflen = 1;
			*pbuf = sProtclHandl.ReportEventreq.EventId;
			break;
		}
		case e_AskAnswerrsp:
		{
			*pbuflen = sizeof(LBS_AskAnswerrsp);
			Mem_Copy(pbuf, &sProtclHandl.AskAnswerrsp, *pbuflen);
			break;
		}
		case e_ReportDriverreq_DriverName:
		{
			*pbuflen = sProtclHandl.ReportDriverreq.DriverNameLen;
			Mem_Copy(pbuf, &sProtclHandl.ReportDriverreq.DriverName, *pbuflen);
			break;
		}
		case e_ReportDriverreq_DriverID:
		{
			*pbuflen = sizeof(sProtclHandl.ReportDriverreq.DriverID);
			Mem_Copy(pbuf, &sProtclHandl.ReportDriverreq.DriverID, *pbuflen);
			break;
		}
		case e_ReportDriverreq_driverCertificateId:
		{
			*pbuflen = sizeof(sProtclHandl.ReportDriverreq.driverCertificateId);
			Mem_Copy(pbuf, &sProtclHandl.ReportDriverreq.driverCertificateId, *pbuflen);
			break;
		}
		case e_ReportDriverreq_OrgName:
		{
			*pbuflen = sProtclHandl.ReportDriverreq.OrgNameLen;
			Mem_Copy(pbuf, &sProtclHandl.ReportDriverreq.OrgName, *pbuflen);
			break;
		}
		case e_ReportMediaDatarsp: //校验多媒体数据上传应答
			*pbuflen = sizeof(LBS_ReportMediaDatarsp);
			Mem_Copy(pbuf, &sProtclHandl.ReportMediaDatarsp, *pbuflen);
		break;
		case e_RSAdev:
		{
			*pbuflen = sizeof(LBS_RSAdev);
			Mem_Copy(pbuf, &sProtclHandl.RSAdev, *pbuflen);
			break;
		}
		case e_RSAplat:
		{
			*pbuflen = sizeof(LBS_RSAplat);
			Mem_Copy(pbuf, &sProtclHandl.RSAplat, sizeof(LBS_RSAplat));
			break;
		}
#endif
		case e_CarCtlrsp:
		{
			*pbuflen = 2;
			Mem_Copy(pbuf, &sProtclHandl.CarCtlrsp.sequenceId, *pbuflen);
			break;
		}
#if 0//zq comment for mtk-lbs desn't support
		case e_ReportDriveDatarsp:
		{
			*pbuflen = sizeof(LBS_ReportDriveDatarsp);
			Mem_Copy(pbuf, &sProtclHandl.ReportDriveDatarsp, *pbuflen);
			break;
		}
		case e_TrackLocationCtlreq:
		{
			*pbuflen = sizeof(LBS_TrackLocationCtlreq);
			Mem_Copy(pbuf, &sProtclHandl.TrackLocationCtlreq, *pbuflen);
			break;
		}
		case e_TextDownloadreq:
		{
			*pbuflen = 1;
			Mem_Copy(pbuf, &sProtclHandl.TextDownloadreq.flag, *pbuflen);
			break;
		}
		case e_SetEventreq_EventType:	//事件设置//设置类型
		{
			*pbuflen = 1;
			Mem_Copy(pbuf, &sProtclHandl.SetEventreq.EventType, *pbuflen);
			break;
		}
		case e_SetEventreq_totalNum:	//事件设置//设置总数
		{
			*pbuflen = 1;
			Mem_Copy(pbuf, &sProtclHandl.SetEventreq.totalNum, *pbuflen);
			break;
		}
		case e_AskDownloadreq:
		{
			*pbuflen = sizeof(LBS_AskDownloadreq);
			Mem_Copy(pbuf, &sProtclHandl.AskDownloadreq, *pbuflen);
			break;
		}
		case e_DemandMenureq_DemandType:
		{
			*pbuflen = 1;
			Mem_Copy(pbuf, &sProtclHandl.DemandMenureq.DemandType, *pbuflen);
			break;
		}
		case e_DemandMenureq_MenuNum:
		{
			*pbuflen = 1;
			Mem_Copy(pbuf, &sProtclHandl.DemandMenureq.MenuNum, *pbuflen);
			break;
		}
		case e_DemandServreq:
		{
			*pbuflen = sizeof(LBS_DemandServreq);
			Mem_Copy(pbuf, &sProtclHandl.DemandMenureq, *pbuflen);
			break;
		}
		case e_CallBackreq:
		{
			*pbuflen = sizeof(sProtclHandl.CallBackreq);
			Mem_Copy(pbuf, sProtclHandl.CallBackreq, *pbuflen);
			break;
		}	
		case e_SetPhonebookreq_BookType:	//设置电话本//类型
		{
			*pbuflen = 1;
			Mem_Copy(pbuf, &sProtclHandl.SetPhonebookreq.BookType, *pbuflen);
			break;
		}
		case e_SetPhonebookreq_ContackNum:	//设置电话本//数量
		{
			*pbuflen = 1;
			Mem_Copy(pbuf, &sProtclHandl.SetPhonebookreq.ContactNum, *pbuflen);
			break;
		}
		case e_CarCtlreq:			
		{
			*pbuflen = sizeof(LBS_CarCtlreq);
			Mem_Copy(pbuf, &sProtclHandl.CarCtlreq, *pbuflen);
			break;
		}	
		case e_GetDriveDatareq:
		{
			*pbuflen = sizeof(LBS_GetDriveDatareq);
			Mem_Copy(pbuf, &sProtclHandl.GetDriveDatareq, *pbuflen);
			break;
		}
		case e_TakePhotoreq: //拍照参数
		{
			*pbuflen = sizeof(TakePhotoStrct);
			Mem_Copy(pbuf, &sProtclHandl.TakePhotoreqpara, *pbuflen); //peijl_1008 undo
			break;
		}	
		case e_SMediaDataCheckreq:
		{
			*pbuflen = sizeof(LBS_SMediaDataCheckreq);
			Mem_Copy(pbuf, &sProtclHandl.SmediaDataCheckreq, *pbuflen);
			break;
		}
		case e_ReportSMediareq:
		{
			*pbuflen = sizeof(LBS_ReportSMediareq);
			Mem_Copy(pbuf, &sProtclHandl.ReportSmediareq, *pbuflen);
			break;
		}
		case e_RSPSMediaDataCheckreq:
		{
			*pbuflen = sizeof(LBS_SMediaDataCheckrsp);
			Mem_Copy(pbuf, &sProtclHandl.SMediaDataCheckrsp, *pbuflen);
			break;
		}
		case e_DemandSetCancl:
			*pbuflen = sizeof(LBS_DemandSetCancelreq);
			Mem_Copy(pbuf,&sProtclHandl.DemandSetCancelreq, *pbuflen);
		break;
		case e_ElecMenuRpt:
			*pbuflen = 12; //固定长度for 808 test
			Mem_Copy(pbuf,&sProtclHandl.ElecMenu, 12);
		break;
		case e_CallcarOrder:
		case e_CallcarInfoOrder:
			*pbuflen = sizeof(STRUCT_CALLCARORDER);
			Mem_Copy(pbuf,&sProtclHandl.stCallcarOrder,*pbuflen);
		break;
		case e_CallcarAccept:
			*pbuflen=1;
			*pbuf= sProtclHandl.CallcarAccept;
		break;
		case e_CallcarResult:
			*pbuflen=1;
			*pbuf= sProtclHandl.CallcarResult;
		break;
#endif
		default:
		break;
	}

//	OSSemPost(JT808_PrtclHandPrtctSem);
	return 0;
}
u8 Write_ProtclHandl(Protocol_Handle_e type, u8 *pbuf, u16 buflen)
{
	u8 err;
	if (type >= e_ProHandlMAX || NULL==pbuf)
		return 1;

//	OSSemPend(JT808_PrtclHandPrtctSem, 0, &err);
	switch(type)
	{
		//发送
		case eDevSeqid: //终端发送流水号
			Mem_Copy(sProtclHandl.DevSeqId,pbuf,sizeof(sProtclHandl.DevSeqId));
		break;
		case eDevMsgid:{
			oa_memcpy(sProtclHandl.DevMsgId, pbuf, buflen);
		}break;
		case eServSeqid: //平台请求
			Mem_Copy(sProtclHandl.ServSeqId,pbuf,buflen);
		break;
		case eServMsgid: //平台请求
			Mem_Copy(sProtclHandl.ServMsgId,pbuf,buflen);
		break;
		case eRsp2ServSeq: //平台请求应答结果
			if(buflen!=1)
				return 1;
			sProtclHandl.Rsp2Serv=*pbuf;
		break;
#if 0 //zq comment for mtk-lbs desn't support
		case eMediaInfo: //多媒体上报
			Mem_Copy(&sProtclHandl.MediaEventinfo,pbuf,buflen);
		break;
#endif
		//接收
		case eRsp2DevReq: //对终端请求应答
			Mem_Copy(&sProtclHandl.PlatComrsp,pbuf,buflen);
		break;
		case eSpclServPara:	//监管平台
			Mem_Copy(&sProtclHandl.spclServ,pbuf,buflen);
		break;
#if 0 //zq comment for mtk-lbs desn't support
		case eRecordPara: //录音参数
			Mem_Copy(&sProtclHandl.spclServ,pbuf,buflen);
		break;
//		case eParaReq:
//			if(buflen!=sizeof(LBS_GetDevParamrsp))
//				return 1;
//			Mem_Copy(&sProtclHandl.GetDevParamrsp,pbuf,buflen);
//		break;
		case e_ReportEventreq:
			sProtclHandl.ReportEventreq.EventId = *pbuf;
		break;
		case e_AskAnswerrsp:
		{
			Mem_Copy(&sProtclHandl.AskAnswerrsp, pbuf, buflen);
			break;
		}
		case e_ReportDriverreq_DriverName:
		{
			sProtclHandl.ReportDriverreq.DriverNameLen=(u8)buflen;
			Mem_Copy(&sProtclHandl.ReportDriverreq.DriverName, pbuf, buflen);
			break;
		}
		case e_ReportDriverreq_DriverID:
		{
			Mem_Copy(&sProtclHandl.ReportDriverreq.DriverID, pbuf, buflen);
			break;
		}
		case e_ReportDriverreq_driverCertificateId:
		{
			Mem_Copy(&sProtclHandl.ReportDriverreq.driverCertificateId, pbuf, buflen);
			break;
		}
		case e_ReportDriverreq_OrgName:
		{
			sProtclHandl.ReportDriverreq.OrgNameLen=(u8)buflen;
			Mem_Copy(&sProtclHandl.ReportDriverreq.OrgName, pbuf, buflen);
			break;
		}
		case e_RSAdev:
		{
			Mem_Copy(&sProtclHandl.RSAdev, pbuf, buflen);
			break;
		}
		case e_RSAplat:
		{
			Mem_Copy(&sProtclHandl.RSAdev, pbuf, buflen);
			break;
		}
		case e_ReportMediaDatarsp:				//校验多媒体数据上传应答
		{
			Mem_Copy(&sProtclHandl.ReportMediaDatarsp, pbuf, buflen);
			break;
		}
		case e_CarCtlrsp:
		{
			Mem_Copy(&sProtclHandl.CarCtlrsp.sequenceId, pbuf, buflen);
			break;
		}
		case e_ElecMenuRpt:
			Mem_Copy(&sProtclHandl.ElecMenu, pbuf, buflen);
		break;
		case e_ReportDriveDatarsp:
		{
			Mem_Copy(&sProtclHandl.ReportDriveDatarsp, pbuf, buflen);
			break;
		}
#endif

		case e_TrackLocationCtlreq:
		{
			Mem_Copy(&sProtclHandl.TrackLocationCtlreq, pbuf, buflen);
			break;
		}

		case e_TextDownloadreq:
		{
			Mem_Copy(&sProtclHandl.TextDownloadreq, pbuf, buflen);
			break;
		}
#if 0
		case e_SetEventreq_EventType:	//事件设置//设置类型
		{
			Mem_Copy(&sProtclHandl.SetEventreq.EventType, pbuf, buflen);
			break;
		}
		case e_SetEventreq_totalNum:	//事件设置//设置总数
		{
			sProtclHandl.SetEventreq.totalNum = *pbuf;
			break;
		}
		case e_AskDownloadreq:
		{
			sProtclHandl.SetEventreq.totalNum = *pbuf;
			break;
		}
		case e_DemandMenureq_DemandType:
		{
			sProtclHandl.DemandMenureq.DemandType = *pbuf;
			break;
		}
		case e_DemandMenureq_MenuNum:
		{
			sProtclHandl.DemandMenureq.MenuNum = *pbuf;
			break;
		}
		case e_DemandSetCancl:
			Mem_Copy(&sProtclHandl.DemandSetCancelreq, pbuf, buflen);
		break;
		case e_DemandServreq:
		{
			Mem_Copy(&sProtclHandl.DemandMenureq.MenuNum, pbuf, buflen);
			break;
		}
		case e_CallBackreq:
		{
			Mem_Copy(sProtclHandl.CallBackreq, pbuf, buflen);
			break;
		}	
		case e_SetPhonebookreq_BookType:	//设置电话本//类型
		{
			sProtclHandl.SetPhonebookreq.BookType = *pbuf;
			break;
		}
		case e_SetPhonebookreq_ContackNum:	//设置电话本//数量
		{
			sProtclHandl.SetPhonebookreq.ContactNum = *pbuf;
			break;
		}
		case e_CarCtlreq:			
		{
			Mem_Copy(&sProtclHandl.CarCtlreq, pbuf, buflen);
			break;
		}	
		case e_GetDriveDatareq:
		{
			Mem_Copy(&sProtclHandl.GetDriveDatareq, pbuf, buflen);
			break;
		}
		case e_TakePhotoreq:
		{
			Mem_Copy(&sProtclHandl.TakePhotoreqpara, pbuf, buflen); //peijl_1008 undo
			break;
		}	
		case e_SMediaDataCheckreq:
		{
			Mem_Copy(&sProtclHandl.SmediaDataCheckreq, pbuf, buflen);
			break;
		}
		case e_RSPSMediaDataCheckreq:
		{
			Mem_Copy(&sProtclHandl.SMediaDataCheckrsp, pbuf, buflen);
			break;
		}
		case e_ReportSMediareq:
		{
			Mem_Copy(&sProtclHandl.ReportSmediareq, pbuf, buflen);
			break;
		}
#endif
		case e_Authen:
			if(buflen>AUTHENMAXLEN)
				return 1;
			Mem_Copy(&sProtclHandl.Authen, pbuf, buflen);
			sProtclHandl.Authen[buflen]='\0';
		break;
		default:
		break;		
	}
//	OSSemPost(JT808_PrtclHandPrtctSem);
	return 0;
}
/*********************************************************
*Function:       JT808_dataChg
*Description:   808协议转义 
*				0x7e <-> 0x7d+0x02,0x7d<->0x7d+0x01
*Calls:          
*Called By:      
*Input:			flag 表示1:正向转义 0:反向转义
*				pstr 为需要被转义的数据首地址 常指针
*				strlen 转义前数据长度
*Output:        pstr 亦为输出地址
*				*Destlen 转义后数据长度
*Return:        函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
u8 JT808_dataChg(u8 flag, u8 *pstr, u16 strlen, u16 *Destlen)
{
	int i=0;
	int j=0;
	u8 status=0;
	u16 len = strlen;

	if(pstr==NULL || strlen==0)
	{
		DEBUG("turning parameter error!pstr=%x,stringlen=%d",pstr,strlen);
		return 1;
	}

	if(flag)
	{
	//flag==1时候是将0x7e -> 0x7d+0x02, 	0x7d->0x7d+0x01
		for(i=0;i<strlen;i++)
		{
			if(*(pstr+j) == 0x7e)
			{
				memmove(pstr+j+2, pstr+j+1, len-j);
				*(pstr+j) = 0x7d;
				*(pstr+j+1) = 0x02;
				j += 2;
				len ++;
			}
			else if(*(pstr+j) == 0x7d)
			{
				memmove(pstr+j+2, pstr+j+1, len-j);
				*(pstr+j) = 0x7d;
				*(pstr+j+1) = 0x01;
				j += 2;
				len ++;
			}
			else
			{
				j++;
			}
		}
	}
	else
	{
	//flag==0时是将0x7d+0x02->0x7e, 	0x7d+0x01->0x7d
		for(i=0;i<strlen;i++)
		{
			if((*(pstr+j)==0x7d)&&(*(pstr+j+1)==0x02))
			{
				*(pstr+j) = 0x7e;
				j++;
				memmove(pstr+j, pstr+j+1, len-j);
				strlen--;
			}
			else if((*(pstr+j)==0x7d)&&(*(pstr+j+1)==0x01))
			{
				j++;
				memmove(pstr+j, pstr+j+1, len-j);
				strlen--;
			}
			else
			{
				j++;
			}
		}
	}
	*Destlen = j;

	return status;
}
oa_bool ip_is_valued (u8 *ip, u8 len)
{
	u8 i;
	u8 pot=0;
	u16 potdata=0;
	if(ip==NULL)
		return OA_FALSE;
	if(len<7 ||len>15)
		return OA_FALSE;
	if(ip[0]=='.'||ip[len-1]=='.')
		return OA_FALSE;
	for(i=0;i<len-1;i++)
	{
		if(ip[i]=='.'&&ip[i+1]=='.')
		return OA_FALSE;
	}

	for(i=0;i<len;i++)
	{
		if(ip[i]=='.')
		{
			if(potdata>255)
				return OA_FALSE;
			potdata=0;
			pot++;
		}
		else if(ip[i]>='0'&&ip[i]<='9')
			potdata=potdata*10+ip[i]-'0';
		else
			return OA_FALSE;
	}
	if((pot!=3)||(potdata>255))
		return OA_FALSE;

	return OA_TRUE;
}
//判断len长度字符串是否为手机号 格式0~9字符串，或【+】+【0~9】字符串 peijl_20120829 添加
u8 ISStrTelValid(u8 *Tel,u8 len)
{
	u8 i;
	if(Tel==NULL ||0==len)
		return OA_FALSE;
	if((*Tel<'0')||(*Tel>'9')&&(*Tel!='+'))
		return OA_FALSE;
	for(i=1;i<len;i++)
	{
		if((*Tel<'0')||(*Tel>'9'))
			return OA_FALSE;
	}
	return OA_TRUE;
}
u8 WriteLbsCfgPara(Enum_CtrlCfgPara CfgPara, u8 *pValue,u8 len,UpdateModeEnum UpdateMode)
{
	u8 err;
	//oa_bool need_reconn = OA_FALSE;
	#if 0
	if((CfgPara==eCfaParaMax)&&(UpdateMode==UpdateFlash))
	{//未进行参数设置，而备份参数
		OSSemPend(HXCfgParaSem,0,&err);
		Flash_Write_Buffer(NULL,0,SERV_CTRLCFG_ADDR,Sector_Erase);	//擦除配置
		Flash_Write_Buffer((u8 *)&LbsCfgStruct,sizeof(STRUCT_LBS_CFG),SERV_CTRLCFG_ADDR,Write_Mode);	//写入配置	
		OSSemPost(HXCfgParaSem);
		return 0;		
	}
	#endif
	//参数有效性判断
	if(NULL==pValue ||len==0){
		if (UpdateMode != UpdateFlash){
			return 1;
		}
	}
		
	switch(CfgPara)
	{
		case eNET:
		case eNET_sub:
			if(*pValue!=eNET_TCP&&*pValue!=eNET_UDP&&*pValue!=eNET_SMS)
				return 1;
		break;
		case eAPN:
		case eAPN_sub:
			if(len>10)
			return 1;
		break;
		case eAPNUser:
		case eAPNUser_sub:
			if(len>MAXPPPUSERLEN)
			return 1;
		break;
		case eAPNPwd:
		case eAPNPwd_sub:
			if(len>MAXPPPPWDLEN)
			return 1;
		break;
		case eLclTEL: //808协议最大12字节 peijl_130311 添加本机号错误，设默认值
			if(len>12) //808协议最大12字节 peijl_130311 添加本机号错误，设默认值
				return 1;
		break;
		case eServTel:
		case eResetTel:
		case eFactryTel:
		case eSMSTel:
		case eAlarmSMSTel:
		case eMonitTel:
		case eSpeclSMSTel:
			if(len>TELMAXLEN){
				DEBUG("number is err!");
				return 1;
			}
		break;
		case eCarid: 
			if(len>CarIDMAXLEN)
				return 1;
		break;
		case eIP:
		case eIP_sub:
			//if(!ISAscIPValid(pValue,len)){
			if (!ip_is_valued(pValue,len)){
				return 1;
			}
				
		break;
		case eUdpPort:
		case eTcpPort:
			if((len!=4))//||(*(pValue+2)!=0)||(*(pValue+3)!=0)) //>65535
				return 1;
		break;
		case eHtTime:
		case eTcptime:
		case eTcpnum:
		case eUdptime:
		case eUdpnum:
		case eSMSTime:
		case eSMSNum:
		case eRpttime_def:
		case eRpttime_unlog:
		case eRpttime_sleep:
		case eRpttime_emergency:
		case eRptDistanc_def:	
		case eRptDistanc_sleep:
		case eRptDistanc_unlog:
		case eRptDistanc_emergency:
			if(len!=4)
				return 1;
		break;
		case eRptStrategy:
			if((len!=1)||
				(*pValue!=0&&*pValue!=1&&*pValue!=2))
				return 1;
		break;
		case eRptType:
			if((len!=1)||
			(*pValue!=0&&*pValue!=1))
				return 1;
		break;
		case eRptCog:
			if((len!=4)
			||(*pValue>180)||(*(pValue+1)!=0)||(*(pValue+2)!=0)||(*(pValue+3)!=0)) //>108
				return 1;
		break;	
		case eAnswerType:
		break;
		case eMaxCalltime:
		break;
		case eMaxCalltime_month:
		break;
		case eAlarmmaskword:
		break;
		case etxtSMSAlarm:
		break;
		case eAlarmPhoto:
		break;
		case eAlarmPhotoStor:
		break;
		case eKeyAlarm:
		break;
		case eOverspeed:
		break;
		case eOverspeedtime:
		break;
		case eDrivTime:
		break;
		case eDrivTime_day:
		break;
		case eRestTime_min:
		break;
		case eStopcarTime_max:
		break;
		case eImagQuality:
		break;
		case eImagBright:
		break;
		case eImagContrast:
		break;
		case eImagSatuatn:
		break;
		case eImagClor:
		break;
		case eOdometer: //里程表精度 
		break;
		case eCarclor:
		break;
		case ePrivic:
		case eCity:
			if(len!=2)
				return 1;
		break;
		default:
		break;
	}
	//参数设置
	//OSSemPend(HXCfgParaSem,0,&err);
if (UpdateMode == UpdateOnly){
	DEBUG("parameters id:0x%04X", CfgPara);
	switch(CfgPara)
	{
		#if 0
		case eNET:
			LbsCfgStruct.SERV_NETTYPE=*pValue;
		break;
		case eAPN:				
			LbsCfgStruct.ServAPNLen=len;
			Mem_Copy(LbsCfgStruct.ServAPN,pValue,len);
		break;
		#endif
		case eUdpPort:{ //协议不明，端口号暂定义相同
			//Mem_Copy((u8 *)&LbsCfgStruct.ServPort_udp,pValue,len);
			//Mem_Copy((u8 *)&LbsCfgStruct.SubservPort_udp,pValue,len);
			u32 tmp;
			DEBUG("set UdpPort");
			if (len == 4){
				oa_memcpy((u8 *)&tmp, pValue, len);
				if (tmp <= PORT_MAX){
					if (tmp == dev_now_params.server_udp_port){
						PRINT_SAMEPARA;
						break;
					}
					else{
						dev_now_params.server_udp_port = tmp;
						//need_reconn = OA_TRUE;
						plat_paraset = reconn;
					}
				}
				else{
					DEBUG("param err ");
				}
				//oa_memcpy((u8 *)&dev_now_params.server_udp_port, pValue, len);
				//need_reconn = OA_TRUE;
			}
			else {
				DEBUG("param err ");
			}
		}
		break;
		case eTcpPort:{//do not forget reconnect !!!
			//Mem_Copy((u8 *)&LbsCfgStruct.ServPort_tcp,pValue,len);
			//Mem_Copy((u8 *)&LbsCfgStruct.SubservPort_tcp,pValue,len);
			u32 tmp;
			DEBUG("set TcpPort");
			if (len == 4){
				oa_memcpy((u8 *)&tmp, pValue, len);
				if (tmp <= PORT_MAX){
					if (tmp == dev_now_params.server_tcp_port){
						PRINT_SAMEPARA;
						break;
					}
					else{
						dev_now_params.server_tcp_port = tmp;
						//need_reconn = OA_TRUE;
						plat_paraset = reconn;
					}
				}
				else{
					DEBUG("param err ");
				}
			}
			else {
				DEBUG("param err ");
			}
			#if 0
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.server_tcp_port, pValue, len);
				need_reconn = OA_TRUE;
			}
			else {
				DEBUG("param err ");
			}
			#endif
			}
		break;
		case eIP:{
			//LbsCfgStruct.ServIPLen=len;
			//Mem_Copy(LbsCfgStruct.ServIP,pValue,len);
			//LbsCfgStruct.ServIP[len]='\0';
			u8 ip_tmp[SERVER_IP_MAX_LEN] = {0x0};
			DEBUG("set ip");
			if (len <=  SERVER_IP_MAX_LEN){
				oa_memcpy(ip_tmp, pValue, len);
				if (oa_strlen(dev_now_params.m_server_ip) == len){
					if (!oa_strncmp(dev_now_params.m_server_ip, ip_tmp, len)){
						PRINT_SAMEPARA;
						break;
					}	
					else{//not equal
						oa_memset(dev_now_params.m_server_ip, 0x0, sizeof(dev_now_params.m_server_ip));
						oa_memcpy(dev_now_params.m_server_ip, ip_tmp, len);
						//need_reconn = OA_TRUE;
						plat_paraset = reconn;
					}
				}
				else{//not equal
					oa_memset(dev_now_params.m_server_ip, 0x0, sizeof(dev_now_params.m_server_ip));
					oa_memcpy(dev_now_params.m_server_ip, ip_tmp, len);
					//need_reconn = OA_TRUE;
					plat_paraset = reconn;
				}
				#if 0
				oa_memset(&dev_now_params.m_server_ip, 0x0, sizeof(dev_now_params.m_server_ip));
				oa_memcpy((u8 *)&dev_now_params.m_server_ip, pValue, len);
				need_reconn = OA_TRUE;
				#endif
			}
			else{
				DEBUG("param err ");
			}
			}
		break;
		#if 0
		case eAPNUser:
			LbsCfgStruct.ServUserLen=len;
			Mem_Copy(LbsCfgStruct.ServAPNUser,pValue,len);
		break;
		case eAPNPwd:
			LbsCfgStruct.ServPwdLen=len;
			Mem_Copy(LbsCfgStruct.ServAPNPwd,pValue,len);
		break;
		case eNET_sub:
			LbsCfgStruct.SubservNETTYPE=*pValue;
		break;
		case eAPN_sub:
			LbsCfgStruct.SubServAPNLen=len;
			Mem_Copy(LbsCfgStruct.SubservAPN,pValue,len);
		break;
		case eIP_sub:
			LbsCfgStruct.SubservIPLen=len;
			Mem_Copy(LbsCfgStruct.SubservIP,pValue,len);
			LbsCfgStruct.SubservIP[len]='\0';
		break;
		case eAPNUser_sub:
			LbsCfgStruct.SubServUserLen=len;
			Mem_Copy(LbsCfgStruct.SubservAPNUser,pValue,len);
		break;
		case eAPNPwd_sub:
			LbsCfgStruct.SubServPwdLen=len;
			Mem_Copy(LbsCfgStruct.SubservAPNPwd,pValue,len);
		break;
		#endif
		case eLclTEL:{
			u8 tel_tmp[TERM_TEL_NUM_MAX_LEN] = {0x0};
			//LbsCfgStruct.LclTELlen=len;
			//Mem_Copy(LbsCfgStruct.LclTEL,pValue,len);
			oa_memcpy(tel_tmp, pValue, len);
			DEBUG("set LclTEL");
			if (len <= TERM_TEL_NUM_MAX_LEN){
				if (!oa_memcmp((u8 *)&dev_now_params.term_tel_num[0], tel_tmp, 11)) break;
				oa_memset(&dev_now_params.term_tel_num, 0x0, sizeof(dev_now_params.term_tel_num));
				oa_memcpy((u8 *)&dev_now_params.term_tel_num, pValue, len);
				//plat_paraset = rereg;
			}
			else{
				DEBUG("param err ");
			}
		}
		break;
		case eHtTime:
			//Mem_Copy((u8 *)&LbsCfgStruct.HB_time,pValue,len);	//	comment for 808 test
			DEBUG("set hearttime");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.heartbeat_interval, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eTcptime:
			//Mem_Copy((u8 *)&LbsCfgStruct.rsp_time_tcp,pValue,len);
			DEBUG("Tcptime");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.tcp_ack_timeout, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eTcpnum:
			//Mem_Copy((u8 *)&LbsCfgStruct.rsp_num_tcp,pValue,len);
			DEBUG("Tcpnum");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.tcp_retrans_times, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		#if 0
		case eUdptime:
			Mem_Copy((u8 *)&LbsCfgStruct.rsp_time_udp,pValue,len);		
		break;
		case eUdpnum:
			Mem_Copy((u8 *)&LbsCfgStruct.rsp_num_udp,pValue,len);		
		break;
		#endif
		case eSMSTime:
			//Mem_Copy((u8 *)&LbsCfgStruct.rsp_time_sms,pValue,len);
			DEBUG("SMSTime");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.sms_ack_timeout, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eSMSNum:
			//Mem_Copy((u8 *)&LbsCfgStruct.rsp_num_sms,pValue,len);
			DEBUG("SMSNum");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.sms_retrans_times, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eRptStrategy:{
			//Mem_Copy((u8 *)&LbsCfgStruct.ReportStrategy,pValue,len);
			u32 tmp;
			DEBUG("RptStrategy");

			if (len == 4){
				oa_memcpy((u8 *)&tmp, pValue, len);
				if (tmp >= 0 && tmp < 3)	oa_memcpy((u8 *)&dev_now_params.report_strategy, pValue, len);
				
			}
			else {
				DEBUG("param err ");
			}
		}
		break;
		case eRptType:{
			//Mem_Copy((u8 *)&LbsCfgStruct.ReportType,pValue,len);
			u32 tmp;
			DEBUG("RptType");
			if (len == 4){
				oa_memcpy((u8 *)&tmp, pValue, len);
				if (tmp == 0 || tmp == 1)	oa_memcpy((u8 *)&dev_now_params.report_type, pValue, len);
				
			}
			else {
				DEBUG("param err ");
			}
		}
		break;
		case eRpttime_def:
			//Mem_Copy((u8 *)&LbsCfgStruct.DefaultReportTime,pValue,len);
			DEBUG("Rpttime_def");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.default_reporttime, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eRpttime_unlog:
			//Mem_Copy((u8 *)&LbsCfgStruct.unLoginReportTime,pValue,len);
			DEBUG("Rpttime_unlog");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.unlogin_reporttime, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eRpttime_sleep:
			//Mem_Copy((u8 *)&LbsCfgStruct.sleepReportTime,pValue,len);
			DEBUG("Rpttime_sleep");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.sleep_reporttime, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eRpttime_emergency:
			//Mem_Copy((u8 *)&LbsCfgStruct.UrgentReportTime,pValue,len);
			DEBUG("Rpttime_emergency");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.urgent_reporttime, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eRptDistanc_def:	
			//Mem_Copy((u8 *)&LbsCfgStruct.DefaultReportDistanc,pValue,len);
			DEBUG("RptDistanc_def");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.default_reportdistance, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eRptDistanc_sleep:
			//Mem_Copy((u8 *)&LbsCfgStruct.sleepReportDistanc,pValue,len);	
			DEBUG("RptDistanc_sleep");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.sleep_reportdistance, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eRptDistanc_unlog:
			//Mem_Copy((u8 *)&LbsCfgStruct.unLoginReportDistanc,pValue,len);
			DEBUG("RptDistanc_unlog");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.unlogin_reportdistance, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eRptDistanc_emergency:
			//Mem_Copy((u8 *)&LbsCfgStruct.UrgentReportDistanc,pValue,len);
			DEBUG("RptDistanc_emergency");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.urgent_reportdistance, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eRptCog:
			//Mem_Copy((u8 *)&LbsCfgStruct.ReportCog,pValue,len);
			DEBUG("RptCog");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.corner_reportangle, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eServTel:
			//LbsCfgStruct.ServTellen=len;
			//Mem_Copy(LbsCfgStruct.ServTel,pValue,len);
			DEBUG("ServTel");
			if (len < TEL_NUM_MAX_LEN)
			{
				oa_memset(dev_now_params.monitor_platform_num, 0x0, TEL_NUM_MAX_LEN);
				oa_memcpy(dev_now_params.monitor_platform_num, pValue, len);
			}
			else
			{
				DEBUG("param err ");
			}
		break;
		case eResetTel:
			//LbsCfgStruct.ResetTellen=len;
			//Mem_Copy(LbsCfgStruct.ResetTel,pValue,len);
			DEBUG("ResetTel");
			if (len < TEL_NUM_MAX_LEN){
				oa_memset(dev_now_params.reset_num, 0x0, TEL_NUM_MAX_LEN);
				oa_memcpy(dev_now_params.reset_num, pValue, len);
			}
			else{
				DEBUG("param err ");
			}
		break;
		case eFactryTel:
			//LbsCfgStruct.FactySetTellen=len;
			//Mem_Copy(LbsCfgStruct.FactySetTel,pValue,len);
			DEBUG("FactryTel");
			if (len < TEL_NUM_MAX_LEN){
				oa_memset(dev_now_params.restore_factory_settings_num, 0x0, TEL_NUM_MAX_LEN);
				oa_memcpy(dev_now_params.restore_factory_settings_num, pValue, len);
			}
			else{
				DEBUG("param err ");
			}
		break;
		case eSMSTel:
			//LbsCfgStruct.ServSMSTellen=len;
			//Mem_Copy(LbsCfgStruct.ServSMSTel,pValue,len);
			DEBUG("SMSTel");
			if (len < TEL_NUM_MAX_LEN)
			{
				oa_memset(dev_now_params.monitor_platform_sms_num, 0x0, TEL_NUM_MAX_LEN);
				oa_memcpy(dev_now_params.monitor_platform_sms_num, pValue, len);
			}
			else
			{
				DEBUG("param err ");
			}
		break;
		case eAlarmSMSTel:
			//LbsCfgStruct.AlarmSMSTellen=len;
			//Mem_Copy(LbsCfgStruct.AlarmSMSTel,pValue,len);
			DEBUG("AlarmSMSTel");
			if (len < TEL_NUM_MAX_LEN){
				oa_memset(dev_now_params.terminal_sms_num, 0x0, TEL_NUM_MAX_LEN);
				oa_memcpy(dev_now_params.terminal_sms_num, pValue, len);
			}
			else{
				DEBUG("param err ");
			}
		break;
		#if 0
		case eMonitTel:
			LbsCfgStruct.MonitorTellen=len;
			Mem_Copy(LbsCfgStruct.MonitorTel,pValue,len);
		break;
		case eSpeclSMSTel:
			LbsCfgStruct.ServSpecialSMSTellen=len;
			Mem_Copy(LbsCfgStruct.ServSpecialSMSTel,pValue,len);
		break;
		case eAnswerType:
			Mem_Copy((u8 *)&LbsCfgStruct.DevTelAnswerType,pValue,len);		
		break;
		case eMaxCalltime:
			Mem_Copy((u8 *)&LbsCfgStruct.MaxTalkingTime,pValue,len);		
		break;
		case eMaxCalltime_month:
			Mem_Copy((u8 *)&LbsCfgStruct.MaxTalkingTime_month,pValue,len);		
		break;
		#endif
		case eAlarmmaskword:
			//Mem_Copy((u8 *)&LbsCfgStruct.AlarmMaskWord,pValue,len);
			DEBUG("Alarmmaskword");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.alarm_mask, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case etxtSMSAlarm:
			//Mem_Copy((u8 *)&LbsCfgStruct.AlarmSMStextFlag,pValue,len);
			DEBUG("txtSMSAlarm");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.alarm_send_sms_mask, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		#if 0
		case eAlarmPhoto:
			Mem_Copy((u8 *)&LbsCfgStruct.AlarmPhotoFlag,pValue,len);		
		break;
		case eAlarmPhotoStor:
			Mem_Copy((u8 *)&LbsCfgStruct.AlarmPhotoStoreFlag,pValue,len);		
		break;
		case eKeyAlarm:
			Mem_Copy((u8 *)&LbsCfgStruct.KeyAlarmFlag,pValue,len);		
		break;
		#endif
		case eOverspeed:
			//Mem_Copy((u8 *)&LbsCfgStruct.OverSpeed,pValue,len);
			DEBUG("Overspeed");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.max_speed, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eOverspeedtime:
			//Mem_Copy((u8 *)&LbsCfgStruct.OverSpeedTime,pValue,len);
			DEBUG("Overspeedtime");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.speed_duration, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eDrivTime:
			//Mem_Copy((u8 *)&LbsCfgStruct.MaxDriveTime,pValue,len);
			DEBUG("DrivTime");
			if (len == 4){
				
				oa_memcpy((u8 *)&dev_now_params.continuous_drive_time_threshold, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eDrivTime_day:
			//Mem_Copy((u8 *)&LbsCfgStruct.MaxDriveTime_day,pValue,len);
			DEBUG("DrivTime_day");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.day_add_drive_time_threshold, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eRestTime_min:
			//Mem_Copy((u8 *)&LbsCfgStruct.MinRestTime,pValue,len);
			DEBUG("RestTime_min");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.min_rest_time, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		case eStopcarTime_max:
			#if 0//???
			Mem_Copy((u8 *)&LbsCfgStruct.MaxRestTime,pValue,len);		
			if(ReadAlarmPara(StaAlarm0,ALARM_OVERTIME_PARKING)==SET)
			{ //值重设后，重新判断状态
				WriteAlarmPara(RESET,StaAlarm0,ALARM_OVERTIME_PARKING);
			}
			#endif
			DEBUG("StopcarTime_max");
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.max_park_time, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
		break;
		#if 0
		case eImagQuality:
			Mem_Copy((u8 *)&LbsCfgStruct.ImageQuality,pValue,len);		
		break;
		case eImagBright:
			Mem_Copy((u8 *)&LbsCfgStruct.Brightness,pValue,len);	
		break;
		case eImagContrast:
			//char_to_int(pValue,&U32Temp);
			Mem_Copy((u8 *)&LbsCfgStruct.Contrast,pValue,len);		
		break;
		case eImagSatuatn:
			Mem_Copy((u8 *)&LbsCfgStruct.Saturation,pValue,len);	
		break;
		case eImagClor:
			Mem_Copy((u8 *)&LbsCfgStruct.Color,pValue,len);		
		break;
		#endif
		case eOdometer: //里程表初始值
			DEBUG("Odometer");
			//Mem_Copy((u8 *)&LbsCfgStruct.Odometer,pValue,len);		
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.vehicle_odometer, pValue, len);
			}
			else {
				DEBUG("param err ");
			}

			mile_stat_set_value(dev_now_params.vehicle_odometer);
		break;
		case ePrivic:{
			u16 U16Temp = 0;
			char_to_short(pValue,&U16Temp);
			//Mem_Copy((u8 *)&LbsCfgStruct.ProvinceId,pValue,len);	
			DEBUG("Privic");
			if (len == 2){
				if (dev_now_params.vehicle_province_id == U16Temp) break;
				oa_memcpy((u8 *)&dev_now_params.vehicle_province_id, pValue, len);
				plat_paraset = rereg;
			}
			else {
				DEBUG("param err ");
			}

		}
		break;
		case eCity:{
			u16 U16Temp = 0;
			char_to_short(pValue,&U16Temp);
			//Mem_Copy((u8 *)&LbsCfgStruct.CityId,pValue,len);
			DEBUG("City");
			if (len == 2){
				if (dev_now_params.vehicle_province_id == U16Temp) break;
				oa_memcpy((u8 *)&dev_now_params.vehicle_city_id, pValue, len);
				plat_paraset = rereg;
			}
			else {
				DEBUG("param err ");
			}
		}
		break;
		case eCarid:{
			//LbsCfgStruct.CarIdlen=len;
			//Mem_Copy(LbsCfgStruct.CarId,pValue,len);
			u8 temp[VEHICLE_LICENCE_MAX_LEN] = {0x0};
			oa_memcpy(temp, pValue, len);
			DEBUG("Carid");
			if (len < VEHICLE_LICENCE_MAX_LEN){
				if (!oa_memcmp(dev_now_params.vehicle_license, temp, VEHICLE_LICENCE_MAX_LEN)) break;
				oa_memset(dev_now_params.vehicle_license, 0x0, 	VEHICLE_LICENCE_MAX_LEN);
				oa_memcpy(dev_now_params.vehicle_license, pValue, len);
				plat_paraset = rereg;
			}
			else{
				DEBUG("param err ");
			}
		}
		break;
		case eCarclor:{
			DEBUG("Carclor");
			//LbsCfgStruct.CarIdColor=*pValue;
			if (dev_now_params.plate_color == *pValue) break;
			dev_now_params.plate_color = *pValue;
			plat_paraset = rereg;
		}
		break;
		default:{
			DEBUG("not support!");
			return 3;
		}
		break;
	}
}
	if(UpdateMode == UpdateFlash)  			/*需要更新到flash*/
	{
		#if 0
		Flash_Write_Buffer(NULL,0,SERV_CTRLCFG_ADDR,Sector_Erase);	//擦除配置
		Flash_Write_Buffer((u8 *)&LbsCfgStruct,sizeof(STRUCT_LBS_CFG),SERV_CTRLCFG_ADDR,Write_Mode);	//写入配置	
		#endif
		oa_bool ret;
		//DEBUG("traceing!");
		ret = dev_params_save();
		if (ret == OA_TRUE){
			print_key_dev_params();
		}
		else{
			DEBUG("save dev parameters err!");
			return 1;
		}
		//msybe need reconnect!!!
		
	}
	//OSSemPost(HXCfgParaSem);
	return 0;		
}

/*------------------------------------平台请求消息处理函数---------------------------------------*/
/*将参数paramID对应的小端监控平台参数转换成大端，paramID对应的val内容与原参数相同，无后续操作；
参数不同需要设置同时终端可能有相应动作
	peijl_20120828 修改*/
u8 ParaConvertandSet(Enum_CtrlCfgPara paramID,u8 *Srcval,u8 len,u16 *devAct)
{
	u8  U8Temp;
	u16 U16Temp;
	u32 U32Temp;

	if(NULL==Srcval ||NULL==devAct)
		return 1;
	switch(paramID)
	{
		case eIP:
		case eIP_sub:{
			u8 t[16];
			//if(!ISAscIPValid(Srcval,len))
			if (!ip_is_valued(Srcval,len))
				return 1;
			WriteLbsCfgPara(paramID, Srcval,len,UpdateOnly);
			#if 0
			if(eIP==paramID)
				ReadLbsCfgPara(eIP, t,&U8Temp);
			else
				ReadLbsCfgPara(eIP_sub, t,&U8Temp);
			if(U8Temp==len &&(0==memcmp(t,Srcval,len)))
			{;}
			else
			{
				if(eIP==paramID)
				{
					WriteLbsCfgPara(eIP, Srcval,len,UpdateOnly); //不保存，所有参数完成再后保存到flash
					*devAct|=ActionLoginHX;
				}
				else
					WriteLbsCfgPara(eIP_sub, Srcval,len,UpdateOnly);
			}
			#endif
		}
		break;
		case eUdpPort:
		case eTcpPort:
		{u32 t;
			char_to_int(Srcval,&U32Temp);
			if((len!=4)||(U32Temp>65535))
				return 1;
			ReadLbsCfgPara(paramID,(u8 *)&t,&U8Temp);
			if(U32Temp!=t)
			{
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,len,UpdateOnly); //不保存，所有参数完成再后保存到flash
				//*devAct|=ActionLoginHX;//zq
			}
		}
		break;
		case eAPNUser:
		case eAPNPwd:
			WriteLbsCfgPara(paramID,Srcval,len,UpdateOnly);
			//*devAct|=ActionHXPPP|ActionLoginHX;zq
		break;
		case eAPNUser_sub:
		case eAPNPwd_sub:
			WriteLbsCfgPara(paramID,Srcval,len,UpdateOnly);
		break;
		case eAPN:
		case eAPN_sub:
		break;
	//	case eLclTEL:
	//		WriteLbsCfgPara(paramID,Srcval,len,UpdateOnly);
	//		*devAct|=ActionHXPPP|ActionLoginHX;
	//	break;
		case eServTel:
		case eResetTel:
		case eFactryTel:
		case eSMSTel:
		case eAlarmSMSTel:
		case eMonitTel:
		case eSpeclSMSTel:
			if(!ISStrTelValid(Srcval,len)){
				DEBUG("number err!");
				return 1;
			}
				
			WriteLbsCfgPara(paramID,Srcval,len,UpdateOnly);
		break;
		case eCarid:   //peijl_130220 修改：车辆基本参数改变时，要先注销再重注册
			WriteLbsCfgPara(paramID,Srcval,len,UpdateOnly);
			//*devAct|=ActionLoginHX|ActionUnregistServ;zq
		break;
		case eRptStrategy:
			if(4!=len)
				return 1;
			char_to_int(Srcval,&U32Temp);
			if(0==U32Temp ||1==U32Temp ||2==U32Temp)
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly);
			else
				return 1;
		break;
		case eRptType: //0,1
		case eAnswerType: //0自动接听 1acc on自动，off手动
			if(4!=len)
				return 1;
			char_to_int(Srcval,&U32Temp);
			if(0==U32Temp ||1==U32Temp)
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly);
			else		 
				return 1;
		break;
		case eRptCog:
			if(4!=len)
				return 1;
			char_to_int(Srcval,&U32Temp);
			if(U32Temp<180)
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly);
			else
				return 1;
		break;
		case eHtTime:
		case eTcptime:
		case eTcpnum:
		case eUdptime:
		case eUdpnum:
		case eSMSTime:
		case eSMSNum:
		case eRpttime_def:
		case eRpttime_unlog:
		case eRpttime_sleep:
		case eRpttime_emergency:
		case eRptDistanc_def:	
		case eRptDistanc_sleep:
		case eRptDistanc_unlog:
		case eRptDistanc_emergency:
		case eMaxCalltime:
		case eMaxCalltime_month:
		case eAlarmmaskword:
		case etxtSMSAlarm:
		case eAlarmPhoto:
		case eAlarmPhotoStor:
		case eKeyAlarm:
		case eOverspeed:
		case eOverspeedtime:
		case eDrivTime:
		case eDrivTime_day:
		case eRestTime_min:
		case eStopcarTime_max:
		case eOdometer: //里程表精度 
			if(4!=len)
				return 1;
			char_to_int(Srcval,&U32Temp);
			//DEBUG(" set params!");
			WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly); //不保存，所有参数完成再后保存到flash
		break;
#if 0
		case eImagQuality:
			if(4!=len)
				return 1;
			char_to_int(Srcval,&U32Temp);
			if(U32Temp>0&&U32Temp<=10)
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly); //不保存，所有参数完成再后保存到flash
			else
				return 1;
		break;
		case eImagBright:
		case eImagContrast:
		case eImagSatuatn:
			if(4!=len)
				return 1;
			if(U32Temp>=0&&U32Temp<=127)
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly); //不保存，所有参数完成再后保存到flash
			else
				return 1;
		break;
		case eImagClor:
			if(4!=len)
				return 1;
			if(U32Temp>=0&&U32Temp<=255)
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly); //不保存，所有参数完成再后保存到flash
			else
				return 1;
		break;
#endif
		case ePrivic:
		case eCity:
			char_to_short(Srcval,&U16Temp);
			WriteLbsCfgPara(paramID,(u8 *)&U16Temp,2,UpdateOnly); //不保存，所有参数完成再后保存到flash
		break;
		case eCarclor:
			WriteLbsCfgPara(paramID, Srcval,1,UpdateOnly); //不保存，所有参数完成再后保存到flash
		//	*devAct|=ActionUnregistServ;
		break;
		default:
			return 1;
		break;
	}	
	return 0;
}
/*********************************************************
*Function:		check_set_dev_param
*Description:	平台下发设置终端参数请求解析过程
*Calls:          
*Called By:      
*Input:			
*				pbuf 是消息体首地址
*				pbuflen 是组成的报文长度
*Output:
*Return:		ActionOK
*Others:         
*********************************************************/
static u8 ServReq_SetParam(u8 *pbuf, const u16 buflen)
{
	u8 paraNum=0;
	u8 i=0;
	u8 len=0; //参数长
	u16 devAct=0; //参数设置时终端动作
	u8 Ary[200]={0}; //peijl_1012  加入ProtocolHandle必要?
	u16 alllen=buflen;
	u32 paramID;
	u8 sta=1;
	u8 ret = 0;
	if(pbuf==NULL)
	{
		DEBUG("check_set_dev_param param error!");
		return 1;
	}
	//空参数列表
	if(buflen==1&&*pbuf==0)
		return 0;;

	paraNum=*pbuf++; //参数总数
	alllen--;
	for(i=0;(i<paraNum)&&(alllen>=5);i++)
	{
		if(pbuf==NULL)
			return 1;
		memcpy(Ary,pbuf,4);  //ID
		char_to_int(Ary, &paramID);
		len=*(pbuf+4);		  //LEN
		if(len>0)
		{
			memcpy(Ary,(pbuf+5),len);
			sta	=ParaConvertandSet(paramID,(pbuf+5),len,&devAct);
			if(ActionOK==sta)
			{				
				pbuf+=(5+len);
			}
			else
				break;
		}
		alllen-=(5+len);
	}
	//所有参数写入flash,设置参数后终端相应动作
	ret = WriteLbsCfgPara(eCfaParaMax, NULL,0, UpdateFlash); //save it
	if (ret == 3) return 3;//means unsupport
//	WriteLinkTaskPara(devAct,ActionTypePara,SET);zq
	return sta;
}
/*********************************************************
*Function:       JT808MsgRsp_Send
*Description:  	 终端对平台请求发送应答报文
				无需平台回复
*Calls:          
*Called By:      
*Input:         Sendbuf 发送数据首址
				Sendlen 发送数据长度
*Output:       
*Return:         函数执行结果，对应enum _REQRSP_RESULT
				 平台应答结果：enum 0~4
				 ParaError 参数错误或组包错误
				 ServErr	网络问题
				 DevError
*Others:         Sendlen段的数据不能改变
	做成	20130109

*********************************************************/
u8 JT808MsgRsp_Send(u16 DevMsgId,u16 totalPackt,u16 SubPackt/*,u8 *Sendbuf,u16 Sendbuflen*/)
{
	u16 Senddatalen=0;

	u8 status;
	u8 err;
	u8 SeqId[2];	//发送流水号
	//u8 *pbuf=Sendbuf;	//发送缓冲区
	u16 U16Temp;
	u16 ret;
#if 0
	//重发机制
	u32 Retry=0;
	u32 RetryNum=0;
	u32 RetryWaittim=0; //重发等待时间T(n)=T(n-1)*n
	//CPU_SR cpu_sr;
#endif
	u8 pbuf[DATA_MAX_LEN] = 0;
#if 0
	if(Sendbuf==NULL)
	{
		DEBUG("JT808MsgRsp_Send param error!");
		return RspErrOther;
	}
#endif

	if (0 == JT808Msg_Build(DevMsgId, totalPackt, SubPackt, pbuf, DATA_MAX_LEN, &U16Temp)){	
		if (U16Temp > DATA_MAX_LEN){
			OA_DEBUG_USER(" data is too long!");
			return RspErrOther;
		}

		ret = escape_copy_to_send(pbuf, U16Temp);
		if (ret > 0){
			oa_soc_send_req();//check datas in buffer & send	
		}
		return 0;
	}
	else{
		DEBUG(" pacakge build error!");
		return RspErrOther; 
	}

#if 0 //I do it in function escape_copy_to_send
	//给包加上流水号,流水号第加
	Read_ProtclHandl(eDevSeqid,SeqId,&U16Temp);
	memcpy(pbuf+11,SeqId,2);

	memcpy(SeqId,pbuf+11,2);	 //还原

	//校验，除标示头尾和校验本身
	if(1==XOR_Check(pbuf+1, Senddatalen-3,(pbuf+Senddatalen-2)))
		return RspErrOther;
	//对标示头和尾之外的包数据进行转义
	status = JT808_dataChg(1, pbuf+1, Senddatalen-2, &U16Temp);
	Senddatalen=U16Temp+2; //包长
			#if 0
			{//TEST
			u16 test;
			DEBUG(PrintDebug,"终端RSP发送包 %d!\r\n",Senddatalen);
			for(test=0;test<Senddatalen;test++)
			{
			DEBUG(PrintInfo,"0x%02x ",*(pbuf+test));
			}
			DEBUG(PrintInfo,"\r\n");
			}
			#endif	
/* 发送包 */
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif
#if 0
	//取重发机制参数
	RetryNum=JT808_UploadRetrynum(); RetryNum=2; //peijl_130227 暂定
	RetryWaittim=JT808_UploadOvertime();RetryWaittim=30;	

	//发送协议包
	do{
		//未登陆平台，普通数据上传放弃
		#if 0
		ReadLinkTaskPara(&U16Temp,NetStatusPara);
		if(!(U16Temp&LinkHXSeverOk))	
		{
			if((REGISTERS!=DevMsgId) &&(LOGIN!=DevMsgId))
			{
				status=ServUnlog_SendErr;
				break;
			}
		}
		#endif				
		status = WCM_IPSend(SYS_CHANNEL, pbuf,Senddatalen);
		if(status == WCM_OK)
		{
			status=RspOK;	
			break;				
		}
		else if(WCM_INPUT_ERR==	status)
		{
			status=RspErrOther;
			break;
		}
		else
		{
			Retry++;
			status=ServUnlog_SendErr; //发送有问题
		}
		RetryWaittim=(Retry+1)*RetryWaittim; //JT808超时等待机制
	}while(Retry<RetryNum);

	return status;
#endif
}
static u8 ServReq_DevControl(u8 *pmsgbody, u16 msgbodylen)
{
	u8 cmd;
	u16 i=0;
	u16 len=0;
	u16 Paralen = 0;
	u8 *p=NULL;
	u8 step=0;
	if(pmsgbody==NULL)
	{
		DEBUG("LBS_DevCtlreq commandId error!");
		return 1;
	}
	if(msgbodylen==0)
		return 0;

	cmd=*pmsgbody++;
	switch(cmd)
	{
		case eOnlineUpgrad_Ctrl:{
			oa_uint8 *p0 = NULL;
			oa_uint8 *p1 = NULL;
			oa_uint8 *p2 = NULL;
			oa_uint8 *p3 = NULL;
			oa_uint8 *p4 = NULL;
			oa_uint8 *p5 = NULL;
			oa_uint8 *p6 = NULL;
			oa_uint8 *p_p = NULL;
			oa_uint8 len;
			DEBUG("wireless_updata");
			//extract paras
			DEBUG("param:%s", pmsgbody);
			memset(&up_paras, 0x0, sizeof(up_paras));
			p_p = pmsgbody;
			for(i = 0;i < 11;i++){
				p = oa_strstr(p_p, ";");
				if (p){
					if (i == 1) p0 = p;//user name
					else if (i == 2) p1 = p;//password
					else if (i == 3) p2 = p;//ip
					else if (i == 4) p3 = p;//tcp port
					else if (i == 5) p6 = p;//udp port
					else if (i == 8) p4 = p;//firmware
					else if (i == 9) p5 = p;
					p_p = p+1;
					continue;
				}
				else{
					DEBUG("paras err");
					goto control_fail;
				} 
			}
			len = p1 - (p0+1);
			if (len > 0 && len < SERVER_IP_MAX_LEN){
				oa_memcpy(up_paras.usr, p0+1, len);
			}else{
				DEBUG("paras err");
				goto control_fail;
			}
			len = p2 - (p1+1);
			if (len > 0 && len < SERVER_IP_MAX_LEN){
				oa_memcpy(up_paras.pw, p1+1, len);
			}else{
				DEBUG("paras err");
				goto control_fail;
			}
			len = p3 - (p2+1);
			if (len > 0 && len < SERVER_IP_MAX_LEN){
				oa_memcpy(up_paras.ip, p2+1, len);
				if(!ip_is_valued(up_paras.ip, oa_strlen(up_paras.ip))){
					DEBUG("ip err");
					goto control_fail;
				}
			}else{
				DEBUG("paras err");
				goto control_fail;
			}
			
			char_to_short(p3+1, &up_paras.port);
			len = p6 - (p3+1);
			if (len > 0){
				oa_memcpy(up_paras.port_str, p3+1, len);
				if (!oa_is_digit_str(up_paras.port_str, oa_strlen(up_paras.port_str))){
					DEBUG("paras err");
					goto control_fail;
				}
			}else{
				DEBUG("paras err");
				goto control_fail;
			}
			
			len = p5 - (p4+1);
			if (len > 0){
				oa_memcpy(up_paras.fw, p4+1, len);
			}
			else{
				DEBUG("paras err");
				goto control_fail;
			}

			control_type = wireless_update;
			break;
		}
		
		case eSpclServ_Ctrl:{
			oa_uint8 *p0 = NULL;
			oa_uint8 *p1 = NULL;
			oa_uint8 *p_p = NULL;
			oa_uint8 len;
			DEBUG("conn_to_specified_server");
			//extract paras
			DEBUG("param:%s", pmsgbody);
			oa_memset(&up_paras, 0x0, sizeof(up_paras));
			p_p = pmsgbody;
			for(i = 0;i < 7;i++){
				p = oa_strstr(p_p, ";");
				if (p){
					if (i == 4) p0 = p;
					else if (i == 5) p1 = p;
					p_p = p+1;
					continue;
				}
				else{
					DEBUG("paras err");
					goto control_fail;
				}
			}
			len = p1 - (p0+1);
			if (len > 0 && len < SERVER_IP_MAX_LEN){
				oa_memcpy(up_paras.ip, p0+1, len);
				if(!ip_is_valued(up_paras.ip, oa_strlen(up_paras.ip))){
					DEBUG("ip err");
					goto control_fail;
				}
			}else{
				DEBUG("paras err");
				goto control_fail;
			}

			char_to_short(p1+1, &up_paras.port);
			control_type = conn_to_specified_server;
			break;
		}
		case eDevPowoff_Ctrl:	//终端关机
		{
			DEBUG("power down");
			control_type = term_powerdown;
			break;
		}

		case eDevReset_Ctrl:	//终端复位
		{
			//参数保存flash
			//WriteLinkTaskPara(ActionResetLbs,ActionTypePara,SET);
			DEBUG("term reset");
			control_type = term_reset;
			break;
		}

		case eDevFactoryset_Ctrl:	//恢复出厂设置
		{
			//断开平台连接
			//参数保存flash
			//接收通话
			control_type = factory_setting;
			//factory_set();
			//oa_module_restart(NULL);
			break;
		}
		case eCloseDataCommu_Ctrl:	//关闭数据通信
		{  
			control_type = turnoff_datatraffic;
			break;
		}
		case eCloseWCMCommu_Ctrl:	//关闭无限通信
		{ 
			control_type = turnoff_allwireless;
			break;
		}
		default:
		{
	
			DEBUG("LBS_DevCtlreq commandId error!");
			return 0;
		}
	}
	
	return 0;
control_fail:
	return 1;
}
/*********************************************************
*Function:     check_track_location_ctl
*Description:  平台请求：临时位置信息跟踪
*Calls:          
*Called By:      
*Input:         pmsgbody 查询消息体首地址
				msgbodylen 查询消息体长度
*Output:        
*Return:        函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8 ServReq_GetPositionData_Period(u8 *pmsgbody, u16 msgbodylen)
{	
	if((pmsgbody==NULL)||(msgbodylen<2))
	{
		DEBUG("check_track_location_ctl param error!\r\n");
		return 1;
	}
	DEBUG("Period position trace:");
	DEBUG("    Inteval is    0x%02x%02x    Limit time is 0x%02x%02x%02x%02x",*pmsgbody,
	*(pmsgbody+1),*(pmsgbody+2),*(pmsgbody+3),*(pmsgbody+4),*(pmsgbody+5));
	Write_ProtclHandl(e_TrackLocationCtlreq, pmsgbody,msgbodylen);
	return 0;
}
/*********************************************************
*Function:     ServReq_Textinfo
*Description:  校验平台下发文本信息下发请求
*文本信息以中心短信格式发送调度屏
*Calls:          
*Called By:      
*Input:         pprotHandle 句柄的指针
				pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:        函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8 ServReq_Textinfo(u8 *pbuf, u16 buflen)
{
	u8 U8Temp=0;
	u8 type=0;
	
	if((pbuf==NULL)||(buflen==0))
	{
		//DEBUG(PrintInfo,"check_text_download param error!\r\n");
		DEBUG("check_text_download param error!");
		return 1;
	}
	
	if(0)
	{
	//处理
	return 0;
	}
	else
	{
	type=*pbuf++;	//文本处理方式,暂未分类 bit	
	if(buflen>1) //peijl_20121229 修改：空文本不处理
	{	
		#if 0
		if(type&0x10)
		{
			u16 u16temp;
			u16temp = buflen-1;
			//zjw_show_soon_mess(0,pbuf,u16temp);
			zjw_show_adv_mess(0,pbuf,u16temp);
			Flash_Write_Buffer(NULL,0,ADV_LED_DATA_ADDR,Sector_Erase);
			Flash_Write_Buffer((u8 *)&u16temp,sizeof(u16temp),ADV_LED_DATA_ADDR,Write_Mode);
			Flash_Write_Buffer(pbuf,u16temp,ADV_LED_DATA_ADDR+sizeof(u16temp),Write_Mode);
//		u8 buf[12]={0};
//		buf[0]=0x10;
//		buf[1]=0x01;
//		buf[2]=0x01;
//		buf[3]=0x11;
//		buf[4]=0x11;
//		buf[5]=0x11;
//	
//		buf[6]=0x14;
//		buf[7]=0x12;
//		buf[8]=0x30;
//		buf[9]=0x11;
//		buf[10]=0x11;
//		buf[11]=0x11;
//		zjw_send_mess(0,0,SHOW_TYPE1,&buf[0],&buf[6],pbuf,buflen-1);
		}
		#endif
		if(type&0x0D)
		{
			#if SCREEN_N990>0
			{
				u8 Time[6]={0};
				app_GetPosinf(Time,GPSTime,BCD_Code);
				SScrn_SMS_Send('E',"000",3,Time,pbuf,buflen-1); //短信格式？？
			}
			#else
				SScrn_CenterSMS_Send(pbuf,buflen-1);
			#endif 
		}
	}
	return ActionOK;
	}
}
/*********************************************************
*Function:      handle_aritifial_ack_alarm()
*Description:  handle artifial ack alarm
*Return:        void
*Others:         
*********************************************************/
u8 handle_aritifial_ack_alarm(u8 *pbuf, u16 buflen)
{
	u32 alarm_bit;

	if (NULL == pbuf || buflen == 0){
		DEBUG("handle_aritifial_ack_alarm param error!");
		return 1;
	}

	char_to_int(pbuf+2, &alarm_bit);
	if (alarm_bit & ALARM_EMERGENCY_k){
		DEBUG("confirm ALARM_EMERGENCY_k");
		if (ReadAlarmPara(StaAlarm0, ALARM_EMERGENCY_k) == SET){
			WriteAlarmPara(RESET, StaAlarm0, ALARM_EMERGENCY_k);
			return 0;
		}
		else return 1;
	}
	else if (alarm_bit & ALARM_ENTER_AREA){
		DEBUG("confirm ALARM_ENTER_AREA");
		if (ReadAlarmPara(StaAlarm0, ALARM_ENTER_AREA) == SET){
			WriteAlarmPara(RESET, StaAlarm0, ALARM_ENTER_AREA);
			return 0;
		}
		else return 1;
	}
	else if (alarm_bit & ALARM_TOUCH_LINE_k){
		DEBUG("confirm ALARM_TOUCH_LINE_k");
		if (ReadAlarmPara(StaAlarm0, ALARM_TOUCH_LINE_k) == SET){
			WriteAlarmPara(RESET, StaAlarm0, ALARM_TOUCH_LINE_k);
			return 0;
		}
		else return 1;
	}

	return 1;
}
/*********************************************************
*Function:     set_round_area
*Description:  下发圆形区域
*Calls:          
*Called By:      
*Input:         pprotHandle 句柄的指针
				pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:        函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8 set_round_area(u8 *pbuf, u16 buflen)
{	
	u8 	option;//操作类型  0:更新  1:追加 2:修改
	u8 area_num;
	u16 read_len;
	u8 i;
	u8 ret;

	if((pbuf==NULL)||(buflen==0))
	{
		//DEBUG(PrintInfo,"check_car_ctl param error!\r\n");
		DEBUG("check_car_ctl param error!");
		return 1;
	}
	//DEBUG(PrintDebug,"设置圆形区域\r\n");
	
	option =  *pbuf++;
	area_num = 	*pbuf++;

	switch (option){
		case 0:{
			DEBUG("设置圆形区域:更新");
		}break;
		case 1:{
			DEBUG("设置圆形区域:追加");
		}break;
		case 2:{
			DEBUG("设置圆形区域:修改");
		}break;
		default:{
			DEBUG("设置圆形区域:错误");
			return 1;
		}break;
	}
	
	DEBUG("区域数目:%d buflen:%d", area_num, buflen);
	if (area_num > MAX_AREA_SUM){
		if (option == 0){
			DEBUG("area num is too large, only update 24st");
			area_num = 24;
		}
		else if (option == 1 || option == 2){
			DEBUG("area num is too large, don't do it");
			return 1;
		}
		
	}
	
	for(i=0;i<area_num;i++)
	{
		ret = SaveAreaData(pbuf,Circular_Area,option,&read_len);  //保存
		if(option == 0)	 //如果是更新操作，只在第一次清除，后面则改为追加
		{
			option = 1;//
		}
		pbuf+=read_len;
		if (ret == 1){
			DEBUG("已成功处理前%d个区域", i);
			return 1;
		}
	}
	return ret;
}
/*********************************************************
*Function:     set_rect_area
*Description:  下发矩形区域
*Calls:          
*Called By:      
*Input:         pprotHandle 句柄的指针
				pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:        函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8 set_rect_area(u8 *pbuf, u16 buflen)
{
	u8 	option;//操作类型  0:更新  1:追加 2:修改
	u8 area_num;
	u16 read_len;
	u8 ret;
	u8 i;

	if((pbuf==NULL)||(buflen==0))
	{
		//DEBUG(PrintInfo,"check_car_ctl param error!\r\n");
		DEBUG("check_car_ctl param error!");
		return 1;
	}
	//DEBUG(PrintDebug,"设置矩形区域\r\n");
	option =  *pbuf++;
	area_num = 	*pbuf++;
	
	switch (option){
		case 0:{
			DEBUG("设置矩形区域:更新");
		}break;
		case 1:{
			DEBUG("设置矩形区域:追加");
		}break;
		case 2:{
			DEBUG("设置矩形区域:修改");
		}break;
		default:{
			DEBUG("设置矩形区域:错误");
			return 1;
		}break;
	}
	
	DEBUG("区域数目:%d buflen:%d", area_num, buflen);
	if (area_num > MAX_AREA_SUM){
		if (option == 0){
			DEBUG("area num is too large, only update 24st");
			area_num = 24;
		}
		else if (option == 1 || option == 2){
			DEBUG("area num is too large, don't do it");
			return 1;
		}
		
	}
	
	for(i=0;i<area_num;i++)
	{
		ret = SaveAreaData(pbuf,Rectangle_Area,option,&read_len);  //保存
		if(option == 0)	 //如果是更新操作，只在第一次清除，后面则改为追加
		{
			option = 1;//
		}
		pbuf+=read_len;
		if (ret == 1){
			DEBUG("已成功处理前%d个区域", i);
			return 1;
		}
	}
	return ret;
}
	
/*********************************************************
*Function:     set_poly_area
*Description:  下发多边形区域
*Calls:          
*Called By:      
*Input:         pprotHandle 句柄的指针
				pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:        函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8 set_poly_area(u8 *pbuf, u16 buflen)	
{
	u8 	option;//操作类型  0:更新  1:追加 2:修改
	u8 area_num;
	u16 read_len;
	u8 i;
	u8 ret;

	if((pbuf==NULL)||(buflen==0))
	{
		//DEBUG(PrintInfo,"check_car_ctl param error!\r\n");
		DEBUG("check_car_ctl param error!");
		return 1;
	}
	//DEBUG(PrintDebug,"设置多边形区域\r\n");
	option =  *pbuf++;
	area_num = 	*pbuf++;

	switch (option){
		case 0:{
			DEBUG("设置多边形区域:更新");
		}break;
		case 1:{
			DEBUG("设置多边形区域:追加");
		}break;
		case 2:{
			DEBUG("设置多边形区域:修改");
		}break;
		default:{
			DEBUG("设置多边形区域:错误");
			return 1;
		}break;
	}
	
	DEBUG("区域数目:%d buflen:%d", area_num, buflen);
	if (area_num > MAX_AREA_SUM){
		if (option == 0){
			DEBUG("area num is too large, only update 24st");
			area_num = 24;
		}
		else if (option == 1 || option == 2){
			DEBUG("area num is too large, don't do it");
			return 1;
		}
		
	}
	
	for(i=0;i<area_num;i++)
	{
		ret = SaveAreaData(pbuf,Poly_Area,option,&read_len);  //保存
		if(option == 0)	 //如果是更新操作，只在第一次清除，后面则改为追加
		{
			option = 1;//
		}
		pbuf+=read_len;
		if (ret == 1){
			DEBUG("已成功处理前%d个区域", i);
			return 1;
		}
	}
	return ret;
}
/*********************************************************
*Function:     del_area_message
*Description:  删除区域
*Calls:          
*Called By:      
*Input:         pprotHandle 句柄的指针
				pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:        函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8 del_area_message(u8 *pbuf, u16 buflen,u8 area_type)	
{
	u8 area_num;
	u8 i;

	if((pbuf==NULL)||(buflen==0))
	{
		//DEBUG(PrintInfo,"check_car_ctl param error!\r\n");
		DEBUG("check_car_ctl param error!");
		return 1;
	}
	
	area_num = 	*pbuf++;
	if (area_num > MAX_AREA_SUM){
		DEBUG("area num is too large");
		return 1;
	}

	switch (area_type){
		case Circular_Area:{
			DEBUG("删除圆形区域数目:%d", area_num);
		}break;
		case Rectangle_Area:{
			DEBUG("删除矩形区域数目:%d", area_num);
		}break;
		case Poly_Area:{
			DEBUG("删除多边形区域数目:%d", area_num);
		}break;
		default:{
			DEBUG("删除区域出错");
		}break;
	}
	
	if(area_num)
	{
		for(i=0;i<area_num;i++)
		{
			 DelAreaData(pbuf,area_type);  //删除区域
			 pbuf+=4;
		}
	}
	else
	{
		DelAreaData(NULL,area_type);//删除所有区域
	}
	return 1;
}

/*********************************************************
*Function:		ServReq_808data_handle
*Description:	对监控平台请求命令的处理及应答
*				1.简单处理在本函数完成
*				2.需任务调度做通知，并传参数
*				3.请求包应答
*Calls:          
*Called By:		JT808_PrtclAnaly() 
*Input:         ServReqMsgid 平台请求消息id
				msgbody	请求消息的消息体首址
				msgbodylen 请求消息的消息体长度
*Output:        
*Return:        处理结果 返回0表示成功
*Others:         
*********************************************************/
u8 JT808_ServReq_handle(u16 ServReqMsgid,u8 *msgbody,u16 msgbodylen/*,u8 *sendbuf,u16 sendbuflen*/)
{
	u8 	status = 0;
	u16 senddatalen=0; //应答或发送数据长度
	oa_bool need_reconn = OA_FALSE;
	if(msgbody==NULL)
	{
		DEBUG(" check_msg_rsp parameter error!");
		return RspErrOther;
	}
	//消息体处理
	//JT808_ServReq_MSG();
	switch(ServReqMsgid){
		case SET_DEV_PARAM:{//platform request to set device paramaters & device ack common 
			//DEBUG(" Recieve parameter-setting requry from moniter server.");
			status = ServReq_SetParam(msgbody,msgbodylen);	//处理
			Write_ProtclHandl(eRsp2ServSeq,&status,1); //peijl_20120918 添加：请求处理结果
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/); //PEIJL_130109 修改
			break;
		}
		case GET_DEV_PARAM:{//platform request to get device paramaters
			DEBUG(" Recieve parameter-getting requry from moniter srever.");
			JT808MsgRsp_Send(GET_DEV_PARAM_rsp,1,0/*,sendbuf,sendbuflen*/);
			break;
		}
		case DEV_CONTROL:{//device control
			status = ServReq_DevControl(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/);
			break;
		}
		case GET_LOCATION:{//get device gps info
			status=JT808MsgRsp_Send(GET_LOCATION_rsp,1,0/*,sendbuf,sendbuflen*/);
		}
		break;
		case TRACK_LOCATION_CTL:{
			status = ServReq_GetPositionData_Period(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/);
			break;
		}
//status=3 为暂时处理
		case ARTIFICIAL_ACK_ALARM:{
			status = handle_aritifial_ack_alarm(msgbody, msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/);
		}break;
		case TEXT_DOWNLOAD:
		{
			status = ServReq_Textinfo(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/);
			break;
		}
		#if 0
		case SET_EVENT:
		{
			status = ServReq_SetEvent(msgbody,msgbodylen);
			status=3;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			break;
		}
		case ASK_DOWNLOAD:
		{
			status = ServReq_AskQuestion(msgbody,msgbodylen);
			status=3;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			break;
		}
		case DEMAND_MENU:
		{
			status = ServReq_InfomationMenu(msgbody,msgbodylen);
			status=3;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			break;
		}
		case DEMAND_SERVER:
		{
			status = ServReq_MessagServic(msgbody,msgbodylen);
			status=3;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			break;
		}
		case CALL_BACK:
		{
			status = ServReq_callCtrl(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			break;
		}
		case SET_PHONEBOOK:
		{
			status = check_set_phonebook(msgbody,msgbodylen);
			status=3;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			break;
		}
		case CAR_CTL:
		{
			status = check_car_ctl(msgbody,msgbodylen);
			status=3;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(CAR_CTL_rsp,1,0,sendbuf,sendbuflen);
			break;
		}
		#endif
		case SET_ROUND_AREA:
			status = set_round_area(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/);
		break;
		case DEL_ROUND_AREA:
			del_area_message(msgbody,msgbodylen,Circular_Area);
			status=0;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/);
		break;
		case SET_SQUARE_AREA:
			status = set_rect_area(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/);
		break;
		case DEL_SQUARE_AREA:
			del_area_message(msgbody,msgbodylen,Rectangle_Area);
			status = 0;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/);
		break;
		case SET_POLYGON_AREA:
			status = set_poly_area(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/);
		break;
		case DEL_POLYGON_AREA:
			del_area_message(msgbody,msgbodylen,Poly_Area);
			status=0;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/);
		break;
		#if 0
		case SET_LINE:
		case DEL_LINE:
		{
//			status = check_car_ctl(msgbody,msgbodylen);
			status = 3;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			break;
		}
		case GET_DRIVE_DATA:
		{
		//	status = check_get_drive_data(msgbody,msgbodylen);
			status = 3;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			//set cmd for data uplowd
			break;
		}
		case DOWNLOAD_DRIVE_DATA:
		{
		//	status = check_download_drive_data(msgbody,msgbodylen);
			status = 3;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			break;
		}
		case TAKE_PHOTO:
		{
			status = ServReq_StartPhoto(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			break;
		}
		case SMEDIA_DATA_CHECK:
		{
			status = check_smedia_data_check(msgbody,msgbodylen);
			status=3;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
//			{u8 tm[2]={0}; //peijl_test
//			Write_ProtclHandl(e_RSPSMediaDataCheckreq,tm,2);
//			JT808_SendMsg(SMEDIA_DATA_CHECK_rsp,1,0,sendbuf,sendbuflen);
//			}
			break;
		}
		case REPORT_SMEDIA:
		{
		//	status = check_report_smedia(msgbody,msgbodylen);
			status=3;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			break;
		}
		case START_RECORD:
		{
			status = ServReq_StartRecord(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			break;
		}
		case DOWNLOAD_DATA:
			status = ServReq_CanData(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
		break;
	#if SCREEN_N990>0
		case CALLCAR_ORDER:
			status = ServReq_CallcarOrder(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			if(0==status)
			{	
				STRUCT_CALLCARORDER *p=(STRUCT_CALLCARORDER*)sendbuf;
				Read_ProtclHandl(e_CallcarOrder,sendbuf,&sendbuflen);
//				gOrderInOperatflag=1;
				Mem_Copy(gLastOrder,p->OrderID,50);
				SchedulScrn_CallforcarSend("8888888888",10,p->OrderContent,p->OrderContentlen); //ID固定
			}
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
		break;
		case CALLCAR_ORDER_CONFIRM:
			status = ServReq_CallcarOrderInfo(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			if(0==status)
			{	STRUCT_CALLCARORDER *p=(STRUCT_CALLCARORDER*)sendbuf;
				Read_ProtclHandl(e_CallcarInfoOrder,sendbuf,&sendbuflen);
//				gOrderInOperatflag=2;
				DEBUG(PrintDebug,"---------tel len:%d \r\n",strlen(p->OrderTel));
				p->OrderContent[p->OrderContentlen]='\0'; 
				DEBUG(PrintDebug,"---------内容长度:%d \r\n",p->OrderContentlen); 
				DEBUG(PrintDebug,"---------内容:%s \r\n",p->OrderContent); 
				SchedulScrn_CallforcarInfoSend("8888888888",10,p->OrderTel,strlen(p->OrderTel),p->OrderContent,p->OrderContentlen); //ID固定
			}
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			//peijl_test 暂定对应
			*sendbuf=1; //1成功 2失败
			Write_ProtclHandl(e_CallcarAccept,sendbuf,1);
			JT808MsgRsp_Send(ORDER_ACCEPT,1,0,sendbuf,sendbuflen);
		break;
		case CALLCAR_ORDER_DEL:
			status = ServReq_CallcarOrderDel(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
				SchedulScrn_CallforcarCancelSend("8888888888",10);
				gOrderInOperatflag=0;	
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
		break;
	#endif
		case APTS_OPERATELINE_SET:
			status = ServReq_APTSsetOperatLine(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
		break;
		case APTS_LINE_QUERY:
			status = ServReq_APTSLineQuery(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
		//	JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			JT808MsgRsp_Send(APTS_LINE_QUERY_rsp,1,0,sendbuf,sendbuflen);
			//上报线路
		break;
		#endif
		default:
		{
			DEBUG(" Server MsgId error:%04x!", ServReqMsgid);
			status=3;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/);
			return RspMsgerr;
		}
	}
	return status;
}
/*************************************************************
*Function:		Pro808Analysis
*Description:	定位系统交标808协议解析.
*				1.协议校验
*				2.解析，数据包分为终端请求回应和平台主动请求包
*Calls:			many
*Called By:      
*Input:			data 接收的平台数据首地址
				datalen 接收平台数据长
				sendbuf 缓存区
				sendbuflen	缓冲区实际长
*Return:		unknown
*Others:         
**************************************************************/
u8 JT808_recv_analysis(u8 *data,u16 datalen/*,u8 *sendbuf,u16 sendbuflen*/)
{
	u8 temp=0;
	u8 *pbuf=data;
	u16 ServmsgID=0;
	u16 Reallen; //协议包实际长度
	u16  U16Temp;
#if 0
	if(0)
	{
	u16 test;
	DEBUG(PrintDebug,"Raw Server data:!\r\n"); 	
	for(test=0;test<datalen;test++)
		DEBUG(PrintDebug,"%02x ",*(data+test));
	DEBUG(PrintInfo,"\r\n");	
	}
#endif
	if(NULL==data || datalen==0)  //协议最小长度未定义 ？？？
		return RspPackgerr;	
	//标示头尾判断
	if(*data!=0x7e || *(data+datalen-1)!=0x7e){
		DEBUG(" 0x7e is bad!!!");
		return RspPackgerr;	
	}
		
	//反转义还原数据
	if(JT808_dataChg(0, data+1, datalen-2, &Reallen)){
		DEBUG(" turning parameter error");
		return RspPackgerr;
	}
	Reallen += 2; //加上标示头尾
#if 0
	if(0){
	u8 test;
	DEBUG(PrintDebug,"Server data after translition:!\r\n"); 	
	for(test=0;test<Reallen;test++)
		DEBUG(PrintDebug,"%02x ",*(data+test));
	DEBUG(PrintInfo,"\r\n");
	}
#endif
	//msgid有效性判断
	char_to_short(data+1, &ServmsgID);
	if (!ISPrtclServMsg(ServmsgID)){
		DEBUG(" Unknown server message ID!");
		return RspPackgerr;
	}
	//校验位判断
	if (!XOR_Check(data+1,Reallen-3,&temp)){
		if (temp!=*(data+Reallen-2)){
			DEBUG(" Server data xor err!"); 	
			return RspPackgerr;
		}
	}
	else
		return RspPackgerr;	    //应追加消息错误应答 ???

	pbuf++;//pbuf --> msg id
	switch (ServmsgID){
		/*4种平台应答*/
		case REGISTERS_rsp:{	//注册应答
			LBS_PlatComRsp rsp;
			memcpy(rsp.MsgId, pbuf, 2);
			pbuf+=sizeof(MSG_HEAD);
			memcpy(rsp.SeqId, pbuf, 2);
			pbuf+=2;
			rsp.Rslt = *pbuf;
			#if 0
			//timeout
			{
				u16 temp_seq;
				u16 temp_seq2;
				u16 temp_len;
				char_to_short(&rsp.SeqId[0], &temp_seq2);
				Read_ProtclHandl(eDevSeqid, (u8 *)&temp_seq, &temp_len);
				if (timeout_en == OA_TRUE && temp_seq == temp_seq2){
					timeout_en = OA_FALSE;
				}
			}
			#endif
			//debug info
			DEBUG("register ack:0x%04x", REGISTERS_rsp);
			DEBUG("rsp result:0x%x", rsp.Rslt);
			Write_ProtclHandl(eRsp2DevReq, (u8 *)&rsp, sizeof(LBS_PlatComRsp));
			//reg timeout
			{	
				u16 temp_seq = 0;
				u16 temp_seq2 = 0;
				char_to_short(&rsp.SeqId[0], &temp_seq);
				char_to_short(&sProtclHandl.DevSeqId[0], &temp_seq2);
				//DEBUG("seq:%02x  seq2:%02x", temp_seq, temp_seq2);
				if (timeout_var.timeout_en == OA_TRUE && temp_seq == temp_seq2-1){
					//DEBUG("disable timeout");
					timeout_var.do_timeout = OA_FALSE;
					timeout_var.timeout_en = OA_FALSE;
				}
			}
			
			switch (rsp.Rslt){
				case RspOK:{
					pbuf++;
					temp = Reallen-3-3-sizeof(MSG_HEAD);
					if (temp > 0){ //has authen code & write it into file
						authen_code_handle(pbuf, temp);//write Authentication Code
						return PLAT_TREMREG_ACK;	
					}
					else{
						DEBUG("no authentication code");
						return RspPackgerr;
					}
				}
				break;
				case RspError:
				case RspUnsurport:{
					DEBUG("has already registered:%d", rsp.Rslt);
					return PLAT_TREMREG_ACK;
				}
				break;
				case RspMsgerr:
				case RspAlarmCheck:{
					DEBUG("database has no info:%d", rsp.Rslt);
					return PLAT_TREMREG_ACK;
				}
				break;
				default:{
					DEBUG("Rsp packet err!");
					return RspPackgerr;
				}break;
			}
			
			
//			Write_ProtclHandl(eRsp2DevReq, (u8 *)&rsp, sizeof(LBS_PlatComRsp));	
#if 0	
			if (rsp.Rslt == 0){//0 means register ok
				pbuf++;
				temp = Reallen-3-3-sizeof(MSG_HEAD);
				if (temp > 0){ //has authen code & write it into file
					#if 0
					Flash_Write_Buffer(NULL,0,DEV_REGSTA_ADDR,Sector_Erase);
					Flash_Write_Buffer(&temp,1,DEV_AUTHEN_LEN_ADDR,Write_Mode);	
					Flash_Write_Buffer(pbuf,temp,DEV_AUTHEN_ADDR,Write_Mode);
					temp = Regin;
					Flash_Write_Buffer(&temp,1,DEV_REGSTA_ADDR,Write_Mode);
					#endif
					authen_code_handle(pbuf, temp);//write Authentication Code
					return PLAT_TREMREG_ACK;	
					
				}
				else{
					DEBUG(" no authentication code");
					return RspPackgerr;
				}
			}
			else if{
				//......
			}
#endif
			
//			OSSemPost(JT808_ReqRspSem);
		}
		break;
#if 0
		case REPORT_MEDIA_DATA_rsp:	//多媒体数据上传应答
		{
			LBS_PlatComRsp rsp;
			DEBUG("Recieve Result of media data uploading."); 	
			memcpy(rsp.MsgId,pbuf,2);
			pbuf+=sizeof(MSG_HEAD);
			pbuf+=4; //media id无效
			if(*pbuf==0)
			{
				rsp.Rslt=0;//成功
			}
			else
			{
		//	if(Reallen-3-5-sizeof(MSG_HEAD)>0)
			//包列表---协议未定 ？？？				
				rsp.Rslt=1;//失败
				DEBUG(PrintInfo,"Media data report failed!\r\n"); 	
			}
			Write_ProtclHandl(eRsp2DevReq,(u8 *)&rsp,sizeof(LBS_PlatComRsp));
			OSSemPost(JT808_ReqRspSem);
		}
		
		break;
#endif
		case PLAT_COMMON_rsp:{ //平台通用应答
			LBS_PlatComRsp rsp;
			u16 temp;
			pbuf+=sizeof(MSG_HEAD);
			memcpy(&rsp, pbuf, sizeof(LBS_PlatComRsp));
			//timeout
			{
				u16 temp_msgid = 0;
				u16 temp_msgid2 = 0;
				u16 temp_seq = 0;
				u16 temp_seq2 = 0;
				u16 temp_len;
				char_to_short(&rsp.SeqId[0], &temp_seq);
				char_to_short(&rsp.MsgId[0], &temp_msgid);
				char_to_short(&sProtclHandl.DevSeqId[0], &temp_seq2);
				Read_ProtclHandl(eDevMsgid, (u8 *)&temp_msgid2, &temp_len);
				//DEBUG("seq:%02x msgid:%04x seq2:%02x msgid2:%04x", temp_seq, temp_msgid, temp_seq2, temp_msgid2);
				if (timeout_var.timeout_en == OA_TRUE && temp_seq == temp_seq2-1 && temp_msgid == temp_msgid2){
					//DEBUG("disable timeout");
					timeout_var.do_timeout = OA_FALSE;
					timeout_var.timeout_en = OA_FALSE;
				}
			}
			//debug info
			char_to_short(&rsp.MsgId[0], &temp);
			DEBUG("platform common ack:DevReqmsgID:0x%04x, rsp result:0x%x", temp, rsp.Rslt);
			Write_ProtclHandl(eRsp2DevReq, (u8 *)&rsp, sizeof(LBS_PlatComRsp));	
			return PLAT_COMMON_ACK;
		}
		break;
	/*	case RSA_PLAT: //平台公钥下发
		{
			//这个地方的实现有待商榷
			if(1)//pprotHandle->DevMsgId==RSA_DEV)
			{
				Mem_Copy(pprotHandle->devreqrsp, pprotHandle->platreq, pprotHandle->platreqlen);
				pprotHandle->devreqrsplen = pprotHandle->platreqlen;
				OSSemPost(JT808_ReqRspSem);
				break;
			}
			else
			{
				ServReq_808data_handle(pprotHandle);
				break;
			}
		}
	*/	
		default:{//以下N种为平台请求
			Write_ProtclHandl(eServMsgid, pbuf, 2);
			Write_ProtclHandl(eServSeqid, (pbuf+10), 2);//记录平台消息流水号
			if(*(pbuf+2)&0x20) {//有分包项 peijl_20120912
				pbuf+=(sizeof(MSG_HEAD)+sizeof(MSG_HEAD_sub));
				U16Temp=Reallen-3-(sizeof(MSG_HEAD)+sizeof(MSG_HEAD_sub));
			}
			else{
				pbuf+=sizeof(MSG_HEAD);
				U16Temp=Reallen-3-sizeof(MSG_HEAD);
			}
			DEBUG("msgID:0x%x", ServmsgID);
			JT808_ServReq_handle(ServmsgID, pbuf, U16Temp/*,sendbuf,sendbuflen*/);
			return PLAT_REQ;
		}
		break;
	}
	return 0;					
}
/*********************************************************
*Function:       dev_common_rsp_buildbody
*Description:   终端通用应答
*Calls:          
*Called By:      
*Input:          pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:         函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8  dev_common_rsp_buildbody(u8 *pbuf, u16 *pbuflen)
{
	u16 len;
	*pbuflen=0;		  
	Read_ProtclHandl(eServSeqid,pbuf,&len);
	pbuf+=len;
	*pbuflen+=len;
	Read_ProtclHandl(eServMsgid,pbuf,&len);
	pbuf+=len;
	*pbuflen+=len;
	Read_ProtclHandl(eRsp2ServSeq,pbuf,&len);
	*pbuflen += 1;
	return 0;
}
/*--------------------------------------------------------------------------------------
*Function:       ReadLbsCfgPara()
*Description:	读全局变量LbsCfgStruct
*input:			CfgPara 参数对应的JTT808协议ID
*output:		读出*buflen长度的数据存放到地址pValue
*Return:		读取的数据长度 0:err other:长度  计划修改
---------------------------------------------------------------------------------------*/
u8 ReadLbsCfgPara(Enum_CtrlCfgPara CfgPara,u8 *pValue,u8 *buflen)
{
	u8 err;
	if(pValue==NULL||NULL==buflen)
		return 1;
//	OSSemPend(HXCfgParaSem,0,&err);
	switch(CfgPara)
	{
		#if 0
		case eNET:
			*buflen=1;
			*pValue=LbsCfgStruct.SERV_NETTYPE;
		break;
		#endif
		case eAPN:
			//*buflen = LbsCfgStruct.ServAPNLen;
			*buflen = oa_strlen(dev_now_params.m_apn_name);
			Mem_Copy(pValue,dev_now_params.m_apn_name,*buflen);
			pValue[*buflen]='\0';
		break;
		case eUdpPort:
			*buflen = sizeof(u32);
			Mem_Copy(pValue,(u8 *)&dev_now_params.server_udp_port,sizeof(u32));
		break;
		case eTcpPort:
			*buflen = sizeof(u32);
			Mem_Copy(pValue,(u8 *)&dev_now_params.server_tcp_port,sizeof(u32));
		break;
		case eIP:
			//*buflen = LbsCfgStruct.ServIPLen;
			*buflen = oa_strlen(dev_now_params.m_server_ip);
			Mem_Copy(pValue,dev_now_params.m_server_ip,*buflen);
			pValue[*buflen]='\0';
		break;
		case eAPNUser:
			//*buflen = LbsCfgStruct.ServUserLen;
			*buflen = oa_strlen(dev_now_params.m_apn_username);
			Mem_Copy(pValue,dev_now_params.m_apn_username,*buflen);
			pValue[*buflen]='\0';
		break;
		case eAPNPwd:
			//*buflen = LbsCfgStruct.ServPwdLen;
			*buflen = oa_strlen(dev_now_params.m_apn_password);
			Mem_Copy(pValue,dev_now_params.m_apn_password,*buflen);
			pValue[*buflen]='\0';
		break;
		case eAPN_sub:
			//*buflen = LbsCfgStruct.SubServAPNLen;
			*buflen = oa_strlen(dev_now_params.b_apn_username);
			Mem_Copy(pValue,dev_now_params.b_apn_username,*buflen);
			pValue[*buflen]='\0';
		break;
		case eIP_sub:
			//*buflen = LbsCfgStruct.SubservIPLen;
			*buflen = oa_strlen(dev_now_params.b_server_ip);
			Mem_Copy(pValue,dev_now_params.b_server_ip,*buflen);
			//LbsCfgStruct.SubservIP[LbsCfgStruct.SubservIPLen]='\0';
			pValue[*buflen]='\0';
		break;
		case eAPNUser_sub:
			//*buflen = LbsCfgStruct.SubServUserLen;
			*buflen = oa_strlen(dev_now_params.b_apn_username);
			Mem_Copy(pValue,dev_now_params.b_apn_username,*buflen);
			pValue[*buflen]='\0';
		break;
		case eAPNPwd_sub:
			//*buflen = LbsCfgStruct.SubServPwdLen;
			*buflen = oa_strlen(dev_now_params.b_apn_password);
			Mem_Copy(pValue,dev_now_params.b_apn_password,*buflen);
			pValue[*buflen]='\0';
		break;
		case eLclTEL:
			#if 0
			if(LbsCfgStruct.LclTELlen>12) //808协议最大12字节 peijl_130311 添加本机号错误，设默认值
			{
				*buflen =11;
				Mem_Set(pValue,'0',11);
				pValue[11]='\0';
			}
			else
			{*buflen = LbsCfgStruct.LclTELlen;
			Mem_Copy(pValue,LbsCfgStruct.LclTEL,LbsCfgStruct.LclTELlen);
			pValue[LbsCfgStruct.LclTELlen]='\0';
			}
			#endif
			*buflen = oa_strlen(dev_now_params.term_tel_num);
			Mem_Copy(pValue,dev_now_params.term_tel_num,*buflen);
			pValue[*buflen]='\0';
		break;
		case eHtTime:
			*buflen = sizeof(dev_now_params.heartbeat_interval);
			Mem_Copy(pValue,(u8 *)&dev_now_params.heartbeat_interval,*buflen);
		break;
		case eTcptime:
			*buflen = sizeof(dev_now_params.tcp_ack_timeout);
			Mem_Copy(pValue,(u8 *)&dev_now_params.tcp_ack_timeout,*buflen);
		break;
		case eTcpnum:
			*buflen = sizeof(dev_now_params.tcp_retrans_times);
			Mem_Copy(pValue,(u8 *)&dev_now_params.tcp_retrans_times,*buflen);
		break;
		case eUdptime:
			*buflen = sizeof(dev_now_params.udp_ack_timeout);
			Mem_Copy(pValue,(u8 *)&dev_now_params.udp_ack_timeout,*buflen);
		break;
		case eUdpnum:
			*buflen = sizeof(dev_now_params.udp_retrans_times);
			Mem_Copy(pValue,(u8 *)&dev_now_params.udp_retrans_times,*buflen);
		break;
		case eSMSTime:
			*buflen = sizeof(dev_now_params.sms_ack_timeout);
			Mem_Copy(pValue,(u8 *)&dev_now_params.sms_ack_timeout,*buflen);
		break;
		case eSMSNum:
			*buflen = sizeof(dev_now_params.sms_retrans_times);
			Mem_Copy(pValue,(u8 *)&dev_now_params.sms_retrans_times,*buflen);
		break;
		case eRptStrategy:
			*buflen = sizeof(dev_now_params.report_strategy);
			Mem_Copy(pValue,(u8 *)&dev_now_params.report_strategy,*buflen);
		break;
		case eRptType:
			*buflen = sizeof(dev_now_params.report_type);
			Mem_Copy(pValue,(u8 *)&dev_now_params.report_type,*buflen);
		break;
		case eRpttime_def:
			*buflen = sizeof(dev_now_params.default_reporttime);
			Mem_Copy(pValue,(u8 *)&dev_now_params.default_reporttime,*buflen);
		break;
		case eRpttime_unlog:
			*buflen = sizeof(dev_now_params.unlogin_reporttime);
			Mem_Copy(pValue,(u8 *)&dev_now_params.unlogin_reporttime,*buflen);
		break;
		case eRpttime_sleep:	
			*buflen = sizeof(dev_now_params.sleep_reporttime);
			Mem_Copy(pValue,(u8 *)&dev_now_params.sleep_reporttime,*buflen);
		break;
		case eRpttime_emergency:
			*buflen = sizeof(dev_now_params.urgent_reporttime);
			Mem_Copy(pValue,(u8 *)&dev_now_params.urgent_reporttime,*buflen);
		break;
		case eRptDistanc_def:	
			*buflen = sizeof(dev_now_params.default_reportdistance);
			Mem_Copy(pValue,(u8 *)&dev_now_params.default_reportdistance,*buflen);
		break;
		case eRptDistanc_sleep:
			*buflen = sizeof(dev_now_params.sleep_reportdistance);
			Mem_Copy(pValue,(u8 *)&dev_now_params.sleep_reportdistance,*buflen);
		break;
		case eRptDistanc_unlog:
			*buflen = sizeof(dev_now_params.unlogin_reportdistance);
			Mem_Copy(pValue,(u8 *)&dev_now_params.unlogin_reportdistance,*buflen);
		break;
		case eRptDistanc_emergency:
			*buflen = sizeof(dev_now_params.urgent_reportdistance);
			Mem_Copy(pValue,(u8 *)&dev_now_params.urgent_reportdistance,*buflen);
		break;
		case eRptCog:
			*buflen = sizeof(dev_now_params.corner_reportangle);
			Mem_Copy(pValue,(u8 *)&dev_now_params.corner_reportangle,*buflen);
		break;
		#if 0
		case eServTel:
			*buflen = dev_now_params.;
			Mem_Copy(pValue,LbsCfgStruct.ServTel,*buflen);
			pValue[LbsCfgStruct.ServTellen]='\0';
		break;
		#endif
		case eResetTel:
			*buflen = oa_strlen(dev_now_params.reset_num);
			Mem_Copy(pValue,dev_now_params.reset_num,*buflen);
			pValue[*buflen]='\0';
		break;
		case eFactryTel:
			*buflen = oa_strlen(dev_now_params.restore_factory_settings_num);
			Mem_Copy(pValue,dev_now_params.restore_factory_settings_num,*buflen);
			pValue[*buflen]='\0';
		break;
		#if 0
		case eSMSTel:
			*buflen = oa_strlen(dev_now_params.);
			Mem_Copy(pValue,LbsCfgStruct.ServSMSTel,*buflen);
			pValue[LbsCfgStruct.ServSMSTellen]='\0';
		break;
		#endif
		case eAlarmSMSTel:
			*buflen = oa_strlen(dev_now_params.terminal_sms_num);
			Mem_Copy(pValue,dev_now_params.terminal_sms_num,*buflen);
			pValue[*buflen]='\0';
		break;
		#if 0
		case eMonitTel:
			*buflen = LbsCfgStruct.MonitorTellen;
			Mem_Copy(pValue,LbsCfgStruct.MonitorTel,*buflen);
			pValue[LbsCfgStruct.MonitorTellen]='\0';
		break;
		#endif
		#if 0
		case eSpeclSMSTel:
			*buflen = LbsCfgStruct.ServSpecialSMSTellen;
			Mem_Copy(pValue,LbsCfgStruct.ServSpecialSMSTel,*buflen);
			pValue[LbsCfgStruct.ServSpecialSMSTellen]='\0';
		break;
		#endif
		#if 0
		case eAnswerType:
			*buflen = sizeof(LbsCfgStruct.DevTelAnswerType);
			Mem_Copy(pValue,(u8 *)&LbsCfgStruct.DevTelAnswerType,*buflen);
		break;
		case eMaxCalltime:
			*buflen = sizeof(LbsCfgStruct.MaxTalkingTime);
			Mem_Copy(pValue,(u8 *)&LbsCfgStruct.MaxTalkingTime,*buflen);
		break;
		case eMaxCalltime_month:
			*buflen = sizeof(LbsCfgStruct.MaxTalkingTime_month);
			Mem_Copy(pValue,(u8 *)&LbsCfgStruct.MaxTalkingTime_month,*buflen);
		break;
		#endif
		case eAlarmmaskword:
			*buflen = sizeof(dev_now_params.alarm_mask);
			Mem_Copy(pValue,(u8 *)&dev_now_params.alarm_mask,*buflen);
		break;
		case etxtSMSAlarm:
			*buflen = sizeof(dev_now_params.alarm_send_sms_mask);
			Mem_Copy(pValue,(u8 *)&dev_now_params.alarm_send_sms_mask,*buflen);
		break;
		#if 0
		case eAlarmPhoto:
			*buflen = sizeof(LbsCfgStruct.AlarmPhotoFlag);
			Mem_Copy(pValue,(u8 *)&LbsCfgStruct.AlarmPhotoFlag,*buflen);
		break;
		case eAlarmPhotoStor:
			*buflen = sizeof(LbsCfgStruct.AlarmPhotoStoreFlag);
			Mem_Copy(pValue,(u8 *)&LbsCfgStruct.AlarmPhotoStoreFlag,*buflen);
		break;
		#endif
		#if 0
		case eKeyAlarm:
			*buflen = sizeof(dev_now_params.);
			Mem_Copy(pValue,(u8 *)&LbsCfgStruct.KeyAlarmFlag,*buflen);
		break;
		#endif
		case eOverspeed:
			*buflen = sizeof(dev_now_params.max_speed);
			Mem_Copy(pValue,(u8 *)&dev_now_params.max_speed,*buflen);
		break;
		case eOverspeedtime:
			*buflen = sizeof(dev_now_params.speed_duration);
			Mem_Copy(pValue,(u8 *)&dev_now_params.speed_duration,*buflen);
		case eDrivTime:
			*buflen = sizeof(dev_now_params.continuous_drive_time_threshold);
			Mem_Copy(pValue,(u8 *)&dev_now_params.continuous_drive_time_threshold,*buflen);
		break;
		case eDrivTime_day:
			*buflen = sizeof(dev_now_params.day_add_drive_time_threshold);
			Mem_Copy(pValue,(u8 *)&dev_now_params.day_add_drive_time_threshold,*buflen);
		break;
		case eRestTime_min:
			*buflen = sizeof(dev_now_params.min_rest_time);
			Mem_Copy(pValue,(u8 *)&dev_now_params.min_rest_time,*buflen);
		break;
		case eStopcarTime_max:
			*buflen = sizeof(dev_now_params.max_park_time);
			Mem_Copy(pValue,(u8 *)&dev_now_params.max_park_time,*buflen);
		break;
		#if 0
		case eImagQuality:
			*buflen = sizeof(LbsCfgStruct.ImageQuality);
			Mem_Copy(pValue,(u8 *)&LbsCfgStruct.ImageQuality,*buflen);
		break;
		case eImagBright:
			*buflen = sizeof(LbsCfgStruct.Brightness);
			Mem_Copy(pValue,(u8 *)&LbsCfgStruct.Brightness,*buflen);
		break;
		case eImagContrast:
			*buflen = sizeof(LbsCfgStruct.Contrast);
			Mem_Copy(pValue,(u8 *)&LbsCfgStruct.Contrast,*buflen);
		break;
		case eImagSatuatn:
			*buflen = sizeof(LbsCfgStruct.Saturation);
			Mem_Copy(pValue,(u8 *)&LbsCfgStruct.Saturation,*buflen);
		break;
		case eImagClor:
			*buflen = sizeof(LbsCfgStruct.Color);
			Mem_Copy(pValue,(u8 *)&LbsCfgStruct.Color,*buflen);
		break;
		#endif
		case eOdometer: //里程表精度 
			*buflen = sizeof(dev_now_params.vehicle_odometer);
			Mem_Copy(pValue,(u8 *)&dev_now_params.vehicle_odometer,*buflen);
		break;
		case ePrivic:
			*buflen = sizeof(dev_now_params.vehicle_province_id);
			Mem_Copy(pValue,(u8 *)&dev_now_params.vehicle_province_id,*buflen);
		break;
		case eCity:
			*buflen = sizeof(dev_now_params.vehicle_city_id);
			Mem_Copy(pValue,(u8 *)&dev_now_params.vehicle_city_id,*buflen);
		break;
		case eCarid:
			*buflen = oa_strlen(dev_now_params.vehicle_license);
			Mem_Copy(pValue,dev_now_params.vehicle_license,*buflen);
			pValue[*buflen]='\0';
		break;
		case eCarclor:
			*buflen=1;
			*pValue=dev_now_params.plate_color;
		break;
		case emanufacturersid:
			Mem_Copy(pValue, dev_now_params.manufacturers_id, 5);//5固定长度
		break;
		case etermmodel:
			*buflen = oa_strlen(dev_now_params.term_model);
			Mem_Copy(pValue, dev_now_params.term_model, *buflen);
		break;
		case etermid:
			Mem_Copy(pValue, dev_now_params.term_id, 7);
		default:
		break;
	}
//	OSSemPost(HXCfgParaSem);
	return 0;
}
/*********************************************************
*Function:		dev_registers_buildbody
*Description:	创建终端注册消息体
*Calls:          
*Called By:      
*Input:			pprotHandle 句柄的指针
				Buf 是外部缓冲区首地址
*Output:		pbuflen 是组成的报文长度
*Return:		函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8  dev_registers_buildbody(u8 *Buf, u16 *pbuflen)
{
	u8  *pbuf=Buf;
	u8  paraLen=0;
	u16 U16Temp = 0;
	if(NULL == Buf || NULL == pbuflen)
		return 1;
	ReadLbsCfgPara(ePrivic, (u8 *)&U16Temp,&paraLen);
	short_to_char(pbuf, U16Temp);
	pbuf+=paraLen;
	*pbuflen=paraLen;
	ReadLbsCfgPara(eCity, (u8 *)&U16Temp,&paraLen);
	short_to_char(pbuf, U16Temp);
	pbuf+=paraLen;
	*pbuflen+=paraLen;
	//Flash_Read_Buffer(pbuf,5,DEV_PRODUCT_ADDR+1);
	ReadLbsCfgPara(emanufacturersid, pbuf, &paraLen);
	pbuf+=5;
	*pbuflen+=5;
	//Flash_Read_Buffer(&paraLen,1,DEV_TYPE_ADDR);
	ReadLbsCfgPara(etermmodel, pbuf,&paraLen);
	//Flash_Read_Buffer(pbuf,paraLen,DEV_TYPE_ADDR+1);
	pbuf+=paraLen;
	memset(pbuf,0,20-paraLen);
	pbuf+=(20-paraLen);
	*pbuflen+=20;
	//Flash_Read_Buffer(pbuf,7,DEV_ID_ADDR+1);
	ReadLbsCfgPara(etermid, pbuf, &paraLen);
	pbuf+=7;
	*pbuflen+=7;
	ReadLbsCfgPara(eCarclor, pbuf,&paraLen);
	pbuf+=paraLen;
	*pbuflen+=paraLen;
	ReadLbsCfgPara(eCarid,pbuf,&paraLen);
	pbuf+=paraLen;
	*pbuflen+=paraLen;
	return 0;
}
/*********************************************************
*Function:       dev_login_buildbody
*Description:    创建终端鉴权请求包消息体
*Calls:          
*Called By:      
*Input:
				pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:		函数是否执行成功，返回AcktionOK表示成功
*Others:         
*********************************************************/
static u8  dev_login_buildbody(u8 *pbuf, u16 *pbuflen)
{
	oa_bool ret;
	//if(eSpclServ==GetMonitServ()) //监管平台:获取是哪个监管平台
	if (0){ //peijl_test 120613
		Read_ProtclHandl(e_Authen,pbuf,pbuflen);
	}
	else{
#if 0
		Flash_Read_Buffer(&len,1,DEV_AUTHEN_LEN_ADDR);
		if(len>AUTHENMAXLEN)
		{
			DEBUG(PrintError,"Error authen in flash!!\r\n");
			*pbuflen=0;
			return 1;
		}
		else
		{
			*pbuflen=len;
			Flash_Read_Buffer(pbuf,len,DEV_AUTHEN_ADDR);
		}
#endif
		//read Authentication Code
		ret = read_authen_code(pbuf, pbuflen);
		if (ret = OA_FALSE){
			return ActionError;
		}
		else{
			return ActionOK;
		}
	}
}
/*********************************************************
*Function:		get_dev_param_rsp_buildbody
*Description:	终端对平台请求查询参数的应答包
*Calls:          
*Called By:      
*Input:          pprotHandle 句柄的指针
			pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:         函数是否执行成功，返回0表示成功
*Others:         此功能需和平台对接后方知
	peijl_20120911 修改
*********************************************************/
static u8 get_dev_param_rsp_buildbody(u8 *msgbody, u16 *pbuflen)
{
//	PARAM_LIST *paramlist=NULL;
	u8 len=0;
	u32 para;
	u8 *pbuf=msgbody;
	u8 NUM=0; //参数个数
	*pbuflen =0;
	Read_ProtclHandl(eServSeqid,pbuf,pbuflen);
	pbuf+=2;
	pbuf++; //NUM=*(msgbody+2)
	*pbuflen+=1;

	//heartbeat
	int_to_char(pbuf,(u32)eHtTime);
	pbuf+=4;
	ReadLbsCfgPara(eHtTime,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eTcptime
	int_to_char(pbuf,(u32)eTcptime);
	pbuf+=4;
	ReadLbsCfgPara(eTcptime,(u8 *)&para,&len);
	*pbuf++=len;
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eTcpnum
	int_to_char(pbuf,(u32)eTcpnum);
	pbuf+=4;
	ReadLbsCfgPara(eTcpnum,(u8 *)&para,&len);
	*pbuf++=len;
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eUdptime
	int_to_char(pbuf,(u32)eUdptime);
	pbuf+=4;
	ReadLbsCfgPara(eUdptime,(u8 *)&para,&len);
	*pbuf++=len;
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eUdpnum
	int_to_char(pbuf,(u32)eUdpnum);
	pbuf+=4;
	ReadLbsCfgPara(eUdpnum,(u8 *)&para,&len);
	*pbuf++=len;
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;

	//eAPNUser
	int_to_char(pbuf,(u32)eAPNUser);
	pbuf+=4;
	ReadLbsCfgPara(eAPNUser,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eAPNPwd
	int_to_char(pbuf,(u32)eAPNPwd);
	pbuf+=4;
	ReadLbsCfgPara(eAPNPwd,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eIP
	int_to_char(pbuf,(u32)eIP);
	pbuf+=4;
	ReadLbsCfgPara(eIP,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eAPNUser_sub
	int_to_char(pbuf,(u32)eAPNUser_sub);
	pbuf+=4;
	ReadLbsCfgPara(eAPNUser_sub,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eAPNPwd_sub
	int_to_char(pbuf,(u32)eAPNPwd_sub);
	pbuf+=4;
	ReadLbsCfgPara(eAPNPwd_sub,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eIP_sub
	int_to_char(pbuf,(u32)eIP_sub);
	pbuf+=4;
	ReadLbsCfgPara(eIP_sub,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eTcpPort
	int_to_char(pbuf,(u32)eTcpPort);
	pbuf+=4;
	ReadLbsCfgPara(eTcpPort,(u8 *)&para,&len);
	*pbuf++=len;
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eUdpPort
	int_to_char(pbuf,(u32)eUdpPort);
	pbuf+=4;
	ReadLbsCfgPara(eUdpPort,(u8 *)&para,&len);
	*pbuf++=len;
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eRptStrategy
	int_to_char(pbuf,(u32)eRptStrategy);
	pbuf+=4;
	ReadLbsCfgPara(eRptStrategy,(u8 *)&para,&len);
	*pbuf++=len;
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eRptType
	int_to_char(pbuf,(u32)eRptType);
	pbuf+=4;
	ReadLbsCfgPara(eRptType,(u8 *)&para,&len);
	*pbuf++=len;
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eRpttime_def
	int_to_char(pbuf,(u32)eRpttime_def);
	pbuf+=4;
	ReadLbsCfgPara(eRpttime_def,(u8 *)&para,&len);
	*pbuf++=len;
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eRpttime_sleep
	int_to_char(pbuf,(u32)eRpttime_sleep);
	pbuf+=4;
	ReadLbsCfgPara(eRpttime_sleep,(u8 *)&para,&len);
	*pbuf++=len;
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eRpttime_unlog
	int_to_char(pbuf,(u32)eRpttime_unlog);
	pbuf+=4;
	ReadLbsCfgPara(eRpttime_unlog,(u8 *)&para,&len);
	*pbuf++=len;
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eRpttime_emergency
	int_to_char(pbuf,(u32)eRpttime_emergency);
	pbuf+=4;
	ReadLbsCfgPara(eRpttime_emergency,(u8 *)&para,&len);
	*pbuf++=len;
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eRptDistanc_def
	int_to_char(pbuf,(u32)eRptDistanc_def);
	pbuf+=4;
	ReadLbsCfgPara(eRptDistanc_def,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eRptDistanc_unlog
	int_to_char(pbuf,(u32)eRptDistanc_unlog);
	pbuf+=4;
	ReadLbsCfgPara(eRptDistanc_unlog,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eRptDistanc_emergency
	int_to_char(pbuf,(u32)eRptDistanc_emergency);
	pbuf+=4;
	ReadLbsCfgPara(eRptDistanc_emergency,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eRptDistanc_sleep
	int_to_char(pbuf,(u32)eRptDistanc_sleep);
	pbuf+=4;
	ReadLbsCfgPara(eRptDistanc_sleep,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eRptCog
	int_to_char(pbuf,(u32)eRptCog);
	pbuf+=4;
	ReadLbsCfgPara(eRptCog,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eServTel
	int_to_char(pbuf,(u32)eServTel);
	pbuf+=4;
	ReadLbsCfgPara(eServTel,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eResetTel
	int_to_char(pbuf,(u32)eResetTel);
	pbuf+=4;
	ReadLbsCfgPara(eResetTel,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eFactryTel
	int_to_char(pbuf,(u32)eFactryTel);
	pbuf+=4;
	ReadLbsCfgPara(eFactryTel,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eSMSTel
	int_to_char(pbuf,(u32)eSMSTel);
	pbuf+=4;
	ReadLbsCfgPara(eSMSTel,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eAlarmSMSTel
	int_to_char(pbuf,(u32)eAlarmSMSTel);
	pbuf+=4;
	ReadLbsCfgPara(eAlarmSMSTel,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eAnswerType
	int_to_char(pbuf,(u32)eAnswerType);
	pbuf+=4;
	ReadLbsCfgPara(eAnswerType,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eMaxCalltime
	int_to_char(pbuf,(u32)eMaxCalltime);
	pbuf+=4;
	ReadLbsCfgPara(eMaxCalltime,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eMaxCalltime_month
	int_to_char(pbuf,(u32)eMaxCalltime_month);
	pbuf+=4;
	ReadLbsCfgPara(eMaxCalltime_month,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eMonitTel
	int_to_char(pbuf,(u32)eMonitTel);
	pbuf+=4;
	ReadLbsCfgPara(eMonitTel,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eSpeclSMSTel
	int_to_char(pbuf,(u32)eSpeclSMSTel);
	pbuf+=4;
	ReadLbsCfgPara(eSpeclSMSTel,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eAlarmmaskword
	int_to_char(pbuf,(u32)eAlarmmaskword);
	pbuf+=4;
	ReadLbsCfgPara(eAlarmmaskword,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//etxtSMSAlarm
	int_to_char(pbuf,(u32)etxtSMSAlarm);
	pbuf+=4;
	ReadLbsCfgPara(etxtSMSAlarm,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eAlarmPhoto
	int_to_char(pbuf,(u32)eAlarmPhoto);
	pbuf+=4;
	ReadLbsCfgPara(eAlarmPhoto,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eAlarmPhotoStor
	int_to_char(pbuf,(u32)eAlarmPhotoStor);
	pbuf+=4;
	ReadLbsCfgPara(eAlarmPhotoStor,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eKeyAlarm
	int_to_char(pbuf,(u32)eKeyAlarm);
	pbuf+=4;
	ReadLbsCfgPara(eKeyAlarm,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eOverspeed
	int_to_char(pbuf,(u32)eOverspeed);
	pbuf+=4;
	ReadLbsCfgPara(eOverspeed,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eOverspeedtime
	int_to_char(pbuf,(u32)eOverspeedtime);
	pbuf+=4;
	ReadLbsCfgPara(eOverspeedtime,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eDrivTime
	int_to_char(pbuf,(u32)eDrivTime);
	pbuf+=4;
	ReadLbsCfgPara(eDrivTime,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eDrivTime_day
	int_to_char(pbuf,(u32)eDrivTime_day);
	pbuf+=4;
	ReadLbsCfgPara(eDrivTime_day,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eRestTime_min
	int_to_char(pbuf,(u32)eRestTime_min);
	pbuf+=4;
	ReadLbsCfgPara(eRestTime_min,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eStopcarTime_max
	int_to_char(pbuf,(u32)eStopcarTime_max);
	pbuf+=4;
	ReadLbsCfgPara(eStopcarTime_max,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eImagQuality
	int_to_char(pbuf,(u32)eImagQuality);
	pbuf+=4;
	ReadLbsCfgPara(eImagQuality,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eImagBright
	int_to_char(pbuf,(u32)eImagBright);
	pbuf+=4;
	ReadLbsCfgPara(eImagBright,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eImagContrast
	int_to_char(pbuf,(u32)eImagContrast);
	pbuf+=4;
	ReadLbsCfgPara(eImagContrast,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eImagSatuatn
	int_to_char(pbuf,(u32)eImagSatuatn);
	pbuf+=4;
	ReadLbsCfgPara(eImagSatuatn,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
 	//eImagClor
	int_to_char(pbuf,(u32)eImagClor);
	pbuf+=4;
	*pbuf++=4;
	memset(pbuf,0,4);
	pbuf+=4;
	*pbuflen+=(5+len);
 	NUM++;
	//eOdometer
	int_to_char(pbuf,(u32)eOdometer);
	pbuf+=4;
	ReadLbsCfgPara(eOdometer,(u8 *)&para,&len);
	*pbuf++=len; //len
	int_to_char(pbuf,(u32)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//ePrivic
	int_to_char(pbuf,(u16)ePrivic);
	pbuf+=4;
	ReadLbsCfgPara(ePrivic,(u8 *)&para,&len);
	*pbuf++=len; //len
	short_to_char(pbuf,(u16)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
	//eCity
	int_to_char(pbuf,(u32)eCity);
	pbuf+=4;
	ReadLbsCfgPara(eCity,(u8 *)&para,&len);
	*pbuf++=len; //len
	short_to_char(pbuf,(u16)para);
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;
//	//eCarid
	int_to_char(pbuf,(u32)eCarid);
	pbuf+=4;
	ReadLbsCfgPara(eCarid,pbuf+1,&len);
	*pbuf++=len;
	pbuf+=len;
	*pbuflen+=(5+len);
	NUM++;
	//eCarclor
	int_to_char(pbuf,(u32)eCarclor);
	pbuf+=4;
	ReadLbsCfgPara(eCarclor,(u8 *)&para,&len);
	*pbuf++=len; //len
	*pbuf++=para;
	pbuf+=len;
	*pbuflen+=(5+len);
 	NUM++;

	*(msgbody+2)=NUM;
	return 0;
}
//位置信息汇报的基本信息消息体创建
u8 report_location_msgbody1(u8 *Buf, u16 *pbuflen)
{
	u8 *pbuf=Buf;
	u32 alarmflag;
	float temp=0.0;
	u8  cnt=0;
	STRUCT_RMC GpsInfor;

	if(Buf==NULL||pbuflen==NULL)
		return 1;
	
	GPS_GetPosition(&GpsInfor); //取gps
	if (GpsInfor.Fix_Status == GPS_UNFIXED) {//if don't locate, clear data here!!!
		GPS_clear_data(&GpsInfor);
	}
	
	ReadAllBitAlarmPara(StaAlarm0, &alarmflag);
	int_to_char(pbuf, alarmflag);
	pbuf+=4;
	*pbuflen = 4;
	ReadAllBitAlarmPara(StaSector1, &alarmflag);
	int_to_char(pbuf, alarmflag);
	pbuf+=4;
	*pbuflen += 4;
	alarmflag=0;
	temp=0.0;
	for(cnt=0;cnt<2;cnt++) //纬度,百万分之一度
	{
		alarmflag=alarmflag*10+GpsInfor.Latitude[cnt]-'0';
	}
	for(cnt=2;cnt<8;cnt++) //纬度
	{
		temp=temp*10+GpsInfor.Latitude[cnt]-'0';
	}
	temp=temp*10.0/6.0;
	alarmflag=alarmflag*1000000+(u32)(temp);
	int_to_char(pbuf, alarmflag);
	pbuf+=4;
	*pbuflen += 4;
	alarmflag=0;
	temp=0.0;
	for(cnt=0;cnt<3;cnt++) //经度,百万分之一度
	{
		alarmflag=alarmflag*10+GpsInfor.Longitude[cnt]-'0';
	}
	for(cnt=3;cnt<9;cnt++) //经度
	{
		temp=temp*10+GpsInfor.Longitude[cnt]-'0';
	}
	temp=temp*10.0/6.0;
	alarmflag=alarmflag*1000000+(u32)(temp);
	int_to_char(pbuf, alarmflag);
	pbuf+=4;
	*pbuflen += 4;
	short_to_char(pbuf, GpsInfor.Height);
	pbuf+=2;
	*pbuflen += 2;
	short_to_char(pbuf,GpsInfor.Speed*10);
	pbuf+=2;
	*pbuflen += 2;
	short_to_char(pbuf,GpsInfor.COG);
	pbuf+=2;
	*pbuflen += 2;
	memset(pbuf,0x00,6);
	//time handle when gps doesn't locate
	if (GpsInfor.Fix_Status == GPS_UNFIXED) {
		u8 time[6] = {0x0};
		get_rtc_time(time);
		memcpy(pbuf, time, 6);
	} else {
		memcpy(pbuf, GpsInfor.Time, 6);
	}
	pbuf+=6;
	*pbuflen += 6;
	return 0;
}
//位置信息汇报的附加信息项消息体创建
static u8 report_location_msgbody2(u8 *Buf, u16 *pbuflen)
{
	u8 *pbuf=Buf;
	u32 alarmflag;

	if(Buf==NULL||pbuflen==NULL)
		return 1;
	//位置附加信息项列表
	*pbuf++=0x01;//里程
	*pbuf++=0x04;
	//ReadDistanceData(&alarmflag); //KM?	peijl_test
	{
		//read mileage by zq
		oa_int32 handle, ret;
		TOTAL_MILE temp;
		oa_uint32 dummy_read;
		oa_memset(&temp, 0x0, sizeof(temp));
		handle = oa_fopen(MILEAGE_FILE);
		if (handle < 0){
			DEBUG("mileage file open err!");
			alarmflag = 0;//var name is bad here
		}
		else{
			ret = oa_fread(handle, &temp, sizeof(TOTAL_MILE), &dummy_read);
			if ((ret < 0) || (dummy_read != sizeof(TOTAL_MILE))) {
				DEBUG("read mileage err!");
				alarmflag = 0;
			}
			alarmflag = temp.total_mileage;
			//DEBUG("read miles:%d", alarmflag);
			ret = oa_fclose(handle);
			if (ret < 0){
				DEBUG("close file err!");
				oa_fdelete(MILEAGE_FILE);
			}
		}
		
	}
	int_to_char(pbuf, alarmflag); //100M
	pbuf+=4;
	*pbuflen += 6;
#if 0
	//----------
	*pbuf++=0x02;//油量
	*pbuf++=0x02;
	*pbuf++=0x00;	//暂定
	*pbuf++=0x64;
	*pbuflen +=4;
	//----------
	*pbuf++=0x03;//行驶记录仪速度
	*pbuf++=0x02;
//	short_to_char(pbuf,gDriveRecodSpeed);  //??????	 
	memset(pbuf,0x00,2);
	pbuf+=2;
	*pbuflen +=4;
#endif
	//-----------
	if (overspeed_var.kind != no_os){
		*pbuf++=0x11;//超速报警附加信息
		if (overspeed_var.kind == no_spec){
			*pbuf++=0x01;//len
			*pbuf++=0x00;
			*pbuflen += 3;
		}
		else{
			*pbuf++=0x05;//len
			*pbuf++=overspeed_var.kind;
			int_to_char(pbuf, overspeed_var.id);
			pbuf+=4;
			*pbuflen += 7;
		}

		overspeed_var.kind = no_os;//clear
	}
	//-----------
	if (area_alarm_addition_var.area_kind != no_spec){
		*pbuf=0x12;//进出区域或路线
		pbuf++;
		*pbuf=0x06;//len
		pbuf++;
		*pbuf++ = area_alarm_addition_var.area_kind;
		int_to_char(pbuf, area_alarm_addition_var.id);
		pbuf+=4;
		*pbuf++ = area_alarm_addition_var.in_out;
		*pbuflen += 8;

		oa_memset(&area_alarm_addition_var, 0x0, sizeof(area_alarm_addition_var));//clear
	}
	
/*	*pbuf=0x13;//进出区域或路线
	pbuf++;
	*pbuf=0x07;
	pbuf++;
	memset(pbuf,0x0,7);
	*pbuflen +=9;
*/

#if 0
	ADD_LOCATION_INFO addInfo;
//	READ(meliage,oil,meliage,locationtype11,area_line_id11,locationtype22,area_line_id22,direction);
	addInfo.AddInfoId = 0x1;		//里程，对应车上的里程表读数
	addInfo.AddInfoLen = 4;
	memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
	*pbuflen += sizeof(ADD_LOCATION_INFO);
	memcpy(pbuf+*pbuflen, "1111", addInfo.AddInfoLen);
	*pbuflen += addInfo.AddInfoLen;

	addInfo.AddInfoId = 0x2;		//油量，对应车上油量表读数
	addInfo.AddInfoLen = 2;
	memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
	*pbuflen += sizeof(ADD_LOCATION_INFO);
	memcpy(pbuf+*pbuflen, "22", addInfo.AddInfoLen);
	*pbuflen += addInfo.AddInfoLen;

	addInfo.AddInfoId = 0x3;		//速度，行驶记录功能获取的速度
	addInfo.AddInfoLen = 2;
	memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
	*pbuflen += sizeof(ADD_LOCATION_INFO);
	memcpy(pbuf+*pbuflen, "33", addInfo.AddInfoLen);
	*pbuflen += addInfo.AddInfoLen;

	if(alarmflag & ALARM_OVER_SPEED)
	{
		addInfo.AddInfoId = 0x11;		//超速报警，
		addInfo.AddInfoLen = 1;
		memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
		*pbuflen += sizeof(ADD_LOCATION_INFO);
		*(pbuf+(*pbuflen)) = 0;
		if(*(pbuf+(*pbuflen)))
		{
			*pbuflen += addInfo.AddInfoLen;
			addInfo.AddInfoLen = 4;
			memcpy(pbuf+*pbuflen, "1234", addInfo.AddInfoLen);
			*pbuflen += addInfo.AddInfoLen;
		}
	}
	if(alarmflag & (ALARM_TOUCH_AREA_k|ALARM_TOUCH_LINE_k))
	{
		addInfo.AddInfoId = 0x12;		//进出区域/路线报警附加信息
		addInfo.AddInfoLen = 1;
		memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
		*pbuflen += sizeof(ADD_LOCATION_INFO);
		*(pbuf+(*pbuflen)) = 2;
		*pbuflen += addInfo.AddInfoLen;
		addInfo.AddInfoLen = 4;
		memcpy(pbuf+(*pbuflen), "1234", addInfo.AddInfoLen);
		*pbuflen += addInfo.AddInfoLen;
		addInfo.AddInfoLen = 1;
		*(pbuf+(*pbuflen)) = 0;//0进/1出
		*pbuflen += addInfo.AddInfoLen;
	}
	if(alarmflag & ALARM_DRIVE_SHORT_LONG_k)
	{
		addInfo.AddInfoId = 0x13;		//路段行驶时间不足/过长报警
		addInfo.AddInfoLen = 4;
		memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
		*pbuflen += sizeof(ADD_LOCATION_INFO);
		memcpy(pbuf+(*pbuflen), "1111", addInfo.AddInfoLen);
		*pbuflen += addInfo.AddInfoLen;
		addInfo.AddInfoLen = 2;
		memcpy(pbuf+(*pbuflen), "22", addInfo.AddInfoLen);
		*pbuflen += addInfo.AddInfoLen;
		addInfo.AddInfoLen = 1;
		*(pbuf+(*pbuflen)) = 0;//0不足1过长
		*pbuflen += addInfo.AddInfoLen;
	}
#endif
	return 0;
}
/*
static u8 report_location_msgbody3(u8 *Buf, u16 *pbuflen)
{
	u8 *pbuf=Buf;
	u32 tplen=0;

	if(Buf==NULL||pbuflen==NULL)
		return 1;
	//位置附加信息的自定义区域
	*pbuf++=0xE0;
	*pbuf++=0x04;
	*pbuf++=0x00;
	*pbuf++=0x00;
	*pbuf++=0x00;
	*pbuf++=0x00; //??
	*pbuflen=6;

	*pbuf++=0xE1; //正反转
	*pbuf++=0x01;
	GetClksensorFlag(pbuf);
	tplen+=3;
	int_to_char(Buf+2,tplen);
	*pbuflen+=tplen;
	return 0;

}
*/
/*********************************************************
*Function:		report_location_buildbody
*Description:	创建位置信息汇报消息体
*Calls:          
*Called By:      
*Input:			
				Buf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:         函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8 report_location_msgbody2(u8 *Buf, u16 *pbuflen);
static u8 report_location_msgbody3(u8 *Buf, u16 *pbuflen);
static u8  report_location_buildbody(u8 *Buf, u16 *pbuflen)
{
	u16 tplen=0;
	if(Buf==NULL||pbuflen==NULL)
		return 1;
	*pbuflen=0;
	report_location_msgbody1(Buf, &tplen);
	Buf+=tplen;
	*pbuflen+=tplen;
	tplen=0;
	report_location_msgbody2(Buf, &tplen);
	Buf+=tplen;
	*pbuflen+=tplen;
//	tplen=0;
//	report_location_msgbody3(Buf, &tplen);
//	*pbuflen+=tplen;
	return 0;
}
/*********************************************************
*Function:       get_location_rsp_buildbody
*Description:    位置信息查询应答
*Calls:          
*Called By:      
*Input:          pprotHandle 句柄的指针
			pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:         函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8  get_location_rsp_buildbody(u8 *pbuf, u16 *pbuflen)
{	
	u16 len=0;
	*pbuflen=0;
	Read_ProtclHandl(eServSeqid,pbuf,pbuflen);
	pbuf+=*pbuflen;
	report_location_buildbody(pbuf, &len);
	*pbuflen += len;
	return 0;
}
/*********************************************************
*Function:       report_event_buildbody
*Description:   事件报告
*Calls:          
*Called By:      
*Input:          pprotHandle 句柄的指针
			pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:         函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8  report_event_buildbody(u8 *pbuf, u16 *pbuflen)
{	
/*	u16 len=0;
	Read_protocol_handle(pprotHandle, e_ReportEventreq, pbuf, &len);
	*pbuflen += len;
*/
	return 0;
}
/*********************************************************
*Function:       ask_answer_buildbody
*Description:   提问应答
*Calls:          
*Called By:      
*Input:          pprotHandle 句柄的指针
			pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:         函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8 ask_answer_buildbody(u8 *pbuf, u16 *pbuflen)
{
/*	memcpy(pbuf,pprotHandle->AskAnswerrsp.sequenceId,2);
	pbuf+=2;
	*pbuf=pprotHandle->AskAnswerrsp.AnswerId;
	*pbuflen=3;
*/
	return 0;
}
/*********************************************************
*Function:       demand_set_cancel_buildbody
*Description:   信息点播/取消
*Calls:          
*Called By:      
*Input:          pprotHandle 句柄的指针
			pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:         函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8 demand_set_cancel_buildbody(u8 *pbuf, u16 *pbuflen)
{
/*	u16 len=0;
	Read_protocol_handle(pprotHandle, e_DemandSetCancl, pbuf, &len);
	*pbuflen += len;

	return 0;
	*/
}
/*********************************************************
*Function:       car_ctl_rsp_buildbody
*Description:   车辆控制应答
*Calls:          
*Called By:      
*Input: 		pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:         函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8 car_ctl_rsp_buildbody(u8 *pbuf, u16 *pbuflen)
{
	u16 len;
	Read_ProtclHandl(eServSeqid,pbuf,pbuflen); //平台的流水号
	pbuf+=*pbuflen;
	report_location_buildbody(pbuf, &len);
	*pbuflen += len;
	return 0;
}
/*********************************************************
*Function:       BuildMsgbody
*Description:   根据msg_ID的值，区别调用的是哪个消息体
*Calls:          
*Called By:      BuildMsg
*Input: 		pbuf 是外部缓冲区首地址
*Output:        pbuflen 是组成的报文长度
*Return:         函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
static u8 BuildMsgbody(u16 DevMsgId, u8 *msgbody, u16 *msgbodylen, u16 totalPackt,u16 SubPackt)
{
	u8 status=0;
	if(msgbody==NULL || msgbodylen==NULL ||totalPackt==0)
	{
		return 1;
	}
	
	switch(DevMsgId)//终端消息
	{
#if 0
		case ORDER_REQUEST:
			status=callforcar_request_buildbody(msgbody, msgbodylen);
		break;
		case ORDER_ACCEPT:
			status=callforcar_accept_buildbody(msgbody, msgbodylen);
		break;
		case ORDER_DONE:
			status=callforcar_complete_buildbody(msgbody, msgbodylen);
		break;
#endif
		case DEV_COMMON_rsp:
		{
			status = dev_common_rsp_buildbody(msgbody, msgbodylen);
			break;
		}
		case REGISTERS:
		{
			status = dev_registers_buildbody(msgbody, msgbodylen);
			break;
		}
		case LOGIN:
		{
			status = dev_login_buildbody(msgbody, msgbodylen);
			break;
		}
		case UNREGISTERS:
		{
			//空消息体
			*msgbodylen=0;
			break;
		}
		case HEART_BEAT:
		{
			//Write_ProtclHandl(eDevMsgid, (u8 *)&DevMsgId, 2);//终端发送消息ID by zhuqing @2013/6/26
			//timeout_var.timeout_en = OA_TRUE;
			//timeout_var.do_timeout = OA_FALSE;
			//空消息体
			*msgbodylen=0;
			break;
		}
		case GET_DEV_PARAM_rsp:
		{
			status = get_dev_param_rsp_buildbody(msgbody, msgbodylen);
			break;
		}
		case REPORT_LOCATION:
		{
			
			status = report_location_buildbody(msgbody, msgbodylen);
			break;
		}
		case GET_LOCATION_rsp:
		{
			status = get_location_rsp_buildbody(msgbody, msgbodylen);
			break;
		}
#if 0
		case REPORT_EVENT:
		{
			status=report_event_buildbody(msgbody, msgbodylen);
			break;
		}
		case ASK_ANSWER:
		{
			status=ask_answer_buildbody(msgbody, msgbodylen);
			break;
		}
		case DEMAND_SET_CANCEL:
		{
			status=demand_set_cancel_buildbody(msgbody, msgbodylen);
			break;
		}
		case CAR_CTL_rsp:
		{
			status=car_ctl_rsp_buildbody(msgbody, msgbodylen);
			break;
		}
		case REPORT_DRIVE_DATA:
		{
			status=report_drive_data_buildbody(msgbody, msgbodylen,totalPackt,SubPackt);	  //创建行驶记录上传消息包
			break;
		}
		case REPORT_DRIVER:
		{
			status=report_driver_buildbody(pprotHandle, msgbody, msgbodylen);
			break;
		}
		case REPORT_BILL:
		{
			status=report_bill_buildbody(pprotHandle, msgbody, msgbodylen);
			break;
		}
		case REPORT_MEDIA_EVENT:
		{
			status=report_media_event_buildbody(msgbody, msgbodylen);
			break;
		}
		case REPORT_MEDIA_DATA:
		{
			status=status=report_media_data_buildbody(msgbody, msgbodylen, totalPackt,SubPackt);
			break;
		}
		case SMEDIA_DATA_CHECK_rsp: //多媒体检索
		{
			status=smedia_data_check_rsp_buildbody(msgbody, msgbodylen);
			break;
		}
		case RSA_DEV:
		{
			status=dev_rsa_buildbody(msgbody, msgbodylen);
			break;
		}
		case UPLOAD_DATA:
			status=upload_data_buildbody(msgbody, msgbodylen, totalPackt,SubPackt);
		break;
		case APTS_LINE_QUERY_rsp:
			status=APTSLineQuery_Response(msgbody, msgbodylen);
		break;
		case APTS_INTOUT_STATION:
			status=APTS_IntOutStation_Report(msgbody, msgbodylen);
		break;
#endif
		default:
		{
			DEBUG(" msg_ID  err");
			return 1;
		}
	}
	return status;
}
/*********************************************************
*Function:       TelAsc2BCD
*Description:	 读取本机号并换成BCD码
*Input:          Deslen BCD码位数
				 Srcstr 缓冲区
*Output:		 DesTel BCD本机号存放地址
*Return:         函数是否执行成功，返回0表示成功
*Others:         
*********************************************************/
u8 TelAsc2BCD(u8 *bcdTel,u8 *bcdlen,u8 *ascTel,u8 ascLen)
{
	u8 len;
	u8 i=0;
	u8 j=0;
	if(bcdTel==NULL||bcdlen==NULL||ascTel==NULL)
		return 1;

	if(ascLen%2)
	{
		memset(bcdTel,0x00,ascLen+1);
		*bcdlen=ascLen/2+1;
		*bcdTel=*ascTel-'0';
		i++;j++;
	}
	else
	{
		memset(bcdTel,0x00,ascLen+1);
		*bcdlen=ascLen/2;
	}

	while(j<ascLen)
	{
		*(bcdTel+i)|=(*(ascTel+j)-'0')<<4;
		*(bcdTel+i)|=(*(ascTel+j+1)-'0');
		i++;j+=2;	
	}
	return 0;
}
/*********************************************************
*Function:       JT808DevMsg
*Description:   组建终端发送报文
*Calls:          
*Called By:      
*Input:          DevMsgId 要发送的终端消息类型
				 totalPackt	发送总包数
				 SubPackt	发送当前包号，JT808从1开始
				 Sendbuf，Sendbuflen	发送缓冲区和长度
				 Senddatalen 发送报文实际长度
*Output:         
*Return:         函数是否执行成功，0成功 / 1
*Others:          
*********************************************************/
u8 JT808Msg_Build(u16 DevMsgId,u16 totalPackt,u16 SubPackt,u8 *Sendbuf,u16 Sendbuflen,u16 *Senddatalen)
{
	u8  status = 0;
	u8 	U8Temp = 0;
	u8  *pbuf = NULL;
	u8  tbuf[15] = {0};
	MSG_HEAD *msghead = (MSG_HEAD *)(Sendbuf+1); //消息头
	MSG_HEAD_bodyattr msgbody_attr;	   //消息体属性
	u16 U16Temp = 0;
	
	if (!ISPrtclDevMsg(DevMsgId)||Sendbuf==NULL||Senddatalen==NULL||Sendbuflen==0){
		DEBUG(" JT808Msg_Build parameter error!");
		return 1;
	}
	pbuf = Sendbuf;

	//组建消息头!!!
	*pbuf++=0x7e;
	*Senddatalen=1;
	memset(msghead,0x00,sizeof(MSG_HEAD)); //不设流水号，流水号在发送时重设
	short_to_char(msghead->MsgId, DevMsgId);
	memset(&msgbody_attr,0x00,2); //默认消息不加密
	if (totalPackt > 1){
		msgbody_attr.subpacket = 1;
	}
	else{
		msgbody_attr.subpacket = 0;
	}
	
	ReadLbsCfgPara(eLclTEL,tbuf,&U8Temp);
	TelAsc2BCD(msghead->lclTel, &status, tbuf, U8Temp);//本机号<=12
	pbuf+=sizeof(MSG_HEAD);
	*Senddatalen+=sizeof(MSG_HEAD);
	//消息包封装项
	if (totalPackt>1){
		MSG_HEAD_sub *headsub=(MSG_HEAD_sub*)pbuf;
		short_to_char(headsub->totalpacketNum, totalPackt);
		short_to_char(headsub->SubpacketNo, SubPackt);
		pbuf+=sizeof(MSG_HEAD_sub);
		*Senddatalen+=sizeof(MSG_HEAD_sub);
	}

	//创建消息体!!!
	status = BuildMsgbody(DevMsgId,pbuf, &U16Temp,totalPackt, SubPackt);
	if (status){
		DEBUG("BuildMsgbody error, status=%d!", status);
		return 1;
	}
	//赋值消息体属性的长度
	msgbody_attr.MsgbodyLen = U16Temp;	//给消息体属性赋值消息体长度
	short_to_char(Sendbuf+3, *(u16*)&msgbody_attr);
	pbuf+=U16Temp;
	*Senddatalen+=U16Temp;
//#ifdef LBS_RSA
#if 0
	if(msgbody_attr.DataEncrypt1)
	{	//给消息体加密
		u16 msgbodylen_code;
		Encode(pstr+len, &msgbodylen_code, &msgbody, msgbodylen);//加密后的数据保存在pstr+len中
		U16Temp = msgbodylen_code;
	}
	else
	{
		Mem_Copy(pstr+*Senddatalen, msgbody, msgbodylen);
	}
#endif
	//校验
	*pbuf++=0;
	*Senddatalen += 1;
	//标识尾
	*pbuf=0x7e;
	*Senddatalen+=1;
	if (*Senddatalen > Sendbuflen)
		status=1;
	return status;
}
u8 WriteAlarmPara(u8 AcktionMode,u8 AlarmSector,u32 AlarmBits)
{
	u8 err;
	if(AlarmSector>=StatusNum)
	{
		return 1;
	}
//	OSSemPend(AlarmParaSem,0,&err);
	switch(AcktionMode)
	{
		case SET:
			AlarmFlag[AlarmSector]|=AlarmBits;		
		break;
		case RESET:
			AlarmFlag[AlarmSector]&=~AlarmBits;	
		break;
		default:
		break;
	}
//	OSSemPost(AlarmParaSem);
	return 0;
} 
/*********************************************************
*Function:       AlarmHandle
*Description:    报警发生时处理，包含判断报警屏蔽字，
txt报警，报警时拍照及照片上传或仅存储,关键报警
*Calls:          无
*Called By:      无
*Input:          ALARM_DEF 对应位置信息汇报的报警标志位宏定义
*Output:          
*Return:         ReturnType类型 ParaError 参数错误 
*				 AcktionError 报警屏蔽，AcktionOK 报警
*Others:         
*********************************************************/
u8 AlarmHandle(u32 ALARM_DEF/*,u8 *Str*/)
{
	u32 flag=0;
	u8  paralen=0;
	u8	Tel[TELMAXLEN]={0};
//	u8 *pSMS=Str;

//	if(NULL==Str)
//		return 1;
	if(!IS_Valid_ALARM_FLAG(ALARM_DEF))
		return 1;
	//ReadLbsCfgPara(eAlarmmaskword,(u8 *)&flag,&paralen);
	flag = dev_now_params.alarm_mask;
	if(flag&ALARM_DEF)//报警屏蔽
	{
		return 2;
	}
	else
	{
		WriteAlarmPara(SET,StaAlarm0,ALARM_DEF);
		DevReq2ServPackag_build(REPORT_LOCATION); //告警产生一包位置汇报
		//WriteAlarmPara(SET,StaAlarm0,ALARM_DEF);
		#if 0
		switch(ALARM_DEF)
		{
			case ALARM_EMERGENCY_k:
				DEBUG("ALARMtype:%d", ALARM_EMERGENCY_k);
			break;
			case ALARM_OVER_SPEED:
				DEBUG("ALARMtype:%d", ALARM_OVER_SPEED);
			break;
			case ALARM_DRIVE_TIRED:
				DEBUG("ALARMtype:%d", ALARM_DRIVE_TIRED);
			break;
			case ALARM_PRE_k:
				DEBUG("ALARMtype:%d", ALARM_PRE_k);
			break;
			case ALARM_GNSS_ERR:
				DEBUG("ALARMtype:%d", ALARM_GNSS_ERR);
			break;
			case ALARM_GNSS_ANTENNA:
				DEBUG("ALARMtype:%d", ALARM_GNSS_ANTENNA);
			break;
			case ALARM_GNSS_SHORT_CIRCUIT:
				DEBUG("ALARMtype:%d", ALARM_GNSS_SHORT_CIRCUIT);
			break;
			case ALARM_POWER_UNDERVOLTAGE:
				DEBUG("ALARMtype:%d终端主电源欠压");
			break;
			case ALARM_POWER_BROWN:
				DEBUG("ALARMtype:%d终端主电源掉电");
			break;
			case ALARM_DISPLAY_ERR:
				DEBUG("ALARMtype:%dLCD或显示屏故障");
			break;
			case ALARM_TTS_ERR:
				DEBUG("ALARMtype:%dTTS告警");
			break;
			case ALARM_CAMERA_ERR:
				DEBUG("ALARMtype:%d摄像头故障");
			break;
			case ALARM_DRIVE_OVERTIME:
				DEBUG("ALARMtype:%d当天累计驾驶超时");
			break;
			case ALARM_OVERTIME_PARKING:
				DEBUG("ALARMtype:%d超时停车");
			break;
			case ALARM_ENTER_AREA:
				DEBUG("ALARMtype:%d进出区域报警");
			break;
			case ALARM_TOUCH_LINE_k:
				DEBUG("ALARMtype:%d进出路线报警!\r\n");
			break;
			case ALARM_DRIVE_SHORT_LONG_k:
				DEBUG("ALARMtype:%d路段行驶时间不足或过长!\r\n");
			break;
			case ALARM_DRIVE_DEVIATE:
				DEBUG("ALARMtype:%d路线偏离报警!\r\n");
			break;
			case ALARM_VSS_ERR:
				DEBUG("ALARMtype:%d车辆VSS报警!\r\n");
			break;
			case ALARM_OIL_ERR:
				DEBUG("ALARMtype:%d油量异常报警!\r\n");
			break;
			case ALARM_STEAL:
				DEBUG("ALARMtype:%d车辆被盗!\r\n");
			break;
			case ALARM_START_ILLEGAL_k:
				DEBUG("ALARMtype:%d车辆非法点火!\r\n");
			break;
			case ALARM_MOVE_ILLEGAL_k:
				DEBUG("ALARMtype:%d车辆非法位移!\r\n");
			break;
			case ALARM_ROLLOVER:
				DEBUG("ALARMtype:%d车辆侧翻报警!\r\n");
			break;
			default:
				DEBUG("ALARMtype:%d车辆报警!\r\n");
			break;
		}
		DEBUG("!\r\n");
		#endif

		
		ReadLbsCfgPara(etxtSMSAlarm,(u8 *)&flag,&paralen);
		if(flag&ALARM_DEF)//文本SMS报警
		{
			#if 0
			switch(ALARM_DEF)
			{
				case ALARM_EMERGENCY_k:
					Str_Copy(pSMS,"Emergency alarm");
				break;
				case ALARM_OVER_SPEED:
					Str_Copy(pSMS,"Over-speed driving Alarm!");
				break;
				case ALARM_DRIVE_TIRED:
					Str_Copy(pSMS,"Tired driving alarm!");
				break;
				case ALARM_PRE_k:
					Str_Copy(pSMS,"ALARMtype:%d预警");
				break;
				case ALARM_GNSS_ERR:
					Str_Copy(pSMS,"GNSS error alarm!");
				break;
				case ALARM_GNSS_ANTENNA:
					Str_Copy(pSMS,"GNSS no antenna alarm!");
				break;
				case ALARM_GNSS_SHORT_CIRCUIT:
					Str_Copy(pSMS,"GNSS antenna short circuit alarm!");
				break;
				case ALARM_POWER_UNDERVOLTAGE:
					Str_Copy(pSMS,"low voltage of power suply alarm");
				break;
				case ALARM_POWER_BROWN:
					Str_Copy(pSMS,"cut off power suply alarm!");
				break;
				case ALARM_DISPLAY_ERR:
					Str_Copy(pSMS,"display screen error alarm!"); //调度屏及手柄
				break;
				case ALARM_TTS_ERR:
					Str_Copy(pSMS,"TTS error alarm!");
				break;
				case ALARM_CAMERA_ERR:
					Str_Copy(pSMS,"camera error alarm!");
				break;
				case ALARM_DRIVE_OVERTIME:
					Str_Copy(pSMS,"Tired driving by day alarm!");
				break;
				case ALARM_OVERTIME_PARKING:
					Str_Copy(pSMS,"Overtiime parking alarm!");
				break;
				case ALARM_ENTER_AREA:
					Str_Copy(pSMS,"Alarm when enter or out area!");
				break;
				case ALARM_TOUCH_LINE_k:
					Str_Copy(pSMS,"Alarm when in or out the line");
				break;
				case ALARM_DRIVE_SHORT_LONG_k:
					Str_Copy(pSMS,"Alarm for driving time is not enough or too long.");
				break;
				case ALARM_DRIVE_DEVIATE:
					Str_Copy(pSMS,"Alarm for out of specific line.");
				break;
				case ALARM_VSS_ERR:
					Str_Copy(pSMS,"Alarm for VSS error.");
				break;
				case ALARM_OIL_ERR:
					Str_Copy(pSMS,"Alarm for abnormaol amount of oil.");	//油位传感器
				break;
				case ALARM_STEAL:
					Str_Copy(pSMS,"Theft alarm!");
				break;
				case ALARM_START_ILLEGAL_k:
					Str_Copy(pSMS,"Alarm for car illegal starting");
				break;
				case ALARM_MOVE_ILLEGAL_k:
					Str_Copy(pSMS,"ALARM for car illegal moving.");
				break;
				case ALARM_ROLLOVER:
					Str_Copy(pSMS,"Alarm for car overturned.");
				break;
				default:
					Str_Copy(pSMS,"Unknown alarm.");
				break;
			}
			#endif
			
			ReadLbsCfgPara(eAlarmSMSTel,Tel,&paralen);
			Tel[paralen]='\0';
			//WCM_SendSMS(ASC_SMS,Tel,paralen,pSMS,strlen(pSMS));zq 注释掉了发短信
		}
		#if 0
		ReadLbsCfgPara(eAlarmPhoto,(u8 *)&flag,&paralen);
		if(flag&ALARM_DEF)//报警拍照允许									-
		{
			LBS_Mediainfo mediainfo;
			TakePhotoStrct cameraPara;
			memset(mediainfo.MediaId,0,4);	//get ID
			mediainfo.MediaId[3]=1;
			mediainfo.MediaType=0; 
			mediainfo.MediaCodeFormat=0; //jpeg
			if(ALARM_DEF&ALARM_EMERGENCY_k)
				mediainfo.EventCode=2;
			else if(ALARM_DEF&ALARM_ROLLOVER)
				mediainfo.EventCode=3;
			else
				mediainfo.EventCode=4;
			mediainfo.ChannelId=1;
			Write_ProtclHandl(eMediaInfo, (u8 *)&mediainfo, sizeof(LBS_Mediainfo)); //上传时用

			//报警时暂定使用下列参数作为默认拍照参数
			cameraPara.ChannelId=0x01;
			cameraPara.ShootCmd[0]=0x00;
			cameraPara.ShootCmd[1]=0x01;
			cameraPara.ShootTime[0]=0;									
			cameraPara.ShootTime[1]=0;
			cameraPara.Resolution=PROTCL_IMAGRES320_240;
			ReadLbsCfgPara(eImagQuality,(u8 *)&flag,&paralen);
			cameraPara.Quality=(u8)flag;
			ReadLbsCfgPara(eImagBright,(u8 *)&flag,&paralen);
			cameraPara.Brightness=(u8)flag;
			ReadLbsCfgPara(eImagContrast,(u8 *)&flag,&paralen);
			cameraPara.Contrast=(u8)flag;
			ReadLbsCfgPara(eImagSatuatn,(u8 *)&flag,&paralen);
			cameraPara.Saturation=(u8)flag;
			ReadLbsCfgPara(eImagClor,(u8 *)&flag,&paralen);
			cameraPara.Color=(u8)flag;
			ReadLbsCfgPara(eAlarmPhotoStor,(u8 *)&flag,&paralen);
			if(flag&ALARM_DEF)//拍照保存不发送
			{
				cameraPara.SaveFlag=1;	
			}
			else
			{
				cameraPara.SaveFlag=0;	
			}
			Write_ProtclHandl(e_TakePhotoreq,(u8 *)&cameraPara, sizeof(TakePhotoStrct));
			OSTaskResume(PhotoshotTaskPrio);
		}
		#endif
		ReadLbsCfgPara(eKeyAlarm,(u8 *)&flag,&paralen);
		if(flag&ALARM_DEF) //关键报警
		{
			//undo	
		}
		return 0;
	}
}
u8 ReadAlarmPara(u8 AlarmSector,u32 AlarmBits)
{
	u8 err;
	if(AlarmSector>=StatusNum)
	{
		return 0xFF;//err SET
	}
//	OSSemPend(AlarmParaSem,0,&err);
	if(AlarmFlag[AlarmSector]&AlarmBits)
	{
//		OSSemPost(AlarmParaSem);
		return SET;						   
	}
	else
	{
//		OSSemPost(AlarmParaSem);
		return RESET;			
	}
}
u8 ReadAllBitAlarmPara(u8 AlarmSector,u32 *AlarmBits )
{
	u8 err;
	if(AlarmSector>=StatusNum)
	{
		return 1;
	}
//	OSSemPend(AlarmParaSem,0,&err);	
	*AlarmBits=AlarmFlag[AlarmSector];
//	OSSemPost(AlarmParaSem);
	return 0;
}
/*********************************************************
*Function:      escape_copy_to_send()
*Description:  escape data & copy data to send buffer
*Return:        0:right others:wrong
*Others:         
*********************************************************/
u16 escape_copy_to_send(u8 *buf, u16 len)
{
	u8 data_buf[DATA_MAX_LEN] = {0};
	u16 real_len;
	u8 SeqId[2];
	u16 U16Temp;
	
	if (NULL == buf || len == 0 || len > DATA_MAX_LEN)
	{
		OA_DEBUG_USER(" buf/len err");
		return 0;
	}
	
	oa_memcpy(data_buf, buf, len);
	//给包加上流水号,流水号第加
	Read_ProtclHandl(eDevSeqid, SeqId, &U16Temp);//after reading +1
	oa_memcpy(data_buf+11, SeqId, 2);
	{
		u16 id;
		char_to_short(sProtclHandl.DevSeqId,&id);
		if(id==0xffff)
			id=0;
		else
			id++;
		short_to_char(sProtclHandl.DevSeqId,id);
	}
	//校验，除标示头尾和校验本身
	if (1 == XOR_Check(data_buf+1, len-3,(data_buf+len-2)))
	{
		OA_DEBUG_USER(" XOR err");
		return 0;
	}

	//对标示头和尾之外的包数据进行转义
	JT808_dataChg(1, data_buf+1, len-2, &U16Temp);
	len = U16Temp+2; //total data length
	real_len = oa_write_buffer_force_noinit(g_soc_context.gprs_tx, data_buf, len);
	if (real_len < len)
	{
		OA_DEBUG_USER(" write err");
	}
	return real_len;
}
/*********************************************************
*Function:      DevReq2ServPackag_build()
*Description:  build kinds of packets
*Return:        0:wrong others:right
*Others:         
*********************************************************/
u16 DevReq2ServPackag_build(u16 ReqMsgId) //即时上传数据
{
	//u8 *pbuf = NULL;
	u8 pbuf[DATA_MAX_LEN] = 0;
	u16 U16Temp = 0;
	u16 ret = 0;
	//pbuf = getEmptybuf();
	if (NULL != pbuf)
	{
		//pbuf[0..1]:data length after enpacketing
		//if (0 == JT808Msg_Build(ReqMsgId,1,0,pbuf+2,sizeof(ServUpdatabuf[0])-2,&U16Temp))
		if (0 == JT808Msg_Build(ReqMsgId, 1, 0, pbuf, DATA_MAX_LEN, &U16Temp))
		{
			
			if (U16Temp > DATA_MAX_LEN)
			{
				DEBUG("data is too long!");
				return 0;
			}
			
			#if 0
			//put data length into pbuf[0..1]
			*pbuf = U16Temp>>8;
			*(pbuf+1) = U16Temp&0xff;
			OSQPost(QServUpdata,pbuf);
			#endif
			
			
			ret = escape_copy_to_send(pbuf, U16Temp);
			if (ret > 0){
				if (ReqMsgId == REPORT_LOCATION || ReqMsgId == REGISTERS 
				|| ReqMsgId == LOGIN || ReqMsgId == UNREGISTERS){
					Write_ProtclHandl(eDevMsgid, (u8 *)&ReqMsgId, 2);//终端发送消息ID by zhuqing @2013/6/26
					timeout_var.timeout_en = OA_TRUE;
					oa_memset(&timeout_data, 0x0, sizeof(timeout_data));
					oa_memcpy(timeout_data.data_buf, pbuf, U16Temp);
					timeout_data.data_len = U16Temp;
				}
				return ret;
			}
			else
			{
				return 0;
			}
			
		}
		else
		{	
			DEBUG("build package error...");
			return 0;
		}
	}
	else
	{
		OA_DEBUG_USER(" no empty buffer!");
		return 0;
	}
//盲区数据暂时不管
//#ifdef BLINDDATA
#if 0
	else if(REPORT_LOCATION==ReqMsgId) //位置数据存flash
	{
		u8 err;
		u32 Temp;
		ReadAllBitAlarmPara(StaSector1,&Temp); //peijl_20121018 添加：只gps定位时，位置数据存盲区
		if(Temp&STA_GPS_FIXED)
		{
		DEBUG(PrintDebug,"DevReq2Serv Queue is full.So save position-data to Blind-position flash.\r\n");
		BlinddataBuild_save2Flash(REPORT_LOCATION);
		}
		return ActionOK;
	}
	else if(STADENTDATA_REPPORT==ReqMsgId) //位置数据存flash
	{
		u8 err;
		u32 Temp;
		DEBUG(PrintDebug,"DevReq2Serv Queue is full.So save student-data to flash of Blind area.\r\n");
		BlinddataBuild_save2Flash(STADENTDATA_REPPORT);
		return ActionOK;
	}
	else //其他数据丢弃
	{	DEBUG(PrintDebug,"DevReq2Serv Queue is full.\r\n");
		return ActionError;
	}
#endif
}
/*********************************************************
*Function:      DevReq2ServPackag_build_blind()
*Description:  build kinds of packets when it is blind data
*Return:        0:wrong others:right
*Others:         
*********************************************************/
extern u16 total_write;
oa_bool DevReq2ServPackag_build_blind(u16 ReqMsgId) //即时上传数据
{
	u8 pbuf[DATA_MAX_LEN] = 0;
	u16 U16Temp = 0;
	oa_bool ret;
	//pbuf = getEmptybuf();
	if (NULL != pbuf){
		//pbuf[0..1]:data length after enpacketing
		//if (0 == JT808Msg_Build(ReqMsgId,1,0,pbuf+2,sizeof(ServUpdatabuf[0])-2,&U16Temp))
		if (0 == JT808Msg_Build(ReqMsgId, 1, 0, pbuf, DATA_MAX_LEN, &U16Temp)){
			if (U16Temp > DATA_MAX_LEN){
				OA_DEBUG_USER(" data is too long!");
				return OA_FALSE;
			}
			#if 0
			//put data length into pbuf[0..1]
			*pbuf = U16Temp>>8;
			*(pbuf+1) = U16Temp&0xff;
			OSQPost(QServUpdata,pbuf);
			#endif
			//ret = write_blinddata(pbuf, U16Temp);
			ret = write_blinddata_to_1file(pbuf, U16Temp);
			if (OA_TRUE == ret){
				//print_rtc_time();
				DEBUG("write one blinddata packet!total write num:%d", total_write);
			}
			return ret;
			
		}
		else{	
			DEBUG("build package error.");
			return OA_FALSE;
		}
	}
	else{
		DEBUG("no empty buffer!");
		return OA_FALSE;
	}

}
