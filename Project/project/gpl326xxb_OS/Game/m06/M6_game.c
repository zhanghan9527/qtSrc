/********************************************************************
* Purpose: M6 Games
* Author:  David Zhu
* Date:    2017.07.03
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

#include "M6Open.h"
#include "M6_1Cursor.h"
#include "M6Cursor.h"
#include "M6_3BearRide.h"
#include "key.h"
#include "photoedit.h"

//==============================================================================
// Macros
//==============================================================================
typedef struct {
	INT16S	kind;
	INT16S	xPos;
	INT16S  yPos;
} ITEM, *PITEM;		//

ITEM item[8];
//==============================================================================
// Constants
//==============================================================================
#define OPEN_ANIM		0
#define DIS_MENU_BG		1
#define MENU_SELECT		2

#define TYPEGAME		10
#define FILLINGWORD		30
#define KEYBOARDRACING	50


#define CATMAXSPEED		3.8
#define BEARMAXSPEED	3.8
#define MIMIMAXSPEED	4

#define TRACKTOTALLENGTH	(140+3240+120)

//====卡通索引====
#define SP_START		0
#define CURSOR_SP		SP_START+1
#define SCORE_SP		CURSOR_SP+1
//====打字游戏====
#define CIRCLE_SP		SP_START+3
#define TIME_SP			CIRCLE_SP+1
#define TEXTBAR_SP		TIME_SP+2
#define WORD_SP			TEXTBAR_SP+2
//====单词填空====
#define IMAGE_SP		SP_START+3
#define CHAR_SP			IMAGE_SP+1
#define FILLCHAR_SP		CHAR_SP+10

//====赛车====
#define BEAR_SP			SP_START+1
#define CAT_SP			BEAR_SP+1
#define MIMI_SP			CAT_SP+1
#define TIMER_SP		MIMI_SP+1
#define RANK_SP			TIMER_SP+1
#define START_SP		RANK_SP+1
#define ITEM_SP			START_SP+1

#define		VOICE_ADDR_GAME	(SFX_ADDR_GAME-200*_1K)

const char *M6MenuBGGIF[]={
"Pororo\\M06\\M6_Game1.gif",
"Pororo\\M06\\M6_Game2.gif",
"Pororo\\M06\\M6_Game3.gif",
};

const char* M6_1Sfx_ResourceLocation[]={
"Pororo\\m06\\sfx\\answer_right.drm",
"Pororo\\m06\\sfx\\answer_wrong.drm",
"Pororo\\m06\\sfx\\GAME1_bgm.drm",
"Pororo\\m06\\sfx\\TIME_OUT.drm",
"Pororo\\m06\\sfx\\score.drm",
"Pororo\\m06\\sfx\\ranking.drm",
};
//====GAME1声音索引====
#define RIGHT_SFX		0
#define WRONG_SFX		1
#define GAME1BG_MUSIC	2
#define TIMEOUT_SFX		3
#define SCORE_VOICE		4
#define RANKING_SFX		5
#define WORD_VOICE		6

const char* M6_2Sfx_ResourceLocation[]={
"Pororo\\m06\\sfx\\answer_right.drm",
"Pororo\\m06\\sfx\\answer_wrong.drm",
"Pororo\\m06\\sfx\\GAME2_bgm.drm",
"Pororo\\m06\\sfx\\next_slide.drm",
"Pororo\\m06\\sfx\\score.drm",
};
//====GAME2声音索引====
#define RIGHT_SFX		0
#define WRONG_SFX		1
#define GAME2BG_MUSIC	2
#define SLIDE_SFX		3
#define SCORE_SFX		4

#define WORD_VOICE		6


const char* M6_3Sfx_ResourceLocation[]={
"Pororo\\m06\\SFX\\engine-start.drm",
"Pororo\\m06\\SFX\\failure.drm",
"Pororo\\m06\\SFX\\GAME3_bgm.drm",
"Pororo\\m06\\SFX\\item get.drm",
"Pororo\\m06\\SFX\\lose.drm",
"Pororo\\m06\\SFX\\obstruction.drm",
"Pororo\\m06\\SFX\\obstruction2.drm",
"Pororo\\m06\\SFX\\start-beeps.drm",
"Pororo\\m06\\SFX\\success.drm",
"Pororo\\m06\\SFX\\win.drm",
"Pororo\\m06\\sfx\\next_slide.drm",
};

//====GAME3声音索引====
#define ENGINE_SFX		0
#define FAILURE_SFX		1
#define GAME3BG_MUSIC	2
#define TIEM_MUSIC		3
#define M6_3_LOSE_SFX	4
#define OBSTRUCT1_SFX	5
#define OBSTRUCT2_SFX	6
#define BEEP_SFX		7
#define SUCCESS_SFX		8
#define M6_3_WIN_SFX	9
#define NEXTSLIDE_SFX	10




//================Game 1====================
const INT16U M6_1L1WordLength[]={
	1,1,1,1,1,
	1,1,1,1,1,
	
	2,2,2,2,2,
	2,2,2,2,2,
	2,2,2,2,2,
	2,2,2,2,2,
};
const INT16U M6_1L1WordCode[]={
	0x047C,0x0000,		//1
	0x059F,0x0000,	
	0x0B52,0x0000,		
	0x0776,0x0000,	
	0x0C6C,0x0000,		
	0x07ED,0x0000,	
	0x04CD,0x0000,		
	0x0A47,0x0000,	
	0x0B85,0x0000,		
	0x0473,0x0000,			

	0x040D,0x09BC,		//11			
	0x06EF,0x08AB,		
	0x08D4,0x0776,	
	0x0956,0x06F4,		
	0x0B45,0x0a77,	
	0x03E9,0x09D2,		
	0x0729,0x0a05,	
	0x08A7,0x040F,		
	0x09B9,0x09DA,	
	0x08D2,0x0430,		
	
	0x098D,0x09F7,		//21
	0x083A,0x0BC3,		
	0x04CD,0x041C,	
	0x0440,0x06C1,		
	0x0770,0x0583,	
	0x04CB,0x0B45,		
	0x050C,0x07E6,	
	0x08F4,0x06FE,		//28
	0x0742,0x06E6,	
	0x0C8F,0x0481,		
};

const INT16U M6_1L2WordLength[]={
	3,3,3,3,3,
	3,3,3,3,3,
	3,3,3,3,3,
	3,3,5,5,5,

	4,4,4,4,4,
	4,4,4,4,4
};

const INT16U M6_1L2WordCode[]={
	0x0A7F,0x09B9,0x03FB,0x0000,0x0000,			//31
	0x0887,0x077D,0x09BC,0x0000,0x0000,	
	0x0A27,0x0CC9,0x0488,0x0000,0x0000,	
	0x05DD,0x0CC9,0x0AEB,0x0000,0x0000,	
	0x05DD,0x0982,0x0846,0x0000,0x0000,	

	0x056B,0x083A,0x0673,0x0000,0x0000,			//36
	0x0B68,0x0C79,0x0BCF,0x0000,0x0000,	
	0x0B10,0x0B81,0x0699,0x0000,0x0000,	
	0x0B4F,0x0717,0x066F,0x0000,0x0000,	
	0x0430,0x083D,0x0488,0x0000,0x0000,				

	0x0A05,0x0A27,0x040D,0x0000,0x0000,			//41
	0x0A05,0x05DD,0x0ADF,0x0000,0x0000,	
	0x03F5,0x0935,0x0A7f,0x0000,0x0000,	
	0x0BC4,0x047E,0x0488,0x0000,0x0000,	
	0x0862,0x09F9,0x0A10,0x0000,0x0000,				

	0x05EC,0x04AC,0x07E6,0x0000,0x0000,			//46
	0x0435,0x06EA,0x066F,0x0000,0x0000,		
	0x0935,0x09F7,0x08C9,0x0BAB,0x06EB,	
	0x0BAB,0x06E7,0x08C9,0x06EF,0x08C9,	
	0x0850,0x0CCC,0x0430,0x0CD5,0x0C63,				

	0x08C9,0x0B6D,0x0B45,0x07C4,0x0000,			//51
	0x0A10,0x067B,0x0D0D,0x06FA,0x0000,	
	0x0B02,0x09DB,0x05AB,0x0CD4,0x0000,	
	0x086D,0x0A47,0x05AB,0x09A2,0x0000,	
	0x061A,0x0619,0x045E,0x06E7,0x0000,	

	0x0435,0x08CE,0x05D3,0x0B45,0x0000,			//56
	0x0770,0x09F7,0x0990,0x06E9,0x0000,	
	0x04C4,0x05EC,0x03EA,0x08D2,0x0000,			//58
	0x0C24,0x09F9,0x0945,0x0C81,0x0000,	
	0x0B4F,0x0539,0x09F7,0x0878,0x0000,			//60
};
//================Game 2====================
const char* M6_2Voice_ResourceLocation[]={
"Pororo\\m06\\voice\\01_apple.drm",
"Pororo\\m06\\voice\\02_carrot.drm",
"Pororo\\m06\\voice\\03_potato.drm",
"Pororo\\m06\\voice\\04_melon.drm",
"Pororo\\m06\\voice\\05_water.drm",
"Pororo\\m06\\voice\\06_milk.drm",
"Pororo\\m06\\voice\\07_candy.drm",
"Pororo\\m06\\voice\\08_bread.drm",
"Pororo\\m06\\voice\\09_dog.drm",
"Pororo\\m06\\voice\\10_cat.drm",
"Pororo\\m06\\voice\\11_duck.drm",
"Pororo\\m06\\voice\\12_mouse.drm",
"Pororo\\m06\\voice\\13_lion.drm",
"Pororo\\m06\\voice\\14_tiger.drm",
"Pororo\\m06\\voice\\15_fox.drm",
"Pororo\\m06\\voice\\16_rabbit.drm",
"Pororo\\m06\\voice\\17_cow.drm",
"Pororo\\m06\\voice\\18_horse.drm",
"Pororo\\m06\\voice\\19_pig.drm",
"Pororo\\m06\\voice\\20_hippo.drm",
"Pororo\\m06\\voice\\21_monkey.drm",
"Pororo\\m06\\voice\\22_bear.drm",
"Pororo\\m06\\voice\\23_white.drm",
"Pororo\\m06\\voice\\24_black.drm",
"Pororo\\m06\\voice\\25_red.drm",
"Pororo\\m06\\voice\\26_yellow.drm",
"Pororo\\m06\\voice\\27_pink.drm",
"Pororo\\m06\\voice\\28_blue.drm",
"Pororo\\m06\\voice\\29_green.drm",
"Pororo\\m06\\voice\\30_brown.drm",
"Pororo\\m06\\voice\\31_gold.drm",
"Pororo\\m06\\voice\\32_desk.drm",
"Pororo\\m06\\voice\\33_bottle.drm",
"Pororo\\m06\\voice\\34_leg.drm",
"Pororo\\m06\\voice\\35_mouth.drm",
"Pororo\\m06\\voice\\36_foot.drm",
"Pororo\\m06\\voice\\37_eye.drm",
"Pororo\\m06\\voice\\38_cap.drm",
"Pororo\\m06\\voice\\39_cup.drm",
"Pororo\\m06\\voice\\40_baby.drm",
"Pororo\\m06\\voice\\41_clock.drm",
"Pororo\\m06\\voice\\42_snake.drm",
"Pororo\\m06\\voice\\43_pencil.drm",
"Pororo\\m06\\voice\\44_ruler.drm",
"Pororo\\m06\\voice\\45_turtle.drm",
"Pororo\\m06\\voice\\46_coffee.drm",
"Pororo\\m06\\voice\\47_car.drm",
"Pororo\\m06\\voice\\48_doll.drm",
"Pororo\\m06\\voice\\49_book.drm",
"Pororo\\m06\\voice\\50_ball.drm",
};

const INT16S ImageSize[]={
	96,106,	  85, 94,  119, 85,   93, 96,  120,122,   85,107,  139,101,  101,100,   84,112,  92,104,
   111,110,   95,103,  160,117,  154,116,  117, 99,   76,135,  148,116,  123,120,   86,106, 146, 98,
    89,110,   71,130,  180,111,  180,111,  180,111,  180,111,  180,111,  180,111,  180,111,	180,111,
   139, 89,  108,113,   55,117,  148,103,  130, 79,  124,116,  154, 91,  139,114,   92, 91,   84,120,
   117,117,   90,114,  142,105,  169, 75,  132, 90,  117, 93,  145, 95,  115,125,  108,118,  136,107,
};

const char *WordChar[50]={
"APPLE",
"CARROT",
"POTATO",
"MELON",
"WATER",
"MILK",
"CANDY",
"BREAD",
"DOG",
"CAT",

"DUCK",
"MOUSE",
"LION",
"TIGER",
"FOX",
"RABBIT",
"COW",
"HORSE",
"PIG",
"HIPPO",

"MONKEY",
"BEAR",
"WHITE",
"BLACK",
"RED",
"YELLOW",
"PINK",
"BLUE",
"GREEN",
"BROWN",

"GOLD",
"DESK",
"BOTTLE",
"LEG",
"MOUTH",
"FOOT",
"EYE",
"CAP",
"CUP",
"BABY",

"CLOCK",
"SNAKE",
"PENCIL",
"RULER",
"TURTLE",
"COFFEE",
"CAR",
"DOLL",
"BOOK",
"BALL",
};


const INT16S WordLength[50]={
	5,6,6,5,5,4,5,5,3,3,
	4,5,4,5,3,6,3,5,3,5,
	6,4,5,5,3,6,4,4,5,5,
	4,4,6,3,5,4,3,3,3,4,
	5,5,6,5,6,6,3,4,4,4,
};

//================Game 3====================
const char *M6_Game3ReadyGIF[]={
"Pororo\\M06\\M6_Game3_ready-1.gif",
"Pororo\\M06\\M6_Game3_ready-2.gif",
"Pororo\\M06\\M6_Game3_ready-3.gif",
"Pororo\\M06\\M6_Game3_ready-4.gif",
"Pororo\\M06\\M6_Game3_ready-5.gif",
"Pororo\\M06\\M6_Game3_ready-6.gif",
"Pororo\\M06\\M6_Game3_ready-7.gif",
};

const char *M6_Game3MiddleGIF[]={
"Pororo\\M06\\M6_Game3_middle-1.gif",
"Pororo\\M06\\M6_Game3_middle-2.gif",
"Pororo\\M06\\M6_Game3_middle-3.gif",
"Pororo\\M06\\M6_Game3_middle-4.gif",
"Pororo\\M06\\M6_Game3_middle-5.gif",
"Pororo\\M06\\M6_Game3_middle-6.gif",
};

const char *M6_Game3EndGIF[]={
"Pororo\\M06\\M6_Game3_end-1.gif",
"Pororo\\M06\\M6_Game3_end-2.gif",
"Pororo\\M06\\M6_Game3_end-3.gif",
"Pororo\\M06\\M6_Game3_end-4.gif",
"Pororo\\M06\\M6_Game3_end-5.gif",
"Pororo\\M06\\M6_Game3_end-6.gif",
};

const char *M6_Game3LoseAnim[]={
"Pororo\\M06\\M6_Game3_lose-1.gif",
"Pororo\\M06\\M6_Game3_lose-2.gif",
"Pororo\\M06\\M6_Game3_lose-3.gif",
"Pororo\\M06\\M6_Game3_lose-4.gif",
};

const char *M6_Game3WinAnim[]={
"Pororo\\M06\\M6_Game3_win-1.gif",
"Pororo\\M06\\M6_Game3_win-2.gif",
"Pororo\\M06\\M6_Game3_win-3.gif",
"Pororo\\M06\\M6_Game3_win-4.gif",
};

const INT16S M6_3ItemXOffset[]={
		7,35,
	   16,52,
	   15,60,
	   20,49,
};

//==============================================================================
// Global Variables
//==============================================================================
INT16S M6Score;
INT16S M6Level;
INT16S M6MenuIdx;
INT8U Game_StateBak;
INT16S cw_warningBG;
int game_timebak;
INT16S M6AnimTimebak;
INT16S HeartLastTimebak;


extern INT16U	EventFlag;
//=============Game 1=================================
INT16U M6_1GameTime;

INT16S RandWord[5];
INT16S WordRandIdx[30];
INT16U NowWordIdx;
INT16U M6_1WhichWord;


INT16U M6RecorderName1[3];
INT16U M6RecorderName2[3];
INT16U M6RecorderName3[3];
INT16U M6RecordScore[3];

INT16S	Word_SP_xPos[5];
INT16S	Word_SP_yPos[5];

//=============Game 2=================================				
INT16S M6AnimTime;
INT16S CursorStatus;	
INT16S M6CursorXpos,M6CursorYpos;

INT16S Mistake1;
INT16S Mistake2;	
INT16U FirstInput;

INT16S M6WhichQuestion;
INT16S WordIdx[10];	
INT16S WordCharNum; 
INT16S M6Input1;
INT16S M6Input2;

char   *WordCharPtr;
char   WordLetter0[10]="";
char   WordLetter1[10]="";	
INT16S Blank1Idx,Blank2Idx;	
INT16S WhichBlank;
//=============Game 3=================================
FP32	CatSpeed,BearSpeed,MimiSpeed;
FP32	CatPos,BearPos,MimiPos;	
FP32	CatRandSpeed,BearRandSpeed,MimiRandSpeed;		
FP32 	M6_3BGTimer;	
FP32 	Mimi_SP_Pos;	
INT16S	Cat_SP_Pos,Bear_SP_Pos;	

INT16U  CatRideStatus,BearRideStatus,MimiRideStatus;
INT16U  CatHitTimer,BearHitTimer,MimiHitTimer;

INT16U	MimiInWhichLine;

INT16U	SkipLineidx;
INT16U	LineSort[5]={2,1,0,1,2};

INT16U  MimiRank;
INT16U  M6_3MiddleCycleNum;
INT16S 	M6BGIdx;

INT16S	HeartLastTime;

//==============================================================================
// External Declarations
//==============================================================================

extern const INT16U ALIGN4 M6Open_SPRITE_Palette1[];
extern const INT16U ALIGN4 M6_1_SPRITE_Palette1[];
extern const INT16U ALIGN4 M6_2_SPRITE_Palette1[];
extern const INT16U ALIGN4 M6_3_SPRITE_Palette1[];

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

//==============================================================================
// Routines
//==============================================================================

INT16S show_M6pointline32(INT16S sp,INT16S x,INT16S y)
{
	INT16S m=0;
	if(abc){
		show_word32(sp,x,y,abc);
		sp++;
		m++;
		x+=32;		
	}
	if(game_time/10%2){
		set_sprite_display_init(sp,x,y,M6_1Cursor_Frame[0]);
	}else set_sprite_image_number(sp,0);

	return m;

}

#define Writing32_Line	7
void show_M6writing32(INT16S sp,INT16S num, INT16S LetterXPos, INT16S LetterYPos)
{

	INT16S i,x,y,m,n,k;

	x=LetterXPos;
	y=LetterYPos;
	ClearSomeSp(sp,sp+num+2);
	m=writeing_max;

	for(i=0;i<=m;i++){
		if(Words_pos==i){
			k=show_M6pointline32(sp,x,y);
			
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

void show_writtenName(INT16S sp,INT16U *Name,INT16S LetterXPos, INT16S LetterYPos)
{
	INT16S i,x,y,m,n,k;

	x=LetterXPos;
	y=LetterYPos;

	m=3;

	for(i=0;i<=m;i++){
		if(i==m) break;
		n=Name[i];
		show_word32(sp,x,y,n);
		x+=32;
		if(x>=LetterXPos+32*Writing32_Line){
			x=LetterXPos;
			y+=32;
		}
		sp++;		

	}
}
void initKoreaInput(void)
{
	int i;
	writeing_max=0;			//韩文输入法	
	for(i=0;i<Words_max;i++) writeing_Words[i]=0;  //韩文输入法	
	InitABC();
	show_M6writing32(30,Words_max,0,0);	
	Words_pos=0,Keyboard=1;		
}

void randCreateM6_1Word(void)
{
	int i,j,k,tmp;
	
	WordRandIdx[0]=GetRandom(30);
//	WordRandIdx[0]=5;
	j=1;
	do{
		tmp=GetRandom(30);
		for(k=0;k<j;k++){
			if(tmp==WordRandIdx[k]) break; 
		}
		if(k==j){
			WordRandIdx[j]=tmp;
			j++;
		}	
	}while(j<30);				
}

void createAndMoveWord(void)
{
	int i;
	int canMove;

	for(i=0;i<5;i++) {
		if(get_sprite_y(WORD_SP+i)>=150) break;
	}	
	if(i != 5 )	canMove=0;	
	else canMove=1;	

	if(canMove){
		if(game_time%2==0 ){
			for(i=0;i<5;i++) {
				set_sprite_posInc(WORD_SP+i,0,1);										
			}	
		}	
	}else{
/*	
		if((game_time/6)%2==0){
			for(i=0;i<5;i++) set_sprite_posInc(WORD_SP+i,4,0);

		}else{
			for(i=0;i<5;i++) set_sprite_posInc(WORD_SP+i,-4,0);
		}	
*/		
	}	
}	

