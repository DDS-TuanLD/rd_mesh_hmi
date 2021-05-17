#include "app_lcd_model.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "proj/common/types.h"
#include "vendor/common/mesh_node.h"
#include "vendor/common/cmd_interface.h"
#include "vendor/common/scene.h"
#include "drivers/8258/flash.h"
#include "drivers/8258/uart.h"
#include "app_lcd_serial.h"
#include "drivers/8258/flash.h"
#include "user_flash.h"
#define GW_ADDR_SAVE 0x29500

model_vendor_btn_scene_t model_vd_btn_scene;

u16 get_gw_addr(){
   u8 buf[2];
   flash_read_page(GW_ADDR_SAVE, 2, buf);
   return buf[0] << 8 | buf[1];
}

u16 find_sceneId_from_btn_onclick(u8 btn_onclick){
    for(int i = 0; i< MAX_SCENE_SAVE; i++){
        if(model_vd_btn_scene.btn[i].bid == btn_onclick)
        {
            return model_vd_btn_scene.btn[i].sceneId;
        }
    }
    return 0;
}

u8 find_appId_from_btn_onclick(u8 btn_onclick){
    for(int i = 0; i< MAX_SCENE_SAVE; i++){
        if(model_vd_btn_scene.btn[i].bid == btn_onclick)
        {
            return model_vd_btn_scene.btn[i].appId & 0x00ff;
        }
    }
    return 0;
}

void module_send_btn_onclick_to_hc(u8 btn_onclick, u16 sceneId){
    btn_onclick_data_status btn_onclick_status = {0};
    btn_onclick_status.header = MODULE_REQUEST_TIME_UPDATE;
    btn_onclick_status.bid = btn_onclick;
    btn_onclick_status.sceneId = sceneId;
    btn_onclick_status.ask_time = 0x00;

    #if 1
        SendOpParaDebug(ADR_ALL_NODES, 0, VENDOR_LCD_SEND_BTN_ONCLICK, (u8 *)&btn_onclick_status, sizeof(btn_onclick_status));
    #else
        u16 gw_addr = get_gw_addr();
        SendOpParaDebug(gw_addr, 0, VENDOR_LCD_SEND_BTN_ONCLICK, (u8 *)&btn_onclick_status, sizeof(btn_onclick_status));
    #endif
    return;
}

void module_handler_btn_onclick_itself(u16 sceneId, u16 rbgID){
    if(sceneId != 0){
        //access_cmd_scene_recall(ADR_ALL_NODES, 0, sceneId, CMD_NO_ACK, 0);
    }
    if(rbgID != 0){
        rgb_scene_call rgb = {0};
        rgb.appId = sceneId;
        rgb.header = MODULE_SEND_RGB_HEADER;
        SendOpParaDebug(ADR_ALL_NODES, 0, MODULE_SEND_SGBID_ITSELF_OP, (u8 *)&rgb, sizeof(rgb));
    }
    return;
}

void module_handler_btn_onclick(u8 btn_onclick){
    u16 sceneId = find_sceneId_from_btn_onclick(btn_onclick);
    u8 rbgID   = find_appId_from_btn_onclick(btn_onclick);
    module_send_btn_onclick_to_hc(btn_onclick, sceneId);
    module_handler_btn_onclick_itself(sceneId, rbgID);
    return;
}

void module_send_update_time_to_gw(){
    btn_onclick_data_status btn_onclick_status = {0};
    btn_onclick_status.header = MODULE_REQUEST_TIME_UPDATE;
    btn_onclick_status.bid = 0x00;
    btn_onclick_status.sceneId = 0x00;
    btn_onclick_status.ask_time = 0x01;
    SendOpParaDebug(ADR_ALL_NODES, 0, VENDOR_LCD_SEND_BTN_ONCLICK, (u8 *)&btn_onclick_status, sizeof(btn_onclick_status));
    return;
}

void module_report_relay_st_to_gw(u8 relay, u8 st){
    return;
}

