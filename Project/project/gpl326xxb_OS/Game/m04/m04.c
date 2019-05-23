#include "application.h"
#include "data.h"
#include "math.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "photo_buttton.h"
#include "photoedit.h"
#include "m3_stiker.h"
#include "m4_open.h"


extern const INT16U video_SPRITE_Palette1[],M3_SPRITE_Palette1[],M4_SPRITE_Palette1[];
extern INT32U scaler_ptr,decode_mode_ck,BLOCK_ENCODE;
extern VIDEO_INFO video_information;
extern INT16S img_board,deroca_music;
extern int D_pic;
extern const char* m3_MusicNames[];
extern const INT16S Theme_Frame[];
INT16S Video_Yuv=0,Video_First=0;



const char* m4_Names[]={
	"Pororo\\m04\\M4_open1.gif",
	"Pororo\\m04\\M4_open2.gif",
	"Pororo\\m04\\M4_open3.gif",
	"Pororo\\m04\\M4_open4.gif",
	"Pororo\\m04\\M4_open5.gif",
	"Pororo\\m04\\M4_open6.gif",
//6
	"Pororo\\m04\\M4_1_menu.gif",
	"Pororo\\m04\\M4_2_menu.gif",

	"Pororo\\m04\\M4_2-1_menu.gif",
	"Pororo\\m04\\M4_2-2_menu.gif",
//10	
	"Pororo\\m04\\album_1.gif",
	"Pororo\\m04\\album_2.gif",
	
};
INT16S Decora_Video=0;
void View_Decora(INT32U frame)
{
	int i,j,n,nn;
	INT16U *bmp,*Tv,*Sc;
	INT16U m,rgb;
	
	bmp=(INT16U*)VideoBuff;			//320x240  RGB data
	Sc=(INT16U*)ScalerBuff;			//320x240  RGB data
	Tv=(INT16U*)TVOutBuff;			//320x240  RGB data

	img_scale.input_buf1=(void*)(frame);
	img_scale.output_buf1=(void*)(ScalerBuff);//320x240 RGB buff
	image_scale(&img_scale);

	if(img_board){
		for(j=0;j<240;j++){
			n=j*320;
			for(i=0;i<320;i++){
				rgb=*(bmp+n+i);
				if(rgb==0x1f) *(Tv+n+i)=*(Sc+n+i);
				else *(Tv+n+i)=rgb;
			}
		}
	}else{
		for(j=0;j<240;j++){
			n=j*320;
			for(i=0;i<320;i++){
				*(Tv+n+i)=*(Sc+n+i);
			}
		}
	}
}

#define Num_Xsize	16
#define Num_Ypos	213
void show_pagenum(INT16S no,INT16S page)
{
	INT16S x;
//*
	x=160-Num_Xsize*7/2;
	no+=1;
	if(no>999) no=999;
	if(page>999) page=999;
	
	set_sprite_display_init(20,x+Num_Xsize*0,Num_Ypos,Photo_Num_Frame[no/100%10]);
	set_sprite_display_init(21,x+Num_Xsize*1,Num_Ypos,Photo_Num_Frame[no/10%10]);
	set_sprite_display_init(22,x+Num_Xsize*2,Num_Ypos,Photo_Num_Frame[no%10]);
	set_sprite_display_init(23,x+Num_Xsize*3,Num_Ypos,Photo_Num_Frame[10]);
 	set_sprite_display_init(28,x+Num_Xsize*4,Num_Ypos,Photo_Num_Frame[page/100%10]);
	set_sprite_display_init(25,x+Num_Xsize*5,Num_Ypos,Photo_Num_Frame[page/10%10]);
	set_sprite_display_init(26,x+Num_Xsize*6,Num_Ypos,Photo_Num_Frame[page%10]);
	
	/*
	set_sprite_display_init(27,x,Num_Ypos,Photo_Num_Frame[no/1000%10]);
	set_sprite_display_init(20,x+Num_Xsize,Num_Ypos,Photo_Num_Frame[no/100%10]);
	set_sprite_display_init(21,x+Num_Xsize*2,Num_Ypos,Photo_Num_Frame[no/10%10]);
	set_sprite_display_init(22,x+Num_Xsize*3,Num_Ypos,Photo_Num_Frame[no%10]);
	set_sprite_display_init(23,x+Num_Xsize*4,Num_Ypos,Photo_Num_Frame[10]);
 	set_sprite_display_init(24,x+Num_Xsize*5,Num_Ypos,Photo_Num_Frame[page/1000%10]);
 	set_sprite_display_init(28,x+Num_Xsize*6,Num_Ypos,Photo_Num_Frame[page/100%10]);
	set_sprite_display_init(25,x+Num_Xsize*7,Num_Ypos,Photo_Num_Frame[page/10%10]);
	set_sprite_display_init(26,x+Num_Xsize*8,Num_Ypos,Photo_Num_Frame[page%10]);
	//*/
}

