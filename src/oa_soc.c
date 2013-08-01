/*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  MobileTech(Shanghai) Co., Ltd.
 *
 *****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   oa_soc.c
 *
 * Project:
 * --------
 *   OPEN AT project
 *
 * Description:
 * ------------
 *   This module contains gprs socket connect for OPENAT.
 *
 * Author:
 * -------
 *   simon.
 *
 ****************************************************************************/
#include "oa_type.h"
#include "oa_api.h"
#include "oa_setting.h"
#include "oa_soc.h"
//------------customer code-------------
#include "oa_jt808.h"
#include "oa_platform.h"
#include "oa_debug.h"
#include "oa_app.h"
#include "oa_sms.h"
extern ProtocolHandle sProtclHandl;
extern action_kind plat_paraset;
extern dev_control_type control_type;
extern upgrade_paras up_paras;
extern timeout_struct timeout_var;
extern soc_bak_context back_con;
extern oa_bool try_unlock;
//-------------------------------------
/*debug*/
#define GPRS_DEBUG OA_DEBUG_USER

/*
* do not re-connect the network immediately after the old one closed.
* wait a period of time (for example, 5sec) to detach network,and then re-connect
*/
#define OA_GPRS_WAITING_RELEASING 2000//10000 /*mSec*/
#define OA_GPRS_WAITING_RESEND 2000
//Gprs heartbeat time, for keep gprs connecting, default 60s, if not use heartbeat, set OA_GPRS_HEARTBEAT_TIME = 0
#define OA_GPRS_HEARTBEAT_TIME 60

//Gprs rx/tx buffer length
#define OA_MAX_SOC_RCV_BUFLEN  2000
#define OA_MAX_SOC_SEND_BUFLEN  20480

oa_uint8 gprs_rx_buffer[OA_MAX_SOC_RCV_BUFLEN+1]={0}; // gprs接收数据缓冲区, 30K bytes
oa_uint8 gprs_tx_buffer[OA_MAX_SOC_SEND_BUFLEN+1]={0}; // gprs 发送缓冲区，gprs一次最大能发送1400 bytes

oa_soc_context g_soc_context = {0};
extern void oa_soc_set_apn_cb(oa_bool result);
extern void oa_soc_fill_addr_struct(void);
//------------customer code-------------
extern DEV_PLAT_PARAS dev_running;
//-------------------------------------
//for NOINIT buffer
#define OA_GPRS_BUFFER_NOINIT_SIZE (oa_sram_noinit_get_size() - (1024*50))//500k-50k?
void oa_soc_can_resend(void);
void oa_soc_notify_ind_user_callback(void *inMsg);
void oa_soc_init_fast(void)
{
	//init socket context
	oa_memset(&g_soc_context, 0, sizeof(g_soc_context));//add by sx for solving ftp problem@13/5/8
	g_soc_context.state = OA_SOC_STATE_OFFLINE;
	g_soc_context.socket_id = -1;
	g_soc_context.can_connect = OA_TRUE;
	g_soc_context.is_blocksend = OA_FALSE;
	g_soc_context.recon_counter = 0;

	oa_memcpy(g_soc_context.apn_info.apn, g_soc_param.apninfo, oa_strlen(g_soc_param.apninfo));
	oa_memcpy(g_soc_context.apn_info.userName, g_soc_param.apnname, oa_strlen(g_soc_param.apnname));
	oa_memcpy(g_soc_context.apn_info.password, g_soc_param.apnpassword, oa_strlen(g_soc_param.apnpassword));

#if 0	
	/*create gprs rx/tx ring buffers*/
	//g_soc_context.gprs_tx = oa_create_ring_buffer(gprs_tx_buffer, sizeof(gprs_tx_buffer));
	g_soc_context.gprs_rx = oa_create_ring_buffer(gprs_rx_buffer, sizeof(gprs_rx_buffer));
#endif
	
	oa_free_buffer_noinit(g_soc_context.gprs_tx);//zhuqing add here for freeing memory
	/*using noinitial buffer*/
	g_soc_context.gprs_tx = oa_create_ring_buffer_noinit(NULL, OA_GPRS_BUFFER_NOINIT_SIZE);

	//fill socket connect server address 
	oa_soc_fill_addr_struct();
	//register socket network notify event callback function 
	oa_soc_notify_ind_register(oa_soc_notify_ind_user_callback);
}


