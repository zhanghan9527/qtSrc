/********************************************************************
* Purpose: M0 Setting
* Author:  David Zhu
* Date:    2017.06.20
* Copyright MCS Co., Ltd.. ALL RIGHTS RESERVED.
* Version : 1.00
* History :
*********************************************************************/

//==============================================================================
// Includes
//==============================================================================
#include "application.h"
#include "Sprite_demo.h"
#include "Text_demo.h"
#include "data.h"

#include "M0_Open.h"
#include "key.h"
#include "photoedit.h"

//==============================================================================
// Macros
//==============================================================================
extern const char* Back_bg[];

//==============================================================================
// Constants
//==============================================================================
//=============程序状态==================

#define OPEN_ANIM		0
#define DIS_MENU_BG		1
#define MENU_SELECT		2

#define SET_USER		10
#define USER_PROFILE	11
#define SETTING_TODAY	12
#define SETTING_TIME	13
#define SETTING_BRIGHT	14
#define SETTING_VOLUME	15

#define SET_HOMEBG		30

#define SET_HOMEICON	50



#define SETTING_JUST	60

//=============位置信息==================

#define YEAR_XPOS		58	
#define DATE_YPOS		134	
#define MONTH_XPOS		158	
#define DATE_XPOS		235	

#define HOUR_XPOS		106	
#define MINUTE_XPOS		182	
#define TIME_YPOS		134	

#define FRAME_XPOS		5	
#define FRAME_YPOS		9	


#define NUM_WIDTH		14	


//====卡通索引
#define SP_START		0

#define YEAR_SP			SP_START+1
#define MONTH_SP		YEAR_SP+4
#define DATE_SP			MONTH_SP+2
#define LASTYEAR_SP		DATE_SP+2
#define LASTMONTH_SP	LASTYEAR_SP+4
#define LASTDATE_SP		LASTMONTH_SP+2
#define NEXTYEAR_SP		LASTDATE_SP+2
#define NEXTMONTH_SP	NEXTYEAR_SP+4
#define NEXTDATE_SP		NEXTMONTH_SP+2

#define HOUR_NUM		NEXTDATE_SP+2
#define MINUTE_NUM		HOUR_NUM+2
#define LASTHOUR_NUM	MINUTE_NUM+2
#define LASTMIN_NUM		LASTHOUR_NUM+2
#define NEXTHOUR_NUM	LASTMIN_NUM+2
#define NEXTMIN_NUM		NEXTHOUR_NUM+2
#define SELFRAME		NEXTMIN_NUM+2

#define VOLUME			SP_START+1
#define BRIGHT_SP		VOLUME+5

#define SEL_FRAME		SP_START+1

#define ICON_SP			SP_START+1
#define HOMEICON_SP		ICON_SP+10
#define CURSORICON_SP	HOMEICON_SP+4
const char* M0Sfx_ResourceLocation[]={
" "
};

const INT16S ProfileInputHeadPos[]={
	128, 80,
	112,128,
	112,164,
	204,164
};

const char *M0MenuBGGIF[]={
"Pororo\\M0\\M0_1_menu.gif",
"Pororo\\M0\\M0_2_menu.gif",
"Pororo\\M0\\M0_3_menu.gif",
};

const char *M0_2BGGIF[]={
"Pororo\\M0\\M0_2-1_bgset.gif",
"Pororo\\M0\\M0_2-2_bgset.gif",
};
const INT16S M0_3HomeIconPos[]={
	6,29,	73,26,	141, 24,201,26,	269,24,
	6,92,	77,95,	137,100,207,90,	260,96,	
};

const int M0_3IconSize[]={
	63,44,	67,49,	58,51,	57,50,	37,49,
	66,51,	54,43,	64,34,	45,50,	56,41,
};

const int M0_3HomeIconSize[]={
	72,51,	77,56,	65,58,	65,57,	42,56,
	75,58,	62,51,	72,38,	52,57,	63,48,
};

const int M0_3SelectingIconSize[]={
	76,59,	81,64,	72,67,	70,64,	55,66,
	78,65,	72,60,	77,49,	63,66,	72,58,
};

const int DateSelFramePos[]={
		45,122,144,122,220,122
};

const int TimeSelFramePos[]={
		91,122,167,122
};


INT16S M0_3HomeIconStatus[10]={0,0,0,0,0,0,0,0,0,0};

INT16S M0toHomeIconIdx[]={1,3,5,7,9,2,4,6,8,0};
INT16S HometoMoIconIdx[]={9,0,5,1,6,2,7,3,8,4};


INT16U DaysEachMonth[]={31,29,31,30,31,30,31,31,30,31,30,31};
//==============================================================================
// Global Variables
//==============================================================================
INT16S ProfileMenuIdx;
INT16S AgeNumSpIdx,MonthNumSpIdx,DayNumSpIdx;		
INT16S AgeNum[2],MonthNum[2],DayNum[2];
INT16S AgeTemp,MonthTemp,DayTemp;



INT16S	Time_Hour,Time_Minute,Time_Second,Time_24h;
INT8S 	lockFlag,indoor_light_Freq;
INT16S 	Date_Day,Date_Month,Date_Year;
INT16S 	Last_Day,Last_Month,Last_Year;
INT16S 	Next_Day,Next_Month,Next_Year;
INT16S 	Last_Hour,Last_Min;
INT16S 	Next_Hour,Next_Min;

INT16S 	M0HomeScreenSelIdx;


INT16S 	HomeIconNum;

INT16S 	M0HomeIconIdx[4];


INT16U  M0PlayerOriName[4];	
INT16S  M0PlayerAge;		
INT16U  M0PlayerBirthMonth;			
INT16U  M0PlayerBirthDay;

INT8S 	M0LCD_light;
INT8S 	M0volum;
INT16S 	auto_screen_off;
INT32S 	save_date;
//==============================================================================
// External Declarations
//==============================================================================
extern INT8S alarmFlag,alarm;
extern INT8S alarm_music;
extern INT16U Day,Month,Year;
extern INT16U 	TouchAreaFlag;
extern const 	INT16U ALIGN4 M0_SPRITE_Palette1[];

extern int BoardIndex,BoardIndex0;
//=======================输入法=====================

extern INT32U Word_Kb_Num;
extern INT16S Keyboard;
extern INT16S writeing_max,writeing_Words[];
extern INT16S Words_max,Words_pos;
extern INT8U ABC[6],ABC_idx;
extern INT16S abc;

extern const INT16U ALIGN4 words_SPRITE_Palette1[];

//==============================================================================
// Forward Declarations
//==============================================================================
extern void Act_LoadSetting();
extern void GetDate(INT16U date);
extern void Get_Rtc_Data();
//==============================================================================
// Routines
//==============================================================================
INT16S show_M0pointline32(INT16S sp,INT16S x,INT16S y)
{
	INT16S m=0;
	if(abc){
		show_word32(sp,x,y,abc);
		sp++;
		m++;
		x+=32;
	}
	if(game_time/10%2){
//		show_word32(sp,x,y+3,'_');
		set_sprite_display_init(sp,x,y+4,M0Cursor_Frame[0]);		
		
	}else set_sprite_image_number(sp,0);

	return m;
}



