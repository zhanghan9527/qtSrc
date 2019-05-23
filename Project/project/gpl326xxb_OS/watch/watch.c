#include "application.h"
#include "data.h"
#include "math.h"
#include "menu_sp.h"
#include "clock_bg.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"

extern const INT16U ALIGN4 watch_SPRITE_Palette1[];



INT16U Day=1,Month=1,Year=2017,Week;
INT16U Time_day;
INT16U Time_hour=8,Time_min=0,Time_sec=0;

extern t_rtc Rtc_time,Rtc_alarm;
extern INT16S OS_game;

INT16S getMonthDate(INT16S month,INT16S year)
{
	if(month==2){
		if(year%4==0) return 29;
		else return 28;
	}else if(month==1||month==3||month==5||month==7||month==8||month==10||month==12){
		return 31;
	}else return 30;
}

//2000.1.1 is week 6
#define YEAY_START	2000
#define WEEK_START	6
#define YEAY_MAX	100
INT16U Date_Start=5000;
void GetDate(INT16U date)
{
	INT16U n,m;
	
	date+=Date_Start;
	Day=1;Month=1;Year=YEAY_START;
	Week=(WEEK_START+date)%7;
	m=date;	

	while(1){
		n=getMonthDate(Month,Year);
		if(m<n){
			Day+=m;
			break;
		}else{
			m-=n;
			Month++;
			if(Month>12){
				Year++;
				Month=1;
			}
		}
	}
}
INT16U GetDays(INT16S date,INT16S month,INT16S year)
{
	INT16U no=0,i;

	for(i=YEAY_START;i<year;i++){
		if(i%4==0) no+=366;
		else no+=365;
	}
	for(i=1;i<month;i++){
		no+=getMonthDate(i,year);
	}
	no+=date-1;
	return no;
}
INT16S GetWeekNo(INT16S date,INT16S month,INT16S year)
{
	INT16U no=0,i;

	no=GetDays(date,month,year);
	return 1+(WEEK_START-1+no)%7;
}
void Get_Date_Start()
{
	Date_Start=0;
	while(Time_day>=1000){
		Date_Start+=1000;
		Time_day-=1000;
	}
}

void Get_Rtc_Reset()
{
	rtc_day_get(&Rtc_time);
	if(Rtc_time.rtc_day<90){
		OS_game=2;
	}
}
void Get_Rtc_Data()
{
	rtc_day_get(&Rtc_time);
	if(Rtc_time.rtc_day<90){
		Date_Start=5000;
		Time_day=GetDays(1,1,2017)-Date_Start;
		Rtc_time.rtc_day=Time_day+90;
		Rtc_time.rtc_sec=0;
		Rtc_time.rtc_min=0;
		Rtc_time.rtc_hour=8;
		rtc_day_set(&Rtc_time);
		rtc_time_set(&Rtc_time);
		rtc_day_get(&Rtc_time);
		Act_SaveSetting();
		OS_game=2;
	}
	Time_day=Rtc_time.rtc_day-90;
	rtc_time_get(&Rtc_time);
	Time_hour=Rtc_time.rtc_hour;
	Time_min=Rtc_time.rtc_min;
	Time_sec=Rtc_time.rtc_sec;
	if(Time_hour>=24) Time_hour=0;
	if(Time_min>=60) Time_min=0;
	if(Time_sec>=60) Time_sec=0;
	
	GetDate(Time_day);

}

void Set_Rtc_Time()
{
	Rtc_time.rtc_sec=0;
	Rtc_time.rtc_min=Time_min;
	Rtc_time.rtc_hour=Time_hour;
	rtc_time_set(&Rtc_time);
}
void Set_Rtc_Day()
{
	Rtc_time.rtc_day=Time_day+90;
	rtc_day_set(&Rtc_time);
}
void Set_Rtc_reset()
{
	Rtc_time.rtc_day=1;
	rtc_day_set(&Rtc_time);
}

//===========================================================
#define Time_X	-8
#define Time_Y	36
#define Time_W	18
#define Date_X	20
#define Date_Y	62
#define Date_W	10