void oa_soc_init(void)
{
	//register network APN, only set APN success,can connect network
	//oa_gprs_set_apn_req(&g_soc_context.apn_info, oa_soc_set_apn_cb);
	oa_gprs_apn_init(&g_soc_context.apn_info);

	//register socket network notify event callback function 
	//oa_soc_notify_ind_register(oa_soc_notify_ind_user_callback);//zhuqing mv it to func 'oa_soc_init_fast'
}

extern oa_soc_set_parameter g_soc_param;
void oa_soc_fill_addr_struct(void)
{
	g_soc_context.addr_type = OA_IP_ADDRESS;
	g_soc_context.soc_addr.addr_len = 4;
	g_soc_context.soc_addr.port = g_soc_param.port;
	g_soc_context.soc_addr.sock_type = g_soc_param.connct_type;
	if(!oa_strcmp(g_soc_param.serve_ipaddr, (oa_char*)"0.0.0.0"))
	{
		g_soc_context.addr_type = OA_DOMAIN_NAME;
		oa_memset(g_soc_context.soc_addr.addr, 0x0, sizeof(g_soc_context.soc_addr.addr));
		oa_memcpy(g_soc_context.soc_addr.addr, g_soc_param.serve_host_name, oa_strlen(g_soc_param.serve_host_name));
	}
	else
	{
		g_soc_context.addr_type = OA_IP_ADDRESS;
		oa_memset(g_soc_context.soc_addr.addr, 0x0, sizeof(g_soc_context.soc_addr.addr));
		oa_memcpy(g_soc_context.soc_addr.addr, g_soc_param.serve_ipaddr, oa_strlen(g_soc_param.serve_ipaddr));
	}
}

void fill_soc(u8 *ip, u32 port)
{
	g_soc_param.port = port;
	oa_memset(g_soc_param.serve_ipaddr, 0x0, sizeof(g_soc_param.serve_ipaddr));
	oa_memcpy(g_soc_param.serve_ipaddr, ip, oa_strlen(ip));
}

void oa_soc_set_apn_cb(oa_bool result)
{
      DEBUG("%s:set apn ok! %s, %s, %s", __FILE__, __func__, g_soc_context.apn_info.apn, g_soc_context.apn_info.userName, g_soc_context.apn_info.password);
      g_soc_context.can_connect = OA_TRUE;
}

//Looped check the socket connect state. if offline and can connect, do connect request
void oa_soc_state_check(void)
{
	static oa_uint16 check_counter=0;
	check_counter++;

	if (g_soc_context.state == OA_SOC_STATE_ONLINE){
		if (check_counter >= 50){
			check_counter = 0;
			DEBUG("I am online");
		}
		return;	
	}
	
	if (g_soc_context.state == OA_SOC_STATE_OFFLINE && g_soc_context.can_connect){
		//------------zhuqing add code----------------
		g_soc_context.can_connect = OA_FALSE;
		//------------------------------------------
		oa_soc_connect_req();
		g_soc_context.recon_counter++;
	}
	else{
		g_soc_context.recon_counter = 0;
		//DEBUG(" not ready for connect or doing connecting");
	}
#if 0
	if(++check_counter>10 && g_soc_context.state == OA_SOC_STATE_ONLINE)
	{
	    oa_int32 retransmit_times = oa_soc_tcp_retransmit_times(g_soc_context.socket_id);

	    if(retransmit_times>3)
	    {/*warning: any data sent during a phone call, TCP layer will retransmit it
	       *  that will cause a nonsensical counter by oa_tcpip_get_tcp_retransmit_times after a call
	       */
	        GPRS_DEBUG("%s:retransmit too many times=%d!",__FILE__, __func__,retransmit_times);
	        oa_soc_close_req( );
	    }
	    check_counter = 0;
	}
#endif    
}

