#include "application.h"
#include "data.h"
#include "math.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "photo_buttton.h"
#include "m3_stiker.h"
#include "photoedit.h"
#include "m3_open.h"


extern const INT16U video_SPRITE_Palette1[],M3_SPRITE_Palette1[];
extern INT32U scaler_ptr,decode_mode_ck,BLOCK_ENCODE;
extern VIDEO_INFO video_information;
extern INT16S img_board;
extern int D_pic;
#define StikerMax	5
INT16S deroca_music=0,stikerSp[5],stikerX[5],stikerY[5];

const char* m3_MusicNames[]={
	"Pororo\\SFX\\M3_BGM1.drm",
	"Pororo\\SFX\\M3_BGM2.drm",
	"Pororo\\SFX\\M3_BGM3.drm",
	"Pororo\\SFX\\M3_BGM4.drm",
};

const char* m3_Names[]={
	"Pororo\\m3\\M3_open_guide_p1.gif",
	"Pororo\\m3\\M3_open_guide_p2.gif",
	"Pororo\\m3\\M3_open_guide_p3.gif",
	"Pororo\\m3\\M3_open_guide_p4.gif",
	"Pororo\\m3\\M3_open_guide_p5.gif",
	"Pororo\\m3\\M3_open_guide_p6.gif",
//6
	"Pororo\\m04\\M4_1_menu.gif",
	"Pororo\\m04\\M4_2_menu.gif",
//8
	"Pororo\\m3\\save_1.gif",
	"Pororo\\m3\\save_2.gif",
//10	
	"Pororo\\m04\\album_1.gif",
	"Pororo\\m04\\album_2.gif",
//12
	"Pororo\\m3\\M3_2_bgm_0.gif",
	"Pororo\\m3\\M3_2_bgm_1.gif",
	"Pororo\\m3\\M3_2_bgm_2.gif",
	"Pororo\\m3\\M3_2_bgm_3.gif",
	"Pororo\\m3\\M3_2_bgm_4.gif",
	
};
#define Decora_Len	19
INT16S Load_DecoraData(INT16S mode,INT16S num)
{
	char path[100];
	INT16S fd,i,nRet;
	INT16S DataBuff[Decora_Len];
	int len;
	int j,no;
	INT16U* list=(INT16U*)pFileList;
	
	sprintf(path,"Pororo\\user\\Decora.ini");
	AddRootPath(path);

	fd = open(rootPath, O_RDONLY);
	if(fd==-1) return num;

	len=lseek(fd,0,SEEK_END);
	lseek(fd,0,SEEK_SET);
	while(len>=Decora_Len*2){
		nRet = read(fd,(INT32U)DataBuff,Decora_Len*2);
		if(mode){
			if(DataBuff[1]==num){
				D_pic=DataBuff[0];
				img_board=DataBuff[2];
				deroca_music=DataBuff[3];
				for(i=0;i<5;i++){
					stikerSp[i]=DataBuff[4+i];
					stikerX[i]=DataBuff[9+i];
					stikerY[i]=DataBuff[14+i];
				}
				break;
			}
		}else if(DataBuff[1]){
			no=DataBuff[1];
			for(i=0;i<num;i++){
				if(no<*(list+2*i+1)){
					break;
				}
			}
			if(i!=num){
				for(j=num;j>i;j--){
					*(list+2*j)=*(list+2*j-2);
					*(list+2*j+1)=*(list+2*j-1);
				}
			}
			*(list+2*i)=DataBuff[0] | 0x8000;
			*(list+2*i+1)=no;
			num++;
		}
		len-=Decora_Len*2;
	}
//	File_Max
	nRet = close(fd);
	return num;
}
INT16S Get_decora_info(INT16U picnum,INT32U listAddr,INT16U Max)
{
	INT16U i,ret;
	INT16U* list=(INT16U*)listAddr;
	INT16U vid;

	if(picnum>=Max) return -1;
	i=Max-picnum-1;
	vid=*(list+2*i);
	if(vid>=0x8000){
		sprintf((char*)file_info.f_name,"B:\\M1-1_video\\M1-1_%04d.avi",vid&0x7fff);
		ret=*(list+2*i+1) | 0x8000;
	}else{
		seekdir(*(list+2*i));
		ret = _findnext(&file_info);
		if(ret==0) return *(list+2*i+1);
	}
	return ret;
}

