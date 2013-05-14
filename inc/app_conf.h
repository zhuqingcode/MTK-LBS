#ifndef APP_CONF_H
#define APP_CONF_H
#define SOFTWARE_VER	"V115TJ101" //"TJ_V100"	//����汾

#define APTS_ENABLE		0
/***************************����������****************************************/
/*IO�ӿ���Ŀ�� ��Ҫ�Ƿ��������*/
#define IOKindNum		7	//������ʹ�õ�IO����Ŀ ��С��PerpherIO_Define���ֵ
#define PerpherKindNum	30	//�ն�������Ŀ����С��Perpher_Define���ֵ

/*���蹤��״̬λ����,��enum _Perpher��Ӧ*/
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
/*--���蹦�ܲ�������--*/
//ȫ�������б�
#define LIST_SMSEnable				(1<<0) //���Ź�������
#define LIST_CallEnable				(1<<1) //�绰��������
#define LimitTelEnable				(1<<2) //���Ƶ绰����
#define CameraEnble					(1<<3) //��������
#define OneCardEnable				(1<<4) //һ��ͨ
#define UnloadSensorEnble			(1<<5) //ж�ϴ�����
#define ClkwiseSensorEnble			(1<<6) //�ߵ�ƽ��Ч
//#define AnticlkwiseSensorEnble		1<<7 //�ߵ�ƽ��Ч
#define Taximeter_photoEnble		(1<<7) //�Ƽ�����������
#define OnePerphFuncMaxNum		(8)	 //һ������Ĺ����������
//�ֱ�����������
#define SMSEnable					(1<<0) //���Ź�������
#define CallEnable					(1<<1) //�绰��������
#define LimitTelEnable				(1<<2) //���Ƶ绰����
//����ͷ����
#define CameraNum1					(0x01) //1·����ͷ
#define CameraNum2					(0x02) //2·����ͷ
#define CameraNum3					(0x03) //3·����ͷ
#define CameraNum4					(0x04) //���4·����ͷ

//�ӽ�����������
#define UnloadSensorHighEnble		(1<<0) //�ߵ�ƽ��Ч
//��ת����������
#define ClkwiseSensorHighEnble		(1<<0) //����ߵ�ƽ��Ч
#define AnticlkwiseSensorHighEnble	(1<<0) //����ߵ�ƽ��Ч
//�ӽ�����������
#define AnticlkwiseSensorHighEnble	(1<<0) //�س��ߵ�ƽ��Ч

#if 0
//�����������
typedef enum _UPDATE_MODE
{
	UpdateFlash,
	UpdateOnly
}UpdateModeEnum;
#endif
//�����������
typedef enum _Paratype
{
	ePara1=0x01,	//IO���������Ӧ��ϵ �ⲿ��ѯ������
	ePara2,			//���蹦�ܲ���		 �ⲿ��ѯ������
	ePara3,			//����״̬			 �ⲿ��ѯ
	ePara4			//���蹦��			�ⲿ��ѯ
} PARATYPE_Define;
//����ӿ�ö�٣���ӦPARA1����
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
	DVR_In1=7,  //3512��IO����
	DVR_In2=8,
	DVR_In3=9,
	DVR_In4=10,
#endif
	IOKindMax,
} IO_Define;
//��������ö��,��ӦPARA2����
typedef enum _Perpher
{//0x00������,���в�ѯ����ʱ��0x01��ʼ
	Camera=0x01,	//����ͷ
	SchedulScrn=2,	//D5����D6 ����û����
	PrintfUsart=3,
	UnloadSensor=4,	//ж�����������ӽ���������
	ClkwiseSensor=5,	//���򴫸���
	AnticlkwiseSensor=6,//���򴫸���
	HandDev1=7,	      //LH7.0	 
	LiDaICCardReader=8,// �������IC��������
	TaxiMeter=9,		//�Ƽ���
	HandDev0=10,		//һ��ʼ�õ��ֱ�
	LedDev1=11,		//���ع����
	LCDN990=12,		//N990��
	StudentCardReader=13,//ѧ��У����
	PowerAndOil_ctrl=14, //���Ͷϵ����
	Passenger=15,        //�ؿ�
	Door1Ctrl=16,		 //��������
	DDSCamera=17,		 //DDS˾��ƣ�ͼ��
	Mobileye=18,		 //��ȫ�г�
	PerpherKindMax,
}Perpher_Define;
//�ն˲����ṹ��
typedef struct _Termnl_CFG_
{
	u8 DevID[12];//�ն��������
	u8 HwVer[3]; //Ӳ���汾
}Termnl_CFG;
//��������ṹ��
typedef struct _Perpher_CFG_
{
	u8 IO_Perpher[IOKindMax];	//ePara1��IO��IO_Define������Perpher_Define��Ӧ��ϵ
	u8 Perpher_Cfg[PerpherKindMax]; //ePara2��IO_Define���蹦��
	u32 Perpher_Func;			//�豸֧�ֵ����й��ܣ�λ����
	u32 PerpherStatus;			//�������� /�𻵻���״̬,��Perpher_Define�ӵ�1λ��ʼ��Ӧ
}Perpher_CFG;

void  app_LBSInit (void);
void Factory_Set(void);	//�����ָ���������
u8 ReadPerpherCfg(u8 paraType,u8 eMode,u8 *Value);
u8 WritePerpherCfg(u8 paraType,u8 eMode,u8 Value,UpdateModeEnum UpdateMode);

#endif