INT16S checkInputRightOrError(void)
{

	int WordIdxTmp,i;
	
	WordIdxTmp=RandWord[NowWordIdx];	
	if(M6Level==0){
		if(writeing_max==M6_1L1WordLength[WordIdxTmp]){
			for(i=0;i<writeing_max;i++){
				if(writeing_Words[i]!=M6_1L1WordCode[WordIdxTmp*2+i]) break;
			}

			if(i==writeing_max) return 0;
			else return -1;
	
		}else return -1;
	}else{
		if(writeing_max==M6_1L2WordLength[WordIdxTmp]){
			WordIdxTmp=RandWord[NowWordIdx];
			for(i=0;i<writeing_max;i++){
				if(writeing_Words[i]!=M6_1L2WordCode[WordIdxTmp*5+i]) break;
			}

			if(i==writeing_max) return 0;
			else return -1;
	
		}else return -1;
	}	
	
}

void restoreTypeGame(void)
{					
	LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game1_bg.gif");
	set_sprite_display_init(CIRCLE_SP,320-50,2,M6_1Circle_Frame[0]);	
	if(M6Level==0){
	
		set_sprite_display_init(WORD_SP+0,Word_SP_xPos[0],Word_SP_yPos[0],M6_1L1Text_Frame[RandWord[0]]);								
		set_sprite_display_init(WORD_SP+1,Word_SP_xPos[1],Word_SP_yPos[1],M6_1L1Text_Frame[RandWord[1]]);													
		set_sprite_display_init(WORD_SP+2,Word_SP_xPos[2],Word_SP_yPos[2],M6_1L1Text_Frame[RandWord[2]]);																		
		set_sprite_display_init(WORD_SP+3,Word_SP_xPos[3],Word_SP_yPos[3],M6_1L1Text_Frame[RandWord[3]]);																							
		set_sprite_display_init(WORD_SP+4,Word_SP_xPos[4],Word_SP_yPos[4],M6_1L1Text_Frame[RandWord[4]]);																												
	}else{
		set_sprite_display_init(WORD_SP+0,Word_SP_xPos[0],Word_SP_yPos[0],M6_1L2Text_Frame[RandWord[0]]);								
		set_sprite_display_init(WORD_SP+1,Word_SP_xPos[1],Word_SP_yPos[1],M6_1L2Text_Frame[RandWord[1]]);													
		set_sprite_display_init(WORD_SP+2,Word_SP_xPos[2],Word_SP_yPos[2],M6_1L2Text_Frame[RandWord[2]]);																		
		set_sprite_display_init(WORD_SP+3,Word_SP_xPos[3],Word_SP_yPos[3],M6_1L2Text_Frame[RandWord[3]]);																							
		set_sprite_display_init(WORD_SP+4,Word_SP_xPos[4],Word_SP_yPos[4],M6_1L2Text_Frame[RandWord[4]]);											
	}	
	M6AnimTime=M6AnimTimebak;
	game_time=game_timebak;						
	Game_State=Game_StateBak;
}