void Save_DecoraData()
{
	char path[100];
	INT16S fd,i,nRet;
	INT16S DataBuff[Decora_Len];
	int len;
	
	sprintf(path,"Pororo\\user\\Decora.ini");
	AddRootPath(path);

	fd = open(rootPath, O_CREAT|O_RDWR);
	if(fd==-1) return;
	len=lseek(fd,0,SEEK_END);
	lseek(fd,0,SEEK_SET);
	while(len>=Decora_Len*2){
		nRet = read(fd,(INT32U)DataBuff,Decora_Len*2);
		if(DataBuff[0]==0){
			lseek(fd,-Decora_Len*2,SEEK_CUR);
			break;
		}
		len-=Decora_Len*2;
	}
	DataBuff[0]=D_pic;
	DataBuff[1]=VID_Nums;
	DataBuff[2]=img_board;
	DataBuff[3]=deroca_music;
	for(i=0;i<5;i++){
		DataBuff[4+i]=stikerSp[i];
		DataBuff[9+i]=stikerX[i];
		DataBuff[14+i]=stikerY[i];
	}
	
	nRet = write(fd,(INT32U)DataBuff,Decora_Len*2);
	nRet = close(fd);

	VID_Nums++;
	Act_SaveNums();
}

void Del_DecoraData(INT16S no,INT16S pic)
{
	char path[100];
	INT16S fd,i,nRet;
	INT16S DataBuff[Decora_Len];
	int len;
	
	sprintf(path,"Pororo\\user\\Decora.ini");
	AddRootPath(path);

	fd = open(rootPath, O_CREAT|O_RDWR);
	if(fd==-1) return;
	len=lseek(fd,0,SEEK_END);
	lseek(fd,0,SEEK_SET);
	while(len>=Decora_Len*2){
		nRet = read(fd,(INT32U)DataBuff,Decora_Len*2);
		if(no && DataBuff[1]==no){
			lseek(fd,-Decora_Len*2,SEEK_CUR);
			for(i=0;i<Decora_Len;i++) DataBuff[i]=0;
			nRet = write(fd,(INT32U)DataBuff,Decora_Len*2);
			break;
		}else if(pic && DataBuff[0]==pic){
			lseek(fd,-Decora_Len*2,SEEK_CUR);
			for(i=0;i<Decora_Len;i++) DataBuff[i]=0;
			nRet = write(fd,(INT32U)DataBuff,Decora_Len*2);
			//break;
		}
		len-=Decora_Len*2;
	}
	nRet = close(fd);
	DrvNand_flush_allblk();
}

