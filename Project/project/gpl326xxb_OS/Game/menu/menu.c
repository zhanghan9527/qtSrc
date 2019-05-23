#include "application.h"
#include "data.h"
#include "math.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "main_ui.h"
#include "icon.h"

extern const INT16U menu_SPRITE_Palette1[];
extern const INT16U words_SPRITE_Palette1[];
extern const INT16U RES_logo_Bg01[];
extern const INT16U RES_logo_Bg02[];
extern const INT16U RES_logo_Bg03[];
extern const INT16U RES_logo_Bg04[];
extern const INT16U RES_logo_Bg05[];
extern const INT16U RES_logo_Bg06[];
extern const INT16U RES_logo_Bg07[];
extern const INT16U RES_logo_Bg08[];
extern const INT16U RES_logo_Bg09[];
extern const INT16U RES_logo_Bg10[];

extern const INT16U RES_logo_Bg11[];
extern const INT16U RES_logo_Bg12[];
extern const INT16U RES_logo_Bg13[];
extern const INT16U RES_logo_Bg14[];
extern const INT16U RES_logo_Bg15[];
extern const INT16U RES_logo_Bg16[];
extern const INT16U RES_logo_Bg17[];
extern const INT16U RES_logo_Bg18[];
extern const INT16U RES_logo_Bg19[];
extern const INT16U RES_logo_Bg20[];

extern const INT16U RES_logo_Bg21[];
extern const INT16U RES_logo_Bg22[];
extern const INT16U RES_logo_Bg23[];
extern const INT16U RES_logo_Bg24[];
extern const INT16U RES_logo_Bg25[];
extern const INT16U RES_logo_Bg26[];
extern const INT16U RES_logo_Bg27[];
extern const INT16U RES_logo_Bg28[];


INT32U Word_Kb_Num;
INT16S Singel=0,Singel_time=0;
INT16S key_pos=0;
//=================================================================
void show_Title()
{
	//*
	if(LVLevel>0) set_sprite_display_init(0,10,10,main_power_Frame[LVLevel]);
	else{
		if(game_time/12%2) set_sprite_display_init(0,10,10,main_power_Frame[0]);
		else set_sprite_display_init(0,0,0,0);
	}

	if(player.Miss_User) set_sprite_display_init(1,226,10,main_ui_Frame[0]);
	else set_sprite_display_init(1,0,0,0);
	if(player.InBox_Mail) set_sprite_display_init(2,270,6,main_ui_Frame[1]);
	else set_sprite_display_init(2,0,0,0);
}
void show_Date(INT16S sp)
{
	INT16S h,m;
//-----------Date
#define LockDataX	92
#define LockDataY	72
#define LockDataXsize	16
	set_sprite_display_init(sp+4,LockDataX,LockDataY,Main_date_Frame[Year/1000]);
	set_sprite_display_init(sp+5,LockDataX+LockDataXsize,LockDataY,Main_date_Frame[Year/100%10]);
	set_sprite_display_init(sp+6,LockDataX+LockDataXsize*2,LockDataY,Main_date_Frame[Year/10%10]);
	set_sprite_display_init(sp+7,LockDataX+LockDataXsize*3,LockDataY,Main_date_Frame[Year%10]);
	set_sprite_display_init(sp+8,LockDataX+LockDataXsize*4,LockDataY,Main_date_Frame[10]);

	set_sprite_display_init(sp+9,LockDataX+LockDataXsize*5-5,LockDataY,Main_date_Frame[Month/10]);
	set_sprite_display_init(sp+10,LockDataX+LockDataXsize*6-5,LockDataY,Main_date_Frame[Month%10]);
	set_sprite_display_init(sp+11,LockDataX+LockDataXsize*7-5,LockDataY,Main_date_Frame[10]);

	set_sprite_display_init(sp+12,LockDataX+LockDataXsize*8-10,LockDataY,Main_date_Frame[Day/10]);
	set_sprite_display_init(sp+13,LockDataX+LockDataXsize*9-10,LockDataY,Main_date_Frame[Day%10]);

	
	if(Rtc_time.rtc_hour>=12) set_sprite_display_init(sp+15,30,128,main_time_Frame[11]);
	else set_sprite_display_init(sp+15,30,128,main_time_Frame[10]);

	h=Rtc_time.rtc_hour%12;
	m=Rtc_time.rtc_min;
	if(h==0) h=12;
	set_sprite_display_init(sp+16,91,105,main_time_Frame[h/10]);
	set_sprite_display_init(sp+17,91+42,105,main_time_Frame[h%10]);
	if(game_time/12%2) set_sprite_display_init(sp+18,91+84+2,105,main_time_Frame[12]);
	else set_sprite_display_init(sp+18,0,0,0);
	set_sprite_display_init(sp+19,91+126-24,105,main_time_Frame[m/10]);
	set_sprite_display_init(sp+20,91+168-24,105,main_time_Frame[m%10]);
}

