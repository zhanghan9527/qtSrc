#include "application.h"
#include "data.h"
#include "math.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "photo_buttton.h"
#include "key.h"
#include "photoedit.h"

extern const INT16U ALIGN4 video_SPRITE_Palette1[];
void Act_VideoDemo();
void sensor_view(INT16S frame);
void auto_switch_csi_frame();
//=================================================================
IMAGE_SCALE_STRUCT Sensor_scale;
int pic_Max;
INT16S Pic_No;
INT16S Avi_Encode_Flag,imgmode=0;
INT16S imgno=0;

extern INT32U scaler_ptr,decode_mode_ck,BLOCK_ENCODE;

extern IMAGE_DECODE_STRUCT image_decode_struct;


#define MESSAGE_SP  	0
#define KEYDELAY  12


INT32U uSensorFlag;
INT32U SensorBuff1,SensorBuff2;
INT32U VideoBuff;

INT32U Sensor_Md;
INT32U sen_baseaddr;

INT32U TmpBuff1;
INT32U TmpBuff2;
INT32U TmpBuff3;


//========================================================================
void sensorFree()
{
	if(SensorBuff1) gp_free((void*)SensorBuff1);
	if(Sensor_Md) gp_free((void*)Sensor_Md);
	SensorBuff1=Sensor_Md=0;
}
void sensorMalloc()
{
	sensorFree();
	SensorBuff1 = (INT32U) gp_malloc_align((SENSOR_WIDTH*SENSOR_HEIGHT)*4, 64);//malloc senseor buffer
	SensorBuff2 = SensorBuff1+(SENSOR_WIDTH*SENSOR_HEIGHT)*2;
    sensor_buf_1=SensorBuff1;
    sensor_buf_2=SensorBuff2;
    
	TmpBuff1 = SensorBuff1+(SENSOR_WIDTH*SENSOR_HEIGHT);
	TmpBuff2 = SensorBuff1+(SENSOR_WIDTH*SENSOR_HEIGHT)*2;
	TmpBuff3 = SensorBuff1+(SENSOR_WIDTH*SENSOR_HEIGHT)*3;

	Sensor_Md =(INT32U)gp_malloc_align((SENSOR_WIDTH*SENSOR_HEIGHT)*2,64);
}

void sensorMalloc2()
{
	sensorFree();
	SensorBuff1 = (INT32U) gp_malloc_align((SENSOR_WIDTH*SENSOR_HEIGHT)*4, 64);//malloc senseor buffer
	SensorBuff2 = SensorBuff1+(SENSOR_WIDTH*SENSOR_HEIGHT)*2;
    sensor_buf_1=SensorBuff1;
    sensor_buf_2=SensorBuff2;
    
	TmpBuff1 = SensorBuff1+(SENSOR_WIDTH*SENSOR_HEIGHT);
	TmpBuff2 = SensorBuff1+(SENSOR_WIDTH*SENSOR_HEIGHT)*2;
	TmpBuff3 = SensorBuff1+(SENSOR_WIDTH*SENSOR_HEIGHT)*3;
}

void openSensor()
{
	int i;
    *(int*)SENSOR_Attrib|=SENSOR_ON;
    *(int*)SENSOR_Dir|=SENSOR_ON;
   	*(int*)SENSOR_Buffer|= SENSOR_ON;
   	*(int*)SENSOR_Data|=SENSOR_ON;	
   	drv_msec_wait(100);
}

