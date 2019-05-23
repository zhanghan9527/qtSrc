#include "application.h"
#include "data.h"
#include "gplib.h"
#include "math.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "photoedit.h"
#include "photo_buttton.h"

extern const INT16U video_SPRITE_Palette1[];
INT16S img_board=0;
INT32U Encode_frame=0;
extern IMAGE_SCALE_STRUCT Sensor_scale;

#define VideoFormatSelectCmdBufferSize	5
#define VideoFormatSelectTaskStackSize	1024

OS_EVENT *VideoFormatSelectCmd_q;        //Message queen index
void *VideoFormatSelectCmdBuffer[VideoFormatSelectCmdBufferSize];	    //Point message array index
INT32U VideoFormatSelectTaskStack[VideoFormatSelectTaskStackSize];

typedef struct video_info_s{
	INT8U  mode;   
	INT32U type;   
	INT8U* path;   
	INT8U* avi_dec_buffer[2];   
	INT32S index;   
	INT32U total;   
}video_info_t;
//video_info_t *video_info;
typedef enum VIDEO_DECODE_TYPE{
	DECODE_MJPEG = 0X01,   
	DECODE_MPEG4,   
	ENCODE_JPEG,
	ENCODE_MJPEG	   
}VIDEO_FORMAT_ARR;

typedef enum CONTROL_SELECT{
	SPEED_CONTROL = 0x00,
	VOLUME_CONTROL,
	FF_FB_CONTROL
}SELECT_CONTROL;


#define VIDEO_ENCODE_MODE	0x00  
#define VIDEO_DECODE_MODE	0x02
#define BOARD_MAX	20

extern VIDEO_INFO video_information;
//=================================================================
//*
void Act_VideoDemo()
{	
	while(1){
		//buff_Malloc(1,600,0);
		AddRootPath("15002\\gameM09\\M09_02V01.AVI");
		//AddRootPath("video\\Clip_M09_4.AVI");
		VideoDecode_byname(rootPath);
	}
}//*/

//  save H_flip
void Video_view(INT32U frame,INT16S BMP)
{
	int i,j,n,nn,k=0;
	INT16S* sen;
	INT16S* bmp;
	INT16S* tv;
	INT32S* sen2;
	INT32S* bmp2;
	INT32S* tv2;

	if(BMP==1){
		sen=(INT16S*)frame;			//320x240 RGB data
		bmp=(INT16S*)VideoBuff;		//320x240 RGB data
		tv=(INT16S*)TVOutBuff;		//320x240 TV buff

		//add frame 
		if(img_board){		
			for(i=0;i<240;i++){
				n=i*320;
				for(j=0;j<320;j++){
					if(*(bmp+n+j)!=0x001F) *(sen+n+j)=*(bmp+n+j);
				}
			}

			Sensor_scale.input_buf1=(void*)(frame);
			Sensor_scale.output_buf1=(void*)(Encode_frame);
			image_scale(&Sensor_scale);
		}
		//*
		for(i=0;i<240;i++){
			n=i*320;
			for(j=0;j<320;j++){
				*(tv+n+j)=*(sen+n+j);
			}
		}//*/
	}else if(BMP==2){
		sen=(INT16S*)frame;			//320x240  YUV data
		tv=(INT16S*)TVOutBuff;		//320x240 YUV buff

		if(VideoThum){
			img_scale.input_buf1=(void*)(frame);
			img_scale.output_buf1=(void*)(ScalerBuff);//320x240 RGB buff
			image_scale(&img_scale);
		}else{
			for(i=0;i<240;i++){
				n=i*320;
				for(j=0;j<320;j++){
					*(tv+n+j)=*(sen+n+j);
				}
			}//*/
		}
	}else{
		sen2=(INT32S*)frame;		//320x240 YUV data
		bmp2=(INT32S*)VideoBuff;		//320x240 YUV data
		tv2=(INT32S*)TVOutBuff;		//240x320 TV buff

		//sensor_turn();
		if(img_board&0x1000) return;
		
		if(img_board && img_board<100){		//add frame
			for(i=0;i<240;i++){
				n=i*160;
				nn=i*160;
				for(j=0;j<160;j++){
					if(*(bmp2+nn+j)!=0) *(sen2+n+j)=*(bmp2+n+j);
				}
			}
		}
		
		for(i=0;i<160;i++){
		for(j=0;j<120;j++){
			*(tv2+(2*i)*120+j)=*(sen2+(2*j)*160+i);
			*(tv2+(2*i+1)*120+j)=*(sen2+(2*j+1)*160+i);
		}}
	}
}
void video_type_select(INT32U type)
{
	OSQPost(VideoFormatSelectCmd_q,(void *)type);   
	OSTimeDly(2);   
}