void disPlayerRank(void)
{ 					
	if(MimiPos>=BearPos && MimiPos>=CatPos){					
		MimiRank=0;				
	}else if(MimiPos <BearPos && MimiPos<CatPos){
		MimiRank=2;				
	}else{
		MimiRank=1;				
	}	
		set_sprite_display_init(RANK_SP,256,0,m6_3Rank_Frame[MimiRank]);					
}

void disPlayerRider(void)
{
	if(BearSpeed){
		if(BearPos>=MimiPos){
			if(BearPos-MimiPos>320) set_sprite_disable(BEAR_SP);
			else{
			 	if(BearRideStatus==0){			
				 	set_sprite_display_init(BEAR_SP, Bear_SP_Pos, 112-51*2,M6_3BearRide_Frame[(game_time/(INT16S)(10-BearSpeed))%4]);
				}else{
					BearHitTimer++;
				 	set_sprite_display_init(BEAR_SP, Bear_SP_Pos, 112-51*2,M6_3BearCry_Frame[(BearHitTimer/12)%2]);
					if(BearHitTimer>=24)BearRideStatus=0; 				
				}			 	
			} 
		}else {
			if(MimiPos-BearPos>160) set_sprite_disable(BEAR_SP);
			else{
			 	if(BearRideStatus==0){			
				 	set_sprite_display_init(BEAR_SP, Bear_SP_Pos, 112-51*2,M6_3BearRide_Frame[(game_time/(INT16S)(10-BearSpeed))%4]);
				}else{
					BearHitTimer++;
				 	set_sprite_display_init(BEAR_SP, Bear_SP_Pos, 112-51*2,M6_3BearCry_Frame[(BearHitTimer/12)%2]);
					if(BearHitTimer>=24)BearRideStatus=0; 				
				}			 	
			}
		}
	}		
	if(CatSpeed){
		if(CatPos>=MimiPos){
			if(CatPos-MimiPos>320) set_sprite_disable(CAT_SP);
			else {
				if(CatRideStatus==0){			
				 	set_sprite_display_init(CAT_SP, Cat_SP_Pos, 112-51,M6_3CatRide_Frame[(game_time/(INT16S)(10-CatSpeed))%4]);
				}else{
					CatHitTimer++;
					set_sprite_display_init(CAT_SP, Cat_SP_Pos, 112-51,M6_3CatCry_Frame[(CatHitTimer/12)%2]);		
					if(CatHitTimer>=24)CatRideStatus=0; 				
				}
			} 
		}else {
			if(MimiPos-CatPos>160) set_sprite_disable(CAT_SP);
			else {
				if(CatRideStatus==0){			
				 	set_sprite_display_init(CAT_SP, Cat_SP_Pos, 112-51,M6_3CatRide_Frame[(game_time/(INT16S)(10-CatSpeed))%4]);
				}else{
					CatHitTimer++;
					set_sprite_display_init(CAT_SP, Cat_SP_Pos, 112-51,M6_3CatCry_Frame[(CatHitTimer/12)%2]);		
					if(CatHitTimer>=24)CatRideStatus=0; 				
				}
			}	
		}	
	}	
//	if(MimiSpeed){
		if(MimiRideStatus==0){
			set_sprite_display_init(MIMI_SP, (INT16S) Mimi_SP_Pos, 112-51*MimiInWhichLine,M6_3MiMiRide_Frame[(game_time/(INT16S)(12-MimiSpeed))%4]);
		}else {
			MimiHitTimer++;
			set_sprite_display_init(MIMI_SP, (INT16S) Mimi_SP_Pos, 112-51*MimiInWhichLine,M6_3MimiCry_Frame[(MimiHitTimer/12)%2]);		
			if(MimiHitTimer>=24)MimiRideStatus=0; 
		}	
//	}		

}				
void playMove(void)
{

	// 熊
		if(BearSpeed<BEARMAXSPEED-BearRandSpeed) BearSpeed+=0.2;
		else BearSpeed=BEARMAXSPEED-BearRandSpeed;
		BearPos+=BearSpeed;


	// 猫
		if(CatSpeed<CATMAXSPEED-CatRandSpeed) CatSpeed+=0.2;
		else CatSpeed=CATMAXSPEED-CatRandSpeed;
		CatPos+=CatSpeed;
		
	// Mimi					
		if(KeyBoardCode> K_Space && KeyBoardCode<=K_Tilde){
			BoardRepeat=0;										
			if(HeartLastTime) {
				MimiSpeed+=0.75;
				if(MimiSpeed>8) MimiSpeed= MIMIMAXSPEED+4;
			}	
			else {
				if(MimiSpeed<MIMIMAXSPEED) MimiSpeed+=0.75;
				else {
					MimiSpeed=MIMIMAXSPEED;
				}	
			}	
		}
		MimiPos+=MimiSpeed;

		Mimi_SP_Pos+=MimiSpeed;	
		Bear_SP_Pos=(INT16S) (BearPos-MimiPos+get_sprite_x(MIMI_SP));
		Cat_SP_Pos=(INT16S) (CatPos-MimiPos+get_sprite_x(MIMI_SP));

	if(game_time%50==0){
		if(MimiSpeed>=0.2) MimiSpeed-=0.2;
		else MimiSpeed=0;
	}	
	if(HeartLastTime){
		HeartLastTime--;
		if(HeartLastTime%5) {
			if(MimiSpeed>=0.2) MimiSpeed-=0.2;
			else MimiSpeed = 0;
		}	
	}
	
}

void changeTrack(void)
{
	if(KeyBoardCode==K_Space){
		if(SkipLineidx<4)SkipLineidx++;
		else SkipLineidx=0;
		MimiInWhichLine=LineSort[SkipLineidx];
	}
}
void checkCancelKey(void)
{		
	int i;		
	if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
		SPU_StopChannel(MUSIC_CHCHANL);	
		game_timebak=game_time;					
		Game_StateBak=Game_State;
		SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[WARNING_SFX], 64, 80, SFX_CHANL);		
		cw_warningBG=0;
		if(M6MenuIdx==2){
		
			Game_State=KEYBOARDRACING+19;	
		}else if(M6MenuIdx==0){	
			for (i=0;i<5;i++){
				Word_SP_xPos[i]=get_sprite_x(WORD_SP+i);Word_SP_yPos[i]=get_sprite_y(WORD_SP+i);
			}		
			Game_State=TYPEGAME+19;	
		}
		ClearSomeSp(0,60);		
		LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-1.gif");
		
		
	}
}

void checkM6_2CancelKey()
{												
	if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
		SPU_StopChannel(MUSIC_CHCHANL);	//关掉声音						 							 							
		ClearSomeSp(SP_START,FILLCHAR_SP+1);			
		LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-1.gif");					 						
		SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[WARNING_SFX], 64, 80, SFX_CHANL);					
		cw_warningBG=0;
		Game_StateBak=Game_State;											
		Game_State=FILLINGWORD+19;	
	}				
}					

void checkTimeOut(void)
{
	M6_1GameTime--;											
	if(M6_1GameTime==0){
		Game_State=KEYBOARDRACING+15;		//时间到
	}else 
		set_sprite_display_init(TIMER_SP,64,0,M6_3Timer_Frame[(30*25-M6_1GameTime)/75]);
}

void randCreateItem()
{
	int i,j;
	int tmp;
	for(j=0;j<8;j++){
		if(item[j].kind!=-1) break;
	}		
	if(j==8 && M6_3MiddleCycleNum>=9){		//全部清除后， 再产生新的8个
		for(i=0;i<8;i++){
			if(item[i].kind==-1){
				item[i].kind=GetRandom(4);
				item[i].xPos=MimiPos+320-26+80*GetRandom(2)+(i/2)*160+GetRandom(20);
				item[i].yPos=176-51*GetRandom(3);	
			}	
		}
		if(item[i].xPos>TRACKTOTALLENGTH){
			item[i].xPos=item[i].xPos-64;	
		}
		
		for(i=0;i<8;i+=2){		//消除item 在同一个位置
			while(item[i].yPos == item[i+1].yPos){
				item[i+1].yPos=176-51*GetRandom(3);	
			}
		}
	}	
	
}

