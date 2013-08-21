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
 *   oa_area.c
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
#include "oa_debug.h"
#include "oa_area.h"
#include "oa_blinddata.h"
#include "oa_gps.h"
#include "oa_jt808.h"
#include "oa_alarm.h"
#include "oa_dev_params.h"
extern DEVICE_PARAMS dev_now_params;
extern os_struct overspeed_var;
area_alarm_addition_struct area_alarm_addition_var = {{no_spec},{0},{0}};
 /*********************************************************
*Function:     has_areadata_dir_n_c()
*Description:  if system desn't have area data directory,create it 
*Return:		
*Others:         
*********************************************************/
oa_bool has_specific_file(const oa_wchar *file_name)
{
	oa_int32 handle, ret;
	
	handle = oa_fopen(file_name);
	if (handle < 0) return OA_FALSE;
	else{
		ret = oa_fclose(handle);
		if (ret < 0){
			DEBUG("ret:%d", ret);
			DEBUG("close err!");
			return OA_FALSE;
		}
		return OA_TRUE;	
	}
}
/*********************************************************
*Function:     r_w_circle_area_data_file()
*Description:  has specifie file? If doesn't have, create it
*Return:		
*Others:         
*********************************************************/
oa_bool r_w_circle_area_data_file(circle_area_item *p_item, r_w_enum r_w, u8 pos, u8 option)
{
	oa_int32 handle, ret;
	oa_bool ret_bool;
	oa_uint32 dummy_read, dummy_write;
	circle_area_item circle_area_var;
	oa_uint32 offset;
	u8 i;

	//read area num manage file
	if (NULL == p_item || pos >= MAX_AREA_SUM){
		DEBUG("params err!");
		return OA_FALSE;
	}
	
	handle = oa_fopen(CIRCLE_AREA_DATA);
	
	if (handle < 0){
		//create
		handle = oa_fcreate(CIRCLE_AREA_DATA);
		if (handle < 0) return OA_FALSE;
		//initial
		oa_memset(&circle_area_var, invalid, sizeof(circle_area_var));
		for (i=0; i<MAX_AREA_SUM; i++){
			ret = oa_fwrite(handle, &circle_area_var, sizeof(circle_area_var), &dummy_write);
			if ((ret < 0) || (dummy_write != sizeof(circle_area_var))){
				DEBUG("init file failed!");
				oa_fclose(handle);
				return OA_FALSE;
			}
		}
	}
	oa_memset(&circle_area_var, 0x0, sizeof(circle_area_var));
	if(r_w == file_read){
		offset = pos * sizeof(circle_area_item);
		oa_fseek(handle, offset, OA_FILE_BEGIN);
		ret = oa_fread(handle, &circle_area_var, sizeof(circle_area_item), &dummy_read);
		if ((ret < 0) || (dummy_read != sizeof(circle_area_item))){
			DEBUG("read err!");
			oa_fclose(handle);
			return OA_FALSE;
		}
		oa_memcpy(p_item, &circle_area_var, sizeof(circle_area_item));
		oa_fclose(handle);
		return OA_TRUE;
	}
	else if(r_w == file_write){
		//look for the invalid data & overwrite it
		for (i=0; i<MAX_AREA_SUM; i++){
			offset = i * sizeof(circle_area_item);
			oa_fseek(handle, offset, OA_FILE_BEGIN);
			ret = oa_fread(handle, &circle_area_var, sizeof(circle_area_item), &dummy_read);
			if ((ret < 0) || (dummy_read != sizeof(circle_area_item))){
				DEBUG("read err!");
				oa_fclose(handle);
				return OA_FALSE;
			}
			else{
				
				if (add_area == option){
					if (circle_area_var.is_valid == invalid){
						oa_fseek(handle, offset, OA_FILE_BEGIN);
						oa_memcpy(&circle_area_var, p_item, sizeof(circle_area_item));
						ret = oa_fwrite(handle, &circle_area_var, sizeof(circle_area_item), &dummy_write);
						if ((ret < 0) || (dummy_write != sizeof(circle_area_item))){
							DEBUG("write err!");
							oa_fclose(handle);
							return OA_FALSE;
						}
						oa_fclose(handle);
						return OA_TRUE;
					}
					else if (circle_area_var.area_id == p_item->area_id && circle_area_var.is_valid == valid){
						oa_fseek(handle, offset, OA_FILE_BEGIN);
						oa_memcpy(&circle_area_var, p_item, sizeof(circle_area_item));
						ret = oa_fwrite(handle, &circle_area_var, sizeof(circle_area_item), &dummy_write);
						if ((ret < 0) || (dummy_write != sizeof(circle_area_item))){
							DEBUG("write err!");
							oa_fclose(handle);
							return OA_FALSE;
						}
						oa_fclose(handle);
						return OA_TRUE;
					}
				}
				else if (alter_area == option){
					if (circle_area_var.area_id == p_item->area_id && circle_area_var.is_valid == valid){
						oa_fseek(handle, offset, OA_FILE_BEGIN);
						oa_memcpy(&circle_area_var, p_item, sizeof(circle_area_item));
						ret = oa_fwrite(handle, &circle_area_var, sizeof(circle_area_item), &dummy_write);
						if ((ret < 0) || (dummy_write != sizeof(circle_area_item))){
							DEBUG("write err!");
							oa_fclose(handle);
							return OA_FALSE;
						}
						oa_fclose(handle);
						return OA_TRUE;
					}
				}
			}
		}

		oa_fclose(handle);
		return OA_FALSE;
	}
}
/*********************************************************
*Function:     r_w_circle_area_data_file()
*Description:  has specifie file? If doesn't have, create it
*Return:		
*Others:         
*********************************************************/
oa_bool r_w_rect_area_data_file(rect_area_item *p_item, r_w_enum r_w, u8 pos, u8 option)
{
	oa_int32 handle, ret;
	oa_bool ret_bool;
	oa_uint32 dummy_read, dummy_write;
	rect_area_item rect_area_var;
	oa_uint32 offset;
	u8 i;

	//read area num manage file
	if (NULL == p_item || pos >= MAX_AREA_SUM){
		DEBUG("params err!");
		return OA_FALSE;
	}
	
	handle = oa_fopen(RECT_AREA_DATA);
	
	if (handle < 0){
		//create
		handle = oa_fcreate(RECT_AREA_DATA);
		if (handle < 0) return OA_FALSE;
		//initial
		oa_memset(&rect_area_var, invalid, sizeof(rect_area_var));
		for (i=0; i<MAX_AREA_SUM; i++){
			ret = oa_fwrite(handle, &rect_area_var, sizeof(rect_area_var), &dummy_write);
			if ((ret < 0) || (dummy_write != sizeof(rect_area_var))){
				DEBUG("init file failed!");
				oa_fclose(handle);
				return OA_FALSE;
			}
		}
	}
	oa_memset(&rect_area_var, 0x0, sizeof(rect_area_var));
	if(r_w == file_read){
		offset = pos * sizeof(rect_area_var);
		oa_fseek(handle, offset, OA_FILE_BEGIN);
		ret = oa_fread(handle, &rect_area_var, sizeof(rect_area_var), &dummy_read);
		if ((ret < 0) || (dummy_read != sizeof(rect_area_var))){
			DEBUG("read err!");
			oa_fclose(handle);
			return OA_FALSE;
		}
		oa_memcpy(p_item, &rect_area_var, sizeof(rect_area_var));
		oa_fclose(handle);
		return OA_TRUE;
	}
	else if(r_w == file_write){
		//look for the invalid data & overwrite it
		for (i=0; i<MAX_AREA_SUM; i++){
			offset = i * sizeof(rect_area_var);
			oa_fseek(handle, offset, OA_FILE_BEGIN);
			ret = oa_fread(handle, &rect_area_var, sizeof(rect_area_var), &dummy_read);
			if ((ret < 0) || (dummy_read != sizeof(rect_area_var))){
				DEBUG("read err!");
				oa_fclose(handle);
				return OA_FALSE;
			}
			else{
				
				if (add_area == option){
					if (rect_area_var.is_valid == invalid){
						oa_fseek(handle, offset, OA_FILE_BEGIN);
						oa_memcpy(&rect_area_var, p_item, sizeof(rect_area_var));
						ret = oa_fwrite(handle, &rect_area_var, sizeof(rect_area_var), &dummy_write);
						if ((ret < 0) || (dummy_write != sizeof(rect_area_var))){
							DEBUG("write err!");
							oa_fclose(handle);
							return OA_FALSE;
						}
						oa_fclose(handle);
						return OA_TRUE;
					}
					else if (rect_area_var.area_id == p_item->area_id && rect_area_var.is_valid == valid){
						oa_fseek(handle, offset, OA_FILE_BEGIN);
						oa_memcpy(&rect_area_var, p_item, sizeof(rect_area_var));
						ret = oa_fwrite(handle, &rect_area_var, sizeof(rect_area_var), &dummy_write);
						if ((ret < 0) || (dummy_write != sizeof(rect_area_var))){
							DEBUG("write err!");
							oa_fclose(handle);
							return OA_FALSE;
						}
						oa_fclose(handle);
						return OA_TRUE;
					}
				}
				else if (alter_area == option){
					if (rect_area_var.area_id == p_item->area_id && rect_area_var.is_valid == valid){
						oa_fseek(handle, offset, OA_FILE_BEGIN);
						oa_memcpy(&rect_area_var, p_item, sizeof(rect_area_var));
						ret = oa_fwrite(handle, &rect_area_var, sizeof(rect_area_var), &dummy_write);
						if ((ret < 0) || (dummy_write != sizeof(rect_area_var))){
							DEBUG("write err!");
							oa_fclose(handle);
							return OA_FALSE;
						}
						oa_fclose(handle);
						return OA_TRUE;
					}
				}
			}
		}

		oa_fclose(handle);
		return OA_FALSE;
	}
}
/*********************************************************
*Function:     r_w_circle_area_data_file()
*Description:  has specifie file? If doesn't have, create it
*Return:		
*Others:         
*********************************************************/
oa_bool r_w_poly_area_data_file(poly_area_item *p_item, r_w_enum r_w, u8 pos, u8 option)
{
	oa_int32 handle, ret;
	oa_bool ret_bool;
	oa_uint32 dummy_read, dummy_write;
	poly_area_item poly_area_var;
	oa_uint32 offset;
	u8 i;

	//read area num manage file
	if (NULL == p_item || pos >= MAX_AREA_SUM){
		DEBUG("params err!");
		return OA_FALSE;
	}
	
	handle = oa_fopen(POLY_AREA_DATA);
	
	if (handle < 0){
		//create
		handle = oa_fcreate(POLY_AREA_DATA);
		if (handle < 0) return OA_FALSE;
		//initial
		oa_memset(&poly_area_var, invalid, sizeof(poly_area_var));
		for (i=0; i<MAX_AREA_SUM; i++){
			ret = oa_fwrite(handle, &poly_area_var, sizeof(poly_area_var), &dummy_write);
			if ((ret < 0) || (dummy_write != sizeof(poly_area_var))){
				DEBUG("init file failed!");
				oa_fclose(handle);
				return OA_FALSE;
			}
		}
	}
	oa_memset(&poly_area_var, 0x0, sizeof(poly_area_var));
	if(r_w == file_read){
		offset = pos * sizeof(poly_area_var);
		oa_fseek(handle, offset, OA_FILE_BEGIN);
		ret = oa_fread(handle, &poly_area_var, sizeof(poly_area_var), &dummy_read);
		if ((ret < 0) || (dummy_read != sizeof(poly_area_var))){
			DEBUG("read err!");
			oa_fclose(handle);
			return OA_FALSE;
		}
		oa_memcpy(p_item, &poly_area_var, sizeof(poly_area_var));
		oa_fclose(handle);
		return OA_TRUE;
	}
	else if(r_w == file_write){
		//look for the invalid data & overwrite it
		for (i=0; i<MAX_AREA_SUM; i++){
			offset = i * sizeof(poly_area_var);
			oa_fseek(handle, offset, OA_FILE_BEGIN);
			ret = oa_fread(handle, &poly_area_var, sizeof(poly_area_var), &dummy_read);
			if ((ret < 0) || (dummy_read != sizeof(poly_area_var))){
				DEBUG("read err!");
				oa_fclose(handle);
				return OA_FALSE;
			}
			else{
				
				if (add_area == option){
					if (poly_area_var.is_valid == invalid){
						oa_fseek(handle, offset, OA_FILE_BEGIN);
						oa_memcpy(&poly_area_var, p_item, sizeof(poly_area_var));
						ret = oa_fwrite(handle, &poly_area_var, sizeof(poly_area_var), &dummy_write);
						if ((ret < 0) || (dummy_write != sizeof(poly_area_var))){
							DEBUG("write err!");
							oa_fclose(handle);
							return OA_FALSE;
						}
						oa_fclose(handle);
						return OA_TRUE;
					}
					else if (poly_area_var.area_id == p_item->area_id && poly_area_var.is_valid == valid){
						oa_fseek(handle, offset, OA_FILE_BEGIN);
						oa_memcpy(&poly_area_var, p_item, sizeof(poly_area_var));
						ret = oa_fwrite(handle, &poly_area_var, sizeof(poly_area_var), &dummy_write);
						if ((ret < 0) || (dummy_write != sizeof(poly_area_var))){
							DEBUG("write err!");
							oa_fclose(handle);
							return OA_FALSE;
						}
						oa_fclose(handle);
						return OA_TRUE;
					}
				}
				else if (alter_area == option){
					if (poly_area_var.area_id == p_item->area_id && poly_area_var.is_valid == valid){
						oa_fseek(handle, offset, OA_FILE_BEGIN);
						oa_memcpy(&poly_area_var, p_item, sizeof(poly_area_var));
						ret = oa_fwrite(handle, &poly_area_var, sizeof(poly_area_var), &dummy_write);
						if ((ret < 0) || (dummy_write != sizeof(poly_area_var))){
							DEBUG("write err!");
							oa_fclose(handle);
							return OA_FALSE;
						}
						oa_fclose(handle);
						return OA_TRUE;
					}
				}
			}
		}

		oa_fclose(handle);
		return OA_FALSE;
	}
}
/*********************************************************
*Function:     handle_circle_area()
*Description:  has specifie file? If doesn't have, create it
*Return:		
*Others:         
*********************************************************/
oa_bool write_circle_area_data(u8 *buf, u16 *read_len, u8 option)
{
	oa_int32 handle, ret, i;
	u8 illegal = 0x0;
	u32 u32temp;
	u16 u16temp;
	circle_area_item circle_area_var;
	
	if (NULL == buf || NULL == read_len){
		DEBUG("params err!");
		return OA_FALSE;
	}
	//------------------------------------------------
	//area id
	char_to_int(buf, &u32temp);
	circle_area_var.area_id = u32temp;
	//area property
	char_to_short(buf+4, &u16temp);
	oa_memcpy((u8 *)&circle_area_var.area_para, (u8 *)&u16temp, 2);
	//latitude
	char_to_int(buf+6, &u32temp);
	circle_area_var.center_point_lat = u32temp;
	//longtitude
	char_to_int(buf+10, &u32temp);
	circle_area_var.center_point_lon = u32temp;
	//radius
	char_to_int(buf+14, &u32temp);
	circle_area_var.radius = u32temp;
	//
	if (circle_area_var.area_para.depend_time){
		//start time
		oa_memcpy(circle_area_var.start_time, buf+18, 6);
		//stop time
		oa_memcpy(circle_area_var.stop_time, buf+24, 6);
		//exist these word
		if (circle_area_var.area_para.speed_limit){
			//the most high speed
			char_to_short(buf+30, &u16temp);
			circle_area_var.max_speed = u16temp;
			//continuous time
			circle_area_var.continue_time = *(buf+32);
			//total length
			*read_len = 33;
		}
		else{//doesn't exist
			oa_memset((u8 *)&circle_area_var.max_speed, illegal, 2);
			circle_area_var.continue_time = illegal;
			*read_len = 30;
		}
	}
	else{
		//start time
		oa_memset(circle_area_var.start_time, illegal, 6);
		//stop time
		oa_memset(circle_area_var.stop_time, illegal, 6);
		//exist these word
		if (circle_area_var.area_para.speed_limit){
			//max speed
			char_to_short(buf+18, &u16temp);
			circle_area_var.max_speed = u16temp;
			//continuous time
			circle_area_var.continue_time = *(buf+20);
			//total length
			*read_len = 21;
		}
		else{//doesn't exist
			oa_memset((u8 *)&circle_area_var.max_speed, illegal, 2);
			circle_area_var.continue_time = illegal;
			*read_len = 18;
		}
	}
	OA_DEBUG_USER("区域ID:%d 中心点纬度:%f 中心点经度:%f 半径:%d", circle_area_var.area_id, (float)circle_area_var.center_point_lat/1000000, 
						(float)circle_area_var.center_point_lon/1000000, 
						circle_area_var.radius);
	//-----------------------------------------------
	circle_area_var.is_valid = valid;
	ret = r_w_circle_area_data_file(&circle_area_var, file_write, 0/*has no effect*/, option);
	if (OA_TRUE == ret) return OA_TRUE;
	else if (OA_FALSE == ret && option == alter_area){
		option = add_area;
		ret = r_w_circle_area_data_file(&circle_area_var, file_write, 0/*has no effect*/, option);
		if (OA_TRUE == ret) return OA_TRUE;
		else return OA_FALSE;
	}
	else return OA_FALSE;
}

