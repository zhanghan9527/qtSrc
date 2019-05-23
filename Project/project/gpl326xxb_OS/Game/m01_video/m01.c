#include "application.h"
#include "data.h"
#include "gplib.h"
#include "math.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "photoedit.h"
#include "photo_buttton.h"
#include "pub_WarningSp.h"
#include "m1_open.h"

extern const INT16U video_SPRITE_Palette1[],M1_SPRITE_Palette1[];
extern INT16S img_board;
INT16S VideoPub_Idx;

const char* m1_MusicNames[]={
	"Pororo\\SFX\\start-recording.drm",
	"Pororo\\SFX\\01_party.drm",
	"Pororo\\SFX\\02_idol.drm",
	"Pororo\\SFX\\03_flower.drm",
	"Pororo\\SFX\\04_sea.drm",
	"Pororo\\SFX\\05_heart.drm",
};
extern const char* m2_Names[];
const char* m1_Names[]={
	"Pororo\\m1\\M1_open_guide_p1.gif",
	"Pororo\\m1\\M1_open_guide_p2.gif",
	"Pororo\\m1\\M1_open_guide_p3.gif",
	"Pororo\\m1\\M1_open_guide_p4.gif",
	"Pororo\\m1\\M1_open_guide_p5.gif",
	"Pororo\\m1\\M1_open_guide_p6.gif",
//6
	"Pororo\\m1\\M1_1_menu.gif",
	"Pororo\\m1\\M1_2_menu.gif",
//8	
	"Pororo\\m1\\M1_2_menu_01_party.gif",
	"Pororo\\m1\\M1_2_menu_02_idol.gif",
	"Pororo\\m1\\M1_2_menu_04_flower.gif",
	"Pororo\\m1\\M1_2_menu_03_sea.gif",
	"Pororo\\m1\\M1_2_menu_05_heart.gif",
};
const INT16S Warning_Type[]={
	1,1,1,1,1,	1,1,1,0,0,
	0,0,0,0,1,	1,
//16
	1,1,1,1,1,	1,1,1,0,0,
};
extern INT16U MailShow;
void Show_mailRev()
{
	if(MailShow){
		MailShow--;
		set_sprite_display_init(201,100,208,Pub_mail_Frame[0]);
		set_sprite_nCharNum(201,VideoPub_Idx);
	}else set_sprite_disable(201);
}
INT16S Show_Warning(INT16S sp,INT16S No)
{
	INT16S ret,m=0;
	
	set_sprite_disable(sp);
	if(No!=14)SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[WARNING_SFX], 64, 127, 12);
	
	game_time=0;
	while(1){
		if(No<16) {
			set_sprite_display_init(sp,0,0,pub_WarningSp_Frame[No+m]);
			set_sprite_nCharNum(sp,VideoPub_Idx);
		}else set_sprite_display_init(sp,0,0,WarningSp_Frame[No+m-16]);
		if(No==4 || No==8 ) {
			PaintTFT_Power();
			Sleep();
		}else PaintTFT_cell4();
		if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
			ret=0;
			break;
		}
		if(Warning_Type[No]==0){
			if(game_time>=50) {
				ret=0;
				break;
			}
		}else{
			if(KeyBoardCode==K_Left || KeyBoardCode==K_Right){
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
				m=(m+1)%2;
			}else if(KeyBoardCode==K_Ok || KeyBoardCode==K_Enter){
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);
				ret=m;
				break;
			}
		}
		
		//if(No!=4 && Goto_desktop_Home()){
		//	ret=-1;
		//	break;
		//}
	}
	set_sprite_disable(sp);
	KeyBoardCode=0;
	return ret;
}
extern int counter;
void show_Loading()
{
	int dt;
	
	dt=((tiny_counter_get() | 0x10000) - counter) & (0xFFFF);
	if(dt>=40*375){
		if(game_time%3==0){
			set_sprite_display_init(200,0,0,Photo_Loading_Frame[game_time/3%7]);
			paint_ppu_spriteram_cell4(ppu_register_set,Sprite_Coordinate_320X240,Left_Top_coordinate_mode,Spritebuf_number);
			gplib_ppu_go_and_wait_done(ppu_register_set);
		}
		game_time++;
		counter=tiny_counter_get();
	}
}
const INT16S Video_FrameNum[]={
	79,97,124,99,90
};
const INT16S Video_MaxTime[]={
	100,100,124,100,100
};
INT16S Video_FrameNo=0;
INT16S GetVideo_Frame(INT16S board,int time)
{
	int i,j,n,nn;
	INT16U *bmp,*Tv;
	INT16U m,rgb;
	
	bmp=(INT16U*)VideoBuff;			//320x240  RGB data
	Tv=(INT16U*)TVOutBuff;			//320x240  RGB data

	m=time*Video_FrameNum[board-1]/32500;
	if(m>=Video_FrameNum[board-1]) m=Video_FrameNum[board-1]-1;
	if(m!=Video_FrameNo){
		GetBmpData_cmp(10+board,1+m);
		Video_FrameNo=m;
		return 1;
	}else return 0;
}