void disRandomItem(void)
{
	int i;
	for(i=0;i<8;i++){
		if(item[i].kind!=-1 ){
			if(item[i].xPos>=MimiPos){
				if(item[i].xPos-MimiPos>320){
				 	set_sprite_disable(ITEM_SP+i);
				}else {
					switch (item[i].kind){
						case 0:
							set_sprite_display_init(ITEM_SP+i, item[i].xPos-(int)MimiPos+get_sprite_x(MIMI_SP),item[i].yPos,M6_3Heart_Frame[(game_time/5)%4]);					
						break;
						case 1:
							set_sprite_display_init(ITEM_SP+i, item[i].xPos-(int)MimiPos+get_sprite_x(MIMI_SP),item[i].yPos,M6_3Gift_Frame[(game_time/5)%4]);					
						break;
						case 2:
							set_sprite_display_init(ITEM_SP+i, item[i].xPos-(int)MimiPos+get_sprite_x(MIMI_SP),item[i].yPos,M6_3Block_Frame[0]);					
						break;
						case 3:
							set_sprite_display_init(ITEM_SP+i, item[i].xPos-(int)MimiPos+get_sprite_x(MIMI_SP),item[i].yPos,M6_3Shit_Frame[0]);					
						break;																		
					}
				}	
			}else {
				if(MimiPos-item[i].xPos>160){
					set_sprite_disable(ITEM_SP+i);
				}else {
					switch (item[i].kind){
						case 0:
							set_sprite_display_init(ITEM_SP+i, item[i].xPos-(int)MimiPos+get_sprite_x(MIMI_SP),item[i].yPos,M6_3Heart_Frame[(game_time/5)%4]);					
						break;
						case 1:
							set_sprite_display_init(ITEM_SP+i, item[i].xPos-(int)MimiPos+get_sprite_x(MIMI_SP),item[i].yPos,M6_3Gift_Frame[(game_time/5)%4]);					
						break;
						case 2:
							set_sprite_display_init(ITEM_SP+i, item[i].xPos-(int)MimiPos+get_sprite_x(MIMI_SP),item[i].yPos,M6_3Block_Frame[0]);					
						break;
						case 3:
							set_sprite_display_init(ITEM_SP+i, item[i].xPos-(int)MimiPos+get_sprite_x(MIMI_SP),item[i].yPos,M6_3Shit_Frame[0]);					
						break;																		
					}
				}	
			}		
		}		
	}
}

void checkPlayerHitItem(void)
{
	int i;
	for(i=0;i<8;i++){
		if(item[i].kind!=-1 && get_sprite_spcell(ITEM_SP+i) != 0 ){		// 
			if(get_sprite_x(MIMI_SP)>=get_sprite_x(ITEM_SP+i)+61+60){	//没有碰到,并且移出屏幕
				item[i].kind=-1;			//清除
				set_sprite_disable(ITEM_SP+i);			
			}else if(item[i].yPos==get_sprite_y(MIMI_SP)+64){	//MiMi 在同一赛道
				if(get_sprite_x(MIMI_SP)+128-16>get_sprite_x(ITEM_SP+i) +M6_3ItemXOffset[2*item[i].kind] 
					&&  get_sprite_x(MIMI_SP)+64<get_sprite_x(ITEM_SP+i)+M6_3ItemXOffset[2*item[i].kind+1]){
					//碰到了
					switch (item[i].kind){
						case 0:
							MimiSpeed+=2;
							HeartLastTime =50;
							
//							if(MimiSpeed>MIMIMAXSPEED)MimiSpeed=MIMIMAXSPEED;	//加速
							item[i].kind=-1;			//清除
							set_sprite_disable(ITEM_SP+i);
							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[NEXTSLIDE_SFX], 64, 80, SFX_CHANL);
						break;
						case 1:
							MimiSpeed+=2;
							HeartLastTime =50;							
//							if(MimiSpeed>MIMIMAXSPEED)MimiSpeed=MIMIMAXSPEED;	//加速
							item[i].kind=-1;			//清除
							set_sprite_disable(ITEM_SP+i);
							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[NEXTSLIDE_SFX], 64, 80, SFX_CHANL);
						break;
						case 2:
							MimiSpeed=0;				//减速
							item[i].kind=-1;			//清除
							set_sprite_disable(ITEM_SP+i);
							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OBSTRUCT1_SFX+GetRandom(2)], 64, 80, SFX_CHANL);
							MimiRideStatus=1;
							MimiHitTimer=0;
							BoardRepeat=0;
							set_sprite_display_init(MIMI_SP, (INT16S) Mimi_SP_Pos, 112-51*MimiInWhichLine,M6_3MimiCry_Frame[0]);														
						break;
						case 3:
							MimiSpeed=0;				//减速
							item[i].kind=-1;			//清除
							set_sprite_disable(ITEM_SP+i);
							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OBSTRUCT1_SFX+GetRandom(2)], 64, 80, SFX_CHANL);
							MimiRideStatus=1;
							MimiHitTimer=0;
							BoardRepeat=0;							
							set_sprite_display_init(MIMI_SP,(INT16S) Mimi_SP_Pos, 112-51*MimiInWhichLine,M6_3MimiCry_Frame[0]);																					
						break;																		
					}					
				}			
			
			}
			if(item[i].yPos==get_sprite_y(BEAR_SP)+64){

				if( get_sprite_x(BEAR_SP)+128-25>=get_sprite_x(ITEM_SP+i)+M6_3ItemXOffset[2*item[i].kind] ){
					//碰到了
					switch (item[i].kind){
						case 0:
							BearSpeed++;
							if(BearSpeed>BEARMAXSPEED)BearSpeed=BEARMAXSPEED;	//加速
							item[i].kind=-1;			//清除
							set_sprite_disable(ITEM_SP+i);
//							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[NEXTSLIDE_SFX], 64, 80, SFX_CHANL);
						break;
						case 1:
							BearSpeed++;
							if(BearSpeed>BEARMAXSPEED)BearSpeed=BEARMAXSPEED;	//加速
							item[i].kind=-1;			//清除
							set_sprite_disable(ITEM_SP+i);
//							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[NEXTSLIDE_SFX], 64, 80, SFX_CHANL);
						break;
						case 2:
							BearSpeed=0;	//减速
							item[i].kind=-1;			//清除
							set_sprite_disable(ITEM_SP+i);
//							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OBSTRUCT1_SFX+GetRandom(2)], 64, 80, SFX_CHANL);
							BearRideStatus=1;
							BearHitTimer=0;

						break;
						case 3:
							BearSpeed=0;	//减速
							item[i].kind=-1;			//清除
							set_sprite_disable(ITEM_SP+i);
//							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OBSTRUCT1_SFX+GetRandom(2)], 64, 80, SFX_CHANL);
							BearRideStatus=1;
							BearHitTimer=0;

						break;																		
					}					
				}			
			
			}else if(item[i].yPos==get_sprite_y(CAT_SP)+64){			

				if(get_sprite_x(CAT_SP)+128-29>=get_sprite_x(ITEM_SP+i)+M6_3ItemXOffset[2*item[i].kind]){
					//碰到了
					switch (item[i].kind){
						case 0:
							CatSpeed++;
							if(CatSpeed>CATMAXSPEED)CatSpeed=CATMAXSPEED;	//加速
							item[i].kind=-1;			//清除
							set_sprite_disable(ITEM_SP+i);
//							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[NEXTSLIDE_SFX], 64, 80, SFX_CHANL);
						break;
						case 1:
							CatSpeed++;
							if(CatSpeed>CATMAXSPEED)CatSpeed=CATMAXSPEED;	//加速
							item[i].kind=-1;			//清除
							set_sprite_disable(ITEM_SP+i);
//							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[NEXTSLIDE_SFX], 64, 80, SFX_CHANL);
						break;
						case 2:
							CatSpeed=0;	//减速
							item[i].kind=-1;			//清除
							set_sprite_disable(ITEM_SP+i);
//							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OBSTRUCT1_SFX+GetRandom(2)], 64, 80, SFX_CHANL);
							CatRideStatus=1;
							CatHitTimer=0;
						break;
						case 3:
							CatSpeed=0;	//减速
							item[i].kind=-1;			//清除
							set_sprite_disable(ITEM_SP+i);
//							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OBSTRUCT1_SFX+GetRandom(2)], 64, 80, SFX_CHANL);
							CatRideStatus=1;
							CatHitTimer=0;
						break;																		
					}
				}						
			}	
		}
	}
}
void restoreM6_3Screen(void)
{
	if(Game_StateBak==KEYBOARDRACING+1){
		LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game3_explain.gif");
	}else if( (Game_StateBak==KEYBOARDRACING+2)||(Game_StateBak==KEYBOARDRACING+3) ){
		set_sprite_display_init(0,0,0,M6_3Ready_Frame[M6BGIdx]);
		set_sprite_display_init(START_SP,0,0,M6_3Start_Frame[M6AnimTime/7]);							
	}else {	
		if(Game_StateBak==KEYBOARDRACING+4){
			set_sprite_display_init(0,0,0,M6_3Ready_Frame[M6BGIdx]);

		}else if(Game_StateBak==KEYBOARDRACING+5){		
			set_sprite_display_init(0,0,0,M6_3Middle_Frame[M6BGIdx]);	
		}else if(Game_StateBak==KEYBOARDRACING+6){			
			set_sprite_display_init(0,0,0,M6_3End_Frame[M6BGIdx]);		
		}

		set_sprite_display_init(TIMER_SP,64,0,M6_3Timer_Frame[(int)(MimiPos*10/TRACKTOTALLENGTH)]);						
		disPlayerRank();
		disPlayerRider();
		disRandomItem();				
	}	
	M6AnimTime=M6AnimTimebak;
	HeartLastTime=HeartLastTimebak;	
	game_time=game_timebak;						
	Game_State=Game_StateBak;
	
}

void restoreM6_2PreviousPage(void)
{
	int i;
	LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game2_bg.gif");
	set_sprite_display_init(IMAGE_SP,160-ImageSize[2*WordIdx[M6WhichQuestion]]/2,92-ImageSize[2*WordIdx[M6WhichQuestion]+1]/2,M6Image_Frame[WordIdx[M6WhichQuestion]]);						
	
	for(i=0;i<WordCharNum;i++){
		if(WordLetter1[i]=='[')
			set_sprite_display_init(CHAR_SP+i,160-(52*WordCharNum)/2+i*52,180,M6UnderLine_Frame[0]);														
		else
			set_sprite_display_init(CHAR_SP+i,160-(52*WordCharNum)/2+i*52,180,M6BlackChar_Frame[WordLetter1[i]-65]);								
	}	
	if(M6Level==0){	
		if(Game_StateBak==FILLINGWORD+4){
			if (WhichBlank==1) set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6BlackChar_Frame[M6Input1-65]);				 						 			
		}else{
 			if(Mistake1==0)
				set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6BlackChar_Frame[WordLetter0[Blank1Idx]-65]);				 						 					 	
			else	
				set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6RedChar_Frame[WordLetter0[Blank1Idx]-65]);				 						 						 										
		
		}	
	}else{
		if(Game_StateBak==FILLINGWORD+8){	
			if (WhichBlank==1){
				set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6BlackChar_Frame[M6Input1-65]);
			}else if (WhichBlank==2){	
				set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6BlackChar_Frame[M6Input1-65]);		
				set_sprite_display_init(FILLCHAR_SP+1,160-(52*WordCharNum)/2+Blank2Idx*52,180,M6BlackChar_Frame[M6Input2-65]);		
			}		
		}else{
			if (WhichBlank==1){
	 			if(Mistake1==0)
					set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6BlackChar_Frame[WordLetter0[Blank1Idx]-65]);				 						 					 	
				else	
					set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6RedChar_Frame[WordLetter0[Blank1Idx]-65]);					 
			
			}else if (WhichBlank==2){		
	 			if(Mistake1==0)
					set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6BlackChar_Frame[WordLetter0[Blank1Idx]-65]);				 						 					 	
				else	
					set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6RedChar_Frame[WordLetter0[Blank1Idx]-65]);					 
			 
	 			if(Mistake2==0)
					set_sprite_display_init(FILLCHAR_SP+1,160-(52*WordCharNum)/2+Blank2Idx*52,180,M6BlackChar_Frame[WordLetter0[Blank2Idx]-65]);
				else	
					set_sprite_display_init(FILLCHAR_SP+1,160-(52*WordCharNum)/2+Blank2Idx*52,180,M6RedChar_Frame[WordLetter0[Blank2Idx]-65]);							
			}	
		}	
	}	
	M6AnimTime=M6AnimTimebak;
	game_time=game_timebak;								
	Game_State=Game_StateBak;							
}

