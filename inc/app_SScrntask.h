#ifndef _APP_SSCRN_TASK_H_
#define _APP_SSCRN_TASK_H_


/*֪ͨ�������¼���*/
#define AREA_INTO_FLAG  (1<<0) //������֪ͨ
#define AREA_OUT_FLAG	(1<<1) //������֪ͨ
#define AREA_SPEED_OVER	(1<<2) //�����ڳ���

#define SCHOOL_GET_ON_BUS		(1<<3)  //��ѧ�ϳ�״̬
#define SCHOOL_GET_OFF_BUS		(1<<4)  //��ѧ�³�״̬
#define HOME_GET_ON_BUS		(1<<5)  //��ѧ�ϳ�״̬
#define HOME_GET_OFF_BUS		(1<<6)  //��ѧ�³�״̬


/*��������*/
void App_TaskSScrnRcvManage(void *Para);
void App_TaskSScrnSendManage(void *Para);


#endif
