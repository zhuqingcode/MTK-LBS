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
#include "oa_debug.h"
#define PROTCL_JT808
//#define GPS_INTVL 50	//50ticks gps任务运行间隔
#define CHECK_GPS_ERR	(50)	//判断gps模块异常的时间
#define UPDATE_DISTANC  0.3	//行驶里程大于0.3km时做一次里程统计
const float EPSINON = 0.00001;

extern DEVICE_PARAMS dev_now_params;
extern DEV_PLAT_PARAS dev_running;
/*********************************************************
*Function:      ISGpsAntOK()
*Description:   检测天线状态，并触发或取消天线故障报警。
*Calls:         GPS_GetAntaStatus() 硬件查询gps天线开路
*Called By:     无 
*Input:         无
*Output:        无
*Return:        无
*Others:         
*********************************************************/
static u8 ISGpsAntOK()
{
	if(GPS_ANT_OPEN==GPS_GetAntaStatus())
	{
	}
	
#if 0
	{//天线开路
		if(ReadAlarmPara(StaAlarm0,ALARM_GNSS_ANTENNA)==RESET)
		{
#if PROTCL_JT808
			u8 Buf[50]={0};
			AlarmHandle(ALARM_GNSS_ANTENNA,Buf); //报警操作
			DevReq2ServPackag_build(REPORT_LOCATION); //告警产生一包位置汇报
#elif PROTCL_HX54
			WriteAlarmPara(SET,StaAlarm0,ALARM_GNSS_ANTENNA);	//告警
			DEBUG(PrintInfo,"GPS task:Gps antenna alarm!\r\n");
#endif
		}
		return 0;
	}
	else
	{
		if(ReadAlarmPara(StaAlarm0,ALARM_GNSS_ANTENNA)==SET)
		{
			WriteAlarmPara(RESET,StaAlarm0,ALARM_GNSS_ANTENNA);	//取消告警
			DEBUG(PrintInfo,"GPS task:Gps antenna is ok.\r\n");
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
	static u8  ModelCnt = 0; //计算收不到gps数据的时间
	u8 U8Temp = 0;
	static u8  COGcnt = 0; //拐点判断用
	static u16 Oldcog = 0; //拐点判断用
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
	static u32 park_times = 0;
	static oa_bool fatigue = OA_FALSE;//fatigue check
	static u8 time_last[6] = {0x0};
	u8 time_cur[6] = {0x0};
	static oa_uint8 day_drive = 0;
	oa_uint16 build_ret;
	oa_uint16 soc_ret;
	u32 flag;
#if 0	
	if (dev_running.plat_status == OFFLINE){//concern connect to platform first
		goto redo;
	}
#endif
	if (OA_TRUE == task_runed){
		DEBUG("(:(:(:(:(:(:(:(:task is %s running:):):):):):):):)", __func__);
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
#if 0	
	//gps antenna status detect 
	//ISGpsAntOK();
	ret = gps_ana_detect();
	if (OA_FALSE == ret){//gps ana is off
		DEBUG("gps ana is off");
		handle_alarm_status(StaAlarm0, ALARM_GNSS_ANTENNA, SET, OA_TRUE);
	}
	else	{//gps ana is on
		if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ANTENNA) == SET){
			handle_alarm_status(StaAlarm0, ALARM_GNSS_ANTENNA, RESET, OA_TRUE);
		}
		
	}
#endif
	//gps data analysis:mileage statistics, speed alarm, driver fatigue,cog and so on.
	result = GPS_DataAnaly();//update gps datas
	//DEBUG("result:0x%X", result);
	if (result & GPS_NO_DATA){//analysis gps data failed for 30s, module status must be changed to broken 
		if (ModelCnt * GPS_RUN_SECONDS < CHECK_GPS_ERR){
			ModelCnt++;							
		}	
		else{//handle this alarm
			DEBUG("gps model err");
			ret = handle_alarm_status(StaAlarm0, ALARM_GNSS_ERR, SET,OA_TRUE);
			//if (OA_TRUE == ret)	ModelCnt = 0;
		}
	}
	else{//if gps module's is good, change it to good because gps data is normal
		if (ReadAlarmPara(StaAlarm0, ALARM_GNSS_ERR) == SET){//cancel this alarm, upload instantly
			//WriteAlarmPara(RESET, StaAlarm0, ALARM_GNSS_ERR);
			handle_alarm_status(StaAlarm0, ALARM_GNSS_ERR, RESET, OA_TRUE);
			DEBUG("gps model is OK.");
		}
		//clear it
		if (ModelCnt > 0) ModelCnt = 0;
		if (result & NMEA_RMC_OK){//gps data handle
			GPS_GetPosition(&gps_info);//copy gps data into 'gps_info'
			//just for printing
			if (infoprintCnt++ >= 15){
				DEBUG("Time: %x %x %x %x:%x:%x ",gps_info.Time[0],gps_info.Time[1],gps_info.Time[2],gps_info.Time[3],gps_info.Time[4],gps_info.Time[5]);
				DEBUG("Latitude: %c%c%c°%c%c%c%c%c%c′",gps_info.North_Indicator,gps_info.Latitude[0],
				gps_info.Latitude[1],gps_info.Latitude[2],gps_info.Latitude[3],gps_info.Latitude[4],
				gps_info.Latitude[5],gps_info.Latitude[6],gps_info.Latitude[7]);
				DEBUG("Longitude: %c%c%c%c°%c%c%c%c%c%c′",gps_info.East_Indicator,gps_info.Longitude[0],gps_info.Longitude[1],
				gps_info.Longitude[2],gps_info.Longitude[3],gps_info.Longitude[4],gps_info.Longitude[5],gps_info.Longitude[6],
				gps_info.Longitude[7],gps_info.Longitude[8]);
				DEBUG("COG: %d; Speed: %d (km/h)  ",gps_info.COG,gps_info.Speed);
				DEBUG("Height: %d",gps_info.Height);
				infoprintCnt = 0;
			}
			//set location flag
			if ((ReadAlarmPara(StaSector1, STA_GPS_FIXED) == RESET) && (GPS_FIXED == gps_info.Fix_Status)){
				//WriteAlarmPara(SET, StaSector1, STA_GPS_FIXED);
				handle_alarm_status(StaSector1, STA_GPS_FIXED, SET, OA_TRUE);
				DEBUG("GPS locate done");
				Oldcog = gps_info.COG;//
				if (OA_FALSE == rtc_status){
					set_rtc_time(gps_info.Time); //when gps locate ok, set rtc time at once
					rtc_status = OA_TRUE;
					get_rtc_time(time_last);//get current time
				}
				
			}
			//-------------------------day overtime drive-----------------------------------
			if (OA_TRUE == rtc_status){//rtc is ok
				get_rtc_time(time_cur);
				if (!oa_memcmp(time_last, time_cur, 3)){//in one day
					if (gps_info.Speed > 0)	day_drive++;
					if (day_drive * GPS_RUN_SECONDS >= dev_now_params.day_add_drive_time_threshold){
							ret = handle_alarm_status(StaAlarm0, ALARM_OVERTIME_PARKING, SET, OA_TRUE);
							DEBUG("day overtime drive");
					}
				}
				else{
					oa_memcpy(time_last, time_cur, sizeof(time_last));
					day_drive = 0;
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
				if (COGcnt > 6)//cog变化大于15度持续3s以上
				{
					COGcnt = 0;
					Oldcog = gps_info.COG;
					DEBUG(" Old Cog: %d km", __FILE__,  __func__, __LINE__, Oldcog);
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
				DEBUG("speed max!");
			}
			
			if (gps_info.Speed > speed){//handle this alarm & upload instantly
				DEBUG("over speed");
				ret = handle_alarm_status(StaAlarm0, ALARM_OVER_SPEED, SET, OA_TRUE);
			}
			else if (gps_info.Speed <= speed){
				if (ReadAlarmPara(StaAlarm0, ALARM_OVER_SPEED) == SET){
					//WriteAlarmPara(RESET, StaAlarm0, ALARM_OVER_SPEED);//cancel this alarm
					DEBUG("cancel over speed");
					handle_alarm_status(StaAlarm0, ALARM_OVER_SPEED, RESET, OA_TRUE);
				}
			}
			//--------------------------------------------------------------------
			//--------------------------over time park------------------------------
			if (0 == gps_info.Speed){
				park_times++;
				if (park_times * GPS_RUN_SECONDS >= dev_now_params.max_park_time){
					handle_alarm_status(StaAlarm0, ALARM_OVERTIME_PARKING, SET, OA_TRUE);
					DEBUG("overtime park");
				}
			}
			else if (gps_info.Speed > 0){
				if (park_times > 0)	park_times = 0;
				if (ReadAlarmPara(StaAlarm0, ALARM_OVERTIME_PARKING) == SET){
					ret = handle_alarm_status(StaAlarm0, ALARM_OVERTIME_PARKING, RESET, OA_TRUE);
					DEBUG("cancel overtime park");
				}
			}
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
			if (dev_now_params.report_strategy >= 0 && dev_now_params.report_strategy <= 2){//Periodically reported
				if (upload_times * GPS_RUN_SECONDS >= dev_now_params.default_reporttime){
					DEBUG("@@@");
					print_rtc_time();
					DEBUG("send one location packet!maybe blind data");
					ret = handle_alarm_status(OA_FALSE, OA_FALSE, SET, OA_FALSE);//just send
					if (ret == OA_TRUE)	upload_times = 0;
				}
			}
#if 0
			else if (dev_now_params.report_strategy == 1){//distance reported
				if ((u32)(1000 * d_r_distance) >= dev_now_params.default_reportdistance){
					DEBUG("send one location packet!");
					ret = handle_alarm_status(OA_FALSE, OA_FALSE, SET, OA_FALSE);//just send
					if (ret == OA_TRUE)	d_r_distance = 0.0;
				}
			}
			else if (dev_now_params.report_strategy == 2){//Periodically reported & distance reported
				if (upload_times * GPS_RUN_SECONDS >= dev_now_params.default_reporttime){
					DEBUG("send one location packet!");
					ret = handle_alarm_status(OA_FALSE, OA_FALSE, SET, OA_FALSE);//just send
					if (ret == OA_TRUE)	upload_times = 0;
				}

				if ((u32)(1000 * d_r_distance) >= dev_now_params.default_reportdistance){
					DEBUG("send one location packet!");
					ret = handle_alarm_status(OA_FALSE, OA_FALSE, SET, OA_FALSE);//just send
					if (ret == OA_TRUE)	d_r_distance = 0.0;
				}

			}
#endif
			//--------------------------------------------------------------------
			//-------------------------fatigue driving-------------------------------
			if (gps_info.Speed > 0){
				driver_time++;
				if (driver_time * GPS_RUN_SECONDS > BASIC_DRIVETIME){//concern that driver has already drived for 30min firstly.
					fatigue = OA_TRUE;							//then distinguish fatigue driving
				}
				
				if (driver_time * GPS_RUN_SECONDS > dev_now_params.continuous_drive_time_threshold){
					DEBUG("fatigue driving!");
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
			DEBUG("NMEA_RMC_UNFIXED!");
			//设置未定位标志
			if (ReadAlarmPara(StaSector1,STA_GPS_FIXED) == SET)
				//WriteAlarmPara(RESET,StaSector1,STA_GPS_FIXED);
				handle_alarm_status(StaSector1, STA_GPS_FIXED, RESET, OA_TRUE);
		}
		if (result &UBX_CFG_RST_OK){
			DEBUG("UBX_CFG_RST_OK!");
		}
		if (result &UBX_CFG_MSG_OK){
			DEBUG("UBX_CFG_MSG_OK!");
		}
		if (result & NMEA_GGA_OK){//高度定位OK
//				GPS_GetPosition(&gps_info);
//				DEBUG(PrintDebug," Height: %d\r\n",gps_info.Height);
		}
		else if (result & NMEA_GGA_UNFIXED){
			DEBUG("NMEA_GGA_UNFIXED!");
		}
	}

redo : oa_timer_start(OA_TIMER_ID_5, oa_app_gps, NULL, OA_GPS_SCHEDULER_PERIOD);//run per 500 millisecond
	return;
}
#if 0
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
			DEBUG("err %s step : %d", __FILE__, __func__, gps_init_step);
			goto redoit;
		}
	}
	
	if (gps_init_step == 0x10)/*finish gps initial*/
	{
		DEBUG("gps initial ok!");
		oa_timer_stop(OA_TIMER_ID_2);
		oa_timer_start(OA_TIMER_ID_3, oa_app_gps, NULL, OA_GPS_SCHEDULER_PERIOD);//run per 500 millisecond
		return;
	}
	
redoit : oa_timer_start(OA_TIMER_ID_2, oa_app_gps_init, NULL, OA_GPS_SCHEDULER_PERIOD);//run per 500 millisecond
	return;	
}
#endif