INT8U Hour_24=0,MM_DD=0;
void show_watch_time()
{
	int n;
	
	if(Hour_24==0){
		n=Time_hour%12;
		if(n==0) n=12;
	}else n=Time_hour;
	set_sprite_display_init(10,Time_X,Time_Y,Num_Big_Frame[n/10]);
	set_sprite_display_init(11,Time_X+Time_W*1,Time_Y,Num_Big_Frame[n%10]);	
	if(game_time/12%2) set_sprite_display_init(12,Time_X+Time_W*2-5,Time_Y-4,Num_Big_Frame[10]);
	else set_sprite_display_init(12,Time_X+Time_W*2-5,Time_Y,0);
	set_sprite_display_init(13,Time_X+Time_W*3-10,Time_Y,Num_Big_Frame[Time_min/10]);
	set_sprite_display_init(14,Time_X+Time_W*4-10,Time_Y,Num_Big_Frame[Time_min%10]);

	if(Hour_24==0){
		if(Time_hour>=12) set_sprite_display_init(15,Time_X+Time_W*5-0,Time_Y-11,Num_Small_White_Frame[12]);
		else set_sprite_display_init(15,Time_X+Time_W*5-0,Time_Y-10,Num_Small_White_Frame[11]);
	}
	if(game_time/12%2) set_sprite_display_init(18,Time_X+Time_W*5-12,Time_Y+7,Num_Small_White_Frame[10]);
	else set_sprite_display_init(18,0,0,0);
	set_sprite_display_init(16,Time_X+Time_W*5-12+10,Time_Y+8,Num_Small_White_Frame[Time_sec/10]);
	set_sprite_display_init(17,Time_X+Time_W*5-12+20,Time_Y+8,Num_Small_White_Frame[Time_sec%10]);

	if(MM_DD){
		set_sprite_display_init(20,Date_X,Date_Y,Num_Small_White_Frame[Month/10]);
		set_sprite_display_init(21,Date_X+Date_W,Date_Y,Num_Small_White_Frame[Month%10]);
		set_sprite_display_init(22,Date_X+Date_W*2,Date_Y,Num_Small_White_Frame[13]);
		set_sprite_display_init(23,Date_X+Date_W*3,Date_Y,Num_Small_White_Frame[Day/10]);
		set_sprite_display_init(24,Date_X+Date_W*4,Date_Y,Num_Small_White_Frame[Day%10]);
	}else{
		set_sprite_display_init(20,Date_X,Date_Y,Num_Small_White_Frame[Day/10]);
		set_sprite_display_init(21,Date_X+Date_W,Date_Y,Num_Small_White_Frame[Day%10]);
		set_sprite_display_init(22,Date_X+Date_W*2,Date_Y,Num_Small_White_Frame[13]);
		set_sprite_display_init(23,Date_X+Date_W*3,Date_Y,Num_Small_White_Frame[Month/10]);
		set_sprite_display_init(24,Date_X+Date_W*4,Date_Y,Num_Small_White_Frame[Month%10]);
	}
	set_sprite_display_init(25,Date_X+Date_W*5,Date_Y,Num_Small_White_Frame[13]);
	set_sprite_display_init(26,Date_X+Date_W*6,Date_Y,Num_Small_White_Frame[Year/10%10]);
	set_sprite_display_init(27,Date_X+Date_W*7,Date_Y,Num_Small_White_Frame[Year%10]);
}
INT16S PowerNum;
void show_PowerNum()
{
	set_sprite_display_init(20,Date_X,Date_Y,Num_Small_White_Frame[PowerNum/1000%10]);
	set_sprite_display_init(21,Date_X+Date_W,Date_Y,Num_Small_White_Frame[PowerNum/100%10]);
	set_sprite_display_init(23,Date_X+Date_W*3,Date_Y,Num_Small_White_Frame[PowerNum/10%10]);
	set_sprite_display_init(24,Date_X+Date_W*4,Date_Y,Num_Small_White_Frame[PowerNum%10]);
	sleeptime=WAITINGTIME;
}

INT8S alarm_hour=12,alarm_minute=0,alarm_music=0;
INT8S alarmFlag=0,alarm=0;
extern INT8S Alarm_Sound;
INT16S alarmtime=0;
void main_watch()
{
	int i;
	INT16S m=0;
	
	Desk_flag=1;	

	
	Palette_ram_cpy( 1,0, watch_SPRITE_Palette1+256	,16);
	Palette_ram_cpy( 2,0, watch_SPRITE_Palette1+0	,256);
	spMalloc(500);
	LoadData_1(0,SpriteCellData,"iWatch\\watch\\SpriteCell.bin");
	
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	//gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)_All_Buttons_CellData);//sprite cell data
	clear_sprite(Spritebuf_number);
	Set_Sprite_Group_bank((INT32U*)Clock_BG_Frame,2,28);
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);

	//Fade_in();
	sleeptime=WAITINGTIME;
	game_time=0;
	step=10;
	homeFlag=0;
	while(1){
		PaintTFT_cell4();
	
		if(alarm==1){
			alarm=2;
			alarmtime=60*25;
		}else if(alarmtime){
			sleeptime=WAITINGTIME;
			alarmtime--;
			if(game_time%30==5 && SPU_Get_SingleChannel_Status(12)==0){
				//SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[A_beeps_SFX+Alarm_Sound], 64, 127, 12);
			}
		}
		if(game_time>25 && Goto_desktop_watch()){			
			return;
		}
		if(game_time>100){
			//watchdog_init();
			//R_SYSTEM_WATCHDOG_CTRL = 0x8004;//0.125s reset sys
			//while(1);
		}
		switch(step){
			case 10:
				LoadTextBmp(C_PPU_TEXT1,"iWatch\\spy_menu\\bg.gif");
				step++;
			break;
			case 11:
				Get_Rtc_Data();
				show_watch_time();
				//show_PowerNum();
				set_sprite_display_init(0,0,0,Clock_BG_Frame[game_time/2%28]);
				//m=TestReleaseState();
				if(m==1 || m==4){
//					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
//					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, 12);
					return;
				}
			
			break;
//--------------------------------------------------------select game			
		}
	}

}

extern const INT16U RES_Watch_Page_START[];
INT16S LoadTextBmp_RAM(INT32U RAM_Addr)
{
	image_decode_Sdram_to_buff((INT32U)RAM_Addr,TVOutBuff);
	InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);
}