INT16S Video_Coding()
{
	if(video_encode_status() == VIDEO_CODEC_PROCESS_PAUSE)
	{         
		video_encode_resume();            
	}
	if(video_encode_status() == VIDEO_CODEC_PROCESSING) return 1;
	else return 0;
}

void Init_Video()
{

	openSensor();
    //ScalerSW_Init();
	user_defined_video_codec_entrance();	
	video_encode_entrance();
}
void Start_Video()
{
	VIDEO_ARGUMENT arg;
	
	arg.bScaler = 1;		// must be 1      
	arg.TargetWidth = 320; 	//encode width   
	arg.TargetHeight = 240;	//encode height   
	arg.SensorWidth	= 320;	//sensor input width    
	arg.SensorHeight = 240;	//sensor input height   
	arg.DisplayWidth = 320;	//display width      
	arg.DisplayHeight = 240;	//display height     
	arg.DisplayBufferWidth = 320;	//display buffer width    
	arg.DisplayBufferHeight = 240;//display buffer height   
	arg.VidFrameRate = 15;	//video encode frame rate    
	arg.AudSampleRate = 8000;//audio record sample rate   
	//arg.OutputFormat = IMAGE_OUTPUT_FORMAT_YUYV; //display output format    
	//InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_YUYV);//BMP_RGB565
	
	arg.OutputFormat = IMAGE_OUTPUT_FORMAT_RGB565; //display output format    
	InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);//BMP_RGB565
	
	fill_mem(TVOutBuff,FRAME_SIZE*2,0);//0x1f
	Init_Sensor_scale(320,240,
					320,240,
					320,240,
					SCALER_CTRL_IN_RGB565,SCALER_CTRL_OUT_YUYV,// SCALER_CTRL_OUT_RGB565
					ScalerBuff);	//SCALER_CTRL_IN_YUYV

	video_encode_preview_start(arg);
#ifdef __GC0329_DRV_C_
	//sccb_write_Reg8Data8(0x62, 0x17, 0x15);	//0 mirrow,	1, UPdown
#endif	

	Wait_Sensor(10);
	//MemAddr=(INT32U) gp_malloc_align((FRAME_SIZE*2), 64);
}
void GetBmpData_YUV(INT16S img_board)
{
	char path[50];
	INT8U	*yuv;
	INT32S i,j,n,nn,fd,nRet;
	INT32S 	r,g,b,st,y;
	INT8S	tmp;
	INT8U col[256*3];	//256 RGB565
	INT8U temp[2000];	

	yuv=(INT8U*)(VideoBuff);		//320x240 YUV data

	sprintf(path,"JJCell\\m3\\frame\\photoframe%02d.bmp",img_board);
	AddRootPath(path);
	fd = open(rootPath, O_RDONLY);
	if(fd!=-1){
		lseek(fd,0x0a,SEEK_SET);
		nRet = read(fd,(INT32U)&temp,2);
		st=*(temp)+*(temp+1)*0x100;
	
		lseek(fd,0x36,SEEK_SET);
		nRet = read(fd,(INT32U)&temp,1024);
		for(i=0;i<256;i++){
			b=*(temp+i*4);
			g=*(temp+i*4+1);
			r=*(temp+i*4+2);
			y=(299*r+587*g+114*b)/1000;
			*(col+3*i)=y;
			tmp=128+(500*r-420*g-80*b)/1000;
			*(col+3*i+1)=tmp;
			tmp=128+(500*b-340*g-160*r)/1000;
			*(col+3*i+2)=tmp;
		}
//-----------------------------------------------320x240 YUV
		for(j=0;j<240;j++){
			n=j*320*2;
			nn=(239-j)*320;
			lseek(fd,st+nn,SEEK_SET);
			nRet = read(fd,(INT32U)&temp,320);
			for(i=0;i<160;i++){
				if(temp[2*i]!=0xff || temp[2*i+1]!=0xff){
					if(temp[2*i]==0xff){
						*(yuv+n+4*i)=*(col+3*temp[2*i+1]+1);
						*(yuv+n+4*i+1)=*(col+3*temp[2*i+1]+0);
						*(yuv+n+4*i+2)=*(col+3*temp[2*i+1]+2);
						*(yuv+n+4*i+3)=*(col+3*temp[2*i+1]+0);
					}else if(temp[2*i+1]==0xff){
						*(yuv+n+4*i)=*(col+3*temp[2*i]+1);
						*(yuv+n+4*i+1)=*(col+3*temp[2*i]+0);
						*(yuv+n+4*i+2)=*(col+3*temp[2*i]+2);
						*(yuv+n+4*i+3)=*(col+3*temp[2*i]+0);
					}else{
						*(yuv+n+4*i)=*(col+3*temp[2*i]+1);
						*(yuv+n+4*i+1)=*(col+3*temp[2*i]+0);
						*(yuv+n+4*i+2)=*(col+3*temp[2*i+1]+2);
						*(yuv+n+4*i+3)=*(col+3*temp[2*i+1]+0);
					}
				}else{
					*(yuv+n+4*i)=0;
					*(yuv+n+4*i+1)=0;
					*(yuv+n+4*i+2)=0;
					*(yuv+n+4*i+3)=0;
				}
			}
		}
		close(fd);
	}
}

