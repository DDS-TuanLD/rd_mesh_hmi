#ifndef __USER_MODEL_H__
#define __USER_MODEL_H__

#include "proj/common/types.h"
#include "proj_lib/sig_mesh/app_mesh.h"

#define RD_OPCODE_TYPE_DEVICE_SEND	(0xE0)
#define RD_OPCODE_TYPE_DEVICE_RSP	(0xE1)

#define GW_SET_TD					0x0001
#define GW_SV_GW_AD					0x0002
#define GW_RES_TD					0x0003

#define DV_SET_TD					0x0001
#define DV_SV_GW_AD					0x0002
#define DV_RES_TD					0x0003

#if(__PROJECT_NODE_LCD__)
#define DV_TYPE_LCD		0x02
#define DV_FEATURE_LCD	0x03
#define DV_APP_LCD		0x03
#endif

typedef struct {
	u16 header;
	u8 type;
	u8 feature;
	u8 app;
}mess_type_dev_rceiv;

typedef struct {
	u16 header;
	u8 type;
	u8 feature;
	u8 app;
}mess_type_dev_rponse;

extern int RD_Messenger_Process_Type_Device(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
extern int RD_Messenger_Process_Null(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);

#endif
