#include "application.h"
#include "data.h"
#include "math.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "photo_buttton.h"
#include "key.h"
#include "photoedit.h"
#include "m2_open.h"

extern const INT16U ALIGN4 video_SPRITE_Palette1[],M2_SPRITE_Palette1[];
void Act_VideoDemo();
void sensor_view(INT16S frame);
void auto_switch_csi_frame();
//=================================================================

const char* m2_Names[]={
	"Pororo\\m2\\M2_open1.gif",
	"Pororo\\m2\\M2_open2.gif",
	"Pororo\\m2\\M2_open3.gif",
	"Pororo\\m2\\M2_open4.gif",
	"Pororo\\m2\\M2_open5.gif",
	"Pororo\\m2\\M2_open6.gif",
//6
	"Pororo\\m2\\M2_1_menu.gif",
	"Pororo\\m2\\M2_2_menu.gif",
//8	
	"Pororo\\m2\\save_1.gif",
	"Pororo\\m2\\save_2.gif",
	
};

void GetBmpData_cmp(INT16S img_mode,INT16S board)
{	
	char path[100];
	INT16S Col_buf[256];	//256 RGB565
	INT8U BMP_data[1024];	//a line data
	
	INT16U* bmp;
	INT8U *tmp;
	INT32S i,j,n,nn,nRet,st;
	INT16U rgb,r,g,b;
	
	bmp=(INT16U*)VideoBuff;			//320x240  RGB data
	tmp=(INT8U*)(ScalerBuff+FRAME_SIZE*2);			//320x240  RGB data
	
	Paintdelay();
	if(img_mode==0){
		sprintf(path,"Pororo\\m3\\frame\\M3_1_frame%02d_cmp.bin",board);
	}else if(img_mode==1){
		sprintf(path,"Pororo\\m3\\frame2\\M3_2_frame%02d_cmp.bin",board);
	}else if(img_mode==11){
		sprintf(path,"Pororo\\m1\\01_party\\01_party-%02d_cmp.bin",board);
	}else if(img_mode==12){
		sprintf(path,"Pororo\\m1\\02_idol\\02_idol-%02d_cmp.bin",board);
	}else if(img_mode==13){
		sprintf(path,"Pororo\\m1\\03_flower\\03_flower-%02d_cmp.bin",board);
	}else if(img_mode==14){
		sprintf(path,"Pororo\\m1\\04_sea\\04_sea-%02d_cmp.bin",board);
	}else if(img_mode==15){
		sprintf(path,"Pororo\\m1\\05_heart\\05_heart-%02d_cmp.bin",board);
	}
	LoadData_1(1,(INT32U)tmp,path);
	
	if(1){
		BMP_data[0]=*(tmp+0x0a);
		BMP_data[1]=*(tmp+0x0b);
		st=BMP_data[0]+BMP_data[1]*0x100;
	
		for(i=0;i<256;i++){
			b=*(tmp+0x36+i*4)>>3;
			g=*(tmp+0x37+i*4)>>2;
			r=*(tmp+0x38+i*4)>>3;
			Col_buf[i]=(r<<11)+(g<<5)+(b<<0);
		}
		for(j=0;j<240;j++){
			n=j*320;
			nn=st+(239-j)*320;
			for(i=0;i<320;i++){
				rgb=*(tmp+nn+i);
				*(bmp+n+i)=Col_buf[rgb];
			}
		}
	}
}


#define BOARD_MAX	15
//=====================================================================
void Main_m02()
{
	INT16S ret,img_board=0;
	INT16S i,m,menu_page;
 
	spMalloc(700+900+300);
	sensorMalloc();
	
	LoadData_1(1,SpriteCellData,"Pororo\\video\\SpriteCell_cmp.BIN");
	INIT_VideoPub(700);
	Palette_ram_cpy( 1,0, M2_SPRITE_Palette1+256*0	,256);
	LoadData_1(1,SpriteCellData+1600*_1K,"Pororo\\m2\\SpriteCell_cmp.BIN");
	gplib_ppu_fade_effect_set(ppu_register_set, 0xff);
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	clear_sprite(Spritebuf_number);
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);

	step=0;
	while(1)
	{
		switch(step){
//--------------------------------------------------view
			case 0:
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,0,0,m2_Open_Frame[0]);
				set_sprite_nCharNum(0,1600);
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[MENU_OPEN_SFX], 64, 127, SFX_CHANL);
				game_time=0;
				step++;
			break;
			case 1:
				#define Tile_Time	3
				if(game_time%Tile_Time==0 && game_time<6*Tile_Time){
					set_sprite_display_init(0,0,0,m2_Open_Frame[game_time/Tile_Time]);
				}else if(game_time>=Tile_Time*6+10){
					menu_page=0;
					step++;
				}
			break;
			case 2:
				clear_sprite(Spritebuf_number);
				LoadTextBmp(C_PPU_TEXT1,m2_Names[6+menu_page]);
				step++;
			break;
			case 3:
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					img_board=menu_page;
					step=9;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=100;
				}else if(KeyBoardCode==K_Left || KeyBoardCode==K_Right){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					menu_page=(menu_page+1)%2;
					LoadTextBmp(C_PPU_TEXT1,m2_Names[6+menu_page]);
				} 
			break;