//check current connect state. if state == offline, do socket connect request
void oa_soc_connect_req(void)
{
	oa_int16 ret = OA_SOC_SUCCESS;

	if (g_soc_context.state == OA_SOC_STATE_ONLINE) return;

	if (g_soc_context.state == OA_SOC_STATE_OFFLINE)
	{
	      //fill the socket address first
	      oa_soc_fill_addr_struct( );

	      if (g_soc_context.soc_addr.sock_type == OA_SOCK_STREAM 
	      ||g_soc_context.soc_addr.sock_type == OA_SOCK_DGRAM )
	      {
			//create socket id firstly. socket id range(0-10), if ret id <0, it's error
			g_soc_context.socket_id = oa_soc_create(g_soc_context.soc_addr.sock_type, 0);
	      }
	      else
	      {
			//invalid sock_type, return
			DEBUG("%s:sock_type invalid!", __func__);
			return;
	      }

	      if (g_soc_context.socket_id >= 0)
	      {
		  	DEBUG("%s:sock_id=%d create ok!", __func__,g_soc_context.socket_id);
	      }
		else if (g_soc_context.socket_id < 0)
		{
			DEBUG("%s:socket create err!", __func__);
			return;
		}

	      DEBUG("Ready to connect:addr=%s,port=%d,soc_type=%d,addr_type=%d", 
	                               g_soc_context.soc_addr.addr, 
	                               g_soc_context.soc_addr.port, 
	                               g_soc_context.soc_addr.sock_type,
	                               g_soc_context.addr_type);
	      
	      //if create socket id success, goto socket connect request
	      ret = oa_soc_connect(g_soc_context.socket_id, g_soc_context.addr_type, &g_soc_context.soc_addr);


	      if(ret == OA_SOC_WOULDBLOCK)
	      {  
			DEBUG("%s:sock_id=%d connect block waiting!",__func__,g_soc_context.socket_id);
			//wait for connect indication.will received asynchronous info in oa_soc_notify_ind
			g_soc_context.state = OA_SOC_STATE_CONNECT;
	      }
	      else if(ret == OA_SOC_SUCCESS)
	      {
			DEBUG("%s:sock_id=%d connect ok!", __func__,g_soc_context.socket_id);
			//socket already connected
			//oa_sleep(100); //just connected need delay 100ms then send gprs data, else may lost data
			g_soc_context.state = OA_SOC_STATE_ONLINE;
			//oa_soc_send_req( );

			/*do not hearbeat currently*/
			//oa_soc_start_heartbeat_timer();
	      }
	      else
	      {
			DEBUG("%s:sock_id=%d connect fail ret=%d!",__func__,g_soc_context.socket_id,ret);
			oa_soc_close_req();
	      }
	  }

      return;
}