/*********************************************************
*Function:     write_rect_area_data()
*Description:  has specifie file? If doesn't have, create it
*Return:		
*Others:         
*********************************************************/
oa_bool write_rect_area_data(u8 *buf, u16 *read_len, u8 option)
{
	oa_int32 handle, ret, i;
	u8 illegal = 0x0;
	u32 u32temp;
	u16 u16temp;
	rect_area_item rect_area_var;
	
	if (NULL == buf || NULL == read_len){
		DEBUG("params err!");
		return OA_FALSE;
	}
	//------------------------------------------------
	//area id
	char_to_int(buf, &u32temp);
	rect_area_var.area_id = u32temp;
	//area property
	char_to_short(buf+4, &u16temp);
	oa_memcpy((u8 *)&rect_area_var.area_para, (u8 *)&u16temp, 2);
	//left up latitude
	char_to_int(buf+6, &u32temp);
	rect_area_var.left_up_lat = u32temp;
	//left up longtitude
	char_to_int(buf+10, &u32temp);
	rect_area_var.left_up_lon = u32temp;
	//right down latitude
	char_to_int(buf+14, &u32temp);
	rect_area_var.right_down_lat = u32temp;
	//right down longtitude
	char_to_int(buf+18, &u32temp);
	rect_area_var.right_down_lon = u32temp;
	//this bit is effective
	if (rect_area_var.area_para.depend_time){
		//start time
		oa_memcpy(rect_area_var.start_time, buf+22, 6);
		//stop time
		oa_memcpy(rect_area_var.stop_time, buf+28, 6);
		//exist these word
		if (rect_area_var.area_para.speed_limit){
			//max speed
			char_to_short(buf+34, &u16temp);
			rect_area_var.max_speed = u16temp;
			//continuous time
			rect_area_var.continue_time = *(buf+36);
			//total length
			*read_len = 37;
		}
		else{//doesn't exist
			oa_memset((u8 *)&rect_area_var.max_speed, illegal, 2);
			rect_area_var.continue_time = illegal;
			*read_len = 34;
		}
	}
	else{
		//start time
		oa_memset(rect_area_var.start_time, illegal, 6);
		//stop time
		oa_memset(rect_area_var.stop_time, illegal, 6);
		//exist these word
		if (rect_area_var.area_para.speed_limit){
			//max speed
			char_to_short(buf+22, &u16temp);
			rect_area_var.max_speed = u16temp;
			//continuous time
			rect_area_var.continue_time = *(buf+24);
			//total length
			*read_len = 25;
		}
		else{//doesn't exist
			oa_memset((u8 *)&rect_area_var.max_speed, illegal, 2);
			rect_area_var.continue_time = illegal;
			*read_len = 22;
		}
	}
	DEBUG("区域ID:%d", rect_area_var.area_id);
	OA_DEBUG_USER("左上点:纬度:%f 经度:%f", (float)rect_area_var.left_up_lat/1000000, (float)rect_area_var.left_up_lon/1000000);
	OA_DEBUG_USER("右下点:纬度:%f 经度:%f", (float)rect_area_var.right_down_lat/1000000, (float)rect_area_var.right_down_lon/1000000);
	//-----------------------------------------------
	rect_area_var.is_valid = valid;
	ret = r_w_rect_area_data_file(&rect_area_var, file_write, 0/*has no effect*/, option);
	if (OA_TRUE == ret) return OA_TRUE;
	else if (OA_FALSE == ret && option == alter_area){
		option = add_area;
		ret = r_w_rect_area_data_file(&rect_area_var, file_write, 0/*has no effect*/, option);
		if (OA_TRUE == ret) return OA_TRUE;
		else return OA_FALSE;
	}
	else return OA_FALSE;
}
/*********************************************************
*Function:     write_poly_area_data()
*Description:  has specifie file? If doesn't have, create it
*Return:		
*Others:         
*********************************************************/
oa_bool write_poly_area_data(u8 *buf, u16 *read_len, u8 option)
{
	oa_int32 handle, ret, i;
	u8 illegal = 0x0;
	u32 u32temp;
	u16 u16temp;
	u32 temp0, temp1;
	poly_area_item poly_area_var;
	
	if (NULL == buf || NULL == read_len){
		DEBUG("params err!");
		return OA_FALSE;
	}
	//------------------------------------------------
	//area id
	char_to_int(buf, &u32temp);
	poly_area_var.area_id = u32temp;
	//area property
	char_to_short(buf+4, &u16temp);
	oa_memcpy((u8 *)&poly_area_var.area_para, (u8 *)&u16temp, 2);
	//
	if (poly_area_var.area_para.depend_time){
		//start time
		oa_memcpy(poly_area_var.start_time, buf+6, 6);
		//stop time
		oa_memcpy(poly_area_var.stop_time, buf+12, 6);
		//exist these word
		if (poly_area_var.area_para.speed_limit){
			//the most high speed
			char_to_short(buf+18, &u16temp);
			poly_area_var.max_speed = u16temp;
			//continuous time
			poly_area_var.continue_time = *(buf+20);
			//total area points
			char_to_short(buf+21,&u16temp);
			if (u16temp > MAX_AREA_POINTS || u16temp == 0){
				DEBUG("too many points");
				return OA_FALSE;
			}
			else poly_area_var.total_point = u16temp;
			for (i=0; i<u16temp; i++){
				
				char_to_int(buf+23+i*8, &temp0);
				char_to_int(buf+27+i*8, &temp1);
				poly_area_var.vertax[i].vertax_lat = temp0;
				poly_area_var.vertax[i].vertax_lon = temp1;
			}
			//total length
			*read_len = 23 + u16temp * 8;
		}
		else{//doesn't exist
			oa_memset((u8 *)&poly_area_var.max_speed, illegal, 2);
			poly_area_var.continue_time = illegal;
			//total area points
			char_to_short(buf+18,&u16temp);
			if (u16temp > MAX_AREA_POINTS || u16temp == 0){
				DEBUG("too many points");
				return OA_FALSE;
			}
			else poly_area_var.total_point = u16temp;
			for (i=0; i<u16temp; i++){
				char_to_int(buf+20+i*8, &temp0);
				char_to_int(buf+24+i*8, &temp1);
				poly_area_var.vertax[i].vertax_lat = temp0;
				poly_area_var.vertax[i].vertax_lon = temp1;
			}
			//total length
			*read_len = 20 + u16temp * 8;
		}
	}
	else{
		//start time
		oa_memset(poly_area_var.start_time, illegal, 6);
		//stop time
		oa_memset(poly_area_var.stop_time, illegal, 6);
		//exist these word
		if (poly_area_var.area_para.speed_limit){
			//max speed
			char_to_short(buf+6, &u16temp);
			poly_area_var.max_speed = u16temp;
			//continuous time
			poly_area_var.continue_time = *(buf+8);
			//total area points
			char_to_short(buf+9,&u16temp);
			if (u16temp > MAX_AREA_POINTS || u16temp == 0){
				DEBUG("too many points");
				return OA_FALSE;
			}
			else poly_area_var.total_point = u16temp;
			for (i=0; i<u16temp; i++){
				char_to_int(buf+11+i*8, &temp0);
				char_to_int(buf+15+i*8, &temp1);
				poly_area_var.vertax[i].vertax_lat = temp0;
				poly_area_var.vertax[i].vertax_lon = temp1;
			}
			//total length
			*read_len = 11 + u16temp * 8;
		}
		else{//doesn't exist
			oa_memset((u8 *)&poly_area_var.max_speed, illegal, 2);
			poly_area_var.continue_time = illegal;
			//total area points
			char_to_short(buf+6,&u16temp);
			if (u16temp > MAX_AREA_POINTS || u16temp == 0){
				DEBUG("too many points");
				return OA_FALSE;
			}
			else poly_area_var.total_point = u16temp;
			for (i=0; i<u16temp; i++){
				char_to_int(buf+8+i*8, &temp0);
				char_to_int(buf+12+i*8, &temp1);
				poly_area_var.vertax[i].vertax_lat = temp0;
				poly_area_var.vertax[i].vertax_lon = temp1;
			}
			*read_len = 8 + i * 8;
		}
	}
	for(i=0; i<poly_area_var.total_point; i++){
		OA_DEBUG_USER("第%d 个顶点:纬度:%f 经度:%f", i+1, (float)poly_area_var.vertax[i].vertax_lat/1000000, 
															(float)poly_area_var.vertax[i].vertax_lon/1000000);
	}
	//-----------------------------------------------
	poly_area_var.is_valid = valid;
	ret = r_w_poly_area_data_file(&poly_area_var, file_write, 0/*has no effect*/, option);
	if (OA_TRUE == ret) return OA_TRUE;
	else if (OA_FALSE == ret && option == alter_area){
		option = add_area;
		ret = r_w_poly_area_data_file(&poly_area_var, file_write, 0/*has no effect*/, option);
		if (OA_TRUE == ret) return OA_TRUE;
		else return OA_FALSE;
	}
	else return OA_FALSE;
}
/*********************************************************
*Function:     write_area_data()
*Description:  write area data to file
*Return:		
*Others:         
*********************************************************/
u8 write_area_data(u8 *buf, Area_Type_enum area_type, u16 *read_len, u8 option)
{
	oa_bool ret;
	
	if (NULL == buf || NULL == read_len){
		DEBUG("buf/len err");
		return OA_FALSE;
	}
#if 0
	ret = has_areadata_dir();
	if (!ret){//doesn't exist dir
		DEBUG("doesn't exist areadata dir!");
		return OA_FALSE;
	}
#endif
	switch (area_type){
		case Circular_Area:{
			ret = write_circle_area_data(buf, read_len, option);
		}break;
		case Rectangle_Area:{
			ret = write_rect_area_data(buf, read_len, option);
		}break;
		case Poly_Area:{
			ret = write_poly_area_data(buf, read_len, option);
		}break;
		default:{
			DEBUG("area's type is err!");
		}break;
	}

	if (OA_TRUE == ret) return 0;
	else return 1;

}
/*********************************************************
*Function:     del_area_data()
*Description:  write area data to file
*Return:		
*Others:         
*********************************************************/
u8 del_area_data(u32 area_id, Area_Type_enum area_type, del_option_enum del_option)
{
	oa_int32 handle, ret;
	oa_uint32 dummy_read, dummy_write;
	u8 i;
	oa_uint32 offset;
	circle_area_item circle_area_var;
	rect_area_item rect_area_var;
	poly_area_item poly_area_var;
#if 0	
	ret = has_areadata_dir();
	if (!ret){//doesn't exist dir
		DEBUG("doesn't exist areadata dir!");
		return 1;
	}
#endif
	switch (area_type){
		case Circular_Area:{
			handle = oa_fopen(CIRCLE_AREA_DATA);
		}break;
		case Rectangle_Area:{
			handle = oa_fopen(RECT_AREA_DATA);
		}break;
		case Poly_Area:{
			handle = oa_fopen(POLY_AREA_DATA);
		}break;
		default:{
			DEBUG("area's type is err!");
			return 1;
		}break;
	}
	
	if (handle < 0) return 0;//doesn't exist = delete ok

	switch (area_type){
		case Circular_Area:{
			for (i=0; i<MAX_AREA_SUM; i++){
				offset = i * sizeof(circle_area_item);
				oa_fseek(handle, offset, OA_FILE_BEGIN);
				ret = oa_fread(handle, &circle_area_var, sizeof(circle_area_item), &dummy_read);
				if ((ret < 0) || (dummy_read != sizeof(circle_area_item))){
					DEBUG("read err!");
					goto fail;
				}
				else{
					if (one_area == del_option){
						if (circle_area_var.area_id == area_id && circle_area_var.is_valid == valid){
							circle_area_var.is_valid = invalid;
							oa_fseek(handle, offset, OA_FILE_BEGIN);
							ret = oa_fwrite(handle, &circle_area_var, sizeof(circle_area_item), &dummy_write);
							if ((ret < 0) || (dummy_write != sizeof(circle_area_item))){
								DEBUG("write err!");
								goto fail;
							}
							oa_fclose(handle);
							return 0;
						}
						else if (circle_area_var.area_id == area_id && circle_area_var.is_valid == invalid){
							oa_fclose(handle);
							return 0;
						}
					}
					else if (all_area == del_option){
						if (circle_area_var.is_valid == valid){
							circle_area_var.is_valid = invalid;
							oa_fseek(handle, offset, OA_FILE_BEGIN);
							ret = oa_fwrite(handle, &circle_area_var, sizeof(circle_area_item), &dummy_write);
							if ((ret < 0) || (dummy_write != sizeof(circle_area_item))){
								DEBUG("write err!");
								goto fail;
							}
						}
						continue;
					}
				}
			}
		}break;
		case Rectangle_Area:{
			for (i=0; i<MAX_AREA_SUM; i++){
				offset = i * sizeof(rect_area_var);
				oa_fseek(handle, offset, OA_FILE_BEGIN);
				ret = oa_fread(handle, &rect_area_var, sizeof(rect_area_var), &dummy_read);
				if ((ret < 0) || (dummy_read != sizeof(rect_area_var))){
					DEBUG("read err!");
					goto fail;
				}
				else{
					if (one_area == del_option){
						if (rect_area_var.area_id == area_id && rect_area_var.is_valid == valid){
							rect_area_var.is_valid = invalid;
							oa_fseek(handle, offset, OA_FILE_BEGIN);
							ret = oa_fwrite(handle, &rect_area_var, sizeof(rect_area_var), &dummy_write);
							if ((ret < 0) || (dummy_write != sizeof(rect_area_var))){
								DEBUG("write err!");
								goto fail;
							}
							oa_fclose(handle);
							return 0;
						}
						else if (rect_area_var.area_id == area_id && rect_area_var.is_valid == invalid){
							oa_fclose(handle);
							return 0;
						}
					}
					else if (all_area == del_option){
						if (rect_area_var.is_valid == valid){
							rect_area_var.is_valid = invalid;
							oa_fseek(handle, offset, OA_FILE_BEGIN);
							ret = oa_fwrite(handle, &rect_area_var, sizeof(rect_area_var), &dummy_write);
							if ((ret < 0) || (dummy_write != sizeof(rect_area_var))){
								DEBUG("write err!");
								goto fail;
							}
						}
						continue;
					}
				}
			}
		}break;
		case Poly_Area:{
			for (i=0; i<MAX_AREA_SUM; i++){
				offset = i * sizeof(poly_area_var);
				oa_fseek(handle, offset, OA_FILE_BEGIN);
				ret = oa_fread(handle, &poly_area_var, sizeof(poly_area_var), &dummy_read);
				if ((ret < 0) || (dummy_read != sizeof(poly_area_var))){
					DEBUG("read err!");
					goto fail;
				}
				else{
					if (one_area == del_option){
						if (poly_area_var.area_id == area_id && poly_area_var.is_valid == valid){
							poly_area_var.is_valid = invalid;
							oa_fseek(handle, offset, OA_FILE_BEGIN);
							ret = oa_fwrite(handle, &poly_area_var, sizeof(poly_area_var), &dummy_write);
							if ((ret < 0) || (dummy_write != sizeof(poly_area_var))){
								DEBUG("write err!");
								goto fail;
							}
							oa_fclose(handle);
							return 0;
						}
						else if (poly_area_var.area_id == area_id && poly_area_var.is_valid == invalid){
							oa_fclose(handle);
							return 0;
						}
					}
					else if (all_area == del_option){
						if (poly_area_var.is_valid == valid){
							poly_area_var.is_valid = invalid;
							oa_fseek(handle, offset, OA_FILE_BEGIN);
							ret = oa_fwrite(handle, &poly_area_var, sizeof(poly_area_var), &dummy_write);
							if ((ret < 0) || (dummy_write != sizeof(poly_area_var))){
								DEBUG("write err!");
								goto fail;
							}
						}
						continue;
					}
					
				}
			}
		}break;
		default:{
			DEBUG("area's type is err!");
			return 1;
		}break;
	}
	//do not find = delete ok
	DEBUG("不存在此ID的区域");
	oa_fclose(handle);
	return 0;

fail:	oa_fclose(handle);    
	return 1;
}
/*********************************************************
*Function:     circle_area_inout_judge()
*Description:  circle_area_inout_judge
*Return:		
*Others:         
*********************************************************/
void circle_area_inout_judge(u32 lat, u32 lon, u8 *time, area_status_enum *p_cur, 
								u32 *p_id, u16 speed, u32 *os_bit, u8 *os_time, Area_Para_struct *p_pro)
{
	oa_bool ret;
	u8 i, i_o, res;
	circle_area_item circle_area_var;
	Cir_Area_Desc circle_area_desc;

	if (has_specific_file(CIRCLE_AREA_DATA)){
		//handle circle area
		for (i=0; i<MAX_AREA_SUM; i++){
			oa_memset(&circle_area_var, 0x0, sizeof(circle_area_var));
			oa_memset(&circle_area_desc, 0x0, sizeof(circle_area_desc));
			//read circle area data
			ret = r_w_circle_area_data_file(&circle_area_var, file_read, i, none_area);
			if (OA_TRUE == ret && circle_area_var.is_valid == valid){
				//copy data
				circle_area_desc.lat = circle_area_var.center_point_lat;
				circle_area_desc.lon = circle_area_var.center_point_lon;
				circle_area_desc.rad = circle_area_var.radius;
				p_id[i] = circle_area_var.area_id;
				oa_memcpy(p_pro + i, &circle_area_var.area_para, sizeof(circle_area_var.area_para));
				//DEBUG("read lat:%d lon:%d rad:%d id:%d", circle_area_desc.lat, circle_area_desc.lon, circle_area_desc.rad, circle_area_var.area_id);
				//compare
				if (1 == circle_area_var.area_para.depend_time){
					//rtc is ok?
					if (time[0] == 0x0 && time[1] == 0x0 && time[2] == 0x0) continue;
					//if it is cycle
					if (circle_area_var.start_time[0] == 0x0 && circle_area_var.stop_time[0] == 0x0){//circle time
						if (circle_area_var.start_time[1] != 0x0 && circle_area_var.stop_time[1] != 0x0)
							res = CompareTime(circle_area_var.start_time, circle_area_var.stop_time, time, 1);
						else if (circle_area_var.start_time[2] != 0x0 && circle_area_var.stop_time[2] != 0x0)
							res = CompareTime(circle_area_var.start_time, circle_area_var.stop_time, time, 2);
						else res = CompareTime(circle_area_var.start_time, circle_area_var.stop_time, time, 3);
					}
					else{//if it is not cycle
						res = CompareTime(circle_area_var.start_time, circle_area_var.stop_time, time, 0);
						if (!res){
							DEBUG("delete one circle area");
							del_area_data(circle_area_var.area_id, Circular_Area, one_area);
						}
						else if (res == 2) res = 0;
					}
					if (0 == res) continue;
					else if (1 == res){
						i_o = Circular_Judge(lon, lat, &circle_area_desc);
						//DEBUG("i_o:%d", i_o);
						//inside
						if (1 == i_o){
							p_cur[i] = area_inside;
							if (circle_area_var.area_para.speed_limit){
								if (speed > circle_area_var.max_speed){
									os_time[i] = circle_area_var.continue_time;
									*os_bit |= (1<<i);
								}
							}
						}
						else{//outside
							p_cur[i] = area_outside;
						}
					}
				}
				else{
					i_o = Circular_Judge(lon, lat, &circle_area_desc);
					//DEBUG("i_o:%d", i_o);
					//inside
					if (1 == i_o){
						p_cur[i] = area_inside;
						if (circle_area_var.area_para.speed_limit){
							if (speed > circle_area_var.max_speed){
								os_time[i] = circle_area_var.continue_time;
								*os_bit |= (1<<i);	
							}
						}
					}
					else{//outside
						p_cur[i] = area_outside;
					}
				}
			}
			else if (circle_area_var.is_valid == invalid){//area is invalid
				p_cur[i] = area_err;
			}
		}
	}
	
	return;
}

