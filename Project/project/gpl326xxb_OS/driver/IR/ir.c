#include "application.h"
#include "ir.h"
INT32U send_recive_flag;
INT32U change_flag = 1;
INT32U count_flag = 0; 
#define freq_38Khz  76000//38000*2
#define Low              0
#define negate           1
#define none             2
INT32U IR_flag_Status = none ;

INT8U ir_get_data[max_byte_count]= {0};
INT8U ir_send[max_byte_count]= {0};	 //0--3:为id_,4--7:为id_self,8--14：消息内容；15：检验码；
INT8S ir_read=0,ir_sendend=0,ir_flag=0;

void send_start_end_flag() //send command or addr
{
	while(1){
		if(count_flag < 152) //671(9ms)  76(1ms)
		 	IR_flag_Status = negate;
		else if(count_flag < 304) //1013(4.5ms)   114(0.5ms)
			IR_flag_Status = Low; 
		else {
			count_flag = 0;
			IR_flag_Status = none;
			break;
		}
	}
}

void Get_ir_data(INT8U *data)
{
	Copy8(ir_get_data,data,max_byte_count);
}
/*
void send_delay(unsigned int num1,unsigned int num2)
{
	while(1)
	{
		if(count_flag < num1) //(38  0.5ms)
		 	IR_flag_Status = negate;
		else if(count_flag < num2) //( 76   1ms)
			IR_flag_Status = Low; 
		else 
		{
			count_flag = 0;
			IR_flag_Status = none;
			break;
		}
	}
}
*/


void send_0() //send 0
{
	while(1)
	{
		if(count_flag <= 15*2)				//42     8     21   15
	 		IR_flag_Status = negate;
		else if(count_flag <= 30*2)			//84      15     42   30
			IR_flag_Status = Low;
		else {
			IR_flag_Status = none;
			count_flag = 0;
			break;//IR_flag_Status = none;
		}
	}
}

void send_1() //send 1
{
	while(1)
	{
		if(count_flag <= 15*2)				//42  8    21  15
		 	IR_flag_Status = negate;
		else if(count_flag <= 55*2)			//171  23  86  45
			IR_flag_Status = Low;
		else{ 
			IR_flag_Status = none;
			count_flag = 0;
			break;
		}
	}	
}

void TimerIsrFunction(void)
{
	count_flag++;
	switch (IR_flag_Status){
		case Low :	
			gpio_write_io(ir_send_io,0);		
			break ;
			
		case negate :
			change_flag = ~change_flag;
			gpio_write_io(ir_send_io,change_flag);
			break;
			
		case none :
//			gpio_write_io(ir_send_io,0);
//			break;
		default:
		//	count_flag = 0;
			break ;
		
	}
} 
void ir_send_byte( INT8U data)
{
	INT8U i,temp,Byte_num;
	Byte_num = 8;
	temp = data;
	for(i=0;i<Byte_num;i++){
		if((data & 0x01) == 0)
			send_0();
		else
			send_1();
		data >>= 1;
	
	}
/*	data = temp;
	for(i=0;i<Byte_num;i++)
	{
		if((data & 0x01) == 0)
			send_0();
		else
			send_1();
		data >>= 1;
	
	}
*/
}


////////////////////////////////////////////////
//ir send