void GetBmpData_m3(INT16S img_board)
{	
	char path[100];
	INT16S Col_buf[256];	//256 RGB565
	INT8U BMP_data[1024];	//a line data
	
	INT16U* bmp;
	INT32S i,j,n,nn,nRet,fd,st;
	INT16U r,g,b;
	
	bmp=(INT16U*)VideoBuff;			//320x240  RGB data
	
	disk_safe_exit(Disk_Type);
	Paintdelay();
	if(img_board<100){
		sprintf(path,"JJCell\\m3\\frame\\photoframe%02d.bmp",img_board);
	}else{
		sprintf(path,"JJCell\\m3\\frame\\sticekr%02d.bmp",img_board-100);
	}

	AddRootPath(path);
	fd = open(rootPath, O_RDONLY);
	if(fd!=-1){
		lseek(fd,0x0a,SEEK_SET);
		nRet = read(fd,(INT32U)&BMP_data,2);
		st=BMP_data[0]+BMP_data[1]*0x100;
	
		lseek(fd,0x36,SEEK_SET);
		nRet = read(fd,(INT32U)&BMP_data,1024);
		for(i=0;i<256;i++){
			b=BMP_data[i*4]>>3;
			g=BMP_data[i*4+1]>>2;
			r=BMP_data[i*4+2]>>3;
			Col_buf[i]=(r<<11)+(g<<5)+(b<<0);
		}
		for(j=0;j<240;j++){
			n=j*320;
			nn=(239-j)*320;
			lseek(fd,st+nn,SEEK_SET);
			nRet = read(fd,(INT32U)&BMP_data,320);
			for(i=0;i<320;i++){
				*(bmp+n+i)=Col_buf[BMP_data[i]];
			}
		}
		close(fd);
	}
}