INT16S M6AnimTimebak;
INT16S HeartLastTimebak;

INT16S Show_M6Warning(void)
{		
	int i;		
	int step1;
	
	SPU_StopChannel(MUSIC_CHCHANL);	
	game_timebak=game_time;	
	M6AnimTimebak=M6AnimTime;
	HeartLastTimebak=HeartLastTime;																
	Game_StateBak=Game_State;
	SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[WARNING_SFX], 64, 80, SFX_CHANL);		
	cw_warningBG=0;	
	ClearSomeSp(0,60);		
	LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-1.gif");	
	
	step1 = 0;	
	
	while(1){
		PaintTFT_cell4();	
		switch (step1){
			case 0:

				if(M6MenuIdx==2){
					step1=5;	
				}else if(M6MenuIdx==1){						
					step1=10;					
				}else if(M6MenuIdx==0){	
					for (i=0;i<5;i++){
						Word_SP_xPos[i]=get_sprite_x(WORD_SP+i);Word_SP_yPos[i]=get_sprite_y(WORD_SP+i);
					}		
					step1=15;	
				}
		
			break;
		
			case 5:		//M6-3 Warning
			
				if(KeyBoardCode==K_Left&& cw_warningBG==0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TOUCH_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-2.gif");
					cw_warningBG^=1;
					
				}else if(KeyBoardCode==K_Right && cw_warningBG!=0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TOUCH_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-1.gif");
					cw_warningBG^=1;				

				}else if(KeyBoardCode==K_Ok){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);																	
					if( cw_warningBG==1){
						return 1;	
					}else{
						restoreM6_3Screen();												
						return 0;
					}
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
						restoreM6_3Screen();						
						return 0;
				}				
			
			break;
			
			case 10:		//M6_2
				if(KeyBoardCode==K_Left&& cw_warningBG==0){
					
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TOUCH_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-2.gif");
					cw_warningBG^=1;
					
				}else if(KeyBoardCode==K_Right && cw_warningBG!=0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TOUCH_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-1.gif");
					cw_warningBG^=1;				

				}else if(KeyBoardCode==K_Ok){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);																	
					if( cw_warningBG!=0){
						SPU_StopChannel(MUSIC_CHCHANL);					
						return 1;	
					}else{
						restoreM6_2PreviousPage();
						return 0;	
					}
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					restoreM6_2PreviousPage();				
					return 0;						
				}			
			break;												
			
			case 15:		//M6-1 Warning
				if(KeyBoardCode==K_Left&& cw_warningBG==0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TOUCH_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-2.gif");
					cw_warningBG^=1;
					
				}else if(KeyBoardCode==K_Right && cw_warningBG!=0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TOUCH_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-1.gif");
					cw_warningBG^=1;				

				}else if(KeyBoardCode==K_Ok){	
				
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);																	
					if( cw_warningBG==1){
						return 1;	
					}else{
						restoreTypeGame();	
						return 0;						
					}
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					restoreTypeGame();					
					return 0;						
				}			
			break;
		}	
	}		
						
		
}





void main_m06(void)
{
	INT16S ret,m;	
	INT16S tmp;
	INT16S i,j,k; 
	INT16S IdxTmp;
	

	
		
	
	
	

 	
	spMalloc(1830+700+390+900);
	Word_Kb_Num=1830;		//韩文输入法	
	Words_max=6;			//韩文输入法	
	
	writeing_max=0;			//韩文输入法	
	for(i=0;i<Words_max;i++) writeing_Words[i]=0;  //韩文输入法	
	Words_pos=0; 	

	INIT_VideoPub(1830+700+390);
	LoadData_1(1,SpriteCellData,"Pororo\\M06\\SpriteCell_cmp_Open.bin");
	LoadData_1(1,SpriteCellData+Word_Kb_Num*_1K,"Pororo\\words\\SpriteCell_cmp.bin");		
	Palette_ram_cpy(1,0, M6Open_SPRITE_Palette1+0,256);
	Palette_ram_cpy( 2,240, words_SPRITE_Palette1	,16);		//韩文输入法	

 	
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
  	gplib_ppu_sprite_zoom_enable_set(ppu_register_set, 1);		// value:0=disable 1=enable
  	
 	clear_sprite(Spritebuf_number);	
 	  	
	set_sprite_display_init(0,0,0,M6Open_Frame[0]);
	SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[MENU_OPEN_SFX], 64, 80, SFX_CHANL);													   	    	   	      	
  	gplib_ppu_fade_effect_set(ppu_register_set, 0x0); 	
 	
	Game_State=0;M6AnimTime=0;
	M6MenuIdx=0;
	
	while(1){
		PaintTFT_cell4();
		
		if(0 == SPU_Get_SingleChannel_Status(MUSIC_CHCHANL)){
		  	if(Game_State>=TYPEGAME+3 && Game_State<=TYPEGAME+7 ){
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[GAME1BG_MUSIC], 64, 80, MUSIC_CHCHANL);
		  	}else if(Game_State>=FILLINGWORD+3 && Game_State<=FILLINGWORD+10 ){
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[GAME2BG_MUSIC], 64, 80, MUSIC_CHCHANL);
			}else if(Game_State>=KEYBOARDRACING+4 && Game_State<KEYBOARDRACING+7 ){
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[GAME3BG_MUSIC], 64, 80, MUSIC_CHCHANL);			
			}
		}		

		switch(Game_State){

   	    	case OPEN_ANIM:		//Open Anim
				M6AnimTime++;
				if(M6AnimTime<6*3) set_sprite_display_init(0,0,0,M6Open_Frame[M6AnimTime/3]);
				if(M6AnimTime>=3*6+10){
					LoadTextBmp(C_PPU_TEXT1,M6MenuBGGIF[M6MenuIdx]);		 								
					ClearSomeSp(0,0);
					Game_State++;
				}
   	    		
   	    	break;	
   	    	case DIS_MENU_BG:		//设置主菜单背景
				LoadTextBmp(C_PPU_TEXT1,M6MenuBGGIF[M6MenuIdx]);		 								
				clear_sprite(Spritebuf_number);
   	    		Game_State++;
   	    	break;				  			
   	    	case MENU_SELECT:		//主菜单操作
				if(KeyBoardCode==K_Right){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);																
						M6MenuIdx++;						
						if(M6MenuIdx==3){
							M6MenuIdx = 0;
						}
						LoadTextBmp(C_PPU_TEXT1,M6MenuBGGIF[M6MenuIdx]);		 													
				}else if(KeyBoardCode==K_Left){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);																
						if(M6MenuIdx>0){
							M6MenuIdx--;
						}else M6MenuIdx=2;				
					LoadTextBmp(C_PPU_TEXT1,M6MenuBGGIF[M6MenuIdx]);		 								
				}else if(KeyBoardCode==K_Ok){		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);
					M6Score=0;													
					Game_State=10+M6MenuIdx*20;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);
					Game_State=0xff;					
					OS_game=0;				
				}
   	    	break;

			case TYPEGAME:
				LoadData_1(1,SpriteCellData,"Pororo\\M06\\SpriteCell_cmp_G1.bin");				
				Palette_ram_cpy(3,0, M6_1_SPRITE_Palette1,256);	
				LoadDRM(0,6,SpriteCellData+(1830+700+390)*_1K,(int*)&DrmAddr[0],M6_1Sfx_ResourceLocation);										
				LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game1_explain.gif"); 	  
				Words_max=6;			//韩文输入法					
				
				for (i=0;i<3;i++){
					M6RecorderName1[i]=player.RecorderName1[i];
					M6RecorderName2[i]=player.RecorderName2[i];					
					M6RecorderName3[i]=player.RecorderName3[i];
					M6RecordScore[i]=player.RecordScore[i];
				}
				
				Game_State++;			
			break;	
			case TYPEGAME+1:
				if(KeyBoardCode==K_Ok){		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);													
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game_lv1.gif"); 	  					
					M6Level=0;
					Game_State++;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);
					Game_State=DIS_MENU_BG;					
				}				
			break;
			case TYPEGAME+2:		//选择级别
				if(KeyBoardCode==K_Left&& M6Level!=0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game_lv1.gif");
					M6Level--;
				}else if(KeyBoardCode==K_Right && M6Level==0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game_lv2.gif");
					M6Level++;				
				}else if(KeyBoardCode==K_Ok){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);																	
					player.PlayGameTime++;

					
					Game_State++;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					Game_State=TYPEGAME;	
				}			
			break;
			case TYPEGAME+3:
				LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game1_bg.gif");
				game_time=0;
				M6WhichQuestion=0;M6Score=0;
				M6_1GameTime=60*25;											


				set_sprite_display_init(CIRCLE_SP,320-50,2,M6_1Circle_Frame[0]);
				set_sprite_display_init(TIME_SP,320-50+9,2+13,M6_1TimeNum_Frame[(M6_1GameTime/25)/10]);				
				set_sprite_display_init(TIME_SP+1,320-50+9+16,2+13,M6_1TimeNum_Frame[(M6_1GameTime/25)%10]);								
				set_sprite_display_init(TEXTBAR_SP,51,199,M6_1TextBar_Frame[0]);				
							
				initKoreaInput();
				BoardIndex=BoardIndex0;		//消除主菜单按键											
				
				randCreateM6_1Word();
				M6_1WhichWord = 0;
				
				for (i=0;i<5;i++){
				 	RandWord[i]=WordRandIdx[M6_1WhichWord];
				 	M6_1WhichWord++;
				} 				
	
