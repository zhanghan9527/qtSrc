#ifndef __TOUCH_PANEL_H_
#define __TOUCH_PANEL_H_

#include "data.h"

#define		TMP_MODE	1	//0 old(IOB);	1-new(IOB+IOG)

#define		TFT_FLAG	1
#define		KEY_FLAG	1
#define		ENC_FLAG	1
#define 	SLEEP_FLAG	1

#define		TPC_TEST		1
#define		E2FLASH_FLAG	1
#define		LV_FLAG			1
#define		CARDIN_FLAG		1
//========================================================
#define WIDTH		PPU_TEXT_SIZE_HPIXEL4
#define HIGHT		PPU_TEXT_SIZE_VPIXEL4

#define FIFOSize 	4
#define FIFOREAD 	0

#define TPIO_LINE0		0x40	//IO_F6 ,7	//IOH 6-7
#define TPIO_LINE2		0x80 
#define TPIO_LINE		(TPIO_LINE0 | TPIO_LINE2)

#if TMP_MODE==0
	#define TPIO_X0		0x1000		//IO_B 10,11,12,13
	#define TPIO_X1		0x2000
	#define TPIO_Y0		0x0400
	#define TPIO_Y1		0x0800
	#define TPIO_XY		(TPIO_X0 | TPIO_X1 | TPIO_Y0 | TPIO_Y1)

	#define  READ_X_IO				\
				*(int*)IOB_DIR_ADDR   |= (TPIO_X0 | TPIO_X1);	\
				*(int*)IOB_BUFFER_ADDR = (*(int*)IOB_BUFFER_ADDR&(~TPIO_XY))|TPIO_X0; \
				*(int*)IOB_DIR_ADDR   &= ~(TPIO_Y0 | TPIO_Y1) 		 //IOB13 HIGH  ,IOB12  low  读x轴坐标前初始化IO状态
	#define  READ_Y_IO				\
				*(int*)IOB_DIR_ADDR   |= (TPIO_Y0 | TPIO_Y1);	\
				*(int*)IOB_BUFFER_ADDR = (*(int*)IOB_BUFFER_ADDR&(~TPIO_XY))|TPIO_Y0; \
				*(int*)IOB_DIR_ADDR   &= ~(TPIO_X0 | TPIO_X1)		 //IOB11 HIGH ,IOB10  low	读y轴坐标前初始化IO状态

	#define  READ_X_AD				\
				R_ADC_MADC_CTRL = (R_ADC_MADC_CTRL&0xFFF8) |0x0001			// select ain1 //读x轴坐标选用的ad通道
	#define  READ_Y_AD			\
				R_ADC_MADC_CTRL = (R_ADC_MADC_CTRL&0xFFF8) |0x0000			// select ain3 //读y轴坐标选用的ad通道	

#else

	#define TPIO_X0		0x4000		//IOG_14,15
	#define TPIO_X1		0x8000
	#define TPIO_Y0		0x0100		//IO_I 8,9
	#define TPIO_Y1		0x0200
	#define TPIO_X2		(TPIO_X0 | TPIO_X1)
	#define TPIO_Y2		(TPIO_Y0 | TPIO_Y1)

	#define  READ_X_IO				\
				*(int*)IOG_DIR_ADDR   |= (TPIO_X0 | TPIO_X1);	\
				*(int*)IOG_BUFFER_ADDR = (*(int*)IOG_BUFFER_ADDR&(~TPIO_X2))|TPIO_X0; \
				*(int*)IOI_BUFFER_ADDR = (*(int*)IOI_BUFFER_ADDR&(~TPIO_Y2));	\
				*(int*)IOI_DIR_ADDR   &= ~(TPIO_Y0 | TPIO_Y1) 		 //IOB13 HIGH  ,IOB12  low  读x轴坐标前初始化IO状态
	#define  READ_Y_IO				\
				*(int*)IOI_DIR_ADDR   |= (TPIO_Y0 | TPIO_Y1);	\
				*(int*)IOI_BUFFER_ADDR = (*(int*)IOI_BUFFER_ADDR&(~TPIO_Y2))|TPIO_Y0;	\
				*(int*)IOG_BUFFER_ADDR = (*(int*)IOG_BUFFER_ADDR&(~TPIO_X2)); \
				*(int*)IOG_DIR_ADDR   &= ~(TPIO_X0 | TPIO_X1)		 //IOB11 HIGH ,IOB10  low	读y轴坐标前初始化IO状态

	#define  READ_X_AD				\
				R_ADC_MADC_CTRL = (R_ADC_MADC_CTRL&0xFFF8) |0x0005			// select ain1 //读x轴坐标选用的ad通道
	#define  READ_Y_AD			\
				R_ADC_MADC_CTRL = (R_ADC_MADC_CTRL&0xFFF8) |0x0004			// select ain3 //读y轴坐标选用的ad通道	

#endif


#define 	LV_LINE		BIT9
#define  READ_LV_AD				\
			R_ADC_MADC_CTRL = (R_ADC_MADC_CTRL&0xFFF8) |0x0003		// select ain3
//========================================================

#define KEY_INTPUT1		0x07f0		//IOC  4-10

#define KEY_VOLUP		((INT64U)1<<4)
#define KEY_VOLDOWN		((INT64U)1<<5)
#define KEY_HOME		((INT64U)1<<6)
#define KEY_REPEAT		((INT64U)1<<7)
#define KEY_ANSWER		((INT64U)1<<8)
#define KEY_MUSIC		((INT64U)1<<9)
#define KEY_TOOL		((INT64U)1<<10)


