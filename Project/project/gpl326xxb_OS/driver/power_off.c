#include "application.h"
#include "Sprite_demo.h"
#include "Text_demo.h"
#include "data.h"

//#include "power_down.h"


//软件开关机


INT8U PowerPress=38,delayPower=13;

extern const unsigned short PowerOff_sfx[];
extern const INT16U _sleep_anima_CellIdx[];
//extern const INT16U _sleep_anima_CellData[];

void set_wake_up(int m)
{
	t_rtc time;
	INT32U tmp,i;

	*(INT8U *)0x7FFFFF=0;
	
	//Close_Sensor();
	if(DISKUSED==FS_NAND1){
	   DrvNand_flush_allblk();
    }
    
	if(alarmFlag==0 && m){
		idp_rtc_alm_pin_set(0);
		return;	
	}
	
//	R_RTC_CTRL &= ~(0x07 << 8);
//	R_RTC_CTRL |= 0x8420;		//enable ALMEN
//	R_RTC_INT_STATUS |= 0x51F;	

	if(m==0){
		rtc_time_get(&time);
			
		time.rtc_sec += 3;
		if(time.rtc_sec >= 60){
			time.rtc_min++;
			time.rtc_sec = 0;
		}
		
		if(time.rtc_min >= 60){
			time.rtc_hour++;
			time.rtc_min = 0;
		}	
		rtc_alarm_set(&time); 
	}else{
		time.rtc_sec = 0;
		time.rtc_min = alarm_minute;
		time.rtc_hour = alarm_hour;

		rtc_alarm_set(&time);
	}
	
//	rtc_int_clear(RTC_ALM_IEN);
//	rtc_int_set(RTC_ALM_IEN, RTC_ALM_IEN);
	idp_rtc_alm_pin_set(0);
	
	idp_rtc_alm_pin_wakeup_set(1);	
	
	idp_rtc_int_clear(GPX_RTC_ALM_IEN);
	idp_rtc_int_set(GPX_RTC_ALM_IEN, GPX_RTC_ALM_IEN); 
}

void ShowOffAnima(void)
{
	char path[50];
	INT16S i,j;

	if(OS_game==11){
		if(Video_Coding()){
			video_encode_stop();
		}
		video_encode_preview_stop();
		video_encode_exit();        // Release the resources needed by Video encoder
	}else if(OS_game==13 || OS_game==14 || OS_game==19){
		video_decode_stop();
		video_decode_exit();
	}
	sensorFree();
	Close_Sensor();
	InitFileSystem(DISKUSED);
	InitState();
	if(Desk_flag){
		Get_Rtc_Data();
		Act_SaveSetting();
		Save_player();
	}
	paint_ppu_spriteram_cell4(ppu_register_set,Sprite_Coordinate_320X240,Left_Top_coordinate_mode,Spritebuf_number);
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OFF_SFX], 64, 80, SFX_CHANL);
	LoadTextBmp(C_PPU_TEXT1,"Pororo\\menu\\ON_OFF\\M_off-1.gif");
	gplib_ppu_go_and_wait_done(ppu_register_set);
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);
	
	Chdir_Root("");
	for(i=1;i<=6;i++){
		sprintf(path,"Pororo\\menu\\ON_OFF\\M_off-%d.gif",i);
		LoadTextBmp(C_PPU_TEXT1,path);
		for(j=0;j<3;j++){
			gplib_ppu_go_and_wait_done(ppu_register_set);
			Paintdelay();
		}
	}//*/

#if DISKUSED==FS_NAND1
	DrvNand_flush_allblk();
#endif
	_deviceunmount(DISKUSED);

	gpio_write_io(T430_RESET_PIN, DATA_LOW);
//	*(int*)IOG_BUFFER_ADDR&=~BIT13;	
	set_wake_up(1);
	while(1);
}

void delay_10(void)
{
	int i;
	for(i=0;i<100000;i++) i=i;
}

INT8S pressoff;
INT8S light=0;
void PowerOn(void)
{
	int i,j;

	for(i=0;i<10;i++) delay_10();

	idp_rtc_alm_pin_set(1);	
	pressoff=0;
	delayPower=13;
	PowerPress=38;
	light=0;
}

INT16S sleeptime=WAITINGTIME;  //two minute shutdown

extern INT16S OS_game,PowerOff_Video;
void PowerOff(void)
{
	int ret;
	if (s_usbd_pin) return;
    
    //*(int*)IOF_BUFFER_ADDR &= ~BIT6;
	if((*(int*)PWR_DETECT_Data & PWR_DETECT)){
		if(pressoff){
		//if(0){
			delayPower=6;
			if(PowerPress){
				PowerPress--;
			}else if(Desk_flag){
				ret=Show_Warning(100,4);
				if(ret==1){
					ShowOffAnima();
				}
			}else{
				ShowOffAnima();
			}
		}

	}else{
		if(delayPower){
			delayPower--;
		}else{
			PowerPress=38;
			pressoff=1;
		}
	}
}

void Sleep(void)
{
	if (s_usbd_pin) return;
    if(DISKUSED==FS_NAND1){
		if(LVLevel==-2){
			//PowerOff_Video=1;
			ShowOffAnima();
		}
	}
	
	if( KeyBoardCode){
		sleeptime=WAITINGTIME;
	}else{
		sleeptime--;		
		if(sleeptime<25*60*4){
			SPU_StopChannel(SPEECHCHANL);
			SPU_StopChannel(MUSIC_CHCHANL);
			gpio_write_io(T430_RESET_PIN, DATA_LOW);
			while(1){
				PaintTFT_Power();
				sleeptime--;
				if(sleeptime<=0){
					ShowOffAnima();
				}
				if(KeyBoardCode || (*(int*)PWR_DETECT_Data & PWR_DETECT) ){
					sleeptime=WAITINGTIME;
					gpio_write_io(T430_RESET_PIN, DATA_HIGH);
					KeyBoardCode=0;
					break;
				}
			}
		}
	}
}
extern INT8S alarmFlag,alarm;
void goto_sleep()
{
/*
	if(alarmFlag){
		PowerOff_Video=0;
		SetWaveIo(0);
		//led_pwm_Disable();
		Get_Rtc_Data();
		gpio_write_io(T430_RESET_PIN, DATA_LOW);		
		//Close_Sensor();
		R_TFT_CTRL = 0;
		
		
		SDRAM_Self_Refresh_Demo();
		

		dac_init();
		adc_init();
		TP_IOInit();	//init touch	
		AD_Init();		//init AD for LV
		KEY_IOInit();	//init key
		//InitCardin();	//init SD

		JMP17009_ST7735S_8bit_init();
		//gpio_write_io(T430_RESET_PIN, DATA_HIGH);
		//led_pwm_init();	
		sleeptime=WAITINGTIME;
	}else ShowOffAnima();	//*/
}
extern INT16S power_pop,old_power;
void ShowPower()
{
	if(old_power) old_power--;
	if(game_time%100==5){
		if(s_usbd_pin){
			LVLevel=3;

		}else GetLV();
	}
#if(DISKUSED==FS_NAND1)
	if(power_pop){
		power_pop=0;
		Show_Warning(100,8);
	} 
#endif	
}