void closeSensor()
{

   	*(int*)SENSOR_Buffer&=~ SENSOR_ON;
   	*(int*)SENSOR_Data&=~SENSOR_ON;	
}
void openSensor2()
{
	*(int*)SENSOR_Buffer|= SENSOR_ON;
	drv_msec_wait(100);
}
void DSC_demo_Init(int Sensor)	   //	 DSC_demo_Init(SENSOR_QVGA);
{
    DBG_PRINT("Enable Sensor\r\n");
    openSensor();

    if(Sensor==SENSOR_IR){//front sensor IR
		// Setup CMOS sensor

	//	user_defined_image_decode_entrance(C_TV_QVGA);
		uSensorFlag =  FT_CSI_YUVIN | FT_CSI_YUVOUT | FT_CSI_RGB1555 | 0;
		CSI_Init(SENSOR_WIDTH, SENSOR_HEIGHT, uSensorFlag, SensorBuff1, SensorBuff1);
		R_CSI_TG_CTRL0 = 0x2C67;
		R_CSI_TG_CTRL1 = 0xC090;	 

    }else if(Sensor==SENSOR_QVGA2){
    	uSensorFlag =  FT_CSI_YUVIN | FT_CSI_YUVOUT|  FT_CSI_RGB1555 | 0;
    	CSI_Init(400,300,uSensorFlag,SensorBuff1,SensorBuff1);
    }else if(Sensor==SENSOR_VGA){
    	uSensorFlag =  FT_CSI_YUVIN | FT_CSI_YUVOUT | FT_CSI_RGB1555 | 0;
    	//uSensorFlag =  FT_CSI_YUVIN | FT_CSI_RGB1555 | 0;
		CSI_Init(640,480,uSensorFlag,SensorBuff1,SensorBuff1);
    }else if(Sensor==SENSOR_VGA2){
    	uSensorFlag =  FT_CSI_YUVIN | FT_CSI_YUVOUT | FT_CSI_RGB1555 | 0;
		CSI_Init(800,600,uSensorFlag,SensorBuff1,SensorBuff1);
    }else if(Sensor==SENSOR_SVGA){
    	uSensorFlag =  FT_CSI_YUVIN | FT_CSI_YUVOUT | FT_CSI_RGB1555 | 0;
		CSI_Init(1280,720,uSensorFlag,SensorBuff1,SensorBuff1);
    }else if(Sensor==SENSOR_XVGA){
    	uSensorFlag =  FT_CSI_YUVIN | FT_CSI_YUVOUT | FT_CSI_RGB1555 | 0;
		CSI_Init(1600,1200,uSensorFlag,SensorBuff1,SensorBuff1);
    }

	buf_ck=0;
	scaler_task_state=2;
	avi_encode_eof_isr_register((void*)auto_switch_csi_frame);
	user_defined_set_sensor_irq(1);
	Sensor_EnableFrameEndInterrupt(1);	//enable CMOS SENSOR 
}


void Wait_Sensor(INT16S n)
{
	INT16S i;
	for(i=0;i<n;i++) {
		if(scaler_task_state==0) scaler_task_state=2;
		//PaintTFT_cell4();
		Paintdelay();
	}
}
void Close_Sensor()
{
	int i;
	Sensor_EnableFrameEndInterrupt(0);
	user_defined_set_sensor_irq(0);
	R_CSI_TG_CTRL0=0;
	R_CSI_TG_CTRL1=0;
	//closeSensor();
	Avi_Encode_Flag=0;
	scaler_task_state=0;
	for(i=0;i<5;i++) Paintdelay();
//    while(1);
}
INT16S Sensor_Turn;
INT16S key_turn=0;
#define KEY_TURN 0x20
void Change_SensorMode(INT16S Mod);

void sensor_turn()
{
	*(int*)IOH_BUFFER_ADDR|= (KEY_TURN);
	if(*(int*)IOH_DATA_ADDR & KEY_TURN){
		key_turn=1;
	}else{
		key_turn=0;
	}
	//key_turn=0;			//qqq		
	
	if(key_turn!=Sensor_Turn){
		if(key_turn) Change_SensorMode(11);
		else Change_SensorMode(10);
		sleeptime=WAITINGTIME;
	}
}


