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
 *   oa_gps.c
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
#include "oa_gps.h"
#include "oa_dev_params.h"
#include "oa_jt808.h"
#include "oa_platform.h"
#define PROTCL_JT808
//#define GPS_INTVL 50	//50ticks gps�������м��
#define CHECK_GPS_ERR	(50)	//�ж�gpsģ���쳣��ʱ��
#define UPDATE_DISTANC  0.3	//��ʻ��̴���0.3kmʱ��һ�����ͳ��
const float EPSINON = 0.00001;

extern DEVICE_PARAMS dev_now_params;
extern DEV_PLAT_PARAS dev_running;
/*********************************************************
*Function:      ISGpsAntOK()
*Description:   �������״̬����������ȡ�����߹��ϱ�����
*Calls:         GPS_GetAntaStatus() Ӳ����ѯgps���߿�·
*Called By:     �� 
*Input:         ��
*Output:        ��
*Return:        ��
*Others:         
*********************************************************/
static u8 ISGpsAntOK()
{
	if(GPS_ANT_OPEN==GPS_GetAntaStatus())
	{
	}
	
#if 0
	{//���߿�·
		if(ReadAlarmPara(StaAlarm0,ALARM_GNSS_ANTENNA)==RESET)
		{
#if PROTCL_JT808
			u8 Buf[50]={0};
			AlarmHandle(ALARM_GNSS_ANTENNA,Buf); //��������
			DevReq2ServPackag_build(REPORT_LOCATION); //�澯����һ��λ�û㱨
#elif PROTCL_HX54
			WriteAlarmPara(SET,StaAlarm0,ALARM_GNSS_ANTENNA);	//�澯
			Trace(PrintInfo,"GPS task:Gps antenna alarm!\r\n");
#endif
		}
		return 0;
	}
	else
	{
		if(ReadAlarmPara(StaAlarm0,ALARM_GNSS_ANTENNA)==SET)
		{
			WriteAlarmPara(RESET,StaAlarm0,ALARM_GNSS_ANTENNA);	//ȡ���澯
			Trace(PrintInfo,"GPS task:Gps antenna is ok.\r\n");
		}
		return 0;
	}
	
#endif
	
}
/*********************************************************
*Function:      oa_app_gps()
*Description:  handle the gps data
*Return:        void
*Others:         
*********************************************************/
void oa_app_gps(void)
{
	static u8  gps_status = GPS_ERR;
	static oa_bool rtc_status = OA_FALSE;//false:has not set;true:has set
	u32 result = 0;
	static oa_bool task_runed = OA_TRUE;
	oa_bool ret;
	static u8  ModelCnt = 0; //�����ղ���gps���ݵ�ʱ��
	u8 U8Temp = 0;
	static u8  COGcnt = 0; //�յ��ж���
	static u16 Oldcog = 0; //�յ��ж���
	u16 U16Temp = 0;
	static u32 IntvlCog = 0;
	static u8 infoprintCnt = 0;
	static FP32 IntvlDistanc = 0.0;
	static FP32 d_r_distance = 0.0;
	STRUCT_RMC gps_info;
	u32 speed;
	static u8 upload_times;
	static u32 driver_time;
	static u32 relax_time;
	static oa_bool vehicle_status;//OA_FALSE:park,OA_TRUE:driving
	static oa_bool fatigue = OA_FALSE;//fatigue check 
	oa_uint16 build_ret;
	oa_uint16 soc_ret;
	u32 flag;
#if 0	
	if (dev_running.plat_status == OFFLINE){//concern connect to platform first
		goto redo;
	}
#endif
	if (OA_TRUE == task_runed){
		OA_DEBUG_USER("<<<<<<<<<<<<<task %s is running......>>>>>>>>>>>>>", __func__);
		mile_stat_init();//init here
		task_runed = OA_FALSE;
	}
	
	upload_times++;

	if (GPS_ERR == gps_status){
		//initial gps module
		gps_status = GPS_SW_Init();//I modify some code
		if (GPS_ERR == gps_status){
			goto redo;
		}
	}
	
	//gps antenna status detect 
	//ISGpsAntOK();
	ret = gps_ana_detect();
	if (OA_FALSE == ret){//gps ana is off
		Trace("gps ana is off");
		handle_alarm_status(StaAlarm0, ALARM_GNSS_ANTENNA, SET, OA_TRUE);
	}
	else	{//gps ana is on
		if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ANTENNA) == SET){
			handle_alarm_status(StaAlarm0, ALARM_GNSS_ANTENNA, RESET, OA_TRUE);
		}
		
	}
	//gps data analysis:mileage statistics, speed alarm, driver fatigue,cog and so on.
	result = GPS_DataAnaly();//update gps datas
