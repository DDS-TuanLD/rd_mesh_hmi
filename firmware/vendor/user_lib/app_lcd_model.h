#ifndef __APP_LCD_MODEL_H__
#define __APP_LCD_MODEL_H__
#include "proj/common/types.h"
#include "proj_lib/sig_mesh/app_mesh.h"

#define MODULE_SEND_RGB_HEADER             0x000a
#define MODULE_REQUEST_TIME_UPDATE         0x000a

#define VENDOR_LCD_SEND_BTN_ONCLICK        0x52
#define MODULE_SEND_SGBID_ITSELF_OP        0xE3
#define VD_BTN_SCENE_SET					0xE2
#define VD_BTN_SCENE_STATUS					0xE3

#define BTN_SAVE_CLICK_MODE			0x010A
#define BTN_DELETE_CLICK_MODE		0x020A

#define BTN_ACTION_SUCCESS			0
#define BTN_ACTION_FAILURE			-1

#define MAX_SCENE_SAVE 60

typedef struct btn_onclick_data_status{
    u16 header;
    u8 bid;
    u16 sceneId;
    u8 ask_time;
}btn_onclick_data_status;

typedef struct {
    u16 header;
    u16 appId;
}rgb_scene_call;

typedef struct {
	u16 header;
	u8 bid;
    u16 sceneId;
	u8 appId;
}model_btn_scene_receive_t;

typedef struct {
	u16 header;
	u8 bid;
	u16 sceneId;
	u8 appId;
}model_btn_scene_response_t;

typedef struct {
	u8 writted;
	u8 bid;
    u16 sceneId;
	u8 appId;
}model_btn_scene_save_t;

typedef struct{
	model_btn_scene_save_t btn[MAX_SCENE_SAVE];
}model_vendor_btn_scene_t;

extern model_vendor_btn_scene_t model_vd_btn_scene;

extern void module_handler_btn_onclick(u8 btn_onclick);
extern void module_send_update_time_to_gw();
extern void module_report_relay_st_to_gw(u8 relay, u8 st);
extern int cb_vd_btn_scene_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
extern int cb_vd_btn_scene_sts(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#endif