void main_m04()
{
	INT16S i,m,page=0,pageno=0;
	INT16S play=0;
	VIDEO_CODEC_STATUS status;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;	
	INT16U ret=0,codeflag=0;
	INT8S end=0,pause=0;
	INT16S vd_time,vd_cur;
	INT16S menu_page=0,menu_page2=0;
	INT16U Decora_Pic=0;

	user_defined_video_codec_entrance();
	video_decode_entrance();
	spMalloc(1600+1200+400);

	INIT_VideoPub(700);
	LoadData_1(1,SpriteCellData,"Pororo\\video\\SpriteCell_cmp.BIN");
	Palette_ram_cpy( 1,0, M4_SPRITE_Palette1+256*0	,256);
	Palette_ram_cpy( 3,0, M3_SPRITE_Palette1+256*2	,256);
	LoadData_1(1,SpriteCellData+1600*_1K,"Pororo\\m04\\SpriteCell_cmp.BIN");
	LoadDRM(0,4,SpriteCellData+3200*_1K,(int*)&DrmAddr[0],m3_MusicNames);

	Init_image_scale(320,240,
					320,240,
					SCALER_CTRL_IN_YUYV,SCALER_CTRL_OUT_RGB565,// SCALER_CTRL_OUT_RGB565
					SensorBuff1,ScalerBuff);

	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	gplib_ppu_fade_effect_set(ppu_register_set, 0);

	if(OS_game==148){
		step=80;
	}else if(OS_game==145){
		step=70;
	}else game_time=step=0;
	Decora_Video=0;
	
	while(1)
	{
		switch(step){
//--------------------------------------------------tile
			case 0:
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,0,0,m4_Open_Frame[0]);
				set_sprite_nCharNum(0,1600);
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[MENU_OPEN_SFX], 64, 127, SFX_CHANL);
				game_time=0;
				step++;
			break;
			case 1:
				#define Tile_Time	3
				if(game_time%Tile_Time==0 && game_time<6*Tile_Time){
					set_sprite_display_init(0,0,0,m4_Open_Frame[game_time/Tile_Time]);
				}else if(game_time>=Tile_Time*6+10){
					clear_sprite(Spritebuf_number);
					Palette_ram_cpy( 1,0, M3_SPRITE_Palette1+256*0	,256);
					LoadData_1(1,SpriteCellData+1600*_1K,"Pororo\\m3\\SpriteCell_cmp.BIN");
					LoadTextBmp(C_PPU_TEXT1,m4_Names[6+menu_page]);
					menu_page=0;
					step++;
				}
			break;
			case 2:
				clear_sprite(Spritebuf_number);
				LoadTextBmp(C_PPU_TEXT1,m4_Names[6+menu_page]);
				step++;
			break;
			case 3:
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(menu_page==0) step=10;
					else step=50;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=100;
				}else if(KeyBoardCode==K_Left || KeyBoardCode==K_Right){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					menu_page=(menu_page+1)%2;
					LoadTextBmp(C_PPU_TEXT1,m4_Names[6+menu_page]);
				} 
			break;
