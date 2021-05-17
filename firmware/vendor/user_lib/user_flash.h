#ifndef __USER_FLASH_H__
#define __USER_FLASH_H__

#include "proj/common/types.h"

#define USER_MODEL_SAVE_FLASH  0x78000

extern void save_user_data_into_flash(unsigned long adr, u8 *data, int len);
extern void get_user_data_from_adr(unsigned long adr, u8 *buf, int len);
extern void write_gw_add_to_flash(u16 addr);
extern u16 read_gw_add_from_flash();

#endif
