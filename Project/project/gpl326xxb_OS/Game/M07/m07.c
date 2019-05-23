/********************************************************************
* Purpose: M7 Mail
* Author:  David Zhu
* Date:    2017.07.07
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

#include "M07Open.h"
#include "key.h"
#include "photoedit.h"
#include "driver_l1.h"
#include "m9scrollbar.h"
#include "words_sp.h"
#include "photo_buttton.h"
//==============================================================================
// Macros
//==============================================================================

//==============================================================================
// Constants
//==============================================================================
#define OPEN_ANIM		0
#define DIS_MENU_BG		1
#define MENU_SELECT		2

#define INBOX			10
#define OUTBOX			30
#define WRITEMAIL		50
#define CW_WARNING		60
#define SEND_WARNING	70

#define SELECT_PHOTO		80
#define SAVE_WARNING	90

//====卡通索引====
#define SP_START		0
#define YEAR_SP			SP_START+1
#define MONTH_SP		YEAR_SP+5
#define DAY_SP			MONTH_SP+3
#define ARROW_SP		DAY_SP+3
#define ICON_SP			ARROW_SP+3
#define SCROLLBAR_SP	ICON_SP+1
#define FRAME_SP		SCROLLBAR_SP+2
#define BUTTON_SP		FRAME_SP+2
#define CURSOR_SP		BUTTON_SP+1
//====INBOX====

//====OUTBOX====

//====WRITEMAIL====
#define LETTER_SP		30

const char *M7MenuBGGIF[]={
"Pororo\\M07\\bg\\M7_1_menu.gif",
"Pororo\\M07\\bg\\M7_2_menu.gif",
"Pororo\\M07\\bg\\M7_3_menu.gif",
};

const char *M7InboxMail[]={
"Pororo\\M07\\inbox\\inmail01.gif",
"Pororo\\M07\\inbox\\inmail02.gif",
"Pororo\\M07\\inbox\\inmail03.gif",
"Pororo\\M07\\inbox\\inmail04.gif",
"Pororo\\M07\\inbox\\inmail05.gif",
"Pororo\\M07\\inbox\\inmail06.gif",
"Pororo\\M07\\inbox\\inmail07.gif",
"Pororo\\M07\\inbox\\inmail08.gif",
"Pororo\\M07\\inbox\\inmail09.gif",
"Pororo\\M07\\inbox\\inmail10.gif",
"Pororo\\M07\\inbox\\inmail11.gif",
};

const char *M7MailPaper[]={
"Pororo\\M07\\paper\\M7_3_paper_1.gif",
"Pororo\\M07\\paper\\M7_3_paper_2.gif",
"Pororo\\M07\\paper\\M7_3_paper_3.gif",
"Pororo\\M07\\paper\\M7_3_paper_4.gif",
"Pororo\\M07\\paper\\M7_3_paper_5.gif",
"Pororo\\M07\\paper\\M7_3_paper_6.gif",
"Pororo\\M07\\paper\\S_M7_3_gift_1.gif",
"Pororo\\M07\\paper\\S_M7_3_gift_2.gif",
};

const char *M7SendAnimGif[]={
"Pororo\\M07\\send\\M7_mail_send-1.gif",
"Pororo\\M07\\send\\M7_mail_send-2.gif",
"Pororo\\M07\\send\\M7_mail_send-3.gif",
"Pororo\\M07\\send\\M7_mail_send-4.gif",
"Pororo\\M07\\send\\M7_mail_send-5.gif",
"Pororo\\M07\\send\\M7_mail_send-6.gif",
"Pororo\\M07\\send\\M7_mail_send-7.gif",

"Pororo\\M07\\send\\M7_mail_sended.gif",
};
//==============================================================================
// Global Variables
//==============================================================================
INT16S	MailinboxIdx;
INT16U	MailNuminbox;
INT16S	MailOutboxIdx;
INT16U	MailNumOutbox;

INT16S CursorXpos,CursorYpos;	
INT16S CursorLine,CursorColumn;	
INT16S CursorLineBak;
INT16U ScrollBarPos;
INT16U ScrollBarPosBak;		//用于保存

INT16U MailPaperIdx;	

INT16U InboxMailNum;
extern int D_pic;
extern const char* m3_Names[];
//==============================================================================
// External Declarations
//==============================================================================
extern INT16S year,month,week,days;
extern INT16S date;

extern const INT16U M08_SPRITE_Palette1[],video_SPRITE_Palette1[],M07_SPRITE_Palette1[];

//=======================输入法=====================

extern INT32U Word_Kb_Num;
extern INT16S Keyboard;
extern INT16S writeing_max,writeing_Words[];
extern INT16S Words_max,Words_pos;
extern INT8U ABC[6],ABC_idx;
extern INT16S abc;

extern const INT16U ALIGN4 words_SPRITE_Palette1[];

INT16U MailTime=0;
INT8S InBoxMail[5];
INT16U Mail_Year[5],Mail_Month[5],Mail_Day[5];
//==============================================================================
// Routines
//==============================================================================
void SaveMail_Date()
{
	INT16S fd,nRet;

	AddRootPath("Pororo\\user\\mail.ini");
	fd = open(rootPath, O_CREAT|O_RDWR);
	if(fd !=-1){
		nRet = write(fd,(INT32U)&InBoxMail,5);		
		nRet = write(fd,(INT32U)&Mail_Year,10);
		nRet = write(fd,(INT32U)&Mail_Month,10);
		nRet = write(fd,(INT32U)&Mail_Day,10);
	}
	nRet = close(fd);
	DrvNand_flush_allblk();
}
void LoadMail_Date()
{
	INT16S fd,nRet;

	AddRootPath("Pororo\\user\\mail.ini");
	fd = open(rootPath, O_CREAT|O_RDWR);
	if(fd !=-1){
		nRet = read(fd,(INT32U)&InBoxMail,5);		
		nRet = read(fd,(INT32U)&Mail_Year,10);
		nRet = read(fd,(INT32U)&Mail_Month,10);
		nRet = read(fd,(INT32U)&Mail_Day,10);
	}
	nRet = close(fd);
}

INT16S GetInBoxMail()
{
	INT16S i;
	
	LoadMail_Date();
	for(i=0;i<5;i++){
		if(InBoxMail[i]==-1) break;
	}
	return i;
}
INT16U MailShow=0;
void SaveInBoxMail()
{
	INT16S i;
	if(player.InBox_Day) return;
	if(MailTime==0) MailTime=60+GetRandom(60*14);
	MailTime--;
	if(MailTime<=0){
		for(i=4;i>0;i--){
			InBoxMail[i]=InBoxMail[i-1];
			Mail_Year[i]=Mail_Year[i-1];
			Mail_Month[i]=Mail_Month[i-1];
			Mail_Day[i]=Mail_Day[i-1];
		}
		player.InBox_Mail++;
		player.InBox_Day=Day;
		if(player.PlayerBirthMonth==Month && player.PlayerBirthDay==Day) InBoxMail[0]=10;
		else{
			while(1){
				InBoxMail[0]=GetRandom(10);
				for(i=1;i<5;i++){
					if(InBoxMail[0]==InBoxMail[i]) break;
				}
				if(i==5) break;
			}
		}
		Mail_Year[0]=Year;
		Mail_Month[0]=Month;
		Mail_Day[0]=Day;
		SaveMail_Date();
		if(OS_game==11 && step>=10 && step<30){
			MailShow=0;
		}else if(OS_game==12 && step>=10){
			MailShow=0;
		}else if(OS_game==16 && step>=10){
			MailShow=0;
		}else{
			MailShow=50;
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[Mail_receive_SFX], 64, 127, SFX_CHANL);
		}
	}
}


#define YEAR_SP_XPOS1	4
#define YEAR_SP_YPOS1	8
void show_M7date(INT16S Y,INT16S M,INT16S D)
{
	int i,j,w,x,y;

	set_sprite_display_init(YEAR_SP,YEAR_SP_XPOS1,YEAR_SP_YPOS1,M08Date_Num_Frame[(Y)/1000]);
	set_sprite_display_init(YEAR_SP+1,YEAR_SP_XPOS1+9,YEAR_SP_YPOS1,M08Date_Num_Frame[(Y)/100%10]);
	set_sprite_display_init(YEAR_SP+2,YEAR_SP_XPOS1+9*2,YEAR_SP_YPOS1,M08Date_Num_Frame[(Y)/10%10]);
	set_sprite_display_init(YEAR_SP+3,YEAR_SP_XPOS1+9*3,YEAR_SP_YPOS1,M08Date_Num_Frame[(Y)%10]);
	set_sprite_display_init(YEAR_SP+4,YEAR_SP_XPOS1+4+9*4,YEAR_SP_YPOS1,M08Y_M_D_Frame[0]);	

	set_sprite_display_init(MONTH_SP+0,YEAR_SP_XPOS1+4+9*4+17,YEAR_SP_YPOS1,M08Date_Num_Frame[(M)/10]);
	set_sprite_display_init(MONTH_SP+1,YEAR_SP_XPOS1+4+9*5+17,YEAR_SP_YPOS1,M08Date_Num_Frame[(M)%10]);		
	set_sprite_display_init(MONTH_SP+2,YEAR_SP_XPOS1+6+9*6+17,YEAR_SP_YPOS1,M08Y_M_D_Frame[1]);		

	set_sprite_display_init(DAY_SP+0,YEAR_SP_XPOS1+6+9*6+17+15,YEAR_SP_YPOS1,M08Date_Num_Frame[(D)/10]);
	set_sprite_display_init(DAY_SP+1,YEAR_SP_XPOS1+6+9*7+17+15,YEAR_SP_YPOS1,M08Date_Num_Frame[D%10]);		
	set_sprite_display_init(DAY_SP+2,YEAR_SP_XPOS1+8+9*8+17+15,YEAR_SP_YPOS1,M08Y_M_D_Frame[2]);		
	
	for(i=0;i<11;i++) set_sprite_nCharNum(YEAR_SP+i,1600*2);
	set_sprite_nCharNum(YEAR_SP+4,1600*4);
	set_sprite_nCharNum(MONTH_SP+2,1600*4);
	set_sprite_nCharNum(DAY_SP+2,1600*4);
}
void SetMail_paper(INT16S Inc)
{
	if(Inc==1){
		if(MailPaperIdx==6 && (player.GetGiftFlag&0x20)==0) MailPaperIdx++;
		if(MailPaperIdx==7 && (player.GetGiftFlag&0x80)==0) MailPaperIdx=0;
	}else{
		if(MailPaperIdx==7 && (player.GetGiftFlag&0x80)==0) MailPaperIdx--;
		if(MailPaperIdx==6 && (player.GetGiftFlag&0x20)==0) MailPaperIdx--;
	}
}
void show_name(INT16S pos)
{
	INT16S x=32,y=180-pos*120;

	if(player.PlayerOriName[3]) show_word(63,x+48,y,player.PlayerOriName[3]);
	else x+=8;
	if(player.PlayerOriName[2]) show_word(62,x+32,y,player.PlayerOriName[2]);
	else x+=8;
	if(player.PlayerOriName[1]) show_word(61,x+16,y,player.PlayerOriName[1]);
	else x+=8;
	if(player.PlayerOriName[0]) show_word(60,x,y,player.PlayerOriName[0]);
	
}
void Check_outbox()
{
	Disk_Type=FS_NAND2;
	Chdir_Root("Mimimail");
	MailNumOutbox=Get_NameList_Soft("Mimimail","MM_*.jpg",pFileList);
	while(MailNumOutbox>5){
		MailOutboxIdx=MailNumOutbox-1;
		Get_file_info(MailOutboxIdx,pFileList,MailNumOutbox);
		unlink((char *)file_info.f_name);
		MailNumOutbox=Del_One_file(MailOutboxIdx,pFileList,MailNumOutbox);
	}
}
void Main_M07()
{	
    INT64U  disk_size;
	INT16S i,m,ret,nRet;
	INT16S img_board=0;
	INT16S M7AnimTime;
	INT16S M7MenuIdx;		
	INT16S M7Emotion;
	INT16S M7Weather;
	INT16S send_warningBG;
	INT16S CursorStatus;	
	INT16U SendReady;
	INT16U SendAnimIdx;
	INT16U cw_warningBG;
	INT16U MailHavePhoto;
	
	INT16S page=0,pageno=0;	

	Disk_Type=FS_NAND2;	
	Chdir_Root("Mimimail");

	spMalloc(1600+500+200);
	INIT_VideoPub(700);
	LoadData_1(1,SpriteCellData,"Pororo\\video\\SpriteCell_cmp.BIN");
	Palette_ram_cpy( 1,0, M07_SPRITE_Palette1+256*0	,256);	
	Palette_ram_cpy( 3,0, M08_SPRITE_Palette1+256*1	,256);	
	LoadData_1(1,SpriteCellData+1600*_1K,"Pororo\\M07\\SpriteCell_cmp.BIN");

	Word_Kb_Num=2100;
	Words_max=128;
	Palette_ram_cpy( 2,240, words_SPRITE_Palette1	,16);
	LoadData_1(0,SpriteCellData+Word_Kb_Num*_1K,"Pororo\\words\\SpriteCell.bin");

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
//		step=20;
//	}else step=0;
	M7AnimTime=0;
	M7MenuIdx=0;	
	CursorStatus=0;

	Game_State=OPEN_ANIM;
//	set_sprite_display_init(0,0,0,Y_8x8_Frame[0]);	
//	LoadTextBmp(C_PPU_TEXT1,"Pororo\\M07\\M07_calender.gif");	
	while(1)
	{
		PaintTFT_cell4();
		switch(Game_State){
//--------------------------------------------------tile
   	    	case OPEN_ANIM:		//Open Anim
				M7AnimTime++;
				set_sprite_display_init(0,0,0,M07Open_Frame[M7AnimTime/3]);
				set_sprite_nCharNum(0,1600);
				if(M7AnimTime>=3*6){
					LoadTextBmp(C_PPU_TEXT1,M7MenuBGGIF[M7MenuIdx]);		 								
					ClearSomeSp(0,0);
					LoadData_1(1,SpriteCellData+1600*_1K,"Pororo\\m9\\SpriteCell_cmp.BIN");
					Game_State++;
				}
   	    		
   	    	break;

  	    	case DIS_MENU_BG:		//设置主菜单背景
  	    		clear_sprite(Spritebuf_number);
				LoadTextBmp(C_PPU_TEXT1,M7MenuBGGIF[M7MenuIdx]);		 								
   	    		Game_State++;
   	    	break;				  			
   	    	case MENU_SELECT:		//主菜单操作
				if(KeyBoardCode==K_Down){
					if(M7MenuIdx<2){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						M7MenuIdx++;
						LoadTextBmp(C_PPU_TEXT1,M7MenuBGGIF[M7MenuIdx]);		 								
					}
				}else if(KeyBoardCode==K_Up){
					if(M7MenuIdx>0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						M7MenuIdx--;
						LoadTextBmp(C_PPU_TEXT1,M7MenuBGGIF[M7MenuIdx]);		 								
					}				

				}else if(KeyBoardCode==K_Ok){		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);	
					if(M7MenuIdx==2) Game_State=INBOX+M7MenuIdx*20;
					else Game_State=INBOX+8+M7MenuIdx*20;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					Game_State=0xff;					
					OS_game=0;				
				}
   	    	break;

//-----------------------------------Inbox-----------------------------------------------
			case INBOX+8:
				MailinboxIdx=0;
				MailNuminbox=GetInBoxMail();
				Game_State=INBOX;
				player.InBox_Mail=0;
   	    	break;
			case INBOX:
				clear_sprite(Spritebuf_number);
				if(MailNuminbox	){
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M07\\bg\\M7_3_bg.gif");				
					set_sprite_display_init(ARROW_SP,8,120-16,photo_buttton_Frame[0]);				
					set_sprite_display_init(ARROW_SP+1,284,120-16,photo_buttton_Frame[1]);								
				
					LoadNoteBookIcon(120,180,M7InboxMail[InBoxMail[MailinboxIdx]]);
//					file_info.f_time;
					Game_State++;										
				}else {
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[WARNING_SFX], 64, 127, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M07\\bg\\M7_warning_no mail.gif");
					game_time =0;
					Game_State=INBOX+4;								
				}				
			break;
			case INBOX+1:			
				show_M7date(Mail_Year[MailinboxIdx],Mail_Month[MailinboxIdx],Mail_Day[MailinboxIdx]);
				if(KeyBoardCode==K_Right){
					if(MailinboxIdx<MailNuminbox-1) MailinboxIdx++;
					else MailinboxIdx=0;
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
			 		LoadNoteBookIcon(120,180,M7InboxMail[InBoxMail[MailinboxIdx]]);
				}else if(KeyBoardCode==K_Left){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
					if(MailinboxIdx>0) MailinboxIdx--;
					else MailinboxIdx=MailNuminbox-1;
			 		LoadNoteBookIcon(120,180,M7InboxMail[InBoxMail[MailinboxIdx]]);

				}else if(KeyBoardCode==K_Ok){		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);													

					Game_State++;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					ClearSomeSp(YEAR_SP,ICON_SP);					
					Game_State=DIS_MENU_BG;					
				}			
			
			break;
			case INBOX+2:
				ClearSomeSp(ARROW_SP,ICON_SP);
				LoadNoteBookTextBmp(C_PPU_TEXT1,320,480,M7InboxMail[InBoxMail[MailinboxIdx]]);				
				ScrollBarPos=0;
				Game_State++;
			break;					
			case INBOX+3:
				show_name(ScrollBarPos);
				set_sprite_display_init(SCROLLBAR_SP+1,298,31+ScrollBarPos*46,M08Scroll_Frame[0]);
				set_sprite_display_init(SCROLLBAR_SP,296,13,M08Scroll_Frame[1]);				
				set_sprite_nCharNum(SCROLLBAR_SP,1600/2);
				set_sprite_nCharNum(SCROLLBAR_SP+1,1600);
				if(KeyBoardCode==K_Down){
					if(ScrollBarPos<2){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						ScrollBarPos++;

						gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0, 120*ScrollBarPos);
					}
				}else if(KeyBoardCode==K_Up){
					if(ScrollBarPos>0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						ScrollBarPos--;
						gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0, 120*ScrollBarPos);
					}				

				}else if(KeyBoardCode==K_Ok){		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);													
					ClearSomeSp(YEAR_SP,SCROLLBAR_SP+1);					
					Game_State=INBOX;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					ClearSomeSp(YEAR_SP,SCROLLBAR_SP+1);					
					Game_State=INBOX;					
				}
				if(ScrollBarPos==0) show_M7date(Mail_Year[MailinboxIdx],Mail_Month[MailinboxIdx],Mail_Day[MailinboxIdx]);
				else ClearSomeSp(YEAR_SP,DAY_SP+2);					
			
			break;					
			case INBOX+4:
			
				if(game_time>50) Game_State=DIS_MENU_BG;

			break;					
			
//-----------------------------------Outbox-----------------------------------------------			
			case OUTBOX+8:
				Check_outbox();
				MailOutboxIdx=0;
				Game_State=OUTBOX;
			break;					
			case OUTBOX:
				if(MailNumOutbox){
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M07\\bg\\M7_3_bg.gif");				
					set_sprite_display_init(ARROW_SP,8,120-16,photo_buttton_Frame[0]);				
					set_sprite_display_init(ARROW_SP+1,284,120-16,photo_buttton_Frame[1]);								
			 		Get_file_info(MailOutboxIdx,pFileList,MailNumOutbox);
					LoadNoteBookIconRel(120,180,(char *)file_info.f_name);
//					file_info.f_time;
					//show_M7date();
					Game_State++;									

				}else {
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[WARNING_SFX], 64, 127, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M07\\bg\\M7_warning_no mail.gif");
					game_time =0;
					Game_State=INBOX+4;								
				}	
			break;		
			case OUTBOX+1:			
				if(KeyBoardCode==K_Right){
					if(MailOutboxIdx<MailNumOutbox-1) MailOutboxIdx++;
					else MailOutboxIdx=0;
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
			 		Get_file_info(MailOutboxIdx,pFileList,MailNumOutbox);
					LoadNoteBookIconRel(120,180,(char *)file_info.f_name);
				}else if(KeyBoardCode==K_Left){
					if(MailOutboxIdx>0) MailOutboxIdx--;
					else MailOutboxIdx=MailNumOutbox-1;
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
			 		Get_file_info(MailOutboxIdx,pFileList,MailNumOutbox);
					LoadNoteBookIconRel(120,180,(char *)file_info.f_name);

				}else if(KeyBoardCode==K_Ok){		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);													

					Game_State++;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					ClearSomeSp(YEAR_SP,ICON_SP);					
					Game_State=DIS_MENU_BG;					
				}			
			break;	
			case OUTBOX+2:
				ClearSomeSp(ARROW_SP,ICON_SP);
				LoadNoteBookTextBmp_Rel(C_PPU_TEXT1,320,480,(char *)file_info.f_name);								
				ScrollBarPos=0;
				set_sprite_display_init(SCROLLBAR_SP+1,298,31+ScrollBarPos*46,M08Scroll_Frame[0]);
				set_sprite_display_init(SCROLLBAR_SP,296,13,M08Scroll_Frame[1]);				
				set_sprite_nCharNum(SCROLLBAR_SP,1600/2);
				set_sprite_nCharNum(SCROLLBAR_SP+1,1600);
				Game_State++;
			break;					
			case OUTBOX+3:
				if(KeyBoardCode==K_Down){
					if(ScrollBarPos<2){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						ScrollBarPos++;

						gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0, 120*ScrollBarPos);
						set_sprite_display_init(SCROLLBAR_SP+1,298,31+ScrollBarPos*46,M08Scroll_Frame[0]);				
					}
				}else if(KeyBoardCode==K_Up){
					if(ScrollBarPos>0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
						ScrollBarPos--;
						gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0, 120*ScrollBarPos);
						set_sprite_display_init(SCROLLBAR_SP+1,298,31+ScrollBarPos*46,M08Scroll_Frame[0]);				
					}				

				}else if(KeyBoardCode==K_Ok){		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);													
					ClearSomeSp(YEAR_SP,SCROLLBAR_SP+1);					
					Game_State=OUTBOX;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					ClearSomeSp(YEAR_SP,SCROLLBAR_SP+1);					
					Game_State=OUTBOX;					
				}
				//if(ScrollBarPos==0) show_M7date();							
				//else ClearSomeSp(YEAR_SP,DAY_SP+2);					
			
			break;					
			case OUTBOX+4:
				if(game_time>50) Game_State=2;
			break;				
			
//-----------------------------------Write a Mail-----------------------------------------------										
			case WRITEMAIL:
				Disk_Type=FS_NAND2;
				Chdir_Root("M2_photo");

				LoadTextBmp(C_PPU_TEXT1,"Pororo\\M9\\M9_1_bg.gif");				
				set_sprite_display_init(ARROW_SP,8,120-16,photo_buttton_Frame[0]);				
				set_sprite_display_init(ARROW_SP+1,284,120-16,photo_buttton_Frame[1]);								
				MailPaperIdx=0;
				LoadNoteBookIcon(120,180,M7MailPaper[MailPaperIdx]);								
				Game_State++;			
			break;		
			case WRITEMAIL+1:			
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
			case WRITEMAIL+2:
				D_pic=0;				
				Game_State++;
				Keyboard=1;
				InitABC();
				writeing_max=126;
				for(i=0;i<writeing_max;i++) writeing_Words[i]=0;
			break;
			case WRITEMAIL+3:
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
			case WRITEMAIL+4:			
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
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);
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
//===================================================			
			case CW_WARNING:
				ClearSomeSp(30,160);
				ret=Show_Warning(200,0);
				if(ret==0){
					Game_State=WRITEMAIL+4;
				}else if(ret==1){
					Game_State=DIS_MENU_BG;
				}else if(ret==-1) Game_State=0xff;
			break;

			case SAVE_WARNING:			//保存提醒
				ret=Show_Warning(200,14);
				if(ret==0){
					Game_State=WRITEMAIL+4;
				}else if(ret==1){
					if(Save_Lettle()==0){
						Check_outbox();
						player.SendMailTime++;
						player.NoMailMonth=Month;
						player.NoMailDay=Day;
						clear_sprite(Spritebuf_number);
						LoadTextBmp(C_PPU_TEXT1,M7SendAnimGif[0]);
						game_time=0;
						Game_State++;
						SendAnimIdx=0;
					}else Game_State=WRITEMAIL+4;
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

//----------------------------------------------------------------------------------
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
					Game_State=WRITEMAIL+3;
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
				if(File_Max==0){
					Game_State=SELECT_PHOTO+8;
					break;
				}else Game_State=SELECT_PHOTO+1;
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
			break;
			case SELECT_PHOTO+1:
				clear_sprite(Spritebuf_number);
				//LoadTextBmp(C_PPU_TEXT1,"AnglePhone\\m06\\M6_1_02_layout.gif");
				show_color_RGB565_bg(255,255,255);
				set_sprite_display_init(0,0,0,photo_ui_Frame[3]);
				showPhotoThum(page);
				Game_State++;
			break;
			case SELECT_PHOTO+2:
				if(pageno+page*4>=File_Max) pageno=File_Max-1-page*4;
				set_sprite_display_init(1,5+pageno%2*155,9+pageno/2*111,photo_ui_Frame[4]);
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					File_No=page*4+pageno;
					Get_file_info(File_No,pFileList,File_Max);
					if(Photo_decode_info((char *)file_info.f_name,320,240)==480){
						ret=Show_Warning(200,16+8);
						if(ret==-1) Game_State=0xff;
						break;
					}
					D_pic=1;
					Game_State=WRITEMAIL+3;					
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					Game_State=SELECT_PHOTO;
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
				ret=Show_Warning(200,9);
				if(ret==-1) Game_State=0xff;
				else Game_State=SELECT_PHOTO;
				File_No=-1;
			break;
			case SELECT_PHOTO+9:
				ret=Show_Warning(200,13);
				if(ret==0) Game_State=SELECT_PHOTO;
				else if(ret==1)Game_State=SELECT_PHOTO;
				else Game_State=0xff;
				File_No=-1;
			break;

		}
		if(Game_State>=52){
			if(TestMenu_Key()){
				ret=Show_Warning(200,0);
				if(ret==0){
					OS_game=17;
				}else Game_State=0xff;
			}
		}else if(Goto_desktop()){
			Game_State=0xff;
		}
		if(Game_State==0xff) break;
	}
}
	