#define Writing32_Line	6
void show_M0writing32(INT16S sp,INT16S num, INT16S LetterXPos, INT16S LetterYPos)
{
	INT16S i,x,y,m,n,k;

	x=LetterXPos;
	y=LetterYPos;
	ClearSomeSp(sp,sp+num+2);
	m=writeing_max;

	for(i=0;i<=m;i++){
		if(Words_pos==i){
			k=show_M0pointline32(sp,x,y);
			
			sp+=k+1;
			x+=32*k;
			if(x>=LetterXPos+32*Writing32_Line){
				x=LetterXPos;
				y+=32;
			}
		}
		if(i==m) break;
		n=writeing_Words[i];
		show_word32(sp,x,y,n);
		x+=32;
		if(x>=LetterXPos+32*Writing32_Line){
			x=LetterXPos;
			y+=32;
		}
		sp++;
	}
}


void initM0KoreaInput(void)
{
	int i;
	writeing_max=0;			//韩文输入法	
	for(i=0;i<Words_max;i++) writeing_Words[i]=0;  //韩文输入法	
	InitABC();
	show_M0writing32(30,Words_max,0,0);	
	Words_pos=0,Keyboard=1;		
} 


void Wait_sound_end()
{
	INT16S i=16;
	while(i)
	{
		i--;
		while(SPU_Get_SingleChannel_Status(i))
		{
			gplib_ppu_go_and_wait_done(ppu_register_set);
		}			
	}	
}
/*
void M0_show_Date(INT16S sp,INT32U *frame)
{
	set_sprite_display_init(sp+0,YEAR_XPOS+NUM_WIDTH*0,DATE_YPOS,frame[Date_Year/1000]);
	set_sprite_display_init(sp+1,YEAR_XPOS+NUM_WIDTH*1,DATE_YPOS,frame[Date_Year%1000/100]);
    set_sprite_display_init(sp+2,YEAR_XPOS+NUM_WIDTH*2,DATE_YPOS,frame[Date_Year%100/10]);
	set_sprite_display_init(sp+3,YEAR_XPOS+NUM_WIDTH*3,DATE_YPOS,frame[Date_Year%10]);
	
	set_sprite_display_init(sp+4,MONTH_XPOS+NUM_WIDTH*0,DATE_YPOS,frame[Date_Month/10]);
	set_sprite_display_init(sp+5,MONTH_XPOS+NUM_WIDTH*1,DATE_YPOS,frame[Date_Month%10]);
	
    set_sprite_display_init(sp+6,DATE_XPOS+NUM_WIDTH*0,DATE_YPOS,frame[Date_Day/10]);
	set_sprite_display_init(sp+7,DATE_XPOS+NUM_WIDTH*1,DATE_YPOS,frame[Date_Day%10]);	
}
*/
void initM0DateSprite(void)
{
	int tmp_max;
	Last_Year=Date_Year-1;
	if(Last_Year<2016){
		Last_Year=2016;
	}
	Last_Month=Date_Month-1;
	if(Last_Month==0){
		Last_Month+=12;
	}			
	tmp_max=getMonthDate(Date_Month,Date_Year);
	Last_Day=Date_Day-1;
	if(Last_Day==0){
		Last_Day+=tmp_max;
	}
	
	Next_Year=Date_Year+1;
	if(Next_Year>2028){
		Next_Year=2028;
	}	
	Next_Month=Date_Month+1;
	if(Next_Month>12){
		Next_Month-=12;
	}			
	tmp_max=getMonthDate(Date_Month,Date_Year);
	Next_Day=Date_Day+1;
	if(Next_Day>tmp_max){
		Next_Day-=tmp_max;
	}	
	
	set_sprite_display_init(LASTYEAR_SP+0,YEAR_XPOS+NUM_WIDTH*0,DATE_YPOS+39,M0Num_Frame[2]);
	set_sprite_display_init(LASTYEAR_SP+1,YEAR_XPOS+NUM_WIDTH*1,DATE_YPOS+39,M0Num_Frame[0]);	
	set_sprite_display_init(LASTYEAR_SP+2,YEAR_XPOS+NUM_WIDTH*2,DATE_YPOS+39,M0Num_Frame[Last_Year/10%10]);	
	set_sprite_display_init(LASTYEAR_SP+3,YEAR_XPOS+NUM_WIDTH*3,DATE_YPOS+39,M0Num_Frame[Last_Year%10]);		

	set_sprite_display_init(LASTMONTH_SP+0,MONTH_XPOS+NUM_WIDTH*0,DATE_YPOS+39,M0Num_Frame[Last_Month/10]);
	set_sprite_display_init(LASTMONTH_SP+1,MONTH_XPOS+NUM_WIDTH*1,DATE_YPOS+39,M0Num_Frame[Last_Month%10]);

	set_sprite_display_init(LASTDATE_SP+0,DATE_XPOS+NUM_WIDTH*0,DATE_YPOS+39,M0Num_Frame[Last_Day/10]);
	set_sprite_display_init(LASTDATE_SP+1,DATE_XPOS+NUM_WIDTH*1,DATE_YPOS+39,M0Num_Frame[Last_Day%10]);


	set_sprite_display_init(YEAR_SP+0,YEAR_XPOS+NUM_WIDTH*0,DATE_YPOS,M0Num_Frame[2]);
	set_sprite_display_init(YEAR_SP+1,YEAR_XPOS+NUM_WIDTH*1,DATE_YPOS,M0Num_Frame[0]);	
	set_sprite_display_init(YEAR_SP+2,YEAR_XPOS+NUM_WIDTH*2,DATE_YPOS,M0Num_Frame[Date_Year/10%10]);	
	set_sprite_display_init(YEAR_SP+3,YEAR_XPOS+NUM_WIDTH*3,DATE_YPOS,M0Num_Frame[Date_Year%10]);		

	set_sprite_display_init(MONTH_SP+0,MONTH_XPOS+NUM_WIDTH*0,DATE_YPOS,M0Num_Frame[Date_Month/10]);
	set_sprite_display_init(MONTH_SP+1,MONTH_XPOS+NUM_WIDTH*1,DATE_YPOS,M0Num_Frame[Date_Month%10]);

	set_sprite_display_init(DATE_SP+0,DATE_XPOS+NUM_WIDTH*0,DATE_YPOS,M0Num_Frame[Date_Day/10]);
	set_sprite_display_init(DATE_SP+1,DATE_XPOS+NUM_WIDTH*1,DATE_YPOS,M0Num_Frame[Date_Day%10]);

	set_sprite_display_init(NEXTYEAR_SP+0,YEAR_XPOS+NUM_WIDTH*0,DATE_YPOS-39,M0Num_Frame[2]);
	set_sprite_display_init(NEXTYEAR_SP+1,YEAR_XPOS+NUM_WIDTH*1,DATE_YPOS-39,M0Num_Frame[0]);	
	set_sprite_display_init(NEXTYEAR_SP+2,YEAR_XPOS+NUM_WIDTH*2,DATE_YPOS-39,M0Num_Frame[Next_Year/10%10]);	
	set_sprite_display_init(NEXTYEAR_SP+3,YEAR_XPOS+NUM_WIDTH*3,DATE_YPOS-39,M0Num_Frame[Next_Year%10]);		

	set_sprite_display_init(NEXTMONTH_SP+0,MONTH_XPOS+NUM_WIDTH*0,DATE_YPOS-39,M0Num_Frame[Next_Month/10]);
	set_sprite_display_init(NEXTMONTH_SP+1,MONTH_XPOS+NUM_WIDTH*1,DATE_YPOS-39,M0Num_Frame[Next_Month%10]);

	set_sprite_display_init(NEXTDATE_SP+0,DATE_XPOS+NUM_WIDTH*0,DATE_YPOS-39,M0Num_Frame[Next_Day/10]);
	set_sprite_display_init(NEXTDATE_SP+1,DATE_XPOS+NUM_WIDTH*1,DATE_YPOS-39,M0Num_Frame[Next_Day%10]);	
	
}

