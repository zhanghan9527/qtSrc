/********************************************************************
* Purpose: M8 Diary
* Author:  David Zhu
* Date:    2017.06.28
* Copyright MCS Co., Ltd.. ALL RIGHTS RESERVED.
* Version : 1.00
* History :
*********************************************************************/

//==============================================================================
// Includes
//==============================================================================
#include "application.h"
#include "data.h"
#include "math.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"

#include "key.h"
#include "photoedit.h"
#include "M08Open.h"
#include "M08Cursor.h"

//==============================================================================
// Macros
//==============================================================================




//==============================================================================
// Constants
//==============================================================================
#define OPEN_ANIM		0
#define CALENDAR		2
#define SET_EMOTION		10
#define SET_WEATHER		14
#define CW_WARNING		17
#define WRITE_DIARY		20
#define SAVE_WARNING	38
#define SELECT_PHOTO	40

#define REVIEW_DIARY	50
#define DEL_WARNING		55




//====卡通索引
#define SP_START		0
#define YEAR_SP			SP_START+1
#define MONTH_SP		YEAR_SP+5
#define DAY_SP			MONTH_SP+3

#define EMOTION_SP		DAY_SP+3
#define WEATHER_SP		EMOTION_SP+4
#define SCROLLBAR_SP	WEATHER_SP+4
#define FRAME_SP		SCROLLBAR_SP+2
#define BUTTON_SP		FRAME_SP+2
#define CURSOR_SP		BUTTON_SP+1



#define DATE_SP			30

//==============================================================================
// Global Variables
//==============================================================================


//INT16S weather;
//INT16S Daily_date=0;
INT16S D_weather=0;
INT16S D_year=0,D_mon=0,D_date=0;
INT16S D_days=0,D_week=0;
INT16S D_Emotion=0;
int D_pic=0;

INT16S Date_Icon[31];
INT16S date;

INT16S year,month,week,days;


const char *M8MailPaper[]={
"M7_3_paper_1.gif",
"M7_3_paper_2.gif",
"M7_3_paper_3.gif",
"M7_3_paper_4.gif",
"M7_3_paper_5.gif",
"M7_3_paper_6.gif",
"S_M7_3_gift_1.gif",
"S_M7_3_gift_2.gif",
};

//==============================================================================
// External Declarations
//==============================================================================

extern INT16S CursorXpos,CursorYpos;	
extern INT16U CursorLine,CursorColumn;	
extern INT16U ScrollBarPos;
extern INT16U MailPaperIdx;	
extern INT16U ScrollBarPosBak;		//用于保存

extern const INT16U ALIGN4 M08_SPRITE_Palette1[];


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

//==============================================================================
// Routines
//==============================================================================
void LoadDaily()
{
	char path[100];
	INT16S fd,i,nRet;
	
	sprintf(path,"Pororo\\daily\\d%d%02d.ini",year,month);
	AddRootPath(path);

	fd = open(rootPath, O_RDONLY);
	if(fd==-1){
		fd = open(rootPath, O_CREAT|O_RDWR);
		for(i=0;i<31;i++) Date_Icon[i]=0;
		nRet = write(fd,(INT32U)Date_Icon,31*2);
		nRet = close(fd);
	    if(DISKUSED==FS_NAND1){
		   DrvNand_flush_allblk();
	    }
	}else{
		nRet = read(fd,(INT32U)Date_Icon,31*2);
		nRet = close(fd);
	}
}
void SaveDaily()
{
	char path[100];
	INT16S fd,i,nRet;
	
	sprintf(path,"Pororo\\daily\\d%d%02d.ini",year,month);
	AddRootPath(path);

	fd = open(rootPath, O_CREAT|O_RDWR);
	nRet = write(fd,(INT32U)Date_Icon,31*2);
	nRet = close(fd);
    if(DISKUSED==FS_NAND1){
	   DrvNand_flush_allblk();
    }
}
INT16S Load_DailyData(INT16S day)
{
	char path[100];
	INT16S fd,i,nRet;
	
	sprintf(path,"Pororo\\daily\\%04d%02d%02d.ini",year,month,day);
	AddRootPath(path);

	fd = open(rootPath, O_RDONLY);
	if(fd==-1){
		return -1;
	}else{
		nRet = read(fd,(INT32U)&D_Emotion,2);
		nRet = read(fd,(INT32U)&D_weather,2);
		nRet = read(fd,(INT32U)&D_pic,4);
		nRet = read(fd,(INT32U)&writeing_Words,128*2);
		nRet = close(fd);
	}
	return 0;
}
void Save_DailyData(INT16S day)
{
	char path[100];
	INT16S fd,i,nRet;
	
	sprintf(path,"Pororo\\daily\\%04d%02d%02d.ini",year,month,day);
	AddRootPath(path);

	fd = open(rootPath, O_CREAT|O_RDWR);
	nRet = write(fd,(INT32U)&D_Emotion,2);
	nRet = write(fd,(INT32U)&D_weather,2);
	nRet = write(fd,(INT32U)&D_pic,4);
	nRet = write(fd,(INT32U)&writeing_Words,128*2);

	nRet = close(fd);
    if(DISKUSED==FS_NAND1){
	   DrvNand_flush_allblk();
    }
}
void Del_DailyData(INT16S day)
{
	char path[100];
	INT16S fd,i,nRet;
	
	sprintf(path,"Pororo\\daily\\%04d%02d%02d.ini",year,month,1+day);
	AddRootPath(path);

	unlink(rootPath);
    if(DISKUSED==FS_NAND1){
	   DrvNand_flush_allblk();
    }
}