//---------------------------------------------------nomal photo
			case 10:
				saveType=0;
				step=12;
				//clear_sprite(Spritebuf_number);
				//LoadTextBmp(C_PPU_TEXT1,m4_Names[10]);
				//m=0;
				//step++;
			break;
			case 11:
				if(KeyBoardCode==K_Ok){
					saveType=m;
					step++;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=2;
				}else if(KeyBoardCode==K_Up || KeyBoardCode==K_Down){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					m=(m+1)%2;
					LoadTextBmp(C_PPU_TEXT1,m4_Names[10+m]);
				} 
			break;
			case 12:
				Disk_Type=FS_NAND2;
				Chdir_Root("M2_photo");
				step++;
			break;
			case 13:
				if(Disk_Type==FS_SD) get_photo_list(3,4);
				else get_photo_list(0,4);
				pageno=page=0;
				if(File_Max==0) step=19;
				else {
					step++;
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
				}
			break;

			case 14:
				clear_sprite(Spritebuf_number);
				//LoadTextBmp(C_PPU_TEXT1,"AnglePhone\\m06\\M6_1_02_layout.gif");
				show_color_RGB565_bg(255,255,255);
				set_sprite_display_init(0,0,0,photo_ui_Frame[3]);
				showPhotoThum(page);
				step++;
			break;
			case 15:
				if(pageno+page*4>=File_Max) pageno=File_Max-1-page*4;
				set_sprite_display_init(1,5+pageno%2*155,9+pageno/2*111,photo_ui_Frame[4]);
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=20;
					File_No=page*4+pageno;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=2;
				}else if(KeyBoardCode==K_Left){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno) pageno--;
					else if(page){
						pageno=3;
						page--;
						step=14;
					}
				}else if(KeyBoardCode==K_Right){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno<3) pageno++;
					else if(page<File_Page-1){
						pageno=0;
						page++;
						step=14;
					}
				}else if(KeyBoardCode==K_Up){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno>=2) pageno-=2;
					else if(page){
						pageno+=2;
						page--;
						step=14;
					}
				}else if(KeyBoardCode==K_Down){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno<2) pageno+=2;
					else if(page<File_Page-1){
						pageno-=2;
						page++;
						step=14;
					}
				}
			break;

			case 19:
				ret=Show_Warning(100,9);
				if(ret==0) step=2;
				else if(ret==1)step=2;
				else step=100;
			break;
//-------------------------------------------------------
			case 20:
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
				if(play) ClearSomeSp(0,9);
				step++;
			break;
			case 21:
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					page=File_No/4;
					pageno=File_No%4;
					step=14;
				}else if(File_Max>1 && KeyBoardCode==K_Left){	//<<
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					File_No=(File_No+File_Max-1)%File_Max;
					step=20;
				}else if(File_Max>1 && KeyBoardCode==K_Right){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					File_No=(File_No+1)%File_Max;
					step=20;
				}else if( KeyBoardCode==K_Del){
					step=25;
				}
			break;
			case 25:		//del
				ret=Show_Warning(100,2);
				if(ret==1){
					unlink((char *)file_info.f_name);
					File_Max=Del_One_file(File_No,pFileList,File_Max);
					File_SaveMax=File_Max;
					Act_SaveNums();
					if(File_Max%4==0) File_Page=File_Max/4;
					else File_Page=1+File_Max/4;
					m=0;
					step=250;
				}else if(ret==0){
					step=21;
				}else step=100;
			break;
			case 250:		//del
				m++;
				set_sprite_display_init(100,0,0,photo_delete_Frame[m/3%4]);
				if(m>30){
					if(File_Max==0){
						step=19;
					}else{
						File_No=File_No%File_Max;
						step=20;
					}		
				}			
			break;
//---------------------------------------------------Video player
			case 50:
				VideoThum=0;
				Decora_Video=0;
				clear_sprite(Spritebuf_number);
				LoadTextBmp(C_PPU_TEXT1,m4_Names[8+menu_page2]);
				step++;
			break;
			case 51:
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					saveType=menu_page2;
					step++;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=2;
				}else if(KeyBoardCode==K_Left || KeyBoardCode==K_Right){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					menu_page2=(menu_page2+1)%2;
					LoadTextBmp(C_PPU_TEXT1,m4_Names[8+menu_page2]);
				}
			break;
			case 52:
				Disk_Type=FS_NAND2;
				step++;
			break;
			case 53:
				if(saveType==0) get_photo_list(1,4);
				else{
					get_photo_list(4,4);
					File_Max=Load_DecoraData(0,File_Max);
					if(File_Max%4==0) File_Page=File_Max/4;
					else File_Page=1+File_Max/4;
				}
				pageno=page=0;
				if(File_Max==0) step=58;
				else step++;
			break;
			case 54:
				clear_sprite(Spritebuf_number);
				//LoadTextBmp(C_PPU_TEXT1,"AnglePhone\\m06\\M6_1_02_layout.gif");
				show_color_RGB565_bg(255,255,255);
				set_sprite_display_init(0,0,0,photo_ui_Frame[3]);
				showVideoThum(page);
				step++;
			break;
			case 55:
				if(pageno+page*4>=File_Max) pageno=File_Max-1-page*4;
				set_sprite_display_init(1,5+pageno%2*155,9+pageno/2*111,photo_ui_Frame[4]);
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=60;
					File_No=page*4+pageno;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=50;
				}else if(KeyBoardCode==K_Left){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno) pageno--;
					else if(page){
						pageno=3;
						page--;
						step=54;
					}
				}else if(KeyBoardCode==K_Right){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno<3) pageno++;
					else if(page<File_Page-1){
						pageno=0;
						page++;
						step=54;
					}
				}else if(KeyBoardCode==K_Up){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno>=2) pageno-=2;
					else if(page){
						pageno+=2;
						page--;
						step=54;
					}
				}else if(KeyBoardCode==K_Down){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(pageno<2) pageno+=2;
					else if(page<File_Page-1){
						pageno-=2;
						page++;
						step=54;
					}
				}
			break;

			case 58:
				ret=Show_Warning(100,10);
				if(ret==0) step=50;
				else if(ret==1)step=50;
				else step=100;
			break;
