#ifndef	__DATA_h__
#define	__DATA_h__

#include "drv_l1_gpio.h"
#include "touch_panel.h"
#include "drv_l1_tv.h"
#include "setting.h"
#include "key.h"


#define IR_TEST		0
#if JTAG==1
	#define	DISKUSED		    FS_SD
	//#define	DISKUSED		    FS_NAND1
#else
	#define	DISKUSED		    FS_NAND1
#endif

//__GC0329_DRV_C_	CMOS_POS	checksum


#define	_1K				1024

#define DISPLAY_TV              		0
#define DISPLAY_TFT						1
#define DISPLAY_NONE					0xFF
#define	DISPLAY_DEVICE				    DISPLAY_TFT

#define	QVGA_MODE			      		0
#define VGA_MODE						1
#define D1_MODE							2
#define TFT_320x240_MODE				3
#define TFT_640x480_MODE				4
#define TFT_800x480_MODE				5
#define TFT_480x272_MODE				6
#define TFT_128x128_MODE				7
#define TFT_240X320_MODE				8
#define TV_TFT_MODE						TFT_480x272_MODE
#define	PPU_MODE						VGA_MODE

#define	DISPLAYUSED		    	TV_TFT_MODE

#define	SENSOR_WIDTH	640
#define	SENSOR_HEIGHT	480

#if	DISPLAYUSED==TFT_480x272_MODE
	#define PPU_TEXT_SIZE_HPIXEL4   480
	#define PPU_TEXT_SIZE_VPIXEL4   272	
#elif	DISPLAYUSED==TFT_320x240_MODE
	#define PPU_TEXT_SIZE_HPIXEL4   320
	#define PPU_TEXT_SIZE_VPIXEL4   240	
#elif PPU_MODE==VGA_MODE
	#define PPU_TEXT_SIZE_HPIXEL4   640
	#define PPU_TEXT_SIZE_VPIXEL4   480	
#elif PPU_MODE==QVGA_MODE
	#define PPU_TEXT_SIZE_HPIXEL4   320
	#define PPU_TEXT_SIZE_VPIXEL4   240	
#endif

#define BMP_RGB1555  	0
#define BMP_RGB565  	1
#define BMP_YUYV  		3


#define SENSOR_IR		0
#define SENSOR_QVGA		1     //320x240
#define SENSOR_VGA		2     //640x480
#define SENSOR_SVGA		3     //1280x720
#define SENSOR_XVGA		4     //1600x1200

#define SENSOR_QVGA2	11     //400x300
#define SENSOR_VGA2		12     //800x600


//#define PENDEC	1
#define C_PPU_DRV_FRAME_NUM			2
#define FRAME_SIZE 		(PPU_TEXT_SIZE_HPIXEL4 * PPU_TEXT_SIZE_VPIXEL4 )


#define		SFX_ADDR		0x7FF000
#define		SFX_ADDR_GAME	(SFX_ADDR-200*_1K)
#define		SFX_PUBLIC		100


#define BLING_SFX		(SFX_PUBLIC+0)
#define MENU_OPEN_SFX	(SFX_PUBLIC+1)
#define OFF_SFX			(SFX_PUBLIC+2)
#define OK_SFX			(SFX_PUBLIC+3)
#define ON_SFX			(SFX_PUBLIC+4)
#define TAP_SFX			(SFX_PUBLIC+5)
#define WARNING_SFX		(SFX_PUBLIC+6)

#define SHOT_SFX			(SFX_PUBLIC+7)
#define Mail_receive_SFX	(SFX_PUBLIC+8)
#define Next_slide_SFX		(SFX_PUBLIC+9)
#define Icon_select_SFX		(SFX_PUBLIC+10)

#define TOUCH_SFX		TAP_SFX



#define MUSIC_CHCHANL	15
#define SPEECHCHANL		14
#define SFX_CHANL		13
#define WAITSTEP		20
#define VOLUME_MAX		127
#define VOL_MAX_FAME	3

#define WAITINGTIME (25*60*5)

/***************** sprite define **********************/
// Define Sprite CDM Mode Enable
#define SP_CMD_COMPARE()             (R_PPU_SPRITE_CTRL & cdm_enable)
#define EXSP_CMD_COMPARE()           (R_PPU_EXTENDSPRITE_CONTROL & excdm_enable)

// Define Max V3D buffer size
#define SP25D_MAX_CHAR_V             8
#define SP25D_MAX_CHAR_H             8

// Define Sprite Coordinate Fraction Mode Enable
#define SP_FRACTION_ENABLE           0

// Define Max sprite ram size and buffer size 
#define Spritebuf_number             256
#define Sprite_ptr_number            1
#define SpriteRAM_number             512
#define SpriteExRAM_number           1
#define ExSpritebuf_number           8
#define ExSpriteRAM_number           16