//===================================================================
const Weather_size[]={
	34,34,	36,20,	38,38,	38,32
};
#define Daily_W	11
void show_Daily(INT16S pos)
{
	int y=0;
	
	if(pos) y=38;
	set_sprite_display_init(10,16,29+y,M08Date_Num_Frame[(D_year)/1000]);
	set_sprite_display_init(11,16+Daily_W,29+y,M08Date_Num_Frame[(D_year)/100%10]);
	set_sprite_display_init(12,16+Daily_W*2,29+y,M08Date_Num_Frame[(D_year)/10%10]);
	set_sprite_display_init(13,16+Daily_W*3,29+y,M08Date_Num_Frame[(D_year)%10]);

	if(D_mon>=10){
		set_sprite_display_init(14,81,29+y,M08Date_Num_Frame[(D_mon)/10]);
		set_sprite_display_init(15,81+Daily_W,29+y,M08Date_Num_Frame[(D_mon)%10]);
	}else{
		set_sprite_display_init(14,86,29+y,M08Date_Num_Frame[(D_mon)]);
		set_sprite_display_init(15,154,29,0);
	}
	if(D_date>=10){
		set_sprite_display_init(16,120,29+y,M08Date_Num_Frame[(D_date)/10]);
		set_sprite_display_init(17,120+Daily_W,29+y,M08Date_Num_Frame[(D_date)%10]);
	}else{
		set_sprite_display_init(16,125,29+y,M08Date_Num_Frame[(D_date)]);
		set_sprite_display_init(17,154,29,0);
	}
//	set_sprite_display_init(18,193-Weather_size[2*D_weather]/2,38-Weather_size[2*D_weather+1]/2+y,M08_sp_Frame[1+D_weather]);
}
void getNextMonth(INT16S dec,INT16S num)
{
	if(dec==1){
		//if(year==2027 && month==12) return;
		if(month<12) month++;
		else{
			month=1;
			year++;
		}
		week=(week+days)%7;
		date=(date+num)-days;
		days=getMonthDate(month,year);
	}else{
		//if(year==2017 && month==1) return;
		if(month>1) month--;
		else{
			month=12;
			year--;
		}
		days=getMonthDate(month,year);
		week=(week+35-days)%7;
		date=date+days-num;
	}

}


#define YEAR_SP_XPOS	10
#define YEAR_SP_YPOS	12

