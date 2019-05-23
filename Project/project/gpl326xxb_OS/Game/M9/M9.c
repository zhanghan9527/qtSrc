/********************************************************************
* Purpose: M9 Mimi Card
* Author:  David Zhu
* Date:    2017.07.12
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

#include "m9scrollbar.h"
#include "photo_buttton.h"
#include "key.h"
#include "photoedit.h"
#include "driver_l1.h"
#include "words_sp.h"
#include "m9_open.h"

//==============================================================================
// Constants
//==============================================================================
#define OPEN_ANIM		0
#define DIS_MENU_BG		2
#define MENU_SELECT		2

#define WRITELETTER		10

#define CW_WARNING		20
#define SELECT_PHOTO	30

#define SAVE_WARNING	40

#define SHOW_PHOTO		50

//====卡通索引====
#define SP_START		0
#define YEAR_SP			SP_START+1
#define MONTH_SP		YEAR_SP+5
#define DAY_SP			MONTH_SP+3
#define ARROW_SP		DAY_SP+3
#define ICON_SP			ARROW_SP+3
#define SCROLLBAR_SP	ICON_SP+1
#define FRAME_SP		SCROLLBAR_SP+2
#define BUTTON_SP		FRAME_SP+1
#define CURSOR_SP		BUTTON_SP+1
//====INBOX====

//====OUTBOX====

//====WRITEMAIL====

const char* m9_Names[]={
	"Pororo\\m9\\M9_open_guide_p1.gif",
	"Pororo\\m9\\M9_open_guide_p2.gif",
	"Pororo\\m9\\M9_open_guide_p3.gif",
	"Pororo\\m9\\M9_open_guide_p4.gif",
	"Pororo\\m9\\M9_open_guide_p5.gif",
	"Pororo\\m9\\M9_open_guide_p6.gif",
};

const char *M9MenuBGGIF[]={
"Pororo\\M9\\M9_1_menu.gif",
"Pororo\\M9\\M9_2_menu.gif",
"Pororo\\M9\\M9_3_menu.gif",
};

extern const char *M7MailPaper[];
extern const char *M7SendAnimGif[];
//==============================================================================
// Global Variables
//==============================================================================
extern const INT16U video_SPRITE_Palette1[],M9_SPRITE_Palette1[];
extern INT32U scaler_ptr,decode_mode_ck,BLOCK_ENCODE;
extern VIDEO_INFO video_information;

extern INT16S CursorXpos,CursorYpos;	
extern INT16U CursorLine,CursorColumn;	
extern INT16U ScrollBarPos;

extern INT16U MailPaperIdx;	
extern const char* m3_Names[];
//==============================================================================
// External Declarations
//==============================================================================
extern INT16S year,month,week,days;
extern INT16S date;
extern const INT16U M08_SPRITE_Palette1[],video_SPRITE_Palette1[];
extern INT16S img_board,Video_Yuv,Video_First;
extern const INT16S Theme_Frame[];

//==============================================================================
// Forward Declarations
//==============================================================================
extern int D_pic;
//==============================================================================
// Routines
//==============================================================================

void showThum_M9(INT16S no)
{
	INT16S k,m;
	INT16S x,y;

	if(no!=-1){
		Get_file_info(no,pFileList,File_Max);
		if(image_decode_to_thum((char *)file_info.f_name,THUM_W,THUM_H,VideoBuff)>=0){
			Copy32(VideoBuff+0x10000,VideoBuff,0x10000/4);
		}
	}else Copy32(VideoBuff,VideoBuff+0x10000,0x10000/4);
	x=86;
	y=56;
	show_photo_thum(x,y);
}
#define Writing_X		32
#define Writing_Y		184
#define Writing_Line	16
void show_writing_m9(INT16S sp,INT16S pos)
{
	INT16S i,x,y,m,n,k;

	x=Writing_X;
	if(pos==0) y=Writing_Y;
	else if(pos==1)y=Writing_Y-120;
	else y=Writing_Y-240;
	ClearSomeSp(sp,sp+128+4);
	m=writeing_max;

	for(i=0;i<=m;i++){
		if(Words_pos==i){
			if(abc){
				show_word(sp,x,y,abc);
				sp++;
				x+=16;
			}
			if(x>=Writing_X+16*Writing_Line){
				x=Writing_X;
				y+=16;
			}
		}
		if(i==m) break;
		n=writeing_Words[i];
		show_word(sp,x,y,n);
		x+=16;
		if(x>=Writing_X+16*Writing_Line){
			x=Writing_X;
			y+=32;
		}
		sp++;
	}
}
INT16S Save_Lettle()
{
	INT16S ret;

	if(OS_game==17){
		Disk_Type=FS_NAND2;
		Chdir_Root("Mimimail");
	}else{
		Disk_Type=FS_SD;
		Chdir_Root("mimicard_photo");
		ret=showPop_Save(1);
		if(ret==-1) {
			Game_State=0xff;
			return -1;
		}else if(ret==1) return -1;
	}
	image_scale_YUV_320x480(ScalerBuff,IconBuff,320,480);
	if(buff_to_save_jpgfile(IconBuff,ScalerBuff,320,480)==0){
		ret=0;
	}else ret=-1;
	
	return ret;
}
void SaveMail_Buff(INT16S sp,INT16S St,INT16S tvSt,INT16S Num)
{
	int i;
	
	set_sprite_disable(sp);
	PaintTFT_cell4();
	PaintTFT_cell4();	
	for(i=0;i<Num;i++){
		Copy32(ScalerBuff+320*2*(St+i),TV_buffer[0]+320*2*(tvSt+i),290/2);
	}
}

extern INT16S height_Two;
void Main_M9()
{
	INT16S i,m,ret;
	INT16S M9MenuIdx;		
	INT16S send_warningBG;
	INT16S CursorStatus;	
	INT16U SendReady;
	INT16U SendAnimIdx;
	INT16U cw_warningBG;
	INT16S page=0,pageno=0;
	INT8S end=0;
    INT8S pause=0,codeflag=0;
	INT16S vd_time,vd_cur;	

	user_defined_video_codec_entrance();
	video_decode_entrance();
	
	spMalloc(1600+500+200);
	LoadData_1(1,SpriteCellData,"Pororo\\video\\SpriteCell_cmp.BIN");
	INIT_VideoPub(700);
	Palette_ram_cpy( 1,0, M9_SPRITE_Palette1+256*1	,256);	
	Palette_ram_cpy( 3,0, M08_SPRITE_Palette1+256*1	,256);
	LoadData_1(1,SpriteCellData+1600*_1K,"Pororo\\M9\\SpriteCell_cmp2.BIN");

	Word_Kb_Num=2100;
	Words_max=128;
	Palette_ram_cpy( 2,240, words_SPRITE_Palette1	,16);
	LoadData_1(0,SpriteCellData+Word_Kb_Num*_1K,"Pororo\\words\\SpriteCell.bin");
	Init_image_scale(320,240,
					320,240,
					SCALER_CTRL_IN_YUYV,SCALER_CTRL_OUT_RGB565,// SCALER_CTRL_OUT_RGB565
					SensorBuff1,ScalerBuff);

	Get_Rtc_Data();
	year=Year;
	month=Month;
	date=Day;
	week=(Week+35+1-Day)%7;
	days=getMonthDate(month,year);

	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	clear_sprite(Spritebuf_number);
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[MENU_OPEN_SFX], 64, 127, SFX_CHANL);													   	    	   	      		
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);

//	if(OS_game==150) {
//		OS_game=15;
//		Game_State=20;
//	}else Game_State=0;
	M9MenuIdx=0;	
	CursorStatus=0;
	
	MailPaperIdx=0;	
	
	Game_State=OPEN_ANIM;
//	set_sprite_display_init(0,0,0,Y_8x8_Frame[0]);	
//	LoadTextBmp(C_PPU_TEXT1,"Pororo\\M9\\M9_calender.gif");	
	while(1)
	{
		switch(Game_State){
//--------------------------------------------------tile
			case 0:
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,0,0,m9_Open_Frame[0]);
				set_sprite_nCharNum(0,1600);
				game_time=0;
				Game_State++;
			break;
			case 1:
				#define Tile_Time	3
				if(game_time%Tile_Time==0 && game_time<6*Tile_Time){
					set_sprite_display_init(0,0,0,m9_Open_Frame[game_time/Tile_Time]);
				}else if(game_time>=Tile_Time*6+10){
					clear_sprite(Spritebuf_number);
					LoadData_1(1,SpriteCellData+1600*_1K,"Pororo\\m9\\SpriteCell_cmp.BIN");
					LoadTextBmp(C_PPU_TEXT1,M9MenuBGGIF[M9MenuIdx]);
					Game_State++;
				}
			break;

  	    	case 2:		//设置主菜单背景
  	    		clear_sprite(Spritebuf_number);
				LoadTextBmp(C_PPU_TEXT1,M9MenuBGGIF[M9MenuIdx]);		 								
   	    		Game_State++;
   	    	break;				  			
   	    	case 3:		//主菜单操作
				if(KeyBoardCode==K_Down){
					if(M9MenuIdx<2){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						M9MenuIdx++;
						LoadTextBmp(C_PPU_TEXT1,M9MenuBGGIF[M9MenuIdx]);		 								
					}
				}else if(KeyBoardCode==K_Up){
					if(M9MenuIdx>0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						M9MenuIdx--;
						LoadTextBmp(C_PPU_TEXT1,M9MenuBGGIF[M9MenuIdx]);		 								
					}

				}else if(KeyBoardCode==K_Ok){		
					Game_State=9;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					Game_State=0xff;					
					OS_game=0;				
				}
   	    	break;

//-----------------------------------Write a Letter----------------------------------------
			case 9:
				if(SD_Mount!=1){
					ret=Show_Warning(200,13);
					if(ret==0) Game_State=2;
					else if(ret==1)Game_State=2;
					else Game_State=100;
				}else{
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);													
					if(M9MenuIdx==1) Game_State=SHOW_PHOTO+10;
					else Game_State=10+M9MenuIdx*30;
				}
   	    	break;

//-----------------------------------Write a Letter----------------------------------------
			case WRITELETTER:
				Disk_Type=FS_NAND2;
				Chdir_Root("M2_photo");

				LoadTextBmp(C_PPU_TEXT1,"Pororo\\M9\\M9_1_bg.gif");				
				set_sprite_display_init(ARROW_SP,8,120-16,photo_buttton_Frame[0]);				
				set_sprite_display_init(ARROW_SP+1,284,120-16,photo_buttton_Frame[1]);								
				MailPaperIdx=0;
				LoadNoteBookIcon(120,180,M7MailPaper[MailPaperIdx]);								
				Game_State++;			
			break;		
			case WRITELETTER+1:			
				if(KeyBoardCode==K_Right){
					if(MailPaperIdx<7){
						MailPaperIdx++;
					}else MailPaperIdx=0;
					SetMail_paper(1);
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
					LoadNoteBookIcon(120,180,M7MailPaper[MailPaperIdx]);								
				}else if(KeyBoardCode==K_Left){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
					if(MailPaperIdx>0){
						MailPaperIdx--;
					}else MailPaperIdx=7;
					SetMail_paper(0);
					LoadNoteBookIcon(120,180,M7MailPaper[MailPaperIdx]);			

				}else if(KeyBoardCode==K_Ok){		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);													

					Game_State++;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					//ClearSomeSp(YEAR_SP,ICON_SP);					
					Game_State=DIS_MENU_BG;					
				}			
			break;	
			case WRITELETTER+2:
				D_pic=0;				
				Game_State++;
				Keyboard=1;
				InitABC();
				writeing_max=126;
				for(i=0;i<writeing_max;i++) writeing_Words[i]=0;
			break;	
			case WRITELETTER+3:
				ScrollBarPos=0;	
				SendReady=0;
				CursorLine=0;CursorColumn=0;
				CursorXpos=0;
				Words_pos=0;
				CursorYpos=174+32*CursorLine-120*ScrollBarPos;
				
				clear_sprite(Spritebuf_number);
				gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0, 120*ScrollBarPos);
				LoadNoteBookTextBmp(C_PPU_TEXT1,320,480,M7MailPaper[MailPaperIdx]);
				if(D_pic) showThum_M9(File_No);
				gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0, 120*ScrollBarPos);
				Copy32(ScalerBuff,TVOutBuff,FRAME_SIZE);
				
				//set_sprite_display_init(FRAME_SP,83,52-120*ScrollBarPos,M9Frame_Frame[0]);
				set_sprite_display_init(SCROLLBAR_SP+1,298,31+ScrollBarPos*46,M08Scroll_Frame[0]);
				set_sprite_display_init(SCROLLBAR_SP,296,13,M08Scroll_Frame[1]);				
				set_sprite_nCharNum(SCROLLBAR_SP,1600/2);
				set_sprite_nCharNum(SCROLLBAR_SP+1,1600);
				set_sprite_display_init(SCROLLBAR_SP+1,298,31+ScrollBarPos*46,M08Scroll_Frame[0]);
				set_sprite_nCharNum(SCROLLBAR_SP+1,1600);
				show_M7date(year,month,date);
				Game_State++;
			break;
			case WRITELETTER+4:			
				gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0, 120*ScrollBarPos);
				show_writing_m9(30,ScrollBarPos);
				if(CursorLine==8){
					set_sprite_display_init(BUTTON_SP,101,196+4+240-120*ScrollBarPos,M08Button_Frame[3]);																						
					set_sprite_nCharNum(BUTTON_SP,1600/2);
				}else{
					set_sprite_display_init(BUTTON_SP,101,196+4+240-120*ScrollBarPos,M08Button_Frame[2]);																																				
					set_sprite_nCharNum(BUTTON_SP,1600/2);
				}
				set_sprite_display_init(SCROLLBAR_SP+1,298,31+ScrollBarPos*46,M08Scroll_Frame[0]);

				if(ScrollBarPos && CursorLine>=0 && CursorLine <=7 ){
					if(game_time%5==0) CursorStatus^=1;
					CursorLine=Words_pos/16;
					if(CursorLine>=3){
						if(ScrollBarPos==1){
							SaveMail_Buff(CURSOR_SP,170,50,190);
							ScrollBarPos=2;
						}
					}else if(ScrollBarPos==2) {
						ScrollBarPos=1;
					}
					if(CursorStatus==1){
						CursorXpos=30+(Words_pos%16)*16;
						if(abc) CursorXpos+=16;
						if(CursorLine>=3)CursorYpos=178+32*CursorLine-120*ScrollBarPos;
						else CursorYpos=174+32*CursorLine-120*ScrollBarPos;
						set_sprite_display_init(CURSOR_SP,CursorXpos,CursorYpos,M08Cursor_Frame[0]);
						set_sprite_nCharNum(CURSOR_SP,1600*2);
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
				}else if(ScrollBarPos==0) {
					set_sprite_display_init(CURSOR_SP,83-1,52,photo_ui_Frame[4]);
					set_sprite_nCharNum(CURSOR_SP,0);
				}else set_sprite_disable(CURSOR_SP);
			
				if(KeyBoardCode==K_Down || KeyBoardCode==K_Enter){
					write_abc();
					if(ScrollBarPos==0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						SaveMail_Buff(CURSOR_SP,0,0,190);
						ScrollBarPos++;
					}else if(CursorLine<7){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						Words_pos=(CursorLine+1)*16;
					}else if(CursorLine==7){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);
						CursorLine++;
					}
				}else if(ScrollBarPos>0 && KeyBoardCode==K_Up){
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
						ScrollBarPos=0;						
					}				

				}else if(KeyBoardCode==K_Ok){	
					write_abc();
					if(ScrollBarPos==0){	
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);
						saveType=0;
						Game_State=SELECT_PHOTO;
					}else if(CursorLine==8){
						SaveMail_Buff(CURSOR_SP,240,0,190);
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);
						Game_State=SAVE_WARNING;
					}else {
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						if(ScrollBarPos==1) SaveMail_Buff(CURSOR_SP,170,50,190);
						CursorLine=8;
						ScrollBarPos=2;
					}
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					write_abc();
					Game_State=CW_WARNING;					
				}
				if(ScrollBarPos==0) show_M7date(year,month,date);
				else ClearSomeSp(YEAR_SP,DAY_SP+2);					
				
			break;	
//----------------------------------------------------------			
			case CW_WARNING:
				ClearSomeSp(30,160);
				ret=Show_Warning(200,0);
				if(ret==0){
					Game_State=WRITELETTER+4;
				}else if(ret==1){
					Game_State=DIS_MENU_BG;
				}else if(ret==-1) Game_State=0xff;
			break;

			case SAVE_WARNING:			//保存提醒
				ret=Show_Warning(200,14);
				if(ret==0){
					Game_State=WRITELETTER+4;
				}else if(ret==1){
					if(Save_Lettle()==0){
						clear_sprite(Spritebuf_number);
						LoadTextBmp(C_PPU_TEXT1,M7SendAnimGif[0]);
						game_time=0;
						Game_State++;
						SendAnimIdx=0;
					}else Game_State=WRITELETTER+4;
				}else if(ret==-1) Game_State=0xff;
			break;
			case SAVE_WARNING+1:
				game_time++;
				if(game_time%3==0){
					SendAnimIdx++;
//					if(SendAnimIdx==1) SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[WARNING_SFX], 64, 127, SFX_CHANL);
					if(SendAnimIdx>=14) {
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[BLING_SFX], 64, 127, SFX_CHANL);	
						LoadTextBmp(C_PPU_TEXT1,"Pororo\\M07\\send\\M7_mail_sended.gif");					
						game_time=0;
						Game_State++;
					}else{
						LoadTextBmp(C_PPU_TEXT1,M7SendAnimGif[SendAnimIdx%7]);
						gpio_write_io(T430_RESET_PIN, DATA_HIGH);
						//tft_tft_en_set(TRUE); 

					}	
				}	
			break;
			case SAVE_WARNING+2:
				if(KeyBoardCode==K_Ok){		
					//SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);													
					//Game_State=DIS_MENU_BG;					
				}else if(game_time>25){								
					Game_State=DIS_MENU_BG;					
				}
			break;									
//================================================================
			case SELECT_PHOTO:
				clear_sprite(Spritebuf_number);
				m=saveType;
				LoadTextBmp(C_PPU_TEXT1,m3_Names[10+m]);
				Game_State=SELECT_PHOTO+6;
			break;
			case SELECT_PHOTO+6:
				if(KeyBoardCode==K_Ok){
					saveType=m;
					Game_State=SELECT_PHOTO+7;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					Game_State=WRITELETTER+3;
					File_No=-1;
				}else if(KeyBoardCode==K_Up || KeyBoardCode==K_Down){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					m=(m+1)%2;
					LoadTextBmp(C_PPU_TEXT1,m3_Names[10+m]);
				}
			break;
			case SELECT_PHOTO+7:
				if(saveType==1) Disk_Type=FS_SD;
				else Disk_Type=FS_NAND2;
				if((Disk_Type==2&&SD_Mount!=1)){
					Game_State=SELECT_PHOTO+9;
					break;
				}else{
					if(Disk_Type==FS_SD) Chdir_Root("mimicard_photo");
					else Chdir_Root("M2_photo");
				}
				game_time=0;
				if(Disk_Type==FS_SD) get_photo_list(3,4);
				else get_photo_list(0,4);
				set_sprite_disable(200);
				pageno=page=0;
				if(File_Max==0) {
					Game_State=SELECT_PHOTO+8;
					break;
				}else Game_State=SELECT_PHOTO+1;
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
			break;
			case SHOW_PHOTO+10:
				game_time=0;
				Disk_Type=FS_SD;
				Chdir_Root("mimicard_photo");
				get_photo_list(3,4);
				pageno=page=0;
				if(File_Max==0) Game_State=SHOW_PHOTO+18;
				else Game_State++;
			break;
			case SHOW_PHOTO+11:
			case SELECT_PHOTO+1:
				clear_sprite(Spritebuf_number);
				//LoadTextBmp(C_PPU_TEXT1,"AnglePhone\\m06\\M6_1_02_layout.gif");
				show_color_RGB565_bg(255,255,255);
				set_sprite_display_init(0,0,0,photo_ui_Frame[3]);
				showPhotoThum(page);
				Game_State++;
			break;
			case SHOW_PHOTO+12:
			case SELECT_PHOTO+2:
				if(Disk_Type==2 && SD_Mount!=1){
					if(Game_State==SELECT_PHOTO+2) Game_State=SELECT_PHOTO;
					else Game_State=2;
					break;
				}
				if(pageno+page*4>=File_Max) pageno=File_Max-1-page*4;
				set_sprite_display_init(1,5+pageno%2*155,9+pageno/2*111,photo_ui_Frame[4]);
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					File_No=page*4+pageno;
					if(Game_State==SHOW_PHOTO+12){
		 				Game_State=50;
						break;
					}
					Get_file_info(File_No,pFileList,File_Max);
					if(Photo_decode_info((char *)file_info.f_name,320,240)==480){
						ret=Show_Warning(200,16+8);
						if(ret==-1) Game_State=0xff;
						break;
					}
					D_pic=1;
					Game_State=WRITELETTER+3;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(Game_State==SELECT_PHOTO+2) Game_State=SELECT_PHOTO;
					else Game_State=2;
					File_No=-1;
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
			case SELECT_PHOTO+8:
			case SHOW_PHOTO+18:
				ret=Show_Warning(200,9);
				if(ret==-1) Game_State=0xff;
				else if(Game_State==SELECT_PHOTO+8)Game_State=SELECT_PHOTO;
				else Game_State=2;
				File_No=-1;
			break;
			case SELECT_PHOTO+9:
				ret=Show_Warning(200,13);
				if(ret==0) Game_State=SELECT_PHOTO;
				else if(ret==1)Game_State=SELECT_PHOTO;
				else Game_State=0xff;
				File_No=-1;
			break;
//------------------------------------------------------
			case 50:
				clear_sprite(Spritebuf_number);
				Get_file_info(File_No,pFileList,File_Max);
				InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);
				set_sprite_display_init(1,8,104,photo_buttton_Frame[0]);
				set_sprite_display_init(2,284,104,photo_buttton_Frame[1]);
				show_pagenum(File_No,File_Max);
				if(Photo_decode_to_buff((char *)file_info.f_name,320,240,TVOutBuff)>=0){
					DBG_PRINT("decode jpg OK \r\n");
					//Turn_PhotoH(ScalerBuff);
				}
				//if(play) ClearSomeSp(0,9);
				ScrollBarPos=0;
				gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0,0);
				Game_State++;
			break;
			case 51:
				if(Disk_Type==2 && SD_Mount!=1){
					Game_State=2;
					break;
				}
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					page=File_No/4;
					pageno=File_No%4;
					Game_State=SHOW_PHOTO+11;
				}else if(File_Max>1 && KeyBoardCode==K_Left){	//<<
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					File_No=(File_No+File_Max-1)%File_Max;
					Game_State=50;
				}else if(File_Max>1 && KeyBoardCode==K_Right){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					File_No=(File_No+1)%File_Max;
					Game_State=50;
				}else if(height_Two==480 && KeyBoardCode==K_Up && ScrollBarPos){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					ScrollBarPos--;
					gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0,120*ScrollBarPos);
				}else if(height_Two==480 && KeyBoardCode==K_Down && ScrollBarPos<2){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					ScrollBarPos++;
					gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0,120*ScrollBarPos);
				}else if( KeyBoardCode==K_Del){
					Game_State=55;
				}
			break;
			case 55:		//del
				ret=Show_Warning(200,2);
				if(ret==1){
					unlink((char *)file_info.f_name);
					File_Max=Del_One_file(File_No,pFileList,File_Max);
					File_SaveMax=File_Max;
					Act_SaveNums();
					if(File_Max%4==0) File_Page=File_Max/4;
					else File_Page=1+File_Max/4;
					Game_State=155;
					m=0;
				}else if(ret==0){
					Game_State=51;
				}else Game_State=0xff;
			break;
			case 155:		//del
				m++;
				set_sprite_display_init(100,0,0,photo_delete_Frame[m/3%4]);
				if(m>30){
					if(File_Max==0){
						Game_State=SHOW_PHOTO+18;
					}else{
						File_No=File_No%File_Max;
						Game_State=50;
					}
				}			
			break;
//------------------------------------------------------video play in SD
			case 70:
				VideoThum=0;
				Decora_Video=0;
				clear_sprite(Spritebuf_number);
				Disk_Type=FS_SD;
				game_time=0;
				get_photo_list(5,4);
				set_sprite_disable(200);
				pageno=page=0;
				if(File_Max==0) Game_State=78;
				else Game_State++;
			break;
			case 71:
				clear_sprite(Spritebuf_number);
				//LoadTextBmp(C_PPU_TEXT1,"AnglePhone\\m06\\M6_1_02_layout.gif");
				show_color_RGB565_bg(255,255,255);
				set_sprite_display_init(0,0,0,photo_ui_Frame[3]);
				showVideoThum(page);
				Game_State++;
			break;
			case 72:
				if(Disk_Type==2 && SD_Mount!=1){
					Game_State=2;
					break;
				}
				if(pageno+page*4>=File_Max) pageno=File_Max-1-page*4;
				set_sprite_display_init(1,6+pageno%2*155,9+pageno/2*111,photo_ui_Frame[4]);
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					Game_State=75;
					File_No=page*4+pageno;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					Game_State=2;
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

			case 78:
				ret=Show_Warning(200,10);
				if(ret==0) Game_State=2;
				else if(ret==1)Game_State=2;
				else Game_State=100;
			break;
//---------------------------------------------
			case 75:
				clear_sprite(Spritebuf_number);
				SPU_StopChannel(MUSIC_CHCHANL);
				img_board=0;
				Decora_Video=0;
				ret=Get_file_info(File_No,pFileList,File_Max);
				if(file_info.f_name[6]<'a'){
					InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,3);//BMP_RGB565
					VideoThum=0;
					ret=Get_video_Info(0);
				}else{
					img_board=11+(file_info.f_name[6]-'a');
					InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);//BMP_RGB565
					VideoThum=1;
					ret=Get_video_Info(0);
					if(img_board) PlayPhoto_Frame(img_board,Theme_Frame[img_board-11]);
				}

				set_sprite_display_init(50,120,80,photo_buttton_Frame[2]);
				set_sprite_display_init(1,8,104,photo_buttton_Frame[0]);
				set_sprite_display_init(2,284,104,photo_buttton_Frame[1]);
				show_pagenum(File_No,File_Max);
				
				vd_time=video_information.TotalDuration;
				//codeflag=1;
				Decora_Video=0;
				VideoThum=0;
				Video_Yuv=0;
				end=pause=1;
				Game_State++;
			break;

			case 76:
				if(Disk_Type==2 && SD_Mount!=1){
					Game_State=2;
					break;
				}
				if(img_board==0) Show_CodeTime(0,vd_time);
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					page=File_No/4;
					pageno=File_No%4;
					Game_State=71;
				}
				if(KeyBoardCode==K_Ok || KeyBoardCode==K_Play){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					codeflag=1;
					pause=0;
					end=0;
					Game_State++;
					game_time=0;
					clear_sprite(Spritebuf_number);
				}else if(KeyBoardCode==K_Left || KeyBoardCode==K_LastVideo){	//<<
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					File_No=(File_No+File_Max-1)%File_Max;
					Game_State=75;
				}else if(KeyBoardCode==K_Right || KeyBoardCode==K_NextVideo){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					File_No=(File_No+1)%File_Max;
					Game_State=75;
				}else if( KeyBoardCode==K_Del){
					Game_State=79;
				}
			break;
			case 77:
				if(Disk_Type==2 && SD_Mount!=1){
					Game_State=2;
					break;
				}
				vd_cur=video_decode_get_current_time();
				if(img_board==0) Show_CodeTime(0,vd_cur);
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					codeflag=2;
					Game_State=75;
					break;
				}else if(KeyBoardCode==K_Left || KeyBoardCode==K_LastVideo){	//<<
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					codeflag=2;
					//File_No=(File_No+File_Max-1)%File_Max;
					Game_State=75;
					break;
				}else if(KeyBoardCode==K_Right || KeyBoardCode==K_NextVideo){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					codeflag=2;
					File_No=(File_No+1)%File_Max;
					Game_State=75;
					break;				
				}else if(end==1){
					//File_No=(File_No+1)%File_Max;
					Game_State=75;
					break;
				}
				if(pause==0){
					sleeptime=WAITINGTIME;
					//if(game_time<20) set_sprite_display_init(50,120,80,photo_buttton_Frame[5]);
					//else 
					set_sprite_display_init(50,120,80,0);
					if(KeyBoardCode==K_Ok || KeyBoardCode==K_Play){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
						SPU_StopChannel(MUSIC_CHCHANL);
						codeflag=3;
						pause=1;
					}
					break;
				}else{
					if(end==0)set_sprite_display_init(50,120,80,photo_buttton_Frame[5]);
					else set_sprite_display_init(50,120,80,photo_buttton_Frame[2]);
					if(KeyBoardCode==K_Ok || KeyBoardCode==K_Play){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
						codeflag=3;
						pause=0;
					}
				}
			break;

			case 79:		//del
				ret=Show_Warning(100,2);
				if(ret==1){
					unlink((char *)file_info.f_name);
					File_Max=Del_One_file(File_No,pFileList,File_Max);
					File_SaveMax=File_Max;
					Act_SaveNums();
					if(File_Max%4==0) File_Page=File_Max/4;
					else File_Page=1+File_Max/4;
					Game_State=179;
					m=0;
				}else if(ret==0) Game_State=76;
				else Game_State=0xff;
			break;
			case 179:		//del
				m++;
				set_sprite_display_init(100,0,0,photo_delete_Frame[m/3%4]);
				if(m>30){
					if(File_Max==0){
						Game_State=78;
					}else{
						File_No=File_No%File_Max;
						Game_State=75;
					}
				}			
			break;
		}
//---------------------------------------------------------------------------
		if(codeflag==1)                // Start to decode the specified file 
		{
			if(video_decode_status() == VIDEO_CODEC_PROCESSING)
				video_decode_stop();

			ret=Get_video_Info(1);
			if(ret==-1){
				codeflag=0;
				continue;
			}
			vd_time=video_information.TotalDuration;
			if(Video_First==0){
				audio_decode_volume(0);
				Video_First++;
				PaintTFT_go();PaintTFT_go();PaintTFT_go();
			}else audio_decode_volume(63*volum/3);
//------------------------------------------------------
		}else if(codeflag==2){         // Stop 
			if(VIDEO_PROCESSING())
			{
				video_decode_stop();
			}
		}else if(codeflag==3){         // pause & resume
			if(video_decode_status() == VIDEO_CODEC_PROCESS_PAUSE)
			{
				video_decode_resume();
			}
			else if(video_decode_status() == VIDEO_CODEC_PROCESSING)
 			{
				video_decode_pause();
			}
		}else{        				 // END
			if(video_decode_status()==VIDEO_CODEC_PROCESS_END){
				end=1;
				pause=1;
				//break;
			}
		}
		if(Video_First==1) Video_First++;
		else if(codeflag) codeflag=0;
		if(pause==0) PaintTFT_go(); 		//video_decode_FrameReady
		else PaintTFT_cell4(); 
		if(Game_State>=12 && Game_State<50){
			if(TestMenu_Key()){
				ret=Show_Warning(200,0);
				if(ret==0){
					OS_game=19;
				}else Game_State=0xff;
			}
		}else if(Goto_desktop()){
			Game_State=0xff;
		}
		if(Game_State==0xff) break;
	}
	Copy32(TVOutBuff,TV_buffer[0],FRAME_SIZE/2);
	InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);//BMP_RGB565
	video_decode_stop();
	video_decode_exit();
	InitFileSystem(DISKUSED);
}
	
