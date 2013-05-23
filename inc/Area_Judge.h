#ifndef _AREA_JUDGE_H_
#define _AREA_JUDGE_H_
#include "oa_debug.h"

#define MAX_AREA_SUM     (24)
#define MAX_AREA_POINTS  (128)
#define AREA_PRINT_EN  	 (0)

#if AREA_PRINT_EN
#define AREA_Printf(arg...) Trace(PrintInfo,##arg) //peijl_test
#else
#define AREA_Printf DEBUG
#endif


/*区域类型枚举*/
typedef enum
{
	Circular_Area = 0,
	Rectangle_Area,
	Poly_Area,
}
Area_Type_enum;
/*区域属性定义*/
//#pragma pack(1)
typedef struct
{
	u16 depend_time:1;//时间有效，在该段时间内，会判断是否进出该区域，及相关报警上报
	u16 speed_limit:1;//速度限制，在区域内，速度有限制  
	u16 alarm_inside_to_driver:1;//驶入区域通知驾驶员使能位	
	u16 alarm_inside_to_plat:1;	//驶入区域通知平台使能位
	u16 alarm_outside_to_driver:1;//驶出区域通知驾驶员使能位
	u16 alarm_outside_to_plat:1; //驶出区域通知平台使能位
	u16 N_or_S:1; //南北纬区分，0表示区域信息中的所有纬度为北纬  1表示为南纬
	u16 E_or_W:1; //东西经区分，0表示区域信息中的所有经度为东经  1表示为西经
	u16 limit_open_door:1; //区域内禁止开门，将产生报警位
	u16 Reserved:5;	  //保留位
	u16 close_wcm_inside:1;	//0:区域内开启通信模块 1:区域内关闭通信模块
	u16 GNSS_EN_inside:1;  //0:区域内开启GNSS定位 1:区域内关闭GNSS定位
}
Area_Para_struct;
//#pragma pack()
/*区域项内容定义*/
//#pragma pack(1)
typedef struct
{
	u32 area_id;//区域ID
	Area_Para_struct area_para;	  //区域属性
	u8 start_time[6]; //开始时间  如果区域属性depend_time无效则无该位
	u8 stop_time[6];  //结束时间  如果区域属性depend_time无效则无该位
	u16 max_speed;  //区域限速  当区域属性1位为1时有效
	u8 continue_time; //持续时间  单位为秒 当区域属性1位为1时有效

	u8 area_type;//区域类型  圆形或多边形
	u16 point_num;//多边形区域顶点数         
	u8 flag;//有效位  当值为0xaa时有效	
}
Area_attr_struct;
//#pragma pack()
//#pragma pack(1)
#if 0
typedef struct
{
	u8 area_attr[MAX_AREA_SUM];//参数信息 值为0xAA为有效
	u8 area_type[MAX_AREA_SUM];//各个区域信息类型
	u32 area_id[MAX_AREA_SUM];//所有已存区域ID
	u32 actual_id[MAX_AREA_SUM];//自定义分配序号ID
	u8 reserve_data[10];//保留数据，以备以后利用
	u32 max_actual_id;  //当前最大的自定义分配序号ID
	u8 manage_vail_flag;//用于判断数据是否有效 当值为0xaa时有效
	 
}
Area_Manage_struct;
#endif
//#pragma pack()
//#pragma pack(1)
typedef struct
{	
	u32 Lat;
	u32 Lon;   
}
Area_Point;
//#pragma pack()
//#pragma pack(1)
typedef struct
{
	Area_Point area_point[MAX_AREA_POINTS];
}
Poly_Area_Desc;
//#pragma pack()
//#pragma pack(1)
typedef struct
{
	u32 lat;
	u32 lon;
	u32 rad;
}
Cir_Area_Desc;
//#pragma pack()
//#pragma pack(1)
typedef union
{
	Poly_Area_Desc poly_area_desc; 
	Cir_Area_Desc  cir_area_desc;
}
Area_Desc;
//#pragma pack()
/*外部函数接口声明*/
u8 SaveAreaData(u8 *pbuf,u8 area_type,u8 option,u16 *readlen);
u8 DelAreaData(u8* pbuf,u8 areatype);
u8 Area_Judge_Init(void);
u32 Area_Judge(u32 lon,u32 lat,u16 speed,u8 door_state,u8 *now_time);
#endif