void image_scale_to_buff_In(INT32U frame_buffer)
{	
	img_scale.input_buf1=(void*)frame_buffer;
	image_scale(&img_scale);
}
void Sensor_scale_to_buff()
{
	INT32S ret,crop=0;

	Sensor_scale.input_buf1=(void*)(ppu_buf+crop);
	ret=image_scale(&Sensor_scale);
}
//==========================================================================
void Init_Sensor_scale(INT16U InputWidth,INT16U InputHeight,
					  INT16U OutputWidth,INT16U OutputHeight,
					  INT16U OutBuffWidth,INT16U OutBuffHeight,
					  INT16U InputFormat,INT16U OutputFormat,
					  INT32U outputBuff1
					  )
{
    Sensor_scale.cmd_id=0;
    Sensor_scale.status=0;
    Sensor_scale.special_effect=0;
    Sensor_scale.scaler_input_format=InputFormat;
    Sensor_scale.scaler_output_format=OutputFormat;

    Sensor_scale.input_width=InputWidth;
    Sensor_scale.input_height=InputHeight;
    Sensor_scale.input_visible_width=InputWidth;
    Sensor_scale.input_visible_height=InputHeight;
    Sensor_scale.input_offset_x=0;
    Sensor_scale.input_offset_y=0;
    Sensor_scale.input_buf1=NULL;
    
    Sensor_scale.output_buffer_width=OutBuffWidth;
    Sensor_scale.output_buffer_height=OutBuffHeight;
    Sensor_scale.output_width_factor=65536*InputWidth/OutputWidth;
    Sensor_scale.output_height_factor=65536*InputHeight/OutputHeight;
    Sensor_scale.out_of_boundary_color=0;
    Sensor_scale.output_buf1=(void*)outputBuff1;
    Sensor_scale.input_buf1=(void*)ppu_buf;
}
//==================================================================
#define GC0329_ID				0x62
#define GC0308_ID				0x42
void Change_SensorMode(INT16S Mod)
{
	if(Mod==0){		//qvga --nomal
	
		key_turn=0;
  		Avi_Encode_Flag=1;
	    sensor_buf_1=SensorBuff1;
	    sensor_buf_2=SensorBuff2;
	    ppu_buf=sensor_buf_1;
		DSC_demo_Init(SENSOR_VGA);
		Init_Sensor_scale(640,480,
						320,240,
						320,240,
						SCALER_CTRL_IN_YUYV,SCALER_CTRL_OUT_RGB565,// SCALER_CTRL_OUT_RGB565
						ScalerBuff);	//SCALER_CTRL_IN_UYVY
		Wait_Sensor(25);
	}else if(Mod==1){		//qvga --nomal	
		key_turn=1;

  		Avi_Encode_Flag=1;
	    sensor_buf_1=SensorBuff1;
	    sensor_buf_2=SensorBuff2;
	    ppu_buf=sensor_buf_1;
		DSC_demo_Init(SENSOR_QVGA);
		Init_Sensor_scale(320,240,
						320,240,
						320,240,
						SCALER_CTRL_IN_YUYV,SCALER_CTRL_OUT_RGB565,// SCALER_CTRL_OUT_RGB565
						ScalerBuff);	//SCALER_CTRL_IN_UYVY
		Wait_Sensor(25);
	}else if(Mod==2){		//qvga --nomal	
		*(int*)IOH_ATTRIB_ADDR&=~ (KEY_TURN);
		*(int*)IOH_DIR_ADDR   &=~ (KEY_TURN);
		*(int*)IOH_BUFFER_ADDR&=~ (KEY_TURN);
		*(int*)IOH_DATA_ADDR  &=~ (KEY_TURN);
		Paintdelay();

		if(*(int*)IOH_DATA_ADDR & KEY_TURN){
			key_turn=1;
		}else{
			key_turn=0;
		}	

  		Avi_Encode_Flag=1;
	    sensor_buf_1=SensorBuff1;
	    sensor_buf_2=SensorBuff2;
	    ppu_buf=sensor_buf_1;
		DSC_demo_Init(SENSOR_QVGA);
		Init_Sensor_scale(320,240,
						320,240,
						320,240,
						SCALER_CTRL_IN_YUYV,SCALER_CTRL_OUT_RGB565,// SCALER_CTRL_OUT_RGB565
						ScalerBuff);	//SCALER_CTRL_IN_UYVY
		Wait_Sensor(25);
	
	}else if(Mod==10){	//zi pai
		#ifdef	__GC0308_DRV_C_
			sccb_write_Reg8Data8(GC0308_ID, 0x14, 0x13);
		#else
			sccb_write_Reg8Data8(GC0329_ID, 0x17, 0x15);
		#endif
		Sensor_Turn=0;
	}else if(Mod==11){	//nomal
		#ifdef	__GC0308_DRV_C_
			sccb_write_Reg8Data8(GC0308_ID, 0x14, 0x11);
		#else	//__GC0329_DRV_C_
			sccb_write_Reg8Data8(GC0329_ID, 0x17, 0x17);	//0 mirrow,	1, UPdown
		#endif
		Sensor_Turn=1;
	}else if(Mod==20){	//change to QVGA
	    sensor_buf_1=SensorBuff1;
	    sensor_buf_2=SensorBuff2;
	    ppu_buf=sensor_buf_1;
		R_CSI_TG_HRATIO = 0x0102;
		R_CSI_TG_VRATIO = 0x0102;
	}else if(Mod==21){	//change to VGA
	    sensor_buf_1=SensorBuff1;
	    sensor_buf_2=SensorBuff1;
	    ppu_buf=sensor_buf_1;
		R_CSI_TG_HRATIO = 0;
		R_CSI_TG_VRATIO = 0;
	}
}