void show_calendar()
{
	int i,j,w,x,y;

	clear_sprite(Spritebuf_number);
	
	set_sprite_display_init(YEAR_SP,YEAR_SP_XPOS,YEAR_SP_YPOS,M08Date_Num_Frame[(year)/1000]);
	set_sprite_display_init(YEAR_SP+1,YEAR_SP_XPOS+10,YEAR_SP_YPOS,M08Date_Num_Frame[(year)/100%10]);
	set_sprite_display_init(YEAR_SP+2,YEAR_SP_XPOS+20,YEAR_SP_YPOS,M08Date_Num_Frame[(year)/10%10]);
	set_sprite_display_init(YEAR_SP+3,YEAR_SP_XPOS+30,YEAR_SP_YPOS,M08Date_Num_Frame[(year)%10]);
	set_sprite_display_init(YEAR_SP+4,YEAR_SP_XPOS+44,YEAR_SP_YPOS,M08Y_M_D_Frame[0]);	
	
	set_sprite_display_init(MONTH_SP,142,28,M08Month_Num_Frame[month-1]);

	for(i=0;i<days;i++){
		w=week+i;
		x=24+(week+i)%7*42;
		y=96+(w/7)*23;

		if(Date_Icon[i]) set_sprite_display_init(DATE_SP+i*2,x-3,y-2,M8Heart_Frame[0]);
		if(year==Year && month==Month && i==Day-1) set_sprite_display_init(DATE_SP+i*2+1,x,y,M08Date_Num_Frame[i+1]);	//显示今天
		else set_sprite_display_init(DATE_SP+i*2+1,x,y,M08Date_Num_Frame[i+1]);		

		if(i==date-1)set_sprite_display_init(FRAME_SP,x-6,y-8,M8DateFrame_Frame[0]);		//光标

	}
}

#define YEAR_SP_XPOS1	4
#define YEAR_SP_YPOS1	8


void show_M8date()
{
	int i,j,w,x,y;

	
	set_sprite_display_init(YEAR_SP,YEAR_SP_XPOS1,YEAR_SP_YPOS1,M08Date_Num_Frame[(year)/1000]);
	set_sprite_display_init(YEAR_SP+1,YEAR_SP_XPOS1+9,YEAR_SP_YPOS1,M08Date_Num_Frame[(year)/100%10]);
	set_sprite_display_init(YEAR_SP+2,YEAR_SP_XPOS1+9*2,YEAR_SP_YPOS1,M08Date_Num_Frame[(year)/10%10]);
	set_sprite_display_init(YEAR_SP+3,YEAR_SP_XPOS1+9*3,YEAR_SP_YPOS1,M08Date_Num_Frame[(year)%10]);
	set_sprite_display_init(YEAR_SP+4,YEAR_SP_XPOS1+4+9*4,YEAR_SP_YPOS1,M08Y_M_D_Frame[0]);	

	set_sprite_display_init(MONTH_SP+0,YEAR_SP_XPOS1+4+9*4+17,YEAR_SP_YPOS1,M08Date_Num_Frame[(month)/10]);
	set_sprite_display_init(MONTH_SP+1,YEAR_SP_XPOS1+4+9*5+17,YEAR_SP_YPOS1,M08Date_Num_Frame[(month)%10]);		
	set_sprite_display_init(MONTH_SP+2,YEAR_SP_XPOS1+6+9*6+17,YEAR_SP_YPOS1,M08Y_M_D_Frame[1]);		

	set_sprite_display_init(DAY_SP+0,YEAR_SP_XPOS1+6+9*6+17+15,YEAR_SP_YPOS1,M08Date_Num_Frame[(date)/10]);
	set_sprite_display_init(DAY_SP+1,YEAR_SP_XPOS1+6+9*7+17+15,YEAR_SP_YPOS1,M08Date_Num_Frame[date%10]);		
	set_sprite_display_init(DAY_SP+2,YEAR_SP_XPOS1+8+9*8+17+15,YEAR_SP_YPOS1,M08Y_M_D_Frame[2]);		
}

const Weatreh_Pos[]={
	64,125,		175,125,	64,222,		175,222
};
//=====================================================================

void setCursorDis(int i)
{
	int w,x,y;
	i--;
	w=week+i;
	x=24+(week+i)%7*42;
	y=96+(w/7)*23;
	set_sprite_display_init(FRAME_SP,x-6,y-8,M8DateFrame_Frame[0]);		//光标
}

