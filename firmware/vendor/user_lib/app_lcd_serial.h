#ifndef __APP_SERIAL_H__
#define __APP_SERIAL_H__
#include "proj/common/types.h"
//#define UART_HEADER_DATA_0  0xaa
//#define UART_HEADER_DATA_1  0xaa
#define UART_HMI_TO_MODULE	0xfa
#define UART_MODULE_TO_HMI  0xf5

#define UART_HEADER_DATA_0  0x55
#define UART_HEADER_DATA_1  0xaa

#define UART_TYPE_CMD       0x01
#define UART_TYPE_RES		0x00

#define UART_MAX_LEN		30
#define UART_MIN_LEN 		7

#define LCD_CALL_SCENE_CMD	0x2000
#define LCD_UPDATE_SENSOR_DATA_CMD	0x2010
#define LCD_UPDTE_TIME_CMD	0x2020
#define LCD_RESET_MODULE_CMD	0x0000
#define LCD_SEND_RELAY_CMD      0x1000
#define LCD_UPDATE_TIME_CMD		0x0010

typedef struct {
	u8 dt[UART_MAX_LEN];
	u8 len;
	u8 flag;
    u8 count;
}uart_mess_t;

extern uart_mess_t uart_last_mess_sent;
extern uart_mess_t uart_res_need;

extern int rx_from_uart_cb(void);
extern u8 is_uart_mess_response_waiting();
extern u16 find_first_valid_uart_mess_header();
extern int is_uart_data_reiv_valid(u8 *para, int len);
extern void uart_handler_data_in_main_loop(u8 *para, int len);
extern void module_update_time_hmi(u8 h, u8 m);
extern void module_update_sensor_para_hmi(u8 *t, u8 *h);
extern u8 create_crc_check(u8 *d);

#endif
