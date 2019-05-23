/*
* Purpose: IR driver/interface
*
* Author: sunxw
*         allenlin modify 2008/06/10
*
* Date: 2008/3/29
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.01         
*/

//Include files
#include "drv_l1_tv.h"
#include "drv_l2_ir.h"
#include "ir.h"


//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L2_IR) && (_DRV_L2_IR == 1)                     //
//================================================================//

// Variables defined in this file 
//INT16S iTimetick;
INT32S iReceivecount;
INT32S IR_Status;
INT32S Repeat_Flag;
INT32S Repeatcount;

INT8U OperationCodeValue;//change  crf 2010.10.20
#define max_value_bit_count	(max_byte_count*8)//32
//INT32S OperationCodeValue;
//INT8S *pSavetimedataaddr;
//INT8S  Savetimebuffer[32];

//INT8U iTimetick;
INT32U iTimetick = 0;
INT32U iTimetick_count;  //用来测量去除杂讯的基准值

INT32U *pSavetimedataaddr;
//INT8U  Savetimebuffer[32];
INT32U  Savetimebuffer[max_value_bit_count];

INT32U *pSavetimedataaddr_1;	//add by crf 2010.1018
//INT8U  Savetimebuffer_1[32];
INT32U  Savetimebuffer_1[max_value_bit_count];
INT8U Switch_Buffer = 0 ;
INT8U Time_to_Decode = 0 ;
INT32U check_receive_data = 0;
INT32U check_receive_data_1 = 0;
INT32U lost_num_count = 0;
INT32U erro_num_count =0;
INT32U receive_num_count = 0;

#define  min_data_1    50//100
#define  max_data_1    180
#define  min_data_2   190//600
#define  max_data_2   400// 900

INT32U iTimetick_temp_1_min = min_data_1;
INT32U iTimetick_temp_1_max =max_data_1;
INT32U iTimetick_temp_2_max  =max_data_2;
INT32U iTimetick_temp_2_min   =min_data_2;

INT8U	check_byte = 0 ;



#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)

typedef struct 
{
    INT8U  key_code;
    INT8U   key_type;
} IRkeySYSPara;
IRkeySYSPara ir_key_para;
fp_msg_qsend ir_msg_QSend;
void   *ir_msgQId;
INT32U ir_msgId;

#elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)

#define IR_KEY_NONE_TYPE    MAX_IR_KEY_TYPES
#define KEY_CODE_NONE   0x0

IR_KEY_TYPES_ENUM  bin_ir_key_type=IR_KEY_NONE_TYPE;
INT8U  bin_ir_key_code=KEY_CODE_NONE;

IR_KEY_TYPES_ENUM bin_ir_press_status_get(void);
INT8U bin_ir_keycode_get(void);
#endif

// Functions defined in this file 

void call_drv_l2_ir_decode();
void drv_l2_ir_init(void);
INT32S drv_l2_getirkey(void);
static void 	ir_time_tick(void);
static void 	drv_l2_ir_start_time_with_irq(INT32U freg);
static void 	drv_l2_ir_start_time_without_irq(void);
static INT16S 	drv_l2_ir_get_time_value(void);
//static void 	drv_l2_ir_delay(INT32U delay_time);
static void 	drv_l2_ir_stop_time(void);
static void 	drv_l2_ir_set_extB(INT32S inputedge);
static void 	drv_l2_ir_clr_extB(void);
static INT32S	drv_l2_ir_check_io(void);
static void 	drv_l2_ir_set_io(void);
//static void 	drv_l2_ir_int_set(void);
static void 	ir_extab_int(void);
static void 	drv_l2_ir_decode(void);
static void 	drv_l2_ir_reinit(void);
static void 	drv_l2_ir_sentrepeatcmd(void);