void Main_M08()
{
    INT64U  disk_size;
	INT16S i,m,ret,nRet;
	INT16S img_board=0;
	INT16S M8AnimTime;	
	INT16S M8Emotion,M8Weather;
	INT16S cw_warningBG;
	INT16S d_warningBG;
	INT16S CursorStatus;	

	INT16U w,x,y;
	INT16S page=0,pageno=0;

	Disk_Type=DISKUSED;	
	Chdir_Root("NoteBook");
	nRet=chdir("daily");
	if(nRet<0){
		nRet=mkdir("daily");
		nRet=chdir("daily");
	}
	spMalloc(500+200+900);
	Word_Kb_Num=500;		//韩文输入法	
	Words_max=126;			//韩文输入法	
	
	Get_Rtc_Data();
	year=Year;
	month=Month;
	date=Day;
	week=(Week+35+1-Day)%7;
	days=getMonthDate(month,year);

	INIT_VideoPub(700);
	LoadData_1(1,SpriteCellData,"Pororo\\M8\\SpriteCell_cmp.BIN");
	LoadData_1(0,SpriteCellData+Word_Kb_Num*_1K,"Pororo\\words\\SpriteCell.bin");		
//	LoadDRM_1("Pororo\\M8\\sfx\\sound_fortune.drm",SFX_ADDR_GAME,0);
	gplib_ppu_fade_effect_set(ppu_register_set, 0xff);
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	clear_sprite(Spritebuf_number);
	Palette_ram_cpy( 1,0, M08_SPRITE_Palette1+256*0	,256);
	Palette_ram_cpy( 3,0, M08_SPRITE_Palette1+256*1	,256);	
	Palette_ram_cpy( 2,240, words_SPRITE_Palette1	,16);		//韩文输入法		
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	set_sprite_display_init(0,0,0,M08Open_Frame[0]);	
	SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[MENU_OPEN_SFX], 64, 127, SFX_CHANL);													   	    	   	      		
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);

//	if(OS_game==150) {
//		OS_game=15;
//		step=20;
//	}else step=0;
	M8AnimTime=0;
	CursorStatus=0;
	
	Game_State=OPEN_ANIM;