//-----------------------------------------------------
			case 9:
				InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);
				Change_SensorMode(0);
				step++;
			break;

			case 10:				
			    sensor_buf_1=SensorBuff1;
			    sensor_buf_2=SensorBuff2;
				R_CSI_TG_CTRL0 |=1;
				Wait_Sensor(5);
			case 11:
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,130,172,photo_buttton_Frame[7]);
				if(img_board){
					set_sprite_display_init(1,8,104,photo_buttton_Frame[0]);
					set_sprite_display_init(2,284,104,photo_buttton_Frame[1]);
					GetBmpData_cmp(0,img_board);
				}
				step++;
			break;
			case 12:
				sensor_view(img_board);
				if(img_board && KeyBoardCode==K_Right){
					if(img_board<BOARD_MAX) img_board++;
					else img_board=1;
			 		step=11;
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
				}else if(img_board && KeyBoardCode==K_Left){
					if(img_board>1) img_board--;
					else img_board=BOARD_MAX;
			 		step=11;
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
				}
				if(KeyBoardCode==K_Ok){
					step=20;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					Close_Sensor();
					step=2;
				}
			break;
//--------------------------------------------------take a photo
			case 20:
				clear_sprite(Spritebuf_number);
				//sensor_view(img_board);
 				//Wait_Sensor(5);
 				scaler_task_state=3;
 				while(scaler_task_state);
 				
 				//Close_Sensor();
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[SHOT_SFX], 64, 127, 12);
				Copy32(ScalerBuff,TVOutBuff,FRAME_SIZE/2);
				player.TalkPhotoTime++;
				step=24;
			break;
			case 21:
				Copy32(TVOutBuff,ScalerBuff,FRAME_SIZE/2);
 				if(img_board){
		 			image_scale_RGB(ppu_buf,Sensor_Md,640,480);
		 			Paintdelay();
		 			WriteBmpData(Sensor_Md);
			 		image_scale_YUV(Sensor_Md,ppu_buf,640,480);
			 		Paintdelay();
 				}
				//Sensor_Turn	&&  save
				//buff_Turn(ppu_buf);
				buff_to_save_jpgfile(ppu_buf,Sensor_Md,640,480);
 				
 				step++;
 				m=0;
			break;
			case 22:
				m++;
				if(m<4*7){
					set_sprite_display_init(50,0,0,photo_save_Frame[1+m/2%7]);
				}else{
					if(m==28) SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[BLING_SFX], 64, 80, 12);
					set_sprite_display_init(50,0,0,photo_save_Frame[0]);
				}
				if(m>40){
					step=2;
				}
			break;
//----------------------------------------------------
			case 24:
				clear_sprite(Spritebuf_number);
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					saveType=0;
					step=25;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back || TestMenu_Key()){
					ret=Show_Warning(100,16+2);
					if(ret==0){
						OS_game=12;
						step=24;
					}else if(OS_game!=12)step=100;
					else step=2;
				}
			break;
			case 25:
				m=saveType;
				LoadTextBmp(C_PPU_TEXT1,m2_Names[8+m]);
				step=250;
			break;
			case 250:
				if(KeyBoardCode==K_Ok){
					saveType=m;
					step=26;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					Copy32(TVOutBuff,ScalerBuff,FRAME_SIZE/2);
					step=24;
				}else if(KeyBoardCode==K_Up || KeyBoardCode==K_Down){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					m=(m+1)%2;
					LoadTextBmp(C_PPU_TEXT1,m2_Names[8+m]);
				} 
			break;
			case 26:
				//if(DISKUSED==FS_SD) saveType=1;
				if(saveType==1) Disk_Type=FS_SD;
				else Disk_Type=FS_NAND2;
				if((Disk_Type==2&&SD_Mount!=1)){
					step++;
					break;
				}
				ret=showPop_Save(1);
				if(ret==0){
					if(Disk_Type==FS_SD) Chdir_Root("mimicard_photo");
					else Chdir_Root("M2_photo");
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					step=21;
				}else step=25;
			break;
			case 27:
				ret=Show_Warning(100,13);
				if(ret==0) step=25;
				else if(ret==1)step=25;
				else step=100;
			break;

		}
		PaintTFT_cell4();
		if(step!=24 && Goto_desktop()){
			step=100;
		}
		if(step==100) break;
	}
	sensorFree();
	Close_Sensor();
	InitFileSystem(DISKUSED);
	//Act_SaveNums();
}