/*				
				RandWord[0]=GetRandom(30);
				j=1;
				do{
					tmp=GetRandom(30);
					for(i=0;i<j;i++){
						if(tmp==RandWord[i]) break; 
					}
					if(i==j){
						RandWord[j]=tmp;
						j++;
					}	
				}while(j<6);
//*/				
				NowWordIdx=0;
				Word_SP_xPos[0]=68,Word_SP_xPos[1]=112,Word_SP_xPos[2]=68,Word_SP_xPos[3]=112,Word_SP_xPos[4]=68;
				Word_SP_yPos[0]=0, Word_SP_yPos[1]=-50,Word_SP_yPos[2]=-100,Word_SP_yPos[3]=-150,Word_SP_yPos[4]=-200;				
				
				if(M6Level==0){
				
					set_sprite_display_init(WORD_SP+0,Word_SP_xPos[0],Word_SP_yPos[0],M6_1L1Text_Frame[RandWord[0]]);								
					set_sprite_display_init(WORD_SP+1,Word_SP_xPos[1],Word_SP_yPos[1],M6_1L1Text_Frame[RandWord[1]]);													
					set_sprite_display_init(WORD_SP+2,Word_SP_xPos[2],Word_SP_yPos[2],M6_1L1Text_Frame[RandWord[2]]);																		
					set_sprite_display_init(WORD_SP+3,Word_SP_xPos[3],Word_SP_yPos[3],M6_1L1Text_Frame[RandWord[3]]);																							
					set_sprite_display_init(WORD_SP+4,Word_SP_xPos[4],Word_SP_yPos[4],M6_1L1Text_Frame[RandWord[4]]);																												
				}else{
					set_sprite_display_init(WORD_SP+0,Word_SP_xPos[0],Word_SP_yPos[0],M6_1L2Text_Frame[RandWord[0]]);								
					set_sprite_display_init(WORD_SP+1,Word_SP_xPos[1],Word_SP_yPos[1],M6_1L2Text_Frame[RandWord[1]]);													
					set_sprite_display_init(WORD_SP+2,Word_SP_xPos[2],Word_SP_yPos[2],M6_1L2Text_Frame[RandWord[2]]);																		
					set_sprite_display_init(WORD_SP+3,Word_SP_xPos[3],Word_SP_yPos[3],M6_1L2Text_Frame[RandWord[3]]);																							
					set_sprite_display_init(WORD_SP+4,Word_SP_xPos[4],Word_SP_yPos[4],M6_1L2Text_Frame[RandWord[4]]);											
				}						

			

				Game_State++;				
			break;
			case TYPEGAME+4:
				M6_1GameTime--;
				set_sprite_display_init(TIME_SP,320-50+9,2+13,M6_1TimeNum_Frame[(M6_1GameTime/25)/10]);				
				set_sprite_display_init(TIME_SP+1,320-50+9+16,2+13,M6_1TimeNum_Frame[(M6_1GameTime/25)%10]);

				createAndMoveWord();				

				show_M6writing32(30,Words_max,60,202);
				m=Test_Keyboard();		//0: 正确，  1：错误，  -1： 没有按键
				if(writeing_max==Words_max){
					InitABC();					
				}				
				
				if(KeyBoardCode==K_Enter && (writeing_max || abc) ){
					write_abc();
					if(checkInputRightOrError()==0){				
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[RIGHT_SFX], 64, 80, SFX_CHANL);										
						Game_State++;		//正确
					}else{
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[WRONG_SFX], 64, 80, SFX_CHANL);															
						M6AnimTime =0;
						Game_State+=3; 	
					}	
				}
				
				if(M6_1GameTime==0){
					ClearSomeSp(0,60);
					SPU_StopChannel(MUSIC_CHCHANL);										
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game1_end_1.gif"); 	  

					set_sprite_display_init(SCORE_SP+1,112-10,112-4,M6_1Score_Frame[(M6Score/10)%10]);				 						 						 				
					set_sprite_display_init(SCORE_SP+2,112+47-10,112-4,M6_1Score_Frame[M6Score%10]);				 						 						 								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TIMEOUT_SFX], 64, 80, SFX_CHANL);					
					
					initKoreaInput();									
					Words_max=3;			//韩文输入法										
					game_time=-1;

					Game_State=TYPEGAME+10;
				}								
				checkCancelKey();
			break;
			case TYPEGAME+5:		//答对
				if(NowWordIdx%2==0){
					set_sprite_posInc(WORD_SP+NowWordIdx,16,0);
					if(get_sprite_x(WORD_SP+NowWordIdx)>=320){
						Game_State++;
					}					

				}else{
					set_sprite_posInc(WORD_SP+NowWordIdx,-16,0);
					if(get_sprite_x(WORD_SP+NowWordIdx)<-160){
						Game_State++;						
					}									
				}
			break;
			case TYPEGAME+6:	//产生下一个单词

			 	RandWord[NowWordIdx]=WordRandIdx[M6_1WhichWord];
			 	M6_1WhichWord++;				
			 	if(M6_1WhichWord>=30){
					randCreateM6_1Word();
					M6_1WhichWord = 0;
			 	}	 

				if(NowWordIdx<4){
					if(M6Level==0)
						set_sprite_display_init(WORD_SP+NowWordIdx,68+(NowWordIdx%2)*44,get_sprite_y(WORD_SP+NowWordIdx+1)-200,M6_1L1Text_Frame[RandWord[NowWordIdx]]);
					else	
						set_sprite_display_init(WORD_SP+NowWordIdx,68+(NowWordIdx%2)*44,get_sprite_y(WORD_SP+NowWordIdx+1)-200,M6_1L2Text_Frame[RandWord[NowWordIdx]]);
				}else{	
					if(M6Level==0)
						set_sprite_display_init(WORD_SP+NowWordIdx,68+(NowWordIdx%2)*44,get_sprite_y(WORD_SP+0)-200,M6_1L1Text_Frame[RandWord[NowWordIdx]]);
					else	
						set_sprite_display_init(WORD_SP+NowWordIdx,68+(NowWordIdx%2)*44,get_sprite_y(WORD_SP+0)-200,M6_1L2Text_Frame[RandWord[NowWordIdx]]);				
				}				

				//改变Bottom Word							
				if(NowWordIdx<4) NowWordIdx++;
				else NowWordIdx=0;	
				M6Score+=1;
				initKoreaInput();
				Game_State-=2;
			break;
			case TYPEGAME+7:	//答错了			
				M6AnimTime++;

				if((M6AnimTime/4)%2==0){
					for(i=0;i<5;i++) set_sprite_posInc(WORD_SP+i,4,0);

				}else{
					for(i=0;i<5;i++) set_sprite_posInc(WORD_SP+i,-4,0);
				}				
				
				if(M6AnimTime>=4*4){
					Game_State++;
				}

			break;
			case TYPEGAME+8:
				initKoreaInput();				
				Game_State-=4;
			break;			
			
			case TYPEGAME+10:		//时间到
				if(game_time==0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[SCORE_SFX], 64, 80, SFX_CHANL);				
				}
				if(KeyBoardCode==K_Ok || game_time>=3*25 ){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);					
					ClearSomeSp(SCORE_SP,SCORE_SP+2);
					//getM6_1Record();
					for(i=0;i<3;i++){
						if(M6Score>M6RecordScore[i])	break;
					}
					if(i<3){		//前3名
						MimiRank=i;

						for (j=2;j>i;j--){
							M6RecordScore[j]=M6RecordScore[j-1];
						}
						
						M6RecordScore[MimiRank]=M6Score;						
						initKoreaInput();
						BoardIndex=BoardIndex0;		//消除按键						 						
						LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game1_end_2.gif"); 	  
						game_time=-1;
						Game_State++;
					}else{
						Game_State=DIS_MENU_BG;		//返回主菜单
					}
				}			
			break;
			case TYPEGAME+11:		//输入名字
				if(game_time==0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[RANKING_SFX], 64, 80, SFX_CHANL);				
				}			
				show_M6writing32(30,Words_max,116,112);
				m=Test_Keyboard();		//0: 正确，  1：错误，  -1： 没有按键				
				if(writeing_max==Words_max){
					InitABC();					
				}
				if(KeyBoardCode==K_Ok||KeyBoardCode==K_Enter){	

					if(ABC_idx) write_abc();


					for (j=2;j>MimiRank;j--){
						if(j==2){
							for (i=0;i<3;i++) M6RecorderName3[i]=M6RecorderName2[i];
						}else if(j==1){
							for (i=0;i<3;i++) M6RecorderName2[i]=M6RecorderName1[i];
						}	
					}
					switch (MimiRank){
						case 0:
							for (i=0;i<3;i++) M6RecorderName1[i]=writeing_Words[i];						
						break;
						case 1:
							for (i=0;i<3;i++) M6RecorderName2[i]=writeing_Words[i];						
						break;
						case 2:
							for (i=0;i<3;i++) M6RecorderName3[i]=writeing_Words[i];						
						break;												
					}
					for (i=0;i<3;i++){
						player.RecorderName1[i]=M6RecorderName1[i];
						player.RecorderName2[i]=M6RecorderName2[i];					
						player.RecorderName3[i]=M6RecorderName3[i];
						player.RecordScore[i]=M6RecordScore[i];
					}					
					Save_player();			
										
					ClearSomeSp(30,60);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game1_end_3.gif"); 	  
					Game_State++;
				}				
			break;
			
			case TYPEGAME+12:		//显示排名
			
				set_sprite_display_init(SCORE_SP+0,212,   92,M6_1TimeNum_Frame[M6RecordScore[0]/10]);								
				set_sprite_display_init(SCORE_SP+1,212+18,92,M6_1TimeNum_Frame[M6RecordScore[0]%10]);												
				
				set_sprite_display_init(SCORE_SP+2,212,	  92+43,M6_1TimeNum_Frame[M6RecordScore[1]/10]);								
				set_sprite_display_init(SCORE_SP+3,212+18,92+43,M6_1TimeNum_Frame[M6RecordScore[1]%10]);												
				
				set_sprite_display_init(SCORE_SP+4,212,	  92+86,M6_1TimeNum_Frame[M6RecordScore[2]/10]);								
				set_sprite_display_init(SCORE_SP+5,212+18,92+86,M6_1TimeNum_Frame[M6RecordScore[2]%10]);												

				show_writtenName(30,M6RecorderName1,99,98-16);								
				show_writtenName(40,M6RecorderName2,99,141-16);												
				show_writtenName(50,M6RecorderName3,99,184-16);
				game_time=0;

				Game_State++;																
			break;			

			case TYPEGAME+13:		
				if(KeyBoardCode==K_Ok||KeyBoardCode==K_Enter ||  game_time>=3*25 ){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);					
					Game_State=DIS_MENU_BG;
				}				
								
			break;	
			
			case TYPEGAME+19:		//M6-1 Warning
				if(KeyBoardCode==K_Left&& cw_warningBG==0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TOUCH_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-2.gif");
					cw_warningBG^=1;
					
				}else if(KeyBoardCode==K_Right && cw_warningBG!=0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TOUCH_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-1.gif");
					cw_warningBG^=1;				

				}else if(KeyBoardCode==K_Ok){	
				
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);																	
					if( cw_warningBG==1){
						Game_State=DIS_MENU_BG;	
					}else{
						restoreTypeGame();	
					}
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
						restoreTypeGame();					
				}			
			break;													
					

			
			case FILLINGWORD:	
				LoadData_1(1,SpriteCellData,"Pororo\\M06\\SpriteCell_cmp_G2.bin");				
				Palette_ram_cpy(3,0, M6_2_SPRITE_Palette1,256);	
				LoadDRM(0,5,SpriteCellData+(1830+700+390)*_1K,(int*)&DrmAddr[0],M6_2Sfx_ResourceLocation);										
				LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game2_explain.gif"); 	  
				Game_State++; 	    	 											
			case FILLINGWORD+1:	
				if(KeyBoardCode==K_Ok){		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);													
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game_lv1.gif"); 	  					
					M6Level=0;
					Game_State++;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);
					Game_State=DIS_MENU_BG;					
				}				
			break;
			case FILLINGWORD+2:	
				if(KeyBoardCode==K_Left&& M6Level!=0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game_lv1.gif");
					M6Level--;
				}else if(KeyBoardCode==K_Right && M6Level==0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game_lv2.gif");
					M6Level++;				
				}else if(KeyBoardCode==K_Ok){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);																	

					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game2_bg.gif");
					game_time=0;CursorStatus=0;	
					M6WhichQuestion=0;M6Score=0;
					WordIdx[M6WhichQuestion]=GetRandom(50);					
					player.PlayGameTime++;					
					Game_State++;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					Game_State=FILLINGWORD;	
				}			
			break;
			case FILLINGWORD+3:	
					ClearSomeSp(CHAR_SP,CHAR_SP+9);
					set_sprite_display_init(IMAGE_SP,160-ImageSize[2*WordIdx[M6WhichQuestion]]/2,
					92-ImageSize[2*WordIdx[M6WhichQuestion]+1]/2,M6Image_Frame[WordIdx[M6WhichQuestion]]);
				
					WordCharNum=WordLength[WordIdx[M6WhichQuestion]];
					WordCharPtr =(char*)WordChar[WordIdx[M6WhichQuestion]];
					
					WordLetter0[0]=0;
					strcat(WordLetter0,WordCharPtr);					
					WordLetter1[0]=0;
					strcat(WordLetter1,WordCharPtr);										
					
					Blank1Idx=GetRandom(WordCharNum);	
					
					if(M6Level!=0){
						do{
							Blank2Idx=GetRandom(WordCharNum);	
						}while(Blank2Idx==Blank1Idx);

						if(Blank1Idx>Blank2Idx){
							tmp=Blank1Idx;
							Blank1Idx=Blank2Idx;
							Blank2Idx=tmp;
						}						
						
					}

					WordCharPtr=WordLetter0;
