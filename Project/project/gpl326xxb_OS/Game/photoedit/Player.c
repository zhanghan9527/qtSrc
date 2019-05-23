#include "application.h"
#include "data.h"
#include "math.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "photo_buttton.h"
#include "photoedit.h"


extern const INT16U video_SPRITE_Palette1[];
extern INT32U scaler_ptr,decode_mode_ck,BLOCK_ENCODE;
extern VIDEO_INFO video_information;
INT16U Voice_Max;
INT16U VideoThum=0;
extern INT16S img_board,Video_Yuv,Video_First;

//文件名必须满足的格式  10_%05d.bmp ,其他不查找
//==================================================================
INT16U Get_NameList_Soft(char *pPath,char *pExp,INT32U listAddr)
{
	INT16S ret,num;
	struct f_info fin;
	INT16S i,j;
	int no,m;
	INT16U* list=(INT16U*)listAddr;

	num=0;
	if(pExp==NULL) return 0;
	Chdir_Root(pPath);
	ret = _findfirst(pExp,&fin,D_ALL);
	if(ret<0) return 0;
	while(1){
		//no=( (fin.f_name[3]-0x30)*10000+fin.f_name[4]-0x30)*1000+(fin.f_name[5]-0x30)*100+(fin.f_name[6]-0x30)*10+(fin.f_name[7]-0x30);
		i=0;
		while(1){
			if(fin.f_name[i]=='.') break;
			else i++;
		}
		no=fin.f_name[i-1]-0x30;
		i-=2;m=10;
		while(1){
			if(fin.f_name[i]>='0' && fin.f_name[i]<='9'){
				no+=m*(fin.f_name[i]-0x30);
				m*=10;
				i--;
			}else break;
		}
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
		*(list+2*i)=fin.entry;
		*(list+2*i+1)=no;
		num++;
		while(1){
			ret =_findnext(&fin);
			if(ret<0 || num>=4000) break;
			if(fin.f_size==0){
				unlink((char *)fin.f_name);
			}else break;
		}
		if(ret<0 || num>=4000) break;
		if(Disk_Type==FS_SD){
			show_Loading();
		}
	}
	return num;
}

INT16U Get_NameList_Sd(char *pPath,char *pExp)
{
	INT16S ret,num=0;
	struct f_info fin;
	INT16S i,j;
	int no=0,m,Max=1;

	if(pExp==NULL) return 0;
	Chdir_Root(pPath);
	ret = _findfirst(pExp,&fin,D_ALL);
	if(ret<0) return 0;

	while(1){
		i=0;
		while(1){
			if(fin.f_name[i]=='.') break;
			else i++;
		}
		no=fin.f_name[i-1]-0x30;
		i-=2;m=10;
		while(1){
			if(fin.f_name[i]>='0' && fin.f_name[i]<='9'){
				no+=m*(fin.f_name[i]-0x30);
				m*=10;
				i--;
			}else break;
		}
		if(no>=Max) Max=no+1;

		num++;
		ret =_findnext(&fin);
		if(ret<0) break;
	}
	return Max;
}


void get_photo_list(INT16S list,INT16S page_num)
{
	if(list==0){
		File_Max=Get_NameList_Soft("M2_photo","M2_*.jpg",pFileList);
		//File_SaveMax=File_Max;
	}else if(list==1){
		File_Max=Get_NameList_Soft("M1-1_video","M1-1_*.avi",pFileList);
		//VID_Nums=File_Max;
	}else if(list==2){
		File_Max=Get_NameList_Soft("voice","10_?????.wav",pFileList);
		//Voice_Max=File_Max;
	}else if(list==3){
		File_Max=Get_NameList_Soft("mimicard_photo","photo_*.jpg",pFileList);
		//File_SaveMax=File_Max;
	}else if(list==4){
		File_Max=Get_NameList_Soft("M1-2_video","M1-2*.avi",pFileList);
		//VID_Nums=File_Max;
	}else if(list==5){
		File_Max=Get_NameList_Soft("mimicard_video","video_*.avi",pFileList);

//-----------------------------------------------------
	}else if(list==10){
		PicSd_Nums=Get_NameList_Sd("mimicard_photo","photo_*.jpg");
	}else if(list==11){
		VidSd_Nums=Get_NameList_Sd("mimicard_video","video_*.avi");
	}
	Act_SaveNums();
	if(File_Max%page_num==0) File_Page=File_Max/page_num;
	else File_Page=1+File_Max/page_num;
	File_No=0;
}
void GetSD_Nums()
{
	INT16S m;
	
	if(SD_Mount==1){
		m=Disk_Type;
		Disk_Type=FS_SD;
		get_photo_list(10,6);
		get_photo_list(11,6);
		InitFileSystem(m);
	}
}