INT16S M0_set_Date(void)
{
	INT16S whichBar;
	int tmp_max,flag;
	
	LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_1-2_day.gif");
	
	Get_Rtc_Data();	
	
	Date_Day=Day;
	Date_Month=Month;
	Date_Year=Year;
	whichBar=0;
	set_sprite_display_init(SELFRAME, DateSelFramePos[2*whichBar],DateSelFramePos[2*whichBar+1],M0SelectFrame_Frame[0]);								
	initM0DateSprite();
	while(1)
	{
		PaintTFT_cell4();
		flag=0;
		if(KeyBoardCode==K_Up){
			if(whichBar==0){
				Date_Year++;
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
				if(Date_Year>2027){
					Date_Year=2027;
				}
				flag=1;
			}else if(whichBar==1){
				Date_Month++;
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);				
				if(Date_Month>12){
					Date_Month-=12;
				}
				flag=1;
			}else if(whichBar==2){
				tmp_max=getMonthDate(Date_Month,Date_Year);
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);				
				Date_Day++;
				if(Date_Day>tmp_max){
					Date_Day-=tmp_max;
				}
			}	
		}else if(KeyBoardCode==K_Down){	
				
			if(whichBar==0){
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);							
				Date_Year--;
				if(Date_Year<2017){
					Date_Year=2017;
				}
				flag=1;
			}else if(whichBar==1){
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);			
				Date_Month--;
				if(Date_Month==0){
					Date_Month+=12;
				}
				flag=1;
			}else if(whichBar==2){
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);			
				tmp_max=getMonthDate(Date_Month,Date_Year);
				Date_Day--;
				if(Date_Day==0){
					Date_Day+=tmp_max;
				}
			}
		}else if(KeyBoardCode==K_Left){
			if(whichBar>0) {
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);							
				whichBar--;
			}
			if(whichBar==0)
				set_sprite_display_init(SELFRAME, DateSelFramePos[2*whichBar],DateSelFramePos[2*whichBar+1],M0SelectFrame_Frame[0]);						
			else 	
				set_sprite_display_init(SELFRAME, DateSelFramePos[2*whichBar],DateSelFramePos[2*whichBar+1],M0SelectFrame_Frame[1]);
		}else if(KeyBoardCode==K_Right){
			if(whichBar<2) {
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);							
				whichBar++;
			}	
			if(whichBar==0)
				set_sprite_display_init(SELFRAME, DateSelFramePos[2*whichBar],DateSelFramePos[2*whichBar+1],M0SelectFrame_Frame[0]);						
			else 	
				set_sprite_display_init(SELFRAME, DateSelFramePos[2*whichBar],DateSelFramePos[2*whichBar+1],M0SelectFrame_Frame[1]);
		}else if(KeyBoardCode==K_Ok){
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, 12);
/*				
				Day=Date_Day;
				Month=Date_Month;
				Year=Date_Year;
				
				Time_day=GetDays(Day,Month,Year);
				Get_Date_Start();
				Set_Rtc_Day();
				Wait_sound_end();
				Act_SaveSetting();			
*/				
				return 0;				
		}
		if(flag){
			tmp_max=getMonthDate(Date_Month,Date_Year);
			if(Date_Day>tmp_max){
				Date_Day=tmp_max;
			}			
		}
		initM0DateSprite();		
	
		if(Goto_desktop()){
			return -1;
		}
		
		if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);						
			return -2;
		}				
	}			
} 

void initM0TimeSprite(void)
{
	Last_Hour=Time_Hour-1;
	if(Last_Hour<0){
		Last_Hour+=24;
	}
	Last_Min=Time_Minute-1;
	if(Last_Min<0){
		Last_Min+=60;
	}
	
	Next_Hour=Time_Hour+1;
	if(Next_Hour>=24){
		Next_Hour-=24;
	}
	Next_Min=Time_Minute+1;
	if(Next_Min>=60){
		Next_Min-=60;
	}		
	set_sprite_display_init(HOUR_NUM+0, HOUR_XPOS+NUM_WIDTH*0,TIME_YPOS,M0Num_Frame[Time_Hour/10]);		
	set_sprite_display_init(HOUR_NUM+1, HOUR_XPOS+NUM_WIDTH*1,TIME_YPOS,M0Num_Frame[Time_Hour%10]);				
	
	set_sprite_display_init(MINUTE_NUM+0, MINUTE_XPOS+NUM_WIDTH*0,TIME_YPOS,M0Num_Frame[Time_Minute/10]);		
	set_sprite_display_init(MINUTE_NUM+1, MINUTE_XPOS+NUM_WIDTH*1,TIME_YPOS,M0Num_Frame[Time_Minute%10]);	
	
	set_sprite_display_init(NEXTHOUR_NUM+0, HOUR_XPOS+NUM_WIDTH*0,TIME_YPOS-39,M0Num_Frame[Next_Hour/10]);		
	set_sprite_display_init(NEXTHOUR_NUM+1, HOUR_XPOS+NUM_WIDTH*1,TIME_YPOS-39,M0Num_Frame[Next_Hour%10]);				
	
	set_sprite_display_init(NEXTMIN_NUM+0, MINUTE_XPOS+NUM_WIDTH*0,TIME_YPOS-39,M0Num_Frame[Next_Min/10]);		
	set_sprite_display_init(NEXTMIN_NUM+1, MINUTE_XPOS+NUM_WIDTH*1,TIME_YPOS-39,M0Num_Frame[Next_Min%10]);
	
	set_sprite_display_init(LASTHOUR_NUM+0, HOUR_XPOS+NUM_WIDTH*0,TIME_YPOS+39,M0Num_Frame[Last_Hour/10]);		
	set_sprite_display_init(LASTHOUR_NUM+1, HOUR_XPOS+NUM_WIDTH*1,TIME_YPOS+39,M0Num_Frame[Last_Hour%10]);				
	
	set_sprite_display_init(LASTMIN_NUM+0, MINUTE_XPOS+NUM_WIDTH*0,TIME_YPOS+39,M0Num_Frame[Last_Min/10]);		
	set_sprite_display_init(LASTMIN_NUM+1, MINUTE_XPOS+NUM_WIDTH*1,TIME_YPOS+39,M0Num_Frame[Last_Min%10]);	
}
/*
void M0_show_time(INT16S sp,INT16S hour,INT16S minute,INT32U *frame)
{
	INT8U m;

	if(Hour_24==0){
		if(hour>=12){
			m=1;				
		}else m=0;
		
		set_sprite_display_init(sp,27,167,frame[12+m]);  //am. pm	
		hour%=12;
		if(hour==0)hour=12;
	}else{
		set_sprite_spcell(sp,0);  							//清除AM  PM  标志
	}	
	set_sprite_display_init(sp+2,HOUR_XPOS+NUM_WIDTH*0,TIME_YPOS,frame[hour/10]);
	set_sprite_display_init(sp+3,HOUR_XPOS+NUM_WIDTH*1,TIME_YPOS,frame[hour%10]);

	set_sprite_display_init(sp+5,MINUTE_XPOS+NUM_WIDTH*0,TIME_YPOS,frame[minute/10]);
	set_sprite_display_init(sp+6,MINUTE_XPOS+NUM_WIDTH*0,TIME_YPOS,frame[minute%10]);
}
*/
INT16S M0_set_time(void)
{
	INT16S ret;
	INT16S whichBar;	

	LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_1-3_time.gif");
	
	Get_Rtc_Data();	
	
	Time_Hour=Time_hour;
	Time_Minute=Time_min;
	Time_Second=Time_sec;	  	
	
	initM0TimeSprite();
	whichBar=0;
	set_sprite_display_init(SELFRAME, TimeSelFramePos[2*whichBar],TimeSelFramePos[2*whichBar+1],M0SelectFrame_Frame[1]);									
	while(1)
	{
		PaintTFT_cell4();

		if(KeyBoardCode==K_Up){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);						
 			if(whichBar==0){
				Time_Hour++;
				if(Time_Hour>=24){
					Time_Hour-=24;
				}

			}else if(whichBar==1){
				Time_Minute++;
				if(Time_Minute>=60){
					Time_Minute-=60;
				}			
			}	
		}else if(KeyBoardCode==K_Down){	
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);						
			if(whichBar==0){
				Time_Hour--;
				if(Time_Hour<0){
					Time_Hour+=24;
				}
			}else if(whichBar==1){
				Time_Minute--;
				if(Time_Minute<0){
					Time_Minute+=60;
				}
			}
		}else if(KeyBoardCode==K_Left){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);						
			if(whichBar>0) {
				whichBar--;
			}
			set_sprite_display_init(SELFRAME, TimeSelFramePos[2*whichBar],TimeSelFramePos[2*whichBar+1],M0SelectFrame_Frame[1]);
		}else if(KeyBoardCode==K_Right){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);						
			if(whichBar<1) {
				whichBar++;
			}		
			set_sprite_display_init(SELFRAME, TimeSelFramePos[2*whichBar],TimeSelFramePos[2*whichBar+1],M0SelectFrame_Frame[1]);
		}else if(KeyBoardCode==K_Ok){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, 12);