/*********************************************************
*Function:     area_inout_judge()
*Description:  write area data to file
*Return:		
*Others:         
*********************************************************/
void rect_area_inout_judge(u32 lat, u32 lon, u8 *time, area_status_enum *p_cur, 
									u32 *p_id, u16 speed, u32 *os_bit, u8 *os_time, Area_Para_struct *p_pro)
{
	oa_bool ret;
	u8 i, i_o, res;
	rect_area_item rect_area_var;
	Poly_Area_Desc rect_area_desc;
	
	if (has_specific_file(RECT_AREA_DATA)){
		//handle rect area
		for (i=0; i<MAX_AREA_SUM; i++){
			oa_memset(&rect_area_var, 0x0, sizeof(rect_area_var));
			oa_memset(&rect_area_desc, 0x0, sizeof(rect_area_desc));
			//read rect area data
			ret = r_w_rect_area_data_file(&rect_area_var, file_read, i, none_area);
			if (OA_TRUE == ret && rect_area_var.is_valid == valid){
				//copy data
				rect_area_desc.area_point[0].Lat = rect_area_var.left_up_lat;
				rect_area_desc.area_point[0].Lon = rect_area_var.left_up_lon;
				rect_area_desc.area_point[1].Lat = rect_area_var.left_up_lat;
				rect_area_desc.area_point[1].Lon = rect_area_var.right_down_lon;
				rect_area_desc.area_point[2].Lat = rect_area_var.right_down_lat;
				rect_area_desc.area_point[2].Lon = rect_area_var.right_down_lon;
				rect_area_desc.area_point[3].Lat = rect_area_var.right_down_lat;
				rect_area_desc.area_point[3].Lon = rect_area_var.left_up_lon;
				p_id[i] = rect_area_var.area_id;
				oa_memcpy(p_pro + i, &rect_area_var.area_para, sizeof(rect_area_var.area_para));
				//compare	
				if (1 == rect_area_var.area_para.depend_time){
					//rtc is ok?
					//if it is cycle
					if (rect_area_var.start_time[0] == 0x0 && rect_area_var.stop_time[0] == 0x0){//circle time
						if (rect_area_var.start_time[1] != 0x0 && rect_area_var.stop_time[1] != 0x0)
							res = CompareTime(rect_area_var.start_time, rect_area_var.stop_time, time, 1);
						else if (rect_area_var.start_time[2] != 0x0 && rect_area_var.stop_time[2] != 0x0)
							res = CompareTime(rect_area_var.start_time, rect_area_var.stop_time, time, 2);
						else res = CompareTime(rect_area_var.start_time, rect_area_var.stop_time, time, 3);
					}
					else{//if it is not cycle
						res = CompareTime(rect_area_var.start_time, rect_area_var.stop_time, time, 0);
						if (!res){
							DEBUG("delete one rect area");
							del_area_data(rect_area_var.area_id, Rectangle_Area, one_area);
						}
						else if (res == 2) res = 0;
					}
					
					if (0 == res) continue;
					else if (1 == res){
						i_o = poly_Judge(lon, lat, &rect_area_desc, 4);
						//inside
						if (1 == i_o){
							p_cur[i] = area_inside;
							if (rect_area_var.area_para.speed_limit){
								if (speed > rect_area_var.max_speed){
									os_time[i] = rect_area_var.continue_time;
									*os_bit |= (1<<i);	
								}
							}
						}
						else{
							p_cur[i] = area_outside;
						}
					}
				}
				else{
					i_o = poly_Judge(lon, lat, &rect_area_desc, 4);
					//inside
					if (1 == i_o){
						p_cur[i] = area_inside;
						if (rect_area_var.area_para.speed_limit){
							if (speed > rect_area_var.max_speed){
								os_time[i] = rect_area_var.continue_time;
								*os_bit |= (1<<i);
							}
						}
					}
					else{
						p_cur[i] = area_outside;
					}
				}
			}
			else if (rect_area_var.is_valid == invalid){//area is invalid
				p_cur[i] = area_err;
			}
		}
	}

	return;
}