void PlayVideo_Frame(INT16S board,INT16S time)
{
	int i,j,n,nn;
	INT16U *bmp,*Tv;
	INT16U m,rgb;
	
	bmp=(INT16U*)VideoBuff;			//320x240  RGB data
	Tv=(INT16U*)TVOutBuff;			//320x240  RGB data

	m=time*Video_FrameNum[board-1]/Video_MaxTime[board-1];
	if(m!=Video_FrameNo && m<Video_FrameNum[board-1]){
		GetBmpData_cmp(10+board,1+m);
		Video_FrameNo=m;

		for(j=0;j<166;j++){
			n=(j*240/166)*320;
			nn=50+(j+20)*320;
			for(i=0;i<220;i++){
				rgb=*(bmp+n+i*320/220);
				if(rgb==0x1f) *(Tv+nn+i)=0xC618;
				else *(Tv+nn+i)=rgb;
			}
		}
	}
}

void Show_CodeTime(INT16S mode,INT16S time)
{
	if(mode && (game_time/12%2) ) set_sprite_display_init(60,238,6,photo_Time_Frame[10]);
	else set_sprite_display_init(60,238,6,0);
	
	set_sprite_display_init(61,260,6,photo_Time_Frame[time/60]);
	set_sprite_display_init(62,272,6,photo_Time_Frame[11]);
	set_sprite_display_init(63,284,6,photo_Time_Frame[time%60/10]);
	set_sprite_display_init(64,300,6,photo_Time_Frame[time%10]);
}
void INIT_VideoPub(INT16S buff)
{
	Palette_ram_cpy( 0,0, video_SPRITE_Palette1+256*0	,256);
	LoadData_1(1,SpriteCellData+buff*_1K,"Pororo\\video\\pubSpriteCell_cmp.BIN");
	VideoPub_Idx=buff;
}
//=================================================================
void Main_m01()
{
	int i;
	INT16S m;
	char path[64];
	char board_path[2];
	INT32S  ret;   
	INT64U  disk_size;   
	MEDIA_SOURCE   src;   
	struct f_info fin;
	INT16S pause=0,codeflag=0,codetime=0;
	int codeT=0;
	INT16S board;
	INT16S menu_page=0;
	INT16S video_mode=0;

	spMalloc(700+900+600);
	Palette_ram_cpy( 1,0, M1_SPRITE_Palette1+256*0	,256);
	
	INIT_VideoPub(700);
	LoadData_1(1,SpriteCellData,"Pororo\\video\\SpriteCell_cmp.BIN");
	LoadData_1(1,SpriteCellData+1600*_1K,"Pororo\\m1\\SpriteCell_cmp.BIN");
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	clear_sprite(Spritebuf_number);
	Init_Video();
	mic_set_pga_gain(1,8);
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);

	img_board=0;
	if(OS_game==115){
		OS_game=11;
		step=4+video_mode;
		board=video_mode;
	}else step=0;
	while(1)
	{
		switch(step){
//--------------------------------------------------tile
			case 0:
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,0,0,m1_Open_Frame[0]);
				set_sprite_nCharNum(0,1600);
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[MENU_OPEN_SFX], 64, 127, SFX_CHANL);
				game_time=0;
				step++;
			break;
			case 1:
				#define Tile_Time	3
				if(game_time%Tile_Time==0 && game_time<6*Tile_Time){
					set_sprite_display_init(0,0,0,m1_Open_Frame[game_time/Tile_Time]);
				}else if(game_time>=Tile_Time*6+10){
					clear_sprite(Spritebuf_number);
					LoadTextBmp(C_PPU_TEXT1,m1_Names[6+0]);
					LoadDRM(0,6,SpriteCellData+2200*_1K,(int*)&DrmAddr[0],m1_MusicNames);
					menu_page=0;
					step++;
				}
			break;
			case 2:					//-------------video Mode  1/2
				clear_sprite(Spritebuf_number);
				LoadTextBmp(C_PPU_TEXT1,m1_Names[6+menu_page]);
				step++;
			break;
			case 3:
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					video_mode=menu_page;
					saveType=0;
					img_board=board=0;
					step=7;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=100;
				}else if(KeyBoardCode==K_Left || KeyBoardCode==K_Right){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					menu_page=(menu_page+1)%2;
					LoadTextBmp(C_PPU_TEXT1,m1_Names[6+menu_page]);
				} 
			break;
			case 7:									//SD & NAND
				clear_sprite(Spritebuf_number);
				m=saveType;
				LoadTextBmp(C_PPU_TEXT1,m2_Names[8+m]);
				step++;
			break;
			case 8:
				if(KeyBoardCode==K_Ok){
					saveType=m;
					if((saveType==1&&SD_Mount!=1)){
						step=9;
						break;
					}
					if(saveType==1){
						Disk_Type=FS_SD;
						Chdir_Root("mimicard_video");
					}else{
						Disk_Type=FS_NAND2;
						if(video_mode==0){
							Chdir_Root("M1-1_video");
						}else {
							Chdir_Root("M1-2_video");
						}
					}
					disk_size=vfsFreeSpace(Disk_Type);
					if(video_mode==0 && disk_size>=0x100000){
						step=4+video_mode;
						board=video_mode;
					}else if(video_mode==1 && disk_size>=0x280000){
						step=4+video_mode;
						board=video_mode;
					}else{
						step=30;
						break;
					}
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=2;
				}else if(KeyBoardCode==K_Up || KeyBoardCode==K_Down){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					m=(m+1)%2;
					LoadTextBmp(C_PPU_TEXT1,m2_Names[8+m]);
				} 
			break;
			case 9:
				ret=Show_Warning(100,13);
				if(ret==0) step=7;
				else if(ret==1)step=7;
				else step=100;
			break;
