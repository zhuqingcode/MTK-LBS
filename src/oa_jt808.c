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
#include "oa_gps.h"
#include "oa_platform.h"
#include "oa_blinddata.h"
//#include "oa_sms.h"
#define PRINT_SAMEPARA	Trace("(%s:%s:%d): this parameter is same as the original, so I do nothing...", __FILE__, __func__, __LINE__)
#define PORT_MAX 65535
extern DEVICE_PARAMS dev_now_params;
extern oa_soc_context g_soc_context;
oa_bool need_reconn = OA_FALSE;
dev_control_type control_type = none;

oa_uint16 serial_num = 0;//������ˮ��
static u32 AlarmFlag[StatusNum]={0};

//#define UPDATA_BUFNUM		12 //�ϴ����ݶ��еĻ���������
//#define UPDATA_BUFLEN_MAX	200
//#define BUFFER_MAX_LEN 198
//#define BUFFER_MAX_LEN DATA_MAX_LEN
#if 0
u8 Queue_ServUpdatabufindex[UPDATA_BUFNUM]; //posbufд����б�ʾ��0Ϊռ�ã�1ʹ����
u8 ServUpdatabuf[UPDATA_BUFNUM][UPDATA_BUFLEN_MAX];
#endif
extern DEV_PLAT_PARAS dev_running;
ProtocolHandle sProtclHandl = {0};

u16 escape_copy_to_send(u8 *buf, u16 len);
u16 DevReq2ServPackag_build(u16 ReqMsgId);
u8 JT808Msg_Build(u16 DevMsgId,u16 totalPackt,u16 SubPackt,u8 *Sendbuf,u16 Sendbuflen,u16 *Senddatalen);
#if 0
/*********************************************************
*Function:      term_reg_enpacket()
*Description:  enpacket the terminal register message
*Return:        oa_bool
*Others:         
*********************************************************/
oa_bool term_reg_enpacket(oa_uint8 *buf, oa_uint16 *p_len)
{
	if (NULL == buf || NULL == p_len)
	{
		OA_DEBUG_USER("err:(%s:%s:%d):", __FILE__, __func__,__LINE__);
		return OA_FALSE;
	}
	*p_len = 0;
	//province id
	short_to_2char(buf, dev_now_params.vehicle_province_id);
	buf += sizeof(dev_now_params.vehicle_province_id);
	*p_len += 2;
	//city id
	short_to_2char(buf, dev_now_params.vehicle_city_id);
	buf += sizeof(dev_now_params.vehicle_city_id);
	*p_len += 2;
	//manufacturers id
	oa_memcpy(buf, dev_now_params.manufacturers_id, MAN_ID_MAX_LEN);
	buf += MAN_ID_MAX_LEN;
	*p_len += MAN_ID_MAX_LEN;
	//terminal model
	oa_memcpy(buf, dev_now_params.term_model, TERM_MODEL_MAX_LEN);
	buf += TERM_MODEL_MAX_LEN;
	*p_len += TERM_MODEL_MAX_LEN;
	//terminal id
	oa_memcpy(buf, dev_now_params.term_id, TERM_ID_MAX_LEN);
	buf += TERM_ID_MAX_LEN;
	*p_len += TERM_ID_MAX_LEN;
	//plate colar
	oa_memcpy(buf, &dev_now_params.plate_color, sizeof(dev_now_params.plate_color));
	buf += sizeof(dev_now_params.plate_color);
	*p_len += sizeof(dev_now_params.plate_color);
	//vehicle lisence
	oa_memcpy(buf, dev_now_params.vehicle_license, oa_strlen(dev_now_params.vehicle_license));
	buf += oa_strlen(dev_now_params.vehicle_license);
	*p_len += oa_strlen(dev_now_params.vehicle_license);
	return OA_TRUE;
}
/*********************************************************
*Function:      fill_reg_packet()
*Description:  enpacket the terminal register message
*Return:        oa_bool
*Others:         
*********************************************************/
oa_bool msg_head_enpacket(oa_uint8 *buf, oa_uint16 body_len)
{
	if (NULL == buf)
	{
		return OA_FALSE;
	}
	//---------------��Ϣͷ----------------
	//��ʾλ
	buf[0] = JT808_FLAG;
	//��Ϣid
	buf[1] = TERM_REG;
	//��Ϣ������
	short_to_2char(&buf[2], body_len & 0x03ff);
	//�ն��ֻ���
	oa_memcpy(&buf[4], dev_now_params.term_tel_num, sizeof(dev_now_params.term_tel_num));
	//��Ϣ��ˮ��
	short_to_2char(&buf[10], serial_num);
	//-----------------------------------------	
	
}
/*********************************************************
*Function:      fill_reg_packet()
*Description:  enpacket the terminal register message
*Return:        oa_bool
*Others:         
*********************************************************/
oa_bool send_buffer_enpacket(void)
{
	
	
}
#endif
//=====================================��LBS����ֲ======================================
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
		case eDevSeqid: //�ն˷�����ˮ��
			*pbuflen=sizeof(sProtclHandl.DevSeqId);
			Mem_Copy(pbuf,sProtclHandl.DevSeqId,sizeof(sProtclHandl.DevSeqId));
			{
				u16 id;
				char_to_short(sProtclHandl.DevSeqId,&id);
				if(id==0xffff)
					id=0;
				else
					id++;
				short_to_char(sProtclHandl.DevSeqId,id);
			}
		break;
		case eServSeqid: //��ˮ��
			*pbuflen=sizeof(sProtclHandl.ServSeqId);
			Mem_Copy(pbuf,sProtclHandl.ServSeqId,sizeof(sProtclHandl.ServSeqId));
		break;
		case eServMsgid: //ƽ̨������Ϣid
			*pbuflen=sizeof(sProtclHandl.ServMsgId);
			Mem_Copy(pbuf,sProtclHandl.ServMsgId,sizeof(sProtclHandl.ServMsgId));
		break;
		case eRsp2ServSeq: //ƽ̨����Ӧ����
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
		case eMediaInfo:  //��ý����Ϣ�ϴ�
			*pbuflen=sizeof(sProtclHandl.MediaEventinfo);
			Mem_Copy(pbuf,&sProtclHandl.MediaEventinfo,*pbuflen);
		break;
		case eSpclServPara:	//���ƽ̨
			*pbuflen=sizeof(sProtclHandl.spclServ);
			Mem_Copy(pbuf,&sProtclHandl.spclServ,*pbuflen);
		break;
		case eRecordPara: //¼������
			*pbuflen=sizeof(sProtclHandl.StartRecordreq);
			Mem_Copy(pbuf,&sProtclHandl.StartRecordreq,*pbuflen);
		break;
#endif
		case eRsp2DevReq: //���ƽ̨Ӧ��
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
		case e_ReportMediaDatarsp: //У���ý�������ϴ�Ӧ��
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
		case e_SetEventreq_EventType:	//�¼�����//��������
		{
			*pbuflen = 1;
			Mem_Copy(pbuf, &sProtclHandl.SetEventreq.EventType, *pbuflen);
			break;
		}
		case e_SetEventreq_totalNum:	//�¼�����//��������
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
		case e_SetPhonebookreq_BookType:	//���õ绰��//����
		{
			*pbuflen = 1;
			Mem_Copy(pbuf, &sProtclHandl.SetPhonebookreq.BookType, *pbuflen);
			break;
		}
		case e_SetPhonebookreq_ContackNum:	//���õ绰��//����
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
		case e_TakePhotoreq: //���ղ���
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
			*pbuflen = 12; //�̶�����for 808 test
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
		//	if(buflen-50>sizeof(sProtclHandl.stCallcarOrder.OrderContent)) //���ݹ���δ���� ����
		//		return 1;
			Mem_Copy(sProtclHandl.stCallcarOrder.OrderID,pbuf,50);
			if(buflen-50>sizeof(sProtclHandl.stCallcarOrder.OrderContent)) //���ݹ���δ���� ����
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
		//	if(buflen-50-20>sizeof(sProtclHandl.stCallcarOrder.OrderContent)) //���ݹ���δ���� ����
		//		return 1;
			Mem_Copy(sProtclHandl.stCallcarOrder.OrderID,pbuf,50);
			Mem_Copy(sProtclHandl.stCallcarOrder.OrderTel,pbuf+50,20);
			if(buflen-50-20>sizeof(sProtclHandl.stCallcarOrder.OrderContent)) //���ݹ���δ���� ����
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
		//����
		case eDevSeqid: //�ն˷�����ˮ��
			Mem_Copy(sProtclHandl.DevSeqId,pbuf,sizeof(sProtclHandl.DevSeqId));
		break;
		case eServSeqid: //ƽ̨����
			Mem_Copy(sProtclHandl.ServSeqId,pbuf,buflen);
		break;
		case eServMsgid: //ƽ̨����
			Mem_Copy(sProtclHandl.ServMsgId,pbuf,buflen);
		break;
		case eRsp2ServSeq: //ƽ̨����Ӧ����
			if(buflen!=1)
				return 1;
			sProtclHandl.Rsp2Serv=*pbuf;
		break;
#if 0 //zq comment for mtk-lbs desn't support
		case eMediaInfo: //��ý���ϱ�
			Mem_Copy(&sProtclHandl.MediaEventinfo,pbuf,buflen);
		break;
#endif
		//����
		case eRsp2DevReq: //���ն�����Ӧ��
			Mem_Copy(&sProtclHandl.PlatComrsp,pbuf,buflen);
		break;
		case eSpclServPara:	//���ƽ̨
			Mem_Copy(&sProtclHandl.spclServ,pbuf,buflen);
		break;