void PlayPhoto_Frame(INT16S mode,INT16S board)
{
	int i,j,n,nn;
	INT16U *bmp,*Tv,*Sc;
	INT16U m,rgb;
	
	bmp=(INT16U*)VideoBuff;			//320x240  RGB data
	Sc=(INT16U*)ScalerBuff;			//320x240  RGB data
	Tv=(INT16U*)TVOutBuff;			//320x240  RGB data

	if(board){
		GetBmpData_cmp(mode,board);

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
const INT16S Photo_StikerSize[]={
	30,30,		32,32,		48,44,	34,30,	66,26,
	50,58,		94,46,		64,46,	66,20,	219,130,
	164,66,		70,72,		134,50
};
const INT16S VideoStikeSel[]={
	5,9,	108,9,	212,9,	5,120,	160,120,
};
const INT16S VideoStikeSmall[]={
	99,93,	53,75,	40,79,	119,72,	133,49,
	99,93,	75,54,	68,63,	86,87,	134,80
};
const INT16S VideoStikeSmallSp[]={
	10,2,3,4,5,
	10,6,7,8,9
};
void show_VideoStike(INT16S page,INT16S no)
{
	int i,m,n;
	
	if((player.GetGiftFlag&0x10)==0) set_sprite_display_init(10,0,0,M3_vstikeBg_Frame[0]);
	else set_sprite_display_init(10,0,0,M3_vstikeBg_Frame[page]);
	for(i=0;i<5;i++){
		m=page*10+2*i;
		n=2*i;
		if(page==0 && i==0) set_sprite_display_init(11+i,0,0,0);
		else if(page==1 && i==0 && (player.GetGiftFlag&0x10)==0) set_sprite_display_init(11+i,0,0,0);
		else if(i<3) set_sprite_display_init(11+i,VideoStikeSel[n]+51-VideoStikeSmall[m]/2,VideoStikeSel[n+1]+55-VideoStikeSmall[m+1]/2,M3_2stikerBar_Frame[VideoStikeSmallSp[page*5+i]]);
		else set_sprite_display_init(11+i,VideoStikeSel[n]+77-VideoStikeSmall[m]/2,VideoStikeSel[n+1]+55-VideoStikeSmall[m+1]/2,M3_2stikerBar_Frame[VideoStikeSmallSp[page*5+i]]);
	}
	if(no<3) set_sprite_display_init(16,VideoStikeSel[2*no],VideoStikeSel[2*no+1],M3_2stikerBar_Frame[0]);
	else set_sprite_display_init(16,VideoStikeSel[2*no],VideoStikeSel[2*no+1],M3_2stikerBar_Frame[1]);
	for(i=0;i<7;i++){
		set_sprite_nCharNum(10+i,1600);
	}
}
const INT16S StikeAniSize[]={
	99,93,	53,75,	54,143,	167,111,	133,49,	
	99,93,	100,70,	86,76,	141,145,	230,130,	
};
const INT16S StikeAniNum[]={
	7, 5, 5, 2,7,
	7,14, 2,14,2,
};
void Show_StikeAni()
{
	INT16S i,m,n,x,y;
	for(i=0;i<StikerMax;i++){
		if(stikerSp[i]){
			m=stikerSp[i];
			n=game_time/3%StikeAniNum[m];
			x=stikerX[i]-StikeAniSize[2*m]/2;
			y=stikerY[i]-StikeAniSize[2*m+1]/2;
			if(m==5) set_sprite_display_init(10+i,x,y,M3_2_stiker9_Frame[n]);
			else if(m==1) set_sprite_display_init(10+i,x,y,M3_2_stiker1_Frame[n]);
			else if(m==2) set_sprite_display_init(10+i,x,y,M3_2_stiker2_Frame[n]);
			else if(m==3) set_sprite_display_init(10+i,x,y,M3_2_stiker3_Frame[n]);
			else if(m==4) set_sprite_display_init(10+i,x,y,M3_2_stiker4_Frame[n]);
			else if(m==6) set_sprite_display_init(10+i,x,y,M3_2_stiker5_Frame[n]);
			else if(m==7) set_sprite_display_init(10+i,x,y,M3_2_stiker6_Frame[n]);
			else if(m==8) set_sprite_display_init(10+i,x,y,M3_2_stiker7_Frame[n]);
			else if(m==9) set_sprite_display_init(10+i,x,y,M3_2_stiker8_Frame[n]);
			set_sprite_nCharNum(10+i,1600);
		}else set_sprite_disable(10+i);
	}
}
void Show_StikePhoto()
{
	INT16S i,m,n,x,y;
	for(i=0;i<StikerMax;i++){
		if(stikerSp[i]){
			set_sprite_display_init(10+i,stikerX[i]-Photo_StikerSize[2*stikerSp[i]]/2,stikerY[i]-Photo_StikerSize[2*stikerSp[i]+1]/2,M3_stiker_Frame[stikerSp[i]]);
			set_sprite_nCharNum(10+i,1600);
		}else set_sprite_disable(10+i);
	}
}
void Setimg_board(INT16S Inc)
{
	if(Inc==1){
		if(img_board==8 && (player.GetGiftFlag&0x04)==0) img_board++;
		if(img_board==9 && (player.GetGiftFlag&0x08)==0) img_board=0;
	}else{
		if(img_board==9 && (player.GetGiftFlag&0x08)==0) img_board--;
		if(img_board==8 && (player.GetGiftFlag&0x04)==0) img_board--;
	}
}

void main_m03()
{
	INT16S i,m,page=0,pageno=0;
	INT16S play=0;
	VIDEO_CODEC_STATUS status;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;	
	int ret=0,codeflag=0;
	INT8S end=0;
    INT8S pause=0;
	INT16S vd_time,vd_cur;
	INT16S stiker,stikerNo,stikerPage;
	INT16S menu_page=0;
	INT16S Disk_TypeOld=0;

	user_defined_video_codec_entrance();
	video_decode_entrance();
	spMalloc(1600+850+350);
	
	INIT_VideoPub(700);
	LoadData_1(1,SpriteCellData,"Pororo\\video\\SpriteCell_cmp.BIN");
	Palette_ram_cpy( 1,0, M3_SPRITE_Palette1+256*1	,256);
	Palette_ram_cpy( 3,0, M3_SPRITE_Palette1+256*2	,256);
	LoadData_1(1,SpriteCellData+1600*_1K,"Pororo\\m3\\SpriteCell_cmp2.BIN");
	LoadDRM(0,4,SpriteCellData+2800*_1K,(int*)&DrmAddr[0],m3_MusicNames);

	Init_image_scale(320,240,
					320,240,
					SCALER_CTRL_IN_YUYV,SCALER_CTRL_OUT_RGB565,// SCALER_CTRL_OUT_RGB565
					SensorBuff1,ScalerBuff);

	clear_sprite(Spritebuf_number);
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	gplib_ppu_fade_effect_set(ppu_register_set, 0);

	game_time=0;
	if(OS_game==135) step=2;
	else step=0;
	while(1)
	{
		switch(step){
//--------------------------------------------------tile
			case 0:
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,0,0,m3_Open_Frame[0]);
				set_sprite_nCharNum(0,1600);
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[MENU_OPEN_SFX], 64, 127, SFX_CHANL);
				game_time=0;
				step++;
			break;
			case 1:
				#define Tile_Time	3
				if(game_time%Tile_Time==0 && game_time<6*Tile_Time){
					set_sprite_display_init(0,0,0,m3_Open_Frame[game_time/Tile_Time]);
				}else if(game_time>=Tile_Time*6+10){
					clear_sprite(Spritebuf_number);
					Palette_ram_cpy( 1,0, M3_SPRITE_Palette1+256*0	,256);
					LoadData_1(1,SpriteCellData+1600*_1K,"Pororo\\m3\\SpriteCell_cmp.BIN");
					LoadTextBmp(C_PPU_TEXT1,m3_Names[6+menu_page]);
					menu_page=0;
					step++;
				}
			break;
			case 2:
				clear_sprite(Spritebuf_number);
				LoadTextBmp(C_PPU_TEXT1,m3_Names[6+menu_page]);
				step++;
			break;
			case 3:
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					saveType=0;
					if(menu_page==0) step=10;
					else step=50;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=100;
				}else if(KeyBoardCode==K_Left || KeyBoardCode==K_Right){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					menu_page=(menu_page+1)%2;
					LoadTextBmp(C_PPU_TEXT1,m3_Names[6+menu_page]);
				} 
			break;
//---------------------------------------------------nomal photo
			case 10:
				clear_sprite(Spritebuf_number);
				m=saveType;
				LoadTextBmp(C_PPU_TEXT1,m3_Names[10+m]);
				step++;
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
					LoadTextBmp(C_PPU_TEXT1,m3_Names[10+m]);
				}
			break;
			case 12:
				//if(DISKUSED==FS_SD) saveType=1;
				if(saveType==1) Disk_Type=FS_SD;
				else Disk_Type=FS_NAND2;
				if((Disk_Type==2&&SD_Mount!=1)){
					step=18;
					break;
				}else{
					if(Disk_Type==FS_SD) Chdir_Root("mimicard_photo");
					else Chdir_Root("M2_photo");
					step++;
				}
			break;
			case 13:
				game_time=0;
				if(Disk_Type==FS_SD) get_photo_list(3,4);
				else get_photo_list(0,4);
				set_sprite_disable(200);
				pageno=page=0;
				if(File_Max==0) {
					step=19;
					break;
				}else step++;
				Disk_TypeOld=Disk_Type;
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
			break;

			case 14:
				Disk_Type=Disk_TypeOld;
				if(Disk_Type==FS_SD) {
						Chdir_Root("mimicard_photo");
					_findfirst("photo_*.jpg",&file_info,D_ALL);
				}else {
					Chdir_Root("M2_photo");
					_findfirst("M2_*.jpg",&file_info,D_ALL);
				}
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
					File_No=page*4+pageno;
					Get_file_info(File_No,pFileList,File_Max);
					if(Photo_decode_info((char *)file_info.f_name,320,240)==480){
						ret=Show_Warning(200,16+8);
						if(ret==-1) Game_State=0xff;
						break;
					}
					step=20;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=10;
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

			case 18:
			case 180:
				ret=Show_Warning(100,13);
				if(ret==0) {
					if(step==180) step=25;
					else step=10;
				}else step=100;
			break;
			case 19:
				ret=Show_Warning(100,9);
				if(ret==0) step=10;
				else if(ret==1)step=10;
				else step=100;
			break;

//-------------------------------------------------------
			case 20:
				clear_sprite(Spritebuf_number);
				Get_file_info(File_No,pFileList,File_Max);
				InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);
				if(Photo_decode_to_buff((char *)file_info.f_name,320,240,ScalerBuff)>=0){
				}
				if(SensorBuff1==0){
					SensorBuff1 = (INT32U) gp_malloc_align((SENSOR_WIDTH*SENSOR_HEIGHT)*2, 64);//malloc senseor buffer
				}
				set_sprite_display_init(1,8,104,photo_buttton_Frame[0]);
				set_sprite_display_init(2,284,104,photo_buttton_Frame[1]);
				//if(play) ClearSomeSp(0,9);
				img_board=0;
				stiker=0;
				stikerSp[0]=stikerSp[1]=stikerSp[2]=stikerSp[3]=stikerSp[4]=0;
				step++;
			break;
			case 21:
				PlayPhoto_Frame(0,img_board);
				step++;
			break;
			case 22:
				set_sprite_display_init(1,8,104,photo_buttton_Frame[0]);
				set_sprite_display_init(2,284,104,photo_buttton_Frame[1]);
				Show_StikePhoto();
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=14;
				}else if(KeyBoardCode==K_Left){	//<<
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(img_board) img_board--;
					else img_board=15;
					step--;
				}else if(KeyBoardCode==K_Right){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(img_board<15) img_board++;
					else img_board=0;
					step--;
				}else if(KeyBoardCode==K_Ok){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					stikerNo=0;
					set_sprite_disable(1);
					set_sprite_disable(2);
					step++;
				}
			break;
			case 23:
				Show_StikePhoto();
				if(stikerNo<5) set_sprite_display_init(50,0,200,M3_stikeBar_Frame[stikerNo]);
				else if(stikerNo<9) set_sprite_display_init(50,-160,200,M3_stikeBar_Frame[stikerNo]);
				else set_sprite_display_init(50,-320,200,M3_stikeBar_Frame[stikerNo]);
				set_sprite_nCharNum(50,1600);
				
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					set_sprite_disable(50);
					if(stiker) step++;
					else step--;
				}else if(KeyBoardCode==K_Left){	//<<
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerNo) stikerNo--;
					//else stikerNo=12;
				}else if(KeyBoardCode==K_Right){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerNo<12) stikerNo++;
					//else stikerNo=0;
				}else if(KeyBoardCode==K_Ok){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					set_sprite_disable(50);
					if(stikerNo){
						stikerSp[stiker]=stikerNo;
						stikerX[stiker]=160;
						stikerY[stiker]=120;
						stiker++;
						step++;
					}else step=25;
				}
			break;
			case 24:
				Show_StikePhoto();
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					stikerNo=0;
					if(stiker)stiker--;
					stikerSp[stiker]=0;
					step--;
				}else if(RepeatKeyCode==K_Left){	//<<
					if(BoardRepeat==0)SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerX[stiker-1]>0) stikerX[stiker-1]-=2;
				}else if(RepeatKeyCode==K_Right){	//>>
					if(BoardRepeat==0)SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerX[stiker-1]<320) stikerX[stiker-1]+=2;
				}else if(RepeatKeyCode==K_Up){	//<<
					if(BoardRepeat==0)SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerY[stiker-1]>0) stikerY[stiker-1]-=2;
				}else if(RepeatKeyCode==K_Down){	//>>
					if(BoardRepeat==0)SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerY[stiker-1]<240) stikerY[stiker-1]+=2;
				}else if(KeyBoardCode==K_Ok){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					m=0;
					step++;
				}
			break;
			case 25:
				Show_StikePhoto();
				m=0;
				step=250;
			break;
			case 250:
				set_sprite_display_init(50,0,0,photo_ui_Frame[1+m]);
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					set_sprite_disable(50);
					if(stiker) step=24;
					else step=22;
				}else if(KeyBoardCode==K_Left || KeyBoardCode==K_Right){	//<<
					if(BoardRepeat==0)SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					m=(m+1)%2;
				}else if(KeyBoardCode==K_Ok){	//>>
					set_sprite_disable(50);
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					if(m==0 && stiker<5){
						stikerNo=0;
						step=23;
					}else if(m){
						step=26;
					}else step=24;
				}
			break;
			case 26:
				Copy32(VideoBuff,TVOutBuff,FRAME_SIZE/2);
				clear_sprite(Spritebuf_number);
				LoadTextBmp(C_PPU_TEXT1,m3_Names[8]);
				step++;
				m=0;
			break;
			case 27:
				if(KeyBoardCode==K_Ok){
					Copy32(TVOutBuff,VideoBuff,FRAME_SIZE/2);
					if(m==1){
						if(SD_Mount!=1) {
							step=180;
							break;
						}else{
							Disk_Type=FS_SD;
							Chdir_Root("mimicard_photo");
						}
					}else {
						Disk_Type=FS_NAND2;
						Chdir_Root("M2_photo");
					}
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					step++;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					set_sprite_disable(50);
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					Copy32(TVOutBuff,VideoBuff,FRAME_SIZE/2);
					step=25;
				}else if(KeyBoardCode==K_Up || KeyBoardCode==K_Down){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					m=(m+1)%2;
					LoadTextBmp(C_PPU_TEXT1,m3_Names[8+m]);
				} 
			break;
			case 28:
				Show_StikePhoto();
				PaintTFT_cell4();
				PaintTFT_cell4();
				Copy32(VideoBuff,TV_buffer[0],FRAME_SIZE/2);
				if(Save_makeup_Buff()) {
					step=25;
				}else {
					step++;
					m=0;
				}
			break;
			case 29:
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
//---------------------------------------------------Video player
			case 50:
				VideoThum=0;
				sensorFree();
				saveType=0;
				Disk_Type=FS_NAND2;
				step=53;
			break;
			case 53:
				if(saveType==0) get_photo_list(1,4);
				else get_photo_list(4,4);
				page=pageno=0;
				if(File_Max==0) step=58;
				else step++;
			break;
			case 54:
				_findfirst("M1-1_*.avi",&file_info,D_ALL);
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
					File_No=page*4+pageno;
					step=60;
					img_board=0;
					stiker=0;
					stikerSp[0]=stikerSp[1]=stikerSp[2]=stikerSp[3]=stikerSp[4]=0;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=2;
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
				if(ret==0) step=2;
				else if(ret==1)step=2;
				else step=100;
			break;
