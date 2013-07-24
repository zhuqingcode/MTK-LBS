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
 *   oa_sms.h
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
#include "oa_sms.h"
#include "oa_uart.h"
#include "oa_dev_params.h"
#include "oa_use_lock.h"
#include "oa_gps.h"
#include "oa_platform.h"
#include "oa_lbs2mtk.h"
#include "oa_jt808.h"
#include "oa_hw.h"
#include "oa_sw.h"
#include "oa_debug.h"

#include <stdio.h>
#include <stdlib.h>
extern char *strtok(char s[], const char *delim);
oa_sms_context message;
oa_bool sms_enable = OA_TRUE;//
extern DEVICE_PARAMS dev_now_params;
extern uart_struct uart_contain;
extern USE_LOCK now_use_lock;
extern oa_uint8 acc_status;
extern STRUCT_RMC Pos_Inf;
extern DEV_PLAT_PARAS dev_running;
extern oa_soc_context g_soc_context;
extern oa_bool try_unlock;
oa_char *p_keyword[] = {
 HB,
 RSP_TCP,
 RSP_UDP,
 RSP_SMS,
 Retry_TCP,
 Retry_UDP,
 Retry_SMS,
 IP,
 UDPPORT,
 TCPPORT,
 Sub_IP,
 Sub_User,
 Sub_Pwd,
 TEL,
 NETTYPE,
 VPDNUSR,
 VPDNPWD,
 Rpt_strategy,
 Rpttime_unlog,
 Rpttime_sleep,
 Rpttime_alarm,
 Rpttime_def,
 Rptdis_unlog,
 Rptdis_sleep,
 Rptdis_alarm,
 Rptdis_def,
 Rptcog,
 servertel,
 resettel,
 factorysettel,
 alarmsmstel,
 monitertel,
 spclsmstel,
 swh_alarmmask,
 swh_alarmsms,
 swh_alarmphoto,
 swh_alarmphotostore,
 swh_keyalarm,
 overspeed,
 overspeedtime,
 tireddrivetime,
 daydrivetime,
 min_resttime,
 max_parktime,
 provincID,
 cityID,
 carID,
 carcolor,
 UPIP,
 UPPORT,
 UPPORT_UDP,
 UPPORT_TCP,
 UPNET,
 UPFTPUSR,
 UPFTPPWD,
 UPPROG_NAME,
 STATUS,
 STATICS,
 GPS,
 PARARST,
 UPDATE,
 SNUMS,
 VERSA,
 CLRLOG,
 AUTHEN,
 RESTART,
 DEVID,
};
oa_uint8 KEYWORDS_SIZE = sizeof(p_keyword)/4;
//unicode
oa_uint8 termID[] = {0x7e,0xc8,0x7a,0xef,0x7f,0x16,0x53,0xf7,0x0,':'};//"终端编号:"
oa_uint8 CardNum[] = {0x53,0x61,0x53,0xF7,0x0,':'};//"卡号:"
oa_uint8 wcm[] = {0x0,'W',0x0,'C',0x0,'M',0x72,0xB6,0x60,0x01,0x0,':'};//"WCM状态:"
oa_uint8 zc[] = {0x6B,0x63,0x5E,0x38};//"正常"
oa_uint8 bzc[] = {0x4E,0x0D,0x6B,0x63,0x5E,0x38};//"不正常"
oa_uint8 xhqd[] = {0x4F,0xE1,0x53,0xF7,0x5F,0x3A,0x5E,0xA6,0x0,':'};//"信号强度:"
oa_uint8 acc[] = {0x0,'A',0x0,'C',0x0,'C',0x72,0xB6,0x60,0x01,0x0,':'};//"ACC状态:"
oa_uint8 dk[] = {0x62,0x53,0x5F,0x00};//"打开"
oa_uint8 gb[] = {0x51,0x73,0x95,0xED};//"关闭"
oa_uint8 gps[] = {0x0,'G',0x0,'P',0x0,'S',0x72,0xB6,0x60,0x01,0x0,':'};//"GPS状态:"
oa_uint8 du[] = {0x65,0xAD,0x8D,0xEF};//"断路"
oa_uint8 du2[] = {0x77,0xED,0x8D,0xEF};//"短路"
oa_uint8 pt[] = {0x5E,0x73,0x53,0xF0,0x72,0xB6,0x60,0x01,0x0,':'};//"平台状态:"
oa_uint8 lj[] = {0x8F,0xDE,0x63,0xA5};//"连接"
oa_uint8 wlj[] = {0x67,0x2A,0x8F,0xDE,0x63,0xA5};//"未连接"
oa_uint8 fh[] = {0x0,';'};//";"

oa_uint8 ydw[] = {0x5D,0xF2,0x5B,0x9A,0x4F,0x4D};//"已定位"
oa_uint8 wdw[] = {0x67,0x2A,0x5B,0x9A,0x4F,0x4D};//"未定位"
oa_uint8 sj[] = {0x65,0xF6,0x95,0xF4,0x0,':'};//"时间:"
oa_uint8 wd[] = {0x7E,0xAC,0x5E,0xA6,0x0,':'};//"纬度:"
oa_uint8 jd[] = {0x7E,0xCF,0x5E,0xA6,0x0,':'};//"经度:"
oa_uint8 sd[] = {0x90,0x1F,0x5E,0xA6,0x0,':'};//"速度:"
oa_uint8 kmh[] = "(km/h)";//{0x0,'(',0x0,'k',0x0,'m',0x0,'/',0x0,'h',0x0,')'};//"(km/h)"
oa_uint8 fx[] = {0x65,0xB9,0x54,0x11,0x0,':'};//"方向:"
oa_uint8 mk[] = {0x6A,0x21,0x57,0x57,0x0,':'};//"模块:"
oa_uint8 tx[] = {0x59,0x29,0x7E,0xBF,0x0,':'};//"天线:"
//gbk
oa_uint8 termID_gbk[] = {0xD6,0xD5,0xB6,0xCB,0xB1,0xE0,0xBA,0xC5,':'};//"终端编号:"
oa_uint8 CardNum_gbk[] = {0xbf,0xa8,0xba,0xc5,':'};//"卡号:"
oa_uint8 wcm_gbk[] = {'W','C','M',0xd7,0xb4,0xcc,0xac,':'};//"WCM状态:"
oa_uint8 zc_gbk[] = {0xd5,0xfd,0xb3,0xa3};//"正常"
oa_uint8 bzc_gbk[] = {0xb2,0xbb,0xd5,0xfd,0xb3,0xa3};//"不正常"
oa_uint8 xhqd_gbk[] = {0xd0,0xc5,0xba,0xc5,0xc7,0xbf,0xb6,0xc8,':'};//"信号强度:"
oa_uint8 acc_gbk[] = {'A','C','C',0xd7,0xb4,0xcc,0xac,':'};//"ACC状态:"
oa_uint8 dk_gbk[] = {0xb4,0xf2,0xbf,0xaa};//"打开"
oa_uint8 gb_gbk[] = {0xb9,0xd8,0xb1,0xd5};//"关闭"
oa_uint8 gps_gbk[] = {'G','P','S',0xd7,0xb4,0xcc,0xac,':'};//"GPS状态:"
oa_uint8 du_gbk[] = {0xb6,0xcf,0xc2,0xb7};//"断路"
oa_uint8 du2_gbk[] = {0xb6,0xcc,0xc2,0xb7};//"短路"
oa_uint8 pt_gbk[] = {0xc6,0xbd,0xcc,0xa8,0xd7,0xb4,0xcc,0xac,':'};//"平台状态:"
oa_uint8 lj_gbk[] = {0xc1,0xac,0xbd,0xd3};//"连接"
oa_uint8 wlj_gbk[] = {0xce,0xb4,0xc1,0xac,0xbd,0xd3};//"未连接"
oa_uint8 fh_gbk[] = {';'};//";"

oa_uint8 ydw_gbk[] = {0xD2,0xd1,0xb6,0xa8,0xce,0xbb};//"已定位"
oa_uint8 wdw_gbk[] = {0xc1,0xb4,0xb6,0xa8,0xce,0xbb};//"未定位"
oa_uint8 sj_gbk[] = {0xca,0xb1,0xbc,0xe4,':'};//"时间:"
oa_uint8 wd_gbk[] = {0xce,0xb3,0xb6,0xc8,':'};//"纬度:"
oa_uint8 jd_gbk[] = {0xbe,0xad,0xb6,0xc8,':'};//"经度:"
oa_uint8 sd_gbk[] = {0xcb,0xd9,0xb6,0xc8,':'};//"速度:"
oa_uint8 kmh_gbk[] = "(km/h)";//{0x0,'(',0x0,'k',0x0,'m',0x0,'/',0x0,'h',0x0,')'};//"(km/h)"
oa_uint8 fx_gbk[] = {0xb7,0xbd,0xcf,0xf2,':'};//"方向:"
oa_uint8 mk_gbk[] = {0xc4,0xa3,0xbf,0xe9,':'};//"模块:"
oa_uint8 tx_gbk[] = {0xcc,0xec,0xcf,0xdf,':'};//"天线:"