//	set_sprite_display_init(0,0,0,Y_8x8_Frame[0]);	
//	LoadTextBmp(C_PPU_TEXT1,"Pororo\\M8\\M8_calender.gif");	
	while(1)
	{
		PaintTFT_cell4();
		switch(Game_State){
//--------------------------------------------------tile
   	    	case OPEN_ANIM:		//Open Anim
				M8AnimTime++;
				set_sprite_display_init(0,0,0,M08Open_Frame[M8AnimTime/3]);
				if(M8AnimTime>=3*6){
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M8\\M8_calender.gif");
					ClearSomeSp(0,0);
					LoadData_1(1,SpriteCellData+0*_1K,"Pororo\\m9\\SpriteCell_cmp.BIN");
					Game_State=CALENDAR;
				}
   	    		
   	    	break;	
//------------------------------calendar--------------------
			case CALENDAR:
				clear_sprite(Spritebuf_number);
				LoadTextBmp(C_PPU_TEXT1,"Pororo\\M8\\M8_calender.gif");		 								
				LoadDaily();				
				show_calendar();
				
				game_time=0;
				Game_State++;

			case CALENDAR+1:
				show_calendar();
				if(KeyBoardCode==K_Down){
					if(date<days+1-7){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						date+=7;
						setCursorDis(date);
					}else{
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);																	
						getNextMonth(1,7);
						LoadDaily();													
					}
				}else if(KeyBoardCode==K_Up){
					if(date>7){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						date-=7;
						setCursorDis(date);
					}else{
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);																	
						getNextMonth(-1,7);							
						LoadDaily();																			
					}
				}else if(KeyBoardCode==K_Left){
					if(date>1){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						date--;
						setCursorDis(date);
					}else{
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);																	
						getNextMonth(-1,1);							
						LoadDaily();																			
					}
				}else if(KeyBoardCode==K_Right){
					if(date<days){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						date++;
						setCursorDis(date);
					}else{
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);																	
						getNextMonth(1,1);							
						LoadDaily();																			
					}
				}else if(KeyBoardCode==K_Ok){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);																	
					ClearSomeSp(DATE_SP,DATE_SP+62);
	
					if(Date_Icon[date-1]){
						if(Load_DailyData(date)==0) Game_State=REVIEW_DIARY;
						else Date_Icon[date-1]=0;
					}else{
	//					Daily_date=0;
	
						D_year=year;
						D_mon=month;
						D_date=date;					

						D_days=getMonthDate(month,year);
						D_week=(Week+35+1-Day)%7;
	
						M8Emotion=0;M8Weather=0;	
						D_pic=0;
						D_weather=0;D_Emotion=0;
						Keyboard=1;
						InitABC();
						writeing_max=126;			//韩文输入法	
						for(i=0;i<writeing_max;i++) writeing_Words[i]=0;  //韩文输入法						

						LoadNoteBookTextBmp(C_PPU_TEXT1,320,480,"Pororo\\M8\\M8_bg.gif");										
						Game_State=SET_EMOTION;										
					}
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					ClearSomeSp(YEAR_SP,DATE_SP+62);					
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);																	
					Game_State=0xff;					
					OS_game=0;
				}
			break;

			case SET_EMOTION:
				clear_sprite(Spritebuf_number);
				show_M8date();
				ScrollBarPos=0;
				set_sprite_display_init(EMOTION_SP,146,5-120*ScrollBarPos,M08Emotion_Frame[D_Emotion]);
				set_sprite_display_init(FRAME_SP,146-5,5-5,M8DateFrame_Frame[0]);				
				set_sprite_display_init(WEATHER_SP,172,5-120*ScrollBarPos,M08Weather_Frame[D_weather]);
				set_sprite_display_init(SCROLLBAR_SP,296,13,M08Scroll_Frame[1]);
				set_sprite_display_init(SCROLLBAR_SP+1,298,31,M08Scroll_Frame[0]);				
				Game_State++;
			break;
			case SET_EMOTION+1:
				if(KeyBoardCode==K_Down|| KeyBoardCode==K_Ok){
					set_sprite_display_init(EMOTION_SP,146,5,M08Emotion_Frame[0]);
					set_sprite_display_init(EMOTION_SP+1,146,5+23*1,M08Emotion_Frame[1]);				
					set_sprite_display_init(EMOTION_SP+2,146,5+23*2,M08Emotion_Frame[2]);				
					set_sprite_display_init(EMOTION_SP+3,146,5+23*3,M08Emotion_Frame[3]);														
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);																					
					M8Emotion=D_Emotion;
					Game_State++;										
				}else if(KeyBoardCode==K_Right){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);																	
					ClearSomeSp(EMOTION_SP+1,EMOTION_SP+3);										
					Game_State=SET_WEATHER;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					ret=Show_Warning(200,0);
					if(ret==0){
					}else if(ret==1){
						Game_State=CALENDAR;
					}else Game_State=0xff;
				}
			
			break;
			case SET_EMOTION+2:			
				set_sprite_display_init(FRAME_SP,146-5,5-5+23*M8Emotion,M8DateFrame_Frame[0]);
				if(KeyBoardCode==K_Down){
					if(M8Emotion<3){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						M8Emotion++;
						
					}
				}else if(KeyBoardCode==K_Up){
					if(M8Emotion>0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						M8Emotion--;
					}
				}else if(KeyBoardCode==K_Ok){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);
					D_Emotion=M8Emotion;
					ClearSomeSp(EMOTION_SP+1,EMOTION_SP+3);										
					Game_State=SET_EMOTION;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);	
					ClearSomeSp(EMOTION_SP+1,EMOTION_SP+3);
					Game_State=SET_EMOTION;					
				}
			break;

			case SET_WEATHER:
				show_M8date();
				set_sprite_display_init(FRAME_SP,172-5,0,M8DateFrame_Frame[0]);
				set_sprite_display_init(WEATHER_SP,172,5-120*ScrollBarPos,M08Weather_Frame[D_weather]);
				if(KeyBoardCode==K_Down|| KeyBoardCode==K_Ok){
					set_sprite_display_init(WEATHER_SP,172,5,M08Weather_Frame[0]);
					set_sprite_display_init(WEATHER_SP+1,172,5+23*1,M08Weather_Frame[1]);
					set_sprite_display_init(WEATHER_SP+2,172,5+23*2,M08Weather_Frame[2]);
					set_sprite_display_init(WEATHER_SP+3,172,5+23*3,M08Weather_Frame[3]);
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);	
					M8Weather=D_weather;																				
					Game_State++;										
				}else if(KeyBoardCode==K_Right){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);																	
					ClearSomeSp(WEATHER_SP+1,WEATHER_SP+3);					
					ClearSomeSp(FRAME_SP,FRAME_SP);			

					Game_State=WRITE_DIARY;
				}else if(KeyBoardCode==K_Left){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);																					
					//D_weather=0;M8Weather=0;
					//D_Emotion=0;M8Emotion=0;										
					//set_sprite_display_init(EMOTION_SP,146,5,M08Emotion_Frame[0]);
					//set_sprite_display_init(WEATHER_SP,172,5,M08Weather_Frame[0]);
					//set_sprite_display_init(FRAME_SP,146-5,5-5,M8DateFrame_Frame[0]);	
					ClearSomeSp(WEATHER_SP+1,WEATHER_SP+3);															
					Game_State=SET_EMOTION;					
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					ret=Show_Warning(200,0);
					if(ret==0){
					}else if(ret==1){
						Game_State=CALENDAR;
					}else Game_State=0xff;
				}
			break;
			case SET_WEATHER+1:
				set_sprite_display_init(FRAME_SP,172-5,0+23*M8Weather,M8DateFrame_Frame[0]);										
				if(KeyBoardCode==K_Down){
					if(M8Weather<3){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						M8Weather++;
					}
				}else if(KeyBoardCode==K_Up){
					if(M8Weather>0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						M8Weather--;
					}
				}else if(KeyBoardCode==K_Ok){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);																	
					D_weather=M8Weather;
					ClearSomeSp(WEATHER_SP+1,WEATHER_SP+3);					
					Game_State=SET_WEATHER;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);	
					ClearSomeSp(WEATHER_SP+1,WEATHER_SP+3);																				
					Game_State=SET_WEATHER;					
				}
			break;
			case WRITE_DIARY:
				ScrollBarPos=0;
				gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0, 120*ScrollBarPos);
				show_M8date();
				set_sprite_display_init(EMOTION_SP,146,5-120*ScrollBarPos,M08Emotion_Frame[D_Emotion]);
				set_sprite_display_init(WEATHER_SP,172,5-120*ScrollBarPos,M08Weather_Frame[D_weather]);
				set_sprite_display_init(SCROLLBAR_SP+1,298,31+ScrollBarPos*46,M08Scroll_Frame[0]);
				set_sprite_display_init(SCROLLBAR_SP,296,13,M08Scroll_Frame[1]);
				set_sprite_display_init(FRAME_SP+1,83-1,52-1-120*ScrollBarPos,M8DateFrame_Frame[1]);
				show_writing_m9(30,ScrollBarPos);
				if(KeyBoardCode==K_Down|| KeyBoardCode==K_Enter){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					//TVBuffer_to_Buff();
					
					ScrollBarPos=1;					
					gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0, 120*ScrollBarPos);

					ClearSomeSp(YEAR_SP,DAY_SP+2);
					set_sprite_display_init(EMOTION_SP,146,5-120*ScrollBarPos,M08Emotion_Frame[D_Emotion]);															
					set_sprite_display_init(WEATHER_SP,172,5-120*ScrollBarPos,M08Weather_Frame[D_weather]);															
					set_sprite_display_init(FRAME_SP,172-5,5-5-120*ScrollBarPos,M8DateFrame_Frame[0]);														
					set_sprite_disable(FRAME_SP+1);					

					Keyboard=1;
					CursorLine=0;CursorColumn=0;					
					CursorXpos=40+16*CursorColumn;CursorYpos=174+32*CursorLine-120*ScrollBarPos;
					Words_pos=0;
					CursorLine=0;CursorColumn=0;

					Game_State++;						
					
				}else if(KeyBoardCode==K_Ok){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);																	
					Game_State=SELECT_PHOTO;
				}else if(KeyBoardCode==K_Left){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);																					
					set_sprite_disable(FRAME_SP+1);															
					Game_State=SET_WEATHER;						
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					ret=Show_Warning(200,0);
					if(ret==0){
					}else if(ret==1){
						Game_State=CALENDAR;
					}else Game_State=0xff;
				}	
								
			break;			
			case WRITE_DIARY+1:
				gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0, 120*ScrollBarPos);
				set_sprite_display_init(SCROLLBAR_SP+1,298,31+ScrollBarPos*46,M08Scroll_Frame[0]);
				show_writing_m9(30,ScrollBarPos);
				if(CursorLine==8){
					set_sprite_display_init(BUTTON_SP,101,196+4+240-120*ScrollBarPos,M08Button_Frame[1]);
				}else{
					set_sprite_display_init(BUTTON_SP,101,196+4+240-120*ScrollBarPos,M08Button_Frame[0]);
				}						

				if(ScrollBarPos && CursorLine>=0 && CursorLine <=7 ){
					if(game_time%5==0) CursorStatus^=1;
					CursorLine=Words_pos/16;
					if(CursorLine>=3)ScrollBarPos=2;
					else ScrollBarPos=1;
					if(CursorStatus==1){
						CursorXpos=30+(Words_pos%16)*16;
						if(abc) CursorXpos+=16;
						if(CursorLine>=3)CursorYpos=178+32*CursorLine-120*ScrollBarPos;
						else CursorYpos=174+32*CursorLine-120*ScrollBarPos;
						set_sprite_display_init(CURSOR_SP,CursorXpos,CursorYpos,M08Cursor_Frame[0]);
					}else{
						set_sprite_display_init(CURSOR_SP,0,0,0);
					}
					if(KeyBoardCode==K_Left){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						write_abc();
						if(Words_pos) Words_pos--;
					}else if(KeyBoardCode==K_Right && Words_pos<126){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						write_abc();
						Words_pos++;
					}else {
						m=Test_Keyboard9();
						//if(m==0) SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					}
					if(Words_pos>=126){
						Words_pos=126;
						InitABC();
					}
				}else set_sprite_display_init(CURSOR_SP,0,0,0);

				if(KeyBoardCode==K_Down || KeyBoardCode==K_Enter){
					write_abc();
					if(ScrollBarPos==0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						ScrollBarPos++;
					}else if(CursorLine<=7){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						if(CursorLine<7) Words_pos=(CursorLine+1)*16;
						else CursorLine++;
					}
				}else if(KeyBoardCode==K_Up){
					write_abc();
					if(CursorLine>0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						if(CursorLine<8) Words_pos-=16;
						else {
							CursorLine--;
							Words_pos=7*16;
						}
					}else if(CursorLine==0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);	
						set_sprite_display_init(CURSOR_SP,0,0,0);																
						ScrollBarPos=0;						
						Game_State=WRITE_DIARY;
					}				

				}else if(KeyBoardCode==K_Ok){		
					write_abc();
					//TVBuffer_to_Buff();
					if(CursorLine==8){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);	
						game_time=0;
						Game_State=SAVE_WARNING;			
					}else {
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);
						CursorLine=8;
						ScrollBarPos=2;
					}				
															
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					ret=Show_Warning(200,0);
					if(ret==0){
					}else if(ret==1){
						Game_State=CALENDAR;
					}else Game_State=0xff;
				}
				//if(ScrollBarPos==0) show_M8date();							
				//else ClearSomeSp(YEAR_SP,DAY_SP+2);								

			break;						
			case WRITE_DIARY+2:
					
			break;				

			case SAVE_WARNING:
				if(game_time<4*7){
					set_sprite_display_init(200,0,0,M08Save_Frame[game_time/2%7]);
				}else{
					if(game_time==28) SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[BLING_SFX], 64, 80, 12);
					set_sprite_display_init(200,0,0,M08Save_Frame[7]);
				}
				if(game_time>40){
					Game_State++;
				}
			break;	
			case SAVE_WARNING+1:
				Date_Icon[date-1]=1;
				SaveDaily();
				Save_DailyData(date);

				//image_scale_YUV_320x480(IconBuff,SensorBuff1,320,480);					
				//if(buff_to_save_jpgDiary(SensorBuff1,Sensor_Md,320,480)==0){
				//	ret=0;
				//}									
				Game_State=CALENDAR;
			break;