//-----------------------------------------------------
			case 4:
				if(board){
					Video_FrameNo=-1;
					GetVideo_Frame(board,0);
				}
				Start_Video();
				step=10;
			break;

			case 5:
				clear_sprite(Spritebuf_number);
				LoadTextBmp(C_PPU_TEXT1,m1_Names[8+board-1]);
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[board], 64, 127, MUSIC_CHCHANL);
				game_time=0;
				step++;
			break;
			case 6:
				if(game_time<Video_MaxTime[board-1]) PlayVideo_Frame(board,game_time);
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					SPU_StopChannel(MUSIC_CHCHANL);
					step=4;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					SPU_StopChannel(MUSIC_CHCHANL);
					step=2;
				}else if(KeyBoardCode==K_Right){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(board<5) board++;
					else board=1;
					step--;
				}else if(KeyBoardCode==K_Left){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(board>1) board--;
					else board=5;
					step--;
				} 
			break;
//===========================================================
			case 10:
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,120,152,photo_buttton_Frame[2]);
				set_sprite_display_init(1,0,-20,photo_ui_Frame[0]);
				step++;
			break;
			case 11:
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[0], 64, 127, SFX_CHANL);												
					step=20;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					video_encode_preview_stop();
					if(video_mode)step=5;
					else step=2;
				}
			break;
			case 20:
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,120,152,photo_buttton_Frame[5]);
				while(1){
					if(SPU_Get_SingleChannel_Status(SFX_CHANL)==0) break;
				}
				if(board) SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[board], 64, 127, MUSIC_CHCHANL);
				codeflag=1;
				game_time=codeT=codetime=0;
				img_board=board;
				pause=0;
				step++;
			break;
			case 21:				
				if(pause==0){
					sleeptime=WAITINGTIME;
					set_sprite_display_init(0,120,152,photo_buttton_Frame[5]);
					codeT+=100;
					if(img_board) ;//codetime=codeT/2400;
					else{
						codetime=codeT/2380;
						Show_CodeTime(1,codetime);
					}
					if((video_encode_status() != VIDEO_CODEC_PROCESSING) || (video_mode==0 && (codetime>=60 || (disk_size-codetime*80000)<0x80000) ) || (video_mode && (SPU_Get_SingleChannel_Status(MUSIC_CHCHANL)==0)) ){
						SPU_StopChannel(MUSIC_CHCHANL);
						codeflag=2;
						step++;
						break;
					}
					if(img_board) codeT+=GetVideo_Frame(img_board,codeT) * 300;
				}else{
					set_sprite_display_init(0,120,152,photo_buttton_Frame[2]);
				}
				if(img_board==0){
					if(KeyBoardCode==K_Ok && codetime>=2){
						SPU_StopChannel(MUSIC_CHCHANL);
						codeflag=2;
						step++;
					}else if(KeyBoardCode==K_Play){
						codeflag=3;
						pause=(pause+1)%2;
					}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back || TestMenu_Key()){
						if(pause==0){
							codeflag=3;
							pause=(pause+1)%2;
						}
						step=210;
					}
				}
			break;
			case 210:
				ret=Show_Warning(100,16+0);
				if(ret==0){
					OS_game=11;
					step=21;
				}else{
					codeflag=2;
					step++;
				}
			break;
			case 211:
				video_encode_preview_stop();
				if(saveType==1){
					Disk_Type=FS_SD;
					Chdir_Root("mimicard_video");
				}else{
					Disk_Type=FS_NAND2;
					if(video_mode==0){
						Chdir_Root("M1-1_video");
					}else {
						Chdir_Root("M1-2_video");
					}
				}
				disk_safe_exit(Disk_Type);
				ret=unlink((char *)path);
				VID_Nums--;
				Act_SaveNums();
				if(OS_game==11) step=2;
				else step=100;
			break;
			case 22:
				R_CSI_TG_CTRL0 &=~1;
				set_sprite_disable(0);
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);
				DrvNand_flush_allblk();
				player.RecodeTime++;
				//sprintf((char *)(file_info.f_name), (const char *)"%s",path);
				//OS_game=145;
				//step=100;
				m=0;
				step++;
			break;

			case 23:
				m++;
				if(m<4*7){
					set_sprite_display_init(50,0,0,photo_save_Frame[1+m/2%7]);
				}else{
					if(m==28) SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[BLING_SFX], 64, 127, SFX_CHANL);
					set_sprite_display_init(50,0,0,photo_save_Frame[0]);
				}
				if(m>40){
					video_encode_preview_stop();
					step=2;
				}
			break;

			case 30:
				ret=Show_Warning(100,11);
				if(ret==-1) step=100;
				else step=7;
			break;
		}