oa_int16 oa_soc_send_req(void)
{
	oa_uint16 len;
	oa_int16 ret;

	 /*if not online or sending state, return*/
	if (g_soc_context.state != OA_SOC_STATE_ONLINE
	||g_soc_context.is_blocksend == OA_TRUE)
	return 0;
	

	//once socket send, max len is 1024 bytes
	//query 'g_soc_context.gprs_tx',then fill 'g_soc_context.gprs_tx_pending_data' with the query datas to send
	len = oa_query_buffer_noinit(g_soc_context.gprs_tx,
							     g_soc_context.gprs_tx_pending_data, 
							     OA_APP_GPRS_SENDING_SIZE);
	g_soc_context.gprs_tx_pending_size = len;

	if (len > 0)
	{
		ret = oa_soc_send(g_soc_context.socket_id, g_soc_context.gprs_tx_pending_data, len);

		if (ret >= OA_SOC_SUCCESS)
		{
			//DEBUG("%s:sock_id=%d send ok len=%d",__func__,g_soc_context.socket_id,ret);
			//debug info
			{
				u16 i;
				DEBUG("sendlen:%d data:", len);
				for (i=0; i<len; i++){
					debug_no_n("%02x ", g_soc_context.gprs_tx_pending_data[i]);
				}
				DEBUG();
				
			}
			DEBUG("---send ok len=%d",ret);
			//timeout
			if (timeout_var.timeout_en == OA_TRUE){
				DEBUG("enable timeout");
				timeout_var.timeout_times = 0;
				if (timeout_var.do_timeout == OA_FALSE){//don't do duplicate copy
					oa_memset(&back_con, 0x0, sizeof(soc_bak_context));
					oa_memcpy(back_con.data, g_soc_context.gprs_tx_pending_data, len);//backup data
					back_con.len = len;
				}
				
			}
			
			//if send success,dummy read to delete
			len = oa_read_buffer_noinit(g_soc_context.gprs_tx, 
			    							g_soc_context.gprs_tx_pending_data,
			    							g_soc_context.gprs_tx_pending_size);
			g_soc_context.gprs_tx_pending_size = 0;

			//------------customer code-------------
			
			if (ret < len)
			{
				DEBUG(" send data is not equal!");
			}
			return ret;
			//----------------end------------------
			
			//check gprs_tx buff and continue send gprs data
			//oa_soc_send_req( );
			//oa_soc_start_heartbeat_timer();

		}
		else if (ret == OA_SOC_WOULDBLOCK)
		{
			g_soc_context.is_blocksend = OA_TRUE;
			DEBUG("%s:sock_id=%d send block waiting!",__func__,g_soc_context.socket_id);
			//wait for sending result , care event OA_SOC_WRITE in callback function oa_soc_notify_ind
			oa_evshed_start(OA_EVSHED_ID_1, oa_soc_can_resend, NULL, OA_GPRS_WAITING_RESEND);
			return 0;
		}
		else
		{
			//ERROR!.
			DEBUG("%s:sock_id=%d send fail ret=%d!",__func__,g_soc_context.socket_id,ret);
			//oa_soc_close_req();//commend by zq
			return 0;
		}
	}
	else
	{
		DEBUG("no data in send buffer");
		return -1;
	}
}

void oa_soc_can_resend(void){
	DEBUG("%s called", __func__);
	g_soc_context.is_blocksend = OA_FALSE;
}
void oa_soc_can_reconnect_again(void* param)
{
	DEBUG("%s called", __func__);
	g_soc_context.can_connect = OA_TRUE;
}

oa_bool oa_soc_close_req( void )
{
	oa_int8 ret = 0;

	//g_soc_context.can_connect = OA_FALSE;
	//g_soc_context.is_blocksend = OA_FALSE;

	if (g_soc_context.state == OA_SOC_STATE_OFFLINE)
	{
		DEBUG(" socket is already closed!");
		return OA_TRUE;
	}

	if (g_soc_context.socket_id >= 0)
		ret = oa_soc_close(g_soc_context.socket_id);

	if (ret)//close err!
	{
		DEBUG("%s:socket_id=%d,ret=%d", __func__,g_soc_context.socket_id, ret);
		return OA_FALSE;
	}

	g_soc_context.can_connect = OA_FALSE;
	g_soc_context.is_blocksend = OA_FALSE;
	g_soc_context.state = OA_SOC_STATE_OFFLINE;
	g_soc_context.socket_id = -1;
	//oa_sleep(300);//need delete here.
	//do not reconnect immediately
	oa_evshed_start(OA_EVSHED_ID_0, oa_soc_can_reconnect_again, NULL, OA_GPRS_WAITING_RELEASING);
	return OA_TRUE;
}