/*								
			Day=Date_Day;
			Month=Date_Month;
			Year=Date_Year;
			
			Time_hour=Time_Hour;
			Time_min=Time_Minute;
			Time_sec=Time_Second;

			Set_Rtc_Time();			
*/			
			return 0;				
		}
//		M0_show_time(AMPM,Time_Hour,Time_Minute,(INT32U*)M0Num_Frame);		
		initM0TimeSprite();		
		
		if(Goto_desktop()){
			return -1;
		}
		
		if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);						
			return -2;
		}		
			
	}			
} 
void checkDaysOver(void)
{		
	int i;					
	if(DayTemp>31 || DayTemp<1||DayTemp>DaysEachMonth[M0PlayerBirthMonth-1] ) {
		DayNumSpIdx=0;
		for (i=0;i<2;i++) DayNum[i]=0;
		ClearSomeSp(44,45);
	}							
}

void restoreProfileDis(void)
{		
	int i;			
	LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_1-1_profile.gif");												   	    		
	show_M0writing32(30,Words_max,ProfileInputHeadPos[2*0],ProfileInputHeadPos[2*0+1]);
	set_sprite_image_number(30+writeing_max,0);							
	if(AgeNumSpIdx){
		for (i=0;i<AgeNumSpIdx;i++){
			show_word32(40+i,ProfileInputHeadPos[2*1]+i*23,
			ProfileInputHeadPos[2*1+1],AgeNum[i]+0x30);					
		}	
	}	
	if(MonthNumSpIdx){
		for (i=0;i<MonthNumSpIdx;i++){
			show_word32(42+i,ProfileInputHeadPos[2*2]+i*23,
			ProfileInputHeadPos[2*2+1],MonthNum[i]+0x30);					
		}	
	}					
	if(DayNumSpIdx){
		for (i=0;i<DayNumSpIdx;i++){
			show_word32(44+i,ProfileInputHeadPos[2*3]+i*23,
			ProfileInputHeadPos[2*3+1],DayNum[i]+0x30);					
		}	
	}											
}

void M0_show_vol(INT16S sp,INT16S vol)
{
	INT16S i;
	int tmp;
	if(vol == 0) tmp =0;
	else if(vol == 1) tmp =1;
	else if(vol == 2) tmp =3; 
	else if(vol == 3) tmp =5;
	
	
	for(i=0;i<5;i++){
		if(i<tmp){
			set_sprite_display_init(sp+i,72+36*i,98+8,M0Gage_Frame[0]);
		}else  set_sprite_spcell(sp+i,0);
	}
}


INT16S M0_set_Volume(void)
{
	INT16S vol;

	LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_1-4_sound.gif");
    vol= M0volum;
    
    M0_show_vol(VOLUME,vol);
	while(1)
	{
		PaintTFT_cell4();
				
		if(KeyBoardCode==K_Left){
			if(vol)vol--;
			SPU_Set_MainVolume(VOLUME_MAX*vol/VOL_MAX_FAME);
			M0_show_vol(VOLUME,vol);
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, VOLUME_MAX*vol/VOL_MAX_FAME, 12);												
		}else if(KeyBoardCode==K_Right){
			if(vol<3)vol++;
			SPU_Set_MainVolume(VOLUME_MAX*vol/VOL_MAX_FAME);
			M0_show_vol(VOLUME,vol);
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, VOLUME_MAX*vol/VOL_MAX_FAME, 12);												
		}else if(KeyBoardCode==K_Ok){
			SPU_Set_MainVolume(VOLUME_MAX*vol/VOL_MAX_FAME);
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, 12);
//					PlaySound
			if(vol!=volum){
				M0volum=vol;
				Wait_sound_end();
//				Act_SaveSetting();
				return 0;
			}else{
				return 0;					
			}				
		}		
		
		if(Goto_desktop()){
			SPU_Set_MainVolume(VOLUME_MAX*volum/VOL_MAX_FAME);
			return -1;
		}
		if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);							
			SPU_Set_MainVolume(VOLUME_MAX*volum/VOL_MAX_FAME);
			return -2;
		}
				
	}				
} 


void M0_show_Brightness(INT16S sp,INT16S bright)
{
	INT16S i;
	INT16S tmp;
	if(bright == 0) 	 tmp =1;
	else if(bright == 1) tmp =3;
	else if(bright == 2) tmp =5; 
	for(i=0;i<5;i++){
		if(i<tmp){
			set_sprite_display_init(sp+i,72+36*i,98+8,M0Gage_Frame[0]);
		}else  set_sprite_spcell(sp+i,0);
	}	
	
}


INT16S M0_set_Brightness()
{
	INT16S i,m;
	INT16S ret,light;


	LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_1-5_lighting.gif");    
	ret=0;
	m=0;
    step=0;
    light= M0LCD_light;
    
    M0_show_Brightness(BRIGHT_SP,light);
	while(1)
	{
		PaintTFT_cell4();

		if(KeyBoardCode==K_Left){
			if(light>0)light--;
			set_led_light(light);
			M0_show_Brightness(BRIGHT_SP,light);
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);							
		}else if(KeyBoardCode==K_Right){
			if(light<2)light++;
			set_led_light(light);
			M0_show_Brightness(BRIGHT_SP,light);
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);							
		}else if(KeyBoardCode==K_Ok){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[BLING_SFX], 64, 127, SFX_CHANL);							
			if(light!=M0LCD_light){
				M0LCD_light=light;
				Wait_sound_end();
//				Act_SaveSetting();
				return 0;						
			}else {
				return 0;
			}				
		}		

		
		if(Goto_desktop()){
		    light= M0LCD_light;
			set_led_light(light);		    		
			return -1;
		}
		if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
		    light= M0LCD_light;
			set_led_light(light);		    		
			return -2;
		}
	}	
}