//					*(WordCharPtr+Blank1Idx)= '[';
					WordLetter1[Blank1Idx]='[';
					if(M6Level!=0) WordLetter1[Blank2Idx]='[';

					for(i=0;i<WordCharNum;i++){
						if(WordLetter1[i]=='[')
							set_sprite_display_init(CHAR_SP+i,160-(52*WordCharNum)/2+i*52,182,M6UnderLine_Frame[0]);														
						else
							set_sprite_display_init(CHAR_SP+i,160-(52*WordCharNum)/2+i*52,180,M6BlackChar_Frame[WordLetter1[i]-65]);								
					}	

					WhichBlank=0;Mistake1=0;Mistake2=0;
					M6CursorYpos=180;M6CursorXpos=160-(52*WordCharNum)/2+Blank1Idx*52;
					M6Input1 = -1, M6Input2 = -1;
					if(M6Level==0){
						Game_State++;											
					}else {
						Game_State= FILLINGWORD+8;																
					}	
						
			break;		
			case FILLINGWORD+4:		//LEVEL 1
				if(game_time/5%2){
					set_sprite_display_init(CURSOR_SP,M6CursorXpos,M6CursorYpos,M6Cursor_Frame[0]);
				}else{
					set_sprite_disable(CURSOR_SP);					
				}
		
				if( (KeyBoardCode>=K_S_a && KeyBoardCode<=K_S_z) ||(KeyBoardCode>=K_a && KeyBoardCode<=K_z) ){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);
				 	if(KeyBoardCode>=K_a && KeyBoardCode<=K_z) KeyBoardCode-=32;
				 	M6Input1=KeyBoardCode;
					set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6BlackChar_Frame[M6Input1-65]);				 						 	
					set_sprite_disable(CURSOR_SP);										
					WhichBlank++;
					M6AnimTime=0;														 						
					Game_State++;																							
						
				}else checkM6_2CancelKey();
			break;		
			case FILLINGWORD+5:	
				M6AnimTime++;
/*				
				if(game_time/5%2){
					set_sprite_display_init(CURSOR_SP,M6CursorXpos,M6CursorYpos,M6Cursor_Frame[0]);
				}else{
					set_sprite_disable(CURSOR_SP);					
				}			
*/			
				if(SPU_Get_SingleChannel_Status(SFX_CHANL)==0 && M6AnimTime>=25 ){
				 	if(M6Input1==WordLetter0[Blank1Idx]){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[RIGHT_SFX], 64, 80, SFX_CHANL);
				 	}else{
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[WRONG_SFX], 64, 80, SFX_CHANL);					 	
						Mistake1++;
				 	}	
		 			if(Mistake1==0)
						set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6BlackChar_Frame[WordLetter0[Blank1Idx]-65]);				 						 					 	
					else	
						set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6RedChar_Frame[WordLetter0[Blank1Idx]-65]);				 						 						 								
				 	
					M6AnimTime=0;					
					Game_State++;																												
				}else checkM6_2CancelKey();
			break;		
			case FILLINGWORD+6:	
				set_sprite_disable(CURSOR_SP);
				checkM6_2CancelKey();												
			
				M6AnimTime++;
				if(M6AnimTime>=25){				
					LoadDRM(0, 1, VOICE_ADDR_GAME, &DrmAddr[WORD_VOICE], M6_2Voice_ResourceLocation+WordIdx[M6WhichQuestion]);					 	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[WORD_VOICE], 64, 80, SPEECHCHANL);					 								
			 		if(Mistake1==0)M6Score+=10;
			 		M6WhichQuestion++;
			 		if(M6WhichQuestion==10){
			 			Game_State=FILLINGWORD+17;
			 		}else{
			 			Game_State++;		//出下一题	
			 		}

				}
			break;		
			case FILLINGWORD+7:		//出下一题
				if(SPU_Get_SingleChannel_Status(SPEECHCHANL)==0)
				{
					ClearSomeSp(FILLCHAR_SP,FILLCHAR_SP+1);
					
					do{
						IdxTmp=GetRandom(50);
						for (i=0;i<M6WhichQuestion;i++){
							if(IdxTmp==WordIdx[i]) break;
						}
					}while(i != M6WhichQuestion);
					WordIdx[M6WhichQuestion]=IdxTmp;
					
					Game_State=FILLINGWORD+3;
				}
			break;	
			
			case FILLINGWORD+8:		//LEVEL 2

				if(game_time%5==0){
					CursorStatus^=1;
					if(CursorStatus==1) 				
						set_sprite_display_init(CURSOR_SP,M6CursorXpos,M6CursorYpos,M6Cursor_Frame[0]);
					else{
						set_sprite_disable(CURSOR_SP);					
					}
				}			
			
				if( (KeyBoardCode>=K_S_a && KeyBoardCode<=K_S_z) ||(KeyBoardCode>=K_a && KeyBoardCode<=K_z) ){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);
				 	if(KeyBoardCode>=K_a && KeyBoardCode<=K_z) KeyBoardCode-=32;
				 	if (WhichBlank==0){
					 	M6Input1=KeyBoardCode;				 					 	
						set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6BlackChar_Frame[M6Input1-0x41]);
					 	if(M6Input1!=WordLetter0[Blank1Idx]){
							Mistake1++;							
						}	
				 		WhichBlank++;
				 		M6CursorXpos=160-(52*WordCharNum)/2+Blank2Idx*52;
					 }else{
					 	M6Input2=KeyBoardCode;
						set_sprite_display_init(FILLCHAR_SP+1,160-(52*WordCharNum)/2+Blank2Idx*52,180,M6BlackChar_Frame[M6Input2-65]);					 

					 	if(M6Input2!=WordLetter0[Blank2Idx]){
							Mistake2++;					 	
					 	}
						set_sprite_disable(CURSOR_SP);															 	
						WhichBlank++;
						M6AnimTime = 0;												
						Game_State++;
				 	}

				}else checkM6_2CancelKey();
			break;		
			case FILLINGWORD+9:
/*			
				if(game_time%5==0){
					CursorStatus^=1;
					if(CursorStatus==1) 				
						set_sprite_display_init(CURSOR_SP,M6CursorXpos,M6CursorYpos,M6Cursor_Frame[0]);
					else{
						set_sprite_disable(CURSOR_SP);					
					}
				}	
*/				
				M6AnimTime++;				
				if(SPU_Get_SingleChannel_Status(SFX_CHANL)==0 && M6AnimTime>=25){
				 	if(Mistake1 ==0 && Mistake2==0){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[RIGHT_SFX], 64, 80, SFX_CHANL);
				 	}else{
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[WRONG_SFX], 64, 80, SFX_CHANL);					 						 	
					}	

		 			if(Mistake1==0)
						set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6BlackChar_Frame[WordLetter0[Blank1Idx]-65]);				 						 					 	
					else	
						set_sprite_display_init(FILLCHAR_SP,160-(52*WordCharNum)/2+Blank1Idx*52,180,M6RedChar_Frame[WordLetter0[Blank1Idx]-65]);					 
				 
		 			if(Mistake2==0)
						set_sprite_display_init(FILLCHAR_SP+1,160-(52*WordCharNum)/2+Blank2Idx*52,180,M6BlackChar_Frame[WordLetter0[Blank2Idx]-65]);
					else	
						set_sprite_display_init(FILLCHAR_SP+1,160-(52*WordCharNum)/2+Blank2Idx*52,180,M6RedChar_Frame[WordLetter0[Blank2Idx]-65]);					
						
					M6AnimTime=0;
					Game_State++;										
				}else checkM6_2CancelKey();											
						
																
			break;		
			case FILLINGWORD+10:	
				set_sprite_disable(CURSOR_SP);
				checkM6_2CancelKey();
				M6AnimTime++;
				if(M6AnimTime>=25){			

					LoadDRM(0, 1, VOICE_ADDR_GAME, &DrmAddr[WORD_VOICE], M6_2Voice_ResourceLocation+WordIdx[M6WhichQuestion]);					 	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[WORD_VOICE], 64, 80, SPEECHCHANL);					 									 
			 		if(Mistake1==0 && Mistake2==0 )M6Score+=10;
			 		M6WhichQuestion++;
			 		if(M6WhichQuestion==10){
			 			Game_State=FILLINGWORD+17;
				 	}else{
			 			Game_State=FILLINGWORD+7;	
				 	}
				}
			break;	

			case FILLINGWORD+17:
				if(SPU_Get_SingleChannel_Status(SPEECHCHANL)==0 ){			
					SPU_StopChannel(MUSIC_CHCHANL);			 		
					ClearSomeSp(IMAGE_SP,FILLCHAR_SP+1);				 							 		
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game2_end.gif");					 		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[SCORE_SFX], 64, 80, SFX_CHANL);								

					game_time = 0;	
					Game_State++;		
				}
			break;	

			case FILLINGWORD+18:		//显示得分
				
				set_sprite_display_init(SCORE_SP,112-32,112-4,M06_2Score_Frame[M6Score/100]);				 						 						 							
				set_sprite_display_init(SCORE_SP+1,112+15,112-4,M06_2Score_Frame[(M6Score/10)%10]);				 						 						 				
				set_sprite_display_init(SCORE_SP+2,112+62,112-4,M06_2Score_Frame[M6Score%10]);				 						 						 								
				if(KeyBoardCode==K_Ok|| game_time >= 3*25){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);	
					ClearSomeSp(SCORE_SP,SCORE_SP+2);																
					Game_State=DIS_MENU_BG;
				}			

					
			break;			
			
			case FILLINGWORD+19:		//退出警告
				if(KeyBoardCode==K_Left&& cw_warningBG==0){
					
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TOUCH_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-2.gif");
					cw_warningBG^=1;
					
				}else if(KeyBoardCode==K_Right && cw_warningBG!=0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TOUCH_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-1.gif");
					cw_warningBG^=1;				

				}else if(KeyBoardCode==K_Ok){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);																	
					if( cw_warningBG!=0){
						SPU_StopChannel(MUSIC_CHCHANL);					
						Game_State=DIS_MENU_BG;	
					}else{
						restoreM6_2PreviousPage();

					}
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					restoreM6_2PreviousPage();				
				}			
			break;																
					
							
			case KEYBOARDRACING:
				LoadData_1(1,SpriteCellData,"Pororo\\M06\\SpriteCell_cmp_G3.bin");				
				Palette_ram_cpy(1,0, M6_3_SPRITE_Palette1,256);							
				Palette_ram_cpy(3,0, M6_3_SPRITE_Palette1+256,256);			
				
				LoadDRM(0,11,SpriteCellData+(1830+700+390)*_1K,(int*)&DrmAddr[0],M6_3Sfx_ResourceLocation);														
				LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game3_explain.gif"); 	  
				Game_State++;
				
			break;	 	    	 											
			case KEYBOARDRACING+1:	
				if(KeyBoardCode==K_Ok){		
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[ENGINE_SFX], 64, 80, SFX_CHANL);													
					M6BGIdx=0;					