void oa_soc_gprs_recv(oa_uint8* data, oa_uint16 len)
{
	// received gprs data
	oa_uint8 ret;
	//DEBUG("len=%d", len);
	//debuf info
	{
		u16 i;
		DEBUG("receive len:%d data:", len);
		for(i=0; i<len; i++){
			debug_no_n("%02x ", data[i]);
		}
		DEBUG();
	}
	//gprs receive data analysis
	ret = JT808_recv_analysis(data, len);
	switch (ret){
		case PLAT_TREMREG_ACK:{
			switch (sProtclHandl.PlatComrsp.Rslt){
				case RspOK:{
					if (dev_running.plat_status == OFFLINE){
						dev_running.plat_switch = OA_TRUE;
						dev_running.next_step = PLAT_DEV_LOGIN;
					}
				}break;
				case RspError:
				case RspMsgerr:
				case RspUnsurport:
				case RspAlarmCheck:{
					if (!use_is_lock()){//unlocked
						if (dev_running.plat_status == OFFLINE){
							dev_running.plat_switch = OA_TRUE;
							dev_running.next_step = PLAT_DEV_REG;
						}
					}
					else{
						try_unlock = OA_FALSE;
						dev_running.plat_switch = OA_TRUE;
					}
				}break;
				case RspPackgerr:{
					dev_running.plat_switch = OA_TRUE;
					dev_running.next_step = PLAT_DEV_REG;
				}break;
				default:break;
			}
		}
		break;
		case PLAT_COMMON_ACK:{
			switch (sProtclHandl.PlatComrsp.Rslt){
				case RspOK:{
					//case authen
					if (dev_running.doing_what == authen && dev_running.plat_status == OFFLINE){
						dev_running.plat_switch = OA_FALSE;//鉴权后关闭登陆流程
						dev_running.plat_status = ONLINE;//鉴权后表示与平台连接上
						dev_running.doing_what = do_none;//means authen ok
						dev_running.authen_err_time = 0;
						//unlock
						if (use_is_lock()){
							use_unlock();
							try_unlock = OA_FALSE;
							DEBUG("Congratulations to you,device is unlocked!");
						}
					}
					//case unreg
					else if (dev_running.doing_what == unreg && dev_running.plat_status == ONLINE){
						DEBUG("unreg ok!");
						#if 0
						dev_running.plat_switch = OA_TRUE;//if unreg ok, go to reg,authen and so on
						dev_running.plat_status = OFFLINE;
						dev_running.doing_what = do_none;//means unreg ok
						dev_running.next_step = PLAT_DEV_REG;
						#endif
						just_reconn();
					}
					//maybe has something else to do here
					//eg.heartbeat
				}
				break;
				case RspError:
				case RspMsgerr:
				case RspUnsurport:{
					//case authen
					DEBUG("plat common ack fail : %d", sProtclHandl.PlatComrsp.Rslt);
					if (dev_running.doing_what == authen && dev_running.plat_status == OFFLINE){
						dev_running.plat_switch = OA_TRUE;//if authen err, do it again
						if (use_is_lock()){//locked
							try_unlock = OA_FALSE;
						}
						dev_running.authen_err_time++;
						if (dev_running.authen_err_time >= AUTHEN_ERR_MAX_TIMES){
							oa_bool tmp;
							dev_running.authen_err_time = 0;
							DEBUG("authen err too many times!so delete authen automatically");
							tmp = del_authcode();
							if (OA_TRUE == tmp){
								dev_running.next_step = PLAT_DEV_REG;
								dev_running.plat_switch = OA_TRUE;
							}
						}
					}
					//case unreg
					else if (dev_running.doing_what == unreg && dev_running.plat_status == ONLINE){
						dev_running.plat_switch = OA_TRUE;//if unreg err, do it again
					}
					//the same as above
					//......
				}
				break;
				case RspAlarmCheck:{

				}
				break;
				default:break;
			}
		}
		break;
		
		case RspPackgerr:{
			DEBUG(" plat ack err");
		}
		break;
		case PLAT_REQ:{
			//do something here
			//DEBUG("plat request!");
			if (plat_paraset == reconn){
				plat_paraset = 0;
				do_soc_reconn();
			}
			else if (plat_paraset == rereg){
				plat_paraset = 0;
				do_rereg();
			}

			if (none != control_type){
				switch (control_type){
					case wireless_update:{
						ftp_update(&up_paras);
					}
					break;
					case conn_to_specified_server:{
						conn2specific_server(&up_paras);
					}
					break;
					case term_powerdown:{
						do_powerdown();
					}
					break;
					case term_reset:{
						do_reset();
					}
					break;
					case factory_setting:{
						do_factory_set();
					}
					break;
					case turnoff_datatraffic:{//maybe need to store data to blinddata file
						do_turnoff_datatraffic();
					}
					break;
					case turnoff_allwireless:{
						do_turnoff_allwireless();
					}
					break;
					default:break;
				}

				control_type = none;//do not forget it

			}
			
		}
		break;
		default:break;		
	}
}