void ir_send_api_init(void)
{
	//gpio_init();

	gpio_init_io(ir_send_io,1);
	gpio_set_port_attribute(ir_send_io,1);
	gpio_write_io(ir_send_io,0);
	
	//timer_freq_setup(TIMER_A,freq_38Khz,0,TimerIsrFunction);
}
void ir_send_api_uninit(void)
{
	timer_stop(TIMER_A);
	ir_sendend=1;
}
void TimerIsrFunction(void);
void ir_send_data(INT8U *buf,INT8U byte_count)
{
	INT8U i,checksum=0;
	timer_freq_setup(TIMER_A,freq_38Khz,0,TimerIsrFunction);
	for(i=0;i<(byte_count-1);i++){ 
		checksum +=*(buf+i);
	}
	buf[max_byte_count-1] = checksum;
	send_start_end_flag();

	for(i=0;i<max_byte_count;i++){
		ir_send_byte(*(buf+i));
	}
	
	for(i = 0; i <4; i++){
		send_0();
	}
	count_flag = 0;	
	ir_send_api_uninit();
}
//////////////////////////////////////////////////////
//ir decode
void ir_decode_api_uninit(void)
{	
	extab_user_isr_clr(EXTB);
	extab_enable_set(EXTB,0);
}
void ir_decode_api_init(void)
{
	drv_l2_ir_init();
	extab_enable_set(EXTB,TRUE);//add by crf 2010.9.28	
}
void ir_decode_data(void)
{
	call_drv_l2_ir_decode();
}
///////////////////////////////////////////////////////
void test_ir_send(void)
{
	INT8U test_bufer[max_byte_count]={0},i;
	for(i=0;i<max_byte_count;i++)
	{
		test_bufer[i]=i;	
	}
	ir_send_api_init();
	gpio_init_io(IO_A9,0);
	gpio_set_port_attribute(IO_A9,0);
	gpio_write_io(IO_A9,1);
	while(1)
	{
		if(!gpio_read_io(IO_A9)){
			
			ir_send_data(&test_bufer[0],max_byte_count);			
		}
	}
}
void test_ir_decode(void)
{
	INT8U temp =0;
	DBG_PRINT("ent ir decode");
	ir_decode_api_init();
	while(1)
	{	
		ir_decode_data();
		if(temp ==1)
		{
			temp =0;
			ir_decode_api_uninit();
		}else if(temp==2){
			temp =0;
			ir_decode_api_init();
		}
	}
}

//////////////////////////////////////
//ir task
#define IR_TASK_QUEUE_MAX		32
#define IR_TASK_STACK_SIZE		512
#define	TSK_PRI_VID_IR			31
#define RETURN(x)	{nRet = x; goto Return;}
INT32U IRTaskStack[IR_TASK_STACK_SIZE];
OS_EVENT *IRTaskQ;
OS_EVENT *IR_task_state_ack_m;
void *ir_task_q_stack[IR_TASK_QUEUE_MAX];


void ir_task_entry(void* para)
{
	INT32U msg_id;
	INT8U err;
//	INT8U test_bufer[max_byte_count]={0};
	INT8U i;
//	for(i=0;i<max_byte_count;i++)
//	{
//		test_bufer[i]=i;	
//	}

	while(1){
		msg_id = (INT32U) OSQPend(IRTaskQ, 0, &err);
		if((!msg_id) || (err != OS_NO_ERR)) {
	  	    continue;
		}	
		switch(msg_id){
			
				case MSG_IR_SEND_API_INIT:
					ir_send_api_init();
					break;
				case MSG_IR_SEND_ONE_FRAME_DATA:
					ir_send_data(&ir_send[0],max_byte_count);
					break;
				case MSG_IR_SEND_API_EXIT:
					ir_send_api_uninit();
					break;
				case MSG_IR_DECODE_API_INIT:
					ir_decode_api_init();
					break;
				case MSG_IR_DECODE_START:
					ir_decode_data();
					break;
				case MSG_IR_DECODE_API_EXIT:
					ir_decode_api_uninit();
					break;
				case MSG_IR_TASK_EXIT:
						goto IR_TASK_EXIT;
					break;
				default:
				
					break;
			
		}
	}
	IR_TASK_EXIT:
		ir_send_api_uninit();
		ir_decode_api_uninit();
		OSQFlush(IRTaskQ);
	   	OSQDel(IRTaskQ, OS_DEL_ALWAYS, &err);
		OSMboxDel(IR_task_state_ack_m, OS_DEL_ALWAYS, &err);
		IRTaskQ = NULL;
		OSTaskDel(TSK_PRI_VID_IR);
}

INT32S ir_task_demo(void)
{
	INT8S  err;
	INT32S nRet;
	
	IRTaskQ = OSQCreate(ir_task_q_stack, IR_TASK_QUEUE_MAX);
	if(!IRTaskQ) {
    	RETURN(STATUS_FAIL);
    }
    IR_task_state_ack_m = OSMboxCreate(NULL);
	if(!IR_task_state_ack_m) {
		RETURN(STATUS_FAIL);
	}
	err = OSTaskCreate(ir_task_entry, (void *) 0, &IRTaskStack[IR_TASK_STACK_SIZE - 1], TSK_PRI_VID_IR);
	if(err < 0){
		DBG_PRINT("Create ir_task_demo_Demo task fail\r\n");
		RETURN(STATUS_FAIL);		
	}
	nRet = STATUS_OK;
	DBG_PRINT("Create ir_task_demo_Demo task ok\r\n");
Return:		
	return nRet;
}

//////////////////////////////////////