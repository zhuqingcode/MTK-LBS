#ifndef _APP_SSCRN_TASK_H_
#define _APP_SSCRN_TASK_H_


/*通知调度屏事件项*/
#define AREA_INTO_FLAG  (1<<0) //进区域通知
#define AREA_OUT_FLAG	(1<<1) //出区域通知
#define AREA_SPEED_OVER	(1<<2) //区域内超速

#define SCHOOL_GET_ON_BUS		(1<<3)  //上学上车状态
#define SCHOOL_GET_OFF_BUS		(1<<4)  //上学下车状态
#define HOME_GET_ON_BUS		(1<<5)  //上学上车状态
#define HOME_GET_OFF_BUS		(1<<6)  //上学下车状态


/*程序声明*/
void App_TaskSScrnRcvManage(void *Para);
void App_TaskSScrnSendManage(void *Para);


#endif
