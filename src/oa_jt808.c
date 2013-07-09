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
#define PRINT_SAMEPARA	DEBUG(" this parameter is same as the original, so I do nothing...")
#define PORT_MAX 65535
extern DEVICE_PARAMS dev_now_params;
extern oa_soc_context g_soc_context;
extern timeout_struct timeout_var;
dev_control_type control_type = none;
upgrade_paras up_paras;
action_kind plat_paraset = 0x0;
static u32 AlarmFlag[StatusNum]={0};

//#define UPDATA_BUFNUM		12 //ÉÏ´«Êý¾Ý¶ÓÁÐµÄ»º´æÇø¸öÊý
//#define UPDATA_BUFLEN_MAX	200
//#define BUFFER_MAX_LEN 198
//#define BUFFER_MAX_LEN DATA_MAX_LEN
#if 0
u8 Queue_ServUpdatabufindex[UPDATA_BUFNUM]; //posbufÐ´Èë¶ÓÁÐ±êÊ¾£¬0ÎªÕ¼ÓÃ£¬1Ê¹ÓÃÖÐ
u8 ServUpdatabuf[UPDATA_BUFNUM][UPDATA_BUFLEN_MAX];
#endif
extern DEV_PLAT_PARAS dev_running;
ProtocolHandle sProtclHandl = {0};

u16 escape_copy_to_send(u8 *buf, u16 len);
u16 DevReq2ServPackag_build(u16 ReqMsgId);
u8 JT808Msg_Build(u16 DevMsgId,u16 totalPackt,u16 SubPackt,u8 *Sendbuf,u16 Sendbuflen,u16 *Senddatalen);
//=====================================´ÓLBSÉÏÒÆÖ²======================================
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
		case eDevSeqid: //ÖÕ¶Ë·¢ËÍÁ÷Ë®ºÅ
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
		case eServSeqid: //Á÷Ë®ºÅ
			*pbuflen=sizeof(sProtclHandl.ServSeqId);
			Mem_Copy(pbuf,sProtclHandl.ServSeqId,sizeof(sProtclHandl.ServSeqId));
		break;
		case eServMsgid: //Æ½Ì¨ÇëÇóÏûÏ¢id
			*pbuflen=sizeof(sProtclHandl.ServMsgId);
			Mem_Copy(pbuf,sProtclHandl.ServMsgId,sizeof(sProtclHandl.ServMsgId));
		break;
		case eRsp2ServSeq: //Æ½Ì¨ÇëÇóÓ¦´ð½á¹û
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
		case eMediaInfo:  //¶àÃ½ÌåÐÅÏ¢ÉÏ´«
			*pbuflen=sizeof(sProtclHandl.MediaEventinfo);
			Mem_Copy(pbuf,&sProtclHandl.MediaEventinfo,*pbuflen);
		break;
		case eSpclServPara:	//¼à¹ÜÆ½Ì¨
			*pbuflen=sizeof(sProtclHandl.spclServ);
			Mem_Copy(pbuf,&sProtclHandl.spclServ,*pbuflen);
		break;
		case eRecordPara: //Â¼Òô²ÎÊý
			*pbuflen=sizeof(sProtclHandl.StartRecordreq);
			Mem_Copy(pbuf,&sProtclHandl.StartRecordreq,*pbuflen);
		break;
#endif
		case eRsp2DevReq: //¼à¹ÜÆ½Ì¨Ó¦´ð
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
		case e_ReportMediaDatarsp: //Ð£Ñé¶àÃ½ÌåÊý¾ÝÉÏ´«Ó¦´ð
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
		case e_SetEventreq_EventType:	//ÊÂ¼þÉèÖÃ//ÉèÖÃÀàÐÍ
		{
			*pbuflen = 1;
			Mem_Copy(pbuf, &sProtclHandl.SetEventreq.EventType, *pbuflen);
			break;
		}
		case e_SetEventreq_totalNum:	//ÊÂ¼þÉèÖÃ//ÉèÖÃ×ÜÊý
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
		case e_SetPhonebookreq_BookType:	//ÉèÖÃµç»°±¾//ÀàÐÍ
		{
			*pbuflen = 1;
			Mem_Copy(pbuf, &sProtclHandl.SetPhonebookreq.BookType, *pbuflen);
			break;
		}
		case e_SetPhonebookreq_ContackNum:	//ÉèÖÃµç»°±¾//ÊýÁ¿
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
		case e_TakePhotoreq: //ÅÄÕÕ²ÎÊý
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
			*pbuflen = 12; //¹Ì¶¨³¤¶Èfor 808 test
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
#if 0 //zq comment for mtk-lbs desn't support
		case e_APTSStationinfo:
			Mem_Copy(&sProtclHandl.stationInfo, pbuf, buflen);
		break;
		case e_CallcarOrder:
		//	if(buflen-50>sizeof(sProtclHandl.stCallcarOrder.OrderContent)) //Êý¾Ý¹ý³ÌÎ´´¦Àí £¿£¿
		//		return 1;
			Mem_Copy(sProtclHandl.stCallcarOrder.OrderID,pbuf,50);
			if(buflen-50>sizeof(sProtclHandl.stCallcarOrder.OrderContent)) //Êý¾Ý¹ý³ÌÎ´´¦Àí £¿£¿
			{
				sProtclHandl.stCallcarOrder.OrderContentlen=sizeof(sProtclHandl.stCallcarOrder.OrderContent);
				Mem_Copy(sProtclHandl.stCallcarOrder.OrderContent,pbuf+50,sProtclHandl.stCallcarOrder.OrderContentlen);
			}
			else
			{
				sProtclHandl.stCallcarOrder.OrderContentlen=buflen-50;
				Mem_Copy(sProtclHandl.stCallcarOrder.OrderContent,pbuf+50,sProtclHandl.stCallcarOrder.OrderContentlen);
			}
		break;
		case e_CallcarInfoOrder:
		//	if(buflen-50-20>sizeof(sProtclHandl.stCallcarOrder.OrderContent)) //Êý¾Ý¹ý³ÌÎ´´¦Àí £¿£¿
		//		return 1;
			Mem_Copy(sProtclHandl.stCallcarOrder.OrderID,pbuf,50);
			Mem_Copy(sProtclHandl.stCallcarOrder.OrderTel,pbuf+50,20);
			if(buflen-50-20>sizeof(sProtclHandl.stCallcarOrder.OrderContent)) //Êý¾Ý¹ý³ÌÎ´´¦Àí £¿£¿
			{
				sProtclHandl.stCallcarOrder.OrderContentlen=sizeof(sProtclHandl.stCallcarOrder.OrderContent);
				Mem_Copy(sProtclHandl.stCallcarOrder.OrderContent,pbuf+70,sProtclHandl.stCallcarOrder.OrderContentlen);
			}
			else
			{
				sProtclHandl.stCallcarOrder.OrderContentlen=buflen-70;
				Mem_Copy(sProtclHandl.stCallcarOrder.OrderContent,pbuf+70,sProtclHandl.stCallcarOrder.OrderContentlen);
			}

			sProtclHandl.stCallcarOrder.OrderContentlen=buflen-70;
			Mem_Copy(sProtclHandl.stCallcarOrder.OrderContent,pbuf+70,buflen-70);
		break;
		case e_CallcarAccept:
			sProtclHandl.CallcarAccept=*pbuf;
		break;
		case e_CallcarResult:
			sProtclHandl.CallcarResult=*pbuf;
		break;
#endif
		//·¢ËÍ
		case eDevSeqid: //ÖÕ¶Ë·¢ËÍÁ÷Ë®ºÅ
			Mem_Copy(sProtclHandl.DevSeqId,pbuf,sizeof(sProtclHandl.DevSeqId));
		break;
		case eDevMsgid:{
			oa_memcpy(sProtclHandl.DevMsgId, pbuf, buflen);
		}break;
		case eServSeqid: //Æ½Ì¨ÇëÇó
			Mem_Copy(sProtclHandl.ServSeqId,pbuf,buflen);
		break;
		case eServMsgid: //Æ½Ì¨ÇëÇó
			Mem_Copy(sProtclHandl.ServMsgId,pbuf,buflen);
		break;
		case eRsp2ServSeq: //Æ½Ì¨ÇëÇóÓ¦´ð½á¹û
			if(buflen!=1)
				return 1;
			sProtclHandl.Rsp2Serv=*pbuf;
		break;
#if 0 //zq comment for mtk-lbs desn't support
		case eMediaInfo: //¶àÃ½ÌåÉÏ±¨
			Mem_Copy(&sProtclHandl.MediaEventinfo,pbuf,buflen);
		break;
#endif
		//½ÓÊÕ
		case eRsp2DevReq: //¶ÔÖÕ¶ËÇëÇóÓ¦´ð
			Mem_Copy(&sProtclHandl.PlatComrsp,pbuf,buflen);
		break;
		case eSpclServPara:	//¼à¹ÜÆ½Ì¨
			Mem_Copy(&sProtclHandl.spclServ,pbuf,buflen);
		break;