void GetBmpData(INT16S img_mode,INT16S img_board)
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
	if(img_mode==0){
		sprintf(path,"Pororo\\m1\\frame\\M1_1_frame%02d.bmp",img_board);
	}else{
		sprintf(path,"Pororo\\m1\\frame2\\M1_2_frameMo%02d_ani_%02d.bmp",img_board,img_mode);
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
			nn=(478-j*2)*640;
			lseek(fd,st+nn,SEEK_SET);
			nRet = read(fd,(INT32U)&BMP_data,640);
			for(i=0;i<320;i++){
				*(bmp+n+i)=Col_buf[BMP_data[i*2]];
			}
		}
		close(fd);
	}
}
void WriteBmpData(INT32U Output_buff)
{	
	INT16S* tv;
	INT16U* bmp;
	INT16U tt;
	INT32S i,j,n,nn;
	
	//*
	tv=(INT16S*)Output_buff;			//256 RGB565
	bmp=(INT16U*)VideoBuff;			//320x240 RGB data
	for(i=0;i<480;i++){
		n=i/2*320;
		if(1) nn=(i)*640;		//Sensor_Turn
		else nn=(479-i)*640;
		for(j=0;j<640;j++){
			tt=*(bmp+n+j/2);
			if(tt!=0x1f){
				*(tv+nn+j)=tt;
			}
		}
	}	//*/
}
void buff_Turn(INT32U Input_buff)
{
	INT16S* in;
	INT32S i,j,n,nn;
	INT16S temp;

	in=(INT16S*)Input_buff;
	for(i=0;i<480/2;i++){
		n=i*640;
		nn=(479-i)*640;
		for(j=0;j<640;j++){
			temp=*(in+n+j);
			*(in+n+j)=*(in+nn+j);
			*(in+nn+j)=temp;
		}
	}
}


void TurnHV(INT16S frame)	//Í¼Ïñ H V ×ª»»
{
	INT16S* sen;
	INT16S* bmp;
	INT16S* tv;
	int i,j,n,nn,k=0;
	
	sen=(INT16S*)ScalerBuff;		//320x240 RGB data
	bmp=(INT16S*)VideoBuff;		//320x240 RGB data
	tv=(INT16S*)TVOutBuff;		//240x320 TV buff

	if(frame){
		if(1){		//Sensor_Turn
			for(i=0;i<240;i++){
				n=i*320;
				nn=i*320;
				for(j=0;j<320;j++){
					if(*(bmp+nn+j)!=0x001F) *(sen+n+j)=*(bmp+n+j);
				}
			}
		}else{
			for(i=0;i<240;i++){
				n=i*320;
				nn=(239-i)*320;
				for(j=0;j<320;j++){
					if(*(bmp+nn+j)!=0x001F) *(sen+n+j)=*(bmp+nn+j);
				}
			}
		}
	}

	for(i=0;i<240;i++){
	for(j=0;j<320;j++){
		//*(tv+(319-i)*240+j)=*(sen+i+j*320);
		*(tv+(i)*320+j)=*(sen+i*320+j);
	}}
}
void TurnHV_BMP(INT16S frame)	//Í¼Ïñ H V ×ª»»
{
	INT16S* sen;
	INT16S* bmp,tmp;
	INT16S* tv;
	int i,j,n,nn,k=0;
	
	sen=(INT16S*)ppu_buf;		//ppu_buf;
	bmp=(INT16S*)VideoBuff;		//640x480 RGB data
	tv=(INT16S*)TVOutBuff;		//240x320 TV buff

	if(frame){
		if(1){		//Sensor_Turn
			for(i=0;i<480;i++){
				n=i*640;
				for(j=0;j<640;j++){
					tmp=*(bmp+n+j);
					if(tmp!=0x001F) *(sen+n+j)=tmp;
				}
			}
		}else{
			for(i=0;i<240;i++){
				n=i*320;
				nn=(239-i)*320;
				for(j=0;j<320;j++){
					if(*(bmp+nn+j)!=0x001F) *(sen+n+j)=*(bmp+nn+j);
				}
			}
		}
	}

	for(i=0;i<320;i++){
		n=i*240;
		for(j=0;j<240;j++){
			//*(tv+(319-i)*240+j)=*(sen+i+j*320);
			*(tv+n+j)=*(sen+i*2+j*2*640);
		}
	}
}

