#include "user_include.h"
#include "app_include.h"
#include "drivers/8258/timer.h"

void user_fifo_handler(){
	u16 n = user_fifo_get_number_bytes_written();
	u16 r = user_fifo_get_r_point();
	if(n < UART_MIN_LEN){
		return;
	}
	u16 i = find_first_valid_uart_mess_header();
	if((i == 0) && (n >= UART_MIN_LEN)){
		u8 buff[UART_MAX_LEN];
		for(int j = 0; j< n; j++){
			u16 temp = r + j;
			if (temp >= MAX_QUEUE_LEN ){
				temp = temp - MAX_QUEUE_LEN;
			}
			buff[j] = user_fifo_get(temp);
		}
		if(is_uart_data_reiv_valid(buff, n)){
			for(int j = 0; j< n; j++){
				buff[j] = user_fifo_pop();
			}
			//uart_print_data(buff, n, 0, 0);
			 uart_handler_data_in_main_loop(buff, n);
			//pro_debug(i, n);
		}
		return;
	}
	if (i != 0){
		u8 buff[UART_MAX_LEN];
		for(int j = 0; j< i; j++){
			buff[j] = user_fifo_pop();
		}
		if(is_uart_data_reiv_valid(buff, i)){
			//uart_print_data(buff, i, 0, 0);
			uart_handler_data_in_main_loop(buff, i);
			//pro_debug(i, n);
		}
		return;
	}
}

void check_and_resend_mess(){
	if(!is_uart_mess_response_waiting()){
		return;
	}
	static u32 check_time;
	if(clock_time_exceed(check_time, 200*1000)){
		check_time = clock_time();
		if(is_uart_mess_response_waiting()){
			uart_print_data(uart_last_mess_sent.dt, uart_last_mess_sent.len, 0, 0);
			uart_last_mess_sent.count = uart_last_mess_sent.count + 1;
			if(uart_last_mess_sent.count > 5){
				uart_last_mess_sent.flag = 0;
				uart_last_mess_sent.count = 0;
			}
		}
	}
}

void load_btn_scene_data_from_flash(){
	static u32 check_time;
	if(clock_time_exceed(check_time, 2*1000*1000)){
		check_time = clock_time();
		u8 b[5 * MAX_SCENE_SAVE];
		get_user_data_from_adr(USER_MODEL_SAVE_FLASH, b, 5 * MAX_SCENE_SAVE);
		for(int j = 0; j< 5 * MAX_SCENE_SAVE; j++){
			if(b[j] == 0xff){
				b[j] = 0;
			}
		}
		for(int i = 0; i< MAX_SCENE_SAVE; i++){

			u8 temp1[5] = {b[5*i], b[5*i+1], b[5*i+2], b[5*i+3], b[5*i+4]};
			model_btn_scene_save_t *temp2 = (model_btn_scene_save_t *)temp1;
			model_vd_btn_scene.btn[i] = *temp2;
		}
	}
}