//--------------------------------------------------------------
		if(codeflag==1)      
		{ 												// Start to decode the specified file   
			//video_type_select(ENCODE_MJPEG);            
			if(video_encode_status() == VIDEO_CODEC_PROCESSING)         
				continue;            
						                  
			OSTimeDly(10); 

			disk_size=vfsFreeSpace(Disk_Type);
			if(disk_size>0x10000){
				while(1){
					board_path[0]=('a'+img_board-1);
					board_path[1]='\0';
					if(saveType==1){
						if(video_mode) sprintf((char *)(path), (const char *)"video_%s%04d.avi",board_path,VidSd_Nums);
						else sprintf((char *)(path), (const char *)"video_%04d.avi",VidSd_Nums);
					}else{
						if(video_mode) sprintf((char *)(path), (const char *)"M1-2%s_%04d.avi",board_path,VID_Nums);
		        		else sprintf((char *)(path), (const char *)"M1-1_%04d.avi",VID_Nums);
		        	}
					ret = _findfirst((char *)(path),&fin,D_ALL);
					if(ret<0){
						break;	
					}else if(saveType==1) VidSd_Nums++;
					else VID_Nums++;
				}
				VID_Nums++;
				Act_SaveNums();
				if(saveType==1){
					Disk_Type=FS_SD;
					Chdir_Root("mimicard_video");
				}else{
					Disk_Type=FS_NAND2;
					if(video_mode==0){
						Chdir_Root("M1-1_video");
					}else {
						Chdir_Root("M1-2_video");
					}
				}

				src.type = SOURCE_TYPE_FS;
				src.Format.VideoFormat = MJPEG;
				src.type_ID.FileHandle = open((char *)(path), O_WRONLY|O_CREAT|O_TRUNC);         
				if(src.type_ID.FileHandle < 0)         
				{
					DBG_PRINT("file open fail\r\n");            
					disk_safe_exit(Disk_Type);            
				}
				video_encode_start(src);
			}else step=30;
		}
//------------------------------------------------
		if(codeflag==2)      
		{      						// Stop decoding the current file      
			if(Video_Coding())         
			{
				DBG_PRINT("video_encode_stop\r\n");        
				video_encode_stop();
				OSTimeDly(10);             
			}
		}
		if(codeflag==3)							// pause & resume
		{      
			if(video_encode_status() == VIDEO_CODEC_PROCESS_PAUSE)         
			{         
				DBG_PRINT("video encode resume\r\n");            
				video_encode_resume();            
			}         
			else if(video_encode_status() == VIDEO_CODEC_PROCESSING)         
			{         
				DBG_PRINT("video encode pause\r\n");            
				video_encode_pause();            
			}
			OSTimeDly(10);
		}
//----------------------------------------exit video encoding demo
		if(codeflag) codeflag=0;
		if(step<10 || step==23) PaintTFT_cell4();
		else PaintTFT_go();				//video_encode_display_frame_ready
		if(step!=21 && Goto_desktop()){
			step=100;
		}
		if(step==100)
		{
			if(Video_Coding()){
				video_encode_stop();
			}
			break;
		}
	}
	video_encode_preview_stop();
	video_encode_exit();        // Release the resources needed by Video encoder
	closeSensor();
    InitFileSystem(DISKUSED);
}