#define KEY_LEFT		((INT64U)1<<11)
#define KEY_RIGHT		((INT64U)1<<12)
#define KEY_UP			((INT64U)1<<13)
#define KEY_DOWN		((INT64U)1<<14)
#define KEY_OK			((INT64U)1<<15)




#define KEY_START	((INT64U)1<<16)
#define KEY_SELECT	((INT64U)1<<17)
#define KEY_A		((INT64U)1<<16)
#define KEY_B		((INT64U)1<<17)
#define KEY_ENTER	((INT64U)1<<31)

#define KEY_SPACE	((INT64U)1<<31)

//#define KEY_TOOL	((INT64U)1<<31)
#define KEY_DOT		((INT64U)1<<31)



#define KEY_N1		KEY_Q
#define KEY_N2		KEY_W
#define KEY_N3		KEY_E
#define KEY_N4		KEY_R
#define KEY_N5		KEY_T
#define KEY_N6		KEY_Y
#define KEY_N7		KEY_U
#define KEY_N8		KEY_I
#define KEY_N9		KEY_O
#define KEY_N10		KEY_P

#define KEY_DO		KEY_A
#define KEY_RE		KEY_S
#define KEY_MI		KEY_D
#define KEY_FA		KEY_F
#define KEY_SOL		KEY_G
#define KEY_LA		KEY_H
#define KEY_SI		KEY_J
#define KEY_DO2		KEY_K
#define KEY_RE2		KEY_L

//========================================================

#define ENC_INPUT	0x08
#define ENC_OUTPUT	0x04

#define	ACK			0
#define	NACK		1
#define P_I2C_SDA 		0x4000
#define P_I2C_SCL		0x8000

#define P_I2C_Attrib 	*(int*)IOB_ATTRIB_ADDR
#define P_I2C_Dir 		*(int*)IOB_DIR_ADDR
#define P_I2C_Data 		*(int*)IOB_DATA_ADDR
#define P_I2C_BUFF 		*(int*)IOB_BUFFER_ADDR

#define P_CARDIN 		0x08	//ioC3

//#define T80_RESET_PIN   IO_B4	//
#define T430_RESET_PIN   IO_B5	//
#define T430_DSP_PIN   IO_B6	//


#define C_USBDEVICE_PIN   IO_F5 

#define  		PWR_HOLD		BIT13
#define 		PWR_HOLD_Attrib 		IOG_ATTRIB_ADDR
#define 		PWR_HOLD_Dir 			IOG_DIR_ADDR
#define 		PWR_HOLD_Data 			IOG_DATA_ADDR
#define 		PWR_HOLD_Buffer 		IOG_BUFFER_ADDR

#define 		PWR_DETECT		BIT14
#define 		PWR_DETECT_Attrib 		IOG_ATTRIB_ADDR
#define 		PWR_DETECT_Dir 			IOG_DIR_ADDR
#define 		PWR_DETECT_Data 		IOG_DATA_ADDR
#define 		PWR_DETECT_Buffer 		IOG_BUFFER_ADDR
#define 		PWR_DETECT_DRV 			IOG_DRV



#define 		LOW_POWER				LV_LINE
#define 		LOW_POWER_Attrib 		IOF_ATTRIB_ADDR
#define 		LOW_POWER_Dir 			IOF_DIR_ADDR
#define 		LOW_POWER_Data 			IOF_DATA_ADDR
#define 		LOW_POWER_Buffer 		IOF_BUFFER_ADDR
#define 		LOW_POWER_DRV 			IOF_DRV



#define 		SENSOR_ON		BIT7
#define 		SENSOR_Attrib 		IOF_ATTRIB_ADDR
#define 		SENSOR_Dir 			IOF_DIR_ADDR
#define 		SENSOR_Data 		IOF_DATA_ADDR
#define 		SENSOR_Buffer 		IOF_BUFFER_ADDR
#define 		SENSOR_DRV 			IOF_DRV	

#define 	TFT_LIGHT0	 IO_F15
#define 	TFT_LIGHT1	 IO_G13

//*****************************************************************************************
// Global Variable Defintion Area
//*****************************************************************************************
void tv_disable(void);
INT16U GetRandom(INT16U Max);

extern unsigned int TouchFlag,TouchFlags,TouchRelease;
extern int Coordinate_X,Coordinate_Y;
extern int TouchTest;
extern int YCenter,DivValueY,MODIFY_Y;
extern int XCenter,DivValueX,MODIFY_X;
//extern int NOTOUCH;
extern INT16S DragOld_X,DragOld_Y;


#define I2c_ID  0xa0 //	0x64 是 RTC ; 0xa0 是 E2_flag

extern INT16U I2C_Flag,E2_Flag;	//0--right call ,1---write,2---read,0xff---error
extern INT8U I2C_Data[];

extern INT16S LVLevel;
extern INT16U Encflag;

extern INT16S mov_len;
extern INT16S DragDelay;
extern INT16S mov_dxy;

//===========================================================
void TP_IOInit(void);			//触摸屏
void TouchPanelService(void);
unsigned int Average_X_FIFO(void);
unsigned int Average_Y_FIFO(void);

void KEY_IOInit(void);			//矩阵键盘

void ENC_IOInit(void);			//加密
void ENCService(void);

void initI2c(void);
void I2c_Write(INT16U SA,INT16U SubAddr,INT16U n);	//n 最多8个，且 SubAddr%8 +n <=8
void I2c_Read(INT16U SA,INT16U SubAddr,INT16U n);	//数据放在 I2C_Data[8]
void TestE2Flash(void);

void GetLV(void);

void InitCardin(void);
int TestCardin(void);
int testJust(void);
void SaveJust(void);

INT16S TestDragRel(INT16S dx[],INT16S dy[]);
INT16S TestDragRel_2(INT16S HV,INT16S Max_V);



#endif