#if 0 //zq comment for mtk-lbs desn't support
		case eRecordPara: //Â¼Òô²ÎÊý
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
		case e_ReportMediaDatarsp:				//Ð£Ñé¶àÃ½ÌåÊý¾ÝÉÏ´«Ó¦´ð
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
		case e_SetEventreq_EventType:	//ÊÂ¼þÉèÖÃ//ÉèÖÃÀàÐÍ
		{
			Mem_Copy(&sProtclHandl.SetEventreq.EventType, pbuf, buflen);
			break;
		}
		case e_SetEventreq_totalNum:	//ÊÂ¼þÉèÖÃ//ÉèÖÃ×ÜÊý
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
		case e_SetPhonebookreq_BookType:	//ÉèÖÃµç»°±¾//ÀàÐÍ
		{
			sProtclHandl.SetPhonebookreq.BookType = *pbuf;
			break;
		}
		case e_SetPhonebookreq_ContackNum:	//ÉèÖÃµç»°±¾//ÊýÁ¿
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
*Description:   808Ð­Òé×ªÒå 
*				0x7e <-> 0x7d+0x02,0x7d<->0x7d+0x01
*Calls:          
*Called By:      
*Input:			flag ±íÊ¾1:ÕýÏò×ªÒå 0:·´Ïò×ªÒå
*				pstr ÎªÐèÒª±»×ªÒåµÄÊý¾ÝÊ×µØÖ· ³£Ö¸Õë
*				strlen ×ªÒåÇ°Êý¾Ý³¤¶È
*Output:        pstr ÒàÎªÊä³öµØÖ·
*				*Destlen ×ªÒåºóÊý¾Ý³¤¶È
*Return:        º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
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
	//flag==1Ê±ºòÊÇ½«0x7e -> 0x7d+0x02, 	0x7d->0x7d+0x01
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
	//flag==0Ê±ÊÇ½«0x7d+0x02->0x7e, 	0x7d+0x01->0x7d
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
#if 0
//ÅÐ¶Ïlen³¤¶È×Ö·û´®ÊÇ·ñÎªIP IP¸ñÊ½xxx.xxx.xxx.xxx peijl_20120828 ÐÞ¸
oa_bool ISAscIPValid (u8 *IP, u8 len)
{
	u8 i;
	u8 pot=0;
	u16 potdata=0;
	if(IP==NULL)
		return OA_FALSE;
	if(len<7 ||len>15)
		return OA_FALSE;
	if(IP[0]=='.'||IP[len-1]=='.')
		return OA_FALSE;
	for(i=0;i<len-1;i++)
	{
		if(IP[i]=='.'&&IP[i+1]=='.')
		return OA_FALSE;
	}

	for(i=0;i<len;i++)
	{
		if(IP[i]=='.')
		{
			if(potdata>255)
				return OA_FALSE;
			potdata=0;
			pot++;
		}
		else if(IP[i]>='0'&&IP[i]<='9')
			potdata=potdata*10+IP[i]-'0';
		else
			return OA_FALSE;
	}
	if((pot!=3)||(potdata>255))
		return OA_FALSE;

	return OA_TRUE;
}
#endif
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
//ÅÐ¶Ïlen³¤¶È×Ö·û´®ÊÇ·ñÎªÊÖ»úºÅ ¸ñÊ½0~9×Ö·û´®£¬»ò¡¾+¡¿+¡¾0~9¡¿×Ö·û´® peijl_20120829 Ìí¼Ó
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
	{//Î´½øÐÐ²ÎÊýÉèÖÃ£¬¶ø±¸·Ý²ÎÊý
		OSSemPend(HXCfgParaSem,0,&err);
		Flash_Write_Buffer(NULL,0,SERV_CTRLCFG_ADDR,Sector_Erase);	//²Á³ýÅäÖÃ
		Flash_Write_Buffer((u8 *)&LbsCfgStruct,sizeof(STRUCT_LBS_CFG),SERV_CTRLCFG_ADDR,Write_Mode);	//Ð´ÈëÅäÖÃ	
		OSSemPost(HXCfgParaSem);
		return 0;		
	}
	#endif
	//²ÎÊýÓÐÐ§ÐÔÅÐ¶Ï
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
		case eLclTEL: //808Ð­Òé×î´ó12×Ö½Ú peijl_130311 Ìí¼Ó±¾»úºÅ´íÎó£¬ÉèÄ¬ÈÏÖµ
			if(len>12) //808Ð­Òé×î´ó12×Ö½Ú peijl_130311 Ìí¼Ó±¾»úºÅ´íÎó£¬ÉèÄ¬ÈÏÖµ
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
		case eOdometer: //Àï³Ì±í¾«¶È 
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
	//²ÎÊýÉèÖÃ
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
		case eUdpPort:{ //Ð­Òé²»Ã÷£¬¶Ë¿ÚºÅÔÝ¶¨ÒåÏàÍ¬
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
				plat_paraset = rereg;
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
			{ //ÖµÖØÉèºó£¬ÖØÐÂÅÐ¶Ï×´Ì¬
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
		case eOdometer: //Àï³Ì±í¾«¶È
			DEBUG("Odometer");
			//Mem_Copy((u8 *)&LbsCfgStruct.Odometer,pValue,len);		
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.vehicle_odometer, pValue, len);
			}
			else {
				DEBUG("param err ");
			}
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
		default:{DEBUG("not support!");}
		break;
	}
}
	if(UpdateMode == UpdateFlash)  			/*ÐèÒª¸üÐÂµ½flash*/
	{
		#if 0
		Flash_Write_Buffer(NULL,0,SERV_CTRLCFG_ADDR,Sector_Erase);	//²Á³ýÅäÖÃ
		Flash_Write_Buffer((u8 *)&LbsCfgStruct,sizeof(STRUCT_LBS_CFG),SERV_CTRLCFG_ADDR,Write_Mode);	//Ð´ÈëÅäÖÃ	
		#endif
		oa_bool ret;
		//DEBUG("traceing!");
		ret = dev_params_save();
		if (ret == OA_TRUE){
			print_key_dev_params();
		}
		else{
			DEBUG("save dev parameters err!");
		}
		//msybe need reconnect!!!
		
	}
	//OSSemPost(HXCfgParaSem);
	return 0;		
}

/*------------------------------------Æ½Ì¨ÇëÇóÏûÏ¢´¦Àíº¯Êý---------------------------------------*/
/*½«²ÎÊýparamID¶ÔÓ¦µÄÐ¡¶Ë¼à¿ØÆ½Ì¨²ÎÊý×ª»»³É´ó¶Ë£¬paramID¶ÔÓ¦µÄvalÄÚÈÝÓëÔ­²ÎÊýÏàÍ¬£¬ÎÞºóÐø²Ù×÷£»
²ÎÊý²»Í¬ÐèÒªÉèÖÃÍ¬Ê±ÖÕ¶Ë¿ÉÄÜÓÐÏàÓ¦¶¯×÷
	peijl_20120828 ÐÞ¸Ä*/
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
					WriteLbsCfgPara(eIP, Srcval,len,UpdateOnly); //²»±£´æ£¬ËùÓÐ²ÎÊýÍê³ÉÔÙºó±£´æµ½flash
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
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,len,UpdateOnly); //²»±£´æ£¬ËùÓÐ²ÎÊýÍê³ÉÔÙºó±£´æµ½flash
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
		case eCarid:   //peijl_130220 ÐÞ¸Ä£º³µÁ¾»ù±¾²ÎÊý¸Ä±äÊ±£¬ÒªÏÈ×¢ÏúÔÙÖØ×¢²á
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
		case eAnswerType: //0×Ô¶¯½ÓÌý 1acc on×Ô¶¯£¬offÊÖ¶¯
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
		case eOdometer: //Àï³Ì±í¾«¶È 
			if(4!=len)
				return 1;
			char_to_int(Srcval,&U32Temp);
			//DEBUG(" set params!");
			WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly); //²»±£´æ£¬ËùÓÐ²ÎÊýÍê³ÉÔÙºó±£´æµ½flash
		break;
#if 0
		case eImagQuality:
			if(4!=len)
				return 1;
			char_to_int(Srcval,&U32Temp);
			if(U32Temp>0&&U32Temp<=10)
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly); //²»±£´æ£¬ËùÓÐ²ÎÊýÍê³ÉÔÙºó±£´æµ½flash
			else
				return 1;
		break;
		case eImagBright:
		case eImagContrast:
		case eImagSatuatn:
			if(4!=len)
				return 1;
			if(U32Temp>=0&&U32Temp<=127)
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly); //²»±£´æ£¬ËùÓÐ²ÎÊýÍê³ÉÔÙºó±£´æµ½flash
			else
				return 1;
		break;
		case eImagClor:
			if(4!=len)
				return 1;
			if(U32Temp>=0&&U32Temp<=255)
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly); //²»±£´æ£¬ËùÓÐ²ÎÊýÍê³ÉÔÙºó±£´æµ½flash
			else
				return 1;
		break;
