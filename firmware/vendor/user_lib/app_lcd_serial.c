#include "user_serial.h"
#include "proj/drivers/uart.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "proj/common/tstring.h"
#include "app_lcd_handler.h"
#include "user_fifo.h"
#include "app_lcd_serial.h"

uart_mess_t uart_last_mess_sent;
uart_mess_t uart_res_need;

u16 find_first_valid_uart_mess_header(){
	u16 count = 0;
	u16 i = 0;
	u16 r_point = user_fifo_get_r_point();
	while (count <= user_fifo_get_number_bytes_written())
	{
		u16 i1 = (i + r_point);
		u16 i2 = (i + r_point + 1);

		if(i1 > MAX_QUEUE_LEN -1){
			i1 = i1 - MAX_QUEUE_LEN;
		}
		if(i2 > MAX_QUEUE_LEN -1){
			i2 = i2 - MAX_QUEUE_LEN;
		}
		if((user_fifo_get(i1) == UART_HEADER_DATA_0) && (user_fifo_get(i2) == UART_HEADER_DATA_0) && (i1 != r_point)){
			return i;
		}
		i = i + 1;
		count = count + 1;
		if(i == MAX_QUEUE_LEN){
			i = 0;
		}
	}
	return 0;
}

u8 create_crc_check_by_len(u8 *d, u16 len){
	int i = 0;
	uint16_t temp = 0;
	for(i=2; i<len-1; i++){
		temp = temp + d[i];
	}
	return temp & 0x00ff;
}

u8 create_crc_check_by_op(u8 *d, u16 len){
	u16 op = d[2] << 8 | d[3];
	u8 temp = 0xff;
	switch(op){
	case LCD_CALL_SCENE_CMD	:
		temp = create_crc_check_by_len(d, len);
		break;
	case LCD_UPDATE_SENSOR_DATA_CMD	:
		temp = create_crc_check_by_len(d, len);
		break;
	case LCD_UPDTE_TIME_CMD	:
		temp = create_crc_check_by_len(d, len);
		break;
	case LCD_RESET_MODULE_CMD :
		temp = create_crc_check_by_len(d, len);
		break;
	case LCD_UPDATE_TIME_CMD:
		temp = create_crc_check_by_len(d, len);
		break;
	}
	return temp;
}

int is_uart_data_reiv_valid(u8 *para, int len){
	if((para[0] == UART_HEADER_DATA_0) && (para[1] == UART_HEADER_DATA_1) && (para[len-1] == create_crc_check_by_op(para, len))){
		return 1;
	}

	return 0;
}

int add_data_to_queue(u8 *p, int len){
	for(int i = 0; i< len ; i++){
		if(user_fifo_get_number_bytes_written() == MAX_QUEUE_LEN){
			user_fifo_pop();
		}
		user_fifo_push(p[i]);
	}
	return 0;
}

u8 is_uart_mess_response_waiting(){
	return uart_last_mess_sent.flag;
}

void uart_handler_by_act(u8 *para, int len){
	u16 act = para[2] << 8 | para[3];
	u8 scene_res[8] = {0x55, 0xaa, 0x20, 0x00, 0x00, 0x01, 0xfa, 0x1b};
	u8 reset_res[8] = {0x55, 0xaa, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01};
	u8 request_res[8] = {0x55, 0xaa, 0x00, 0x10, 0x00, 0x01, 0x00, 0x11};
	switch (act)
	{
	case LCD_CALL_SCENE_CMD:
		uart_print_data(scene_res, 8, 0, 0);
		module_handler_scene_btn_onclick(para[6]);
		break;
	case LCD_RESET_MODULE_CMD:
		uart_Send(reset_res, 8);
		module_on_reset();
		break;
	case LCD_SEND_RELAY_CMD:
		module_handler_relay_cmd(para[7], para[8]);
		break;
	case LCD_UPDATE_TIME_CMD:
		uart_print_data(request_res, 8, 0, 0);
		module_update_time();
		break;
	default:
		break;
	}
}

void uart_handler_data_in_main_loop(u8 *para, int len){
	if(is_uart_mess_response_waiting()){
		int ok = 1;
		for(int i = 0; i< uart_res_need.len; i++){
			if(uart_res_need.dt[i] != para[i]){
				ok = 0;
			}
		}
		if(ok == 1){
			uart_last_mess_sent.flag = 0;
			uart_last_mess_sent.count = 0;
		}
	}else{
		uart_handler_by_act(para, len);
	}
	return;
}