//	Trace("(%s:%s:%d):result:0x%X", __FILE__,  __func__, __LINE__, result);
	if (result & GPS_NO_DATA){//analysis gps data failed for 30s, module status must be changed to broken 
		if (ModelCnt * GPS_RUN_SECONDS < CHECK_GPS_ERR){
			ModelCnt++;							
		}	
		else{//handle this alarm
			ret = handle_alarm_status(StaAlarm0, ALARM_GNSS_ERR, SET,OA_TRUE);
			if (OA_TRUE == ret)	ModelCnt = 0;
		}
	}
	else{//if gps module's is good, change it to good because gps data is normal
		if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ERR) == SET){//cancel this alarm, upload instantly
			//WriteAlarmPara(RESET, StaAlarm0, ALARM_GNSS_ERR);
			handle_alarm_status(StaAlarm0, ALARM_GNSS_ERR, RESET, OA_TRUE);
			Trace("(%s:%s:%d): Gps model is OK.", __FILE__,  __func__, __LINE__);
		}
		//clear it
		if (ModelCnt > 0) ModelCnt = 0;
		if (result & NMEA_RMC_OK){//gps data handle
			GPS_GetPosition(&gps_info);//copy gps data into 'gps_info'
			//just for printing
			if (infoprintCnt++ >= 15){
				Trace(" Time: %x %x %x %x:%x:%x ",gps_info.Time[0],gps_info.Time[1],gps_info.Time[2],gps_info.Time[3],gps_info.Time[4],gps_info.Time[5]);
				Trace(" Latitude: %c%c%c��%c%c%c%c%c%c��",gps_info.North_Indicator,gps_info.Latitude[0],
				gps_info.Latitude[1],gps_info.Latitude[2],gps_info.Latitude[3],gps_info.Latitude[4],
				gps_info.Latitude[5],gps_info.Latitude[6],gps_info.Latitude[7]);
				Trace(" Longitude: %c%c%c%c��%c%c%c%c%c%c��",gps_info.East_Indicator,gps_info.Longitude[0],gps_info.Longitude[1],
				gps_info.Longitude[2],gps_info.Longitude[3],gps_info.Longitude[4],gps_info.Longitude[5],gps_info.Longitude[6],
				gps_info.Longitude[7],gps_info.Longitude[8]);
				Trace(" COG: %d; Speed: %d (km/h)  ",gps_info.COG,gps_info.Speed);
				Trace(" Height: %d",gps_info.Height);
				infoprintCnt = 0;
			}
			//set location flag
			if ((ReadAlarmPara(StaSector1, STA_GPS_FIXED) == RESET) && (GPS_FIXED == gps_info.Fix_Status)){
				//WriteAlarmPara(SET, StaSector1, STA_GPS_FIXED);
				handle_alarm_status(StaSector1, STA_GPS_FIXED, SET, OA_TRUE);
				Oldcog = gps_info.COG;//
				if (OA_FALSE == rtc_status){
					set_rtc_time(gps_info.Time); //when gps locate ok, set rtc time at once
					rtc_status = OA_TRUE;
				}
				
			}
#if 0
			//-------------------------bent point jugde-----------------------------
			//ReadLbsCfgPara(eRptCog,(u8 *)&IntvlCog,&U8Temp);//changed to statement below
			IntvlCog = dev_now_params.corner_reportangle;
			if (Oldcog <= gps_info.COG)
				U16Temp = gps_info.COG-Oldcog;
			else
				U16Temp = 360-Oldcog+gps_info.COG;
			if (U16Temp >= IntvlCog)
			{
				COGcnt++;
				if (COGcnt > 6)//cog�仯����15�ȳ���3s����
				{
					COGcnt = 0;
					Oldcog = gps_info.COG;
					Trace("(%s:%s:%d): Old Cog: %d km", __FILE__,  __func__, __LINE__, Oldcog);
					DevReq2ServPackag_build(REPORT_LOCATION);
				}	
			}
			else
			{
				if (COGcnt>0) COGcnt=0;
				Oldcog = gps_info.COG;
			}
			//---------------------------------------------------------------------
#endif
			//--------------------------overspeed alarm-----------------------------
			speed = dev_now_params.max_speed;
			//gps_info.Speed = MAX_SPEED;//just for test
			if (speed > MAX_SPEED || gps_info.Speed > MAX_SPEED){
				gps_info.Speed = MAX_SPEED;
				Trace("(%s:%s:%d): speed max!", __FILE__, __func__, __LINE__);
			}
			
			if (gps_info.Speed > speed){//handle this alarm & upload instantly
				ret = handle_alarm_status(StaAlarm0, ALARM_OVER_SPEED, SET, OA_TRUE);
			}
			else if (gps_info.Speed <= speed){
				if (ReadAlarmPara(StaAlarm0, ALARM_OVER_SPEED) == SET){
					//WriteAlarmPara(RESET, StaAlarm0, ALARM_OVER_SPEED);//cancel this alarm & don't upload instantly
					handle_alarm_status(StaAlarm0, ALARM_OVER_SPEED, RESET, OA_TRUE);
				}
			}
			//--------------------------------------------------------------------
			//--------------------------mileage statistis-----------------------------
			IntvlDistanc += GPS_IntvlDistanc(); //km
			d_r_distance = IntvlDistanc;//just for distance report
			if (IntvlDistanc - UPDATE_DISTANC >= EPSINON){
				//GpsDistancCacul(&IntvlDistanc);//changed to statement below
				ret = mile_stat_add(IntvlDistanc);
				if (OA_TRUE == ret){
					IntvlDistanc -= UPDATE_DISTANC;//IntvlDistanc = 0.0;
				}
				
			}
			//--------------------------------------------------------------------
			//------------------------Periodically reported---------------------------
			if (dev_now_params.report_strategy == 0){//Periodically reported
				if (upload_times * GPS_RUN_SECONDS >= dev_now_params.default_reporttime){
					ret = handle_alarm_status(OA_FALSE, OA_FALSE, SET, OA_FALSE);//just send
					if (ret == OA_TRUE)	upload_times = 0;
				}
			}
			else if (dev_now_params.report_strategy == 1){//distance reported
				if ((u32)(1000 * d_r_distance) >= dev_now_params.default_reportdistance){
						ret = handle_alarm_status(OA_FALSE, OA_FALSE, SET, OA_FALSE);//just send
						if (ret == OA_TRUE)	d_r_distance = 0.0;
				}
			}
			else if (dev_now_params.report_strategy == 2){//Periodically reported & distance reported
				if (upload_times * GPS_RUN_SECONDS >= dev_now_params.default_reporttime){
					ret = handle_alarm_status(OA_FALSE, OA_FALSE, SET, OA_FALSE);//just send
					if (ret == OA_TRUE)	upload_times = 0;
				}

				if ((u32)(1000 * d_r_distance) >= dev_now_params.default_reportdistance){
						ret = handle_alarm_status(OA_FALSE, OA_FALSE, SET, OA_FALSE);//just send
						if (ret == OA_TRUE)	d_r_distance = 0.0;
				}

			}
			//--------------------------------------------------------------------
			//-------------------------fatigue driving-------------------------------
			if (gps_info.Speed > 0){
				driver_time++;
				if (driver_time * GPS_RUN_SECONDS > BASIC_DRIVETIME){//concern that driver has already drived for 30min firstly.
					fatigue = OA_TRUE;							//then distinguish fatigue driving
				}
				
				if (driver_time * GPS_RUN_SECONDS > dev_now_params.continuous_drive_time_threshold){
					Trace("(%s:%s:%d): fatigue driving!", __FILE__, __func__, __LINE__);
					ret = handle_alarm_status(StaAlarm0, ALARM_DRIVE_TIRED, SET, OA_TRUE);
					if (ret == OA_TRUE)	driver_time = 0;
					
				}

				if (OA_TRUE == fatigue){
					if (relax_time * GPS_RUN_SECONDS > 120 
					&& relax_time * GPS_RUN_SECONDS < dev_now_params.min_rest_time){//60:just for concerning driver has have a relax before.
					if (ReadAlarmPara(StaAlarm0, ALARM_DRIVE_TIRED) == RESET){
						WriteAlarmPara(SET, StaAlarm0, ALARM_DRIVE_TIRED); //set this kind of alarm
					}
					}
					else if (relax_time * GPS_RUN_SECONDS > dev_now_params.min_rest_time){
						if (ReadAlarmPara(StaAlarm0, ALARM_DRIVE_TIRED) == SET){
							WriteAlarmPara(RESET, StaAlarm0, ALARM_DRIVE_TIRED); //cancel this kind of alarm
						}
					}
					fatigue = OA_FALSE;
				}
				
				
				relax_time = 0;
			}
			else if (gps_info.Speed == 0){
				relax_time++;
				driver_time = 0;
			}
			//---------------------------------------------------------------------
		}
		else if (result & NMEA_RMC_UNFIXED){
			Trace("(%s:%s:%d): NMEA_RMC_UNFIXED!", __FILE__,  __func__, __LINE__);
			//����δ��λ��־
			if (ReadAlarmPara(StaSector1,STA_GPS_FIXED) == SET)
				//WriteAlarmPara(RESET,StaSector1,STA_GPS_FIXED);
				handle_alarm_status(StaSector1, STA_GPS_FIXED, RESET, OA_TRUE);
		}
		if (result & UBX_CFG_RST_OK){
			Trace("(%s:%s:%d): UBX_CFG_RST_OK!", __FILE__,  __func__, __LINE__);
		}
		if (result & UBX_CFG_MSG_OK){
			Trace("(%s:%s:%d): UBX_CFG_MSG_OK!", __FILE__,  __func__, __LINE__);
		}
		if (result & NMEA_GGA_OK){//�߶ȶ�λOK
//				GPS_GetPosition(&gps_info);
//				Trace(PrintDebug," Height: %d\r\n",gps_info.Height);
		}
		else if (result & NMEA_GGA_UNFIXED){
			Trace("(%s:%s:%d): NMEA_GGA_UNFIXED!", __FILE__,  __func__, __LINE__);
		}
	}

