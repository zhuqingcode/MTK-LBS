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


/*��������ö��*/
typedef enum
{
	Circular_Area = 0,
	Rectangle_Area,
	Poly_Area,
}
Area_Type_enum;
/*�������Զ���*/
//#pragma pack(1)
typedef struct
{
	u16 depend_time:1;//ʱ����Ч���ڸö�ʱ���ڣ����ж��Ƿ���������򣬼���ر����ϱ�
	u16 speed_limit:1;//�ٶ����ƣ��������ڣ��ٶ�������  
	u16 alarm_inside_to_driver:1;//ʻ������֪ͨ��ʻԱʹ��λ	
	u16 alarm_inside_to_plat:1;	//ʻ������֪ͨƽ̨ʹ��λ
	u16 alarm_outside_to_driver:1;//ʻ������֪ͨ��ʻԱʹ��λ
	u16 alarm_outside_to_plat:1; //ʻ������֪ͨƽ̨ʹ��λ
	u16 N_or_S:1; //�ϱ�γ���֣�0��ʾ������Ϣ�е�����γ��Ϊ��γ  1��ʾΪ��γ
	u16 E_or_W:1; //���������֣�0��ʾ������Ϣ�е����о���Ϊ����  1��ʾΪ����
	u16 limit_open_door:1; //�����ڽ�ֹ���ţ�����������λ
	u16 Reserved:5;	  //����λ
	u16 close_wcm_inside:1;	//0:�����ڿ���ͨ��ģ�� 1:�����ڹر�ͨ��ģ��
	u16 GNSS_EN_inside:1;  //0:�����ڿ���GNSS��λ 1:�����ڹر�GNSS��λ
}
Area_Para_struct;
//#pragma pack()
/*���������ݶ���*/
//#pragma pack(1)
typedef struct
{
	u32 area_id;//����ID
	Area_Para_struct area_para;	  //��������
	u8 start_time[6]; //��ʼʱ��  �����������depend_time��Ч���޸�λ
	u8 stop_time[6];  //����ʱ��  �����������depend_time��Ч���޸�λ
	u16 max_speed;  //��������  ����������1λΪ1ʱ��Ч
	u8 continue_time; //����ʱ��  ��λΪ�� ����������1λΪ1ʱ��Ч

	u8 area_type;//��������  Բ�λ�����
	u16 point_num;//��������򶥵���         
	u8 flag;//��Чλ  ��ֵΪ0xaaʱ��Ч	
}
Area_attr_struct;
//#pragma pack()
//#pragma pack(1)
#if 0
typedef struct
{
	u8 area_attr[MAX_AREA_SUM];//������Ϣ ֵΪ0xAAΪ��Ч
	u8 area_type[MAX_AREA_SUM];//����������Ϣ����
	u32 area_id[MAX_AREA_SUM];//�����Ѵ�����ID
	u32 actual_id[MAX_AREA_SUM];//�Զ���������ID
	u8 reserve_data[10];//�������ݣ��Ա��Ժ�����
	u32 max_actual_id;  //��ǰ�����Զ���������ID
	u8 manage_vail_flag;//�����ж������Ƿ���Ч ��ֵΪ0xaaʱ��Ч
	 
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
/*�ⲿ�����ӿ�����*/
u8 SaveAreaData(u8 *pbuf,u8 area_type,u8 option,u16 *readlen);
u8 DelAreaData(u8* pbuf,u8 areatype);
u8 Area_Judge_Init(void);
u32 Area_Judge(u32 lon,u32 lat,u16 speed,u8 door_state,u8 *now_time);
#endif