//---------------------------------------------
			case 60:
				clear_sprite(Spritebuf_number);
				SPU_StopChannel(MUSIC_CHCHANL);
				D_pic=img_board=0;
				Decora_Video=0;
				if(saveType==0){
					Decora_Pic=Get_file_info(File_No,pFileList,File_Max);
					InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,3);//BMP_RGB565
					VideoThum=0;
					ret=Get_video_Info(0);
				}else{
					Decora_Pic=Get_decora_info(File_No,pFileList,File_Max);
					if(Decora_Pic&0x8000) {
						Load_DecoraData(1,Decora_Pic&0x7fff);
						step=66;
						break;
					}else if(file_info.f_name[4]>='a') img_board=11+(file_info.f_name[4]-'a');
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
				if(vd_time>60) vd_time=60;
				//codeflag=1;
				Decora_Video=0;
				VideoThum=0;
				Video_Yuv=0;
				end=pause=1;
				step++;
			break;

			case 61:
				if(saveType==0 || D_pic) Show_CodeTime(0,vd_time);
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					page=File_No/4;
					pageno=File_No%4;
					step=54;
				}
				if(KeyBoardCode==K_Ok || KeyBoardCode==K_Play){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					codeflag=1;
					pause=0;
					end=0;
					step++;
					game_time=0;
					clear_sprite(Spritebuf_number);
				}else if(KeyBoardCode==K_Left || KeyBoardCode==K_LastVideo){	//<<
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					File_No=(File_No+File_Max-1)%File_Max;
					step=60;
				}else if(KeyBoardCode==K_Right || KeyBoardCode==K_NextVideo){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					File_No=(File_No+1)%File_Max;
					step=60;
				}else if( KeyBoardCode==K_Del){
					step=65;
				}
			break;
			case 62:
				vd_cur=video_decode_get_current_time();
				if(vd_cur>60) vd_cur=60;
				if(saveType==0 || D_pic) Show_CodeTime(0,vd_cur);
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					codeflag=2;
					step=60;
					break;
				}else if(KeyBoardCode==K_Left || KeyBoardCode==K_LastVideo){	//<<
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					codeflag=2;
					//File_No=(File_No+File_Max-1)%File_Max;
					step=60;
					break;
				}else if(KeyBoardCode==K_Right || KeyBoardCode==K_NextVideo){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					codeflag=2;
					File_No=(File_No+1)%File_Max;
					step=60;
					break;
				}else if(end==1){
					//File_No=(File_No+1)%File_Max;
					step=60;
					break;
				}
				if(pause==0){
					sleeptime=WAITINGTIME;
					if(D_pic) {
						Show_StikeAni(5);
						if(deroca_music){
							if(SPU_Get_SingleChannel_Status(MUSIC_CHCHANL)==0){
								SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[deroca_music-1], 64, 127, MUSIC_CHCHANL);
							}
						}
					}
					//if(game_time<20) set_sprite_display_init(50,120,80,photo_buttton_Frame[5]);
					//else 
					set_sprite_display_init(50,120,80,0);
					if(KeyBoardCode==K_Ok || KeyBoardCode==K_Play){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
						SPU_StopChannel(MUSIC_CHCHANL);
						codeflag=3;
						pause=1;
					}
					break;
				}else{
					if(end==0)set_sprite_display_init(50,120,80,photo_buttton_Frame[5]);
					else set_sprite_display_init(50,120,80,photo_buttton_Frame[2]);
					if(KeyBoardCode==K_Ok || KeyBoardCode==K_Play){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
						codeflag=3;
						pause=0;
					}
				}
			break;

			case 66:
				InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);//BMP_RGB565
				set_sprite_display_init(50,120,80,photo_buttton_Frame[2]);
				set_sprite_display_init(1,8,104,photo_buttton_Frame[0]);
				set_sprite_display_init(2,284,104,photo_buttton_Frame[1]);
				show_pagenum(File_No,File_Max);
				Show_StikeAni(5);
				VideoThum=1;
				Decora_Video=1;
				ret=Get_video_Info(0);
				vd_time=video_information.TotalDuration;
				PlayPhoto_Frame(1,img_board);
				end=pause=1;
				step=61;
			break;

			case 65:		//del
				ret=Show_Warning(100,2);
				if(ret==1){
					if(D_pic){
						Del_DecoraData(Decora_Pic&0x7fff,0);
					}else{
						if(saveType==0) Del_DecoraData(0,Decora_Pic);
						unlink((char *)file_info.f_name);
					}
					File_Max=Del_One_file(File_No,pFileList,File_Max);
					File_SaveMax=File_Max;
					Act_SaveNums();
					if(File_Max%4==0) File_Page=File_Max/4;
					else File_Page=1+File_Max/4;
					step=650;
					m=0;
				}else if(ret==0) step=61;
				else step=100;
			break;
			case 650:		//del
				m++;
				set_sprite_display_init(100,0,0,photo_delete_Frame[m/3%4]);
				if(m>30){
					if(File_Max==0){
						step=58;
					}else{
						File_No=File_No%File_Max;
						step=60;
					}
				}			
			break;