//======================take a photo from album=======================================
			case SELECT_PHOTO:
				Disk_Type=FS_NAND2;
				Chdir_Root("M2_photo");
				Game_State++;				
			break;
			case SELECT_PHOTO+1:
				get_photo_list(0,4);
				pageno=page=0;
				if(File_Max==0) Game_State=SELECT_PHOTO+8;
				else Game_State++;
			break;

			case SELECT_PHOTO+2:
				clear_sprite(Spritebuf_number);
				show_color_RGB565_bg(255,255,255);
				set_sprite_display_init(FRAME_SP,0,0-120*ScrollBarPos,M08AlbumFrame_Frame_Frame[0]);															
				set_sprite_display_init(FRAME_SP+1,5+pageno%2*155,8+pageno/2*111,M8DateFrame_Frame[1]);
				showPhotoThum(page);
				
				Game_State++;
			break;
			case SELECT_PHOTO+3:
				if(pageno+page*4>=File_Max) pageno=File_Max-1-page*4;
				set_sprite_display_init(FRAME_SP+1,5+pageno%2*155,9+pageno/2*111,M8DateFrame_Frame[1]);
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					LoadNoteBookTextBmp(C_PPU_TEXT1,320,480,"Pororo\\M8\\M8_bg.gif");			
					File_No=page*4+pageno;
			 		D_pic=Get_file_info(File_No,pFileList,File_Max);
			 		if(image_decode_to_thum((char *)file_info.f_name,THUM_W,THUM_H,VideoBuff)>=0){
			 			show_photo_thum(86,56);
			 		}
				
					set_sprite_disable(FRAME_SP);
					set_sprite_display_init(FRAME_SP+1,83-2,52-3-120*ScrollBarPos,M8DateFrame_Frame[1]);																 		
					Game_State=WRITE_DIARY;										

				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					set_sprite_disable(FRAME_SP);										
					LoadNoteBookTextBmp(C_PPU_TEXT1,320,480,"Pororo\\M8\\M8_bg.gif");
					sprintf((char *)file_info.f_name,"M2_%04d.jpg",D_pic);
			 		if(image_decode_to_thum((char *)file_info.f_name,THUM_W,THUM_H,VideoBuff)>=0){
			 			show_photo_thum(86,56);
			 		}
					Game_State=WRITE_DIARY;
				}else if(KeyBoardCode==K_Left){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno) pageno--;
					else if(page){
						pageno=3;
						page--;
						Game_State=SELECT_PHOTO+2;
					}
				}else if(KeyBoardCode==K_Right){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno<3) pageno++;
					else if(page<File_Page-1){
						pageno=0;
						page++;
						Game_State=SELECT_PHOTO+2;
					}
				}else if(KeyBoardCode==K_Up){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno>=2) pageno-=2;
					else if(page){
						pageno+=2;
						page--;
						Game_State=SELECT_PHOTO+2;
					}
				}else if(KeyBoardCode==K_Down){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno<2) pageno+=2;
					else if(page<File_Page-1){
						pageno-=2;
						page++;
						Game_State=SELECT_PHOTO+2;
					}
				}
			break;

			case SELECT_PHOTO+8:
				ret=Show_Warning(200,9);
				if(ret==-1) Game_State=0xff;
				else Game_State=WRITE_DIARY;
				File_No=-1;

			break;