#if 0 //zq comment for mtk-lbs desn't support
		case eRecordPara: //¼������
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
		case e_ReportMediaDatarsp:				//У���ý�������ϴ�Ӧ��
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
		case e_SetEventreq_EventType:	//�¼�����//��������
		{
			Mem_Copy(&sProtclHandl.SetEventreq.EventType, pbuf, buflen);
			break;
		}
		case e_SetEventreq_totalNum:	//�¼�����//��������
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
		case e_SetPhonebookreq_BookType:	//���õ绰��//����
		{
			sProtclHandl.SetPhonebookreq.BookType = *pbuf;
			break;
		}
		case e_SetPhonebookreq_ContackNum:	//���õ绰��//����
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
*Description:   808Э��ת�� 
*				0x7e <-> 0x7d+0x02,0x7d<->0x7d+0x01
*Calls:          
*Called By:      
*Input:			flag ��ʾ1:����ת�� 0:����ת��
*				pstr Ϊ��Ҫ��ת��������׵�ַ ��ָ��
*				strlen ת��ǰ���ݳ���
*Output:        pstr ��Ϊ�����ַ
*				*Destlen ת������ݳ���
*Return:        �����Ƿ�ִ�гɹ�������0��ʾ�ɹ�
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
		Trace("turning parameter error!pstr=%x,stringlen=%d",pstr,strlen);
		return 1;
	}

	if(flag)
	{
	//flag==1ʱ���ǽ�0x7e -> 0x7d+0x02, 	0x7d->0x7d+0x01
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
	//flag==0ʱ�ǽ�0x7d+0x02->0x7e, 	0x7d+0x01->0x7d
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
/*--------------------------------------------------------------------------------------
*Function:       WriteLbsCfgPara()
*Description:	��ȫ�ֱ���LbsCfgStruct
*				UpdateOnlyֻ����ram��LbsCfgStruct
*				UpdateFlash ����ramͬʱд��flash
*				дflash�������ڲ������õ�ͬʱдflash��Ҳ������CfgParaΪeCfaParaMaxʱֱ��дflash
*input:			CfgPara ������Ӧ��JTT808Э��ID
*				pValueҪд���ֵ��lenд�볤��
*				UpdateMode UpdateOnlyֻ����ramȫ�ֱ�����UpdateFlashͬʱдflash				
*Return:		ActionOK���ɹ���ParaError��������
*Others:		
---------------------------------------------------------------------------------------*/
//�ж�len�����ַ����Ƿ�ΪIP IP��ʽxxx.xxx.xxx.xxx peijl_20120828 �޸�
u8 ISAscIPValid(u8 *IP,u8 len){
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
//�ж�len�����ַ����Ƿ�Ϊ�ֻ��� ��ʽ0~9�ַ�������+��+��0~9���ַ��� peijl_20120829 ����
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
	return OA_FALSE;
}
u8 WriteLbsCfgPara(Enum_CtrlCfgPara CfgPara, u8 *pValue,u8 len,UpdateModeEnum UpdateMode)
{
	u8 err;
	//oa_bool need_reconn = OA_FALSE;
	#if 0
	if((CfgPara==eCfaParaMax)&&(UpdateMode==UpdateFlash))
	{//δ���в������ã������ݲ���
		OSSemPend(HXCfgParaSem,0,&err);
		Flash_Write_Buffer(NULL,0,SERV_CTRLCFG_ADDR,Sector_Erase);	//��������
		Flash_Write_Buffer((u8 *)&LbsCfgStruct,sizeof(STRUCT_LBS_CFG),SERV_CTRLCFG_ADDR,Write_Mode);	//д������	
		OSSemPost(HXCfgParaSem);
		return 0;		
	}
	#endif
	//������Ч���ж�
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
		case eLclTEL: //808Э�����12�ֽ� peijl_130311 ���ӱ����Ŵ�����Ĭ��ֵ
			if(len>12) //808Э�����12�ֽ� peijl_130311 ���ӱ����Ŵ�����Ĭ��ֵ
				return 1;
		break;
		case eServTel:
		case eResetTel:
		case eFactryTel:
		case eSMSTel:
		case eAlarmSMSTel:
		case eMonitTel:
		case eSpeclSMSTel:
			if(len>TELMAXLEN)
				return 1;
		break;
		case eCarid: 
			if(len>CarIDMAXLEN)
				return 1;
		break;
		case eIP:
		case eIP_sub:
			if(!ISAscIPValid(pValue,len)){
				Trace("(%s:%s:%d):here", __FILE__, __func__, __LINE__);
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
//		case eRptStrategy:
//			if((len!=1)||
//			(*pValue!=0&&*pValue!=1&&*pValue!=2))
//				return 1;
//		break;
//		case eRptType:
//			if((len!=1)||
//			(*pValue!=0&&*pValue!=1))
//				return 1;
//		break;
//		case eRptCog:
//			if((len!=4)
//			||(*pValue>180)||(*(pValue+1)!=0)||(*(pValue+2)!=0)||(*(pValue+3)!=0)) //>108
//				return 1;
//		break;

/*	
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
		case eOdometer: //��̱����� 
		break;
		case eCarclor:
		break;
*/
		case ePrivic:
		case eCity:
			if(len!=2)
				return 1;
		break;
		default:
		break;
	}
	//��������
	//OSSemPend(HXCfgParaSem,0,&err);
if (UpdateMode == UpdateOnly){
	Trace("(%s:%s:%d):parameters id:0x%X", __FILE__, __func__, __LINE__, CfgPara);
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
		case eUdpPort:{ //Э�鲻�����˿ں��ݶ�����ͬ
			//Mem_Copy((u8 *)&LbsCfgStruct.ServPort_udp,pValue,len);
			//Mem_Copy((u8 *)&LbsCfgStruct.SubservPort_udp,pValue,len);
			u32 tmp;
			Trace("(%s:%s:%d):set UdpPort", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&tmp, pValue, len);
				if (tmp <= PORT_MAX){
					if (tmp == dev_now_params.server_udp_port){
						PRINT_SAMEPARA;
						break;
					}
					else{
						dev_now_params.server_udp_port = tmp;
						need_reconn = OA_TRUE;
					}
				}
				else{
					Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
				}
				//oa_memcpy((u8 *)&dev_now_params.server_udp_port, pValue, len);
				//need_reconn = OA_TRUE;
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		}
		break;
		case eTcpPort:{//do not forget reconnect !!!
			//Mem_Copy((u8 *)&LbsCfgStruct.ServPort_tcp,pValue,len);
			//Mem_Copy((u8 *)&LbsCfgStruct.SubservPort_tcp,pValue,len);
			u32 tmp;
			Trace("(%s:%s:%d):set TcpPort", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&tmp, pValue, len);
				if (tmp <= PORT_MAX){
					if (tmp == dev_now_params.server_tcp_port){
						PRINT_SAMEPARA;
						break;
					}
					else{
						dev_now_params.server_tcp_port = tmp;
						need_reconn = OA_TRUE;
					}
				}
				else{
					Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
				}
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
			#if 0
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.server_tcp_port, pValue, len);
				need_reconn = OA_TRUE;
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
			#endif
			}
		break;
		case eIP:{
			//LbsCfgStruct.ServIPLen=len;
			//Mem_Copy(LbsCfgStruct.ServIP,pValue,len);
			//LbsCfgStruct.ServIP[len]='\0';
			u8 ip_tmp[SERVER_IP_MAX_LEN] = {0x0};
			Trace("(%s:%s:%d):set ip", __FILE__, __func__, __LINE__);
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
						need_reconn = OA_TRUE;
					}
				}
				else{//not equal
					oa_memset(dev_now_params.m_server_ip, 0x0, sizeof(dev_now_params.m_server_ip));
					oa_memcpy(dev_now_params.m_server_ip, ip_tmp, len);
					need_reconn = OA_TRUE;

				}
				#if 0
				oa_memset(&dev_now_params.m_server_ip, 0x0, sizeof(dev_now_params.m_server_ip));
				oa_memcpy((u8 *)&dev_now_params.m_server_ip, pValue, len);
				need_reconn = OA_TRUE;
				#endif
			}
			else{
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
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
			Trace("(%s:%s:%d):set LclTEL", __FILE__, __func__, __LINE__);
			if (len <= TERM_TEL_NUM_MAX_LEN){
				oa_memset(&dev_now_params.term_tel_num, 0x0, sizeof(dev_now_params.term_tel_num));
				oa_memcpy((u8 *)&dev_now_params.term_tel_num, pValue, len);
			}
			else{
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		}
		break;
		case eHtTime:
			//Mem_Copy((u8 *)&LbsCfgStruct.HB_time,pValue,len);	//	comment for 808 test
			Trace("(%s:%s:%d):set hearttime", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.heartbeat_interval, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eTcptime:
			//Mem_Copy((u8 *)&LbsCfgStruct.rsp_time_tcp,pValue,len);
			Trace("(%s:%s:%d):Tcptime", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.tcp_ack_timeout, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eTcpnum:
			//Mem_Copy((u8 *)&LbsCfgStruct.rsp_num_tcp,pValue,len);
			Trace("(%s:%s:%d):Tcpnum", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.tcp_retrans_times, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
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
			Trace("(%s:%s:%d):SMSTime", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.sms_ack_timeout, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eSMSNum:
			//Mem_Copy((u8 *)&LbsCfgStruct.rsp_num_sms,pValue,len);
			Trace("(%s:%s:%d):SMSNum", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.sms_retrans_times, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eRptStrategy:{
			//Mem_Copy((u8 *)&LbsCfgStruct.ReportStrategy,pValue,len);
			u32 tmp;
			Trace("(%s:%s:%d):RptStrategy", __FILE__, __func__, __LINE__);

			if (len == 4){
				oa_memcpy((u8 *)&tmp, pValue, len);
				if (tmp >= 0 && tmp < 3)	oa_memcpy((u8 *)&dev_now_params.report_strategy, pValue, len);
				
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		}
		break;
		case eRptType:{
			//Mem_Copy((u8 *)&LbsCfgStruct.ReportType,pValue,len);
			u32 tmp;
			Trace("(%s:%s:%d):RptType", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&tmp, pValue, len);
				if (tmp == 0 || tmp == 1)	oa_memcpy((u8 *)&dev_now_params.report_type, pValue, len);
				
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		}
		break;
		case eRpttime_def:
			//Mem_Copy((u8 *)&LbsCfgStruct.DefaultReportTime,pValue,len);
			Trace("(%s:%s:%d):Rpttime_def", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.default_reporttime, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eRpttime_unlog:
			//Mem_Copy((u8 *)&LbsCfgStruct.unLoginReportTime,pValue,len);
			Trace("(%s:%s:%d):Rpttime_unlog", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.unlogin_reporttime, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eRpttime_sleep:
			//Mem_Copy((u8 *)&LbsCfgStruct.sleepReportTime,pValue,len);
			Trace("(%s:%s:%d):Rpttime_sleep", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.sleep_reporttime, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eRpttime_emergency:
			//Mem_Copy((u8 *)&LbsCfgStruct.UrgentReportTime,pValue,len);
			Trace("(%s:%s:%d):Rpttime_emergency", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.urgent_reporttime, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eRptDistanc_def:	
			//Mem_Copy((u8 *)&LbsCfgStruct.DefaultReportDistanc,pValue,len);
			Trace("(%s:%s:%d):RptDistanc_def", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.default_reportdistance, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eRptDistanc_sleep:
			//Mem_Copy((u8 *)&LbsCfgStruct.sleepReportDistanc,pValue,len);	
			Trace("(%s:%s:%d):RptDistanc_sleep", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.sleep_reportdistance, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eRptDistanc_unlog:
			//Mem_Copy((u8 *)&LbsCfgStruct.unLoginReportDistanc,pValue,len);
			Trace("(%s:%s:%d):RptDistanc_unlog", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.unlogin_reportdistance, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eRptDistanc_emergency:
			//Mem_Copy((u8 *)&LbsCfgStruct.UrgentReportDistanc,pValue,len);
			Trace("(%s:%s:%d):RptDistanc_emergency", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.urgent_reportdistance, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eRptCog:
			//Mem_Copy((u8 *)&LbsCfgStruct.ReportCog,pValue,len);
			Trace("(%s:%s:%d):RptCog", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.corner_reportangle, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eServTel:
			//LbsCfgStruct.ServTellen=len;
			//Mem_Copy(LbsCfgStruct.ServTel,pValue,len);
			Trace("(%s:%s:%d):ServTel", __FILE__, __func__, __LINE__);
			if (len <= TEL_NUM_MAX_LEN)
			{
				oa_memset(&dev_now_params.monitor_platform_num, 0x0, sizeof(dev_now_params.monitor_platform_num));
				oa_memcpy((u8 *)&dev_now_params.monitor_platform_num, pValue, len);
			}
			else
			{
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eResetTel:
			//LbsCfgStruct.ResetTellen=len;
			//Mem_Copy(LbsCfgStruct.ResetTel,pValue,len);
			Trace("(%s:%s:%d):ResetTel", __FILE__, __func__, __LINE__);
			if (len <= TEL_NUM_MAX_LEN){
				oa_memset(&dev_now_params.reset_num, 0x0, 	sizeof(dev_now_params.reset_num));
				oa_memcpy((u8 *)&dev_now_params.reset_num, pValue, len);
			}
			else{
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eFactryTel:
			//LbsCfgStruct.FactySetTellen=len;
			//Mem_Copy(LbsCfgStruct.FactySetTel,pValue,len);
			Trace("(%s:%s:%d):FactryTel", __FILE__, __func__, __LINE__);
			if (len <= TEL_NUM_MAX_LEN){
				oa_memset(&dev_now_params.restore_factory_settings_num, 
																	0x0, 
						sizeof(dev_now_params.restore_factory_settings_num));
				oa_memcpy((u8 *)&dev_now_params.restore_factory_settings_num, pValue, len);
			}
			else{
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eSMSTel:
			//LbsCfgStruct.ServSMSTellen=len;
			//Mem_Copy(LbsCfgStruct.ServSMSTel,pValue,len);
			Trace("(%s:%s:%d):SMSTel", __FILE__, __func__, __LINE__);
			if (len <= TEL_NUM_MAX_LEN)
			{
				oa_memset(&dev_now_params.monitor_platform_sms_num, 0x0, sizeof(dev_now_params.monitor_platform_sms_num));
				oa_memcpy((u8 *)&dev_now_params.monitor_platform_sms_num, pValue, len);
			}
			else
			{
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eAlarmSMSTel:
			//LbsCfgStruct.AlarmSMSTellen=len;
			//Mem_Copy(LbsCfgStruct.AlarmSMSTel,pValue,len);
			Trace("(%s:%s:%d):AlarmSMSTel", __FILE__, __func__, __LINE__);
			if (len <= TEL_NUM_MAX_LEN){
				oa_memset(&dev_now_params.terminal_sms_num, 0x0, 	sizeof(dev_now_params.terminal_sms_num));
				oa_memcpy((u8 *)&dev_now_params.terminal_sms_num, pValue, len);
			}
			else{
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
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
			Trace("(%s:%s:%d):Alarmmaskword", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.alarm_mask, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case etxtSMSAlarm:
			//Mem_Copy((u8 *)&LbsCfgStruct.AlarmSMStextFlag,pValue,len);
			Trace("(%s:%s:%d):txtSMSAlarm", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.alarm_send_sms_mask, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
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
			Trace("(%s:%s:%d):Overspeed", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.max_speed, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eOverspeedtime:
			//Mem_Copy((u8 *)&LbsCfgStruct.OverSpeedTime,pValue,len);
			Trace("(%s:%s:%d):Overspeedtime", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.speed_duration, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eDrivTime:
			//Mem_Copy((u8 *)&LbsCfgStruct.MaxDriveTime,pValue,len);
			Trace("(%s:%s:%d):DrivTime", __FILE__, __func__, __LINE__);
			if (len == 4){
				
				oa_memcpy((u8 *)&dev_now_params.continuous_drive_time_threshold, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eDrivTime_day:
			//Mem_Copy((u8 *)&LbsCfgStruct.MaxDriveTime_day,pValue,len);
			Trace("(%s:%s:%d):DrivTime_day", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.day_add_drive_time_threshold, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eRestTime_min:
			//Mem_Copy((u8 *)&LbsCfgStruct.MinRestTime,pValue,len);
			Trace("(%s:%s:%d):RestTime_min", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.min_rest_time, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eStopcarTime_max:
			#if 0//???
			Mem_Copy((u8 *)&LbsCfgStruct.MaxRestTime,pValue,len);		
			if(ReadAlarmPara(StaAlarm0,ALARM_OVERTIME_PARKING)==SET)
			{ //ֵ����������ж�״̬
				WriteAlarmPara(RESET,StaAlarm0,ALARM_OVERTIME_PARKING);
			}
			#endif
			Trace("(%s:%s:%d):StopcarTime_max", __FILE__, __func__, __LINE__);
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.max_park_time, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
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
		case eOdometer: //��̱�����
			Trace("(%s:%s:%d):Odometer", __FILE__, __func__, __LINE__);
			//Mem_Copy((u8 *)&LbsCfgStruct.Odometer,pValue,len);		
			if (len == 4){
				oa_memcpy((u8 *)&dev_now_params.vehicle_odometer, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case ePrivic:
			#if 0
			//char_to_short(pValue,&U16Temp);
			Mem_Copy((u8 *)&LbsCfgStruct.ProvinceId,pValue,len);	
			#endif
			Trace("(%s:%s:%d):Privic", __FILE__, __func__, __LINE__);
			if (len == 2){
				oa_memcpy((u8 *)&dev_now_params.vehicle_province_id, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eCity:
			//Mem_Copy((u8 *)&LbsCfgStruct.CityId,pValue,len);
			Trace("(%s:%s:%d):City", __FILE__, __func__, __LINE__);
			if (len == 2){
				oa_memcpy((u8 *)&dev_now_params.vehicle_city_id, pValue, len);
			}
			else {
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eCarid:
			//LbsCfgStruct.CarIdlen=len;
			//Mem_Copy(LbsCfgStruct.CarId,pValue,len);
			Trace("(%s:%s:%d):Carid", __FILE__, __func__, __LINE__);
			if (len <= VEHICLE_LICENCE_MAX_LEN){
				oa_memset(&dev_now_params.vehicle_license, 0x0, 	sizeof(dev_now_params.vehicle_license));
				oa_memcpy((u8 *)&dev_now_params.vehicle_license, pValue, len);
			}
			else{
				Trace("param err (%s:%s:%d):", __FILE__, __func__, __LINE__);
			}
		break;
		case eCarclor:
			Trace("(%s:%s:%d):Carclor", __FILE__, __func__, __LINE__);
			//LbsCfgStruct.CarIdColor=*pValue;
			dev_now_params.plate_color = *pValue;
		break;
		default:{Trace("(%s:%s:%d):not support!", __FILE__, __func__, __LINE__);}
		break;
	}
}
	if(UpdateMode == UpdateFlash)  			/*��Ҫ���µ�flash*/
	{
		#if 0
		Flash_Write_Buffer(NULL,0,SERV_CTRLCFG_ADDR,Sector_Erase);	//��������
		Flash_Write_Buffer((u8 *)&LbsCfgStruct,sizeof(STRUCT_LBS_CFG),SERV_CTRLCFG_ADDR,Write_Mode);	//д������	
		#endif
		oa_bool ret;
		//Trace("(%s:%s:%d):traceing!", __FILE__, __func__, __LINE__);
		ret = dev_params_save();
		if (ret == OA_TRUE){
			print_key_dev_params();
		}
		else{
			Trace("(%s:%s:%d):save dev parameters err!", __FILE__, __func__, __LINE__);
		}
		//msybe need reconnect!!!
		
	}
	//OSSemPost(HXCfgParaSem);
	return 0;		
}

/*------------------------------------ƽ̨������Ϣ��������---------------------------------------*/
/*������paramID��Ӧ��С�˼��ƽ̨����ת���ɴ�ˣ�paramID��Ӧ��val������ԭ������ͬ���޺���������
������ͬ��Ҫ����ͬʱ�ն˿�������Ӧ����
	peijl_20120828 �޸�*/
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
			if(!ISAscIPValid(Srcval,len))
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
					WriteLbsCfgPara(eIP, Srcval,len,UpdateOnly); //�����棬���в�������ٺ󱣴浽flash
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
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,len,UpdateOnly); //�����棬���в�������ٺ󱣴浽flash
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
			if(!ISStrTelValid(Srcval,len))
				return 1;
			WriteLbsCfgPara(paramID,Srcval,len,UpdateOnly);
		break;
		case eCarid:   //peijl_130220 �޸ģ��������������ı�ʱ��Ҫ��ע������ע��
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
		case eAnswerType: //0�Զ����� 1acc on�Զ���off�ֶ�
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
		case eOdometer: //��̱����� 
			if(4!=len)
				return 1;
			char_to_int(Srcval,&U32Temp);
			//Trace("(%s:%s:%d): set params!", __FILE__, __func__, __LINE__);
			WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly); //�����棬���в�������ٺ󱣴浽flash
		break;
#if 0
		case eImagQuality:
			if(4!=len)
				return 1;
			char_to_int(Srcval,&U32Temp);
			if(U32Temp>0&&U32Temp<=10)
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly); //�����棬���в�������ٺ󱣴浽flash
			else
				return 1;
		break;
		case eImagBright:
		case eImagContrast:
		case eImagSatuatn:
			if(4!=len)
				return 1;
			if(U32Temp>=0&&U32Temp<=127)
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly); //�����棬���в�������ٺ󱣴浽flash
			else
				return 1;
		break;
		case eImagClor:
			if(4!=len)
				return 1;
			if(U32Temp>=0&&U32Temp<=255)
				WriteLbsCfgPara(paramID,(u8 *)&U32Temp,4,UpdateOnly); //�����棬���в�������ٺ󱣴浽flash
			else
				return 1;
		break;
#endif
		case ePrivic:
		case eCity:
			char_to_short(Srcval,&U16Temp);
			WriteLbsCfgPara(paramID,(u8 *)&U16Temp,2,UpdateOnly); //�����棬���в�������ٺ󱣴浽flash
		break;
		case eCarclor:
			WriteLbsCfgPara(paramID, Srcval,1,UpdateOnly); //�����棬���в�������ٺ󱣴浽flash
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
*Description:	ƽ̨�·������ն˲��������������
*Calls:          
*Called By:      
*Input:			
*				pbuf ����Ϣ���׵�ַ
*				pbuflen ����ɵı��ĳ���
*Output:
*Return:		ActionOK
*Others:         
*********************************************************/
static u8 ServReq_SetParam(u8 *pbuf, const u16 buflen)
{
	u8 paraNum=0;
	u8 i=0;
	u8 len=0; //������
	u16 devAct=0; //��������ʱ�ն˶���
	u8 Ary[200]={0}; //peijl_1012  ����ProtocolHandle��Ҫ?
	u16 alllen=buflen;
	u32 paramID;
	u8 sta=1;
	if(pbuf==NULL)
	{
		Trace("check_set_dev_param param error!");
		return 1;
	}
	//�ղ����б�
	if(buflen==1&&*pbuf==0)
		return 0;;

	paraNum=*pbuf++; //��������
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
	//���в���д��flash,���ò������ն���Ӧ����
	WriteLbsCfgPara(eCfaParaMax, NULL,0, UpdateFlash); //save it
//	WriteLinkTaskPara(devAct,ActionTypePara,SET);zq
	return sta;
}
/*********************************************************
*Function:       JT808MsgRsp_Send
*Description:  	 �ն˶�ƽ̨������Ӧ����
				����ƽ̨�ظ�
*Calls:          
*Called By:      
*Input:         Sendbuf ����������ַ
				Sendlen �������ݳ���
*Output:       
*Return:         ����ִ�н������Ӧenum _REQRSP_RESULT
				 ƽ̨Ӧ������enum 0~4
				 ParaError ����������������
				 ServErr	��������
				 DevError
*Others:         Sendlen�ε����ݲ��ܸı�
	����	20130109

*********************************************************/
u8 JT808MsgRsp_Send(u16 DevMsgId,u16 totalPackt,u16 SubPackt/*,u8 *Sendbuf,u16 Sendbuflen*/)
{
	u16 Senddatalen=0;

	u8 status;
	u8 err;
	u8 SeqId[2];	//������ˮ��
	//u8 *pbuf=Sendbuf;	//���ͻ�����
	u16 U16Temp;
	u16 ret;
#if 0
	//�ط�����
	u32 Retry=0;
	u32 RetryNum=0;
	u32 RetryWaittim=0; //�ط��ȴ�ʱ��T(n)=T(n-1)*n
	//CPU_SR cpu_sr;
#endif
	u8 pbuf[DATA_MAX_LEN] = 0;
#if 0
	if(Sendbuf==NULL)
	{
		Trace("JT808MsgRsp_Send param error!");
		return RspErrOther;
	}
#endif

	if (0 == JT808Msg_Build(DevMsgId, totalPackt, SubPackt, pbuf, DATA_MAX_LEN, &U16Temp)){	
		if (U16Temp > DATA_MAX_LEN){
			OA_DEBUG_USER("(%s:%s:%d): data is too long!", __FILE__, __func__, __LINE__);
			return RspErrOther;
		}

		ret = escape_copy_to_send(pbuf, U16Temp);
		if (ret > 0){
			oa_soc_send_req();//check datas in buffer & send	
		}
		return 0;
	}
	else{
		Trace("(%s:%s:%d): pacakge build error!", __FILE__, __func__, __LINE__);
		return RspErrOther; 
	}

#if 0 //I do it in function escape_copy_to_send
	//����������ˮ��,��ˮ�ŵڼ�
	Read_ProtclHandl(eDevSeqid,SeqId,&U16Temp);
	memcpy(pbuf+11,SeqId,2);

	memcpy(SeqId,pbuf+11,2);	 //��ԭ

	//У�飬����ʾͷβ��У�鱾��
	if(1==XOR_Check(pbuf+1, Senddatalen-3,(pbuf+Senddatalen-2)))
		return RspErrOther;
	//�Ա�ʾͷ��β֮��İ����ݽ���ת��
	status = JT808_dataChg(1, pbuf+1, Senddatalen-2, &U16Temp);
	Senddatalen=U16Temp+2; //����
			#if 0
			{//TEST
			u16 test;
			Trace(PrintDebug,"�ն�RSP���Ͱ� %d!\r\n",Senddatalen);
			for(test=0;test<Senddatalen;test++)
			{
			Trace(PrintInfo,"0x%02x ",*(pbuf+test));
			}
			Trace(PrintInfo,"\r\n");
			}
			#endif	
/* ���Ͱ� */
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
	//ȡ�ط����Ʋ���
	RetryNum=JT808_UploadRetrynum(); RetryNum=2; //peijl_130227 �ݶ�
	RetryWaittim=JT808_UploadOvertime();RetryWaittim=30;	

	//����Э���
	do{
		//δ��½ƽ̨����ͨ�����ϴ�����
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
			status=ServUnlog_SendErr; //����������
		}
		RetryWaittim=(Retry+1)*RetryWaittim; //JT808��ʱ�ȴ�����
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
		Trace("(%s:%s:%d):LBS_DevCtlreq commandId error!",__FILE__, __func__, __LINE__);
		return 1;
	}
	if(msgbodylen==0)
		return 0;

	cmd=*pmsgbody++;
	switch(cmd)
	{
		case eOnlineUpgrad_Ctrl:{
			control_type = wireless_update;
			break;
		}
		
#if 0
		case eOnlineUpgrad_Ctrl:	//��������
		{

			step=0;
			while(len<(msgbodylen-1)) //ȡ��������
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
				else //������
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
			SetUpgradStarttimeS(); //peijl_20121029 ���ӣ�����������ʱ��ʼʱ��
			}
		}
		break;

		case eSpclServ_Ctrl:	//�����ն�����ָ��������
		{
			if(*pmsgbody==1) //�л������࿼ƽ̨
			{
				gServLimittim=0; //��ʱ��
				SetMonitServ(eMainServ);
				WriteLinkTaskPara(ActionHXPPP|ActionLoginHX,ActionTypePara,SET);
			}
			else{ //�л������ƽ̨
				pmsgbody+=2;
				step=0;
				while(len<(msgbodylen-3)) //ȡ����
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
					else //������
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
			control_type = conn_to_specified_server;
			break;
		}
		case eDevPowoff_Ctrl:	//�ն˹ػ�
		{
			Trace("(%s:%s:%d):power down", __FILE__, __func__, __LINE__);
			control_type = term_powerdown;
			break;
		}

		case eDevReset_Ctrl:	//�ն˸�λ
		{
			//��������flash
			//WriteLinkTaskPara(ActionResetLbs,ActionTypePara,SET);
			Trace("(%s:%s:%d):term reset", __FILE__, __func__, __LINE__);
			control_type = term_reset;
			break;
		}

		case eDevFactoryset_Ctrl:	//�ָ���������
		{
			//�Ͽ�ƽ̨����
			//��������flash
			//����ͨ��
			#if 0//zq
			Factory_Set();
			WriteLinkTaskPara(ActionResetLbs,ActionTypePara,SET);
			#endif
			control_type = factory_setting;
			//factory_set();
			//oa_module_restart(NULL);
			break;
		}
		case eCloseDataCommu_Ctrl:	//�ر�����ͨ��
		{  
		#if 0
			//�ر��Ѵ��ڵ����ӣ���������������־
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
				{	  //�ر�ʧ��ʱ��֤����ƽ̨�Ա�������
					WriteLinkTaskPara(LinkHXSeverOk,ActionTypePara,SET);					
					return 1;
				}
				WriteLinkTaskPara(TLPPPOk|HXPPPOk,NetStatusPara,RESET);			
			}
		#endif
			control_type = turnoff_datatraffic;
			break;
		}
		case eCloseWCMCommu_Ctrl:	//�ر�����ͨ��
		{ 
		#if 0
			//�ر��Ѵ��ڵ����ӣ���������������־
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
				{	  //�ر�ʧ��ʱ��֤����ƽ̨�Ա�������
					WriteLinkTaskPara(LinkHXSeverOk,ActionTypePara,SET);					
					return 1;
				}
				WriteLinkTaskPara(TLPPPOk|HXPPPOk,NetStatusPara,RESET);			
			}
			//WCMӲ���ر� ���ڵ绰�Ҷ�


			//�绰
			ReadTelPara(&i,eCallAction);
			if(NoneAction!=i) //�ⲿ�Ҷ�
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
	
			Trace("(%s:%s:%d):LBS_DevCtlreq commandId error!",__FILE__, __func__, __LINE__);
			return 0;
		}
	}
	
	return 0;
}
/*********************************************************
*Function:     check_track_location_ctl
*Description:  ƽ̨������ʱλ����Ϣ����
*Calls:          
*Called By:      
*Input:         pmsgbody ��ѯ��Ϣ���׵�ַ
				msgbodylen ��ѯ��Ϣ�峤��
*Output:        
*Return:        �����Ƿ�ִ�гɹ�������0��ʾ�ɹ�
*Others:         
*********************************************************/
static u8 ServReq_GetPositionData_Period(u8 *pmsgbody, u16 msgbodylen)
{	
	if((pmsgbody==NULL)||(msgbodylen<2))
	{
		Trace("check_track_location_ctl param error!\r\n");
		return 1;
	}
	Trace("Period position trace:\r\n");
	Trace("    Inteval is    0x%02x%02x\r\n    Limit time is 0x%02x%02x%02x%02x\r\n",*pmsgbody,
	*(pmsgbody+1),*(pmsgbody+2),*(pmsgbody+3),*(pmsgbody+4),*(pmsgbody+5));
	Write_ProtclHandl(e_TrackLocationCtlreq, pmsgbody,msgbodylen);
	return 0;;
}
/*********************************************************
*Function:		ServReq_808data_handle
*Description:	�Լ��ƽ̨��������Ĵ�����Ӧ��
*				1.�򵥴����ڱ��������
*				2.�����������֪ͨ����������
*				3.�����Ӧ��
*Calls:          
*Called By:		JT808_PrtclAnaly() 
*Input:         ServReqMsgid ƽ̨������Ϣid
				msgbody	������Ϣ����Ϣ����ַ
				msgbodylen ������Ϣ����Ϣ�峤��
*Output:        
*Return:        ������� ����0��ʾ�ɹ�
*Others:         
*********************************************************/
u8 JT808_ServReq_handle(u16 ServReqMsgid,u8 *msgbody,u16 msgbodylen/*,u8 *sendbuf,u16 sendbuflen*/)
{
	u8 	status = 0;
	u16 senddatalen=0; //Ӧ��������ݳ���
	oa_bool need_reconn = OA_FALSE;
	if(msgbody==NULL)
	{
		Trace("(%s:%s:%d): check_msg_rsp parameter error!", __FILE__, __func__, __LINE__);
		return RspErrOther;
	}
	//��Ϣ�崦��
	//JT808_ServReq_MSG();
	switch(ServReqMsgid){
		case SET_DEV_PARAM:{//platform request to set device paramaters & device ack common 
			//Trace("(%s:%s:%d): Recieve parameter-setting requry from moniter server.", __FILE__, __func__, __LINE__);
			status = ServReq_SetParam(msgbody,msgbodylen);	//����
			Write_ProtclHandl(eRsp2ServSeq,&status,1); //peijl_20120918 ���ӣ����������
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0/*,sendbuf,sendbuflen*/); //PEIJL_130109 �޸�
			break;
		}
		case GET_DEV_PARAM:{//platform request to get device paramaters
			Trace("(%s:%s:%d): Recieve parameter-getting requry from moniter srever.", __FILE__, __func__, __LINE__);
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
//status=3 Ϊ��ʱ����
		#if 0
		case TEXT_DOWNLOAD:
		{
			status = ServReq_Textinfo(msgbody,msgbodylen);
			Write_ProtclHandl(eRsp2ServSeq,&status,1);
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			break;
		}
		
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
		case SET_ROUND_AREA:
		case DEL_ROUND_AREA:
		case SET_SQUARE_AREA:
		case DEL_SQUARE_AREA:
		case SET_POLYGON_AREA:
		case DEL_POLYGON_AREA:
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
				SchedulScrn_CallforcarSend("8888888888",10,p->OrderContent,p->OrderContentlen); //ID�̶�
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
				Trace(PrintDebug,"---------tel len:%d \r\n",strlen(p->OrderTel));
				p->OrderContent[p->OrderContentlen]='\0'; 
				Trace(PrintDebug,"---------���ݳ���:%d \r\n",p->OrderContentlen); 
				Trace(PrintDebug,"---------����:%s \r\n",p->OrderContent); 
				SchedulScrn_CallforcarInfoSend("8888888888",10,p->OrderTel,strlen(p->OrderTel),p->OrderContent,p->OrderContentlen); //ID�̶�
			}
			JT808MsgRsp_Send(DEV_COMMON_rsp,1,0,sendbuf,sendbuflen);
			//peijl_test �ݶ���Ӧ
			*sendbuf=1; //1�ɹ� 2ʧ��
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
			//�ϱ���·
		break;
		#endif
		default:
		{
			Trace("(%s:%s:%d): Server MsgId error:%04x!", __FILE__, __func__, __LINE__, ServReqMsgid);
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
*Description:	��λϵͳ����808Э�����.
*				1.Э��У��
*				2.���������ݰ���Ϊ�ն������Ӧ��ƽ̨���������
*Calls:			many
*Called By:      
*Input:			data ���յ�ƽ̨�����׵�ַ
				datalen ����ƽ̨���ݳ�
				sendbuf ������
				sendbuflen	������ʵ�ʳ�
*Return:		unknown
*Others:         
**************************************************************/
u8 JT808_recv_analysis(u8 *data,u16 datalen/*,u8 *sendbuf,u16 sendbuflen*/)
{
	u8 temp=0;
	u8 *pbuf=data;
	u16 ServmsgID=0;
	u16 Reallen; //Э���ʵ�ʳ���
	u16  U16Temp;
#if 0
	if(0)
	{
	u16 test;
	Trace(PrintDebug,"Raw Server data:!\r\n"); 	
	for(test=0;test<datalen;test++)
		Trace(PrintDebug,"%02x ",*(data+test));
	Trace(PrintInfo,"\r\n");	
	}
#endif
	if(NULL==data || datalen==0)  //Э����С����δ���� ������
		return RspPackgerr;	
	//��ʾͷβ�ж�
	if(*data!=0x7e || *(data+datalen-1)!=0x7e){
		Trace("(%s:%s:%d): 0x7e is bad!!!", __FILE__, __func__, __LINE__);
		return RspPackgerr;	
	}
		
	//��ת�廹ԭ����
	if(JT808_dataChg(0, data+1, datalen-2, &Reallen)){
		Trace("(%s:%s:%d): turning parameter error", __FILE__, __func__, __LINE__);
		return RspPackgerr;
	}
	Reallen += 2; //���ϱ�ʾͷβ
#if 0
	if(0){
	u8 test;
	Trace(PrintDebug,"Server data after translition:!\r\n"); 	
	for(test=0;test<Reallen;test++)
		Trace(PrintDebug,"%02x ",*(data+test));
	Trace(PrintInfo,"\r\n");
	}
#endif
	//msgid��Ч���ж�
	char_to_short(data+1, &ServmsgID);
	if (!ISPrtclServMsg(ServmsgID)){
		Trace("(%s:%s:%d): Unknown server message ID!", __FILE__, __func__, __LINE__);
		return RspPackgerr;
	}
	//У��λ�ж�
	if (!XOR_Check(data+1,Reallen-3,&temp)){
		if (temp!=*(data+Reallen-2)){
			Trace("(%s:%s:%d): Server data xor err!", __FILE__, __func__, __LINE__); 	
			return RspPackgerr;
		}
	}
	else
		return RspPackgerr;	    //Ӧ׷����Ϣ����Ӧ�� ???

	pbuf++;//pbuf --> msg id
	switch (ServmsgID){
		/*4��ƽ̨Ӧ��*/
		case REGISTERS_rsp:{	//ע��Ӧ��
			LBS_PlatComRsp rsp;
			memcpy(rsp.MsgId, pbuf, 2);
			pbuf+=sizeof(MSG_HEAD);
			memcpy(rsp.SeqId, pbuf, 2);
			pbuf+=2;
			rsp.Rslt = *pbuf;

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
						Trace("(%s:%s:%d): no authentication code", __FILE__, __func__, __LINE__);
						return RspPackgerr;
					}
				}
				break;
				case RspError:
				case RspUnsurport:{
					Trace("(%s:%s:%d): has already registered:%d", __FILE__, __func__, __LINE__, rsp.Rslt);
					return PLAT_TREMREG_ACK;
				}
				break;
				case RspMsgerr:
				case RspAlarmCheck:{
					Trace("(%s:%s:%d): database has no info:%d", __FILE__, __func__, __LINE__, rsp.Rslt);
					return RspPackgerr;
				}
				break;
				default:{
					Trace("(%s:%s:%d): Rsp packet err!", __FILE__, __func__, __LINE__);
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
					Trace("(%s:%s:%d): no authentication code", __FILE__, __func__, __LINE__);
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
		case REPORT_MEDIA_DATA_rsp:	//��ý�������ϴ�Ӧ��
		{
			LBS_PlatComRsp rsp;
			Trace("Recieve Result of media data uploading."); 	
			memcpy(rsp.MsgId,pbuf,2);
			pbuf+=sizeof(MSG_HEAD);
			pbuf+=4; //media id��Ч
			if(*pbuf==0)
			{
				rsp.Rslt=0;//�ɹ�
			}
			else
			{
		//	if(Reallen-3-5-sizeof(MSG_HEAD)>0)
			//���б�---Э��δ�� ������				
				rsp.Rslt=1;//ʧ��
				Trace(PrintInfo,"Media data report failed!\r\n"); 	
			}
			Write_ProtclHandl(eRsp2DevReq,(u8 *)&rsp,sizeof(LBS_PlatComRsp));
			OSSemPost(JT808_ReqRspSem);
		}
		
		break;
#endif
		case PLAT_COMMON_rsp:{ //ƽ̨ͨ��Ӧ��
			LBS_PlatComRsp rsp;
			pbuf+=sizeof(MSG_HEAD);
			memcpy(&rsp, pbuf, sizeof(LBS_PlatComRsp));
			Write_ProtclHandl(eRsp2DevReq, (u8 *)&rsp, sizeof(LBS_PlatComRsp));	
			return PLAT_COMMON_ACK;
		}
		break;
	/*	case RSA_PLAT: //ƽ̨��Կ�·�
		{
			//����ط���ʵ���д���ȶ
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
		default:{//����N��Ϊƽ̨����
			Write_ProtclHandl(eServMsgid, pbuf, 2);
			Write_ProtclHandl(eServSeqid, (pbuf+10), 2);
			if(*(pbuf+2)&0x20) {//�зְ��� peijl_20120912
				pbuf+=(sizeof(MSG_HEAD)+sizeof(MSG_HEAD_sub));
				U16Temp=Reallen-3-(sizeof(MSG_HEAD)+sizeof(MSG_HEAD_sub));
			}
			else{
				pbuf+=sizeof(MSG_HEAD);
				U16Temp=Reallen-3-sizeof(MSG_HEAD);
			}
	
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
*Description:	��λϵͳ����808Э�����.
*				1.Э��У��
*				2.���������ݰ���Ϊ�ն������Ӧ��ƽ̨���������
*Calls:			many
*Called By:      
*Input:			data ���յ�ƽ̨�����׵�ַ
				datalen ����ƽ̨���ݳ�
				sendbuf ������
				sendbuflen	������ʵ�ʳ�
*Return:		unknown
*Others:         
**************************************************************/
u8 JT808_PrtclAnaly(u8 *data,u16 datalen,u8 *sendbuf,u16 sendbuflen)
{
	u8 temp=0;
	u8 *pbuf=data;
	u16 ServmsgID=0;
	u16 Reallen; //Э���ʵ�ʳ���
	u16  U16Temp;
#if 0
	if(0)
	{
	u16 test;
	Trace(PrintDebug,"Raw Server data:!\r\n"); 	
	for(test=0;test<datalen;test++)
		Trace(PrintDebug,"%02x ",*(data+test));
	Trace(PrintInfo,"\r\n");	
	}
#endif
	if(NULL==data || datalen==0)  //Э����С����δ���� ������
		return RspPackgerr;	
	//��ʾͷβ�ж�
	if(*data!=0x7e || *(data+datalen-1)!=0x7e)
		return RspPackgerr;	
	//��ת�廹ԭ����
	if(JT808_dataChg(0, data+1, datalen-2, &Reallen))
	{
		Trace("turning parameter error");
		return RspPackgerr;
	}
	Reallen+=2; //���ϱ�ʾͷβ
#if 0
	if(0){
	u8 test;
	Trace(PrintDebug,"Server data after translition:!\r\n"); 	
	for(test=0;test<Reallen;test++)
		Trace(PrintDebug,"%02x ",*(data+test));
	Trace(PrintInfo,"\r\n");
	}
#endif
	//msgid��Ч���ж�
	char_to_short(data+1, &ServmsgID);
	if(!ISPrtclServMsg(ServmsgID))
	{
		Trace("Unknown server message ID!");
		return RspPackgerr;
	}
	//У��λ�ж�
	if(!XOR_Check(data+1,Reallen-3,&temp))
	{
		if(temp!=*(data+Reallen-2))	
		{
			Trace("Server data xor err!"); 	
			return RspPackgerr;
		}
	}
	else
		return RspPackgerr;	    //Ӧ׷����Ϣ����Ӧ�� ???

	pbuf++;//pbuf --> msg id
	switch(ServmsgID)
	{
		/*4��ƽ̨Ӧ��*/
		case REGISTERS_rsp:	//ע��Ӧ��
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
				{ 	//��Ȩ�����ע��״̬����flash
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
			Trace("Recieve Registed result."); 	
		}
		break;
		#if 0
		case REPORT_MEDIA_DATA_rsp:	//��ý�������ϴ�Ӧ��
		{
			LBS_PlatComRsp rsp;
			Trace("Recieve Result of media data uploading."); 	
			memcpy(rsp.MsgId,pbuf,2);
			pbuf+=sizeof(MSG_HEAD);
			pbuf+=4; //media id��Ч
			if(*pbuf==0)
			{
				rsp.Rslt=0;//�ɹ�
			}
			else
			{
		//	if(Reallen-3-5-sizeof(MSG_HEAD)>0)
			//���б�---Э��δ�� ������				
				rsp.Rslt=1;//ʧ��
				Trace(PrintInfo,"Media data report failed!\r\n"); 	
			}
			Write_ProtclHandl(eRsp2DevReq,(u8 *)&rsp,sizeof(LBS_PlatComRsp));
			OSSemPost(JT808_ReqRspSem);
		}
		#endif
		break;
		case PLAT_COMMON_rsp: //ƽ̨ͨ��Ӧ��
		{
			LBS_PlatComRsp rsp;
			Trace("Recieve common-response form moniter server."); 	
			pbuf+=sizeof(MSG_HEAD);
			memcpy(&rsp,pbuf,sizeof(LBS_PlatComRsp));
			Write_ProtclHandl(eRsp2DevReq,(u8 *)&rsp,sizeof(LBS_PlatComRsp));	
			Trace("Server common reply:\r\n"); 	
			Trace("DevReqID    0x%02x%02x\r\n",*pbuf,*(pbuf+1)); 	
			Trace("    DevReqmsgID 0x%02x%02x\r\n",*(pbuf+2),*(pbuf+3)); 	
			Trace("    Rsp result  %02x\r\n",*(pbuf+4)); 	
			//OSSemPost(JT808_ReqRspSem);
		}
		break;
	/*	case RSA_PLAT: //ƽ̨��Կ�·�
		{
			//����ط���ʵ���д���ȶ
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
	*/	//����N��Ϊƽ̨����
		default:
		{
			Write_ProtclHandl(eServMsgid,pbuf,2);
			Write_ProtclHandl(eServSeqid,(pbuf+10),2);
			if(*(pbuf+2)&0x20) //�зְ��� peijl_20120912
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
*Description:   �ն�ͨ��Ӧ��
*Calls:          
*Called By:      
*Input:          pbuf ���ⲿ�������׵�ַ
*Output:        pbuflen ����ɵı��ĳ���
*Return:         �����Ƿ�ִ�гɹ�������0��ʾ�ɹ�
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
*Description:	��ȫ�ֱ���LbsCfgStruct
*input:			CfgPara ������Ӧ��JTT808Э��ID
*output:		����*buflen���ȵ����ݴ�ŵ���ַpValue
*Return:		��ȡ�����ݳ��� 0:err other:����  �ƻ��޸�
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
			if(LbsCfgStruct.LclTELlen>12) //808Э�����12�ֽ� peijl_130311 ���ӱ����Ŵ�����Ĭ��ֵ
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
		case eOdometer: //��̱����� 
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
			Mem_Copy(pValue, dev_now_params.manufacturers_id, 5);//5�̶�����
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
*Description:	�����ն�ע����Ϣ��
*Calls:          
*Called By:      
*Input:			pprotHandle �����ָ��
				Buf ���ⲿ�������׵�ַ
*Output:		pbuflen ����ɵı��ĳ���
*Return:		�����Ƿ�ִ�гɹ�������0��ʾ�ɹ�
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
*Description:    �����ն˼�Ȩ�������Ϣ��
*Calls:          
*Called By:      
*Input:
				pbuf ���ⲿ�������׵�ַ
*Output:        pbuflen ����ɵı��ĳ���
*Return:		�����Ƿ�ִ�гɹ�������AcktionOK��ʾ�ɹ�
*Others:         
*********************************************************/
static u8  dev_login_buildbody(u8 *pbuf, u16 *pbuflen)
{
	oa_bool ret;
	//if(eSpclServ==GetMonitServ()) //���ƽ̨:��ȡ���ĸ����ƽ̨
	if (0){ //peijl_test 120613
		Read_ProtclHandl(e_Authen,pbuf,pbuflen);
	}
	else{
#if 0
		Flash_Read_Buffer(&len,1,DEV_AUTHEN_LEN_ADDR);
		if(len>AUTHENMAXLEN)
		{
			Trace(PrintError,"Error authen in flash!!\r\n");
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
*Description:	�ն˶�ƽ̨�����ѯ������Ӧ���
*Calls:          
*Called By:      
*Input:          pprotHandle �����ָ��
			pbuf ���ⲿ�������׵�ַ
*Output:        pbuflen ����ɵı��ĳ���
*Return:         �����Ƿ�ִ�гɹ�������0��ʾ�ɹ�
*Others:         �˹������ƽ̨�ԽӺ�֪
	peijl_20120911 �޸�
*********************************************************/
static u8 get_dev_param_rsp_buildbody(u8 *msgbody, u16 *pbuflen)
{
//	PARAM_LIST *paramlist=NULL;
	u8 len=0;
	u32 para;
	u8 *pbuf=msgbody;
	u8 NUM=0; //��������
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
//λ����Ϣ�㱨�Ļ�����Ϣ��Ϣ�崴��
u8 report_location_msgbody1(u8 *Buf, u16 *pbuflen)
{
	u8 *pbuf=Buf;
	u32 alarmflag;
	float temp=0.0;
	u8  cnt=0;
	STRUCT_RMC GpsInfor;

	if(Buf==NULL||pbuflen==NULL)
		return 1;
	GPS_GetPosition(&GpsInfor); //ȡgps
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
	for(cnt=0;cnt<2;cnt++) //γ��,�����֮һ��
	{
		alarmflag=alarmflag*10+GpsInfor.Latitude[cnt]-'0';
	}
	for(cnt=2;cnt<8;cnt++) //γ��
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
	for(cnt=0;cnt<3;cnt++) //����,�����֮һ��
	{
		alarmflag=alarmflag*10+GpsInfor.Longitude[cnt]-'0';
	}
	for(cnt=3;cnt<9;cnt++) //����
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
//λ����Ϣ�㱨�ĸ�����Ϣ����Ϣ�崴��
static u8 report_location_msgbody2(u8 *Buf, u16 *pbuflen)
{
	u8 *pbuf=Buf;
	u32 alarmflag;

	if(Buf==NULL||pbuflen==NULL)
		return 1;
	//λ�ø�����Ϣ���б�
	*pbuf++=0x01;//���
	*pbuf++=0x04;
	//ReadDistanceData(&alarmflag); //KM?	peijl_test
	{
		//read mileage by zq
		oa_int32 handle, ret;
		TOTAL_MILE temp;
		oa_uint32 dummy_read;
		handle = oa_fopen(MILEAGE_FILE);
		if (handle < 0){
			Trace("(%s:%s:%d):mileage file open err!", __FILE__, __func__, __LINE__);
			alarmflag = 0;//var name is bad here
		}
		else{
			ret = oa_fread(handle, &temp, sizeof(TOTAL_MILE), &dummy_read);
			if ((ret < 0) || (dummy_read != sizeof(TOTAL_MILE))) {
				OA_DEBUG_USER("(%s:%s:%d):read mileage err!", __FILE__, __func__, __LINE__);
				alarmflag = 0;
			}
			alarmflag = temp.total_mileage;
			ret = oa_fclose(handle);
			if (ret < 0){
				OA_DEBUG_USER("(%s:%s:%d):close file err!", __FILE__,  __func__, __LINE__);
				oa_fdelete(MILEAGE_FILE);
			}
		}
		
	}
	int_to_char(pbuf, alarmflag); //100M
	pbuf+=4;
	*pbuflen += 6;
	*pbuf++=0x02;//����
	*pbuf++=0x02;
	*pbuf++=0x00;	//�ݶ�
	*pbuf++=0x64;
	*pbuflen +=4;
	*pbuf++=0x03;//��ʻ��¼���ٶ�
	*pbuf++=0x02;
//	short_to_char(pbuf,gDriveRecodSpeed);  //??????	 
	memset(pbuf,0x00,2);
	pbuf+=2;
	*pbuflen +=4;
	*pbuf++=0x11;//���ٱ���������Ϣ
	*pbuf++=0x01;
	*pbuf++=0x00; //�ݶ�
	*pbuflen +=3;
/*	*pbuf=0x12;//���������·��
	pbuf++;
	*pbuf=0x06;
	pbuf++;
	memset(pbuf,0x0,6);
	*pbuflen +=8;
	*pbuf=0x13;//���������·��
	pbuf++;
	*pbuf=0x07;
	pbuf++;
	memset(pbuf,0x0,7);
	*pbuflen +=9;
*/

#if 0
	ADD_LOCATION_INFO addInfo;
//	READ(meliage,oil,meliage,locationtype11,area_line_id11,locationtype22,area_line_id22,direction);
	addInfo.AddInfoId = 0x1;		//��̣���Ӧ���ϵ���̱�����
	addInfo.AddInfoLen = 4;
	memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
	*pbuflen += sizeof(ADD_LOCATION_INFO);
	memcpy(pbuf+*pbuflen, "1111", addInfo.AddInfoLen);
	*pbuflen += addInfo.AddInfoLen;

	addInfo.AddInfoId = 0x2;		//��������Ӧ��������������
	addInfo.AddInfoLen = 2;
	memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
	*pbuflen += sizeof(ADD_LOCATION_INFO);
	memcpy(pbuf+*pbuflen, "22", addInfo.AddInfoLen);
	*pbuflen += addInfo.AddInfoLen;

	addInfo.AddInfoId = 0x3;		//�ٶȣ���ʻ��¼���ܻ�ȡ���ٶ�
	addInfo.AddInfoLen = 2;
	memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
	*pbuflen += sizeof(ADD_LOCATION_INFO);
	memcpy(pbuf+*pbuflen, "33", addInfo.AddInfoLen);
	*pbuflen += addInfo.AddInfoLen;

	if(alarmflag & ALARM_OVER_SPEED)
	{
		addInfo.AddInfoId = 0x11;		//���ٱ�����
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
		addInfo.AddInfoId = 0x12;		//��������/·�߱���������Ϣ
		addInfo.AddInfoLen = 1;
		memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
		*pbuflen += sizeof(ADD_LOCATION_INFO);
		*(pbuf+(*pbuflen)) = 2;
		*pbuflen += addInfo.AddInfoLen;
		addInfo.AddInfoLen = 4;
		memcpy(pbuf+(*pbuflen), "1234", addInfo.AddInfoLen);
		*pbuflen += addInfo.AddInfoLen;
		addInfo.AddInfoLen = 1;
		*(pbuf+(*pbuflen)) = 0;//0��/1��
		*pbuflen += addInfo.AddInfoLen;
	}
	if(alarmflag & ALARM_DRIVE_SHORT_LONG_k)
	{
		addInfo.AddInfoId = 0x13;		//·����ʻʱ�䲻��/��������
		addInfo.AddInfoLen = 4;
		memcpy(pbuf+*pbuflen, &addInfo, sizeof(ADD_LOCATION_INFO));
		*pbuflen += sizeof(ADD_LOCATION_INFO);
		memcpy(pbuf+(*pbuflen), "1111", addInfo.AddInfoLen);
		*pbuflen += addInfo.AddInfoLen;
		addInfo.AddInfoLen = 2;
		memcpy(pbuf+(*pbuflen), "22", addInfo.AddInfoLen);
		*pbuflen += addInfo.AddInfoLen;
		addInfo.AddInfoLen = 1;
		*(pbuf+(*pbuflen)) = 0;//0����1����
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
	//λ�ø�����Ϣ���Զ�������
	*pbuf++=0xE0;
	*pbuf++=0x04;
	*pbuf++=0x00;
	*pbuf++=0x00;
	*pbuf++=0x00;
	*pbuf++=0x00; //??
	*pbuflen=6;

	*pbuf++=0xE1; //����ת
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
*Description:	����λ����Ϣ�㱨��Ϣ��
*Calls:          
*Called By:      
*Input:			
				Buf ���ⲿ�������׵�ַ
*Output:        pbuflen ����ɵı��ĳ���
*Return:         �����Ƿ�ִ�гɹ�������0��ʾ�ɹ�
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
*Description:    λ����Ϣ��ѯӦ��
*Calls:          
*Called By:      
*Input:          pprotHandle �����ָ��
			pbuf ���ⲿ�������׵�ַ
*Output:        pbuflen ����ɵı��ĳ���
*Return:         �����Ƿ�ִ�гɹ�������0��ʾ�ɹ�
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
*Description:   �¼�����
*Calls:          
*Called By:      
*Input:          pprotHandle �����ָ��
			pbuf ���ⲿ�������׵�ַ
*Output:        pbuflen ����ɵı��ĳ���
*Return:         �����Ƿ�ִ�гɹ�������0��ʾ�ɹ�
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
*Description:   ����Ӧ��
*Calls:          
*Called By:      
*Input:          pprotHandle �����ָ��
			pbuf ���ⲿ�������׵�ַ
*Output:        pbuflen ����ɵı��ĳ���
*Return:         �����Ƿ�ִ�гɹ�������0��ʾ�ɹ�
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
*Description:   ��Ϣ�㲥/ȡ��
*Calls:          
*Called By:      
*Input:          pprotHandle �����ָ��
			pbuf ���ⲿ�������׵�ַ
*Output:        pbuflen ����ɵı��ĳ���
*Return:         �����Ƿ�ִ�гɹ�������0��ʾ�ɹ�
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
*Description:   ��������Ӧ��
*Calls:          
*Called By:      
*Input: 		pbuf ���ⲿ�������׵�ַ
*Output:        pbuflen ����ɵı��ĳ���
*Return:         �����Ƿ�ִ�гɹ�������0��ʾ�ɹ�
*Others:         
*********************************************************/
static u8 car_ctl_rsp_buildbody(u8 *pbuf, u16 *pbuflen)
{
	u16 len;
	Read_ProtclHandl(eServSeqid,pbuf,pbuflen); //ƽ̨����ˮ��
	pbuf+=*pbuflen;
	report_location_buildbody(pbuf, &len);
	*pbuflen += len;
	return 0;
}
/*********************************************************
*Function:       BuildMsgbody
*Description:   ����msg_ID��ֵ��������õ����ĸ���Ϣ��
*Calls:          
*Called By:      BuildMsg
*Input: 		pbuf ���ⲿ�������׵�ַ
*Output:        pbuflen ����ɵı��ĳ���
*Return:         �����Ƿ�ִ�гɹ�������0��ʾ�ɹ�
*Others:         
*********************************************************/
static u8 BuildMsgbody(u16 DevMsgId, u8 *msgbody, u16 *msgbodylen, u16 totalPackt,u16 SubPackt)
{
	u8 status=0;
	if(msgbody==NULL || msgbodylen==NULL ||totalPackt==0)
	{
		return 1;
	}
	switch(DevMsgId)//�ն���Ϣ
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
		case HEART_BEAT:
		{
			//����Ϣ��
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
			status=report_drive_data_buildbody(msgbody, msgbodylen,totalPackt,SubPackt);	  //������ʻ��¼�ϴ���Ϣ��
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
		case SMEDIA_DATA_CHECK_rsp: //��ý�����
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
			Trace("(%s:%s:%d): msg_ID  err", __FILE__, __func__, __LINE__);
			return 1;
		}
	}
	return status;
}
/*********************************************************
*Function:       TelAsc2BCD
*Description:	 ��ȡ�����Ų�����BCD��
*Input:          Deslen BCD��λ��
				 Srcstr ������
*Output:		 DesTel BCD�����Ŵ�ŵ�ַ
*Return:         �����Ƿ�ִ�гɹ�������0��ʾ�ɹ�
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
*Description:   �齨�ն˷��ͱ���
*Calls:          
*Called By:      
*Input:          DevMsgId Ҫ���͵��ն���Ϣ����
				 totalPackt	�����ܰ���
				 SubPackt	���͵�ǰ���ţ�JT808��1��ʼ
				 Sendbuf��Sendbuflen	���ͻ������ͳ���
				 Senddatalen ���ͱ���ʵ�ʳ���
*Output:         
*Return:         �����Ƿ�ִ�гɹ���0�ɹ� / 1
*Others:          
*********************************************************/
u8 JT808Msg_Build(u16 DevMsgId,u16 totalPackt,u16 SubPackt,u8 *Sendbuf,u16 Sendbuflen,u16 *Senddatalen)
{
	u8  status = 0;
	u8 	U8Temp = 0;
	u8  *pbuf = NULL;
	u8  tbuf[15] = {0};
	MSG_HEAD *msghead = (MSG_HEAD *)(Sendbuf+1); //��Ϣͷ
	MSG_HEAD_bodyattr msgbody_attr;	   //��Ϣ������
	u16 U16Temp = 0;
	
	if (!ISPrtclDevMsg(DevMsgId)||Sendbuf==NULL||Senddatalen==NULL||Sendbuflen==0){
		Trace("(%s:%s:%d): JT808Msg_Build parameter error!", __FILE__, __func__, __LINE__);
		return 1;
	}
	pbuf = Sendbuf;

	//�齨��Ϣͷ!!!
	*pbuf++=0x7e;
	*Senddatalen=1;
	memset(msghead,0x00,sizeof(MSG_HEAD)); //������ˮ�ţ���ˮ���ڷ���ʱ����
	short_to_char(msghead->MsgId, DevMsgId);
	memset(&msgbody_attr,0x00,2); //Ĭ����Ϣ������
	if (totalPackt > 1){
		msgbody_attr.subpacket = 1;
	}
	else{
		msgbody_attr.subpacket = 0;
	}
	
	ReadLbsCfgPara(eLclTEL,tbuf,&U8Temp);
	TelAsc2BCD(msghead->lclTel, &status, tbuf, U8Temp);//������<=12
	pbuf+=sizeof(MSG_HEAD);
	*Senddatalen+=sizeof(MSG_HEAD);
	//��Ϣ����װ��
	if (totalPackt>1){
		MSG_HEAD_sub *headsub=(MSG_HEAD_sub*)pbuf;
		short_to_char(headsub->totalpacketNum, totalPackt);
		short_to_char(headsub->SubpacketNo, SubPackt);
		pbuf+=sizeof(MSG_HEAD_sub);
		*Senddatalen+=sizeof(MSG_HEAD_sub);
	}

	//������Ϣ��!!!
	status = BuildMsgbody(DevMsgId,pbuf, &U16Temp,totalPackt, SubPackt);
	if (status){
		Trace("(%s:%s:%d): BuildMsgbody error, status=%d!", __FILE__, __func__, __LINE__, status);
		return 1;
	}
	//��ֵ��Ϣ�����Եĳ���
	msgbody_attr.MsgbodyLen = U16Temp;	//����Ϣ�����Ը�ֵ��Ϣ�峤��
	short_to_char(Sendbuf+3, *(u16*)&msgbody_attr);
	pbuf+=U16Temp;
	*Senddatalen+=U16Temp;
//#ifdef LBS_RSA
#if 0
	if(msgbody_attr.DataEncrypt1)
	{	//����Ϣ�����
		u16 msgbodylen_code;
		Encode(pstr+len, &msgbodylen_code, &msgbody, msgbodylen);//���ܺ�����ݱ�����pstr+len��
		U16Temp = msgbodylen_code;
	}
	else
	{
		Mem_Copy(pstr+*Senddatalen, msgbody, msgbodylen);
	}
#endif
	//У��
	*pbuf++=0;
	*Senddatalen += 1;
	//��ʶβ
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
*Description:    ��������ʱ�����������жϱ��������֣�
txt����������ʱ���ռ���Ƭ�ϴ�����洢,�ؼ�����
*Calls:          ��
*Called By:      ��
*Input:          ALARM_DEF ��Ӧλ����Ϣ�㱨�ı�����־λ�궨��
*Output:          
*Return:         ReturnType���� ParaError �������� 
*				 AcktionError �������Σ�AcktionOK ����
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
	if(flag&ALARM_DEF)//��������
	{
		return 2;
	}
	else
	{
		WriteAlarmPara(SET,StaAlarm0,ALARM_DEF);
		DevReq2ServPackag_build(REPORT_LOCATION); //�澯����һ��λ�û㱨
		//WriteAlarmPara(SET,StaAlarm0,ALARM_DEF);
		#if 0
		switch(ALARM_DEF)
		{
			case ALARM_EMERGENCY_k:
				Trace("ALARMtype:%d", ALARM_EMERGENCY_k);
			break;
			case ALARM_OVER_SPEED:
				Trace("ALARMtype:%d", ALARM_OVER_SPEED);
			break;
			case ALARM_DRIVE_TIRED:
				Trace("ALARMtype:%d", ALARM_DRIVE_TIRED);
			break;
			case ALARM_PRE_k:
				Trace("ALARMtype:%d", ALARM_PRE_k);
			break;
			case ALARM_GNSS_ERR:
				Trace("ALARMtype:%d", ALARM_GNSS_ERR);
			break;
			case ALARM_GNSS_ANTENNA:
				Trace("ALARMtype:%d", ALARM_GNSS_ANTENNA);
			break;
			case ALARM_GNSS_SHORT_CIRCUIT:
				Trace("ALARMtype:%d", ALARM_GNSS_SHORT_CIRCUIT);
			break;
			case ALARM_POWER_UNDERVOLTAGE:
				Trace("ALARMtype:%d�ն�����ԴǷѹ");
			break;
			case ALARM_POWER_BROWN:
				Trace("ALARMtype:%d�ն�����Դ����");
			break;
			case ALARM_DISPLAY_ERR:
				Trace("ALARMtype:%dLCD����ʾ������");
			break;
			case ALARM_TTS_ERR:
				Trace("ALARMtype:%dTTS�澯");
			break;
			case ALARM_CAMERA_ERR:
				Trace("ALARMtype:%d����ͷ����");
			break;
			case ALARM_DRIVE_OVERTIME:
				Trace("ALARMtype:%d�����ۼƼ�ʻ��ʱ");
			break;
			case ALARM_OVERTIME_PARKING:
				Trace("ALARMtype:%d��ʱͣ��");
			break;
			case ALARM_ENTER_AREA:
				Trace("ALARMtype:%d�������򱨾�");
			break;
			case ALARM_TOUCH_LINE_k:
				Trace("ALARMtype:%d����·�߱���!\r\n");
			break;
			case ALARM_DRIVE_SHORT_LONG_k:
				Trace("ALARMtype:%d·����ʻʱ�䲻������!\r\n");
			break;
			case ALARM_DRIVE_DEVIATE:
				Trace("ALARMtype:%d·��ƫ�뱨��!\r\n");
			break;
			case ALARM_VSS_ERR:
				Trace("ALARMtype:%d����VSS����!\r\n");
			break;
			case ALARM_OIL_ERR:
				Trace("ALARMtype:%d�����쳣����!\r\n");
			break;
			case ALARM_STEAL:
				Trace("ALARMtype:%d��������!\r\n");
			break;
			case ALARM_START_ILLEGAL_k:
				Trace("ALARMtype:%d�����Ƿ����!\r\n");
			break;
			case ALARM_MOVE_ILLEGAL_k:
				Trace("ALARMtype:%d�����Ƿ�λ��!\r\n");
			break;
			case ALARM_ROLLOVER:
				Trace("ALARMtype:%d�����෭����!\r\n");
			break;
			default:
				Trace("ALARMtype:%d��������!\r\n");
			break;
		}
		Trace("!\r\n");
		#endif

		
		ReadLbsCfgPara(etxtSMSAlarm,(u8 *)&flag,&paralen);
		if(flag&ALARM_DEF)//�ı�SMS����
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
					Str_Copy(pSMS,"ALARMtype:%dԤ��");
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
					Str_Copy(pSMS,"display screen error alarm!"); //���������ֱ�
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
					Str_Copy(pSMS,"Alarm for abnormaol amount of oil.");	//��λ������
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
			//WCM_SendSMS(ASC_SMS,Tel,paralen,pSMS,strlen(pSMS));zq ע�͵��˷�����
		}
		#if 0
		ReadLbsCfgPara(eAlarmPhoto,(u8 *)&flag,&paralen);
		if(flag&ALARM_DEF)//������������									-
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
			Write_ProtclHandl(eMediaInfo, (u8 *)&mediainfo, sizeof(LBS_Mediainfo)); //�ϴ�ʱ��

			//����ʱ�ݶ�ʹ�����в�����ΪĬ�����ղ���
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
			if(flag&ALARM_DEF)//���ձ��治����
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
		if(flag&ALARM_DEF) //�ؼ�����
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
/*----------------------------λ����Ϣ���в���------------------------------*/
/*ȡposbuf�Ŀ�һά����ĵ�ַ*/
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
//		Trace(PrintDebug,"position data index��\r\n");	
//		for(i=0;i<POSQUEUE_BUFNUM;i++)
//			Trace(PrintDebug,"%d\r\n",posbufindex[i]);	
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
	u16 real_len;
	u8 SeqId[2];
	u16 U16Temp;
	
	if (NULL == buf || len == 0)
	{
		OA_DEBUG_USER("(%s:%s:%d): buf/len err", __FILE__, __func__, __LINE__);
		return 0;
	}
	//����������ˮ��,��ˮ�ŵڼ�
	Read_ProtclHandl(eDevSeqid, SeqId, &U16Temp);
	oa_memcpy(buf+11, SeqId, 2);

	//У�飬����ʾͷβ��У�鱾��
	if (1 == XOR_Check(buf+1, len-3,(buf+len-2)))
	{
		OA_DEBUG_USER("(%s:%s:%d): XOR err", __FILE__, __func__, __LINE__);
		return 0;
	}

	//�Ա�ʾͷ��β֮��İ����ݽ���ת��
	JT808_dataChg(1, buf+1, len-2, &U16Temp);
	len = U16Temp+2; //total data length
	
	real_len = oa_write_buffer_force_noinit(g_soc_context.gprs_tx, buf, len);
	if (real_len < len)
	{
		OA_DEBUG_USER("(%s:%s:%d): write err", __FILE__, __func__, __LINE__);
	}
	return real_len;
}
/*********************************************************
*Function:      DevReq2ServPackag_build()
*Description:  build kinds of packets
*Return:        0:wrong others:right
*Others:         
*********************************************************/
u16 DevReq2ServPackag_build(u16 ReqMsgId) //��ʱ�ϴ�����
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
				OA_DEBUG_USER("(%s:%s:%d): data is too long!", __FILE__, __func__, __LINE__);
				return 0;
			}
			#if 0
			//put data length into pbuf[0..1]
			*pbuf = U16Temp>>8;
			*(pbuf+1) = U16Temp&0xff;
			OSQPost(QServUpdata,pbuf);
			#endif
			ret = escape_copy_to_send(pbuf, U16Temp);
			if (ret > 0)
			{
				return ret;
			}
			else
			{
				return 0;
			}
			
		}
		else
		{	
			Trace("(%s:%s:%d): build package error.", __FILE__, __func__, __LINE__);
			return 0;
		}
	}
	else
	{
		OA_DEBUG_USER("(%s:%s:%d): no empty buffer!", __FILE__, __func__, __LINE__);
		return 0;
	}
//ä��������ʱ����
//#ifdef BLINDDATA
#if 0
	else if(REPORT_LOCATION==ReqMsgId) //λ�����ݴ�flash
	{
		u8 err;
		u32 Temp;
		ReadAllBitAlarmPara(StaSector1,&Temp); //peijl_20121018 ���ӣ�ֻgps��λʱ��λ�����ݴ�ä��
		if(Temp&STA_GPS_FIXED)
		{
		Trace(PrintDebug,"DevReq2Serv Queue is full.So save position-data to Blind-position flash.\r\n");
		BlinddataBuild_save2Flash(REPORT_LOCATION);
		}
		return ActionOK;
	}
	else if(STADENTDATA_REPPORT==ReqMsgId) //λ�����ݴ�flash
	{
		u8 err;
		u32 Temp;
		Trace(PrintDebug,"DevReq2Serv Queue is full.So save student-data to flash of Blind area.\r\n");
		BlinddataBuild_save2Flash(STADENTDATA_REPPORT);
		return ActionOK;
	}
	else //�������ݶ���
	{	Trace(PrintDebug,"DevReq2Serv Queue is full.\r\n");
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
u16 DevReq2ServPackag_build_blind(u16 ReqMsgId) //��ʱ�ϴ�����
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
				OA_DEBUG_USER("(%s:%s:%d): data is too long!", __FILE__, __func__, __LINE__);
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
			return ret;
			
		}
		else{	
			Trace("(%s:%s:%d): build package error.", __FILE__, __func__, __LINE__);
			return OA_FALSE;
		}
	}
	else{
		OA_DEBUG_USER("(%s:%s:%d): no empty buffer!", __FILE__, __func__, __LINE__);
		return OA_FALSE;
	}

}