redo : oa_timer_start(OA_TIMER_ID_5, oa_app_gps, NULL, OA_GPS_SCHEDULER_PERIOD);//run per 500 millisecond
	return;
}
/*********************************************************
*Function:      oa_app_gps_init()
*Description:  handle the gps initial
*Return:        void
*Others:         
*********************************************************/
void oa_app_gps_init(void)
{
	static oa_uint8  gps_feedback = OA_TRUE;
	static oa_uint8 gps_init_step = 0;
	u32 Response;
	u32 UnResponse;
	u32 result;
	if (gps_feedback)
	{
		switch (gps_init_step)
		{
			case 0x0:/*reset*/
			{
				gps_feedback = OA_FALSE;
				GPS_CFG_RST(Hot_Start, GPS_Stop);
				Response = UBX_CFG_RST_OK;
				UnResponse = UBX_CFG_RST_ERROR;
				goto redoit;
				break;
			}
			case 0x1:/*Close all field output*/
			case 0x2:
			case 0x3:
			case 0x4:
			case 0x5:
			case 0x6:
			case 0x7:
			case 0x8:
			case 0x9:
			case 0x0A:
			case 0x0B:
			{
				gps_feedback = OA_FALSE;
				GPS_CFG_MSG(NMEA_GP, gps_init_step-1, 0x0);
				Response = UBX_CFG_MSG_OK;
				UnResponse = UBX_CFG_MSG_ERROR;
				goto redoit;
				break;
			}
			case 0x0C:/*open GPRMC output*/
			{
				gps_feedback = OA_FALSE;
				GPS_CFG_MSG(NMEA_GP, GPRMC, 0x1);
				Response = UBX_CFG_MSG_OK;
				UnResponse = UBX_CFG_MSG_ERROR;
				goto redoit;
				break;
			}
			case 0x0D:/*open GPGGA output*/
			{
				gps_feedback = OA_FALSE;
				GPS_CFG_MSG(NMEA_GP, GPGGA, 0x1);
				Response = UBX_CFG_MSG_OK;
				UnResponse = UBX_CFG_MSG_ERROR;
				goto redoit;
				break;
			}
			case 0x0E:/*Setting the internal register*/
			{
				gps_feedback = OA_FALSE;
				GPS_CFG_NAV_SET();
				Response = UBX_NAV_MSG_OK;
				UnResponse = UBX_CFG_RST_ERROR;
				goto redoit;
				break;
			}
			case 0x0F:/*Setting the internal register*/
			{
				gps_feedback = OA_FALSE;
				GPS_CFG_RST();
				Response = UBX_CFG_RST_OK;
				UnResponse = UBX_CFG_RST_ERROR;
				goto redoit;
				break;
			}
			default:break;
		}
	}
	else
	{
		result = 0;
		result = GPS_DataAnaly();
		if (result & Response)
		{
			gps_feedback = OA_TRUE;
			gps_init_step++;
		}
		else if (result & UnResponse)
		{
			Trace("err %s step : %d", __FILE__, __func__, gps_init_step);
			goto redoit;
		}
	}
	
	if (gps_init_step == 0x10)/*finish gps initial*/
	{
		Trace("gps initial ok!");
		oa_timer_stop(OA_TIMER_ID_2);
		oa_timer_start(OA_TIMER_ID_3, oa_app_gps, NULL, OA_GPS_SCHEDULER_PERIOD);//run per 500 millisecond
		return;
	}
	
redoit : oa_timer_start(OA_TIMER_ID_2, oa_app_gps_init, NULL, OA_GPS_SCHEDULER_PERIOD);//run per 500 millisecond
	return;	
}