void M0_Setting(INT16S m)
{
	INT16S ret;	
	INT16S SettingMenuIdx;

	INT16S BGSetMenuIdx;	
	INT16S M0AnimTime;
	INT16S M0IconSelIdx;
	INT16S i,j; 
	INT16S CursorStatus;
	INT16S page=0,pageno=0;


 	spMalloc(600+900+700);	
	INIT_VideoPub(600);
	Word_Kb_Num=1500; 		
	LoadData_1(1,SpriteCellData,"Pororo\\M0\\SpriteCell_cmp.bin");
	LoadData_1(1,SpriteCellData+Word_Kb_Num*_1K,"Pororo\\words\\SpriteCell_cmp.bin");	
//	LoadDRM(0,6,SFX_ADDR_GAME,(int*)&DrmAddr[0],M0Sfx_ResourceLocation);		
	Palette_ram_cpy(1,0, M0_SPRITE_Palette1+0,256);
	Palette_ram_cpy(3,0, M0_SPRITE_Palette1+256,256);
	Palette_ram_cpy( 2,240, words_SPRITE_Palette1	,16);		//韩文输入法			
 	
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
  	gplib_ppu_sprite_zoom_enable_set(ppu_register_set, 1);		// value:0=disable 1=enable
  	
 	clear_sprite(Spritebuf_number);	
//	LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\bg\\M0_1_01_1.gif"); 	   	    	 	
 	  	
	set_sprite_display_init(0,0,0,M0_Open_Frame[0]);
	SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[MENU_OPEN_SFX], 64, 127, SFX_CHANL);													   	    	   	      	
  	gplib_ppu_fade_effect_set(ppu_register_set, 0x0); 	

	Words_max=4;
	Keyboard=1;	  	
  	initM0KoreaInput();
 	
	Game_State=0;M0AnimTime=0;
	SettingMenuIdx=0;
		
    while(1)
    {    	
		PaintTFT_cell4();
		
		if(Goto_desktop()){
			Game_State=0xFF;
		}		
		
		    
   	    switch(Game_State)
   	    {		
   	    	case OPEN_ANIM:		//Open Anim
				M0AnimTime++;
				set_sprite_display_init(0,0,0,M0_Open_Frame[M0AnimTime/3]);
				if(M0AnimTime>=3*6){
					LoadTextBmp(C_PPU_TEXT1,M0MenuBGGIF[SettingMenuIdx]);		 								
					ClearSomeSp(0,0);
					Game_State++;
				}
   	    		
   	    	break;				     	    	
   	    	case DIS_MENU_BG:		//设置主菜单背景
				LoadTextBmp(C_PPU_TEXT1,M0MenuBGGIF[SettingMenuIdx]);		 								   	    	

   	    		Game_State++;
   	    	break;				  
   	    	case MENU_SELECT:		//主菜单操作
				if(KeyBoardCode==K_Down){
					if(SettingMenuIdx<2){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						SettingMenuIdx++;
						LoadTextBmp(C_PPU_TEXT1,M0MenuBGGIF[SettingMenuIdx]);		 								   	    							
					}
				}else if(KeyBoardCode==K_Up){
					if(SettingMenuIdx>0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						SettingMenuIdx--;
						LoadTextBmp(C_PPU_TEXT1,M0MenuBGGIF[SettingMenuIdx]);		 								   	    							
					}				

				}else if(KeyBoardCode==K_Ok){		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);													
					Game_State=10+SettingMenuIdx*20;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					Game_State=0xff;					
					OS_game=0;				
				}
   	    	break;
   	    	case 3:
 	    		
   	    	break;
   	    	case 4:   	    		
   	    	break;
   	    	case 5:

   	    	break; 
   	    	
   	    	
			case SET_USER:
				LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_1-1_profile.gif");							
				
//				Act_LoadSetting();	
/*
		nRet = read(fd,(INT32U)&Date_Start,2);
		nRet = read(fd,(INT32U)&volum,1);
		nRet = read(fd,(INT32U)&LCD_light,1);
		nRet = read(fd,(INT32U)&alarmFlag,1);
		nRet = read(fd,(INT32U)&alarm_hour,1);
		nRet = read(fd,(INT32U)&alarm_minute,1);
		nRet = read(fd,(INT32U)&player.HomeIconIdx,8);
		nRet = read(fd,(INT32U)&player.HomeScreenBGIdx,2);		
		nRet = read(fd,(INT32U)&InBoxMail,5);		
		nRet = read(fd,(INT32U)&player.InBox_Day,1);

		nRet = read(fd,(INT32U)&player.PlayerOriName,8);		
		nRet = read(fd,(INT32U)&player.PlayerAge,2);				
		nRet = read(fd,(INT32U)&player.PlayerBirthMonth,2);				
		nRet = read(fd,(INT32U)&player.PlayerBirthDay,2);			
*/				
				initM0KoreaInput();
				
				for (i=0;i<4;i++) {
					writeing_Words[i]=M0PlayerOriName[i]=player.PlayerOriName[i];
					if(writeing_Words[i]!=0) writeing_max++;
				}	
				Words_pos=writeing_max;
				
				M0PlayerAge=player.PlayerAge;
				M0PlayerBirthMonth=player.PlayerBirthMonth;				
				M0PlayerBirthDay=player.PlayerBirthDay;
				M0LCD_light = LCD_light;
				M0volum = volum;
				
				
				if(M0PlayerAge>=0 && M0PlayerAge <100){
					AgeTemp=M0PlayerAge;
					if(AgeTemp>=10) {
						AgeNumSpIdx =2;
						AgeNum[0]=AgeTemp/10; AgeNum[1]=AgeTemp%10;						
					}else {
						AgeNumSpIdx =1;
						AgeNum[0]=AgeTemp%10; AgeNum[1]=0;
					}	
				}else{
					AgeNumSpIdx=0;
					AgeTemp = -1;
					AgeNum[0]=-1,AgeNum[1]=0;
				}	
				
				if(M0PlayerBirthMonth>0 && M0PlayerBirthMonth <13){
					MonthTemp=M0PlayerBirthMonth;
					if(MonthTemp>=10){
					 	MonthNumSpIdx =2;
						MonthNum[0]=MonthTemp/10; MonthNum[1]=MonthTemp%10;					 
					}else{
						MonthNumSpIdx =1;
						MonthNum[0]=MonthTemp%10; MonthNum[1]=0;
					}	
				}else{
					MonthNumSpIdx=0;
					MonthTemp = 0;
					for (i=0;i<2;i++){
						MonthNum[i]=0;					
					}				
				}

				if(M0PlayerBirthDay>0 && M0PlayerBirthDay <32){
					DayTemp=M0PlayerBirthDay;
					if(DayTemp>=10){
						DayNumSpIdx =2;
						DayNum[0]=DayTemp/10; DayNum[1]=DayTemp%10;						
					}else{
						DayNumSpIdx =1;
						DayNum[0]=DayTemp%10; DayNum[1]=0;												
					}	
				}else{
					DayNumSpIdx=0;
					DayTemp =0;	
					for (i=0;i<2;i++){

						DayNum[i]=0;
					}				
				}				
															
				ProfileMenuIdx =0;	
				restoreProfileDis();			

				BoardIndex=BoardIndex0;		//消除主菜单按键
				Game_State++;
			break;
				
			case USER_PROFILE:	
				if(ProfileMenuIdx ==0){
					show_M0writing32(30,Words_max,ProfileInputHeadPos[2*ProfileMenuIdx],ProfileInputHeadPos[2*ProfileMenuIdx+1]);
					m=Test_Keyboard();
					if(writeing_max==Words_max){
						InitABC();					
					}
				}else{
					if(game_time/10%2){
						if(ProfileMenuIdx == 1)
							show_M0pointline32(50,
								ProfileInputHeadPos[2*ProfileMenuIdx]+AgeNumSpIdx*23,ProfileInputHeadPos[2*ProfileMenuIdx+1]);
						else if(ProfileMenuIdx == 2)						
							show_M0pointline32(50,
								ProfileInputHeadPos[2*ProfileMenuIdx]+MonthNumSpIdx*23,ProfileInputHeadPos[2*ProfileMenuIdx+1]);							
						else if(ProfileMenuIdx == 3)											
							show_M0pointline32(50,
								ProfileInputHeadPos[2*ProfileMenuIdx]+DayNumSpIdx*23,ProfileInputHeadPos[2*ProfileMenuIdx+1]);
					}else set_sprite_image_number(50,0);

					m=GetKeyBoardBuff();				

					if(m>=0x30 && m<=0x39 )
					{
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);					
						if(ProfileMenuIdx == 1 && AgeNumSpIdx<2){
							show_word32(40+AgeNumSpIdx,ProfileInputHeadPos[2*ProfileMenuIdx]+AgeNumSpIdx*23,
							ProfileInputHeadPos[2*ProfileMenuIdx+1],m);					
							
							AgeNum[AgeNumSpIdx]=m-0x30;
							AgeNumSpIdx++;
							
							     if(AgeNumSpIdx ==0 ) AgeTemp=-1;
							else if(AgeNumSpIdx ==1 ) AgeTemp=AgeNum[0];
							else if(AgeNumSpIdx ==2 ) AgeTemp=AgeNum[0]*10+AgeNum[1];							
							
						}else if(ProfileMenuIdx == 2 && MonthNumSpIdx<2){
							show_word32(42+MonthNumSpIdx,ProfileInputHeadPos[2*ProfileMenuIdx]+MonthNumSpIdx*23,
							ProfileInputHeadPos[2*ProfileMenuIdx+1],m);					
							
							MonthNum[MonthNumSpIdx]=m-0x30;
							MonthNumSpIdx++;

							if(MonthNumSpIdx ==0 ) MonthTemp=0;
							else if(MonthNumSpIdx ==1 ) MonthTemp=MonthNum[0];
							else if(MonthNumSpIdx ==2 ) MonthTemp=MonthNum[0]*10+MonthNum[1];							
							
							if(MonthTemp>12 || MonthTemp<1) {
								MonthNumSpIdx=0;
								for (i=0;i<2;i++) MonthNum[i]=0;
								ClearSomeSp(42,43);
							}
													
						}else if(ProfileMenuIdx == 3 && DayNumSpIdx<2){						
							show_word32(44+DayNumSpIdx,ProfileInputHeadPos[2*ProfileMenuIdx]+DayNumSpIdx*23,
							ProfileInputHeadPos[2*ProfileMenuIdx+1],m);					
							
							DayNum[DayNumSpIdx]=m-0x30;
							DayNumSpIdx++;												
							
							if(DayNumSpIdx ==0 ) DayTemp=0;
							else if(DayNumSpIdx ==1 ) DayTemp=DayNum[0];
							else if(DayNumSpIdx ==2 ) DayTemp=DayNum[0]*10+DayNum[1];
							
							checkDaysOver();
							
						}
					}else if(KeyBoardCode==K_Del ){	
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);					
						if(ProfileMenuIdx == 1 && AgeNumSpIdx){
							AgeNumSpIdx--;						
							if(AgeNumSpIdx==0)
								AgeNum[AgeNumSpIdx]=-1;
							else	
								AgeNum[AgeNumSpIdx]=0;
														
							set_sprite_disable(40+AgeNumSpIdx);																	
						}else if(ProfileMenuIdx == 2 && MonthNumSpIdx){
							MonthNumSpIdx--;					
							MonthNum[MonthNumSpIdx]=0;								
							set_sprite_disable(42+MonthNumSpIdx);
						}else if(ProfileMenuIdx == 3 && DayNumSpIdx){						
							DayNumSpIdx--;					
							DayNum[DayNumSpIdx]=0;								
							set_sprite_disable(44+DayNumSpIdx);
						}
					}
				}				
				if(KeyBoardCode==K_Ok ){		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);				
					
					for (i=0;i<4;i++) M0PlayerOriName[i]=writeing_Words[i];					
					M0PlayerAge=AgeTemp;							
					M0PlayerBirthMonth=MonthTemp;					
					M0PlayerBirthDay = DayTemp; 					
					ClearSomeSp(30,50);