int btn_scene_set_sts(int state, u16 ele_adr, u16 dst_adr, model_btn_scene_receive_t* btn_set){
	model_btn_scene_response_t state_res = {0};

	if(state == BTN_ACTION_SUCCESS){
		state_res.header = btn_set->header;
		state_res.bid = btn_set->bid;
		state_res.sceneId = btn_set->sceneId;
		state_res.appId = btn_set->appId;
		mesh_tx_cmd_rsp(VD_BTN_SCENE_STATUS, (u8 *)&state_res, sizeof(state_res), ele_adr, dst_adr, 0, 0);
	}
	if(state == BTN_ACTION_FAILURE){
	}
	return 0;
}

int get_current_written_btn_scene_location(){
	int count = 0;
	for(int i = 0; i< MAX_SCENE_SAVE; i++){
		if(model_vd_btn_scene.btn[i].writted == 0) break;
		count = count + 1;
	}
	return count;
}

int get_btn_scene_loc_by_bid(u8 btn, int wn){
	int count = 0;
	for(int i = 0; i< wn; i++){
		if(model_vd_btn_scene.btn[i].bid == btn) break;
		count = count + 1;
	}
	return count;
}

void save_btn_scene_into_model(model_btn_scene_receive_t* btn_set, int *loc, mesh_cb_fun_par_t *cb_par){
	int save_loc = get_btn_scene_loc_by_bid(btn_set->bid, *loc);
	if(*loc > MAX_SCENE_SAVE){
		return ;
	}
	model_vd_btn_scene.btn[save_loc].writted = 0x01;
	model_vd_btn_scene.btn[save_loc].bid = btn_set->bid;
	model_vd_btn_scene.btn[save_loc].sceneId = btn_set->sceneId;
	model_vd_btn_scene.btn[save_loc].appId = btn_set->appId;
	if(save_loc == *loc){
		*loc = *loc + 1;
		model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
		btn_scene_set_sts(BTN_ACTION_SUCCESS, p_model->com.ele_adr, cb_par->adr_src, btn_set);
		return ;
	}
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	btn_scene_set_sts(BTN_ACTION_SUCCESS, p_model->com.ele_adr, cb_par->adr_src, btn_set);
	return;
}

void del_btn_scene_into_model(model_btn_scene_receive_t* btn_set, int *loc, mesh_cb_fun_par_t *cb_par){
	if(*loc > MAX_SCENE_SAVE){
		return ;
	}
	int del_loc = get_btn_scene_loc_by_bid(btn_set->bid, *loc);
	if(del_loc == *loc){
		return ;
	}
	for(int i = del_loc; i< (MAX_SCENE_SAVE - 1); i++){
		model_vd_btn_scene.btn[i] = model_vd_btn_scene.btn[i+1];
	}
	*loc = *loc - 1;
	return ;
}

int is_btn_scene_set_data_valid(model_btn_scene_receive_t* btn_set){
	return 1;
}

int cb_vd_btn_scene_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par){
	model_btn_scene_receive_t* btn_set = (model_btn_scene_receive_t *)par;
	u8 temp[2] = {par[2], par[3]};
	u8 humi[2] = {par[4], par[5]};
	if(!is_btn_scene_set_data_valid(btn_set)){
		return 0;
	}
	static u8 wn = 0;
	if(wn == 0){
		wn = get_current_written_btn_scene_location();
	}
	if(wn >= MAX_SCENE_SAVE){
		return 0;
	}
	switch (btn_set->header)
	{
		case BTN_SAVE_CLICK_MODE:
			save_btn_scene_into_model(btn_set, &wn, cb_par);
			break;
		case BTN_DELETE_CLICK_MODE:
			del_btn_scene_into_model(btn_set, &wn, cb_par);
			break;
		case 0x040A:
			module_update_time_hmi(par[2], par[3]);
			break;
		case 0x030A:
			module_update_sensor_para_hmi(temp, humi);
			break;
		default:
			break;
	}
	save_user_data_into_flash(USER_MODEL_SAVE_FLASH, (u8 *)&model_vd_btn_scene, sizeof(model_vd_btn_scene));
	return 1;
}

int cb_vd_btn_scene_sts(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par){
	int err = 0;
    if(cb_par->model){  // model may be Null for status message
        //model_client_common_t *p_model = (model_client_common_t *)(cb_par->model);
    }
    return err;
}


