#ifndef __USER_FIFO_H__
#define __USER_FIFO_H__
#include "proj/common/types.h"

#if (__PROJECT_NODE_SENSOR_NO_LPN__)
    #define MAX_QUEUE_LEN   100
#endif

#ifndef MAX_QUEUE_LEN
#define MAX_QUEUE_LEN   2048
#endif

typedef struct {
    u8 is_init;
    u16 num_bytes_written;
    u16 w;
    u16 r;
    u8 dt[MAX_QUEUE_LEN];
}user_fifo_t;

extern void user_fifo_init();
extern u8 user_fifo_get(u16 loc);
extern u8 user_fifo_pop();
extern void user_fifo_push(u8 d);
extern void user_fifo_reset();
extern u16 user_fifo_get_r_point();
extern u16 user_fifo_get_w_point();
extern u16 user_fifo_get_number_bytes_written();

#endif