#endif
		case ePrivic:
		case eCity:
			char_to_short(Srcval,&U16Temp);
			WriteLbsCfgPara(paramID,(u8 *)&U16Temp,2,UpdateOnly); //²»±£´æ£¬ËùÓÐ²ÎÊýÍê³ÉÔÙºó±£´æµ½flash
		break;
		case eCarclor:
			WriteLbsCfgPara(paramID, Srcval,1,UpdateOnly); //²»±£´æ£¬ËùÓÐ²ÎÊýÍê³ÉÔÙºó±£´æµ½flash
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
*Description:	Æ½Ì¨ÏÂ·¢ÉèÖÃÖÕ¶Ë²ÎÊýÇëÇó½âÎö¹ý³Ì
*Calls:          
*Called By:      
*Input:			
*				pbuf ÊÇÏûÏ¢ÌåÊ×µØÖ·
*				pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Output:
*Return:		ActionOK
*Others:         
*********************************************************/
static u8 ServReq_SetParam(u8 *pbuf, const u16 buflen)
{
	u8 paraNum=0;
	u8 i=0;
	u8 len=0; //²ÎÊý³¤
	u16 devAct=0; //²ÎÊýÉèÖÃÊ±ÖÕ¶Ë¶¯×÷
	u8 Ary[200]={0}; //peijl_1012  ¼ÓÈëProtocolHandle±ØÒª?
	u16 alllen=buflen;
	u32 paramID;
	u8 sta=1;
	if(pbuf==NULL)
	{
		DEBUG("check_set_dev_param param error!");
		return 1;
	}
	//¿Õ²ÎÊýÁÐ±í
	if(buflen==1&&*pbuf==0)
		return 0;;

	paraNum=*pbuf++; //²ÎÊý×ÜÊý
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
	//ËùÓÐ²ÎÊýÐ´Èëflash,ÉèÖÃ²ÎÊýºóÖÕ¶ËÏàÓ¦¶¯×÷
	WriteLbsCfgPara(eCfaParaMax, NULL,0, UpdateFlash); //save it
//	WriteLinkTaskPara(devAct,ActionTypePara,SET);zq
	return sta;
}
/*********************************************************
*Function:       JT808MsgRsp_Send
*Description:  	 ÖÕ¶Ë¶ÔÆ½Ì¨ÇëÇó·¢ËÍÓ¦´ð±¨ÎÄ
				ÎÞÐèÆ½Ì¨»Ø¸´
*Calls:          
*Called By:      
*Input:         Sendbuf ·¢ËÍÊý¾ÝÊ×Ö·
				Sendlen ·¢ËÍÊý¾Ý³¤¶È
*Output:       
*Return:         º¯ÊýÖ´ÐÐ½á¹û£¬¶ÔÓ¦enum _REQRSP_RESULT
				 Æ½Ì¨Ó¦´ð½á¹û£ºenum 0~4
				 ParaError ²ÎÊý´íÎó»ò×é°ü´íÎó
				 ServErr	ÍøÂçÎÊÌâ
				 DevError
*Others:         Sendlen¶ÎµÄÊý¾Ý²»ÄÜ¸Ä±ä
	×ö³É	20130109

*********************************************************/
u8 JT808MsgRsp_Send(u16 DevMsgId,u16 totalPackt,u16 SubPackt/*,u8 *Sendbuf,u16 Sendbuflen*/)
{
	u16 Senddatalen=0;

	u8 status;
	u8 err;
	u8 SeqId[2];	//·¢ËÍÁ÷Ë®ºÅ
	//u8 *pbuf=Sendbuf;	//·¢ËÍ»º³åÇø
	u16 U16Temp;
	u16 ret;
#if 0
	//ÖØ·¢»úÖÆ
	u32 Retry=0;
	u32 RetryNum=0;
	u32 RetryWaittim=0; //ÖØ·¢µÈ´ýÊ±¼äT(n)=T(n-1)*n
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
	//¸ø°ü¼ÓÉÏÁ÷Ë®ºÅ,Á÷Ë®ºÅµÚ¼Ó
	Read_ProtclHandl(eDevSeqid,SeqId,&U16Temp);
	memcpy(pbuf+11,SeqId,2);

	memcpy(SeqId,pbuf+11,2);	 //»¹Ô­

	//Ð£Ñé£¬³ý±êÊ¾Í·Î²ºÍÐ£Ñé±¾Éí
	if(1==XOR_Check(pbuf+1, Senddatalen-3,(pbuf+Senddatalen-2)))
		return RspErrOther;
	//¶Ô±êÊ¾Í·ºÍÎ²Ö®ÍâµÄ°üÊý¾Ý½øÐÐ×ªÒå
	status = JT808_dataChg(1, pbuf+1, Senddatalen-2, &U16Temp);
	Senddatalen=U16Temp+2; //°ü³¤
			#if 0
			{//TEST
			u16 test;
			DEBUG(PrintDebug,"ÖÕ¶ËRSP·¢ËÍ°ü %d!\r\n",Senddatalen);
			for(test=0;test<Senddatalen;test++)
			{
			DEBUG(PrintInfo,"0x%02x ",*(pbuf+test));
			}
			DEBUG(PrintInfo,"\r\n");
			}
			#endif	
/* ·¢ËÍ°ü */
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
	//È¡ÖØ·¢»úÖÆ²ÎÊý
	RetryNum=JT808_UploadRetrynum(); RetryNum=2; //peijl_130227 ÔÝ¶¨
	RetryWaittim=JT808_UploadOvertime();RetryWaittim=30;	

	//·¢ËÍÐ­Òé°ü
	do{
		//Î´µÇÂ½Æ½Ì¨£¬ÆÕÍ¨Êý¾ÝÉÏ´«·ÅÆú
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
			status=ServUnlog_SendErr; //·¢ËÍÓÐÎÊÌâ
		}
		RetryWaittim=(Retry+1)*RetryWaittim; //JT808³¬Ê±µÈ´ý»úÖÆ
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
		DEBUG("LBS_DevCtlreq commandId error!",__FILE__, __func__, __LINE__);
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
			oa_uint8 *p_p = NULL;
			oa_uint8 len;
			DEBUG("wireless_updata");
			//extract paras
			DEBUG("param:%s", pmsgbody);
			oa_memset(&up_paras, 0x0, sizeof(up_paras));
			p_p = pmsgbody;
			for(i = 0;i < 11;i++){
				p = oa_strstr(p_p, ";");
				if (p){
					if (i == 1) p0 = p;//user name
					else if (i == 2) p1 = p;//password
					else if (i == 3) p2 = p;//ip
					else if (i == 4) p3 = p;//tcp port
					else if (i == 8) p4 = p;//firmware
					else if (i == 9) p5 = p;
					p_p = p+1;
					continue;
				}
				else DEBUG("paras err");
			}
			len = p1 - (p0+1);
			if (len > 0 && len < SERVER_IP_MAX_LEN){
				oa_memcpy(up_paras.usr, p0+1, len);
			}else{
				DEBUG("paras err");
				break;
			}
			len = p2 - (p1+1);
			if (len > 0 && len < SERVER_IP_MAX_LEN){
				oa_memcpy(up_paras.pw, p1+1, len);
			}else{
				DEBUG("paras err");
				break;
			}
			len = p3 - (p2+1);
			if (len > 0 && len < SERVER_IP_MAX_LEN){
				oa_memcpy(up_paras.ip, p2+1, len);
				if(!ip_is_valued(up_paras.ip, oa_strlen(up_paras.ip))){
					DEBUG("ip err");
					break;
				}
			}else{
				DEBUG("paras err");
				break;
			}
			
			char_to_short(p3+1, &up_paras.port);
			len = p5 - (p4+1);
			if (len > 0){
				oa_memcpy(up_paras.fw, p4+1, len);
			}else{
				DEBUG("paras err");
				break;
			}

			control_type = wireless_update;
			break;
		}
		
#if 0
		case eOnlineUpgrad_Ctrl:	//ÎÞÏßÉý¼¶
		{

			step=0;
			while(len<(msgbodylen-1)) //È¡Éý¼¶²ÎÊý
			{
				p = Str_Str(pmsgbody, ";");
				if(p)
				{
					Paralen=p-pmsgbody;
					if(Paralen)
						if(ActionOK!=ServReq_DevCtrl_WritOnlineUpgraparam(step,pmsgbody,Paralen))
							return 1;
					step++; 
					len+=(Paralen+1);
					pmsgbody+=(Paralen+1);
				}
				else //×îºó²ÎÊý
				{
					Paralen=(msgbodylen-1)-len;
					if(Paralen)
						if(ActionOK!=ServReq_DevCtrl_WritOnlineUpgraparam(step,pmsgbody,Paralen))
							return 1;
					step++; 
					len+=Paralen;
					pmsgbody+=Paralen;
				}
			}
			ReadLinkTaskPara(&i,NetStatusPara);
			if(i&LinkTLSeverOk)
			{
			}
			else
			{WriteUpdateServPara(0,NULL,0,UpdateFlash);
			WriteLinkTaskPara(ActionUpdateTL,ActionTypePara,SET);
			SetUpgradStarttimeS(); //peijl_20121029 Ìí¼Ó£ºÉèÖÃÉý¼¶¼ÆÊ±¿ªÊ¼Ê±¼ä
			}
		}
		break;

		case eSpclServ_Ctrl:	//¿ØÖÆÖÕ¶ËÁ¬½ÓÖ¸¶¨·þÎñÆ÷
		{
			if(*pmsgbody==1) //ÇÐ»»»ØÖ÷¼à¿¼Æ½Ì¨
			{
				gServLimittim=0; //ÎÞÊ±ÏÞ
				SetMonitServ(eMainServ);
				WriteLinkTaskPara(ActionHXPPP|ActionLoginHX,ActionTypePara,SET);
			}
			else{ //ÇÐ»»µ½¼à¹ÜÆ½Ì¨
				pmsgbody+=2;
				step=0;
				while(len<(msgbodylen-3)) //È¡²ÎÊý
				{
					p = Str_Str(pmsgbody, ";");
					if(p)
					{
						Paralen=p-pmsgbody;
						if(Paralen)
							if(ActionOK!=ServReq_DevCtrl_WritSpclServparame(step,pmsgbody,Paralen))
								return 1;
						step++; 
						len+=(Paralen+1);
						pmsgbody+=(Paralen+1);
					}
					else //×îºó²ÎÊý
					{
						Paralen=(msgbodylen-3)-len;
						if(Paralen)
							if(ActionOK!=ServReq_DevCtrl_WritSpclServparame(step,pmsgbody,Paralen))
								return 1;
						step++; 
						len+=Paralen;
						pmsgbody+=Paralen;
					}
				}			
				SetMonitServ(eSpclServ);
				WriteLinkTaskPara(ActionHXPPP|ActionLoginHX,ActionTypePara,SET);
			}
		}
		break;