#define ICON_X	50
#define ICON_Xsize	75
#define ICON_Y	160
void show_icon()
{
	int i,x,y,m;
	
	for(i=0;i<4;i++){
		x=(ICON_X+i*ICON_Xsize)-50;
		y=ICON_Y;
		m=player.HomeIconIdx[i];
		if(m==-1) break;
		if(i==key_pos && (game_time/6%2) ){
			//set_sprite_display_init(6+i,x,y,main_icon2_Frame[m]);
			set_sprite_display_init(6+i,x,y,0);
		}else set_sprite_display_init(6+i,x,y,main_icon_Frame[m]);
	}
}


INT16S Test_Icon(INT16S page)
{
	INT16S i;
	for(i=0;i<3;i++){
		if(TestTouch(80*i,80*i+80,240,320)) return i;
		else if(TestTouch(80*i,80*i+80,ICON_Y-172,ICON_Y-96)) return (3+page*6+i);
		else if(TestTouch(80*i,80*i+80,ICON_Y-96,ICON_Y-20)) return (6+page*6+i);
	}
	return -1;
}
INT16S TestMenu_Key()
{
	if(KeyBoardCode==K_Home && OS_game!=0){
		OS_game=0;
		return 1;
	}else if(KeyBoardCode==K_Setting && OS_game!=10){
		OS_game=10;
		return 1;
	}else if(KeyBoardCode==K_Video && OS_game!=11){
		OS_game=11;
		return 1;
	}else if(KeyBoardCode==K_Camera && OS_game!=12){
		OS_game=12;
		return 1;
	}else if(KeyBoardCode==K_Decoration && OS_game!=13){
		OS_game=13;
		return 1;
	}else if(KeyBoardCode==K_Album && OS_game!=14){
		OS_game=14;
		return 1;
	}else if(KeyBoardCode==K_VideoCall && OS_game!=15){
		OS_game=15;
		return 1;
	}else if(KeyBoardCode==K_Game && OS_game!=17){
		OS_game=17;
		return 1;
	}else if(KeyBoardCode==K_EMail && OS_game!=16){
		OS_game=16;
		return 1;
	}else if(KeyBoardCode==K_Diary && OS_game!=18){
		OS_game=18;
		return 1;
	}else if(KeyBoardCode==K_MimiCard && OS_game!=19){
		OS_game=19;
		return 1;
	}
	return 0;
}