void Turn_PhotoH(INT32U input)	//Í¼Ïñ H flip
{
	INT16S* sen;
	INT16S* tv;
	int i,j,n,nn,k=0;
	
	sen=(INT16S*)input;		//320x240 RGB data
	tv=(INT16S*)TVOutBuff;		//240x320 TV buff

	for(i=0;i<320;i++){
	for(j=0;j<240;j++){
		//*(tv+(319-i)*240+j)=*(sen+i+j*320);
		*(tv+(i)*240+239-j)=*(sen+i+j*320);
	}}
}


void sensor_view(INT16S frame)
{
	//sensor_turn();
	//if(scaler_task_state==0 && video_codec_display_flag==0)
	if(scaler_task_state==0)
	{
		Sensor_scale_to_buff();
		//TurnHV_BMP(frame);
		if(frame==1000) View_Call();
		else TurnHV(frame);
		scaler_task_state=10;
	}else if(scaler_task_state==10){
		scaler_task_state=2;
		while(scaler_task_state);
	}
}

void Main_Sensor2()
{
	spMalloc(100);
	sensorMalloc();
	clear_sprite(Spritebuf_number);
	//Palette_ram_cpy( 1,0, M03_SPRITE_Palette1+256*0	,256);
	//LoadData_1(1,SpriteCellData,"15002\\m03\\SpriteCell_cmp.BIN");
	//LoadTextBmp(C_PPU_TEXT1,"15002\\Os\\Lobby\\LB_BG.jpg");

	InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);
	Change_SensorMode(0);
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);
	
	while(1)
	{
		sensor_view(0);
		PaintTFT_cell4();
//		if(Key_States&KEY_HOME){
		if(KeyBoardCode==K_Ok){

			break;	
		}
	}
	sensorFree();
	Close_Sensor();
}

INT16S Sd_Pic=-1,Rom_Pic=-1;

INT16S showPop_Save(INT16S mode)
{
	INT16S ret;
	INT64U  disk_size;
	disk_size=vfsFreeSpace(Disk_Type);
	if(disk_size>=0x100000){
		return 0;
	}
	if(mode==0) return 0;

	ret=Show_Warning(100,11);
	if(ret==-1) return -1;
	return 1;
}

void Scalerbuff_to_save() //
{
	INT16S* sen;
	INT16S* bmp;
	int ret,i,j,n,nn,k=0;
	
	if(1){
		/*
		sen=(INT16S*)ScalerBuff;		//320x240 RGB data
		bmp=(INT16S*)TV_buffer[0];		//240x320 RGB data

		for(i=0;i<240;i++){
		for(j=0;j<320;j++){
			*(sen+i*320+j)=*(bmp+239-i+j*240);
		}}//*/
		
		image_scale_YUV(VideoBuff,IconBuff,320,240);
	}
}

INT16S Save_makeup_Buff()
{

	int ret,i,j,n,nn,k=0;

	ret=showPop_Save(1);
	if(ret==0)
	{
		Scalerbuff_to_save();
		if(buff_to_save_jpgfile(IconBuff,ScalerBuff,320,240)==0){
			ret=0;
		}else ret=1;
	}
	return ret;
}