#endif	
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
				else DEBUG("paras err");
			}
			len = p1 - (p0+1);
			if (len > 0 && len < SERVER_IP_MAX_LEN){
				oa_memcpy(up_paras.ip, p0+1, len);
				if(!ip_is_valued(up_paras.ip, oa_strlen(up_paras.ip))){
					DEBUG("ip err");
					break;
				}
			}else{
				DEBUG("paras err");
				break;
			}

			char_to_short(p1+1, &up_paras.port);
			control_type = conn_to_specified_server;
			break;
		}
		case eDevPowoff_Ctrl:	//ÖÕ¶Ë¹Ø»ú
		{
			DEBUG("power down");
			control_type = term_powerdown;
			break;
		}

		case eDevReset_Ctrl:	//ÖÕ¶Ë¸´Î»
		{
			//²ÎÊý±£´æflash
			//WriteLinkTaskPara(ActionResetLbs,ActionTypePara,SET);
			DEBUG("term reset");
			control_type = term_reset;
			break;
		}

		case eDevFactoryset_Ctrl:	//»Ö¸´³ö³§ÉèÖÃ
		{
			//¶Ï¿ªÆ½Ì¨Á¬½Ó
			//²ÎÊý±£´æflash
			//½ÓÊÕÍ¨»°
			#if 0//zq
			Factory_Set();
			WriteLinkTaskPara(ActionResetLbs,ActionTypePara,SET);
			#endif
			control_type = factory_setting;
			//factory_set();
			//oa_module_restart(NULL);
			break;
		}
		case eCloseDataCommu_Ctrl:	//¹Ø±ÕÊý¾ÝÍ¨ÐÅ
		{  
		#if 0
			//¹Ø±ÕÒÑ´æÔÚµÄÁ¬½Ó£¬¶ø²»ÉèÖÃÖØÁ¬±êÖ¾
			if(WCM_OK==WCM_IPLink_Query(SYS_CHANNEL))
			{
				if(WCM_OK!=WCM_IPCloseLink(SYS_CHANNEL))
					return 1;
				WriteLinkTaskPara(LinkHXSeverOk,NetStatusPara,RESET);
			}
			if(WCM_OK==WCM_IPLink_Query(UPDATE_CHANNEL))
			{
				if(WCM_OK!=WCM_IPCloseLink(UPDATE_CHANNEL))
					return 1;
				WriteLinkTaskPara(LinkTLSeverOk,NetStatusPara,RESET);
			}
			if(WCM_OK==WCM_GetPPPLinkStatus()) 
			{
				if(WCM_OK!=WCM_ClosePPPLink())
				{	  //¹Ø±ÕÊ§°ÜÊ±£¬Ö¤Ã÷ÓëÆ½Ì¨ÈÔ±£³ÖÁ¬½Ó
					WriteLinkTaskPara(LinkHXSeverOk,ActionTypePara,SET);					
					return 1;
				}
				WriteLinkTaskPara(TLPPPOk|HXPPPOk,NetStatusPara,RESET);			
			}
		#endif
			control_type = turnoff_datatraffic;
			break;
		}
		case eCloseWCMCommu_Ctrl:	//¹Ø±ÕÎÞÏÞÍ¨ÐÅ
		{ 
		#if 0
			//¹Ø±ÕÒÑ´æÔÚµÄÁ¬½Ó£¬¶ø²»ÉèÖÃÖØÁ¬±êÖ¾
			if(WCM_OK==WCM_IPLink_Query(SYS_CHANNEL))
			{
				if(WCM_OK!=WCM_IPCloseLink(SYS_CHANNEL))
					return 1;
				WriteLinkTaskPara(LinkHXSeverOk,NetStatusPara,RESET);
			}
			if(WCM_OK==WCM_IPLink_Query(UPDATE_CHANNEL))
			{
				if(WCM_OK!=WCM_IPCloseLink(UPDATE_CHANNEL))
					return 1;
				WriteLinkTaskPara(LinkTLSeverOk,NetStatusPara,RESET);
			}
			if(WCM_OK==WCM_GetPPPLinkStatus()) 
			{
				if(WCM_OK!=WCM_ClosePPPLink())
				{	  //¹Ø±ÕÊ§°ÜÊ±£¬Ö¤Ã÷ÓëÆ½Ì¨ÈÔ±£³ÖÁ¬½Ó
					WriteLinkTaskPara(LinkHXSeverOk,ActionTypePara,SET);					
					return 1;
				}
				WriteLinkTaskPara(TLPPPOk|HXPPPOk,NetStatusPara,RESET);			
			}
			//WCMÓ²¼þ¹Ø±Õ ÏÈÓÚµç»°¹Ò¶Ï


			//µç»°
			ReadTelPara(&i,eCallAction);
			if(NoneAction!=i) //Íâ²¿¹Ò¶Ï
			{	i=HangUp;
				WriteTelPara(&i,eCallAction);
			}
			break;
		#endif
			control_type = turnoff_allwireless;
			break;
		}
		default:
		{
	
			DEBUG("LBS_DevCtlreq commandId error!",__FILE__, __func__, __LINE__);
			return 0;
		}
	}
	
	return 0;
}
/*********************************************************
*Function:     check_track_location_ctl
*Description:  Æ½Ì¨ÇëÇó£ºÁÙÊ±Î»ÖÃÐÅÏ¢¸ú×Ù
*Calls:          
*Called By:      
*Input:         pmsgbody ²éÑ¯ÏûÏ¢ÌåÊ×µØÖ·
				msgbodylen ²éÑ¯ÏûÏ¢Ìå³¤¶È
*Output:        
*Return:        º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
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
*Description:  Ð£ÑéÆ½Ì¨ÏÂ·¢ÎÄ±¾ÐÅÏ¢ÏÂ·¢ÇëÇó
*ÎÄ±¾ÐÅÏ¢ÒÔÖÐÐÄ¶ÌÐÅ¸ñÊ½·¢ËÍµ÷¶ÈÆÁ
*Calls:          
*Called By:      
*Input:         pprotHandle ¾ä±úµÄÖ¸Õë
				pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:        º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
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
	//´¦Àí
	return 0;
	}
	else
	{
	type=*pbuf++;	//ÎÄ±¾´¦Àí·½Ê½,ÔÝÎ´·ÖÀà bit	
	if(buflen>1) //peijl_20121229 ÐÞ¸Ä£º¿ÕÎÄ±¾²»´¦Àí
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
				SScrn_SMS_Send('E',"000",3,Time,pbuf,buflen-1); //¶ÌÐÅ¸ñÊ½£¿£¿
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
*Function:     set_round_area
*Description:  ÏÂ·¢Ô²ÐÎÇøÓò
*Calls:          
*Called By:      
*Input:         pprotHandle ¾ä±úµÄÖ¸Õë
				pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:        º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
*Others:         
*********************************************************/
static u8 set_round_area(u8 *pbuf, u16 buflen)
{	
	u8 	option;//²Ù×÷ÀàÐÍ  0:¸üÐÂ  1:×·¼Ó 2:ÐÞ¸Ä
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
	//DEBUG(PrintDebug,"ÉèÖÃÔ²ÐÎÇøÓò\r\n");
	
	option =  *pbuf++;
	area_num = 	*pbuf++;

	switch (option){
		case 0:{
			DEBUG("ÉèÖÃÔ²ÐÎÇøÓò:¸üÐÂ");
		}break;
		case 1:{
			DEBUG("ÉèÖÃÔ²ÐÎÇøÓò:×·¼Ó");
		}break;
		case 2:{
			DEBUG("ÉèÖÃÔ²ÐÎÇøÓò:ÐÞ¸Ä");
		}break;
		default:{
			DEBUG("ÉèÖÃÔ²ÐÎÇøÓò:´íÎó");
		}break;
	}
	
	DEBUG("ÇøÓòÊýÄ¿:%d buflen:%d", area_num, buflen);
	if (area_num > MAX_AREA_SUM){
		DEBUG("area num is too large");
		return 1;
	}
	
	for(i=0;i<area_num;i++)
	{
		SaveAreaData(pbuf,Circular_Area,option,&read_len);  //±£´æ
		if(option == 0)	 //Èç¹ûÊÇ¸üÐÂ²Ù×÷£¬Ö»ÔÚµÚÒ»´ÎÇå³ý£¬ºóÃæÔò¸ÄÎª×·¼Ó
		{
			option = 1;//
		}
		pbuf+=read_len;
	}
	return 1;
}
/*********************************************************
*Function:     set_rect_area
*Description:  ÏÂ·¢¾ØÐÎÇøÓò
*Calls:          
*Called By:      
*Input:         pprotHandle ¾ä±úµÄÖ¸Õë
				pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:        º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
*Others:         
*********************************************************/
static u8 set_rect_area(u8 *pbuf, u16 buflen)
{
	u8 	option;//²Ù×÷ÀàÐÍ  0:¸üÐÂ  1:×·¼Ó 2:ÐÞ¸Ä
	u8 area_num;
	u16 read_len;
	u8 i;

	if((pbuf==NULL)||(buflen==0))
	{
		//DEBUG(PrintInfo,"check_car_ctl param error!\r\n");
		DEBUG("check_car_ctl param error!");
		return 1;
	}
	//DEBUG(PrintDebug,"ÉèÖÃ¾ØÐÎÇøÓò\r\n");
	option =  *pbuf++;
	area_num = 	*pbuf++;
	
	switch (option){
		case 0:{
			DEBUG("ÉèÖÃ¾ØÐÎÇøÓò:¸üÐÂ");
		}break;
		case 1:{
			DEBUG("ÉèÖÃ¾ØÐÎÇøÓò:×·¼Ó");
		}break;
		case 2:{
			DEBUG("ÉèÖÃ¾ØÐÎÇøÓò:ÐÞ¸Ä");
		}break;
		default:{
			DEBUG("ÉèÖÃ¾ØÐÎÇøÓò:´íÎó");
		}break;
	}
	
	DEBUG("ÇøÓòÊýÄ¿:%d buflen:%d", area_num, buflen);
	if (area_num > MAX_AREA_SUM){
		DEBUG("area num is too large");
		return 1;
	}
	
	for(i=0;i<area_num;i++)
	{
		SaveAreaData(pbuf,Rectangle_Area,option,&read_len);  //±£´æ
		if(option == 0)	 //Èç¹ûÊÇ¸üÐÂ²Ù×÷£¬Ö»ÔÚµÚÒ»´ÎÇå³ý£¬ºóÃæÔò¸ÄÎª×·¼Ó
		{
			option = 1;//
		}
		pbuf+=read_len;
	}
	return 1;
}
	
/*********************************************************
*Function:     set_poly_area
*Description:  ÏÂ·¢¶à±ßÐÎÇøÓò
*Calls:          
*Called By:      
*Input:         pprotHandle ¾ä±úµÄÖ¸Õë
				pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:        º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
*Others:         
*********************************************************/
static u8 set_poly_area(u8 *pbuf, u16 buflen)	
{
	u8 	option;//²Ù×÷ÀàÐÍ  0:¸üÐÂ  1:×·¼Ó 2:ÐÞ¸Ä
	u8 area_num;
	u16 read_len;
	u8 i;

	if((pbuf==NULL)||(buflen==0))
	{
		//DEBUG(PrintInfo,"check_car_ctl param error!\r\n");
		DEBUG("check_car_ctl param error!");
		return 1;
	}
	//DEBUG(PrintDebug,"ÉèÖÃ¶à±ßÐÎÇøÓò\r\n");
	option =  *pbuf++;
	area_num = 	*pbuf++;

	switch (option){
		case 0:{
			DEBUG("ÉèÖÃ¶à±ßÐÎÇøÓò:¸üÐÂ");
		}break;
		case 1:{
			DEBUG("ÉèÖÃ¶à±ßÐÎÇøÓò:×·¼Ó");
		}break;
		case 2:{
			DEBUG("ÉèÖÃ¶à±ßÐÎÇøÓò:ÐÞ¸Ä");
		}break;
		default:{
			DEBUG("ÉèÖÃ¶à±ßÐÎÇøÓò:´íÎó");
		}break;
	}
	
	DEBUG("ÇøÓòÊýÄ¿:%d buflen:%d", area_num, buflen);
	if (area_num > MAX_AREA_SUM){
		DEBUG("area num is too large");
		return 1;
	}
	
	for(i=0;i<area_num;i++)
	{
		SaveAreaData(pbuf,Poly_Area,option,&read_len);  //±£´æ
		if(option == 0)	 //Èç¹ûÊÇ¸üÐÂ²Ù×÷£¬Ö»ÔÚµÚÒ»´ÎÇå³ý£¬ºóÃæÔò¸ÄÎª×·¼Ó
		{
			option = 1;//
		}
		pbuf+=read_len;
	}
	return 1;
}
/*********************************************************
*Function:     del_area_message
*Description:  É¾³ýÇøÓò
*Calls:          
*Called By:      
*Input:         pprotHandle ¾ä±úµÄÖ¸Õë
				pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:        º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
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
			DEBUG("É¾³ýÔ²ÐÎÇøÓòÊýÄ¿:%d", area_num);
		}break;
		case Rectangle_Area:{
			DEBUG("É¾³ý¾ØÐÎÇøÓòÊýÄ¿:%d", area_num);
		}break;
		case Poly_Area:{
			DEBUG("É¾³ý¶à±ßÐÎÇøÓòÊýÄ¿:%d", area_num);
		}break;
		default:{
			DEBUG("É¾³ýÇøÓò³ö´í");
		}break;
	}
	
	if(area_num)
	{
		for(i=0;i<area_num;i++)
		{
			 DelAreaData(pbuf,area_type);  //É¾³ýÇøÓò
			 pbuf+=4;
		}
	}
	else
	{
		DelAreaData(NULL,area_type);//É¾³ýËùÓÐÇøÓò
	}
	return 1;
}

/*********************************************************
*Function:		ServReq_808data_handle
*Description:	¶Ô¼à¿ØÆ½Ì¨ÇëÇóÃüÁîµÄ´¦Àí¼°Ó¦´ð
*				1.¼òµ¥´¦ÀíÔÚ±¾º¯ÊýÍê³É
*				2.ÐèÈÎÎñµ÷¶È×öÍ¨Öª£¬²¢´«²ÎÊý
*				3.ÇëÇó°üÓ¦´ð
*Calls:          
*Called By:		JT808_PrtclAnaly() 
*Input:         ServReqMsgid Æ½Ì¨ÇëÇóÏûÏ¢id
				msgbody	ÇëÇóÏûÏ¢µÄÏûÏ¢ÌåÊ×Ö·
				msgbodylen ÇëÇóÏûÏ¢µÄÏûÏ¢Ìå³¤¶È
*Output:        
*Return:        ´¦Àí½á¹û ·µ»Ø0±íÊ¾³É¹¦
*Others:         
*********************************************************/
u8 JT808_ServReq_handle(u16 ServReqMsgid,u8 *msgbody,u16 msgbodylen/*,u8 *sendbuf,u16 sendbuflen*/)
{
	u8 	status = 0;
	u16 senddatalen=0; //Ó¦´ð»ò·¢ËÍÊý¾Ý³¤¶È
	oa_bool need_reconn = OA_FALSE;
	if(msgbody==NULL)
	{
		DEBUG(" check_msg_rsp parameter error!");
		return RspErrOther;
	}
	//ÏûÏ¢Ìå´¦Àí
	//JT808_ServReq_MSG();
	switch(ServReqMsgid){
		case SET_DEV_PARAM:{//platform request to set device paramaters & device ack common 
			//DEBUG(" Recieve parameter-setting requry from moniter server.");
			status = ServReq_SetParam(msgbody,msgbodylen);	//´¦Àí
			Write_ProtclHandl(eRsp2ServSeq,&status,1); //peijl_20120918 Ìí¼Ó£ºÇëÇó´¦Àí½á¹û
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/); //PEIJL_130109 ÐÞ¸Ä
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
//status=3 ÎªÔÝÊ±´¦Àí
		
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
			set_round_area(msgbody,msgbodylen);
			status=0;
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
			set_rect_area(msgbody,msgbodylen);
			status=0;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/);
		break;
		case DEL_SQUARE_AREA:
			del_area_message(msgbody,msgbodylen,Rectangle_Area);
			status=0;
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/);
		break;
		case SET_POLYGON_AREA:
			set_poly_area(msgbody,msgbodylen);
			status=0;
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
				SchedulScrn_CallforcarSend("8888888888",10,p->OrderContent,p->OrderContentlen); //ID¹Ì¶¨
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
				DEBUG(PrintDebug,"---------ÄÚÈÝ³¤¶È:%d \r\n",p->OrderContentlen); 
				DEBUG(PrintDebug,"---------ÄÚÈÝ:%s \r\n",p->OrderContent); 
				SchedulScrn_CallforcarInfoSend("8888888888",10,p->OrderTel,strlen(p->OrderTel),p->OrderContent,p->OrderContentlen); //ID¹Ì¶¨
			}
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			//peijl_test ÔÝ¶¨¶ÔÓ¦
			*sendbuf=1; //1³É¹¦ 2Ê§°Ü
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
			//ÉÏ±¨ÏßÂ·
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
*Description:	¶¨Î»ÏµÍ³½»±ê808Ð­Òé½âÎö.
*				1.Ð­ÒéÐ£Ñé
*				2.½âÎö£¬Êý¾Ý°ü·ÖÎªÖÕ¶ËÇëÇó»ØÓ¦ºÍÆ½Ì¨Ö÷¶¯ÇëÇó°ü
*Calls:			many
*Called By:      
*Input:			data ½ÓÊÕµÄÆ½Ì¨Êý¾ÝÊ×µØÖ·
				datalen ½ÓÊÕÆ½Ì¨Êý¾Ý³¤
				sendbuf »º´æÇø
				sendbuflen	»º³åÇøÊµ¼Ê³¤
