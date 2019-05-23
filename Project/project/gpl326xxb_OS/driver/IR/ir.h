

typedef enum
{
	MSG_IR_SEND_API_INIT = 0x1000,
	MSG_IR_SEND_ONE_FRAME_DATA,
	MSG_IR_SEND_API_EXIT,
	MSG_IR_DECODE_API_INIT,
	MSG_IR_DECODE_START,
	MSG_IR_DECODE_API_EXIT,
	MSG_IR_TASK_EXIT	
} MSG_IR_VAULE_STATE_ENUM;
extern OS_EVENT *IRTaskQ;
extern INT32S ir_task_demo(void);

#define max_byte_count	15

extern INT8U ir_get_data[max_byte_count];
extern INT8U ir_send[max_byte_count];
extern INT8S ir_read,ir_flag,ir_sendend;

#define ir_send_io		IO_F6

#define 		IR_RX			IO_C10
#define 		IR_TX			ir_send_io

#define 		IR_RX_IO			BIT10
#define 		IR_RX_Attrib 		R_IOC_ATT
#define 		IR_RX_Dir 			R_IOC_DIR
#define 		IR_RX_Data 			R_IOC_I_DATA
#define 		IR_RX_Buffer 		R_IOC_O_DATA
#define 		IR_RX_DRV 			R_IOC_DRV

#define 		IR_TX_IO			BIT6
#define 		IR_TX_Attrib 		R_IOF_ATT
#define 		IR_TX_Dir 			R_IOF_DIR
#define 		IR_TX_Data 			R_IOF_I_DATA
#define 		IR_TX_Buffer 		R_IOF_O_DATA
#define 		IR_TX_DRV 			R_IOF_DRV