//---------------------------------------------
			case 60:
				clear_sprite(Spritebuf_number);
				D_pic=Get_file_info(File_No,pFileList,File_Max);
				InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);//BMP_RGB565
				VideoThum=1;
				ret=Get_video_Info(0);
				VideoThum=0;
				//Copy32(ScalerBuff,TVOutBuff,FRAME_SIZE/2);
				//set_sprite_display_init(0,120,152,photo_buttton_Frame[2]);
				set_sprite_display_init(1,8,104,photo_buttton_Frame[0]);
				set_sprite_display_init(2,284,104,photo_buttton_Frame[1]);
				//codeflag=1;
				end=pause=1;
				stikerSp[0]=stikerSp[1]=stikerSp[2]=stikerSp[3]=stikerSp[4]=0;
				step++;
			break;
			case 61:
				PlayPhoto_Frame(1,img_board);
				step++;
			break;
			case 62:
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(1,8,104,photo_buttton_Frame[0]);
				set_sprite_display_init(2,284,104,photo_buttton_Frame[1]);
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					step=54;
				}else if(KeyBoardCode==K_Left){	//<<
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(img_board) img_board--;
					else img_board=9;
					Setimg_board(0);
					step--;
				}else if(KeyBoardCode==K_Right){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(img_board<9) img_board++;
					else img_board=0;
					Setimg_board(1);
					step--;
				}else if(KeyBoardCode==K_Ok){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					step++;
				}
			break;
			case 63:
				clear_sprite(Spritebuf_number);
				stikerPage=stikerNo=0;
				step++;
			break;
			case 64:
				show_VideoStike(stikerPage,stikerNo);
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					clear_sprite(Spritebuf_number);					
					if(stikerPage==0 && stikerNo==0)  {
						m=0;
						step=66;
					}else if(stikerPage==1 && stikerNo==0 && (player.GetGiftFlag&0x10)==0 )  {
						m=0;
						step=66;
					}else{
						stikerSp[stiker]=stikerPage*5+stikerNo;
						stikerX[stiker]=160;
						stikerY[stiker]=120;
						stiker++;
						step++;
					}
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					clear_sprite(Spritebuf_number);
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stiker) step++;
					else step=62;
				}else if(KeyBoardCode==K_Up){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerNo>2) stikerNo-=2;
				}else if(KeyBoardCode==K_Down){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerNo==0) stikerNo=3;
					else if(stikerNo<=2) stikerNo+=2;
				}else if(KeyBoardCode==K_Left){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerNo>0) stikerNo--;
					else{
						stikerPage=(stikerPage+1)%2;
						stikerNo=4;
					}
				}else if(KeyBoardCode==K_Right){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerNo<4) stikerNo++;
					else{
						stikerPage=(stikerPage+1)%2;
						stikerNo=0;
					}
				}
			break;
			case 65:
				Show_StikeAni();
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stiker)stiker--;
					stikerSp[stiker]=0;
					step=63;
				}else if(RepeatKeyCode==K_Left){	//<<
					if(BoardRepeat==0)SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerX[stiker-1]>0) stikerX[stiker-1]-=2;
				}else if(RepeatKeyCode==K_Right){	//>>
					if(BoardRepeat==0)SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerX[stiker-1]<320) stikerX[stiker-1]+=2;
				}else if(RepeatKeyCode==K_Up){	//<<
					if(BoardRepeat==0)SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerY[stiker-1]>0) stikerY[stiker-1]-=2;
				}else if(RepeatKeyCode==K_Down){	//>>
					if(BoardRepeat==0)SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stikerY[stiker-1]<240) stikerY[stiker-1]+=2;
				}else if(KeyBoardCode==K_Ok){	//>>
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					m=0;
					step++;
				}
			break;
			case 66:
				Show_StikeAni();
				step=660;
				m=0;
			break;
			case 660:
				set_sprite_display_init(50,0,0,photo_ui_Frame[1+m]);
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					set_sprite_disable(50);
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(stiker)step=65;
					else step=62;
				}else if(KeyBoardCode==K_Left || KeyBoardCode==K_Right){	//<<
					if(BoardRepeat==0)SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					m=(m+1)%2;
				}else if(KeyBoardCode==K_Ok){	//>>
					set_sprite_disable(50);
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					if(m==0 && stiker<5){
						step=63;
					}else if(m){
						Copy32(VideoBuff,TVOutBuff,FRAME_SIZE/2);
						clear_sprite(Spritebuf_number);
						LoadTextBmp(C_PPU_TEXT1,m3_Names[12]);
						m=0;
						step=67;
					}else step=65;
				}
			break;

			case 67:
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					SPU_StopChannel(MUSIC_CHCHANL);
					Copy32(TVOutBuff,VideoBuff,FRAME_SIZE/2);
					m=0;
					step=66;
				}else if(KeyBoardCode==K_Up){	//<<
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(m) {
						m--;
						if(m) SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[m-1], 64, 127, MUSIC_CHCHANL);
						else SPU_StopChannel(MUSIC_CHCHANL);
						LoadTextBmp(C_PPU_TEXT1,m3_Names[12+m]);
					}
				}else if(KeyBoardCode==K_Down){	//<<
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(m<4) {
						m++;
						if(m) SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[m-1], 64, 127, MUSIC_CHCHANL);
						else SPU_StopChannel(MUSIC_CHCHANL);
						LoadTextBmp(C_PPU_TEXT1,m3_Names[12+m]);
					}
				}else if(KeyBoardCode==K_Ok){	//>>
					Show_StikeAni();
					Copy32(TVOutBuff,VideoBuff,FRAME_SIZE/2);
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					SPU_StopChannel(MUSIC_CHCHANL);
					deroca_music=m;
					Save_DecoraData();
					m=0;
					step++;
				}				
			break;
			case 68:
				//OS_game=148;
				//step=100;
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

			audio_decode_volume(63);
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
		if(codeflag) codeflag=0;
		if( (step>=20 && step<30) || step==250 ){
			if(TestMenu_Key()){
				ret=Show_Warning(100,16+4);
				if(ret==0){
					OS_game=13;
				}else step=100;
			}
		}else if( (step>=60 && step<70)  || step==660 ){
			if(TestMenu_Key()){
				SPU_StopChannel(MUSIC_CHCHANL);
				ret=Show_Warning(100,16+6);
				if(ret==0){
					OS_game=13;
				}else step=100;
			}
		}else if(Goto_desktop()){
			step=100;
		}
		if(step==100) break;
		if(pause==0) PaintTFT_go(); 		//video_decode_FrameReady
		else PaintTFT_cell4();
	}
	SPU_StopChannel(MUSIC_CHCHANL);
	sensorFree();
	video_decode_stop();
	video_decode_exit();
	InitFileSystem(DISKUSED);
}