// Define sprite constant
#define spirte_pos_max_bit           0x3FF
#define max_sp_combination_number    512
#define data_end_word                0xFFFF
#define hflip_bit                    0x0004
#define vflip_bit                    0x0008
#define pos_x                        0
#define sp_init_en                   0x8
#define p1024_enable                 0x10
#define spv3d_enable                 0x200
#define pal_control_0                7
#define pal_control_1                15
#define cdm_enable                   0x20000
#define excdm_enable                 0x2
#define sp_frac_x1                   4
#define sp_frac_x2                   8
#define sp_frac_x3                   12
#define exsp_frac_x0                 8
#define exsp_frac_x1                 12
#define sp_frac_y0                   2
#define sp_frac_y1                   6
#define sp_frac_y2                   10
#define sp_frac_y3                   14
#define exsp_frac_y0                 10
#define exsp_frac_y1                 14
#define b_spn_group                  28
#define mask_group_id                0x3
#define sp_cdm_init_en               (0x1<<11)
#define sp_no_cdm_en                 (0x1<<15)
#define b_cdm_exspn_group            12
/****************************************************/


int key_Sensor(void);
int SIO_Sensor(void);

#define SPI_CS 81
#define C_SPI_NUM	SPI_0

#define SIO_CS	 	0x00
#define SIO_CLK 	0x01
#define SIO_DB	 	0x02


#define CPY_Palette Palette_ram_cpy


extern INT8U Game_State;
extern INT32U buf_ck,scaler_buf_1,scaler_buf_2;
extern INT32U Tx1_buffer;
extern INT32U Tx2_buffer;
extern INT32U Tx3_buffer;
extern INT32U Tx4_buffer;
extern INT16S game;

extern INT32U uSensorFlag;
extern	INT32U SensorBuff1,SensorBuff2;
extern	INT32U Sensor_Md;
extern	INT32U sen_baseaddr;

extern INT32U sensor_buf_1,sensor_buf_2,ppu_buf,tv_display_flag;
extern INT32U SensorFlag;
extern INT32U TVOutBuff,ScalerBuff,VideoBuff;
extern INT32U IconBuff;
extern INT32U Tx1_BMP_BUFF,Tx2_BMP_BUFF;

extern INT16S playdelay;  
   
extern INT32U TV_buffer[2];
extern PPU_REGISTER_SETS ppu_register_structure;
extern PPU_REGISTER_SETS *ppu_register_set;

extern INT32U	DrmAddr[];
extern int Curren_Frame;
extern int game_time;
extern DMA_STRUCT dma_struct;

extern int PrintfCounter;
extern INT16S SpeechBuff[];
extern INT16S step;
extern INT8U Game_State;
extern INT8S musicStop;

extern PPU_REGISTER_SETS ppu_register_structure;
extern PPU_REGISTER_SETS *ppu_register_set;
extern INT32U Tx1_buffer;
extern INT32U Tx2_buffer;
extern INT32U Tx3_buffer;
extern INT32U Tx4_buffer;

extern INT32U Tx1_BMP_BUFF,Tx2_BMP_BUFF;

extern INT32U Spr_buffer;

extern INT32U TV_buffer[2];
extern INT32U T80_buffer;
extern const INT32U SPSIZE[];

extern INT16U PaletteBuff0[256];
extern INT16U PaletteBuff1[256];
extern INT16U PaletteBuff2[256];
extern INT16U PaletteBuff3[256];


#define TX_COLOR4		0
#define TX_COLOR16		1
#define TX_COLOR64		2
#define TX_COLOR256		3

#define TX_BANK0		0			
#define TX_BANK1		1
#define TX_BANK2		2
#define TX_BANK3		3

#define TX_HSIZE8		0
#define TX_HSIZE16		1
#define TX_HSIZE32		2
#define TX_HSIZE64		3

#define TX_VSIZE8		0
#define TX_VSIZE16		1
#define TX_VSIZE32		2
#define TX_VSIZE64		3
	
#define	TX_DEPTH0 		0	
#define	TX_DEPTH2 		1	
#define	TX_DEPTH4 		2	
#define	TX_DEPTH6 		3


#define					TX_PALETTE0 	0
#define					TX_PALETTE1 	1
#define					TX_PALETTE2 	2
#define					TX_PALETTE3 	3
#define					TX_PALETTE4 	4
#define					TX_PALETTE5 	5
#define					TX_PALETTE6 	6
#define					TX_PALETTE7 	7
#define					TX_PALETTE8 	8
#define					TX_PALETTE9 	9
#define					TX_PALETTE10 	10
#define					TX_PALETTE11 	11
#define					TX_PALETTE12 	12
#define					TX_PALETTE13 	13
#define					TX_PALETTE14 	14
#define					TX_PALETTE15 	15

