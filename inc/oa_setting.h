#ifndef _OA_SETTING_H_
#define _OA_SETTING_H_


#define SET_IPADDR_LEN 15
#define SET_HOST_NAME_LEN 63
#define SET_APNINFO_LEN 40
#define SET_APNNAME_LEN 16
#define SET_APNPASSWORD_LEN 16
#define SET_HOST_DNS_LEN 20


typedef struct
{
    //oa_uint8 dtuid[26+1];//ID
    
    oa_char serve_ipaddr[SET_IPADDR_LEN+1];//service ip address
    oa_char serve_host_name[SET_HOST_NAME_LEN+1];//service host name if no ip address
    oa_int32 port;//service port
    oa_char apninfo[SET_APNINFO_LEN+1];//
    oa_char apnname[SET_APNNAME_LEN+1];
    oa_char apnpassword[SET_APNPASSWORD_LEN+1];
    OA_SOCKET_TYPE connct_type; //0= TCP, 1 = UDP

    oa_uint8 checktail[2];
}oa_soc_set_parameter;

#define OA_SETTING_CNXT_SIZE (sizeof(oa_soc_set_parameter) - sizeof(oa_uint8)*2)//without checktail
#define OA_SETTING_CNXT_TOTAL_SIZE (sizeof(oa_soc_set_parameter))//with checktail

extern oa_soc_set_parameter g_soc_param;

void oa_soc_setting_init(void);
#define OA_SOC_SETTING_FILE L"soc_setting.ini"
#endif/*_OA_SETTING_H_*/