//===================================================================
/*			case 70:
				Disk_Type=FS_NAND2;
				if(saveType==0) {
					Chdir_Root("M1-1_video");
				}else {
					Chdir_Root("M1-2_video");
				}

				clear_sprite(Spritebuf_number);
				InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,3);//BMP_RGB565
				set_sprite_display_init(0,120,80,photo_buttton_Frame[2]);

				ret=Get_video_Info(0);
				end=pause=1;
				OS_game=11;
				step++;
			break;
			case 71:
				if(pause==0){
					SetWaveIo(1);
					set_sprite_display_init(0,120,80,0);
					if(KeyBoardCode==K_Play){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
						codeflag=3;
						pause=1;
					}
				}else{
					if(end==0)set_sprite_display_init(0,120,80,photo_buttton_Frame[5]);
					else set_sprite_display_init(0,120,80,photo_buttton_Frame[2]);
					if(KeyBoardCode==K_Play){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
						if(end){
							codeflag=1;
							pause=0;
							end=0;
						}else{
							codeflag=3;
							pause=0;
						}
						game_time=0;
					}
				}
				if(KeyBoardCode==K_Ok){
					game_time=0;
					codeflag=2;
					step++;
				} 
			break;
			case 72:
				if(game_time<4*7){
					set_sprite_display_init(0,0,0,photo_save_Frame[1+game_time/2%7]);
				}else set_sprite_display_init(0,0,0,photo_save_Frame[0]);
				if(game_time>30){
					step=100;
					OS_game=115;
				}
			break;//*/
//===================================================================
/*			case 80:
				InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);//BMP_RGB565
				set_sprite_display_init(50,120,80,photo_buttton_Frame[2]);
				Show_StikeAni(5);
				VideoThum=1;
				ret=Get_video_Info(0);
				PlayPhoto_Frame(1,img_board);
				//show_Decora();
				end=pause=1;
				Decora_Video=1;
				OS_game=13;
				step++;
			break;
			case 81:
				if(pause==0){
					Show_StikeAni(5);
					set_sprite_display_init(50,120,80,0);
					if(KeyBoardCode==K_Play){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
						codeflag=3;
						pause=1;
					}
				}else{
					if(end==0)set_sprite_display_init(50,120,80,photo_buttton_Frame[5]);
					else set_sprite_display_init(50,120,80,photo_buttton_Frame[2]);
					if(KeyBoardCode==K_Play){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
						if(end){
							codeflag=1;
							pause=0;
							end=0;
						}else{
							codeflag=3;
							pause=0;
						}
						game_time=0;
					}
				}
				if(KeyBoardCode==K_Ok){
					game_time=0;
					codeflag=2;
					step++;
				} 
			break;
			case 82:
				if(game_time==5) Save_DecoraData();
				if(game_time<4*7){
					set_sprite_display_init(50,0,0,photo_save_Frame[1+game_time/2%7]);
				}else set_sprite_display_init(50,0,0,photo_save_Frame[0]);
				if(game_time>30){
					step=100;
					OS_game=135;
				}
			break;//*/
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
		if(Goto_desktop()){
			step=100;
		}
		if(step==100) break;
	}
	Copy32(TVOutBuff,TV_buffer[0],FRAME_SIZE/2);
	InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);//BMP_RGB565
	video_decode_stop();
	video_decode_exit();
	InitFileSystem(DISKUSED);
}
