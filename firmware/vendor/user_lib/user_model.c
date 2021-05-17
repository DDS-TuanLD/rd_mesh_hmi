#include "user_model.h"
#include "user_flash.h"

void module_response_type_dv_to_gw(mess_type_dev_rceiv *rceiv, mesh_cb_fun_par_t *cb_par){
	mess_type_dev_rponse t = {0};
	switch (rceiv->header)
	{
	case GW_SV_GW_AD:
		t.header = DV_SV_GW_AD;
		SendOpParaDebug(cb_par->adr_src, 0, RD_OPCODE_TYPE_DEVICE_RSP, (u8 *)&t, sizeof(t));
		break;
	case GW_SET_TD:
		break;
	case GW_RES_TD:
	#if (__PROJECT_NODE_LCD__)
		t.header = DV_RES_TD;
		t.type = DV_TYPE_LCD;
		t.feature = DV_FEATURE_LCD;
		t.app = DV_APP_LCD;
	#endif
		SendOpParaDebug(cb_par->adr_src, 0, RD_OPCODE_TYPE_DEVICE_RSP, (u8 *)&t, sizeof(t));
		break;
	default:
		break;
	}
	return;
}


int RD_Messenger_Process_Type_Device(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par){
	mess_type_dev_rceiv *rceiv = (mess_type_dev_rceiv *)par;
	switch (rceiv->header)
	{
	case GW_SV_GW_AD:
		write_gw_add_to_flash(cb_par->adr_src);
		module_response_type_dv_to_gw(rceiv, cb_par);
		break;
	case GW_SET_TD:
		module_response_type_dv_to_gw(rceiv, cb_par);
		break;
	case GW_RES_TD:
		module_response_type_dv_to_gw(rceiv, cb_par);
		break;
	default:
		break;
	}
	return 0;
}

int RD_Messenger_Process_Null(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par){
	int err = 0;
    if(cb_par->model){  // model may be Null for status message
        //model_client_common_t *p_model = (model_client_common_t *)(cb_par->model);
    }
    return err;
}