//					LoadTextBmp(C_PPU_TEXT1,M6_Game3ReadyGIF[M6BGIdx]);
					set_sprite_display_init(0,0,0,M6_3Ready_Frame[0]);
					set_sprite_display_init(BEAR_SP,0,112-51*2,M6_3BearRide_Frame[0]);
					set_sprite_display_init(CAT_SP,0,112-51,M6_3CatRide_Frame[0]);
					set_sprite_display_init(MIMI_SP,0,112,M6_3MiMiRide_Frame[0]);

					set_sprite_display_init(RANK_SP,256,0,m6_3Rank_Frame[0]);					
					M6AnimTime=0;game_time=0; HeartLastTime = 0;					

					Game_State++;										
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);
					Game_State=DIS_MENU_BG;					
				}				
			break;
			
			case KEYBOARDRACING+2:	
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){								
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, SFX_CHANL);
					Game_State=DIS_MENU_BG;					
				}	
				if(0 == SPU_Get_SingleChannel_Status(SFX_CHANL)){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[BEEP_SFX], 64, 80, SFX_CHANL);																			
					player.PlayGameTime++;																	
					Game_State++;
				}
			break;			
		
			case KEYBOARDRACING+3:		//开始倒计时
				M6AnimTime++;
				set_sprite_display_init(START_SP,0,0,M6_3Start_Frame[M6AnimTime/7]);
				if(M6AnimTime>=3*25){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[GAME3BG_MUSIC], 64, 80, MUSIC_CHCHANL);							
					
					set_sprite_disable(START_SP);									
					game_time=0;
					CatSpeed=0;BearSpeed=0;MimiSpeed=0;
					CatPos=0;BearPos=0,MimiPos=0;
					Cat_SP_Pos=0;Bear_SP_Pos=0,Mimi_SP_Pos=0;
					CatRandSpeed=GetRandom(3)/10;
					CatRideStatus=0;BearRideStatus=0,MimiRideStatus=0;
					MimiRank=0;
//					do{
						BearRandSpeed=GetRandom(3)/10;
//					}while(BearRandSpeed==CatRandSpeed);	//两个PC 玩家的速度不要一样
					
					M6_3BGTimer=0;M6_3MiddleCycleNum=27;
					
					M6_1GameTime=30*25;											
//					set_sprite_display_init(TIMER_SP,64,0,M6_3Timer_Frame[(30*25-M6_1GameTime)/75]);					

					for(i=0;i<8;i++)  item[i].kind=-1;
					
					SkipLineidx=2;
					MimiInWhichLine=LineSort[SkipLineidx];
					Game_State++;
				}			
			break;								

			case KEYBOARDRACING+4:		//出发

				set_sprite_display_init(TIMER_SP,64,0,M6_3Timer_Frame[(int)(MimiPos*10/TRACKTOTALLENGTH)]);				
				disPlayerRank();
				disPlayerRider();
				playMove();
				changeTrack();
				if(Mimi_SP_Pos>=60){
					Mimi_SP_Pos=60;
					M6_3BGTimer+=MimiSpeed;
					if(M6_3BGTimer>=20){
						M6_3BGTimer-=20;
						M6BGIdx++;					
						if(M6BGIdx>=7) {
							M6BGIdx=0;
							set_sprite_display_init(0,0,0,M6_3Middle_Frame[M6BGIdx]);							
							Game_State++;
						}else{
							set_sprite_display_init(0,0,0,M6_3Ready_Frame[M6BGIdx]);													
						}	
					}	
				}				
				checkCancelKey();				
			break;
			case KEYBOARDRACING+5:		//中间
			
				randCreateItem();
				disRandomItem();
				checkPlayerHitItem();																

//				checkTimeOut();
				set_sprite_display_init(TIMER_SP,64,0,M6_3Timer_Frame[(int)(MimiPos*10/TRACKTOTALLENGTH)]);				
				disPlayerRank();
				disPlayerRider();
				playMove();	
				changeTrack();																
				if(Mimi_SP_Pos>=60){
					Mimi_SP_Pos=60;
					M6_3BGTimer+=MimiSpeed;
					if(M6_3BGTimer>=20){
						M6_3BGTimer-=20;
						M6BGIdx++;					
						if(M6BGIdx>=6) {
							M6BGIdx=0;
							M6_3MiddleCycleNum--;
							if(M6_3MiddleCycleNum==0){
								set_sprite_display_init(0,0,0,M6_3End_Frame[M6BGIdx]);
								Game_State++;
							}else 
								set_sprite_display_init(0,0,0,M6_3Middle_Frame[M6BGIdx]);
						}else{
								set_sprite_display_init(0,0,0,M6_3Middle_Frame[M6BGIdx]);
						}	
					}	
				}				
				checkCancelKey();				
			break;
			case KEYBOARDRACING+6:		//终点
				disRandomItem();
				checkPlayerHitItem();								

				set_sprite_display_init(TIMER_SP,64,0,M6_3Timer_Frame[(int)(MimiPos*10/TRACKTOTALLENGTH)]);				
				disPlayerRank();
				disPlayerRider();				
				playMove();
				changeTrack();	
				if(Mimi_SP_Pos>=60){
					Mimi_SP_Pos=60;
					M6_3BGTimer+=MimiSpeed;
					if(M6_3BGTimer>=20){
						M6_3BGTimer-=20;
						M6BGIdx++;					
						if(M6BGIdx>=6) {
							M6BGIdx=0;
							ClearSomeSp(0,ITEM_SP+8);
							SPU_StopChannel(MUSIC_CHCHANL);												

							if(MimiRank)
							{
								LoadTextBmp(C_PPU_TEXT1,M6_Game3LoseAnim[M6BGIdx]);
								game_time=0;							
								Game_State++;				//失败
							}else {
								LoadTextBmp(C_PPU_TEXT1,M6_Game3WinAnim[M6BGIdx]);
								game_time=0;							
								Game_State+=3;				//成功					
							}	
						}else{
							set_sprite_display_init(0,0,0,M6_3End_Frame[M6BGIdx]);
						}	
					}	
				}																								
				checkCancelKey();				
			break;			
			case KEYBOARDRACING+7:		//失败
				if(game_time==1) 	SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[FAILURE_SFX], 64, 80, SFX_CHANL);
				if(game_time%8==0){
					M6BGIdx++;
					if(M6BGIdx>=4) {
						if(game_time>=75)
							Game_State=DIS_MENU_BG;
						else	
							M6BGIdx = 3;					
					}else{
						LoadTextBmp(C_PPU_TEXT1,M6_Game3LoseAnim[M6BGIdx]);
					}	
				}			
			
			break;	
			
			case KEYBOARDRACING+9:		//成功
				if(game_time==1) 	SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[SUCCESS_SFX], 64, 80, SFX_CHANL);
				if(game_time%8==0){
					M6BGIdx++;
					if(M6BGIdx>=4 ) {
						if(game_time>=75)
							Game_State=DIS_MENU_BG;
						else	
							M6BGIdx = 3;
					}else{
						LoadTextBmp(C_PPU_TEXT1,M6_Game3WinAnim[M6BGIdx]);
					}	
				}			
			
			break;					
			
			break;			
			case KEYBOARDRACING+19:		//M6-3 Warning
				if(KeyBoardCode==K_Left&& cw_warningBG==0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TOUCH_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-2.gif");
					cw_warningBG^=1;
					
				}else if(KeyBoardCode==K_Right && cw_warningBG!=0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TOUCH_SFX], 64, 80, SFX_CHANL);
					LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_warning-1.gif");
					cw_warningBG^=1;				

				}else if(KeyBoardCode==K_Ok){	
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, SFX_CHANL);																	
					if( cw_warningBG==1){
						Game_State=DIS_MENU_BG;	
					}else{
						LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game2_bg.gif");
						restoreM6_3Screen();
					}
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
						LoadTextBmp(C_PPU_TEXT1,"Pororo\\M06\\M6_Game2_bg.gif");
						restoreM6_3Screen();						
				}			
			break;													
	
		}

		if(Game_State>=10  &&(	(Game_State>=KEYBOARDRACING+3 && Game_State<KEYBOARDRACING+7)
							  ||(Game_State>=TYPEGAME+3 && Game_State<=TYPEGAME+7)
							  ||(Game_State>=FILLINGWORD+3 && Game_State<=FILLINGWORD+10) )
		  ) 					  
							  							  
		
		{
			if(TestMenu_Key()){
				ret=Show_M6Warning();
				if(ret==0){
					OS_game=16;
				}else Game_State=0xff;
			}
		}else if(Goto_desktop()){
			Game_State=0xff;
		}

		
		
		if(Game_State==0xff){
			SPU_StopChannel(MUSIC_CHCHANL);		
		 	break;
		} 
   	}
}