// int uart_lcd_handler_data_t(u8 *para, int len){
// 	uart_com_data r = {0};
// 	u16 act = para[4] << 8 | para[5];
// 	u16 temp = uart_last_mess_sent.dt[4] << 8 | uart_last_mess_sent.dt[5];
// 	if(is_uart_mess_response_waiting()){
// 		if((para[3] == UART_TYPE_RES) && (act == temp) && (len == uart_last_mess_sent.len)){
// 			uart_last_mess_sent.flag = 0;
// 		}
// 	}else{
// 		if(para[3] == UART_TYPE_CMD){
// 			para[2] = UART_MODULE_TO_HMI;
// 			para[3] = UART_TYPE_RES;
// 			para[len - 1] = create_crc_check(para);
// 			uart_Send(para, len);
// 		}
// 		add_data_to_queue(para, len);
// 	}
// 	return 0;
// }

int uart_data_handler(u8 *para, int len){
	add_data_to_queue(para, len);
	return 0;
}

int rx_from_uart_cb(void){
	uart_ErrorCLR();

	uart_data_t* p = (uart_data_t *)my_fifo_get(&hci_rx_fifo);
	if(p){
		u32 rx_len = p->len & 0xff; //usually <= 255 so 1 byte should be sufficient
		if (rx_len)
		{
            uart_data_handler(p->data, rx_len);
            my_fifo_pop(&hci_rx_fifo);
		}
	}
	return 0;
}

void module_update_time_hmi(u8 h, u8 m){
	if(is_uart_mess_response_waiting()){
		return;
	}
	u8 para[UART_MAX_LEN];
	u8 len = 0;

	para[0] = UART_HEADER_DATA_0;
	len = len + 1;
	para[1] = UART_HEADER_DATA_1;
	len = len + 1;
	para[2] = LCD_UPDTE_TIME_CMD >> 8;
	len = len + 1;
	para[3] = LCD_UPDTE_TIME_CMD & 0x00ff;
	len = len + 1;
	para[4] = 0x00;
	len = len + 1;
	para[5] = 0x02;
	len = len + 1;
	para[6] =	h;
	len = len + 1;
	para[7] = m;
	len = len + 1;
	para[8] = create_crc_check_by_op(para, len + 1);
	len = len + 1;

	for(int i = 0; i< len; i++){
		uart_last_mess_sent.dt[i] = para[i];
	}
	uart_last_mess_sent.len = len;
	uart_last_mess_sent.flag = 1;
	u8 time_res[8] = {0x55, 0xaa, 0x20, 0x20, 0x00, 0x01, 0x00, 0x41};
	for(int i = 0; i< 8; i++){
		uart_res_need.dt[i] = time_res[i];
	}
	uart_res_need.len = 8;
	uart_print_data(uart_last_mess_sent.dt, uart_last_mess_sent.len, 0, 0);
	return;
}
void module_update_sensor_para_hmi(u8 *t, u8 *h){
	if(is_uart_mess_response_waiting()){
		return;
	}
	u8 para[UART_MAX_LEN];
	u8 len = 0;

	para[0] = UART_HEADER_DATA_0;
	len = len + 1;
	para[1] = UART_HEADER_DATA_1;
	len = len + 1;
	para[2] = LCD_UPDATE_SENSOR_DATA_CMD >> 8;
	len = len + 1;
	para[3] = LCD_UPDATE_SENSOR_DATA_CMD & 0x00ff;
	len = len + 1;
	para[4] = 0x00;
	len = len + 1;
	para[5] = 0x04;
	len = len + 1;
	para[6] = t[0];
	len = len + 1;
	para[7] = t[1];
	len = len + 1;
	para[8] = h[0];
	len = len + 1;
	para[9] = h[1];
	len = len + 1;
	para[10] = create_crc_check_by_op(para, len + 1);
	len = len + 1;

	for(int i = 0; i< len; i++){
		uart_last_mess_sent.dt[i] = para[i];
	}
	uart_last_mess_sent.len = len;
	uart_last_mess_sent.flag = 1;
	u8 data_res[8] = {0x55, 0xaa, 0x20, 0x10, 0x00, 0x01, 0x00, 0x31};
	for(int i = 0; i< 8; i++){
		uart_res_need.dt[i] = data_res[i];
	}
	uart_res_need.len = 8;
	uart_print_data(uart_last_mess_sent.dt, uart_last_mess_sent.len, 0, 0);
	return;
}