*Return:		unknown
*Others:         
**************************************************************/
u8 JT808_recv_analysis(u8 *data,u16 datalen/*,u8 *sendbuf,u16 sendbuflen*/)
{
	u8 temp=0;
	u8 *pbuf=data;
	u16 ServmsgID=0;
	u16 Reallen; //Ð­Òé°üÊµ¼Ê³¤¶È
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
	if(NULL==data || datalen==0)  //Ð­Òé×îÐ¡³¤¶ÈÎ´¶¨Òå £¿£¿£¿
		return RspPackgerr;	
	//±êÊ¾Í·Î²ÅÐ¶Ï
	if(*data!=0x7e || *(data+datalen-1)!=0x7e){
		DEBUG(" 0x7e is bad!!!");
		return RspPackgerr;	
	}
		
	//·´×ªÒå»¹Ô­Êý¾Ý
	if(JT808_dataChg(0, data+1, datalen-2, &Reallen)){
		DEBUG(" turning parameter error");
		return RspPackgerr;
	}
	Reallen += 2; //¼ÓÉÏ±êÊ¾Í·Î²
#if 0
	if(0){
	u8 test;
	DEBUG(PrintDebug,"Server data after translition:!\r\n"); 	
	for(test=0;test<Reallen;test++)
		DEBUG(PrintDebug,"%02x ",*(data+test));
	DEBUG(PrintInfo,"\r\n");
	}
#endif
	//msgidÓÐÐ§ÐÔÅÐ¶Ï
	char_to_short(data+1, &ServmsgID);
	if (!ISPrtclServMsg(ServmsgID)){
		DEBUG(" Unknown server message ID!");
		return RspPackgerr;
	}
	//Ð£ÑéÎ»ÅÐ¶Ï
	if (!XOR_Check(data+1,Reallen-3,&temp)){
		if (temp!=*(data+Reallen-2)){
			DEBUG(" Server data xor err!"); 	
			return RspPackgerr;
		}
	}
	else
		return RspPackgerr;	    //Ó¦×·¼ÓÏûÏ¢´íÎóÓ¦´ð ???

	pbuf++;//pbuf --> msg id
	switch (ServmsgID){
		/*4ÖÖÆ½Ì¨Ó¦´ð*/
		case REGISTERS_rsp:{	//×¢²áÓ¦´ð
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
		case REPORT_MEDIA_DATA_rsp:	//¶àÃ½ÌåÊý¾ÝÉÏ´«Ó¦´ð
		{
			LBS_PlatComRsp rsp;
			DEBUG("Recieve Result of media data uploading."); 	
			memcpy(rsp.MsgId,pbuf,2);
			pbuf+=sizeof(MSG_HEAD);
			pbuf+=4; //media idÎÞÐ§
			if(*pbuf==0)
			{
				rsp.Rslt=0;//³É¹¦
			}
			else
			{
		//	if(Reallen-3-5-sizeof(MSG_HEAD)>0)
			//°üÁÐ±í---Ð­ÒéÎ´¶¨ £¿£¿£¿				
				rsp.Rslt=1;//Ê§°Ü
				DEBUG(PrintInfo,"Media data report failed!\r\n"); 	
			}
			Write_ProtclHandl(eRsp2DevReq,(u8 *)&rsp,sizeof(LBS_PlatComRsp));
			OSSemPost(JT808_ReqRspSem);
		}
		
		break;
#endif
		case PLAT_COMMON_rsp:{ //Æ½Ì¨Í¨ÓÃÓ¦´ð
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
				DEBUG("seq:%02x msgid:%04x seq2:%02x msgid2:%04x", temp_seq, temp_msgid, temp_seq2, temp_msgid2);
				if (timeout_var.timeout_en == OA_TRUE && temp_seq == temp_seq2-1 && temp_msgid == temp_msgid2){
					DEBUG("disable timeout");
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
	/*	case RSA_PLAT: //Æ½Ì¨¹«Ô¿ÏÂ·¢
		{
			//Õâ¸öµØ·½µÄÊµÏÖÓÐ´ýÉÌÈ¶
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
		default:{//ÒÔÏÂNÖÖÎªÆ½Ì¨ÇëÇó
			Write_ProtclHandl(eServMsgid, pbuf, 2);
			Write_ProtclHandl(eServSeqid, (pbuf+10), 2);//¼ÇÂ¼Æ½Ì¨ÏûÏ¢Á÷Ë®ºÅ
			if(*(pbuf+2)&0x20) {//ÓÐ·Ö°üÏî peijl_20120912
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
#if 0
/*************************************************************
*Function:		Pro808Analysis
*Description:	¶¨Î»ÏµÍ³½»±ê808Ð­Òé½âÎö.
*				1.Ð­ÒéÐ£Ñé
*				2.½âÎö£¬Êý¾Ý°ü·ÖÎªÖÕ¶ËÇëÇó»ØÓ¦ºÍÆ½Ì¨Ö÷¶¯ÇëÇó°ü
*Calls:			many
*Called By:      
*Input:			data ½ÓÊÕµÄÆ½Ì¨Êý¾ÝÊ×µØÖ·
				datalen ½ÓÊÕÆ½Ì¨Êý¾Ý³¤
				sendbuf »º´æÇø
				sendbuflen	»º³åÇøÊµ¼Ê³¤
*Return:		unknown
*Others:         
**************************************************************/
u8 JT808_PrtclAnaly(u8 *data,u16 datalen,u8 *sendbuf,u16 sendbuflen)
{
	u8 temp=0;
	u8 *pbuf=data;
	u16 ServmsgID=0;
	u16 Reallen; //Ð­Òé°üÊµ¼Ê³¤¶È
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
	if(NULL==data || datalen==0)  //Ð­Òé×îÐ¡³¤¶ÈÎ´¶¨Òå £¿£¿£¿
		return RspPackgerr;	
	//±êÊ¾Í·Î²ÅÐ¶Ï
	if(*data!=0x7e || *(data+datalen-1)!=0x7e)
		return RspPackgerr;	
	//·´×ªÒå»¹Ô­Êý¾Ý
	if(JT808_dataChg(0, data+1, datalen-2, &Reallen))
	{
		DEBUG("turning parameter error");
		return RspPackgerr;
	}
	Reallen+=2; //¼ÓÉÏ±êÊ¾Í·Î²
#if 0
	if(0){
	u8 test;
	DEBUG(PrintDebug,"Server data after translition:!\r\n"); 	
	for(test=0;test<Reallen;test++)
		DEBUG(PrintDebug,"%02x ",*(data+test));
	DEBUG(PrintInfo,"\r\n");
	}
#endif
	//msgidÓÐÐ§ÐÔÅÐ¶Ï
	char_to_short(data+1, &ServmsgID);
	if(!ISPrtclServMsg(ServmsgID))
	{
		DEBUG("Unknown server message ID!");
		return RspPackgerr;
	}
	//Ð£ÑéÎ»ÅÐ¶Ï
	if(!XOR_Check(data+1,Reallen-3,&temp))
	{
		if(temp!=*(data+Reallen-2))	
		{
			DEBUG("Server data xor err!"); 	
			return RspPackgerr;
		}
	}
	else
		return RspPackgerr;	    //Ó¦×·¼ÓÏûÏ¢´íÎóÓ¦´ð ???

	pbuf++;//pbuf --> msg id
	switch(ServmsgID)
	{
		/*4ÖÖÆ½Ì¨Ó¦´ð*/
		case REGISTERS_rsp:	//×¢²áÓ¦´ð
		{
			LBS_PlatComRsp rsp;
			memcpy(rsp.MsgId,pbuf,2);
			pbuf+=sizeof(MSG_HEAD);
			memcpy(rsp.SeqId,pbuf,2);
			pbuf+=2;
			rsp.Rslt=*pbuf;
			if(rsp.Rslt==0)
			{
				pbuf++;
				temp=Reallen-3-3-sizeof(MSG_HEAD);
				if(temp>0)
				{ 	//¼øÈ¨ÂëºÍÒÑ×¢²á×´Ì¬´æÈëflash
					#if 0
					Flash_Write_Buffer(NULL,0,DEV_REGSTA_ADDR,Sector_Erase);
					Flash_Write_Buffer(&temp,1,DEV_AUTHEN_LEN_ADDR,Write_Mode);	
					Flash_Write_Buffer(pbuf,temp,DEV_AUTHEN_ADDR,Write_Mode);
					temp = Regin;
					Flash_Write_Buffer(&temp,1,DEV_REGSTA_ADDR,Write_Mode);
					#endif
					authen_code_handle(pbuf, temp);//write Authentication Code
				}
			}
			Write_ProtclHandl(eRsp2DevReq,(u8 *)&rsp,sizeof(LBS_PlatComRsp));	
//			OSSemPost(JT808_ReqRspSem);
			DEBUG("Recieve Registed result."); 	
		}
		break;
		#if 0
		case REPORT_MEDIA_DATA_rsp:	//¶àÃ½ÌåÊý¾ÝÉÏ´«Ó¦´ð
		{
			LBS_PlatComRsp rsp;
			DEBUG("Recieve Result of media data uploading."); 	
			memcpy(rsp.MsgId,pbuf,2);
			pbuf+=sizeof(MSG_HEAD);
			pbuf+=4; //media idÎÞÐ§
			if(*pbuf==0)
			{
				rsp.Rslt=0;//³É¹¦
			}
			else
			{
		//	if(Reallen-3-5-sizeof(MSG_HEAD)>0)
			//°üÁÐ±í---Ð­ÒéÎ´¶¨ £¿£¿£¿				
				rsp.Rslt=1;//Ê§°Ü
				DEBUG(PrintInfo,"Media data report failed!\r\n"); 	
			}
			Write_ProtclHandl(eRsp2DevReq,(u8 *)&rsp,sizeof(LBS_PlatComRsp));
			OSSemPost(JT808_ReqRspSem);
		}
		#endif
		break;
		case PLAT_COMMON_rsp: //Æ½Ì¨Í¨ÓÃÓ¦´ð
		{
			LBS_PlatComRsp rsp;
			DEBUG("Recieve common-response form moniter server."); 	
			pbuf+=sizeof(MSG_HEAD);
			memcpy(&rsp,pbuf,sizeof(LBS_PlatComRsp));
			Write_ProtclHandl(eRsp2DevReq,(u8 *)&rsp,sizeof(LBS_PlatComRsp));	
			DEBUG("Server common reply:\r\n"); 	
			DEBUG("DevReqID    0x%02x%02x\r\n",*pbuf,*(pbuf+1)); 	
			DEBUG("    DevReqmsgID 0x%02x%02x\r\n",*(pbuf+2),*(pbuf+3)); 	
			DEBUG("    Rsp result  %02x\r\n",*(pbuf+4)); 	
			//OSSemPost(JT808_ReqRspSem);
		}
		break;
	/*	case RSA_PLAT: //Æ½Ì¨¹«Ô¿ÏÂ·¢
		{
			//Õâ¸öµØ·½µÄÊµÏÖÓÐ´ýÉÌÈ¶
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
	*/	//ÒÔÏÂNÖÖÎªÆ½Ì¨ÇëÇó
		default:
		{
			Write_ProtclHandl(eServMsgid,pbuf,2);
			Write_ProtclHandl(eServSeqid,(pbuf+10),2);
			if(*(pbuf+2)&0x20) //ÓÐ·Ö°üÏî peijl_20120912
			{
				pbuf+=(sizeof(MSG_HEAD)+sizeof(MSG_HEAD_sub));
				U16Temp=Reallen-3-(sizeof(MSG_HEAD)+sizeof(MSG_HEAD_sub));
			}
			else
			{
				pbuf+=sizeof(MSG_HEAD);
				U16Temp=Reallen-3-sizeof(MSG_HEAD);
			}
			JT808_ServReq_handle(ServmsgID,pbuf,U16Temp,sendbuf,sendbuflen);
		}
		break;
	}
	return 0;					
}
#endif
/*********************************************************
*Function:       dev_common_rsp_buildbody
*Description:   ÖÕ¶ËÍ¨ÓÃÓ¦´ð
*Calls:          
*Called By:      
*Input:          pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:         º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
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
*Description:	¶ÁÈ«¾Ö±äÁ¿LbsCfgStruct
*input:			CfgPara ²ÎÊý¶ÔÓ¦µÄJTT808Ð­ÒéID
*output:		¶Á³ö*buflen³¤¶ÈµÄÊý¾Ý´æ·Åµ½µØÖ·pValue
*Return:		¶ÁÈ¡µÄÊý¾Ý³¤¶È 0:err other:³¤¶È  ¼Æ»®ÐÞ¸Ä
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
			if(LbsCfgStruct.LclTELlen>12) //808Ð­Òé×î´ó12×Ö½Ú peijl_130311 Ìí¼Ó±¾»úºÅ´íÎó£¬ÉèÄ¬ÈÏÖµ
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
		case eOdometer: //Àï³Ì±í¾«¶È 
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
			Mem_Copy(pValue, dev_now_params.manufacturers_id, 5);//5¹Ì¶¨³¤¶È
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
*Description:	´´½¨ÖÕ¶Ë×¢²áÏûÏ¢Ìå
*Calls:          
*Called By:      
*Input:			pprotHandle ¾ä±úµÄÖ¸Õë
				Buf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:		pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:		º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
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
*Description:    ´´½¨ÖÕ¶Ë¼øÈ¨ÇëÇó°üÏûÏ¢Ìå
*Calls:          
*Called By:      
*Input:
				pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:		º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»ØAcktionOK±íÊ¾³É¹¦
*Others:         
*********************************************************/
static u8  dev_login_buildbody(u8 *pbuf, u16 *pbuflen)
{
	oa_bool ret;
	//if(eSpclServ==GetMonitServ()) //¼à¹ÜÆ½Ì¨:»ñÈ¡ÊÇÄÄ¸ö¼à¹ÜÆ½Ì¨
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
*Description:	ÖÕ¶Ë¶ÔÆ½Ì¨ÇëÇó²éÑ¯²ÎÊýµÄÓ¦´ð°ü
*Calls:          
*Called By:      
*Input:          pprotHandle ¾ä±úµÄÖ¸Õë
			pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:         º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
*Others:         ´Ë¹¦ÄÜÐèºÍÆ½Ì¨¶Ô½Óºó·½Öª
	peijl_20120911 ÐÞ¸Ä
*********************************************************/
static u8 get_dev_param_rsp_buildbody(u8 *msgbody, u16 *pbuflen)
{
//	PARAM_LIST *paramlist=NULL;
	u8 len=0;
	u32 para;
	u8 *pbuf=msgbody;
	u8 NUM=0; //²ÎÊý¸öÊý
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
//Î»ÖÃÐÅÏ¢»ã±¨µÄ»ù±¾ÐÅÏ¢ÏûÏ¢Ìå´´½¨
u8 report_location_msgbody1(u8 *Buf, u16 *pbuflen)
{
	u8 *pbuf=Buf;
	u32 alarmflag;
	float temp=0.0;
	u8  cnt=0;
	STRUCT_RMC GpsInfor;

	if(Buf==NULL||pbuflen==NULL)
		return 1;
	GPS_GetPosition(&GpsInfor); //È¡gps
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
	for(cnt=0;cnt<2;cnt++) //Î³¶È,°ÙÍò·ÖÖ®Ò»¶È
	{
		alarmflag=alarmflag*10+GpsInfor.Latitude[cnt]-'0';
	}
	for(cnt=2;cnt<8;cnt++) //Î³¶È
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
	for(cnt=0;cnt<3;cnt++) //¾­¶È,°ÙÍò·ÖÖ®Ò»¶È
	{
		alarmflag=alarmflag*10+GpsInfor.Longitude[cnt]-'0';
	}
	for(cnt=3;cnt<9;cnt++) //¾­¶È
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
	//zq modified here
//	if(GpsInfor.Time[0]==0&&GpsInfor.Time[1]==0&&GpsInfor.Time[2]==0&&GpsInfor.Time[3]==0&&GpsInfor.Time[4]==0&&GpsInfor.Time[5]==0)
//		app_GetRtcTime(pbuf);
//	else
		memcpy(pbuf,GpsInfor.Time,6);
	pbuf+=6;
	*pbuflen += 6;
	return 0;
}
//Î»ÖÃÐÅÏ¢»ã±¨µÄ¸½¼ÓÐÅÏ¢ÏîÏûÏ¢Ìå´´½¨
static u8 report_location_msgbody2(u8 *Buf, u16 *pbuflen)
{
	u8 *pbuf=Buf;
	u32 alarmflag;

	if(Buf==NULL||pbuflen==NULL)
		return 1;
	//Î»ÖÃ¸½¼ÓÐÅÏ¢ÏîÁÐ±í
	*pbuf++=0x01;//Àï³Ì
	*pbuf++=0x04;
	//ReadDistanceData(&alarmflag); //KM?	peijl_test
	{
		//read mileage by zq
		oa_int32 handle, ret;
		TOTAL_MILE temp;
		oa_uint32 dummy_read;
		handle = oa_fopen(MILEAGE_FILE);
		if (handle < 0){
			DEBUG("mileage file open err!");
			alarmflag = 0;//var name is bad here
		}
		else{
			ret = oa_fread(handle, &temp, sizeof(TOTAL_MILE), &dummy_read);
			if ((ret < 0) || (dummy_read != sizeof(TOTAL_MILE))) {
				OA_DEBUG_USER("read mileage err!");
				alarmflag = 0;
			}
			alarmflag = temp.total_mileage;
			ret = oa_fclose(handle);
			if (ret < 0){
				OA_DEBUG_USER("close file err!", __FILE__,  __func__, __LINE__);
				oa_fdelete(MILEAGE_FILE);
			}
		}
		
	}
	int_to_char(pbuf, alarmflag); //100M
	pbuf+=4;
	*pbuflen += 6;
	*pbuf++=0x02;//ÓÍÁ¿
	*pbuf++=0x02;
	*pbuf++=0x00;	//ÔÝ¶¨
	*pbuf++=0x64;
	*pbuflen +=4;
	*pbuf++=0x03;//ÐÐÊ»¼ÇÂ¼ÒÇËÙ¶È
	*pbuf++=0x02;
//	short_to_char(pbuf,gDriveRecodSpeed);  //??????	 
	memset(pbuf,0x00,2);
	pbuf+=2;
	*pbuflen +=4;
	*pbuf++=0x11;//³¬ËÙ±¨¾¯¸½¼ÓÐÅÏ¢
	*pbuf++=0x01;
	*pbuf++=0x00; //ÔÝ¶¨
	*pbuflen +=3;
/*	*pbuf=0x12;//½ø³öÇøÓò»òÂ·Ïß
	pbuf++;
	*pbuf=0x06;
	pbuf++;
	memset(pbuf,0x0,6);
	*pbuflen +=8;
	*pbuf=0x13;//½ø³öÇøÓò»òÂ·Ïß
	pbuf++;
	*pbuf=0x07;
	pbuf++;
	memset(pbuf,0x0,7);
	*pbuflen +=9;
*/

#if 0
	ADD_LOCATION_INFO addInfo;
//	READ(meliage,oil,meliage,locationtype11,area_line_id11,locationtype22,area_line_id22,direction);
	addInfo.AddInfoId = 0x1;		//Àï³Ì£¬¶ÔÓ¦³µÉÏµÄÀï³Ì±í¶ÁÊý
	addInfo.AddInfoLen = 4;
	memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
	*pbuflen += sizeof(ADD_LOCATION_INFO);
	memcpy(pbuf+*pbuflen, "1111", addInfo.AddInfoLen);
	*pbuflen += addInfo.AddInfoLen;

	addInfo.AddInfoId = 0x2;		//ÓÍÁ¿£¬¶ÔÓ¦³µÉÏÓÍÁ¿±í¶ÁÊý
	addInfo.AddInfoLen = 2;
	memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
	*pbuflen += sizeof(ADD_LOCATION_INFO);
	memcpy(pbuf+*pbuflen, "22", addInfo.AddInfoLen);
	*pbuflen += addInfo.AddInfoLen;

	addInfo.AddInfoId = 0x3;		//ËÙ¶È£¬ÐÐÊ»¼ÇÂ¼¹¦ÄÜ»ñÈ¡µÄËÙ¶È
	addInfo.AddInfoLen = 2;
	memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
	*pbuflen += sizeof(ADD_LOCATION_INFO);
	memcpy(pbuf+*pbuflen, "33", addInfo.AddInfoLen);
	*pbuflen += addInfo.AddInfoLen;

	if(alarmflag & ALARM_OVER_SPEED)
	{
		addInfo.AddInfoId = 0x11;		//³¬ËÙ±¨¾¯£¬
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
		addInfo.AddInfoId = 0x12;		//½ø³öÇøÓò/Â·Ïß±¨¾¯¸½¼ÓÐÅÏ¢
		addInfo.AddInfoLen = 1;
		memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
		*pbuflen += sizeof(ADD_LOCATION_INFO);
		*(pbuf+(*pbuflen)) = 2;
		*pbuflen += addInfo.AddInfoLen;
		addInfo.AddInfoLen = 4;
		memcpy(pbuf+(*pbuflen), "1234", addInfo.AddInfoLen);
		*pbuflen += addInfo.AddInfoLen;
		addInfo.AddInfoLen = 1;
		*(pbuf+(*pbuflen)) = 0;//0½ø/1³ö
		*pbuflen += addInfo.AddInfoLen;
	}
	if(alarmflag & ALARM_DRIVE_SHORT_LONG_k)
	{
		addInfo.AddInfoId = 0x13;		//Â·¶ÎÐÐÊ»Ê±¼ä²»×ã/¹ý³¤±¨¾¯
		addInfo.AddInfoLen = 4;
		memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
		*pbuflen += sizeof(ADD_LOCATION_INFO);
		memcpy(pbuf+(*pbuflen), "1111", addInfo.AddInfoLen);
		*pbuflen += addInfo.AddInfoLen;
		addInfo.AddInfoLen = 2;
		memcpy(pbuf+(*pbuflen), "22", addInfo.AddInfoLen);
		*pbuflen += addInfo.AddInfoLen;
		addInfo.AddInfoLen = 1;
		*(pbuf+(*pbuflen)) = 0;//0²»×ã1¹ý³¤
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
	//Î»ÖÃ¸½¼ÓÐÅÏ¢µÄ×Ô¶¨ÒåÇøÓò
	*pbuf++=0xE0;
	*pbuf++=0x04;
	*pbuf++=0x00;
	*pbuf++=0x00;
	*pbuf++=0x00;
	*pbuf++=0x00; //??
	*pbuflen=6;

	*pbuf++=0xE1; //Õý·´×ª
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
*Description:	´´½¨Î»ÖÃÐÅÏ¢»ã±¨ÏûÏ¢Ìå
*Calls:          
*Called By:      
*Input:			
				Buf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:         º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
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
*Description:    Î»ÖÃÐÅÏ¢²éÑ¯Ó¦´ð
*Calls:          
*Called By:      
*Input:          pprotHandle ¾ä±úµÄÖ¸Õë
			pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:         º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
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
*Description:   ÊÂ¼þ±¨¸æ
*Calls:          
*Called By:      
*Input:          pprotHandle ¾ä±úµÄÖ¸Õë
			pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:         º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
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
*Description:   ÌáÎÊÓ¦´ð
*Calls:          
*Called By:      
*Input:          pprotHandle ¾ä±úµÄÖ¸Õë
			pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:         º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
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
*Description:   ÐÅÏ¢µã²¥/È¡Ïû
*Calls:          
*Called By:      
*Input:          pprotHandle ¾ä±úµÄÖ¸Õë
			pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:         º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
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
*Description:   ³µÁ¾¿ØÖÆÓ¦´ð
*Calls:          
*Called By:      
*Input: 		pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:         º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
*Others:         
*********************************************************/
static u8 car_ctl_rsp_buildbody(u8 *pbuf, u16 *pbuflen)
{
	u16 len;
	Read_ProtclHandl(eServSeqid,pbuf,pbuflen); //Æ½Ì¨µÄÁ÷Ë®ºÅ
	pbuf+=*pbuflen;
	report_location_buildbody(pbuf, &len);
	*pbuflen += len;
	return 0;
}
/*********************************************************
*Function:       BuildMsgbody
*Description:   ¸ù¾Ýmsg_IDµÄÖµ£¬Çø±ðµ÷ÓÃµÄÊÇÄÄ¸öÏûÏ¢Ìå
*Calls:          
*Called By:      BuildMsg
*Input: 		pbuf ÊÇÍâ²¿»º³åÇøÊ×µØÖ·
*Output:        pbuflen ÊÇ×é³ÉµÄ±¨ÎÄ³¤¶È
*Return:         º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
*Others:         
*********************************************************/
static u8 BuildMsgbody(u16 DevMsgId, u8 *msgbody, u16 *msgbodylen, u16 totalPackt,u16 SubPackt)
{
	u8 status=0;
	if(msgbody==NULL || msgbodylen==NULL ||totalPackt==0)
	{
		return 1;
	}
	
	switch(DevMsgId)//ÖÕ¶ËÏûÏ¢
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
			//¿ÕÏûÏ¢Ìå
			*msgbodylen=0;
			break;
		}
		case HEART_BEAT:
		{
			//Write_ProtclHandl(eDevMsgid, (u8 *)&DevMsgId, 2);//ÖÕ¶Ë·¢ËÍÏûÏ¢ID by zhuqing @2013/6/26
			//timeout_var.timeout_en = OA_TRUE;
			//timeout_var.do_timeout = OA_FALSE;
			//¿ÕÏûÏ¢Ìå
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
			status=report_drive_data_buildbody(msgbody, msgbodylen,totalPackt,SubPackt);	  //´´½¨ÐÐÊ»¼ÇÂ¼ÉÏ´«ÏûÏ¢°ü
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
		case SMEDIA_DATA_CHECK_rsp: //¶àÃ½Ìå¼ìË÷
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
*Description:	 ¶ÁÈ¡±¾»úºÅ²¢»»³ÉBCDÂë
*Input:          Deslen BCDÂëÎ»Êý
				 Srcstr »º³åÇø
*Output:		 DesTel BCD±¾»úºÅ´æ·ÅµØÖ·
*Return:         º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬·µ»Ø0±íÊ¾³É¹¦
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
*Description:   ×é½¨ÖÕ¶Ë·¢ËÍ±¨ÎÄ
*Calls:          
*Called By:      
*Input:          DevMsgId Òª·¢ËÍµÄÖÕ¶ËÏûÏ¢ÀàÐÍ
				 totalPackt	·¢ËÍ×Ü°üÊý
				 SubPackt	·¢ËÍµ±Ç°°üºÅ£¬JT808´Ó1¿ªÊ¼
				 Sendbuf£¬Sendbuflen	·¢ËÍ»º³åÇøºÍ³¤¶È
				 Senddatalen ·¢ËÍ±¨ÎÄÊµ¼Ê³¤¶È
*Output:         
*Return:         º¯ÊýÊÇ·ñÖ´ÐÐ³É¹¦£¬0³É¹¦ / 1
*Others:          
*********************************************************/
u8 JT808Msg_Build(u16 DevMsgId,u16 totalPackt,u16 SubPackt,u8 *Sendbuf,u16 Sendbuflen,u16 *Senddatalen)
{
	u8  status = 0;
	u8 	U8Temp = 0;
	u8  *pbuf = NULL;
	u8  tbuf[15] = {0};
	MSG_HEAD *msghead = (MSG_HEAD *)(Sendbuf+1); //ÏûÏ¢Í·
	MSG_HEAD_bodyattr msgbody_attr;	   //ÏûÏ¢ÌåÊôÐÔ
	u16 U16Temp = 0;
	
	if (!ISPrtclDevMsg(DevMsgId)||Sendbuf==NULL||Senddatalen==NULL||Sendbuflen==0){
		DEBUG(" JT808Msg_Build parameter error!");
		return 1;
	}
	pbuf = Sendbuf;

	//×é½¨ÏûÏ¢Í·!!!
	*pbuf++=0x7e;
	*Senddatalen=1;
	memset(msghead,0x00,sizeof(MSG_HEAD)); //²»ÉèÁ÷Ë®ºÅ£¬Á÷Ë®ºÅÔÚ·¢ËÍÊ±ÖØÉè
	short_to_char(msghead->MsgId, DevMsgId);
	memset(&msgbody_attr,0x00,2); //Ä¬ÈÏÏûÏ¢²»¼ÓÃÜ
	if (totalPackt > 1){
		msgbody_attr.subpacket = 1;
	}
	else{
		msgbody_attr.subpacket = 0;
	}
	
	ReadLbsCfgPara(eLclTEL,tbuf,&U8Temp);
	TelAsc2BCD(msghead->lclTel, &status, tbuf, U8Temp);//±¾»úºÅ<=12
	pbuf+=sizeof(MSG_HEAD);
	*Senddatalen+=sizeof(MSG_HEAD);
	//ÏûÏ¢°ü·â×°Ïî
	if (totalPackt>1){
		MSG_HEAD_sub *headsub=(MSG_HEAD_sub*)pbuf;
		short_to_char(headsub->totalpacketNum, totalPackt);
		short_to_char(headsub->SubpacketNo, SubPackt);
		pbuf+=sizeof(MSG_HEAD_sub);
		*Senddatalen+=sizeof(MSG_HEAD_sub);
	}

	//´´½¨ÏûÏ¢Ìå!!!
	status = BuildMsgbody(DevMsgId,pbuf, &U16Temp,totalPackt, SubPackt);
	if (status){
		DEBUG("BuildMsgbody error, status=%d!", status);
		return 1;
	}
	//¸³ÖµÏûÏ¢ÌåÊôÐÔµÄ³¤¶È
	msgbody_attr.MsgbodyLen = U16Temp;	//¸øÏûÏ¢ÌåÊôÐÔ¸³ÖµÏûÏ¢Ìå³¤¶È
	short_to_char(Sendbuf+3, *(u16*)&msgbody_attr);
	pbuf+=U16Temp;
	*Senddatalen+=U16Temp;
//#ifdef LBS_RSA
#if 0
	if(msgbody_attr.DataEncrypt1)
	{	//¸øÏûÏ¢Ìå¼ÓÃÜ
		u16 msgbodylen_code;
		Encode(pstr+len, &msgbodylen_code, &msgbody, msgbodylen);//¼ÓÃÜºóµÄÊý¾Ý±£´æÔÚpstr+lenÖÐ
		U16Temp = msgbodylen_code;
	}
	else
	{
		Mem_Copy(pstr+*Senddatalen, msgbody, msgbodylen);
	}
#endif
	//Ð£Ñé
	*pbuf++=0;
	*Senddatalen += 1;
	//±êÊ¶Î²
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
*Description:    ±¨¾¯·¢ÉúÊ±´¦Àí£¬°üº¬ÅÐ¶Ï±¨¾¯ÆÁ±Î×Ö£¬
txt±¨¾¯£¬±¨¾¯Ê±ÅÄÕÕ¼°ÕÕÆ¬ÉÏ´«»ò½ö´æ´¢,¹Ø¼ü±¨¾¯
*Calls:          ÎÞ
*Called By:      ÎÞ
*Input:          ALARM_DEF ¶ÔÓ¦Î»ÖÃÐÅÏ¢»ã±¨µÄ±¨¾¯±êÖ¾Î»ºê¶¨Òå
*Output:          
*Return:         ReturnTypeÀàÐÍ ParaError ²ÎÊý´íÎó 
*				 AcktionError ±¨¾¯ÆÁ±Î£¬AcktionOK ±¨¾¯
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
	if(flag&ALARM_DEF)//±¨¾¯ÆÁ±Î
	{
		return 2;
	}
	else
	{
		WriteAlarmPara(SET,StaAlarm0,ALARM_DEF);
		DevReq2ServPackag_build(REPORT_LOCATION); //¸æ¾¯²úÉúÒ»°üÎ»ÖÃ»ã±¨
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
				DEBUG("ALARMtype:%dÖÕ¶ËÖ÷µçÔ´Ç·Ñ¹");
			break;
			case ALARM_POWER_BROWN:
				DEBUG("ALARMtype:%dÖÕ¶ËÖ÷µçÔ´µôµç");
			break;
			case ALARM_DISPLAY_ERR:
				DEBUG("ALARMtype:%dLCD»òÏÔÊ¾ÆÁ¹ÊÕÏ");
			break;
			case ALARM_TTS_ERR:
				DEBUG("ALARMtype:%dTTS¸æ¾¯");
			break;
			case ALARM_CAMERA_ERR:
				DEBUG("ALARMtype:%dÉãÏñÍ·¹ÊÕÏ");
			break;
			case ALARM_DRIVE_OVERTIME:
				DEBUG("ALARMtype:%dµ±ÌìÀÛ¼Æ¼ÝÊ»³¬Ê±");
			break;
			case ALARM_OVERTIME_PARKING:
				DEBUG("ALARMtype:%d³¬Ê±Í£³µ");
			break;
			case ALARM_ENTER_AREA:
				DEBUG("ALARMtype:%d½ø³öÇøÓò±¨¾¯");
			break;
			case ALARM_TOUCH_LINE_k:
				DEBUG("ALARMtype:%d½ø³öÂ·Ïß±¨¾¯!\r\n");
			break;
			case ALARM_DRIVE_SHORT_LONG_k:
				DEBUG("ALARMtype:%dÂ·¶ÎÐÐÊ»Ê±¼ä²»×ã»ò¹ý³¤!\r\n");
			break;
			case ALARM_DRIVE_DEVIATE:
				DEBUG("ALARMtype:%dÂ·ÏßÆ«Àë±¨¾¯!\r\n");
			break;
			case ALARM_VSS_ERR:
				DEBUG("ALARMtype:%d³µÁ¾VSS±¨¾¯!\r\n");
			break;
			case ALARM_OIL_ERR:
				DEBUG("ALARMtype:%dÓÍÁ¿Òì³£±¨¾¯!\r\n");
			break;
			case ALARM_STEAL:
				DEBUG("ALARMtype:%d³µÁ¾±»µÁ!\r\n");
			break;
			case ALARM_START_ILLEGAL_k:
				DEBUG("ALARMtype:%d³µÁ¾·Ç·¨µã»ð!\r\n");
			break;
			case ALARM_MOVE_ILLEGAL_k:
				DEBUG("ALARMtype:%d³µÁ¾·Ç·¨Î»ÒÆ!\r\n");
			break;
			case ALARM_ROLLOVER:
				DEBUG("ALARMtype:%d³µÁ¾²à·­±¨¾¯!\r\n");
			break;
			default:
				DEBUG("ALARMtype:%d³µÁ¾±¨¾¯!\r\n");
			break;
		}
		DEBUG("!\r\n");
		#endif

		
		ReadLbsCfgPara(etxtSMSAlarm,(u8 *)&flag,&paralen);
		if(flag&ALARM_DEF)//ÎÄ±¾SMS±¨¾¯
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
					Str_Copy(pSMS,"ALARMtype:%dÔ¤¾¯");
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
					Str_Copy(pSMS,"display screen error alarm!"); //µ÷¶ÈÆÁ¼°ÊÖ±ú
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
					Str_Copy(pSMS,"Alarm for abnormaol amount of oil.");	//ÓÍÎ»´«¸ÐÆ÷
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
			//WCM_SendSMS(ASC_SMS,Tel,paralen,pSMS,strlen(pSMS));zq ×¢ÊÍµôÁË·¢¶ÌÐÅ
		}
		#if 0
		ReadLbsCfgPara(eAlarmPhoto,(u8 *)&flag,&paralen);
		if(flag&ALARM_DEF)//±¨¾¯ÅÄÕÕÔÊÐí									-
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
			Write_ProtclHandl(eMediaInfo, (u8 *)&mediainfo, sizeof(LBS_Mediainfo)); //ÉÏ´«Ê±ÓÃ

			//±¨¾¯Ê±ÔÝ¶¨Ê¹ÓÃÏÂÁÐ²ÎÊý×÷ÎªÄ¬ÈÏÅÄÕÕ²ÎÊý
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
			if(flag&ALARM_DEF)//ÅÄÕÕ±£´æ²»·¢ËÍ
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
		if(flag&ALARM_DEF) //¹Ø¼ü±¨¾¯
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
#if 0
/*----------------------------Î»ÖÃÐÅÏ¢¶ÓÁÐ²Ù×÷------------------------------*/
/*È¡posbufµÄ¿ÕÒ»Î¬Êý×éµÄµØÖ·*/
u8 *getEmptybuf()
{
	u8 i=0;
	u8 j;
	for(i=0;i<UPDATA_BUFNUM;i++)
	{
		if(Queue_ServUpdatabufindex[i]==0)
		{
			Queue_ServUpdatabufindex[i]=1;
			j=i;
			break; //return (u8 *)(posbuf[i]);
		}
	}
	if(i<UPDATA_BUFNUM)
	{
//		DEBUG(PrintDebug,"position data index£º\r\n");	
//		for(i=0;i<POSQUEUE_BUFNUM;i++)
//			DEBUG(PrintDebug,"%d\r\n",posbufindex[i]);	
		return (u8 *)(ServUpdatabuf[j]);
	}
	else
		return (u8 *)0;
}
#endif
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
	//¸ø°ü¼ÓÉÏÁ÷Ë®ºÅ,Á÷Ë®ºÅµÚ¼Ó
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
	//Ð£Ñé£¬³ý±êÊ¾Í·Î²ºÍÐ£Ñé±¾Éí
	if (1 == XOR_Check(data_buf+1, len-3,(data_buf+len-2)))
	{
		OA_DEBUG_USER(" XOR err");
		return 0;
	}

	//¶Ô±êÊ¾Í·ºÍÎ²Ö®ÍâµÄ°üÊý¾Ý½øÐÐ×ªÒå
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
u16 DevReq2ServPackag_build(u16 ReqMsgId) //¼´Ê±ÉÏ´«Êý¾Ý
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
				if (ReqMsgId == REPORT_LOCATION){
					Write_ProtclHandl(eDevMsgid, (u8 *)&ReqMsgId, 2);//ÖÕ¶Ë·¢ËÍÏûÏ¢ID by zhuqing @2013/6/26
					timeout_var.timeout_en = OA_TRUE;
					timeout_var.do_timeout = OA_FALSE;
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
//Ã¤ÇøÊý¾ÝÔÝÊ±²»¹Ü
//#ifdef BLINDDATA
#if 0
	else if(REPORT_LOCATION==ReqMsgId) //Î»ÖÃÊý¾Ý´æflash
	{
		u8 err;
		u32 Temp;
		ReadAllBitAlarmPara(StaSector1,&Temp); //peijl_20121018 Ìí¼Ó£ºÖ»gps¶¨Î»Ê±£¬Î»ÖÃÊý¾Ý´æÃ¤Çø
		if(Temp&STA_GPS_FIXED)
		{
		DEBUG(PrintDebug,"DevReq2Serv Queue is full.So save position-data to Blind-position flash.\r\n");
		BlinddataBuild_save2Flash(REPORT_LOCATION);
		}
		return ActionOK;
	}
	else if(STADENTDATA_REPPORT==ReqMsgId) //Î»ÖÃÊý¾Ý´æflash
	{
		u8 err;
		u32 Temp;
		DEBUG(PrintDebug,"DevReq2Serv Queue is full.So save student-data to flash of Blind area.\r\n");
		BlinddataBuild_save2Flash(STADENTDATA_REPPORT);
		return ActionOK;
	}
	else //ÆäËûÊý¾Ý¶ªÆú
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
u16 DevReq2ServPackag_build_blind(u16 ReqMsgId) //¼´Ê±ÉÏ´«Êý¾Ý
{
	u8 pbuf[DATA_MAX_LEN] = 0;
	u16 U16Temp = 0;
	oa_bool ret = 0;
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