INT16S image_decode_to_thum(char *path,INT16S Width,INT16S Height,INT32U output)
{
	IMAGE_ARGUMENT image_decode;
	MEDIA_SOURCE image_source;
	INT16S ret,decode_state;	
	IMAGE_INFO image_file_info;
	int j=0;

   //image decoder to used image callback function
    BLOCK_ENCODE=0;
    decode_mode_ck=0;
    
	//image source infomation
	image_source.type=SOURCE_TYPE_FS;                                    //image file infomation form file system
	
	
	image_source.type_ID.FileHandle=(INT32U)open(path, O_RDONLY);       //open jpg file
	if(image_source.type_ID.FileHandle==-1){
		return -1;
	}
	image_decode_Info(&image_file_info,image_source);
	if(image_file_info.Height==0 || image_file_info.Width==0) return -1;
	if(image_file_info.Width*Height*2==image_file_info.Height*Width){
		j=Height*2;
	}else{
		j=Height;
	}

	//image output infomation
	image_decode.ScalerOutputRatio=FIT_OUTPUT_SIZE;//FIT_OUTPUT_SIZE;                  //Fit to output_buffer_width and output_buffer_height for image output size
	image_decode.OutputFormat=IMAGE_OUTPUT_FORMAT_RGB565;//IMAGE_OUTPUT_FORMAT_YUYV; //                 //scaler out format
	image_decode.OutputBufPtr=(INT8U *)(output);            //decode output buffer
	image_decode.OutputBufWidth=Width;                  //width of output buffer 
	image_decode.OutputBufHeight=j;                 //Heigh of output buffer
	image_decode.OutputWidth=Width;                     //scaler width of output image
	image_decode.OutputHeight=j;                    //scaler Heigh of output image

	//image decode function
	image_decode_entrance();//global variable initial for image decode
	image_decode_start(image_decode,image_source);//image decode start	
	
 	while(1)
 	{
		decode_state=image_decode_status();
		if (decode_state==IMAGE_CODEC_DECODE_END){
			 ret=0;
			 break;
		}else{	// if(decode_state==IMAGE_CODEC_DECODE_FAIL){
			 DBG_PRINT("image decode failed\r\n");			 
			 ret=-1;
			 break;
		}
	}
	close(image_source.type_ID.FileHandle);
	image_decode_stop();//image decode stop			 	
	 		
	return ret;
}
#define Thum_Xsize	148
#define Thum_Ysize	103
void show_photo_thum(INT16S x0,INT16S y0)
{
    INT16S i,j;
    INT32U m,mm;
    INT16S *pTv,*pThum;

	pTv=(INT16S*)TVOutBuff;
	pThum=(INT16S*)VideoBuff;
	for(j=0;j<Thum_Ysize;j++){
		mm=x0+(y0+j)*320;
		for(i=0;i<Thum_Xsize;i++){
			if(i<3 && (j<3-i || j>=Thum_Ysize+i-3)) ;//*(pTv+mm+i)=0xffff;
			else if(i>=Thum_Xsize-3 && (j<3+i-Thum_Xsize || j>=Thum_Ysize-i-3+Thum_Xsize)) ;//*(pTv+mm+i)=0xffff;
			else *(pTv+mm+i)=*(pThum+(5+j)*THUM_W+i+2);
		}
	}
}
void showPhotoThum(INT16S page)
{
	INT16S k,m;
	INT16S x,y;
//*	
	m=page*4;
 	for(k=0;k<4;k++){
 		if(m+k>=File_Max) break;
 		Get_file_info(m+k,pFileList,File_Max);
 		if(image_decode_to_thum((char *)file_info.f_name,THUM_W,THUM_H,VideoBuff)>=0){
 			x=8+k%(PAGE_NUM/2)*156;
 			y=13+k/(PAGE_NUM/2)*111;
 			show_photo_thum(x,y);
 		}
 	}//*/
}

