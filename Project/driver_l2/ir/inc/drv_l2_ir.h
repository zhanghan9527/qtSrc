#ifndef __drv_l2_ir_H__
#define __drv_l2_ir_H__
#include "driver_l2.h"
#include "drv_l1_sfr.h"

//using time source define//
//using time source define//
#define IR_TIMER    TIMER_B
#define IR_FREG   5000 // (2000    //0.5 ms)  (5000 0.2ms)
#define IR_FREG1    500		//2   ms
#define IOC10		42

//check value define//
#define IOTriggerMaxNum		max_value_bit_count+2//34//18//34
#define IR_Header_1			9//16 (9)
#define IR_Header_2			8//4   (7)
#define rising_edge			1
#define falling_edge		2

//IR Status define//
#define FindFirstTrigger		1
#define Check_IR_Header_1		2
#define Check_IR_Header_2		3
#define Receive_IR_Data			4
#define Decode_IR_Data			5
#define Receive_IR_Repeat_CMD 	6

#if (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)

#define IR_KEY_NONE_TYPE    MAX_IR_KEY_TYPES
#define KEY_CODE_NONE   0x0

extern IR_KEY_TYPES_ENUM bin_ir_press_status_get(void);
extern INT8U bin_ir_keycode_get(void);
#endif


extern void drv_l2_ir_init(void);
extern INT32S drv_l2_getirkey(void);

#endif		// __drv_l2_ir_H__