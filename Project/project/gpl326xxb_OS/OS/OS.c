#include "application.h"
#include "data.h"
#include "math.h"
#include "menu_sp.h"
#include "Spy_watch.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "ir.h"

#define ASCII_Big	ASCII_Big_IDX
#define ASCII_Small	ASCII_IDX
#define N_BigStr	7

#define	Drag_Dx		12

INT32U PIC_Nums=1,PicSd_Nums=1,VidSd_Nums=1;
INT32U AUDIO_Nums=1,VID_Nums=1;
INT16S File_SaveMax=0;


INT16S power_show=0;
INT16S desk_page=0;
INT16S Menu_Lauout,Desk_bg;

extern const INT16U ALIGN4 _All_Buttons_CellData[],spy_SPRITE_Palette1[],spymenu_SPRITE_Palette1[];
extern const INT16U ALIGN4 OS_SPRITE_Palette1[];
extern INT8U	alarmSoundLockFlag[3];
extern INT16U EventFlag;
//===========================================================
INT16S OS_game=0,PowerOff_Video=0;
INT16S Desk_flag=0,Save_SD=0;
t_rtc Rtc_time,Rtc_alarm;
//===========================================================


const char* SfxName_Pub[]={
	"Pororo\\pub_sfx\\bling.drm",
	"Pororo\\pub_sfx\\menu_open.drm",	
	"Pororo\\pub_sfx\\OFF.drm",	
	"Pororo\\pub_sfx\\OK.drm",	
	"Pororo\\pub_sfx\\ON.drm",
	
	"Pororo\\pub_sfx\\tap.drm",
	"Pororo\\pub_sfx\\warning.drm",

	"Pororo\\pub_sfx\\shot.drm",
	"Pororo\\pub_sfx\\mail_receive.drm",
	"Pororo\\pub_sfx\\next_slide.drm",
	"Pororo\\pub_sfx\\icon_select.drm",
};
extern const INT32U  pub_WarningSp_Frame[];
void Test_Reset()
{
	INT16S m;
	spMalloc(900);
	
	INIT_VideoPub(0);
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	clear_sprite(Spritebuf_number);
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);
	m=0;
	while(1){
		set_sprite_display_init(0,0,0,pub_WarningSp_Frame[6+m]);
		PaintTFT_cell4();
		if(KeyBoardCode==K_Left || KeyBoardCode==K_Right){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
			m=(m+1)%2;
		}else if(KeyBoardCode==K_Ok || KeyBoardCode==K_Enter){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);
			if(m==1) clear_all_user_data();
			break;
		}
	}
}
INT32U PubSFX_ADDR=0;
//===========================================================
void Main_OS()
{
	INT16S i,j;
	INT16S ret,ProAct=0;

	textMalloc();
	Screen_init();
	InitState();

//	TP_IOInit();	//init touch	
	AD_Init();		//init AD for LV
	KEY_IOInit();	//init key
	InitCardin();	//init SD

	testIOHigh();
	R_MEM_IO_CTRL &=~ BIT13;	
	SetWaveIo(1);	//open 喇叭
	i=(INT8U)dac_pga_get();
	dac_pga_set((INT8U)i);
	//Close_Sensor();
	//PowerOn();
	
	//DrvNand_initial();
	mount_and_mk_ch_dir();

	//while(1);
	main_open();

	
	PubSFX_ADDR=(INT32U)gp_malloc_align(100*_1K, 4);
	LoadDRM(0,11,PubSFX_ADDR+100*_1K,(int*)&DrmAddr[SFX_PUBLIC],SfxName_Pub);	
	Desk_flag=0;

	//testmode();
	#if(DISKUSED==FS_NAND1)
		//testmode();
	#endif

	//Test_Reset();
	if(*(INT8U *)0x7FFFFF!=0xa5){
		//Act_logo();
		*(INT8U *)0x7FFFFF=0xa5;
	}else if(DISKUSED==FS_NAND1){
		//Test_Reset();
	}			 //*/

	Init_Setting();
	Load_player();
	Act_LoadNums();
	Act_LoadSetting();
	GetInBoxMail();
	
	GetSD_Nums();
	Get_Rtc_Data();
	if(player.InBox_Day!=Day) player.InBox_Day=0;
	GetNoMail();
	SleepKey=0;
	if(volum<0 || volum>=VOL_MAX_FAME) volum=VOL_MAX_FAME;
	SPU_Set_MainVolume(VOLUME_MAX*volum/VOL_MAX_FAME);
	set_led_light(LCD_light);
	Words_B_20();
	Close_Sensor();
	
#if IR_TEST==1	
	ir_task_demo();	
//	OSQPost(IRTaskQ,(void *)MSG_IR_SEND_API_INIT);
//	OSTimeDly(1);	
//	OSQPost(IRTaskQ,(void *)MSG_IR_DECODE_API_INIT);
    ir_read=0;
#endif	
	//LoadData_1(0,buttonCellData,"iWatch\\pub\\SpriteCell.bin");
	//gplib_ppu_fade_effect_set(ppu_register_set, 0);
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);
	
	if(OS_game==2){ //rtc断电了。第一次开机
		//M22_main(0);
		//M24_main(0);
		//buff_Malloc(0,SP_CellSize,0);	
	}
	Desk_flag=1;
	OS_game=2;
	while(1)
	{
		switch(OS_game){
	//-------------------	//desktop
			case 0:		//spy_menu				
				sleeptime=WAITINGTIME;
				if(EventFlag ){
					OS_game=20;
					break;
				}
				main_menu();
			break;
			case 1:		//spy_menu				
				sleeptime=WAITINGTIME;		
				main_words();
				GotoGame(1,0);
			break;
			case 2:		//spy_menu				
				sleeptime=WAITINGTIME;	
				Desk_flag=0;	
				main_open();
				GotoGame(1,0);
			break;

			case 8:		//USB
				usb_server();
				OS_game=0;					
			break;

			case 9:		//Alarm				
				sleeptime=WAITINGTIME;
				//showAlarm();
				alarmtime=0;
				OS_game=0;					
			break;
/*
//----------------------------------------------------------------
			case 10:		//setting				
				//main_words();
				sleeptime=WAITINGTIME;		
				M0_Setting();
				GotoGame(10,0);
			break;
			case 11:		//video
			case 115:		//video
				Main_m01();	
				GotoGame(11,0);
			break;
			case 12:		//photo
				Main_m02();
				GotoGame(12,0);
			break;
			case 13:			//player
			case 135:
				//main_VideoPlay();
				//ACT_Audio_Encode();
				main_m03();
				GotoGame(13,0);
			break;
			case 14:		//
			case 145:		//
			case 148:		//
				main_m04();
				GotoGame(14,0);
			break;
			case 15:
				main_m05();
				GotoGame(15,0);
			break;
			case 16:		//game
				main_m06();
				GotoGame(16,0);				
			break;
			case 17:		//mail
				Main_M07();
				GotoGame(17,0);
			break;
			case 18:			//daily
				Main_M08();
				GotoGame(18,0);
			break;
			case 19:		//Mimi Card
				Main_M9();
				GotoGame(19,0);
			break;
			case 20:		//Event
				Main_Event();
				GotoGame(20,0);
			break;			

			
			default:
				OS_game=0;
			break;
			//*/
		}
		if(OS_game==100) break;
	}		
	Act_LoadOs();
}
/*
            EXPORT RES_logo_Bg02
            EXPORT RES_logo_Bg03
            EXPORT RES_logo_Bg04
            EXPORT RES_logo_Bg05
            EXPORT RES_logo_Bg06
            EXPORT RES_logo_Bg07
            EXPORT RES_logo_Bg08
            EXPORT RES_logo_Bg09
            EXPORT RES_logo_Bg10

            EXPORT RES_logo_Bg11
            EXPORT RES_logo_Bg12
            EXPORT RES_logo_Bg13
            EXPORT RES_logo_Bg14
            EXPORT RES_logo_Bg15
            EXPORT RES_logo_Bg16
            EXPORT RES_logo_Bg17
            EXPORT RES_logo_Bg18
            EXPORT RES_logo_Bg19
            EXPORT RES_logo_Bg20

            EXPORT RES_logo_Bg21
            EXPORT RES_logo_Bg22
            EXPORT RES_logo_Bg23
            EXPORT RES_logo_Bg24
            EXPORT RES_logo_Bg25
            EXPORT RES_logo_Bg26
            EXPORT RES_logo_Bg27
            EXPORT RES_logo_Bg28

RES_logo_Bg02
        INCBIN  .\watch\pororo\h_s_02.gif
RES_logo_Bg03
        INCBIN  .\watch\pororo\h_s_03.gif
RES_logo_Bg04
        INCBIN  .\watch\pororo\h_s_04.gif
RES_logo_Bg05
        INCBIN  .\watch\pororo\h_s_05.gif
RES_logo_Bg06
        INCBIN  .\watch\pororo\h_s_06.gif
RES_logo_Bg07
        INCBIN  .\watch\pororo\h_s_07.gif
RES_logo_Bg08
        INCBIN  .\watch\pororo\h_s_08.gif
RES_logo_Bg09
        INCBIN  .\watch\pororo\h_s_09.gif
RES_logo_Bg10
        INCBIN  .\watch\pororo\h_s_10.gif
RES_logo_Bg11
        INCBIN  .\watch\pororo\h_s_11.gif
RES_logo_Bg12
        INCBIN  .\watch\pororo\h_s_12.gif
RES_logo_Bg13
        INCBIN  .\watch\pororo\h_s_13.gif
RES_logo_Bg14
        INCBIN  .\watch\pororo\h_s_14.gif
RES_logo_Bg15
        INCBIN  .\watch\pororo\h_s_15.gif
RES_logo_Bg16
        INCBIN  .\watch\pororo\h_s_16.gif
RES_logo_Bg17
        INCBIN  .\watch\pororo\h_s_17.gif
RES_logo_Bg18
        INCBIN  .\watch\pororo\h_s_18.gif
RES_logo_Bg19
        INCBIN  .\watch\pororo\h_s_19.gif
RES_logo_Bg20
        INCBIN  .\watch\pororo\h_s_20.gif
RES_logo_Bg21
        INCBIN  .\watch\pororo\h_s_21.gif
RES_logo_Bg22
        INCBIN  .\watch\pororo\h_s_22.gif
RES_logo_Bg23
        INCBIN  .\watch\pororo\h_s_23.gif
RES_logo_Bg24
        INCBIN  .\watch\pororo\h_s_24.gif
RES_logo_Bg25
        INCBIN  .\watch\pororo\h_s_25.gif
RES_logo_Bg26
        INCBIN  .\watch\pororo\h_s_26.gif
RES_logo_Bg27
        INCBIN  .\watch\pororo\h_s_27.gif
RES_logo_Bg28
        INCBIN  .\watch\pororo\h_s_28.gif
//*/