void test_real_parameter(INT32U receive_data,INT32U min_data_1_1 ,INT32U max_data_1_1,INT32U min_data_2_2  ,INT32U max_data_2_2 )
{
	iTimetick_count ++;
	if( receive_data < max_data_1_1 &&receive_data > min_data_1_1 )
	{
		
		if(receive_data < iTimetick_temp_1_max)
		{
			iTimetick_temp_1_max = receive_data;
		}
		 if(receive_data > iTimetick_temp_1_min )
		{
			iTimetick_temp_1_min = receive_data;
		}
	}
	else if( receive_data < max_data_2_2 &&receive_data > min_data_2_2)
	{
		if(receive_data < iTimetick_temp_2_max)
		{
			iTimetick_temp_2_max = receive_data;
		}
		 if(receive_data > iTimetick_temp_2_min )
		{
			iTimetick_temp_2_min = receive_data;
		}
	}
	if(iTimetick_count == 255)
	{
		iTimetick_count = 0;
		DBG_PRINT ("\n\r  min_1:  %d\n\r", iTimetick_temp_1_max);
		DBG_PRINT ("\n\r  max_1:  %d\n\r", iTimetick_temp_1_min);
		DBG_PRINT ("\n\r  min_2:  %d\n\r", iTimetick_temp_2_max);
		DBG_PRINT ("\n\r  max_2:  %d\n\r", iTimetick_temp_2_min);
		
	}
}

void call_drv_l2_ir_decode()
{
	INT32U i; //add by crf 2010.10.18
	if(Time_to_Decode == 1)
	{
		drv_l2_ir_decode();
		
		if(Switch_Buffer != 0)
		{
			for(i=0;i<max_value_bit_count;i++)
				Savetimebuffer[i]=0;	
		}
		else 
		{
			for(i=0;i<max_value_bit_count;i++)
				Savetimebuffer_1[i]=0;	
		}
	}
	else;

}

// Function Call Definition Area
static void drv_l2_ir_start_time_with_irq(INT32U freg)
{
	timer_freq_setup(IR_TIMER,freg,0,ir_time_tick);
}

static void drv_l2_ir_start_time_without_irq(void)
{
	timer_counter_reset(IR_TIMER);
	//timer_start_without_isr(IR_TIMER,TIMER_SOURCE_4096HZ);
	timer_start_without_isr(IR_TIMER,TIMER_SOURCE_MCLK_DIV_256);
}

static INT16S drv_l2_ir_get_time_value(void)
{
	INT32U time;
	time = TimerCountReturn(IR_TIMER);
	timer_stop(IR_TIMER);
	return (INT16S)time;
}

#if 0
static void drv_l2_ir_delay(INT32U delay_time)
{
	INT32U time;
	do{
		time = TimerCountReturn(IR_TIMER);
	}while(delay_time>=time);	
	timer_stop(IR_TIMER);
}
#endif

static void drv_l2_ir_stop_time(void)
{
	timer_stop(IR_TIMER);
}
#if 0
static void drv_l2_ir_int_set(void)
{
	vic_irq_register(VIC_EXT_AB,ir_extab_int);
	vic_irq_enable(VIC_EXT_AB);
}
#endif
static void drv_l2_ir_set_extB(INT32S inputedge)
{
	INT32U value;
	value = R_INT_KECON;
	value &= 0x6413;     //keep other flag status.
	if(inputedge==rising_edge)
		value |= 0xa2;	
	else
		value |= 0x82;	
	R_INT_KECON = value;	
}

static void drv_l2_ir_clr_extB(void)
{
	INT32U value;
	value = R_INT_KECON;
	value &= 0x6411;
	value |= 0x0080;
	R_INT_KECON = value;
}

static INT32S drv_l2_ir_check_io(void)
{
	if(gpio_read_io(IOC10)==DATA_HIGH)	
		return 1;
	else
		return 0;	
}

static void drv_l2_ir_set_io(void)
{
	IR_RX_Attrib 		|=IR_RX_IO;
	IR_RX_Dir 			&=~IR_RX_IO;	
	IR_RX_Buffer 		|=IR_RX_IO;   
    IR_RX_Data 			|=IR_RX_IO; 

//	gpio_init_io(IOC10, GPIO_INPUT);
//	gpio_set_port_attribute(IOC10,GPIO_INPUT);
//	gpio_write_io(IOC10, DATA_HIGH);
}