//					Act_SaveSetting();
					Game_State++;					
				}else if( KeyBoardCode==K_Down && ProfileMenuIdx<2 ){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					if(ProfileMenuIdx ==0){
						if(ABC_idx) write_abc();
						for (i=0;i<4;i++) M0PlayerOriName[i]=writeing_Words[i];
						set_sprite_image_number(30+writeing_max,0);						
					}else if(ProfileMenuIdx ==1){
						M0PlayerAge=AgeTemp;		
					}	
					ProfileMenuIdx++;						
				}else if(KeyBoardCode==K_Right && ProfileMenuIdx ==2){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					M0PlayerBirthMonth=MonthTemp;
					checkDaysOver();					
					ProfileMenuIdx++;						
				}else if(KeyBoardCode==K_Left && ProfileMenuIdx ==3){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					M0PlayerBirthDay = DayTemp; 
					ProfileMenuIdx--;						
				}else if(KeyBoardCode==K_Up && ProfileMenuIdx <3 && ProfileMenuIdx ){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					if(ProfileMenuIdx ==2){
						M0PlayerBirthMonth=MonthTemp;					
						checkDaysOver();						
					}else if(ProfileMenuIdx ==1){
						M0PlayerAge= AgeTemp;
						set_sprite_image_number(50,0);												
					}	
					ProfileMenuIdx--;										

				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					ClearSomeSp(30,50);													
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					Game_State=0x01;					
				}	
			break;
			
   	    	case SETTING_TODAY:
   	    		ret=M0_set_Date();
   	    		if(ret>=0){
					ClearSomeSp(YEAR_SP,SELFRAME);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_1-3_time.gif");					
	   	    		Game_State=SETTING_TIME;
   	    		}else if(ret==-2){   	//Back    		
					ClearSomeSp(YEAR_SP,SELFRAME);
					restoreProfileDis();
	   	    		Game_State=USER_PROFILE;
	   	    	}else if(ret==-1){   	//Home 键
					ClearSomeSp(YEAR_SP,SELFRAME);	   	    	
	   	    		Game_State=0xff;
	   	    	}	   	    	
   	    	break;
   	    	case SETTING_TIME:
   	    		ret=M0_set_time();
   	    		if(ret>=0){
					ClearSomeSp(HOUR_NUM,SELFRAME);	   	    		   	    	
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_1-4_sound.gif");	   	    		
	   	    		Game_State=SETTING_VOLUME;
   	    		}else if(ret==-2){   	//Back    		
					ClearSomeSp(HOUR_NUM,SELFRAME);	   	    		   	    	
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_1-2_day.gif");
	   	    		Game_State=SETTING_TODAY;
	   	    	}else if(ret==-1){   	//Home 键
					ClearSomeSp(HOUR_NUM,SELFRAME);	   	    		   	    	
	   	    		Game_State=0xff;
	   	    	}
   	    	break;
   	    	case SETTING_VOLUME:
   	    		ret=M0_set_Volume();   	    	
    	    	if(ret>=0){
					ClearSomeSp(VOLUME,VOLUME+5);	   	    		   	    	
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_1-5_lighting.gif");    	    	
	   	    		Game_State=SETTING_BRIGHT;
   	    		}else if(ret==-2){   	//Back    		
					ClearSomeSp(VOLUME,VOLUME+5);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_1-3_time.gif");					
	   	    		Game_State=SETTING_TIME;
	   	    	}else if(ret==-1){   	//Home 键
	   	    		Game_State=0xff;
	   	    	}
   	    	break;			
   	    	case SETTING_BRIGHT:
   	    		ret=M0_set_Brightness();
    	    	if(ret>=0){
					ClearSomeSp(BRIGHT_SP,BRIGHT_SP+5);
					if(Year!=Date_Year){
						player.GetGiftFlag &=~3;
					}

					Day=Date_Day;
					Month=Date_Month;
					Year=Date_Year;
					
					Time_day=GetDays(Day,Month,Year);
					Get_Date_Start();
					Set_Rtc_Day();
				
					Time_hour=Time_Hour;
					Time_min=Time_Minute;
					
					Set_Rtc_Time();
					LCD_light=M0LCD_light;
					volum=M0volum;	
					
					player.PlayerAge=M0PlayerAge;							
					player.PlayerBirthMonth=M0PlayerBirthMonth;					
					player.PlayerBirthDay = M0PlayerBirthDay;					
					
					for (i=0;i<4;i++) {
						player.PlayerOriName[i]=M0PlayerOriName[i];
					}					
					
					Act_SaveSetting();
					Save_player();
	   	    		Game_State=DIS_MENU_BG;
   	    		}else if(ret==-2){   	//Back    		
					ClearSomeSp(BRIGHT_SP,BRIGHT_SP+5);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_1-4_sound.gif");	   	    		
	   	    		Game_State=SETTING_VOLUME;
	   	    	}else if(ret==-1){   	//Home 键
	   	    		Game_State=0xff;
	   	    	}	   	    	
	   	    	
   	    	break;
   	    	
			case SET_HOMEBG:
				BGSetMenuIdx=0;
				LoadTextBmp(C_PPU_TEXT1,M0_2BGGIF[BGSetMenuIdx]);		 								   	    	
   	    		Game_State++;			
			break;
   	    	case SET_HOMEBG+1:		//菜单操作
				if(KeyBoardCode==K_Down){
					if(BGSetMenuIdx<1){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						BGSetMenuIdx++;
						LoadTextBmp(C_PPU_TEXT1,M0_2BGGIF[BGSetMenuIdx]);		 								   	    							
					}
				}else if(KeyBoardCode==K_Up){
					if(BGSetMenuIdx>0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						BGSetMenuIdx--;
						LoadTextBmp(C_PPU_TEXT1,M0_2BGGIF[BGSetMenuIdx]);		 								   	    							
					}				

				}else if(KeyBoardCode==K_Ok){		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);
					Game_State=SET_HOMEBG+(BGSetMenuIdx+1)*4;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
	   	    		Game_State=DIS_MENU_BG;
					OS_game=0;				
				}
   	    	break;
   	    	case SET_HOMEBG+4:
   	    		M0HomeScreenSelIdx=0;
				LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_2_albumframe.gif");
				set_sprite_display_init(SEL_FRAME,FRAME_XPOS+(M0HomeScreenSelIdx%2)*155,FRAME_YPOS+(M0HomeScreenSelIdx/2)*111,M0Frame_Frame[0]);				
				Game_State++;
			break;	   	  	
   	    	case SET_HOMEBG+5:
				if(KeyBoardCode==K_Down){
					if(M0HomeScreenSelIdx<2){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						M0HomeScreenSelIdx+=2;
						set_sprite_display_init(SEL_FRAME,FRAME_XPOS+(M0HomeScreenSelIdx%2)*155,FRAME_YPOS+(M0HomeScreenSelIdx/2)*111,M0Frame_Frame[0]);				
					}
				}else if(KeyBoardCode==K_Up){
					if(M0HomeScreenSelIdx>=2){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						M0HomeScreenSelIdx-=2;
						set_sprite_display_init(SEL_FRAME,FRAME_XPOS+(M0HomeScreenSelIdx%2)*155,FRAME_YPOS+(M0HomeScreenSelIdx/2)*111,M0Frame_Frame[0]);				
					}
				}else if(KeyBoardCode==K_Left){
					if(M0HomeScreenSelIdx%2==1){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						M0HomeScreenSelIdx--;
						set_sprite_display_init(SEL_FRAME,FRAME_XPOS+(M0HomeScreenSelIdx%2)*155,FRAME_YPOS+(M0HomeScreenSelIdx/2)*111,M0Frame_Frame[0]);				
					}					
				}else if(KeyBoardCode==K_Right){
					if(M0HomeScreenSelIdx%2==0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						M0HomeScreenSelIdx++;
						set_sprite_display_init(SEL_FRAME,FRAME_XPOS+(M0HomeScreenSelIdx%2)*155,FRAME_YPOS+(M0HomeScreenSelIdx/2)*111,M0Frame_Frame[0]);				
					}
				}else if(KeyBoardCode==K_Ok){	
					ClearSomeSp(SEL_FRAME,SEL_FRAME);					
					player.HomeScreenBGIdx = M0HomeScreenSelIdx;
					Save_player();					
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[BLING_SFX], 64, 127, SFX_CHANL);
//					ChangeHomeScreenBG();
					Game_State=DIS_MENU_BG;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					ClearSomeSp(SEL_FRAME,SEL_FRAME);												
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
	   	    		Game_State=SET_HOMEBG;
				}
			break;	   	  				
			
   	    	case SET_HOMEBG+8:
   	    		Disk_Type=FS_NAND2;
				get_photo_list(0,4);
				pageno=page=0;
				if(File_Max==0) Game_State=SET_HOMEBG+18;
				else {
	   	    		M0HomeScreenSelIdx=0;   	    					
					Game_State++;
				}	
			break;	
   	    	case SET_HOMEBG+9:

				clear_sprite(Spritebuf_number);
				show_color_RGB565_bg(255,255,255);
				set_sprite_display_init(0,0,0,M0AlbumFrame_Frame[0]);
				showPhotoThum(page);
				set_sprite_display_init(SEL_FRAME,FRAME_XPOS+(pageno%2)*155,FRAME_YPOS+(pageno/2)*111,M0Frame_Frame[0]);				
				Game_State++;
			break;				
   	    	case SET_HOMEBG+10:
				if(pageno+page*4>=File_Max) pageno=File_Max-1-page*4;
				set_sprite_display_init(1,FRAME_XPOS+pageno%2*155,FRAME_YPOS+pageno/2*111,M0Frame_Frame[0]);
				if(KeyBoardCode==K_Ok){
					File_No=page*4+pageno;		//从4开始代表照片转变过来
					Get_file_info(File_No,pFileList,File_Max);

					AddRootPath(Back_bg[4]);
					unlink(rootPath);
					_copy((char *)file_info.f_name,rootPath);
					player.HomeScreenBGIdx=4;

					Save_player();					
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[BLING_SFX], 64, 127, SFX_CHANL);
					set_sprite_disable(0);										
					set_sprite_disable(SEL_FRAME);										
					Game_State=DIS_MENU_BG;	
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					set_sprite_disable(0);										
					set_sprite_disable(SEL_FRAME);					
					LoadTextBmp(C_PPU_TEXT1,M0_2BGGIF[BGSetMenuIdx]);						
					Game_State= SET_HOMEBG+1;
				}else if(KeyBoardCode==K_Left){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno) pageno--;
					else if(page){
						pageno=3;
						page--;
						Game_State--;
					}
				}else if(KeyBoardCode==K_Right){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno<3) pageno++;
					else if(page<File_Page-1){
						pageno=0;
						page++;
						Game_State--;
					}
				}else if(KeyBoardCode==K_Up){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno>=2) pageno-=2;
					else if(page){
						pageno+=2;
						page--;
						Game_State--;
					}
				}else if(KeyBoardCode==K_Down){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno<2) pageno+=2;
					else if(page<File_Page-1){
						pageno-=2;
						page++;
						Game_State--;
					}
				}

			
			break;
			
			
			case SET_HOMEBG+18:
				ret=Show_Warning(100,9);
				if(ret==-1) Game_State=0xff;
				else {
					LoadTextBmp(C_PPU_TEXT1,M0_2BGGIF[BGSetMenuIdx]);		 								   	    					
					Game_State=SET_HOMEBG+1;
				}	
			break;			
				   	  				
			
			case SET_HOMEICON:
				LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_3-1_iconset.gif");		 								   	    	
   	    		Game_State++;			
			
			break;	   	  						  	
			case SET_HOMEICON+1:
				if(KeyBoardCode==K_Ok){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);
					Game_State++;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
	   	    		Game_State=DIS_MENU_BG;
				}
			break;	   	  						  	
			case SET_HOMEICON+2:   	    	  	    	
				LoadTextBmp(C_PPU_TEXT1,"Pororo\\M0\\M0_3-2_iconset.gif");
				HomeIconNum	= 0;
				for (i=0;i<10;i++)	M0_3HomeIconStatus[i]=0;
				
				//Act_LoadSetting();
				for(i=0;i<4;i++){
					M0HomeIconIdx[i]=player.HomeIconIdx[i];
				}					
				
				for(i=0;i<4;i++){
					if(M0HomeIconIdx[i]>=0 && M0HomeIconIdx[i]<=9 ){				
							HomeIconNum++;
							M0_3HomeIconStatus[HometoMoIconIdx[M0HomeIconIdx[i]]]=1;
						}
				}

				for(i=0;i<HomeIconNum;i++)
					set_sprite_display_init(HOMEICON_SP+i,6+i*80,200-M0_3HomeIconSize[2*HometoMoIconIdx[M0HomeIconIdx[i]]+1]/2,
					M0_HomeIcon_Frame[HometoMoIconIdx[M0HomeIconIdx[i]]]);
									
				for (i=0;i<10;i++){
					if(M0_3HomeIconStatus[i]==0){
						set_sprite_display_init(ICON_SP+i,M0_3HomeIconPos[2*i],M0_3HomeIconPos[2*i+1],M0Icon_Frame[i]);
					}else{
						set_sprite_display_init(ICON_SP+i,M0_3HomeIconPos[2*i],M0_3HomeIconPos[2*i+1],M0IconSelected_Frame[i]);
					}
				}
				M0IconSelIdx=0;
				set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0IconSelected_Frame[M0IconSelIdx]);				
				game_time=0;
				CursorStatus=0;
				Game_State++;										
			break;	
			case SET_HOMEICON+3: 

				if(game_time%13==0){
					CursorStatus^=1;
					if(CursorStatus==1) 				
						set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0IconSelected_Frame[M0IconSelIdx]);	
					else{
						set_sprite_disable(ICON_SP+M0IconSelIdx);					
					}
				}
							
				if(KeyBoardCode==K_Down){
					if(M0IconSelIdx<5){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						if(M0_3HomeIconStatus[M0IconSelIdx]==0){
							set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0Icon_Frame[M0IconSelIdx]);
						}else{
							set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0IconSelected_Frame[M0IconSelIdx]);
						}						
						M0IconSelIdx+=5;
						set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0IconSelected_Frame[M0IconSelIdx]);	
					}
				}else if(KeyBoardCode==K_Up){
					if(M0IconSelIdx>=5){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						if(M0_3HomeIconStatus[M0IconSelIdx]==0){
							set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0Icon_Frame[M0IconSelIdx]);
						}else{
							set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0IconSelected_Frame[M0IconSelIdx]);
						}						
						M0IconSelIdx-=5;
						set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0IconSelected_Frame[M0IconSelIdx]);						}
				}else if(KeyBoardCode==K_Left){
					if(M0IconSelIdx%5!=0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						if(M0_3HomeIconStatus[M0IconSelIdx]==0){
							set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0Icon_Frame[M0IconSelIdx]);
						}else{
							set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0IconSelected_Frame[M0IconSelIdx]);
						}						
						M0IconSelIdx--;
						set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0IconSelected_Frame[M0IconSelIdx]);							
					}					
				}else if(KeyBoardCode==K_Right){
					if(M0IconSelIdx%5!=4){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						if(M0_3HomeIconStatus[M0IconSelIdx]==0){
							set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0Icon_Frame[M0IconSelIdx]);
						}else{
							set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0IconSelected_Frame[M0IconSelIdx]);
						}						
						M0IconSelIdx++;
						set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0IconSelected_Frame[M0IconSelIdx]);							
					}
				}else if(KeyBoardCode==K_Enter){						
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);				
					if(M0_3HomeIconStatus[M0IconSelIdx]==0  ){
						if( HomeIconNum<4){
							M0_3HomeIconStatus[M0IconSelIdx]=1;
							set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0IconSelected_Frame[M0IconSelIdx]);							
							
							M0HomeIconIdx[HomeIconNum]=M0toHomeIconIdx[M0IconSelIdx];
							set_sprite_display_init(HOMEICON_SP+HomeIconNum,6+HomeIconNum*80,200-M0_3HomeIconSize[2*M0IconSelIdx+1]/2,M0_HomeIcon_Frame[M0IconSelIdx]);
							HomeIconNum++;																				
						}
					}else if(M0_3HomeIconStatus[M0IconSelIdx]==1){
							M0_3HomeIconStatus[M0IconSelIdx]=0;							
							set_sprite_display_init(ICON_SP+M0IconSelIdx,M0_3HomeIconPos[2*M0IconSelIdx],M0_3HomeIconPos[2*M0IconSelIdx+1],M0Icon_Frame[M0IconSelIdx]);							
					
							for(i=0;i<HomeIconNum;i++){
								if(M0HomeIconIdx[i]==M0toHomeIconIdx[M0IconSelIdx]) break;
							}
//							player.HomeIconIdx[i]=-1;							
							if(i!=HomeIconNum-1){
								for(j=i;j<HomeIconNum-1;j++)
								M0HomeIconIdx[j]=M0HomeIconIdx[j+1];
							}	
							M0HomeIconIdx[HomeIconNum-1]=-1;														
							HomeIconNum--;
							set_sprite_disable(HOMEICON_SP+HomeIconNum);
							for(i=0;i<HomeIconNum;i++)
								set_sprite_display_init(HOMEICON_SP+i,6+i*80,200-M0_3HomeIconSize[2*HometoMoIconIdx[M0HomeIconIdx[i]]+1]/2,M0_HomeIcon_Frame[HometoMoIconIdx[M0HomeIconIdx[i]]]);
					}
					
				}else if(KeyBoardCode==K_Ok){	
					ClearSomeSp(ICON_SP,ICON_SP+13);
					
					for(i=0;i<4;i++){
						player.HomeIconIdx[i]=M0HomeIconIdx[i];
					}					
										
					Save_player();					
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[BLING_SFX], 64, 127, SFX_CHANL);
					Game_State=0xFF;
					OS_game=0;						
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					ClearSomeSp(ICON_SP,ICON_SP+14);												
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
	   	    		Game_State=DIS_MENU_BG;
				}

			break;	

  	    	
			default:
				SPU_StopChannel(15);	//关掉声音
				return;
			break;
	    }
    	
		if(Game_State==0xFF)break;
    	
    }


}
	