void Main_VideoEncode()
{
	int i;
	INT32U MemAddr=0; 
	INT16S m;
	char path[64];
	INT32S  ret;   
	INT64U  disk_size;   
	FP32    zoom_ratio;   
	MEDIA_SOURCE   src;   
	struct f_info fin;
	INT16S codeflag=0;
	int codeT=0,codetime=0;

	if(DISKUSED==FS_SD) Disk_Type=FS_SD;
	else Disk_Type=FS_NAND2;
	Chdir_Root("M1-1_video");
	spMalloc(1000);
	Palette_ram_cpy( 0,0, video_SPRITE_Palette1+256*0	,256);
	LoadData_1(1,SpriteCellData,"Pororo\\video\\SpriteCell_cmp.BIN");
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	clear_sprite(Spritebuf_number);
	Init_Video();
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);
	mic_set_pga_gain(1,8);

	step=0;
	while(1)
	{
		switch(step){
//--------------------------------------------------view
			case 0:
				clear_sprite(Spritebuf_number);
				img_board=0;
				PaintTFT_cell4();
				Start_Video();
				step=10;
			break;

			case 10:
				Chdir_Root("M1-1_video");
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,120,152,photo_buttton_Frame[2]);
				step++;
			break;
			case 11:
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);												
					step=20;
				}
			break;
			case 20:
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,120,152,photo_buttton_Frame[5]);
				WaitNoTouch_go();
				codeflag=1;
				codeT=codetime=0;
				step++;
			break;
			case 21:
				sleeptime=WAITINGTIME;
				codeT++;
				if(KeyBoardCode==K_Ok || codeT>=60*25){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);												
					codeflag=2;
					step++;
				}
			break;
			case 22:
				if(DISKUSED==FS_NAND1){
					DrvNand_flush_allblk();
				}
				step=10;
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
			if(disk_size>0x100000){
				while(1){
		        	sprintf((char *)(path), (const char *)"M1-1_%04d.avi",9000+VID_Nums);
					ret = _findfirst((char *)(path),&fin,D_ALL);
					if(ret<0){
						break;	
					}else VID_Nums++;
				}
				VID_Nums++;
				//Act_SaveNums();

				src.type = SOURCE_TYPE_FS;
				src.Format.VideoFormat = MJPEG;
				src.type_ID.FileHandle = open((char *)(path), O_WRONLY|O_CREAT|O_TRUNC);         
				if(src.type_ID.FileHandle < 0)         
				{
					DBG_PRINT("file open fail\r\n");            
					disk_safe_exit(Disk_Type);            
				}
				video_encode_start(src);
			}
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
//----------------------------------------exit video encoding demo
		if(codeflag) codeflag=0;
		if(step<10) PaintTFT_cell4();
		else PaintTFT_go();
		if(KeyBoardCode==K_Home){
			step=100;
		}
		if(step==100)
		{
			if(Video_Coding()){
				video_encode_stop();
				OSTimeDly(10);
			}
			break;
		}
	}
	video_encode_preview_stop();
	video_encode_exit();        // Release the resources needed by Video encoder
    InitFileSystem(DISKUSED);
}


void VideoEncode_Demo()
{
	VIDEO_ARGUMENT arg;     
	INT32U MemAddr=0; 

	TP_IOInit();	//init touch	
	AD_Init();		//init AD for LV
	KEY_IOInit();	//init key
	//InitCardin();	//init SD
	SetWaveIo(1);	//open À®°È
//*
	textMalloc();
	Screen_init();
	InitState();

	spMalloc(100);
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	clear_sprite(Spritebuf_number);
	gplib_ppu_fade_effect_set(ppu_register_set, 0);


	video_codec_show_image(C_DISPLAY_DEVICE, (INT32U)TVOutBuff, C_DISPLAY_DEVICE, IMAGE_OUTPUT_FORMAT_RGB565);
	fill_mem(TVOutBuff,FRAME_SIZE*2,0x1f);
//*/

	tft_init();   
	//tft_start(C_DISPLAY_DEVICE);
	//JMP17009_ST7735S_8bit_init2();

	HSM240T004_36A_7789S_I80_16();
	gpio_write_io(T430_DSP_PIN, DATA_HIGH);
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);

	openSensor();
	user_defined_video_codec_entrance();
	//while(1);

	video_encode_entrance();
	arg.bScaler = 1;		// must be 1      
	arg.TargetWidth = 320; 	//encode width   
	arg.TargetHeight = 240;	//encode height   
	arg.SensorWidth	= 320;	//sensor input width    
	arg.SensorHeight = 240;	//sensor input height   
	arg.DisplayWidth = 320;	//display width      
	arg.DisplayHeight = 240;	//display height     
	arg.DisplayBufferWidth = 320;	//display buffer width    
	arg.DisplayBufferHeight = 240;//display buffer height   
	arg.VidFrameRate = 30;	//video encode frame rate    
	arg.AudSampleRate = 8000;//audio record sample rate   
	arg.OutputFormat = IMAGE_OUTPUT_FORMAT_RGB565; //display output format    
	video_encode_preview_start(arg);


	//MemAddr=(INT32U) gp_malloc_align((FRAME_SIZE*2), 64);
	
	while(1){
		//PaintTFT_go();
	}
}