extern INT8U ir_get_data[max_byte_count];
extern INT8S ir_read,ir_flag;
static void ir_decode_data_api(INT32U *buf,INT8U byte_count)//add by crf
{
	INT8U i;
	INT32S j;
	INT32U  timevalue;
	INT8U   r_byte_vaule=0,check_bytesum=0;
	INT8U ir_data[max_byte_count];
	
	if(!ir_flag)return;
	
	for(i=0;i<byte_count;i++){
		for(j=0;j<8;j++){
			timevalue = *(INT32U*)(buf+j+i*8);
			if(timevalue >=200*2 ){
				r_byte_vaule |= (1<<j);
			}
		}
		
		ir_data[i]=r_byte_vaule;
		
		DBG_PRINT("vaule=%d\r\n",r_byte_vaule);
		if(i<(byte_count-1)){
			check_bytesum += r_byte_vaule;
			r_byte_vaule=0;
		}
	}
	
	if(check_bytesum == r_byte_vaule){
		Get_ir_data(ir_data);
		ir_read=1;
		DBG_PRINT("decode ok\r\n");
	}else
	{   ir_read=0;
		DBG_PRINT("lost decode\r\n");
	}
}
static void	drv_l2_ir_decode(void)
{
	INT32U  timevalue;
	
	INT32S i;
	INT8U   r_byte_0 ;
	INT8U   r_byte_1 ;
	INT8U   r_byte_2 ;
	INT8U   r_byte_3 ;
	INT8U   check_bytesum ;
	
	
	INT32U *pSavetimedataaddr_real;
	Time_to_Decode =0;

	
	timevalue = 0;
	r_byte_0 =0;
	r_byte_1 = 0;
	r_byte_2 = 0;
	r_byte_3 =0;
	
	if(Switch_Buffer != 0)
		pSavetimedataaddr_real = pSavetimedataaddr ;
	else 
		pSavetimedataaddr_real = pSavetimedataaddr_1;	
		
	ir_decode_data_api(pSavetimedataaddr_real,(max_value_bit_count/8));
	
	
/*	for(i=0;i<8;i++)
	{
	timevalue = *(INT32U*)(pSavetimedataaddr_real+i);
	//	if(timevalue >= 7)
		if(timevalue >=200 )    //add 2010.10.14
			r_byte_0 |= (1<<i);
	}
	DBG_PRINT("1=%d\r\n",r_byte_0);
	for(i=0;i<8;i++)
	{
		timevalue = *(INT32U*)(pSavetimedataaddr_real+i+8);
		if(timevalue >=200)//200
			r_byte_1 |= (1<<i);
	}
	DBG_PRINT("2=%d\r\n",r_byte_1);
	for(i=0;i<8;i++)
	{
		timevalue = *(INT32U*)(pSavetimedataaddr_real+i+16);
		if(timevalue >=200)
			r_byte_2 |= (1<<i);
	}
	DBG_PRINT("3=%d\r\n",r_byte_2);
	for(i=0;i<8;i++)
	{
		timevalue = *(INT32U*)(pSavetimedataaddr_real+i+24);
		if(timevalue >=200)
			r_byte_3 |= (1<<i);
	}
	DBG_PRINT("4=%d\r\n",r_byte_3);
	check_bytesum =(INT8U )( r_byte_0 + r_byte_1+r_byte_2);
	DBG_PRINT("cks=%d\r\n",check_bytesum);
	
	if(check_bytesum == r_byte_3)
	{
		DBG_PRINT ("\n\r ok: 0x%x\n\r", "OK");
	}
	else
	{
		lost_num_count++;
	}	
	receive_num_count++;
	DBG_PRINT("\n\r lost:0x%x\n\r",lost_num_count);
	DBG_PRINT ("\n\r All: 0x%x\n\r", receive_num_count);	
*/	
  #if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)	
//	DBG_PRINT ("\n\r Key Number ID: 0x%x\n\r", OperationCodeValue);
	//SendMSG( hApMessageQ, MSG_IR_KEY, (OperationCodeValue + MSG_IR_KEY) );
	ir_key_para.key_code = OperationCodeValue;
	ir_key_para.key_type = IR_KEY_DOWN_TYPE;
	ir_msg_QSend(ir_msgQId,ir_msgId,(void*)&ir_key_para,sizeof(IRkeySYSPara),0);
  #elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
    bin_ir_key_code = OperationCodeValue;
    bin_ir_key_type = (IR_KEY_TYPES_ENUM) IR_KEY_DOWN_TYPE;
  #endif
}