/*********************************************************
*Function:     area_inout_judge()
*Description:  write area data to file
*Return:		
*Others:         
*********************************************************/
void poly_area_inout_judge(u32 lat, u32 lon, u8 *time, area_status_enum *p_cur, 
									u32 *p_id, u16 speed, u32 *os_bit, u8 *os_time, Area_Para_struct *p_pro)
{
	oa_bool ret;
	u8 i, i_o, res;
	poly_area_item poly_area_var;
	Poly_Area_Desc poly_area_desc;
	
	if (has_specific_file(POLY_AREA_DATA)){
		//hande poly area
		for (i=0; i<MAX_AREA_SUM; i++){
			oa_memset(&poly_area_var, 0x0, sizeof(poly_area_var));
			oa_memset(&poly_area_desc, 0x0, sizeof(poly_area_desc));
			//read poly area data
			ret = r_w_poly_area_data_file(&poly_area_var, file_read, i, none_area);
			if (OA_TRUE == ret && poly_area_var.is_valid == valid){
				u8 loop;
				//copy data
				for (loop=0; loop<poly_area_var.total_point; loop++){
					poly_area_desc.area_point[loop].Lat = poly_area_var.vertax[loop].vertax_lat;
					poly_area_desc.area_point[loop].Lon = poly_area_var.vertax[loop].vertax_lon;
				}
				p_id[i] = poly_area_var.area_id;
				oa_memcpy(p_pro + i, &poly_area_var.area_para, sizeof(poly_area_var.area_para));
				//compare
				if (1 == poly_area_var.area_para.depend_time){
					//rtc is ok?
					if (time[0] == 0x0 && time[1] == 0x0 && time[2] == 0x0) continue;
					//if it is cycle
					if (poly_area_var.start_time[0] == 0x0 && poly_area_var.stop_time[0] == 0x0){//circle time
						if (poly_area_var.start_time[1] != 0x0 && poly_area_var.stop_time[1] != 0x0)
							res = CompareTime(poly_area_var.start_time, poly_area_var.stop_time, time, 1);
						else if (poly_area_var.start_time[2] != 0x0 && poly_area_var.stop_time[2] != 0x0)
							res = CompareTime(poly_area_var.start_time, poly_area_var.stop_time, time, 2);
						else res = CompareTime(poly_area_var.start_time, poly_area_var.stop_time, time, 3);
					}
					else{//if it is not cycle
						res = CompareTime(poly_area_var.start_time, poly_area_var.stop_time, time, 0);
						if (!res){
							DEBUG("delete one poly area");
							del_area_data(poly_area_var.area_id, Poly_Area, one_area);
						}
						else if (res == 2) res = 0;
					}
					if (0 == res) continue;
					else if (1 == res){
						i_o = poly_Judge(lon, lat, &poly_area_desc, poly_area_var.total_point);
						//inside
						if (1 == i_o){
							p_cur[i] = area_inside;
							if (poly_area_var.area_para.speed_limit){
								if (speed > poly_area_var.max_speed){
									os_time[i] = poly_area_var.continue_time;
									*os_bit |= (1<<i);		
								}
							}
						}
						else{//outside
							p_cur[i] = area_outside;
						}
					}
				}
				else{
					i_o = poly_Judge(lon, lat, &poly_area_desc, poly_area_var.total_point);
					//inside
					if (1 == i_o){
						p_cur[i] = area_inside;
						if (poly_area_var.area_para.speed_limit){
							if (speed > poly_area_var.max_speed){
								os_time[i] = poly_area_var.continue_time;
								*os_bit |= (1<<i);		
							}
						}
					}
					else{//outside
						p_cur[i] = area_outside;
					}
				}
			}
			else if (poly_area_var.is_valid == invalid){//area is invalid
				p_cur[i] = area_err;
			}
		}
	}
		
	return;
}
/*********************************************************
*Function:     oa_app_area()
*Description:  write area data to file
*Return:		
*Others:         
*********************************************************/
void oa_app_area(void *para)
{
	static oa_bool task_runed = OA_FALSE;
	static oa_bool start_area = OA_FALSE;
	static oa_uint8 b_t = 0;
	oa_bool ret;
	u32 lon;
	u32 lat;
	u16 speed;
	u8 time[6];
	u8 i;
	area_status_enum cur_status_circle[MAX_AREA_SUM] = {0x0};
	area_status_enum cur_status_rect[MAX_AREA_SUM] = {0x0};
	area_status_enum cur_status_poly[MAX_AREA_SUM] = {0x0};
	static area_status_enum last_status_circle[MAX_AREA_SUM] = {0x0};
	static area_status_enum last_status_rect[MAX_AREA_SUM] = {0x0};
	static area_status_enum last_status_poly[MAX_AREA_SUM] = {0x0};
	u32 os_flag_circle = 0;
	u32 os_flag_rect = 0;
	u32 os_flag_poly = 0;
	static u32 os_circle = 0;
	static u32 os_rect = 0;
	static u32 os_poly = 0;
	u8 os_time_circle[MAX_AREA_SUM] = {0x0};
	u8 os_time_rect[MAX_AREA_SUM] = {0x0};
	u8 os_time_poly[MAX_AREA_SUM] = {0x0};
	static u8 os_cal_circle[MAX_AREA_SUM] = {0x0};
	static u8 os_cal_rect[MAX_AREA_SUM] = {0x0};
	static u8 os_cal_poly[MAX_AREA_SUM] = {0x0};
	u32 area_id[MAX_AREA_SUM] = {0x0};
	Area_Para_struct area_pro[MAX_AREA_SUM];
	STRUCT_RMC gps_info;

	if (OA_FALSE == task_runed){
		DEBUG("(:(:(:(:(:(:(:(:task is %s running:):):):):):):):)", __func__);
		task_runed = OA_TRUE;
	}
#if 0
	ret = has_areadata_dir_n_c();
	//don't exist area data
	if (!ret) goto again;
#endif
	//gps check
	if (GPS_GetFixStatus()) goto again;
	else if (start_area == OA_FALSE) {
		b_t++;
		if (b_t * OA_AREA_RUN_SECOND > 15) {
			start_area = OA_TRUE;
		}
	}

	if (start_area == OA_FALSE) goto again;
	//extract datas
	GetPosinf((u8 *)&lon, GPSLon, 5);
	GetPosinf((u8 *)&lat, GPSLat, 5);
	oa_memset(&gps_info, 0x0, sizeof(gps_info));
	GPS_GetPosition(&gps_info);//copy gps data into 'gps_info'
	//GetPosinf((u8 *)&speed, GPSSpeed, 0);
	speed = gps_info.Speed;
	get_rtc_time(time);
	
	oa_memset(&area_alarm_addition_var, 0x0, sizeof(area_alarm_addition_var));
	oa_memset(area_pro, 0x0, sizeof(area_pro));
	//------------------circle area inside/outside judge------------------
	circle_area_inout_judge(lat, lon, time, cur_status_circle, area_id, speed, &os_flag_circle, os_time_circle, area_pro);
	for (i = 0;i < MAX_AREA_SUM; i++){
		if (cur_status_circle[i] == area_inside && last_status_circle[i] == area_outside){
			DEBUG("进区域,区域id:%d", area_id[i]);

			if (area_pro[i].alarm_inside_to_plat) {
				area_alarm_addition_var.area_kind = os_circle;
				area_alarm_addition_var.id = area_id[i];
				area_alarm_addition_var.in_out = out2in;
				handle_alarm_status(StaAlarm0, ALARM_ENTER_AREA, SET, OA_TRUE);
			}

			if (area_pro[i].alarm_inside_to_driver) {
				handle_alarm_sms(ALARM_ENTER_AREA);
			}
			
		}
		else if (cur_status_circle[i] == area_outside && last_status_circle[i] == area_inside){
			DEBUG("出区域,区域id:%d", area_id[i]);
			if (area_pro[i].alarm_outside_to_plat) {
				area_alarm_addition_var.area_kind = os_circle;
				area_alarm_addition_var.id = area_id[i];
				area_alarm_addition_var.in_out = in2out;
				handle_alarm_status(StaAlarm0, ALARM_ENTER_AREA, SET, OA_TRUE);
			}

			if (area_pro[i].alarm_outside_to_driver) {
				handle_alarm_sms(ALARM_ENTER_AREA);
			}
			
		}
	}
	//oa_memset(&area_alarm_addition_var, 0x0, sizeof(area_alarm_addition_var));
	oa_memcpy(last_status_circle, cur_status_circle, sizeof(last_status_circle));
	//area overspeed judge
	for (i = 0;i < MAX_AREA_SUM; i++){
		if (cur_status_circle[i] == area_inside && (os_flag_circle & (1<<i))){
			os_cal_circle[i]++;
			if (os_cal_circle[i] * OA_AREA_RUN_SECOND > os_time_circle[i]
				&& ReadAlarmPara(StaAlarm0, ALARM_OVER_SPEED) == RESET) {
				overspeed_var.kind = os_circle;
				overspeed_var.id = area_id[i];
				DEBUG("区域内超速,区域id:%d", area_id[i]);
				handle_alarm_status(StaAlarm0, ALARM_OVER_SPEED, SET, OA_TRUE);
				handle_alarm_sms(ALARM_OVER_SPEED);
				os_circle |= (1<<i);
				os_cal_circle[i] = 0;
			}
		}
		else if (cur_status_circle[i] == area_err || 
					cur_status_circle[i] == area_outside || 
					~(os_flag_circle & (1<<i))){
			os_cal_circle[i] = 0;
			if ((os_circle & (1<<i)) && ReadAlarmPara(StaAlarm0, ALARM_OVER_SPEED) == SET) {
				WriteAlarmPara(RESET, StaAlarm0, ALARM_OVER_SPEED);//cancel this alarm
				os_circle &= ~(1<<i);
			}
		}
	}
	
	oa_memset(area_id, 0x0, sizeof(area_id));
	oa_memset(area_pro, 0x0, sizeof(area_pro));
	//------------------rect area inside/outside judge------------------
	rect_area_inout_judge(lat, lon, time, cur_status_rect, area_id, speed, &os_flag_rect, os_time_rect, area_pro);
	for (i = 0;i < MAX_AREA_SUM; i++){
		if (cur_status_rect[i] == area_inside && last_status_rect[i] == area_outside){
			DEBUG("进区域,区域id:%d", area_id[i]);

			if (area_pro[i].alarm_inside_to_plat) {
				area_alarm_addition_var.area_kind = os_rect;
				area_alarm_addition_var.id = area_id[i];
				area_alarm_addition_var.in_out = out2in;
				handle_alarm_status(StaAlarm0, ALARM_ENTER_AREA, SET, OA_TRUE);
			}

			if (area_pro[i].alarm_inside_to_driver) {
				handle_alarm_sms(ALARM_ENTER_AREA);
			}
			
		}
		else if (cur_status_rect[i] == area_outside && last_status_rect[i] == area_inside){
			DEBUG("出区域,区域id:%d", area_id[i]);

			if (area_pro[i].alarm_outside_to_plat) {
				area_alarm_addition_var.area_kind = os_rect;
				area_alarm_addition_var.id = area_id[i];
				area_alarm_addition_var.in_out = in2out;
				handle_alarm_status(StaAlarm0, ALARM_ENTER_AREA, SET, OA_TRUE);
			}

			if (area_pro[i].alarm_outside_to_driver) {
				handle_alarm_sms(ALARM_ENTER_AREA);
			}
			
		}
	}
	//oa_memset(&area_alarm_addition_var, 0x0, sizeof(area_alarm_addition_var));
	oa_memcpy(last_status_rect, cur_status_rect, sizeof(last_status_rect));
	//area overspeed judge
	for (i = 0;i < MAX_AREA_SUM; i++){
		if (cur_status_rect[i] == area_inside && (os_flag_rect & (1<<i))){
			os_cal_rect[i]++;
			if (os_cal_rect[i] * OA_AREA_RUN_SECOND > os_time_rect[i]
				&& ReadAlarmPara(StaAlarm0, ALARM_OVER_SPEED) == RESET) {
				overspeed_var.kind = os_rect;
				overspeed_var.id = area_id[i];
				DEBUG("区域内超速,区域id:%d", area_id[i]);
				handle_alarm_status(StaAlarm0, ALARM_OVER_SPEED, SET, OA_TRUE);
				handle_alarm_sms(ALARM_OVER_SPEED);
				os_cal_rect[i] = 0;
			}
		}
		else if (cur_status_rect[i] == area_err || 
					cur_status_rect[i] == area_outside || 
					~(os_flag_rect & (1<<i))){
			os_cal_rect[i] = 0;
			if ((os_rect & (1<<i)) && ReadAlarmPara(StaAlarm0, ALARM_OVER_SPEED) == SET) {
				WriteAlarmPara(RESET, StaAlarm0, ALARM_OVER_SPEED);//cancel this alarm
				os_rect &= ~(1<<i);
			}
		}
	}
	
	oa_memset(area_id, 0x0, sizeof(area_id));
	oa_memset(area_pro, 0x0, sizeof(area_pro));
	//------------------poly area inside/outside judge------------------
	poly_area_inout_judge(lat, lon, time, cur_status_poly, area_id, speed, &os_flag_poly, os_time_poly, area_pro);
	for (i = 0;i < MAX_AREA_SUM; i++){
		if (cur_status_poly[i] == area_inside && last_status_poly[i] == area_outside){
			DEBUG("进区域,区域id:%d", area_id[i]);

			if (area_pro[i].alarm_inside_to_plat) {
				area_alarm_addition_var.area_kind = os_poly;
				area_alarm_addition_var.id = area_id[i];
				area_alarm_addition_var.in_out = out2in;
				handle_alarm_status(StaAlarm0, ALARM_ENTER_AREA, SET, OA_TRUE);
			}

			if (area_pro[i].alarm_inside_to_driver) {
				handle_alarm_sms(ALARM_ENTER_AREA);
			}
			
		}
		else if (cur_status_poly[i] == area_outside && last_status_poly[i] == area_inside){
			DEBUG("出区域,区域id:%d", area_id[i]);

			if (area_pro[i].alarm_outside_to_plat) {
				area_alarm_addition_var.area_kind = os_poly;
				area_alarm_addition_var.id = area_id[i];
				area_alarm_addition_var.in_out = in2out;
				handle_alarm_status(StaAlarm0, ALARM_ENTER_AREA, SET, OA_TRUE);
			}
			
			if (area_pro[i].alarm_outside_to_driver) {
				handle_alarm_sms(ALARM_ENTER_AREA);
			}
			
		}
	}
	//oa_memset(&area_alarm_addition_var, 0x0, sizeof(area_alarm_addition_var));
	oa_memcpy(last_status_poly, cur_status_poly, sizeof(last_status_poly));
	//area overspeed judge
	for (i = 0;i < MAX_AREA_SUM; i++){
		if (cur_status_poly[i] == area_inside && (os_flag_poly & (1<<i))){
			os_cal_poly[i]++;
			if (os_cal_poly[i] * OA_AREA_RUN_SECOND > os_time_poly[i]
				&& ReadAlarmPara(StaAlarm0, ALARM_OVER_SPEED) == RESET) {
				overspeed_var.kind = os_poly;
				overspeed_var.id = area_id[i];
				DEBUG("区域内超速,区域id:%d", area_id[i]);
				handle_alarm_status(StaAlarm0, ALARM_OVER_SPEED, SET, OA_TRUE);
				handle_alarm_sms(ALARM_OVER_SPEED);
				os_cal_poly[i] = 0;
			}
		}
		else if (cur_status_poly[i] == area_err || 
					cur_status_poly[i] == area_outside || 
					~(os_flag_poly & (1<<i))){
			os_cal_poly[i] = 0;
			if ((os_poly & (1<<i)) && ReadAlarmPara(StaAlarm0, ALARM_OVER_SPEED) == SET) {
				WriteAlarmPara(RESET, StaAlarm0, ALARM_OVER_SPEED);//cancel this alarm
				os_poly &= ~(1<<i);
			}
		}
	}
	
again:
	oa_timer_start(OA_TIMER_ID_9, oa_app_area, NULL, OA_AREA_DETECT_TIME);
}