//Socket Notify Event indication handler framework
void oa_soc_notify_ind_user_callback(void *inMsg)
{
	oa_int32 ret = 0;
	oa_app_soc_notify_ind_struct *soc_notify = (oa_app_soc_notify_ind_struct*) inMsg;

	//if other application's socket id, ignore it.
	if(soc_notify->socket_id != g_soc_context.socket_id){
		DEBUG("%s:sock_id=%d unknow, event_type=%d!",__FILE__, __func__,soc_notify->socket_id,soc_notify->event_type);    
		return;
	}
    
	switch (soc_notify->event_type){
		case OA_SOC_WRITE:{
			/* Notify for send data*/
			if (soc_notify->result == OA_TRUE){
				  g_soc_context.is_blocksend = OA_FALSE;
				 //resend the data ,else will lost data
				  DEBUG("%s:sock_id=%d resend!",__FILE__, __func__,soc_notify->socket_id);
				  //oa_soc_send_req( );//comment by zq
				  //-----------customer code-----------
				  oa_soc_send_req();
				  //---------------end----------------
			}
			else{
				 DEBUG("%s:sock_id=%d send fail err=%d!",__FILE__, __func__,soc_notify->socket_id,soc_notify->error_cause);
				 oa_soc_close_req( );
			}     
		   
			break;
		}
	        
		case OA_SOC_READ:{
		    /* Notify for received data */
			if (soc_notify->result == OA_TRUE){
			//                do
			//                {
			        memset(gprs_rx_buffer, 0 , (OA_MAX_SOC_RCV_BUFLEN*sizeof(oa_uint8))); 
			        //received gprs data, read data for protocol
			        ret = oa_soc_recv(soc_notify->socket_id , (oa_uint8*)gprs_rx_buffer, OA_MAX_SOC_RCV_BUFLEN, 0);
			        //DEBUG("%s:sock_id=%d read ok len=%d", __func__,soc_notify->socket_id,ret);
				DEBUG("+++read ok len=%d", ret);
			        if(ret > 0){   
			            // read data length=ret, 
					oa_soc_gprs_recv((oa_uint8*)gprs_rx_buffer, ret);
			        }
			        else{
					DEBUG("%s:sock_id=%d read err!",__func__,soc_notify->socket_id);
			        }  
			//         }while(ret>0); //Make sure use  do{...}while  to read out all received data.
			}
			else{
				DEBUG("%s:sock_id=%d read fail err=%d!",__func__,soc_notify->socket_id,soc_notify->error_cause);
				oa_soc_close_req( );
			}     
			break;
		}
	        
		case OA_SOC_CONNECT:{
			if (soc_notify->result == OA_TRUE){
				oa_bool ret;
				DEBUG("%s:sock_id=%d connect ok!",__func__,soc_notify->socket_id);
				//if connect OK, goto query gprs_tx data and send
				//oa_sleep(100); //just connected need delay 100ms then send gprs data, else may lost data
				g_soc_context.state = OA_SOC_STATE_ONLINE;
				//oa_soc_send_req( );
				if (OFFLINE == dev_running.plat_status){//判断当前有没有和平台连接上
					if (!use_is_lock()){//unlocked
						ret = has_reg();//判断有没有注册过，如果已经注册就鉴权；没有注册才去注册
						if (OA_FALSE == ret){
							dev_running.plat_switch = OA_TRUE;
							dev_running.next_step = PLAT_DEV_REG;
						}
						else{
							dev_running.plat_switch = OA_TRUE;
							dev_running.next_step = PLAT_DEV_LOGIN;
						}
					}
					else{//locked means has not reged
						dev_running.plat_switch = OA_TRUE;
						dev_running.next_step = PLAT_DEV_REG;
					}
					
				}
			}
			else{
				DEBUG("%s:sock_id=%d connect fail err=%d!",__func__,soc_notify->socket_id,soc_notify->error_cause);
				oa_soc_close_req();
				if (use_is_lock()){
					try_unlock = OA_FALSE;
					dev_running.plat_switch = OA_TRUE;
					dev_running.next_step = PLAT_SOC_INIT;
				}
			}
			break;
		}       
	        
		case OA_SOC_CLOSE:{
			oa_bool ret;
			// the socket is closed by remote server, use soc_close to close local socketID, else will lead OA_SOC_LIMIT_RESOURCE
			//also callded by powerdown!
			DEBUG("sock_id=%d close causeid=%d", soc_notify->socket_id,soc_notify->error_cause);
			if (oa_soc_close_req()){
				if (!use_is_lock()){
					if (dev_running.plat_status == ONLINE){
						dev_running.plat_status = OFFLINE;
					}

					dev_running.plat_switch = OA_TRUE;
					dev_running.next_step = PLAT_SOC_INIT;
				}
				else{
					try_unlock = OA_FALSE;
				}
				
			}
			else{
				DEBUG(" soc close err!");
			}
			 
			break;
		}
	        
		case OA_SOC_ACCEPT:{
			DEBUG("sock_id=%d accept", soc_notify->socket_id);
		}
		break;
		    
		default:
			break;
	}

    //oa_soc_start_heartbeat_timer();
}