static void drv_l2_ir_sentrepeatcmd(void)
{
	Repeatcount++;
	if(Repeatcount >= 2)
	{
      #if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)	
//		DBG_PRINT ("\n\r Key Number ID: 0x%x\n\r", OperationCodeValue);
		//SendMSG( hApMessageQ, MSG_IR_KEY, (OperationCodeValue + MSG_IR_KEY) );
		ir_key_para.key_code = OperationCodeValue;
		ir_key_para.key_type = IR_KEY_REPEAT_TYPE;
		ir_msg_QSend(ir_msgQId,ir_msgId,(void*)&ir_key_para,sizeof(IRkeySYSPara),0);
		Repeatcount = 0;
      #elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)
        bin_ir_key_code = OperationCodeValue;
        bin_ir_key_type = (IR_KEY_TYPES_ENUM) IR_KEY_REPEAT_TYPE;
      #endif
	}	
}

static void ir_extab_int(void)
{
#if 0
	INT32U value;
	value = R_INT_KECON;
	if(value & 0x80)
	{
		value &= 0x64b3;
		R_INT_KECON = value;
#endif	
		switch(IR_Status)
		{
			case FindFirstTrigger:
				drv_l2_ir_clr_extB();		
				IR_Status = Check_IR_Header_1;		
				drv_l2_ir_start_time_with_irq(IR_FREG);
			//	IR_Status = Check_IR_Header_1;	
				break;
			case Check_IR_Header_1:
				drv_l2_ir_clr_extB();
				iTimetick = 0;
				IR_Status = Check_IR_Header_2;	
				drv_l2_ir_start_time_with_irq(IR_FREG);
				break;
			case Check_IR_Header_2:
				drv_l2_ir_stop_time();  //stop time irq;
			//	if(iTimetick<=6) 
			if(iTimetick<=7) 
				{
					if(Repeat_Flag == 1)
					{
						iTimetick = 0;
						IR_Status = Receive_IR_Repeat_CMD;
						drv_l2_ir_start_time_with_irq(IR_FREG1);				
					}
					else
					{
						drv_l2_ir_clr_extB();
						iTimetick = 0;	
						IR_Status = FindFirstTrigger;	
						drv_l2_ir_set_extB(falling_edge);			
					}
				}		
			//	else if(iTimetick>=7) 
			else if(iTimetick>=8) 
				{
					iTimetick = 0;			
					iReceivecount = 0x2;
					IR_Status = Receive_IR_Data;
					drv_l2_ir_start_time_without_irq();
				}
				break;
			case Receive_IR_Data:
				iTimetick = drv_l2_ir_get_time_value();
	//			if((iTimetick > 12)||(iTimetick<3))			//去掉杂讯干扰
				//if((iTimetick > 300)||(iTimetick<80))			//去掉杂讯干扰  add 2010.10.14
				//if((iTimetick > 500)||(iTimetick<120))
//				test_real_parameter(iTimetick,min_data_1,max_data_1,min_data_2,max_data_2);
			
				if((iTimetick >400*2)||(iTimetick<50*2))	  //860   100
				{
					iTimetick = 0;	
					IR_Status = FindFirstTrigger;	
					drv_l2_ir_clr_extB();	
					drv_l2_ir_set_extB(falling_edge);	
				}
		
				if(Switch_Buffer != 0)
				{	
					*(INT32U*)(pSavetimedataaddr_1+(iReceivecount-2))=(INT32U)iTimetick;
				}
				else
				{
					*(INT32U*)(pSavetimedataaddr+(iReceivecount-2))=(INT32U)iTimetick;
				}
				
				
				
				iTimetick = 0;
				iReceivecount++;
				drv_l2_ir_start_time_without_irq();
				if(iReceivecount >= IOTriggerMaxNum)
				{	
					Switch_Buffer = ~Switch_Buffer;
					drv_l2_ir_clr_extB();
					drv_l2_ir_reinit();
					iTimetick = 0;
					//drv_l2_ir_decode();
					Repeat_Flag = 1;	
					//drv_l2_ir_delay(0xa4);	//delay 40ms (why delay 40ms?)*/			
					Time_to_Decode = 1;
					OSQPost(IRTaskQ,(void *)MSG_IR_DECODE_START);
				}
				break;
/*			case Receive_IR_Repeat_CMD:
	
				drv_l2_ir_stop_time();
				if((iTimetick>=47)&&(iTimetick<50))
				{
					drv_l2_ir_sentrepeatcmd();	
					drv_l2_ir_clr_extB();
					iTimetick = 1;	
					IR_Status = Check_IR_Header_1;	
					drv_l2_ir_start_time_with_irq(IR_FREG);						
				}
				else
				{
					Repeat_Flag = 0;
					drv_l2_ir_clr_extB();
					iTimetick = 1;	
					IR_Status = Check_IR_Header_1;	
					drv_l2_ir_start_time_with_irq(IR_FREG);						
				}
			
				break;
*/		
		}	
	//}
}

static void ir_time_tick(void)
{
	INT32S ret;
	
	iTimetick++;
	
	switch(IR_Status)
	{
		case Check_IR_Header_1:
			ret = drv_l2_ir_check_io();
			if(ret==0)
			{
				if(iTimetick >= IR_Header_1)
				{
					drv_l2_ir_stop_time();
					drv_l2_ir_set_extB(rising_edge);
				}	
			}
			else
			{
				iTimetick = 0;	
				IR_Status = FindFirstTrigger;	
				drv_l2_ir_stop_time();
				drv_l2_ir_set_extB(falling_edge);
			}
			break;

		case Check_IR_Header_2:
			ret = drv_l2_ir_check_io();
			if(ret == 1)
			{
				//if(iTimetick == IR_Header_2)
				if(iTimetick >= IR_Header_2)
				{
					drv_l2_ir_set_extB(falling_edge);
				}
				//else if(iTimetick > 10)
				else if(iTimetick > 11)
				{
					iTimetick = 0;	
					IR_Status = FindFirstTrigger;	
					drv_l2_ir_stop_time();
					drv_l2_ir_set_extB(falling_edge);						
				}
			}
			else
			{
				iTimetick = 0;	
				IR_Status = FindFirstTrigger;	
				drv_l2_ir_stop_time();
				drv_l2_ir_set_extB(falling_edge);							
			}
			break;
			
/*		case Receive_IR_Repeat_CMD:
			
			if(iTimetick>=50)
			{
				Repeat_Flag = 0;
				drv_l2_ir_stop_time();
				drv_l2_ir_clr_extB();
				drv_l2_ir_reinit();
			}

			break;
*/
		default:
			break;
	}
}

static void drv_l2_ir_reinit(void)
{
	INT32U i;
	iTimetick = 0;	
	iReceivecount = 0;
	IR_Status = FindFirstTrigger;	
/*	if(Switch_Buffer == 0)
	{
		for(i=0;i<32;i++)
			Savetimebuffer[i]=0;	
	}
	else 
	{
		for(i=0;i<32;i++)
			Savetimebuffer_1[i]=0;	
	}
*/
	drv_l2_ir_set_extB(falling_edge);	
}

void drv_l2_ir_init(void)
{
	INT32U i;
	iTimetick = 0;
	iReceivecount = 0;
	Repeat_Flag = 0;
	Repeatcount = 0;
	OperationCodeValue = 0;
	pSavetimedataaddr_1 = (INT32U *)Savetimebuffer_1;  //add by crf 2010.1018
	pSavetimedataaddr = (INT32U *)Savetimebuffer;
	drv_l2_ir_set_io();
	
	//drv_l2_ir_int_set();
	extab_user_isr_set(EXTB,ir_extab_int);
	
	for(i=0;i<max_value_bit_count;i++)
		Savetimebuffer[i]=0;
		
	for(i=0;i<max_value_bit_count;i++)
		Savetimebuffer_1[i]=0;
	IR_Status = FindFirstTrigger;	
	//drv_l2_ir_set_extB(falling_edge);
}

INT32S drv_l2_getirkey(void)
{
	return 0;
}

#if (defined _PROJ_TURNKEY) && (_PROJ_TYPE == _PROJ_TURNKEY)
void turnkey_irkey_resource_register(void* msg_qsend,void* msgq_id,INT32U msg_id)
{
	ir_msg_QSend = (fp_msg_qsend) msg_qsend;
	ir_msgQId = msgq_id;
	ir_msgId = msg_id;
}


#elif (defined _PROJ_BINROM) && (_PROJ_TYPE == _PROJ_BINROM)

IR_KEY_TYPES_ENUM bin_ir_press_status_get(void)
{
    IR_KEY_TYPES_ENUM ret;

    ret = bin_ir_key_type;
    bin_ir_key_type=IR_KEY_NONE_TYPE;
    return ret;
}

INT8U bin_ir_keycode_get(void)
{
    INT8U ret;

    ret = bin_ir_key_code;
    bin_ir_key_code=KEY_CODE_NONE;
    return ret;
}

#endif
//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L2_IR) && (_DRV_L2_IR == 1)                //
//================================================================//