//------------------------------------------------------------------------------------------------------
#define					SPBLEND_DISABLE		(0 << 14)	// Set Sprite Blending effect disable
#define					SPBLEND_ENABLE		(1 << 14)   // Set Sprite Blending effect enable
//------------------------------------------------------------------------------------------------------
// Define Sprite Depth 0 - 3
//------------------------------------------------------------------------------------------------------
#define					SP_DEPTH1 			(0 << 12)	
#define					SP_DEPTH3 			(1 << 12)	
#define					SP_DEPTH5 			(2 << 12)	
#define					SP_DEPTH7 			(3 << 12)	
//------------------------------------------------------------------------------------------------------
// Define Sprite Palette bank 0 - 15
//------------------------------------------------------------------------------------------------------
#define					SP_PALETTE0 		(0 << 8)	
#define					SP_PALETTE1 		(1 << 8)
#define					SP_PALETTE2 		(2 << 8)
#define					SP_PALETTE3 		(3 << 8)
#define					SP_PALETTE4 		(4 << 8)
#define					SP_PALETTE5 		(5 << 8)
#define					SP_PALETTE6 		(6 << 8)
#define					SP_PALETTE7 		(7 << 8)
#define					SP_PALETTE8 		(8 << 8)
#define					SP_PALETTE9 		(9 << 8)
#define					SP_PALETTE10 		(10 << 8)
#define					SP_PALETTE11 		(11 << 8)
#define					SP_PALETTE12 		(12 << 8)
#define					SP_PALETTE13 		(13 << 8)
#define					SP_PALETTE14 		(14 << 8)
#define					SP_PALETTE15 		(15 << 8)

#define M_PI 3.14159265358979323846
#define Center_coordinate_mode      0
#define Left_Top_coordinate_mode    2


//-----------------------------------------------

// tft io pin setting
#define C_TFT_DATA						IO_B9//IO_C2
#define C_TFT_CLK						IO_B8//IO_C1
#define C_TFT_CS						IO_B0//IO_C3

#define	PPU_YUYV_TYPE3					(3<<20)
#define	PPU_YUYV_TYPE2					(2<<20)
#define	PPU_YUYV_TYPE1					(1<<20)
#define	PPU_YUYV_TYPE0					(0<<20)

#define	PPU_RGBG_TYPE3					(3<<20)
#define	PPU_RGBG_TYPE2					(2<<20)
#define	PPU_RGBG_TYPE1					(1<<20)
#define	PPU_RGBG_TYPE0					(0<<20)

#define PPU_LB							(1<<19)
#define	PPU_YUYV_MODE					(1<<10)
#define	PPU_RGBG_MODE			        (0<<10)

#define TFT_SIZE_800X480                (5<<16)
#define TFT_SIZE_720x480				(4<<16)
#define TFT_SIZE_480x272				(3<<16)
#define TFT_SIZE_480x234				(2<<16)
#define TFT_SIZE_640X480                (1<<16)
#define TFT_SIZE_320X240                (0<<16)

#define	PPU_YUYV_RGBG_FORMAT_MODE		(1<<8)
#define	PPU_RGB565_MODE			        (0<<8)

#define	PPU_FRAME_BASE_MODE			    (1<<7)
#define	PPU_VGA_NONINTL_MODE			(0<<5)

#define	PPU_VGA_MODE					(1<<4)
#define	PPU_QVGA_MODE					(0<<4)
//------------------------------------------------

#define	_1K				1024
#define	SP_CellSize		(20400*_1K)
#define BUTTON_SIZE		(400*_1K)

#define BUTTON_IDX	(SP_CellSize/_1K)
#define PUB_SP_No	80	


extern INT32U SpriteCellData;
extern char rootPath[];

extern INT8U s_usbd_pin;

extern INT16S homeFlag;
extern INT8S volum,LCD_light;
extern INT8S SleepKey;
extern INT16S sleeptime;
extern INT16S Disk_Type,Save_SD;


extern INT32U TmpBuff1;
extern INT32U TmpBuff2;
extern INT32U TmpBuff3;
extern INT32U pFileList,pFileList2;
extern INT8S alarmFlag,alarm;
extern INT8S alarm_hour,alarm_minute,alarm_music,CallSound,TalkSound;
extern INT16S File_Max,File_No,File_Page;
extern INT16S File_SaveMax,File_SaveMax2;
extern INT8U Hour_24,MM_DD;
//-----------------------------------------------
extern INT16S Desk_flag,alarmtime,OS_game,power_show;
extern t_rtc Rtc_time,Rtc_alarm;

extern INT16S saveType,SD_Mount;

extern INT16U Time_day,Week;
extern INT16U Time_hour,Time_min,Time_sec;
extern INT16U Day,Month,Year;

extern PLAYER player;
extern INT16S Call_act,Call_item,Locked;

//====================================================================
extern INT16S Mouse_X,Mouse_Y;
extern int Key_State;	//连续按
extern int Key_old;
extern int Key_States;	//单按
extern int Key_Release;//释放按键
#endif