const char* Back_bg[]={
	"Pororo\\menu\\bg\\bg1.gif",
	"Pororo\\menu\\bg\\bg2.gif",
	"Pororo\\menu\\bg\\bg3.gif",
	"Pororo\\menu\\bg\\bg4.gif",

	"Pororo\\user\\bg.jpg",
};
void Display_Background()
{
	if(player.HomeScreenBGIdx>=4){
		InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);
		AddRootPath(Back_bg[4]);
		if(Photo_decode_to_buff((char *)rootPath,320,240,TVOutBuff)>=0){
			//Turn_PhotoH(ScalerBuff);
		}else fill_mem(TVOutBuff,FRAME_SIZE*2,0);
	}else LoadTextBmp(C_PPU_TEXT1,Back_bg[player.HomeScreenBGIdx]);
}
extern INT16U EventFlag;
void main_menu()
{	
	INT16S i;
	
	spMalloc(500+900);
	Palette_ram_cpy( 1,0, menu_SPRITE_Palette1+256*0	,256);	
	LoadData_1(1,SpriteCellData,"Pororo\\menu\\SpriteCell_cmp.BIN");
	
	INIT_VideoPub(500);
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	LoadTextBmp(C_PPU_TEXT1,"Pororo\\menu\\bg\\bg1.gif");
	Display_Background();
	clear_sprite(Spritebuf_number);
	gplib_ppu_fade_effect_set(ppu_register_set, 0);

	game_time=step=0;
	key_pos=-1;
	while(1)
	{
		PaintTFT_cell4();
		switch(step){
//--------------------------------------------------menu
			case 0:
				show_icon();
				show_Title();
				ClearSomeSp(10,40);
				if(KeyBoardCode==K_Right && player.HomeIconIdx[0]!=-1){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					if(key_pos==-1) key_pos=0;
					else if(key_pos<3 && player.HomeIconIdx[key_pos+1]!=-1)  key_pos++;
					else key_pos=0;
				}else if(KeyBoardCode==K_Left && player.HomeIconIdx[0]!=-1){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					if(key_pos==-1) key_pos=0;
					else if(key_pos>0) key_pos--;
					else {
						key_pos=3;
						while(player.HomeIconIdx[key_pos]==-1){
							key_pos--;
						}
					}
				}else if(KeyBoardCode==K_Ok || KeyBoardCode==K_Enter){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);
					if(key_pos==-1) step=1;
					else{
						OS_game=10+player.HomeIconIdx[key_pos];
						step=100;
					}
					game_time=0;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					key_pos=-1;
					game_time=0;
				}
			break;
			case 1:
				ClearSomeSp(0,10);
				show_Date(10);
				if(KeyBoardCode==K_Ok || KeyBoardCode==K_Enter){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);
					key_pos=-1;
					game_time=0;
					step=0;
				}
				if(game_time>=25*6) step=0;
			break;
			case 2:
			break;
		}
		if(Goto_desktop()){
			step=100;
		}
		if(EventFlag){
			OS_game=20;
			step=100;
		}
		if(step==100) break;
	}
}
void Display_pororo(INT16S m)
{	
/*
	if(m==0) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_01.gif");
	else if(m==1) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_02.gif");
	else if(m==2) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_03.gif");
	else if(m==3) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_04.gif");
	else if(m==4) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_05.gif");
	else if(m==5) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_06.gif");
	else if(m==6) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_07.gif");
	else if(m==7) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_08.gif");
	else if(m==8) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_09.gif");
	else if(m==9) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_10.gif");

	else if(m==10) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_11.gif");
	else if(m==11) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_12.gif");
	else if(m==12) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_13.gif");
	else if(m==13) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_14.gif");
	else if(m==14) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_15.gif");
	else if(m==15) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_16.gif");
	else if(m==16) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_17.gif");
	else if(m==17) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_18.gif");
	else if(m==18) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_19.gif");
	else if(m==19) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_20.gif");

	else if(m==20) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_21.gif");
	else if(m==21) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_22.gif");
	else if(m==22) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_23.gif");
	else if(m==23) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_24.gif");
	else if(m==24) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_25.gif");
	else if(m==25) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_26.gif");
	else if(m==26) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_27.gif");
	else if(m==27) LoadTextBmp(C_PPU_TEXT1,"Pororo\\pororo\\h_s_28.gif");
//*/
	//image_decode_Sdram_to_buff(RES_logo_Bg01,TVOutBuff);
//*	
	if(m==0) image_decode_Sdram_to_buff(RES_logo_Bg01,TVOutBuff);
	else if(m==1) image_decode_Sdram_to_buff(RES_logo_Bg02,TVOutBuff);
	else if(m==2) image_decode_Sdram_to_buff(RES_logo_Bg03,TVOutBuff);
	else if(m==3) image_decode_Sdram_to_buff(RES_logo_Bg04,TVOutBuff);
	else if(m==4) image_decode_Sdram_to_buff(RES_logo_Bg05,TVOutBuff);
	else if(m==5) image_decode_Sdram_to_buff(RES_logo_Bg06,TVOutBuff);
	else if(m==6) image_decode_Sdram_to_buff(RES_logo_Bg07,TVOutBuff);
	else if(m==7) image_decode_Sdram_to_buff(RES_logo_Bg08,TVOutBuff);
	else if(m==8) image_decode_Sdram_to_buff(RES_logo_Bg09,TVOutBuff);
	else if(m==9) image_decode_Sdram_to_buff(RES_logo_Bg10,TVOutBuff);

	else if(m==10) image_decode_Sdram_to_buff(RES_logo_Bg11,TVOutBuff);
	else if(m==11) image_decode_Sdram_to_buff(RES_logo_Bg12,TVOutBuff);
	else if(m==12) image_decode_Sdram_to_buff(RES_logo_Bg13,TVOutBuff);
	else if(m==13) image_decode_Sdram_to_buff(RES_logo_Bg14,TVOutBuff);
	else if(m==14) image_decode_Sdram_to_buff(RES_logo_Bg15,TVOutBuff);
	else if(m==15) image_decode_Sdram_to_buff(RES_logo_Bg16,TVOutBuff);
	else if(m==16) image_decode_Sdram_to_buff(RES_logo_Bg17,TVOutBuff);
	else if(m==17) image_decode_Sdram_to_buff(RES_logo_Bg18,TVOutBuff);
	else if(m==18) image_decode_Sdram_to_buff(RES_logo_Bg19,TVOutBuff);
	else if(m==19) image_decode_Sdram_to_buff(RES_logo_Bg20,TVOutBuff);

	else if(m==20) image_decode_Sdram_to_buff(RES_logo_Bg21,TVOutBuff);
	else if(m==21) image_decode_Sdram_to_buff(RES_logo_Bg22,TVOutBuff);
	else if(m==22) image_decode_Sdram_to_buff(RES_logo_Bg23,TVOutBuff);
	else if(m==23) image_decode_Sdram_to_buff(RES_logo_Bg24,TVOutBuff);
	else if(m==24) image_decode_Sdram_to_buff(RES_logo_Bg25,TVOutBuff);
	else if(m==25) image_decode_Sdram_to_buff(RES_logo_Bg26,TVOutBuff);
	else if(m==26) image_decode_Sdram_to_buff(RES_logo_Bg27,TVOutBuff);
	else if(m==27) image_decode_Sdram_to_buff(RES_logo_Bg28,TVOutBuff);//*/
}
extern const INT16U _logo_Palette1[];
extern const INT16U _icon_Sp_16x16_CellData[];
void main_open()
{
	INT16S i,m;
	
	spMalloc(500);
	Palette_ram_cpy( 1,0, (INT32U)_logo_Palette1+512,256);
	Copy16(SpriteCellData,_icon_Sp_16x16_CellData,57344/2);
	//LoadData_1(1,SpriteCellData,"Pororo\\menu\\SpriteCell_cmp.BIN");
	//DecompressData((INT32U)SpriteCellData,(INT32U)RES_logo_PixelData);
	
	
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	Display_pororo(0);
	InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);
	clear_sprite(Spritebuf_number);
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);

	Spi_init();
	
	power_show=0;
	game_time=step=0;
	while(1){
		PaintTFT_cell4();
		Test_MouseXY();
		set_sprite_display_init(1,Mouse_X-8,Mouse_Y-8,(INT32U)justSp_Frame[0]);
		
		if(step==0 || step==1 || step==25 || step==26 || step==27){
			//if(game_time==2) SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[Mail_receive_SFX], 64, 127, SFX_CHANL);
			if(game_time>=40){
				step=(step+1)%8;				
				//Display_pororo(step);
				game_time=0;
			}
		}else{
			if(game_time>=2){
				step=(step+1)%8;				
				//Display_pororo(step);
				game_time=0;
			}
		}
	}

}//*/