/************************************************************/
/********************** noinit buffer************************/
/************************************************************/
/*function to handle buffers */
//#define PTR_NOINIT_BUF_BEGIN 
oa_buffer *oa_create_ring_buffer_noinit(oa_uint8 *rawBuf, oa_uint16 maxLength)
{
	oa_buffer *temp_buf = (oa_buffer*)oa_sram_noinit_get_buf();

	//GPRS_DEBUG("[GPRS] LOOK BEFORE %X:%X", temp_buf->buf, ((oa_uint8*)temp_buf + sizeof(oa_buffer)));

	if(temp_buf->buf != (oa_uint8*)((oa_uint8*)temp_buf + sizeof(oa_buffer)))//first use SRAM
	{
		oa_sram_noinit_reset();
		temp_buf->buf = (oa_uint8*)((oa_uint8*)temp_buf + sizeof(oa_buffer));
		temp_buf->len = 0;
		temp_buf->offset = 0;
		temp_buf->maxLength = maxLength;
		GPRS_DEBUG("[GPRS] SRAM buffer first init! %X:%X\r\n", temp_buf->buf, ((oa_uint8*)temp_buf + sizeof(oa_buffer)));
	}
	else
	{
		GPRS_DEBUG("[GPRS] SRAM buffer inited before. datalen=%d\r\n", temp_buf->len);
	}

	return temp_buf;
}

