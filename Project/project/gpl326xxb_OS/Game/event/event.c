/********************************************************************
* Purpose: Event
* Author:  David Zhu
* Date:    2017.09.05
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

//==============================================================================
// Macros
//==============================================================================


//==============================================================================
// Constants
//==============================================================================
#define EVENT			0

//====卡通索引====
#define SP_START		0



const INT16U EventSoundList[]={
	9*12,7*12,5*12+12,3*12,3*12,4*12,4*12,5*12
};
const INT16S EventGiftList[]={		//-1: 没有

	-1,-1,0,1,2,3,-1,4
};

const INT16U EventFrameTotalNum[]={
	19,15,12,8,8,9,9,11
};

const char* Event_XmasAnimGif[]={
"Pororo\\event\\xmas\\SV_XMAS-1.gif",
"Pororo\\event\\xmas\\SV_XMAS-2.gif",
"Pororo\\event\\xmas\\SV_XMAS-3.gif",
"Pororo\\event\\xmas\\SV_XMAS-4.gif",
"Pororo\\event\\xmas\\SV_XMAS-5.gif",
"Pororo\\event\\xmas\\SV_XMAS-6.gif",
"Pororo\\event\\xmas\\SV_XMAS-7.gif",
"Pororo\\event\\xmas\\SV_XMAS-8.gif",
"Pororo\\event\\xmas\\SV_XMAS-9.gif",
"Pororo\\event\\xmas\\SV_XMAS-10.gif",
"Pororo\\event\\xmas\\SV_XMAS-11.gif",
"Pororo\\event\\xmas\\SV_XMAS-12.gif",
"Pororo\\event\\xmas\\SV_XMAS-13.gif",
"Pororo\\event\\xmas\\SV_XMAS-14.gif",
"Pororo\\event\\xmas\\SV_XMAS-15.gif",
"Pororo\\event\\xmas\\SV_XMAS-16.gif",
"Pororo\\event\\xmas\\SV_XMAS-17.gif",
"Pororo\\event\\xmas\\SV_XMAS-18.gif",
"Pororo\\event\\xmas\\SV_XMAS-19.gif",
};

const char* Event_ChildAnimGif[]={
"Pororo\\event\\CHILD\\SV_CHILD-1.gif",
"Pororo\\event\\CHILD\\SV_CHILD-2.gif",
"Pororo\\event\\CHILD\\SV_CHILD-3.gif",
"Pororo\\event\\CHILD\\SV_CHILD-4.gif",
"Pororo\\event\\CHILD\\SV_CHILD-5.gif",
"Pororo\\event\\CHILD\\SV_CHILD-6.gif",
"Pororo\\event\\CHILD\\SV_CHILD-7.gif",
"Pororo\\event\\CHILD\\SV_CHILD-8.gif",
"Pororo\\event\\CHILD\\SV_CHILD-9.gif",
"Pororo\\event\\CHILD\\SV_CHILD-10.gif",
"Pororo\\event\\CHILD\\SV_CHILD-11.gif",
"Pororo\\event\\CHILD\\SV_CHILD-12.gif",
"Pororo\\event\\CHILD\\SV_CHILD-13.gif",
"Pororo\\event\\CHILD\\SV_CHILD-14.gif",
"Pororo\\event\\CHILD\\SV_CHILD-15.gif",
};
const char* Event_BirthAnimGif[]={
"Pororo\\event\\BIRTH\\SV_BIRTH-1.gif",
"Pororo\\event\\BIRTH\\SV_BIRTH-2.gif",
"Pororo\\event\\BIRTH\\SV_BIRTH-3.gif",
"Pororo\\event\\BIRTH\\SV_BIRTH-4.gif",
"Pororo\\event\\BIRTH\\SV_BIRTH-5.gif",
"Pororo\\event\\BIRTH\\SV_BIRTH-6.gif",
"Pororo\\event\\BIRTH\\SV_BIRTH-7.gif",
"Pororo\\event\\BIRTH\\SV_BIRTH-8.gif",
"Pororo\\event\\BIRTH\\SV_BIRTH-9.gif",
"Pororo\\event\\BIRTH\\SV_BIRTH-10.gif",
"Pororo\\event\\BIRTH\\SV_BIRTH-11.gif",
"Pororo\\event\\BIRTH\\SV_BIRTH-12.gif",
};
const char* Event_GIFT_1AnimGif[]={
"Pororo\\event\\s1_2\\SV_GIFT_1-1.gif",
"Pororo\\event\\s1_2\\SV_GIFT_1-2.gif",
"Pororo\\event\\s1_2\\SV_GIFT_1-3.gif",
"Pororo\\event\\s1_2\\SV_GIFT_1-4.gif",
"Pororo\\event\\s1_2\\SV_GIFT_1-5.gif",
"Pororo\\event\\s1_2\\SV_GIFT_1-6.gif",
"Pororo\\event\\s1_2\\SV_GIFT_1-7.gif",
"Pororo\\event\\s1_2\\SV_GIFT_1-8.gif",
};
const char* Event_GIFT_2AnimGif[]={
"Pororo\\event\\s3_4\\SV_GIFT_2-1.gif",
"Pororo\\event\\s3_4\\SV_GIFT_2-2.gif",
"Pororo\\event\\s3_4\\SV_GIFT_2-3.gif",
"Pororo\\event\\s3_4\\SV_GIFT_2-4.gif",
"Pororo\\event\\s3_4\\SV_GIFT_2-5.gif",
"Pororo\\event\\s3_4\\SV_GIFT_2-6.gif",
"Pororo\\event\\s3_4\\SV_GIFT_2-7.gif",
"Pororo\\event\\s3_4\\SV_GIFT_2-8.gif",
"Pororo\\event\\s3_4\\SV_GIFT_2-9.gif",
};
const char* Event_GIFT_3AnimGif[]={
"Pororo\\event\\s5\\SV_GIFT_3-1.gif",
"Pororo\\event\\s5\\SV_GIFT_3-2.gif",
"Pororo\\event\\s5\\SV_GIFT_3-3.gif",
"Pororo\\event\\s5\\SV_GIFT_3-4.gif",
"Pororo\\event\\s5\\SV_GIFT_3-5.gif",
"Pororo\\event\\s5\\SV_GIFT_3-6.gif",
"Pororo\\event\\s5\\SV_GIFT_3-7.gif",
"Pororo\\event\\s5\\SV_GIFT_3-8.gif",
"Pororo\\event\\s5\\SV_GIFT_3-9.gif",
"Pororo\\event\\s5\\SV_GIFT_3-10.gif",
"Pororo\\event\\s5\\SV_GIFT_3-11.gif",
};

const char* Event_GiftAnimGif[]={
"Pororo\\event\\sv_gift\\SV_gift_01.gif",
"Pororo\\event\\sv_gift\\SV_gift_02.gif",
"Pororo\\event\\sv_gift\\SV_gift_03.gif",
"Pororo\\event\\sv_gift\\SV_gift_04.gif",
"Pororo\\event\\sv_gift\\SV_gift_05.gif",
"Pororo\\event\\sv_gift\\SV_gift_06.gif",
"Pororo\\event\\sv_gift\\SV_gift_07.gif",
"Pororo\\event\\sv_gift\\SV_gift_08.gif",
"Pororo\\event\\sv_gift\\SV_gift_09.gif",
"Pororo\\event\\sv_gift\\SV_gift_10.gif",
"Pororo\\event\\sv_gift\\SV_gift_11.gif",
"Pororo\\event\\sv_gift\\SV_gift_12.gif",
"Pororo\\event\\sv_gift\\SV_gift_13.gif",
"Pororo\\event\\sv_gift\\SV_gift_14.gif",
"Pororo\\event\\sv_gift\\SV_gift_15.gif",
"Pororo\\event\\sv_gift\\SV_gift_16.gif",
"Pororo\\event\\sv_gift\\SV_gift_17.gif",
"Pororo\\event\\sv_gift\\SV_gift_18.gif",
};

const char* Event_GiftGif[]={
"",
"",
"Pororo\\event\\S_M3_2_gift_1.gif",
"Pororo\\event\\S_M3_2_gift_2.gif",
"Pororo\\event\\S_M3_2_gift_3.gif",
"Pororo\\event\\S_M7_3_gift_1.gif",
"",
"Pororo\\event\\S_M7_3_gift_2.gif",
};



const char* EventSound_ResourceLocation[]={
"Pororo\\event\\sound\\SV_XMAS.drm",
"Pororo\\event\\sound\\SV_CHILD.drm",
"Pororo\\event\\sound\\SV_BIRTH.drm",
"Pororo\\event\\sound\\SV_GIFT1.drm",
"Pororo\\event\\sound\\SV_GIFT2.drm",
"Pororo\\event\\sound\\SV_GIFT3.drm",
"Pororo\\event\\sound\\SV_GIFT4.drm",
"Pororo\\event\\sound\\SV_GIFT5.drm",
"Pororo\\event\\sound\\SV_gift.drm",
};
//====声音索引====

#define	XMAS_SOUND 		0
#define CHILD_SOUND		1
#define BIRTH_SOUND		2
#define GIFT1_SOUND		3
#define GIFT2_SOUND		4
#define GIFT3_SOUND		5
#define GIFT4_SOUND		6
#define GIFT5_SOUND		7
#define GIFT_SOUND		8

//==============================================================================
// Global Variables
//==============================================================================
INT16S EventIdx=0;	//0: XMAS  1:CHILD DAY 2:BIRTH 3:SURP1 4:SURP2 5:SURP3 6:SURP4 7:SURP5
INT16S EventAnimFrameIdx=0;
INT16S FrameTimer=0;
INT16U EventFlag=0;
//INT16U player.GetGiftFlag=0;		//bit2: Theme 1, bit3:Theme 2, bit4:Theme 3, bit5: letter paper 1, bit7:letter paper 2

INT16S Sup_PhotoTime=60,Sup_SendTime=180,Sup_NomailTime=60;
INT16U NoMail_Day=0;

//==============================================================================
// External Declarations
//==============================================================================
void GetNoMail()
{
	INT16S mon;
	if(player.NoMailDay==0 || player.NoMailMonth==0) return;
	if(player.NoMailMonth>Month) return;
	else if(player.NoMailMonth==Month){
		if(player.NoMailDay>=Day) return;
		else NoMail_Day=Day-player.NoMailDay;
	}else{
		if(player.NoMailMonth==Month-1){
			NoMail_Day=getMonthDate(player.NoMailMonth,Year)-player.NoMailDay+Day;
		}else NoMail_Day=10;
	}
}
void Dec_EventTime()
{
	if(player.TalkPhotoTime>=20 && (player.GetGiftFlag&0x10)==0){	
		if(Sup_PhotoTime) Sup_PhotoTime--;
	}
	if(player.SendMailTime>=10 && (player.GetGiftFlag&0x20)==0){
		if(Sup_SendTime) Sup_SendTime--;
	}
	if(NoMail_Day>3 && (player.GetGiftFlag&0x80)==0 ) {
		if(Sup_NomailTime) Sup_NomailTime--;
	}
	if(Month==12 && Day==25);
	else if(player.GetGiftFlag&0x01) player.GetGiftFlag ^=1;
	if(Month==5 && Day==5);
	else if(player.GetGiftFlag&0x02) player.GetGiftFlag ^=2;
}

void GetEventFlag()
{
	if(OS_game==20) return;
	if(Month==12 && Day==25 && (player.GetGiftFlag&0x01)==0){
		EventFlag = 0x01;
		EventIdx=0;
	}else if(Month==5 && Day==5 && (player.GetGiftFlag&0x02)==0){
		EventFlag = 0x02;
		EventIdx=1;
	}else if(Month==player.PlayerBirthMonth && Day==player.PlayerBirthDay && (player.GetGiftFlag&0x04)==0){
		EventFlag = 0x04;
		EventIdx=2;
	}else if(player.PlayGameTime>=10 && (player.GetGiftFlag&0x08)==0){
		EventFlag |= 0x08;
		EventIdx=3;
	}else if(player.TalkPhotoTime>=20 && (player.GetGiftFlag&0x10)==0 ){	
		EventFlag |= 0x10;
		EventIdx=4;
	}else if(player.SendMailTime>=10 && (player.GetGiftFlag&0x20)==0 ){
		EventFlag |= 0x20;
		EventIdx=5;
	}else if(player.RecodeTime>=15 && (player.GetGiftFlag&0x40)==0){
		EventFlag |= 0x40;
		EventIdx=6;
	}else if(NoMail_Day>3 && (player.GetGiftFlag&0x80)==0 ) {
		EventFlag |= 0x80;
		EventIdx=7;
	}
	Dec_EventTime();
}

//==============================================================================
// Routines
//==============================================================================
void changeEventAnimFrame(void)
{				
	switch(EventIdx){
		case 0:
			LoadTextBmp(C_PPU_TEXT1,Event_XmasAnimGif[EventAnimFrameIdx]);								
		break;
		case 1:
			LoadTextBmp(C_PPU_TEXT1,Event_ChildAnimGif[EventAnimFrameIdx]);								
		break;
		case 2:
			LoadTextBmp(C_PPU_TEXT1,Event_BirthAnimGif[EventAnimFrameIdx]);								
		break;						
		case 3:
		case 4:
			LoadTextBmp(C_PPU_TEXT1,Event_GIFT_1AnimGif[EventAnimFrameIdx]);								
		break;
		case 5:
		case 6:
			LoadTextBmp(C_PPU_TEXT1,Event_GIFT_2AnimGif[EventAnimFrameIdx]);								
		break;
		case 7:
			LoadTextBmp(C_PPU_TEXT1,Event_GIFT_3AnimGif[EventAnimFrameIdx]);								
		break;

	}
}					

void Main_Event(void)
{
	INT16S ret,m;	
	INT16S tmp;
	INT16S i,j,k; 
	
	spMalloc(100+390+900);
	INIT_VideoPub(100+390);
	LoadData_1(1,SpriteCellData,"Pororo\\M06\\SpriteCell_cmp_Open.bin");
	LoadDRM(0,9,SpriteCellData+(100+390)*_1K,(int*)&DrmAddr[0],EventSound_ResourceLocation);															

	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
  	gplib_ppu_sprite_zoom_enable_set(ppu_register_set, 1);		// value:0=disable 1=enable
  	
 	clear_sprite(Spritebuf_number);	
  	gplib_ppu_fade_effect_set(ppu_register_set, 0x0); 
  	
 	EventFlag=0;
	Game_State=0;
	
	while(1){
		PaintTFT_cell4();

		switch(Game_State){
		
			case EVENT:
				game_time = 0;
				if(EventSoundList[EventIdx]%EventFrameTotalNum[EventIdx])
					FrameTimer=EventSoundList[EventIdx]/EventFrameTotalNum[EventIdx]+1;
				else	
					FrameTimer=EventSoundList[EventIdx]/EventFrameTotalNum[EventIdx];				
					
				EventAnimFrameIdx = 0;
				
				changeEventAnimFrame();
				Game_State++;					
   	    	break;						

   	    	case EVENT+1:
   	    		if(game_time>=EventSoundList[EventIdx]-1 && SPU_Get_SingleChannel_Status(SFX_CHANL)==0){
   	    			if(EventGiftList[EventIdx]==-1 ){
						player.GetGiftFlag |= (1<< EventIdx);
						Game_State=0xff;   	    			
   	    			}else{
   	    				Game_State++;
   	    			}
   	    		}
				if(game_time==1) SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[EventIdx], 64, 80, SFX_CHANL);
				if(game_time<EventSoundList[EventIdx]-1 && game_time % FrameTimer ==0){
					EventAnimFrameIdx++;
					changeEventAnimFrame();
				}   	    		
/*	   	    			
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);
					Game_State=0xff;					
					OS_game=0;				
				}
//*/				
   	    	break;						

   	    	case EVENT+2:	
				EventAnimFrameIdx = 0;
				game_time = 0;
				LoadTextBmp(C_PPU_TEXT1,Event_GiftAnimGif[EventAnimFrameIdx]);			   	    	
   				Game_State++;
   	    	break;
   	    	
   	    	case EVENT+3:	//播放得到礼物动画
				if(game_time==1) 	SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[8], 64, 80, SFX_CHANL);				
		   	    	
				if(game_time%2==0){
					EventAnimFrameIdx++;
				 	LoadTextBmp(C_PPU_TEXT1,Event_GiftAnimGif[EventAnimFrameIdx]);					
				} 	
				if(game_time>=18*2-1){
					LoadTextBmp(C_PPU_TEXT1,Event_GiftGif[EventIdx]);			   	    					
					player.GetGiftFlag |= (1<< EventIdx);
	   				Game_State++;					
					game_time = 0;
				}
			break;
			
   	    	case EVENT+4:					
				if(game_time==1) 	SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[BLING_SFX], 64, 80, SFX_CHANL);				   	    	
				if(game_time>=3*25-1){
					Game_State=0xff;
				}				
   	    	break;   	    	
		}
//		if(Goto_desktop()){
//			Game_State=0xff;
//		}
		if(Game_State==0xff){
			SPU_StopChannel(MUSIC_CHCHANL);		
		 	break;
		} 
   	}
   	Save_player();
}