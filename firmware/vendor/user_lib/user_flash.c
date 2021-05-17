#include "user_flash.h"
#include "drivers/8258/flash.h"

#define GW_ADDR_FLASH 0x29500

void save_user_data_into_flash(unsigned long adr, u8 *data, int len){
    flash_erase_sector(adr);
    flash_write_page(adr, len, data);
}

void get_user_data_from_adr(unsigned long adr, u8 *buf, int len){
    flash_read_page(adr, len, buf);
}

void write_gw_add_to_flash(u16 addr){
	u8 buf[2] = {addr >> 8, addr & 0x0f};
	flash_erase_sector(GW_ADDR_FLASH);
	flash_write_page(GW_ADDR_FLASH, sizeof(buf), buf);
	return;
}

u16 read_gw_add_from_flash(){
	u8 buf[2];
	flash_read_page(GW_ADDR_FLASH, 2 ,buf);
	return buf[0] << 8 | buf[1];
}