#define BOARD_MAX	20
#define BOARD_MAX2	20
//=====================================================================
void Main_Sensor()
{
    INT64U  disk_size;
	INT16S img_board=0;
	INT16S i,m;
 
	spMalloc(200);
	sensorMalloc();

	if(DISKUSED==FS_SD) Disk_Type=FS_SD;
	else Disk_Type=FS_NAND2;
	Chdir_Root("photo");
	Palette_ram_cpy( 3,0, video_SPRITE_Palette1+256*0	,256);
	LoadData_1(0,SpriteCellData,"Pororo\\m1\\SpriteCell.BIN");
	gplib_ppu_fade_effect_set(ppu_register_set, 0xff);
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);
	Change_SensorMode(0);
	clear_sprite(Spritebuf_number);
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);

	step=0;
	while(1)
	{
		PaintTFT_cell4();
		if(Goto_desktop()){
			step=100;
		}
		switch(step){
//--------------------------------------------------view
			case 0:
				img_board=0;				
				imgno=0;
				step=10;
			case 10:
			    sensor_buf_1=SensorBuff1;
			    sensor_buf_2=SensorBuff2;
				R_CSI_TG_CTRL0 |=1;
				Wait_Sensor(5);
			case 11:
				clear_sprite(Spritebuf_number);
				if(imgno==0){
					set_sprite_display_init(0,0,0,photo_buttton_Frame[0]);
					if(imgmode){
						//set_sprite_display_init(1,74,5,m1_layout_Frame[5]);
						//set_sprite_display_init(2,0,0,m1_layout_Frame[1+imgno]);
					}else{
						//set_sprite_display_init(1,74,5,m1_layout_Frame[4]);
					}
				}else{
					set_sprite_display_init(0,0,0,photo_buttton_Frame[7]);
					set_sprite_display_init(2,0,0,photo_buttton_Frame[1+imgno]);
				}
				if(img_board){
					//if(imgno==0)set_sprite_display_init(3,0,0,m1_layout_Frame[3]);
					//GetBmpData(imgmode+imgno,img_board);
				}
				step++;
			break;
			case 12:
				sensor_view(img_board);
				/*
					if(imgno==0 && TestTouch(0,60,270,320)){	//frame
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						step=11;
						if(img_board){
							img_board=0;
						}else{
							img_board=1;
						}
					}else if(imgmode && imgno==0 && TestTouch(74,129,0,40)){	//Mode
						imgmode=0;
						imgno=0;
						step=11;
				 		SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					}else if(imgmode==0 && TestTouch(129,184,0,40)){
						imgmode=1;
						imgno=0;
						step=11;
				 		SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					}else if(img_board && imgno==0 && TestTouch(0,40,140,180) ){		//<<
						if(img_board>1) img_board--;
						else img_board=BOARD_MAX;
				 		step=11;
				 		SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
					}else if(img_board && imgno==0 && TestTouch(200,240,140,180) ){		//>>
						if(img_board<BOARD_MAX) img_board++;
						else img_board=1;
				 		step=11;
				 		SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
				 	}
				}else if(img_board && imgno==0 && m==2){
					if(img_board<BOARD_MAX) img_board++;
					else img_board=1;
			 		step=11;
			 		SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[DRAG_MOVE_SFX], 64, 127, SFX_CHANL);
				}else if(img_board && imgno==0 && m==3){
					if(img_board>1) img_board--;
					else img_board=BOARD_MAX;
			 		step=11;
			 		SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[DRAG_MOVE_SFX], 64, 127, SFX_CHANL);
				}*/
				if(KeyBoardCode==K_Ok){
					step=20;
				}
			break;
//--------------------------------------------------take a photo
			case 20:
				clear_sprite(Spritebuf_number);
				//sensor_view(img_board);
 				//Wait_Sensor(5);
 				scaler_task_state=3;
 				while(scaler_task_state);
				if( showPop_Save(imgmode) ){
					step=10;
					break;
				}
 				
 				//Close_Sensor();
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[SHOT_SFX], 64, 127, 12);
				step++;
			break;
			case 21:
				//disk_size=vfsFreeSpace(Disk_Type);
				//if(disk_size<0x100000){
 				if(img_board){
		 			image_scale_RGB(ppu_buf,Sensor_Md,640,480);
		 			Paintdelay();
		 			WriteBmpData(Sensor_Md);
			 		image_scale_YUV(Sensor_Md,ppu_buf,640,480);
			 		Paintdelay();
 				}
				//Sensor_Turn	&&  save
				buff_Turn(ppu_buf);
				buff_to_save_jpgfile(ppu_buf,Sensor_Md,640,480);
 				
 				if(imgmode){
 					if(imgno) imgno=0;
 					else imgno++;
 				}
 				step=10;
			break;
			case 25:
			break;

		}
		if(step==100) break;
	}
	sensorFree();
	Close_Sensor();
	//Act_SaveNums();
}

