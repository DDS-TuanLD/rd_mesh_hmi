#ifndef __APP_LCD_HANDLER_H__
#define __APP_LCD_HANDLER_H__
#include "proj/common/types.h"

extern void module_handler_scene_btn_onclick(u8 btn_onclick);
extern void module_on_reset();
extern void module_handler_relay_cmd(u8 relay, u8 mod);
extern void module_update_time();

#endif