void oa_free_buffer_noinit(oa_buffer *buf)
{
}

oa_uint16 oa_write_buffer_noinit(oa_buffer *buf, oa_uint8 *data, oa_uint16 len)
{
    oa_uint16 i, p;

	oa_buffer *temp_buf = (oa_buffer*)oa_sram_noinit_get_buf();
	if(temp_buf->buf != (oa_uint8*)((oa_uint8*)temp_buf + sizeof(oa_buffer)))//first use SRAM
	{
		return (oa_uint16)0;
	}

    if(buf->len + len > buf->maxLength)
    {
        len = buf->maxLength - buf->len;
    }

    for(i = 0, p = buf->offset + buf->len; i < len; i++, p++)
    {
        if(p >= buf->maxLength)
        {
            p -= buf->maxLength;
        }

        buf->buf[p] = data[i];
    }
  
    buf->len += len;
  
    return len;
}

oa_uint16 oa_query_buffer_noinit(oa_buffer *buf, oa_uint8 *data, oa_uint16 len)
{
    oa_uint16 i, p;
	
	oa_buffer *temp_buf = (oa_buffer*)oa_sram_noinit_get_buf();
	
	if(temp_buf->buf != (oa_uint8*)((oa_uint8*)temp_buf + sizeof(oa_buffer)))//first use SRAM
	{
		return (oa_uint16)0;
	}

    if(buf->len < len)
    {
        len = buf->len;
    }
  
    for(i = 0, p = buf->offset; i < len; i++, p++)
    {
        if(p >= buf->maxLength)
        {
            p -= buf->maxLength;
        }

        data[i] = buf->buf[p];
    }

    //for query, don't update buffer control struct.
    //buf->len -= len;
    //buf->offset = p;  

    return len;
}

oa_uint16 oa_read_buffer_noinit(oa_buffer *buf, oa_uint8 *data, oa_uint16 len)
{
    oa_uint16 l;
	
	oa_buffer *temp_buf = (oa_buffer*)oa_sram_noinit_get_buf();
	if(temp_buf->buf != (oa_uint8*)((oa_uint8*)temp_buf + sizeof(oa_buffer)))//first use SRAM
	{
		return (oa_uint16)0;
	}
	
    l = oa_query_buffer_noinit(buf, data, len);
  
    buf->len -= l;
    buf->offset += l;
    if(buf->offset > buf->maxLength)
        buf->offset -= buf->maxLength;

    return l;
}

oa_uint16 oa_write_buffer_force_noinit(oa_buffer *buf, oa_uint8 *data, oa_uint16 len)
{
    oa_uint16 i, p, cut_off=0, real_len;

	oa_buffer *temp_buf = (oa_buffer*)oa_sram_noinit_get_buf();
	if(temp_buf->buf != (oa_uint8*)((oa_uint8*)temp_buf + sizeof(oa_buffer)))//first use SRAM
	{
		return (oa_uint16)0;
	}

	real_len = len;
 
    if(buf->len + len > buf->maxLength)
    {
		cut_off = len - (buf->maxLength - buf->len);
        len = buf->maxLength - buf->len;
    }

    for(i = 0, p = buf->offset + buf->len; i < len; i++, p++)
    {
        if(p >= buf->maxLength)
        {
            p -= buf->maxLength;
        }

        buf->buf[p] = data[i];
    }
	
	if(cut_off)
	{
		
	    for(i = len, p = buf->offset; i < real_len; i++, p++)
	    {
	        if(p >= buf->maxLength)
	        {
	            p -= buf->maxLength;
	        }

	        buf->buf[p] = data[i];
	    }

		buf->offset += cut_off;
		if(buf->offset >= buf->maxLength)
			buf->offset -= buf->maxLength;
	}
	
	buf->len += real_len;
	if(buf->len > buf->maxLength)
		buf->len = buf->maxLength;
  
    return real_len;
}

