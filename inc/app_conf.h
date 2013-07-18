#ifndef APP_CONF_H
#define APP_CONF_H
#define SOFTWARE_VER	"V115TJ101" //"TJ_V100"	//软件版本

#define APTS_ENABLE		0
/***************************外设参数相关****************************************/
/*IO接口数目， 主要是方便遍历用*/
#define IOKindNum		7	//外设数使用的IO口数目 不小于PerpherIO_Define最大值
#define PerpherKindNum	30	//终端外设数目，不小于Perpher_Define最大值

/*外设工作状态位定义,与enum _Perpher对应*/
//#define HandDevice1OK				1<<1 
//#define CameraOK		 			1<<2 
//#define SchedulScrnOK				1<<3 
//#define LiDaICCardReaderOK 	 		1<<4 
//#define TaxiMeterOK					1<<5 
//#define HandDev0OK					1<<6 
//#define PrintfUsartOK	 			1<<7
//#define UnloadSensorOK 	 			1<<8 
//#define ClockwiseSensorOK		  	1<<9 
//#define AnticlockwiseSensorOK	 	1<<10
/*--外设功能参数定义--*/
//全部功能列表
#define LIST_SMSEnable				(1<<0) //短信功能允许
#define LIST_CallEnable				(1<<1) //电话功能允许
#define LimitTelEnable				(1<<2) //限制电话允许
#define CameraEnble					(1<<3) //拍照允许
#define OneCardEnable				(1<<4) //一卡通
#define UnloadSensorEnble			(1<<5) //卸料传感器
#define ClkwiseSensorEnble			(1<<6) //高电平有效
//#define AnticlkwiseSensorEnble		1<<7 //高电平有效
#define Taximeter_photoEnble		(1<<7) //计价器拍照允许
#define OnePerphFuncMaxNum		(8)	 //一个外设的功能最多类型
//手柄调度屏参数
#define SMSEnable					(1<<0) //短信功能允许
#define CallEnable					(1<<1) //电话功能允许
#define LimitTelEnable				(1<<2) //限制电话允许
//摄像头参数
#define CameraNum1					(0x01) //1路摄像头
#define CameraNum2					(0x02) //2路摄像头
#define CameraNum3					(0x03) //3路摄像头
#define CameraNum4					(0x04) //最大4路摄像头

//接近传感器参数
#define UnloadSensorHighEnble		(1<<0) //高电平有效
//正转传感器参数
#define ClkwiseSensorHighEnble		(1<<0) //正向高电平有效
#define AnticlkwiseSensorHighEnble	(1<<0) //反向高电平有效
//接近传感器参数
#define AnticlkwiseSensorHighEnble	(1<<0) //重车高电平有效

#if 0
//外设参数类型
typedef enum _UPDATE_MODE
{
	UpdateFlash,
	UpdateOnly
}UpdateModeEnum;
#endif
//外设参数类型
typedef enum _Paratype
{
	ePara1=0x01,	//IO口与外设对应关系 外部查询与设置
	ePara2,			//外设功能参数		 外部查询与设置
	ePara3,			//外设状态			 外部查询
	ePara4			//外设功能			外部查询
} PARATYPE_Define;
//外设接口枚举，对应PARA1参数
typedef enum _PerpherIO
{
	ComIO_RS485=0,
	Com4IO=1,
	Com5IO=2,
	InputIO1=3,
	InputIO2=4,
	InputIO3=5,
	OutputIO1=6,
#ifdef EVDO_USE
	DVR_In1=7,  //3512侧IO定义
	DVR_In2=8,
	DVR_In3=9,
	DVR_In4=10,
#endif
	IOKindMax,
} IO_Define;
//外设类型枚举,对应PARA2参数
typedef enum _Perpher
{//0x00无外设,进行查询设置时从0x01开始
	Camera=0x01,	//摄像头
	SchedulScrn=2,	//D5或者D6 基本没区别
	PrintfUsart=3,
	UnloadSensor=4,	//卸货传感器（接近传感器）
	ClkwiseSensor=5,	//正向传感器
	AnticlkwiseSensor=6,//反向传感器
	HandDev1=7,	      //LH7.0	 
	LiDaICCardReader=8,// 理达无线IC卡读卡器
	TaxiMeter=9,		//计价器
	HandDev0=10,		//一开始用的手柄
	LedDev1=11,		//车载广告屏
	LCDN990=12,		//N990屏
	StudentCardReader=13,//学生校车打卡
	PowerAndOil_ctrl=14, //断油断电控制
	Passenger=15,        //载客
	Door1Ctrl=16,		 //车开关门
	DDSCamera=17,		 //DDS司机疲劳检测
	Mobileye=18,		 //安全行车
	PerpherKindMax,
}Perpher_Define;
//终端参数结构体
typedef struct _Termnl_CFG_
{
	u8 DevID[12];//终端生产编号
	u8 HwVer[3]; //硬件版本
}Termnl_CFG;
//外设参数结构体
typedef struct _Perpher_CFG_
{
	u8 IO_Perpher[IOKindMax];	//ePara1：IO口IO_Define与外设Perpher_Define对应关系
	u8 Perpher_Cfg[PerpherKindMax]; //ePara2：IO_Define外设功能
	u32 Perpher_Func;			//设备支持的所有功能，位定义
	u32 PerpherStatus;			//外设正常 /损坏或无状态,与Perpher_Define从第1位开始对应
}Perpher_CFG;

void  app_LBSInit (void);
void Factory_Set(void);	//参数恢复出厂设置
u8 ReadPerpherCfg(u8 paraType,u8 eMode,u8 *Value);
u8 WritePerpherCfg(u8 paraType,u8 eMode,u8 Value,UpdateModeEnum UpdateMode);

#endif