//=====================================================================
INT16S Get_video_Info(INT16S flag)
{
	INT16S ret=-1;
	VIDEO_CODEC_STATUS status;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;	

	arg.bScaler = 0x01;	//scaler output size or not
	arg.bUseDefBuf = FALSE;	//use user define buffer 
	//arg.AviDecodeBuf1 = avi_dec_buffer[0];
	//arg.AviDecodeBuf2 = avi_dec_buffer[1];

	arg.DisplayWidth = 320;		//display width
	arg.DisplayHeight = 240;	//display height
	arg.DisplayBufferWidth = 320;	//display buffer width
	arg.DisplayBufferHeight = 240;	//display buffer height

	arg.OutputFormat = IMAGE_OUTPUT_FORMAT_RGB565;	//set output format
	//Get_file_info_UC(no);
	src.type_ID.FileHandle = open((char*)file_info.f_name,O_RDONLY); //open file handle
	if(src.type_ID.FileHandle < 0)
	{
		close(src.type_ID.FileHandle);
		return -1;
	}	
	src.type = SOURCE_TYPE_FS;	//play file by file system
	//if( (file_type>>4)==2){
	//	src.Format.VideoFormat =MPEG4;
	//}else 
	src.Format.VideoFormat =MJPEG;//MJPEG;// MPEG4;

	if(flag){
		status = video_decode_paser_header(&video_information, arg, src);
		if(status != VIDEO_CODEC_STATUS_OK){
			close(src.type_ID.FileHandle);
			return -1;
		}
		status = video_decode_start(arg, src);
		if(status != START_OK) return -1;
		ret=0;
	}else{
		status = video_decode_get_nth_video_frame(&video_information,arg, src,50);
		if(status != START_OK) ret= -1;
		close(src.type_ID.FileHandle);
		ret=0;
	}
	return ret;

//			DBG_PRINT("Aud SampleRate = %d\r\n", information.AudSampleRate);
//			DBG_PRINT("Vid FrameRate = %d\r\n", information.VidFrameRate);
//			DBG_PRINT("resolution = %d x %d\r\n", information.Width, information.Height);
//			DBG_PRINT("Total Time = %d seconds\r\n", information.TotalDuration);
}

void show_video_thum(INT16S x0,INT16S y0)
{
    INT16S i,j;
    INT32U m,mm;
    INT16S *pTv,*pThum;

	pTv=(INT16S*)TVOutBuff;
	pThum=(INT16S*)ScalerBuff;
	for(j=0;j<Thum_Ysize;j++){
		mm=x0+(y0+j)*320;
		m=(j*240/Thum_Ysize)*320;
		for(i=0;i<Thum_Xsize;i++){
			//if(i<3 || j<3 || i>=THUM_W-3 || j>=THUM_H-3) *(pTv+mm+x0+i)=0xffff;
			//else
			*(pTv+mm+i)=*(pThum+m+(i*320/Thum_Xsize));
		}
	}
}
extern INT16S img_board;
const INT16S Theme_Frame[]={
	3,5,6,7,4
};
void showThum_Frame()
{
	int i,j,n,nn;
	INT16U *bmp,*Tv;
	INT16U mode,board,rgb;
	
	bmp=(INT16U*)VideoBuff;			//320x240  RGB data
	Tv=(INT16U*)ScalerBuff;			//320x240  RGB data

	if(img_board<10){
		mode=1;
		board=img_board;
	}else{
		mode=img_board;
		board=Theme_Frame[img_board-11];
	}
	GetBmpData_cmp(mode,board);

	for(j=0;j<240;j++){
		n=j*320;
		for(i=0;i<320;i++){
			rgb=*(bmp+n+i);
			if(rgb==0x1f) ;
			else *(Tv+n+i)=rgb;
		}
	}
}
void showVideoThum(INT16S page)
{
	INT16S k,m;
	INT16S x,y;
	INT16U decora;
//*	
	VideoThum=1;
	m=page*4;
 	for(k=0;k<4;k++){
 		if(m+k>=File_Max) break;
 		img_board=0;
 		if(OS_game==14 && saveType){
 			decora=Get_decora_info(m+k,pFileList,File_Max);
			if(decora&0x8000){
				Load_DecoraData(1,decora&0x7fff);
			}else{
				if(file_info.f_name[4]>='a') img_board=11+(file_info.f_name[4]-'a');
			}
 		}else if(Disk_Type==2){
 			Get_file_info(m+k,pFileList,File_Max);
 			if(file_info.f_name[6]>='a') img_board=11+(file_info.f_name[6]-'a');
 		}else Get_file_info(m+k,pFileList,File_Max);
 		
 		Get_video_Info(0);
		x=8+k%(PAGE_NUM/2)*156;
		y=13+k/(PAGE_NUM/2)*111;
		if(img_board) showThum_Frame();
		show_video_thum(x,y);
		set_sprite_display_init(10+k,x+54,y+32,photo_buttton_Frame[4]);
 	}//*/
 	VideoThum=0;
 	ClearSomeSp(10+k,10+3);
}