//==============================daily view==================================
			case REVIEW_DIARY:
				clear_sprite(Spritebuf_number);
				LoadNoteBookTextBmp(C_PPU_TEXT1,320,480,"Pororo\\M8\\M8_bg.gif");
		 		Disk_Type=FS_NAND2;	
				Chdir_Root("M2_photo");
				sprintf((char *)file_info.f_name,"M2_%04d.jpg",D_pic);
		 		if(image_decode_to_thum((char *)file_info.f_name,THUM_W,THUM_H,VideoBuff)>=0){
		 			show_photo_thum(86,56);
		 		}
				Game_State++;
				ScrollBarPos=0;
				InitABC();
				writeing_max=126;
			//break;
			case REVIEW_DIARY+1:
				set_sprite_display_init(EMOTION_SP,146,5-120*ScrollBarPos,M08Emotion_Frame[D_Emotion]);
				set_sprite_display_init(WEATHER_SP,172,5-120*ScrollBarPos,M08Weather_Frame[D_weather]);
				set_sprite_display_init(SCROLLBAR_SP,296,13,M08Scroll_Frame[1]);																																				
				set_sprite_display_init(SCROLLBAR_SP+1,298,31+ScrollBarPos*46,M08Scroll_Frame[0]);				
				show_writing_m9(30,ScrollBarPos);
				gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0, 120*ScrollBarPos);
				if(KeyBoardCode==K_Down){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					if(ScrollBarPos<2) ScrollBarPos++;
				}else if(KeyBoardCode==K_Up){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					if(ScrollBarPos) ScrollBarPos--;
				}else if(KeyBoardCode==K_Ok || KeyBoardCode==K_Return || KeyBoardCode==K_Back){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);																	
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M8\\M8_calender.gif");		 													
					Game_State=CALENDAR;
				}else if(KeyBoardCode==K_Del){
					Game_State=DEL_WARNING;							
				}	
				if(ScrollBarPos==0) show_M8date();
				else ClearSomeSp(YEAR_SP,DAY_SP+2);								
			break;			
			
			case DEL_WARNING:
				ret=Show_Warning(200,2);
				if(ret==-1) Game_State=0xff;
				else if(ret==1){
					Del_DailyData(date);
					Date_Icon[date-1]=0;
					SaveDaily();
					Game_State=CALENDAR;
				
				}else Game_State=REVIEW_DIARY+1;

			break;									
		}
		if(Game_State>=10 && Game_State<50){
			if(TestMenu_Key()){
				ret=Show_Warning(200,0);
				if(ret==0){
					OS_game=18;
				}else Game_State=0xff;
			}
		}else if(Goto_desktop()){
			Game_State=0xff;
		}
		if(Game_State==0xff) break;
	}
}