/*********************************************************
*Function:      status_extract()
*Description:  search key word in message
*Return:        void
*Others:         
*********************************************************/
void status_extract(oa_char *enquire_temp, u8 *p_len, sms_or_uart which){
	oa_char tmp[256] = {0x0};
	oa_char tmp2[128] = {0x0};
	oa_char tmp3[8] = {0x0};
	oa_uint8 pos = 0;
	oa_uint32 len = 0;
	
	if (which == sms){
		//终端编号
		oa_memcpy(tmp, termID, sizeof(termID));pos += sizeof(termID);
		len = asc2uc(tmp2, dev_now_params.term_id, oa_strlen(dev_now_params.term_id));
		oa_memcpy(&tmp[pos], tmp2, len);pos += len;
		oa_memcpy(&tmp[pos], fh, 2);pos += 2;
		//卡号
		oa_memset(tmp2, 0x0, sizeof(tmp2));
		oa_memcpy(&tmp[pos], CardNum, sizeof(CardNum));pos += sizeof(CardNum);
		len = asc2uc(tmp2, dev_now_params.term_tel_num, oa_strlen(dev_now_params.term_tel_num));
		oa_memcpy(&tmp[pos], tmp2, len);pos += len;
		oa_memcpy(&tmp[pos], fh, 2);pos += 2;
		//WCM状态
		oa_memcpy(&tmp[pos], wcm, sizeof(wcm));pos += sizeof(wcm);
		if (oa_sim_network_is_valid()){
			oa_memcpy(&tmp[pos], zc, 4);pos += 4;
		}
		else{
			oa_memcpy(&tmp[pos], bzc, 6);pos += 6;
		}
		oa_memcpy(&tmp[pos], fh, 2);pos += 2;
		//信号强度
		oa_memset(tmp2, 0x0, sizeof(tmp2));
		oa_memcpy(&tmp[pos], xhqd, sizeof(xhqd));pos += sizeof(xhqd);
		sprintf(tmp3, "%d", oa_network_get_signal_level());
		len = asc2uc(tmp2, tmp3, oa_strlen(tmp3));
		oa_memcpy(&tmp[pos], tmp2, len);pos += len;
		oa_memcpy(&tmp[pos], fh, 2);pos += 2;
		//ACC状态
		oa_memcpy(&tmp[pos], acc, sizeof(acc));pos += sizeof(acc);
		if (acc_status == ACC_ON){
			oa_memcpy(&tmp[pos], dk, 4);pos += 4;
		}
		else{
			oa_memcpy(&tmp[pos], gb, 4);pos += 4;
		}
		oa_memcpy(&tmp[pos], fh, 2);pos += 2;
		//GPS状态
		oa_memcpy(&tmp[pos], gps, sizeof(gps));pos += sizeof(gps);
		if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ERR) == SET){
			oa_memcpy(&tmp[pos], bzc, 6);pos += 6;
		}
		else if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ERR) == RESET){
			oa_memcpy(&tmp[pos], zc, 4);pos += 4;
		}
		oa_memcpy(&tmp[pos], fh, 2);pos += 2;
		//平台状态
		oa_memcpy(&tmp[pos], pt, sizeof(pt));pos += sizeof(pt);
		if (dev_running.plat_status == ONLINE){
			oa_memcpy(&tmp[pos], lj, 4);pos += 4;
		}
		else{
			oa_memcpy(&tmp[pos], wlj, 6);pos += 6;
		}
		oa_memcpy(&tmp[pos], fh, 2);pos += 2;
		oa_memcpy(enquire_temp, tmp, pos);
		*p_len = pos; 
	}
	else if (which == scrn){
		//终端编号
		oa_memcpy(tmp, termID_gbk, sizeof(termID_gbk));pos += sizeof(termID_gbk);
		len = oa_strlen(dev_now_params.term_id);
		oa_memcpy(&tmp[pos], dev_now_params.term_id, len);pos += len;
		oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
		//卡号
		oa_memcpy(&tmp[pos], CardNum_gbk, sizeof(CardNum_gbk));pos += sizeof(CardNum_gbk);
		len = oa_strlen(dev_now_params.term_tel_num);
		oa_memcpy(&tmp[pos], dev_now_params.term_tel_num, len);pos += len;
		oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
		//WCM状态
		oa_memcpy(&tmp[pos], wcm_gbk, sizeof(wcm_gbk));pos += sizeof(wcm_gbk);
		if (oa_sim_network_is_valid()){
			oa_memcpy(&tmp[pos], zc_gbk, sizeof(zc_gbk));pos += sizeof(zc_gbk);
		}
		else{
			oa_memcpy(&tmp[pos], bzc_gbk, sizeof(bzc_gbk));pos += sizeof(bzc_gbk);
		}
		oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
		//信号强度
		oa_memset(tmp2, 0x0, sizeof(tmp2));
		oa_memcpy(&tmp[pos], xhqd_gbk, sizeof(xhqd_gbk));pos += sizeof(xhqd_gbk);
		sprintf(tmp2, "%d", oa_network_get_signal_level());
		len = oa_strlen(tmp2);
		oa_memcpy(&tmp[pos], tmp2, len);pos += len;
		oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
		//ACC状态
		oa_memcpy(&tmp[pos], acc_gbk, sizeof(acc_gbk));pos += sizeof(acc_gbk);
		if (acc_status == ACC_ON){
			oa_memcpy(&tmp[pos], dk_gbk, sizeof(dk_gbk));pos += sizeof(dk_gbk);
		}
		else{
			oa_memcpy(&tmp[pos], gb_gbk, sizeof(gb_gbk));pos += sizeof(gb_gbk);
		}
		oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
		//GPS状态
		oa_memcpy(&tmp[pos], gps_gbk, sizeof(gps_gbk));pos += sizeof(gps_gbk);
		if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ERR) == SET){
			oa_memcpy(&tmp[pos], bzc_gbk, 6);pos += 6;
		}
		else if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ERR) == RESET){
			oa_memcpy(&tmp[pos], zc_gbk, 4);pos += 4;
		}
		oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
		//平台状态
		oa_memcpy(&tmp[pos], pt_gbk, sizeof(pt_gbk));pos += sizeof(pt_gbk);
		if (dev_running.plat_status == ONLINE){
			oa_memcpy(&tmp[pos], lj_gbk, 4);pos += 4;
		}
		else{
			oa_memcpy(&tmp[pos], wlj_gbk, 6);pos += 6;
		}
		oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
		oa_memcpy(enquire_temp, tmp, pos);
		*p_len = pos; 
	}
}
/*********************************************************
*Function:      gps_extract()
*Description:  search key word in message
*Return:        void
*Others:         
*********************************************************/
void gps_extract(oa_char *enquire_temp, u8 *p_len, sms_or_uart which){
	oa_char tmp[256] = {0x0};
	oa_char tmp2[128] = {0x0};
	oa_char tmp3[128] = {0x0};
	oa_uint8 pos = 0;
	oa_uint32 len = 0;
	oa_time_struct t = {0};
	oa_uint8 i;
	if (sms == which){
		//gps状态
		oa_memcpy(tmp, gps, sizeof(gps));pos += sizeof(gps);
		oa_memcpy(&tmp[pos], ydw, 6);pos += 6;
		oa_memcpy(&tmp[pos], fh, 2);pos += 2;

		if (GPS_FIXED == Pos_Inf.Fix_Status){
		//时间
		oa_memcpy(&tmp[pos], sj, sizeof(sj));pos += sizeof(sj);
		t.nYear = ((Pos_Inf.Time[0]>>4)&0x0F)*10+ (Pos_Inf.Time[0]&0x0F);
		t.nMonth = ((Pos_Inf.Time[1]>>4)&0x0F)*10+ (Pos_Inf.Time[1]&0x0F);//-1
		t.nDay = ((Pos_Inf.Time[2]>>4)&0x0F)*10+ (Pos_Inf.Time[2]&0x0F);
		t.nHour = ((Pos_Inf.Time[3]>>4)&0x0F)*10+ (Pos_Inf.Time[3]&0x0F);
		t.nMin = ((Pos_Inf.Time[4]>>4)&0x0F)*10+ (Pos_Inf.Time[4]&0x0F);
		t.nSec = ((Pos_Inf.Time[5]>>4)&0x0F)*10+ (Pos_Inf.Time[5]&0x0F);
		sprintf(tmp3, "%d %d %d %d %d %d", t.nYear, t.nMonth, t.nDay, t.nHour, t.nMin, t.nSec);
		len = asc2uc(tmp2, tmp3, oa_strlen(tmp3));
		oa_memcpy(&tmp[pos], tmp2, len);pos += len;
		oa_memcpy(&tmp[pos], fh, 2);pos += 2;
		//纬度
		oa_memcpy(&tmp[pos], wd, sizeof(wd));pos += sizeof(wd);
		oa_memset(tmp3, 0x0, sizeof(tmp3));
		oa_strcat(tmp3, "N");
		tmp3[1] = Pos_Inf.Latitude[0];
		tmp3[2] = Pos_Inf.Latitude[1];
		tmp3[3] = '.';
		tmp3[4] = Pos_Inf.Latitude[2];
		tmp3[5] = Pos_Inf.Latitude[3];
		tmp3[6] = Pos_Inf.Latitude[4];
		tmp3[7] = Pos_Inf.Latitude[5];
		tmp3[8] = Pos_Inf.Latitude[6];
		tmp3[9] = Pos_Inf.Latitude[7];
		oa_memset(tmp2, 0x0, sizeof(tmp2));
		len = asc2uc(tmp2, tmp3, oa_strlen(tmp3));
		oa_memcpy(&tmp[pos], tmp2, len);pos += len;
		oa_memcpy(&tmp[pos], fh, 2);pos += 2;
		//经度
		oa_memcpy(&tmp[pos], jd, sizeof(jd));pos += sizeof(jd);
		oa_memset(tmp3, 0x0, sizeof(tmp3));
		oa_strcat(tmp3, "E");
		tmp3[1] = Pos_Inf.Longitude[0];
		tmp3[2] = Pos_Inf.Longitude[1];
		tmp3[3] = Pos_Inf.Longitude[2];
		tmp3[4] = '.';
		tmp3[5] = Pos_Inf.Longitude[3];
		tmp3[6] = Pos_Inf.Longitude[4];
		tmp3[7] = Pos_Inf.Longitude[5];
		tmp3[8] = Pos_Inf.Longitude[6];
		tmp3[9] = Pos_Inf.Longitude[7];
		tmp3[10] = Pos_Inf.Longitude[8];
		oa_memset(tmp2, 0x0, sizeof(tmp2));
		len = asc2uc(tmp2, tmp3, oa_strlen(tmp3));
		oa_memcpy(&tmp[pos], tmp2, len);pos += len;
		oa_memcpy(&tmp[pos], fh, 2);pos += 2;
		//速度
		oa_memcpy(&tmp[pos], sd, sizeof(sd));pos += sizeof(sd);
		oa_memset(tmp3, 0x0, sizeof(tmp3));
		sprintf(tmp3, "%03d", Pos_Inf.Speed);
		oa_strcat(tmp3, kmh);
		oa_memset(tmp2, 0x0, sizeof(tmp2));
		len = asc2uc(tmp2, tmp3, oa_strlen(tmp3));
		oa_memcpy(&tmp[pos], tmp2, len);pos += len;
		oa_memcpy(&tmp[pos], fh, 2);pos += 2;
		//方向
		oa_memcpy(&tmp[pos], fx, sizeof(fx));pos += sizeof(fx);
		oa_memset(tmp3, 0x0, sizeof(tmp3));
		sprintf(tmp3, "%03d", Pos_Inf.COG);
		oa_memset(tmp2, 0x0, sizeof(tmp2));
		len = asc2uc(tmp2, tmp3, oa_strlen(tmp3));
		oa_memcpy(&tmp[pos], tmp2, len);pos += len;
		oa_memcpy(&tmp[pos], fh, 2);pos += 2;
		oa_memcpy(enquire_temp, tmp, pos);
		*p_len = pos; 
		}
		else if (GPS_UNFIXED == Pos_Inf.Fix_Status){
			//未定位
			oa_memcpy(&tmp[pos], wdw, sizeof(wdw));pos += sizeof(wdw);
			oa_memcpy(&tmp[pos], fh, 2);pos += 2;
			//模块
			if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ERR) == SET){
				oa_memcpy(&tmp[pos], bzc, sizeof(bzc));pos += sizeof(bzc);
			}
			else if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ERR) == RESET){
				oa_memcpy(&tmp[pos], zc, sizeof(zc));pos += sizeof(zc);
			}
			oa_memcpy(&tmp[pos], fh, 2);pos += 2;
			if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ANTENNA) == SET){
				oa_memcpy(&tmp[pos], dk, sizeof(dk));pos += sizeof(dk);
			}
			else if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ANTENNA) == RESET){
				oa_memcpy(&tmp[pos], zc, sizeof(zc));pos += sizeof(zc);
			}
			oa_memcpy(&tmp[pos], fh, 2);pos += 2;
			oa_memcpy(enquire_temp, tmp, pos);
			*p_len = pos; 
		}
	}
	else if (scrn == which){
		//gps状态
		oa_memcpy(tmp, gps_gbk, sizeof(gps_gbk));pos += sizeof(gps_gbk);
		oa_memcpy(&tmp[pos], ydw_gbk, 6);pos += 6;
		oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);

		if (GPS_FIXED == Pos_Inf.Fix_Status){
		//时间
		oa_memcpy(&tmp[pos], sj_gbk, sizeof(sj_gbk));pos += sizeof(sj_gbk);
		t.nYear = ((Pos_Inf.Time[0]>>4)&0x0F)*10+ (Pos_Inf.Time[0]&0x0F);
		t.nMonth = ((Pos_Inf.Time[1]>>4)&0x0F)*10+ (Pos_Inf.Time[1]&0x0F);//-1
		t.nDay = ((Pos_Inf.Time[2]>>4)&0x0F)*10+ (Pos_Inf.Time[2]&0x0F);
		t.nHour = ((Pos_Inf.Time[3]>>4)&0x0F)*10+ (Pos_Inf.Time[3]&0x0F);
		t.nMin = ((Pos_Inf.Time[4]>>4)&0x0F)*10+ (Pos_Inf.Time[4]&0x0F);
		t.nSec = ((Pos_Inf.Time[5]>>4)&0x0F)*10+ (Pos_Inf.Time[5]&0x0F);
		sprintf(tmp3, "%d %d %d %d %d %d", t.nYear, t.nMonth, t.nDay, t.nHour, t.nMin, t.nSec);
		len = oa_strlen(tmp3);
		oa_memcpy(&tmp[pos], tmp3, len);pos += len;
		oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
		//纬度
		oa_memcpy(&tmp[pos], wd_gbk, sizeof(wd_gbk));pos += sizeof(wd_gbk);
		oa_memset(tmp3, 0x0, sizeof(tmp3));
		oa_strcat(tmp3, "N");
		tmp3[1] = Pos_Inf.Latitude[0];
		tmp3[2] = Pos_Inf.Latitude[1];
		tmp3[3] = '.';
		tmp3[4] = Pos_Inf.Latitude[2];
		tmp3[5] = Pos_Inf.Latitude[3];
		tmp3[6] = Pos_Inf.Latitude[4];
		tmp3[7] = Pos_Inf.Latitude[5];
		tmp3[8] = Pos_Inf.Latitude[6];
		tmp3[9] = Pos_Inf.Latitude[7];
		len = oa_strlen(tmp3);
		oa_memcpy(&tmp[pos], tmp3, len);pos += len;
		oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
		//经度
		oa_memcpy(&tmp[pos], jd_gbk, sizeof(jd_gbk));pos += sizeof(jd_gbk);
		oa_memset(tmp3, 0x0, sizeof(tmp3));
		oa_strcat(tmp3, "E");
		tmp3[1] = Pos_Inf.Longitude[0];
		tmp3[2] = Pos_Inf.Longitude[1];
		tmp3[3] = Pos_Inf.Longitude[2];
		tmp3[4] = '.';
		tmp3[5] = Pos_Inf.Longitude[3];
		tmp3[6] = Pos_Inf.Longitude[4];
		tmp3[7] = Pos_Inf.Longitude[5];
		tmp3[8] = Pos_Inf.Longitude[6];
		tmp3[9] = Pos_Inf.Longitude[7];
		tmp3[10] = Pos_Inf.Longitude[8];
		len = oa_strlen(tmp3);
		oa_memcpy(&tmp[pos], tmp3, len);pos += len;
		oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
		//速度
		oa_memcpy(&tmp[pos], sd_gbk, sizeof(sd_gbk));pos += sizeof(sd_gbk);
		oa_memset(tmp3, 0x0, sizeof(tmp3));
		sprintf(tmp3, "%03d", Pos_Inf.Speed);
		oa_strcat(tmp3, kmh);
		len = oa_strlen(tmp3);
		oa_memcpy(&tmp[pos], tmp3, len);pos += len;
		oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
		//方向
		oa_memcpy(&tmp[pos], fx_gbk, sizeof(fx_gbk));pos += sizeof(fx_gbk);
		oa_memset(tmp3, 0x0, sizeof(tmp3));
		sprintf(tmp3, "%03d", Pos_Inf.COG);
		len = oa_strlen(tmp3);
		oa_memcpy(&tmp[pos], tmp3, len);pos += len;
		oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
		oa_memcpy(enquire_temp, tmp, pos);
		*p_len = pos; 
		}
		else if (GPS_UNFIXED == Pos_Inf.Fix_Status){
			//未定位
			oa_memcpy(&tmp[pos], wdw_gbk, sizeof(wdw_gbk));pos += sizeof(wdw_gbk);
			oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
			//模块
			if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ERR) == SET){
				oa_memcpy(&tmp[pos], bzc_gbk, sizeof(bzc_gbk));pos += sizeof(bzc_gbk);
			}
			else if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ERR) == RESET){
				oa_memcpy(&tmp[pos], zc_gbk, sizeof(zc_gbk));pos += sizeof(zc_gbk);
			}
			oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
			if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ANTENNA) == SET){
				oa_memcpy(&tmp[pos], dk_gbk, sizeof(dk_gbk));pos += sizeof(dk_gbk);
			}
			else if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ANTENNA) == RESET){
				oa_memcpy(&tmp[pos], zc_gbk, sizeof(zc_gbk));pos += sizeof(zc_gbk);
			}
			oa_memcpy(&tmp[pos], fh_gbk, sizeof(fh_gbk));pos += sizeof(fh_gbk);
			oa_memcpy(enquire_temp, tmp, pos);
			*p_len = pos; 
		}
	}
}
/*********************************************************
*Function:      need_ack_check()
*Description:  search key word in message
*Return:        void
*Others:         
*********************************************************/
oa_bool set_enquiry_check(oa_char *p_key, oa_uint8 e_len, keyword_context *p_set, 
										e_keyword e_kind)
{
	char *p = NULL;
	oa_char *p_SEMICOLON = NULL; 
	oa_char temp[128] = {0x0};
	
	if (NULL == p_key || e_len == 0 || p_set == NULL){
		DEBUG(" err!");
		return OA_FALSE;
	}

	p = p_key+e_len;
	if (*p == COLON){//means set
		p_set->kind = set;
		p++;
		p_SEMICOLON = oa_strchr(p_key, SEMICOLON);
		if (NULL != p_SEMICOLON)	oa_memcpy(temp, p, p_SEMICOLON-p);
		else {
			DEBUG("format err!");
			return OA_FALSE;
		}//oa_memcpy(temp, p, copy_len - (e_len+1));
		switch (e_kind){
			case e_HB:
			case e_RSP_TCP:
			case e_RSP_UDP:
			case e_Retry_TCP:
			case e_Retry_UDP:
			case e_Retry_SMS:{
				if (oa_is_digit_str(temp, oa_strlen(temp))){
					p_set->context.con_int = oa_atoi(temp);
				}
				else{
					DEBUG(" paras err!");
					return OA_FALSE;
				}
			}break;
			case e_IP:
			case e_Sub_IP:
			case e_UPIP:{
				if (ip_is_valued(temp, oa_strlen(temp))){
					oa_memcpy(p_set->context.con_ch, temp, oa_strlen(temp));
				}
				else{
					DEBUG(" paras err!");
					return OA_FALSE;
				}
			}break;
			case e_TCPPORT:
			case e_UDPPORT:
			case e_UPPORT:{
				if (oa_is_digit_str(temp, oa_strlen(temp))){
					u32 tmp = oa_atoi(temp);
					if (tmp <= PORT_MAX){
						p_set->context.con_int = tmp;
					}
					else{
						DEBUG(" paras err!");
						return OA_FALSE;
					}
				}
				else{
					DEBUG(" format err!");
					return OA_FALSE;
				}
			}break;
			case e_TEL:
			case e_resettel:
			case e_factorysettel:
			case e_servertel:
			case e_alarmsmstel:{
				if (oa_is_phone_addr_str(temp, oa_strlen(temp))){
					if (oa_strlen(temp) < TEL_NUM_MAX_LEN)	oa_memcpy(p_set->context.con_ch, temp, oa_strlen(temp));
					else{
						DEBUG(" paras err!");
						return OA_FALSE;
					}
				}
				else{
					DEBUG(" paras err!");
					return OA_FALSE;
				}
			}break;
			case e_Rpt_strategy:{
				if (oa_is_digit_str(temp, oa_strlen(temp))){
					u32 tmp;
					tmp = oa_atoi(temp);
					if (tmp < Rpt_strategy_max_num)	p_set->context.con_int = tmp;
					else{
						DEBUG(" paras err!");
						return OA_FALSE;
					}
				}
				else{
					DEBUG(" paras err!");
					return OA_FALSE;
				}
			}break;
			case e_Rpttime_unlog:
			case e_Rpttime_sleep:
			case e_Rpttime_alarm:
			case e_Rptdis_alarm:
			case e_Rptdis_def:
			case e_Rpttime_def:
			case e_Rptdis_unlog:
			case e_Rptdis_sleep:
			case e_Rptcog:
			case e_overspeed:
			case e_overspeedtime:
			case e_min_resttime:
			case e_daydrivetime:
			case e_tireddrivetime:
			case e_max_parktime:{
				if (oa_is_digit_str(temp, oa_strlen(temp))){
					p_set->context.con_int = oa_atoi(temp);
					if (e_kind == e_Rptcog){
						if (p_set->context.con_int >= 180){
							DEBUG(" paras err!");
							return OA_FALSE;
						}
					}
				}
				else{
					DEBUG(" paras err!");
					return OA_FALSE;
				}
			}break;
			case e_swh_alarmsms:
			case e_swh_alarmmask:{
				if (digit01_string_check(temp, oa_strlen(temp))){
					if (oa_strlen(temp) <= ALARMBIT_MAX_LEN){
						//DEBUG(" temp:%s!", temp);
						p_set->context.con_int = atobi(temp, oa_strlen(temp));
						DEBUG(" p_set->context.con_int:%u!", p_set->context.con_int);
					}	
					else{
						DEBUG(" paras err!");
						return OA_FALSE;
					}
				}
				else{
					DEBUG(" paras err!");
					return OA_FALSE;
				}
			}break;
			case e_provincID:
			case e_cityID:{
				if (oa_is_digit_str(temp, oa_strlen(temp))){
					p_set->context.con_int = oa_atoi(temp);
					if (p_set->context.con_int > 65535){
						DEBUG(" paras err!");
						return OA_FALSE;
					}
				}
				else{
					DEBUG(" paras err!");
					return OA_FALSE;
				}
			}break;
			case e_carID:{
				if ((oa_strlen(temp) < VEHICLE_LICENCE_MAX_LEN) && digit_alpha_check(temp, oa_strlen(temp))){
					oa_memcpy(p_set->context.con_ch, temp, oa_strlen(temp));
				}
				else{
					DEBUG(" paras err!");
					return OA_FALSE;
				}
			}break;
			case e_carcolor:{
				if (oa_is_digit_str(temp, oa_strlen(temp))){
					p_set->context.con_int = oa_atoi(temp);
					if (p_set->context.con_int > 4){
						DEBUG(" paras err!");
						return OA_FALSE;
					}
				}
			}break;
			case e_UPFTPUSR:
			case e_UPFTPPWD:{
				if (digit_alpha_check(temp, oa_strlen(temp))){
					if (oa_strlen(temp) < FTP_MAX_LEN)		oa_memcpy(p_set->context.con_ch, temp, oa_strlen(temp));
					else{
						DEBUG(" paras err!");
						return OA_FALSE;
					}
				}
				else{
					DEBUG(" paras err!");
					return OA_FALSE;
				}
			}break;
			case e_UPPROG_NAME:{
				if (oa_strlen(temp) < FTP_MAX_LEN)		oa_memcpy(p_set->context.con_ch, temp, oa_strlen(temp));
				else{
					DEBUG(" paras err!");
					return OA_FALSE;
				}
			}break;
			case e_AUTHEN:{
				if (oa_strlen(temp) > 0 && oa_strlen(temp) <= AUTHEN_CODE_MAX_LEN){
					oa_memcpy(p_set->context.con_ch, temp, oa_strlen(temp));
				}
				else if (0 == oa_strlen(temp))	p_set->context.con_ch[0] = 0xff;
			}break;
			case e_DEVID:{
				if (oa_strlen(temp) == DEVID_LEN){
					if (cap_digit_alpha_check(temp, oa_strlen(temp)))	oa_memcpy(p_set->context.con_ch, temp, DEVID_LEN);
					else{
						DEBUG(" paras err!");
						return OA_FALSE;
					}
				}
				else{
					DEBUG(" paras err!");
					return OA_FALSE;
				}
			}break;
			default:{
				DEBUG(" e_kind:%d", e_kind);
				DEBUG(" not support!");
				return OA_FALSE;
			}break;
		}
		
	}
	else if (*p != SEMICOLON){//means enquiry
		return OA_FALSE;
	}
	return OA_TRUE;
}
/*********************************************************
*Function:      lookfor_keywords_loop()
*Description:  search key word in message
*Return:        void
*Others:         
*********************************************************/
e_keyword look4keywords4ms(oa_char *p_sms, u16 sms_len, keyword_context *p_set, oa_uint8 e_i, sms_or_uart which)
{
	oa_char *p_key = NULL;
	oa_char temp[256] = {0x0};
	oa_char ch;
	ack_kind ack_ret;
	oa_uint8 e_len;
	oa_bool ret;
//	oa_uint8 e_i;
	
	if (NULL == p_set || e_i == e_none){
		DEBUG(" p_set err!");
		return e_none;
	}
	
	if (sms == which){
		if (NULL == p_set || e_i == e_none){
			DEBUG(" p_set err!");
			return e_none;
		}
		oa_memcpy(temp, p_sms, sms_len);
		p_key = oa_strstr(temp, p_keyword[e_i]);
	}
	else if (uart == which){
		if (NULL == p_set || e_i == e_none){
			DEBUG(" p_set err!");
			return e_none;
		}
		p_key = oa_strstr(uart_contain.buf, p_keyword[e_i]);
	}
	else if (scrn == which){
		if (NULL == p_set || NULL == p_sms || sms_len > 255){
			DEBUG(" p_set err!");
			return e_none;
		}
		oa_memcpy(temp, p_sms, sms_len);
		p_key = oa_strstr(temp, p_keyword[e_i]);
	}
	
	if (NULL != p_key){
		if (sms == which){
			if (p_key != &temp[0])		return e_none;
		}
		else if (uart == which){
			if (p_key != &uart_contain.buf[0])		return e_none;
		}
		else if (scrn == which){
			if (p_key != &temp[0])		return e_none;
		}
		
		//set/enquiry check
		e_len = oa_strlen(p_keyword[e_i]);
		ret = set_enquiry_check(p_key, e_len, p_set, e_i);
		if (OA_FALSE == ret) return e_none;
		else return e_i;
	}


	return e_none;
}
/*********************************************************
*Function:      handle_keyword4ms()
*Description:  handle the keyword for multiple sms
*Return:        void
*Others:         
*********************************************************/
void handle_common4ms(e_keyword key_kind, oa_char *buf, u8 *len, sms_or_uart which)
{
	u8 ret_len;
	char temp[16] = {0x0};
	char enquire_temp[256] = {0x0};
	
	switch(key_kind){
		case e_HB:{
			sprintf(enquire_temp, "Hearttime:%d;"/*very important here*/, dev_now_params.heartbeat_interval);
		}break;
		case e_RSP_TCP:{
			sprintf(enquire_temp, "RSP_TCP:%d;"/*very important here*/, dev_now_params.tcp_ack_timeout);
		}break;
		case e_RSP_UDP:{
			sprintf(enquire_temp, "RSP_UDP:%d;"/*very important here*/, dev_now_params.udp_ack_timeout);
		}break;
		case e_RSP_SMS:{
			sprintf(enquire_temp, "RSP_SMS:%d;"/*very important here*/, dev_now_params.sms_ack_timeout);
		}break;
		case e_Retry_TCP:{
			sprintf(enquire_temp, "Retry_TCP:%d;"/*very important here*/, dev_now_params.tcp_retrans_times);
		}break;
		case e_Retry_UDP:{
			sprintf(enquire_temp, "Retry_UDP:%d;"/*very important here*/, dev_now_params.udp_retrans_times);
		}break;
		case e_Retry_SMS:{
			sprintf(enquire_temp, "Retry_SMS:%d;"/*very important here*/, dev_now_params.sms_retrans_times);
		}break;
		case e_IP:{
			oa_strcat(enquire_temp, "IP:");
			oa_strcat(enquire_temp, dev_now_params.m_server_ip);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_TCPPORT:{
			sprintf(enquire_temp, "TCPPORT:%d;"/*very important here*/, dev_now_params.server_tcp_port);
		}break;
		case e_UDPPORT:{
			sprintf(enquire_temp, "UDPPORT:%d;"/*very important here*/, dev_now_params.server_udp_port);
		}break;
		case e_TEL:{
			oa_strcat(enquire_temp, "TEL:");
			oa_strcat(enquire_temp, dev_now_params.term_tel_num);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_NETTYPE:{
			if (g_soc_context.soc_addr.sock_type == OA_SOCK_STREAM) oa_strcat(enquire_temp, "NETTYPE:TCP;");
			else oa_strcat(enquire_temp, "NETTYPE:UDP;");
		}break;
		case e_Rpt_strategy:{
			sprintf(enquire_temp, "Rpt_strategy:%d;", dev_now_params.report_strategy);
		}break;
		case e_Rpttime_unlog:{
			sprintf(enquire_temp, "Rpttime_unlog:%d;", dev_now_params.unlogin_reporttime);
		}break;
		case e_Rpttime_sleep:{
			sprintf(enquire_temp, "Rpttime_sleep:%d;", dev_now_params.sleep_reporttime);
		}break;
		case e_Rpttime_alarm:{
			sprintf(enquire_temp, "Rpttime_alarm:%d;", dev_now_params.urgent_reporttime);
		}break;
		case e_Rpttime_def:{
			sprintf(enquire_temp, "Rpttime_def:%d;", dev_now_params.default_reporttime);
		}break;
		case e_Rptdis_unlog:{
			sprintf(enquire_temp, "Rptdis_unlog:%d;", dev_now_params.unlogin_reportdistance);
		}break;
		case e_Rptdis_sleep:{
			sprintf(enquire_temp, "Rptdis_sleep:%d;", dev_now_params.sleep_reportdistance);
		}break;
		case e_Rptdis_def:{
			sprintf(enquire_temp, "Rptdis_def:%d;", dev_now_params.default_reportdistance);
		}break;
		case e_Rptdis_alarm:{
			sprintf(enquire_temp, "Rptdis_alarm:%d;", dev_now_params.urgent_reportdistance);
		}break;
		case e_Rptcog:{
			sprintf(enquire_temp, "Rptcog:%d;", dev_now_params.corner_reportangle);
		}break;
		case e_servertel:{
			oa_strcat(enquire_temp, "servertel:");
			oa_strcat(enquire_temp, dev_now_params.monitor_platform_num);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_resettel:{
			oa_strcat(enquire_temp, "resettel:");
			oa_strcat(enquire_temp, dev_now_params.reset_num);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_factorysettel:{
			oa_strcat(enquire_temp, "factorysettel:");
			oa_strcat(enquire_temp, dev_now_params.restore_factory_settings_num);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_alarmsmstel:{
			oa_strcat(enquire_temp, "alarmsmstel:");
			oa_strcat(enquire_temp, dev_now_params.terminal_sms_num);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_swh_alarmmask:{
			oa_char tmp[33] = {0x0};
			oa_myitoa(dev_now_params.alarm_mask, tmp);
			oa_strcat(enquire_temp, "swh_alarmmask:");
			oa_strcat(enquire_temp, tmp);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_swh_alarmsms:{
			oa_char tmp[33] = {0x0};
			oa_myitoa(dev_now_params.alarm_send_sms_mask, tmp);
			//DEBUG(" temp:%s!", tmp);
			oa_strcat(enquire_temp, "swh_alarmsms:");
			oa_strcat(enquire_temp, tmp);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_overspeed:{
			sprintf(enquire_temp, "overspeed:%d;", dev_now_params.max_speed);
		}break;
		case e_overspeedtime:{
			sprintf(enquire_temp, "overspeedtime:%d;", dev_now_params.speed_duration);
		}break;
		case e_min_resttime:{
			sprintf(enquire_temp, "min_resttime:%d;", dev_now_params.min_rest_time);
		}break;
		case e_max_parktime:{
			sprintf(enquire_temp, "max_parktime:%d;", dev_now_params.max_park_time);
		}break;
		case e_daydrivetime:{
			sprintf(enquire_temp, "daydrivetime:%d;", dev_now_params.day_add_drive_time_threshold);
		}break;
		case e_tireddrivetime:{
			sprintf(enquire_temp, "tireddrivetime:%d;", dev_now_params.continuous_drive_time_threshold);
		}break;
		case e_provincID:{
			sprintf(enquire_temp, "provincID:%d;", dev_now_params.vehicle_province_id);
		}break;
		case e_cityID:{
			sprintf(enquire_temp, "cityID:%d;", dev_now_params.vehicle_city_id);
		}break;
		case e_carID:{
			oa_strcat(enquire_temp, "carID:");
			oa_strcat(enquire_temp, dev_now_params.vehicle_license);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_carcolor:{
			sprintf(enquire_temp, "carcolor:%d;", dev_now_params.plate_color);
		}break;
		case e_UPIP:{
			oa_strcat(enquire_temp, "UPIP:");
			oa_strcat(enquire_temp, dev_now_params.update_server_ip);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_UPPORT:{
			sprintf(enquire_temp, "UPPORT:%d;", dev_now_params.update_server_port);
		}break;
		case e_UPFTPUSR:{
			oa_strcat(enquire_temp, "UPFTPUSR:");
			oa_strcat(enquire_temp, dev_now_params.ftpusr);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_UPFTPPWD:{
			oa_strcat(enquire_temp, "UPFTPPWD:");
			oa_strcat(enquire_temp, dev_now_params.ftppwd);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_UPPROG_NAME:{
			oa_strcat(enquire_temp, "UPPROG_NAME:");
			oa_strcat(enquire_temp, dev_now_params.ftp_prog_name);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_STATUS:{
			status_extract(enquire_temp, &ret_len, which);
			//oa_strcat(enquire_temp, ";");
		}break;
		case e_GPS:{
			gps_extract(enquire_temp, &ret_len, which);
			//oa_strcat(enquire_temp, ";");
		}break;
		case e_PARARST:{
			oa_strcat(enquire_temp, "PARARST OK;");
		}break;
		case e_UPDATE:{
			oa_strcat(enquire_temp, "doing update;");
		}break;
		case e_SNUMS:{
			oa_strcat(enquire_temp, "SNUMS:");
			oa_strcat(enquire_temp, dev_now_params.term_id);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_VERSA:{
			oa_strcat(temp, "HW,");
			oa_strcat(temp, OA_HW_VERSION_NO);
			oa_strcat(temp, ";");
			oa_strcat(enquire_temp, temp);
			oa_strcat(enquire_temp, "SW,");
			oa_strcat(enquire_temp, OA_SW_VERSION_NO);
			oa_strcat(enquire_temp, ";");
		}break;
		case e_CLRLOG:{
			oa_strcat(enquire_temp, "CLRLOG OK;");
		}break;
		case e_AUTHEN:{
			oa_uint8 code[AUTHEN_CODE_MAX_LEN] = 0x0;
			oa_uint8 len;
			if (read_authen_code(code, &len)){
				oa_strcat(enquire_temp, code);
				oa_strcat(enquire_temp, ";");
			}
		}break;	
		case e_RESTART:{
			oa_strcat(enquire_temp, "RESTART OK;");
		}break;
		case e_DEVID:{
			oa_strcat(enquire_temp, "DEVID:");
			oa_strcat(enquire_temp, dev_now_params.term_id);
			oa_strcat(enquire_temp, ";");
		}break;
		default:{
			oa_strcat(enquire_temp, "not support!");
		}break;
	}
	if (key_kind == e_STATUS || key_kind == e_GPS){
		oa_memcpy(buf, enquire_temp, ret_len);
		*len = ret_len;
	}
	else{
		*len = oa_strlen(enquire_temp);
		oa_memcpy(buf, enquire_temp, *len);
	}
}
/*********************************************************
*Function:      handle_keyword4ms()
*Description:  handle the keyword for mutiple sms
*Return:        void
*Others:         
*********************************************************/
void sendsms4ms(u8 *buf, u16 len, sms_kind s_k){
	DEBUG("send sms");
	if (len == 0){
		DEBUG("len err!");
		return;
	}
	if (s_k == sms_normal)	oa_sms_test_dfalp(buf, message.deliver_num);
	else if(s_k == sms_special) oa_sms_test_ucs2(buf, message.deliver_num, len);
}
/*********************************************************
*Function:      dev_action_handle()
*Description:  maybe device need do something
*Return:        void
*Others:         
*********************************************************/
void dev_action_handle(keyword_context *p_set, sms_or_uart which)
{
	oa_bool ret;
	if (NULL == p_set){
		DEBUG(" p_set err!");
		return;
	}
	
	switch (p_set->act_kind){
		case para_save:{
			ret = dev_params_save();
			if (ret == OA_TRUE)	print_key_dev_params();
		}break;
		case reconn:{
			ret = dev_params_save();
			if (ret == OA_TRUE)	print_key_dev_params();
			if (!use_is_lock()) do_soc_reconn();
		}break;
		case rereg:{
			ret = dev_params_save();
			if (ret == OA_TRUE)	print_key_dev_params();
			if (!use_is_lock()) do_rereg();
		}break;
		case update:{
			ftp_update(NULL);
		}break;
		case reset:{
			set_reset_flag(which, "RESTART OK;");
			do_reset();
		}break;
		case clr_log:{
			clear_miles();
			del_blinddata();
			del_areadata();
		}break;
		case clr_authcode:{
			ret = del_authcode();
			if (OA_TRUE == ret){
				DEBUG("delete authen ok");
				if (OFFLINE == dev_running.plat_status){
					dev_running.next_step = PLAT_DEV_REG;
					dev_running.plat_switch = OA_TRUE;
				}		
				else if (ONLINE == dev_running.plat_status){//if you delete authcode, you must unregsiter & reg again
					dev_running.next_step = PLAT_DEV_UNREG;
					dev_running.plat_switch = OA_TRUE;
				}
			}
		}break;
		case update_authcode:{
			ret = save_authen_code((oa_uint8 *)p_set->context.con_ch, 
										oa_strlen(p_set->context.con_ch));
			if (OA_TRUE == ret){//if you update authcode, you must unregsiter & reg again
				if (ONLINE == dev_running.plat_status){
					dev_running.next_step = PLAT_DEV_UNREG;
					dev_running.plat_switch = OA_TRUE;
				}
				else if (OFFLINE == dev_running.plat_status){
					dev_running.next_step = PLAT_DEV_LOGIN;
					dev_running.plat_switch = OA_TRUE;
				}
				
			}
		}break;
		case fac_set:{
			set_reset_flag(which, "PARARST OK;");
			do_factory_set();
		}break;
		default:break;
	}
#if 0	
	if (para_save == p_set->act_kind){
		ret = dev_params_save();
		if (ret == OA_TRUE)	print_key_dev_params();
	}
	else if (reconn == p_set->act_kind){
		ret = dev_params_save();
		if (ret == OA_TRUE)	print_key_dev_params();
		do_soc_reconn();
	}
	else if (update == p_set->act_kind){
		ftp_update();
	}
	else if (reset == p_set->act_kind){
		do_reset();		
	}
	else if (clr_log == p_set->act_kind){
		clear_miles();
		del_blinddata();
	}
	else if (clr_authcode == p_set->act_kind){
		oa_bool ret = del_authcode();
		if (OA_TRUE == ret){//if you delete authcode, you must unregsiter & reg again
			dev_running.next_step = PLAT_DEV_UNREG;
			dev_running.plat_switch = OA_TRUE;
		}
	}
	else if (update_authcode == p_set->act_kind){//maybe useless
		oa_bool ret = save_authen_code((oa_uint8 *)p_set->context.con_ch, 
										oa_strlen(p_set->context.con_ch));
		if (OA_TRUE == ret){//if you update authcode, you must unregsiter & reg again
			dev_running.next_step = PLAT_DEV_UNREG;
			dev_running.plat_switch = OA_TRUE;
		}
	}
#endif	

	p_set->act_kind = no_act;
}
/*********************************************************
*Function:      handle_keyword4ms()
*Description:  handle the keyword for mutiple sms
*Return:        void
*Others:         
*********************************************************/
void handle_keyword4ms(e_keyword key_kind, 
										keyword_context *p_set)
{
	oa_bool ret;
	char temp[16] = {0x0};
	char enquire_temp[64] = {0x0};
	switch (key_kind){
		case e_HB:{
			if (p_set->kind == set)	{
				if (dev_now_params.heartbeat_interval == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.heartbeat_interval = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_RSP_TCP:{
			if (p_set->kind == set)	{
				if (dev_now_params.tcp_ack_timeout == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.tcp_ack_timeout = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_RSP_UDP:{
			if (p_set->kind == set)	{
				if (dev_now_params.udp_ack_timeout == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.udp_ack_timeout = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_RSP_SMS:{
			if (p_set->kind == set)	{
				if (dev_now_params.sms_ack_timeout == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.sms_ack_timeout = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Retry_TCP:{
			if (p_set->kind == set)	{
				if (dev_now_params.tcp_retrans_times == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.tcp_retrans_times = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Retry_UDP:{
			if (p_set->kind == set)	{
				if (dev_now_params.udp_retrans_times == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.udp_retrans_times = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Retry_SMS:{
			if (p_set->kind == set)	{
				if (dev_now_params.sms_retrans_times == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.sms_retrans_times = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_IP:{
			if (p_set->kind == set){
				u8 ip_len;
				if (oa_strlen(dev_now_params.m_server_ip) == oa_strlen(p_set->context.con_ch)){//length is equal
					ip_len = oa_strlen(dev_now_params.m_server_ip);
					if (!oa_strncmp(dev_now_params.m_server_ip, p_set->context.con_ch, ip_len)){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.m_server_ip, 0x0, sizeof(dev_now_params.m_server_ip));
						oa_memcpy(dev_now_params.m_server_ip, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = reconn;
					}
				}
				else{//not equal
					//DEBUG(" oa_strlen(p_set->context.con_ch):%d!", oa_strlen(p_set->context.con_ch));
					oa_memset(dev_now_params.m_server_ip, 0x0, sizeof(dev_now_params.m_server_ip));
					oa_memcpy(dev_now_params.m_server_ip, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					//DEBUG(" m_server_ip:%s!", dev_now_params.m_server_ip);
					p_set->act_kind = reconn;
				}
			}		
		}break;
		case e_TCPPORT:{
			if (p_set->kind == set)	{
				if (dev_now_params.server_tcp_port == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.server_tcp_port = p_set->context.con_int;
					if (g_soc_context.soc_addr.sock_type == OA_SOCK_STREAM) p_set->act_kind = reconn;
					else p_set->act_kind = para_save;
				}
			}	
		}break;
		case e_UDPPORT:{
			if (p_set->kind == set)	{
				if (dev_now_params.server_udp_port == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.server_udp_port = p_set->context.con_int;
					if (g_soc_context.soc_addr.sock_type == OA_SOCK_DGRAM) p_set->act_kind = reconn;
					else p_set->act_kind = para_save;
				}
			}	
		}break;
		case e_TEL:{
			if (p_set->kind == set){
				if (oa_strlen(dev_now_params.term_tel_num) == oa_strlen(p_set->context.con_ch)){
					if (!oa_strncmp(dev_now_params.term_tel_num, p_set->context.con_ch, oa_strlen(dev_now_params.term_tel_num))){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.term_tel_num, 0x0, sizeof(dev_now_params.term_tel_num));
						oa_memcpy(dev_now_params.term_tel_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = rereg;
					}
				}
				else{
					oa_memset(dev_now_params.term_tel_num, 0x0, sizeof(dev_now_params.term_tel_num));
					oa_memcpy(dev_now_params.term_tel_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					p_set->act_kind = rereg;
				}
			}
		}break;
		case e_Rpt_strategy:{
			if (p_set->kind == set)	{
				if (dev_now_params.report_strategy == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.report_strategy = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
				
			}
		}break;
		case e_Rpttime_unlog:{
			if (p_set->kind == set)	{
				if (dev_now_params.unlogin_reporttime == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.unlogin_reporttime = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Rpttime_sleep:{
			if (p_set->kind == set)	{
				if (dev_now_params.sleep_reporttime == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.sleep_reporttime = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Rpttime_alarm:{
			if (p_set->kind == set)	{
				if (dev_now_params.urgent_reporttime == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.urgent_reporttime = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Rpttime_def:{
			if (p_set->kind == set){
				if (dev_now_params.default_reporttime == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.default_reporttime = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Rptdis_unlog:{
			if (p_set->kind == set){
				if (dev_now_params.unlogin_reportdistance == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.unlogin_reportdistance = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Rptdis_sleep:{
			if (p_set->kind == set){
				if (dev_now_params.sleep_reportdistance == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.sleep_reportdistance = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Rptdis_def:{
			if (p_set->kind == set){
				if (dev_now_params.default_reportdistance == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.default_reportdistance = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Rptdis_alarm:{
			if (p_set->kind == set){
				if (dev_now_params.urgent_reportdistance == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.urgent_reportdistance = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_Rptcog:{
			if (p_set->kind == set){
				if (dev_now_params.corner_reportangle == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.corner_reportangle = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_servertel:{
			if (p_set->kind == set){
				if (oa_strlen(dev_now_params.monitor_platform_num) == oa_strlen(p_set->context.con_ch)){
					if (!oa_strncmp(dev_now_params.monitor_platform_num, p_set->context.con_ch, oa_strlen(dev_now_params.monitor_platform_num))){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.monitor_platform_num, 0x0, sizeof(dev_now_params.monitor_platform_num));
						oa_memcpy(dev_now_params.monitor_platform_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					oa_memset(dev_now_params.monitor_platform_num, 0x0, sizeof(dev_now_params.monitor_platform_num));
					oa_memcpy(dev_now_params.monitor_platform_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					p_set->act_kind = para_save;
					
				}
			}
		}break;
		case e_resettel:{
			if (p_set->kind == set){
				if (oa_strlen(dev_now_params.reset_num) == oa_strlen(p_set->context.con_ch)){
					if (!oa_strncmp(dev_now_params.reset_num, p_set->context.con_ch, oa_strlen(dev_now_params.reset_num))){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;

					}
					else{//not equal
						oa_memset(dev_now_params.reset_num, 0x0, sizeof(dev_now_params.reset_num));
						oa_memcpy(dev_now_params.reset_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					oa_memset(dev_now_params.reset_num, 0x0, sizeof(dev_now_params.reset_num));
					oa_memcpy(dev_now_params.reset_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					p_set->act_kind = para_save;
					
				}
			}
		}break;
		case e_factorysettel:{
			if (p_set->kind == set){
				if (oa_strlen(dev_now_params.restore_factory_settings_num) == oa_strlen(p_set->context.con_ch)){
					if (!oa_strncmp(dev_now_params.restore_factory_settings_num, p_set->context.con_ch, oa_strlen(dev_now_params.restore_factory_settings_num))){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;

					}
					else{//not equal
						oa_memset(dev_now_params.restore_factory_settings_num, 0x0, sizeof(dev_now_params.restore_factory_settings_num));
						oa_memcpy(dev_now_params.restore_factory_settings_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					oa_memset(dev_now_params.restore_factory_settings_num, 0x0, sizeof(dev_now_params.restore_factory_settings_num));
					oa_memcpy(dev_now_params.restore_factory_settings_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					p_set->act_kind = para_save;

				}
			}
			
		}break;
		case e_alarmsmstel:{
			if (p_set->kind == set){
				if (oa_strlen(dev_now_params.terminal_sms_num) == oa_strlen(p_set->context.con_ch)){
					if (!oa_strncmp(dev_now_params.terminal_sms_num, p_set->context.con_ch, oa_strlen(dev_now_params.terminal_sms_num))){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;

					}
					else{//not equal
						oa_memset(dev_now_params.terminal_sms_num, 0x0, sizeof(dev_now_params.terminal_sms_num));
						oa_memcpy(dev_now_params.terminal_sms_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					oa_memset(dev_now_params.terminal_sms_num, 0x0, sizeof(dev_now_params.terminal_sms_num));
					oa_memcpy(dev_now_params.terminal_sms_num, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					p_set->act_kind = para_save;
					
				}
			}
		}break;
		case e_swh_alarmmask:{
			if (p_set->kind == set){
				if (dev_now_params.alarm_mask == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{//not equal
					dev_now_params.alarm_mask = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_swh_alarmsms:{
			if (p_set->kind == set){
				if (dev_now_params.alarm_send_sms_mask == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{//not equal
					dev_now_params.alarm_send_sms_mask = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_overspeed:{
			if (p_set->kind == set)	{
				if (dev_now_params.max_speed == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.max_speed = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_overspeedtime:{
			if (p_set->kind == set)	{
				if (dev_now_params.speed_duration == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.speed_duration = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_daydrivetime:{
			if (p_set->kind == set)	{
				if (dev_now_params.day_add_drive_time_threshold == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.day_add_drive_time_threshold = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_tireddrivetime:{
			if (p_set->kind == set)	{
				if (dev_now_params.continuous_drive_time_threshold == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.continuous_drive_time_threshold = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_min_resttime:{
			if (p_set->kind == set){
				if (dev_now_params.min_rest_time == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.min_rest_time = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_max_parktime:{
			if (p_set->kind == set){
				if (dev_now_params.max_park_time == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.max_park_time = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_provincID:{
			if (p_set->kind == set)	{
				if (dev_now_params.vehicle_province_id == (u16)p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.vehicle_province_id = (u16)p_set->context.con_int;
					p_set->act_kind = rereg;
				}
			}
		}break;
		case e_cityID:{
			if (p_set->kind == set){
				if (dev_now_params.vehicle_city_id == (u16)p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.vehicle_city_id = (u16)p_set->context.con_int;
					p_set->act_kind = rereg;
				}
			}
		}break;
		case e_carID:{
			if (p_set->kind == set){
				if (oa_strlen(dev_now_params.vehicle_license) == oa_strlen(p_set->context.con_ch)){
					if (!oa_strncmp(dev_now_params.vehicle_license, p_set->context.con_ch, oa_strlen(dev_now_params.vehicle_license))){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;

					}
					else{//not equal
						oa_memset(dev_now_params.vehicle_license, 0x0, sizeof(dev_now_params.vehicle_license));
						oa_memcpy(dev_now_params.vehicle_license, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = rereg;
					}
				}
				else{//not equal
						oa_memset(dev_now_params.vehicle_license, 0x0, sizeof(dev_now_params.vehicle_license));
						oa_memcpy(dev_now_params.vehicle_license, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = rereg;
				}
				
			}
		}break;
		case e_carcolor:{
			if (p_set->kind == set){
				if (dev_now_params.plate_color == (u8)p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;
				}
				else{
					dev_now_params.plate_color = (u8)p_set->context.con_int;
					p_set->act_kind = rereg;
				}
			}
		}break;
		case e_UPIP:{
			if (p_set->kind == set){
				u8 ip_len;
				if (oa_strlen(dev_now_params.update_server_ip) == oa_strlen(p_set->context.con_ch)){//length is equal
					ip_len = oa_strlen(dev_now_params.update_server_ip);
					if (!oa_strncmp(dev_now_params.update_server_ip, p_set->context.con_ch, ip_len)){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.update_server_ip, 0x0, sizeof(dev_now_params.update_server_ip));
						oa_memcpy(dev_now_params.update_server_ip, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					//DEBUG(" oa_strlen(p_set->context.con_ch):%d!", oa_strlen(p_set->context.con_ch));
					oa_memset(dev_now_params.update_server_ip, 0x0, sizeof(dev_now_params.update_server_ip));
					oa_memcpy(dev_now_params.update_server_ip, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					//DEBUG(" m_server_ip:%s!", dev_now_params.m_server_ip);
					p_set->act_kind = para_save;
				}
			}	
		}break;
		case e_UPPORT:{
			if (p_set->kind == set)	{
				if (dev_now_params.update_server_port == p_set->context.con_int){
					PRINT_SAMEPARA;
					p_set->act_kind = no_act;
					break;

				}
				else{
					dev_now_params.update_server_port = p_set->context.con_int;
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_UPFTPUSR:{
			if (p_set->kind == set){
				u8 ip_len;
				if (oa_strlen(dev_now_params.ftpusr) == oa_strlen(p_set->context.con_ch)){//length is equal
					ip_len = oa_strlen(dev_now_params.ftpusr);
					if (!oa_strncmp(dev_now_params.ftpusr, p_set->context.con_ch, ip_len)){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.ftpusr, 0x0, sizeof(dev_now_params.ftpusr));
						oa_memcpy(dev_now_params.ftpusr, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					//DEBUG(" oa_strlen(p_set->context.con_ch):%d!", oa_strlen(p_set->context.con_ch));
					oa_memset(dev_now_params.ftpusr, 0x0, sizeof(dev_now_params.ftpusr));
					oa_memcpy(dev_now_params.ftpusr, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					//DEBUG(" m_server_ip:%s!", dev_now_params.m_server_ip);
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_UPFTPPWD:{
			if (p_set->kind == set){
				u8 ip_len;
				if (oa_strlen(dev_now_params.ftppwd) == oa_strlen(p_set->context.con_ch)){//length is equal
					ip_len = oa_strlen(dev_now_params.ftppwd);
					if (!oa_strncmp(dev_now_params.ftppwd, p_set->context.con_ch, ip_len)){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.ftppwd, 0x0, sizeof(dev_now_params.ftpusr));
						oa_memcpy(dev_now_params.ftppwd, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					//DEBUG(" oa_strlen(p_set->context.con_ch):%d!", oa_strlen(p_set->context.con_ch));
					oa_memset(dev_now_params.ftppwd, 0x0, sizeof(dev_now_params.ftppwd));
					oa_memcpy(dev_now_params.ftppwd, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					//DEBUG(" m_server_ip:%s!", dev_now_params.m_server_ip);
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_UPPROG_NAME:{
			if (p_set->kind == set){
				u8 ip_len;
				if (oa_strlen(dev_now_params.ftp_prog_name) == oa_strlen(p_set->context.con_ch)){//length is equal
					ip_len = oa_strlen(dev_now_params.ftp_prog_name);
					if (!oa_strncmp(dev_now_params.ftp_prog_name, p_set->context.con_ch, ip_len)){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.ftp_prog_name, 0x0, sizeof(dev_now_params.ftp_prog_name));
						oa_memcpy(dev_now_params.ftp_prog_name, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
						p_set->act_kind = para_save;
					}
				}
				else{//not equal
					//DEBUG(" oa_strlen(p_set->context.con_ch):%d!", oa_strlen(p_set->context.con_ch));
					oa_memset(dev_now_params.ftp_prog_name, 0x0, sizeof(dev_now_params.ftp_prog_name));
					oa_memcpy(dev_now_params.ftp_prog_name, p_set->context.con_ch, oa_strlen(p_set->context.con_ch));
					//DEBUG(" m_server_ip:%s!", dev_now_params.m_server_ip);
					p_set->act_kind = para_save;
				}
			}
		}break;
		case e_STATUS:{
			p_set->s_k = sms_special;
		}break;
		case e_GPS:{
			p_set->s_k = sms_special;
		}break;
		case e_PARARST:{
			p_set->act_kind = fac_set;
		}break;
		case e_CLRLOG:{
			p_set->act_kind = clr_log;
		}break;
		case e_AUTHEN:{
			if (p_set->kind == set){
				if (p_set->context.con_ch[0] == 0xff)	p_set->act_kind = clr_authcode;
				else if (oa_strlen(p_set->context.con_ch) > 0){
					p_set->act_kind = update_authcode;
				}
			}			
		}break;
		case e_UPDATE:{
			p_set->act_kind = update;
		}break;
		case e_SNUMS:
		case e_VERSA:{

		}break;
		case e_RESTART:{
			p_set->act_kind = reset;
		}break;
		case e_DEVID:{
			if (p_set->kind == set){
				if (!oa_strncmp(dev_now_params.term_id, p_set->context.con_ch, DEVID_LEN)){
						PRINT_SAMEPARA;
						p_set->act_kind = no_act;
						break;
					}
					else{//not equal
						oa_memset(dev_now_params.term_id, 0x0, sizeof(dev_now_params.term_id));
						oa_memcpy(dev_now_params.term_id, p_set->context.con_ch, DEVID_LEN);
						p_set->act_kind = rereg;
					}
			}
		}break;
		case e_none:{
			DEBUG(" not support!");
		}break;
		default:{
			DEBUG(" not support!");
			break;
		}
	}	
}

/*********************************************************
*Function:      oa_app_sms()
*Description:  handle the sms context
*Return:        void
*Others:         
*********************************************************/
void oa_app_sms(void)
{
	//do about sms
	e_keyword key_ret = e_none;
	keyword_context set = {0x0};
	oa_uint8 e_i, i = 0;
	oa_uint8 sn = 0;
	oa_char *p = NULL;
	oa_char prefix[MAX_SMS_NUM][64] = {{0x0}, {0x0}, {0x0}, {0x0}, {0x0}};
	oa_char data[256] = {0x0};
	oa_char sendbuf[256] = {0x0};
	oa_char buf[256] = {0x0};
	oa_uint8 len;
	oa_bool ms_ack;
	oa_bool try_unlock_inside = OA_FALSE;
	sms_kind t_s = sms_normal;

	//len = message.len;
	oa_memcpy(data, message.data, message.len);
	p = strtok(data, ";");
	if (NULL == p){
		DEBUG("format err");
		return;
	}
	while(NULL != p){
		if (i <= MAX_SMS_NUM){
			oa_strcpy(prefix[i], p);
			i++;
			p = strtok(NULL, ";");
		}
		else break;
	}

	if (prefix[i-1][0] == 'A' || prefix[i-1][0] == 'a' && prefix[i-1][1] == 0x0){
		sn = i -1;
		ms_ack = OA_TRUE;
	}
	else if (prefix[i-1][0] == 'N' || prefix[i-1][0] == 'n' && prefix[i-1][1] == 0x0){
		sn = i -1;
		ms_ack = OA_FALSE;
	}
	else{
		sn = i;
		ms_ack = OA_FALSE;
	}
	//add ";"
	for (i = 0;i < sn; i++){
		prefix[i][oa_strlen(prefix[i])] = ';';
		DEBUG("%s", prefix[i]);
	}
#if 0	
	if (sn == 1){
		DEBUG("1 x sms");
		//do not support multiple sms
		for (e_i = 0;e_i < KEYWORDS_SIZE;e_i++){
			key_ret = lookfor_keywords_loop(NULL, len, &set, e_i, sms);
			if (e_none == key_ret){
				continue;
			}
			handle_keyword(NULL, NULL, NULL, key_ret, &set, sms);
			oa_memset(&set, 0x0, sizeof(set));
		}
	}
#endif
	if (sn <= MAX_SMS_NUM){
		DEBUG("%d x sms", sn);
		for (i = 0; i < sn; i++){
			for (e_i = 0;e_i < KEYWORDS_SIZE;e_i++){
				key_ret = look4keywords4ms(prefix[i], oa_strlen(prefix[i]), &set, e_i, sms);
				if (e_none == key_ret){
					continue;
				}
				
				handle_keyword4ms(key_ret, &set);
				if (ms_ack == OA_TRUE){
					handle_common4ms(key_ret, buf, &len, sms);
					DEBUG("\nbuf:%s len:%d", buf, len);
					if (len == 0) return;//do not ack
					if (set.s_k == sms_special){
						t_s = sms_special;
						oa_memcpy(sendbuf, buf, len);
					}
					else if (set.s_k == sms_normal){
						t_s = sms_normal;
						oa_strcat(sendbuf, buf);
					}
					oa_memset(buf, 0x0, sizeof(buf));
				}
				dev_action_handle(&set, sms);
				if (set.kind == 0x1 && use_is_lock()) try_unlock_inside = OA_TRUE;
				oa_memset(&set, 0x0, sizeof(set));
			}
		}
		if (ms_ack == OA_TRUE){
			oa_uint8 n;
			oa_char temp[256] = {0x0};
			if (sms_special == t_s){
				n = len/140;
				if (n > 1){
					DEBUG("sms content is too long");
					return;
				}
				else if (n == 1){//only for 'STATUS;' & 'GPS'
					oa_memcpy(temp, sendbuf, 140);
					sendsms4ms(temp, 140, sms_special);
					oa_memset(temp, 0x0, 256);
					if (len - 140 > 0){
						oa_memcpy(temp, &sendbuf[140], len -140);
						sendsms4ms(temp, len -140, sms_special);
					}
				}
				else if (n == 0){
					oa_memcpy(temp, sendbuf, len);
					sendsms4ms(temp, len, sms_special);
				}
			}
			else if(sms_normal == t_s) sendsms4ms(sendbuf, oa_strlen(sendbuf), sms_normal);
		}

		
		if (try_unlock_inside == OA_TRUE){
			DEBUG("try unlock");
			try_unlock = OA_TRUE;
		}
	}
	else DEBUG("too many sms");
	
	return;
}