//==================================================
void main_VideoPlay()
{
	INT16S i,m,page=0;
	INT16S play=0;
	VIDEO_CODEC_STATUS status;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;	
	int ret=0,codeflag=0;
	INT8S end=0;
    INT8S pause=0;
	INT16S vd_time,vd_cur;
	INT16S filter=0;
	INT16S aditor,aditorX,aditorY;
	INT16S dx,dy;

	user_defined_video_codec_entrance();
	video_decode_entrance();
	spMalloc(1000);
	Palette_ram_cpy( 0,0, video_SPRITE_Palette1+256*0	,256);
	LoadData_1(1,SpriteCellData,"Pororo\\video\\SpriteCell_cmp.BIN");
	Init_image_scale(320,240,
					320,240,
					SCALER_CTRL_IN_YUYV,SCALER_CTRL_OUT_RGB565,// SCALER_CTRL_OUT_RGB565
					SensorBuff1,ScalerBuff);

	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	step=0;
	while(1)
	{
		switch(step){
//--------------------------------------------------list
			case 0:
				if(DISKUSED==FS_SD) Disk_Type=FS_SD;
				else Disk_Type=FS_NAND2;
				Chdir_Root("M1-1_video");
				get_photo_list(1,4);
				page=0;
				if(File_Max==0) step=1;
				else step=60;
			break;
			case 1:
				return;
			break;
			
			case 60:
				clear_sprite(Spritebuf_number);
				Get_file_info(File_No,pFileList,File_Max);
				InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,3);//BMP_RGB565
				set_sprite_display_init(0,120,152,photo_buttton_Frame[2]);
				ret=Get_video_Info(0);
				//codeflag=1;

				step++;
			break;
			case 61:
				if(pause==0){
					SetWaveIo(1);
					set_sprite_display_init(0,120,152,photo_buttton_Frame[5]);
					if(KeyBoardCode==K_Ok){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);												
						codeflag=2;
						pause=1;
					}
					break;
				}else{
					set_sprite_display_init(0,120,152,photo_buttton_Frame[2]);
					if(KeyBoardCode==K_Ok){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);												
						codeflag=1;
						pause=0;
						end=0;
						game_time=0;
					}
				}
			break;
			case 65:		//del
				unlink((char *)file_info.f_name);
				File_Max=Del_One_file(File_No,pFileList,File_Max);
				File_SaveMax=File_Max;
				Act_SaveNums();
				if(File_Max%6==0) File_Page=File_Max/6;
				else File_Page=1+File_Max/6;
				if(File_Max==0){
					step=51;
				}else{
					File_No=File_No%File_Max;
					step=60;
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
			}else audio_decode_volume(63);
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
		if(KeyBoardCode==K_Home){
			step=100;
		}
		if(step==100) break;
	}
	video_decode_stop();
	video_decode_exit();
}

