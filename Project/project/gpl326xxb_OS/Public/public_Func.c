//#include "application.h"
#include "Sprite_demo.h"
#include "Text_demo.h"
#include "data.h"
#include <stdio.h>


void PowerOff(void);
void ShowOffAnima(void);
void Sleep(void);

PPU_REGISTER_SETS ppu_register_structure;
PPU_REGISTER_SETS *ppu_register_set;

INT32U Tx1_buffer;
INT32U Tx2_buffer;
INT32U Tx3_buffer;
INT32U Tx4_buffer;

INT32U TV_buffer[2];
INT16U PaletteBuff0[256];
INT16U PaletteBuff1[256];
INT16U PaletteBuff2[256];
INT16U PaletteBuff3[256];

DMA_STRUCT dma_struct;
nTX_Num_Arr text_info;

INT8U Game_State;
INT8S musicStop;
INT8S volum=6;
INT16S step=0;

INT32U pFileList;
INT32U TVOutBuff,ScalerBuff;
INT32U IconBuff;

extern struct f_info	file_info;

char rootPath[100]="";
void AddRootPath(char* path)
{
	rootPath[0]=0;
	if(DISKUSED==FS_NAND1){
		strcat(rootPath,"A:\\");
	}else if(DISKUSED==FS_NAND2){
		strcat(rootPath,"B:\\");
	}else if(DISKUSED==FS_SD){
		strcat(rootPath,"C:\\");
	}
	strcat(rootPath,path);
}
const short UC_Root[][4]={
	{'A',':','\\',0},
	{'B',':','\\',0},
	{'C',':','\\',0}
};
INT16S Chdir_Root(char* path)
{
	INT16S nRet;

	if(gUnicodePage == UNI_ENGLISH){
		if(Disk_Type==FS_NAND1){
			nRet=chdir("A:\\");
		}else if(Disk_Type==FS_NAND2){
			nRet=chdir("B:\\");
		}else if(Disk_Type==FS_SD){
			nRet=chdir("C:\\");
		}
	}else{
		if(Disk_Type==FS_NAND1){
			nRet=chdir((char*)UC_Root[0]);
		}else if(Disk_Type==FS_NAND2){
			nRet=chdir((char*)UC_Root[1]);
		}else if(Disk_Type==FS_SD){
			nRet=chdir((char*)UC_Root[2]);
		}
	}
	nRet=chdir(path);
	if(nRet<0){
		nRet=mkdir(path);
		nRet=chdir(path);
	}
	return nRet;
}

INT16S Chdir_Root_UC(char *str)
{
	INT16S nRet;
	if(Disk_Type==FS_NAND1){
		nRet=chdir((char*)UC_Root[0]);
	}else if(Disk_Type==FS_NAND2){
		nRet=chdir((char*)UC_Root[1]);
	}else if(Disk_Type==FS_SD){
		nRet=chdir((char*)UC_Root[2]);
	}

	if(*str!=0){
		nRet=chdir(str);
	}
	return nRet;
}


void InitPaletteBuff()
{
    //palette ram init
    gplib_ppu_palette_ram_ptr_set(ppu_register_set, 0, (INT32U)PaletteBuff0);
    gplib_ppu_palette_ram_ptr_set(ppu_register_set, 2, (INT32U)PaletteBuff2);
    gplib_ppu_palette_ram_ptr_set(ppu_register_set, 1, (INT32U)PaletteBuff1);
    gplib_ppu_palette_ram_ptr_set(ppu_register_set, 3, (INT32U)PaletteBuff3);	
}

extern INT32U scaler_task_state;
void InitState()
{
	int i,j;
    INT32U *bptr;
	
	//gplib_ppu_sprite_enable_set(ppu_register_set, 0);	/* disable sprite */
	clear_sprite(Spritebuf_number);
    gplib_ppu_text_enable_set(ppu_register_set, C_PPU_TEXT1, 0);  /* disable text1 */	
    gplib_ppu_text_enable_set(ppu_register_set, C_PPU_TEXT2, 0);  /* disable text2 */
    gplib_ppu_text_enable_set(ppu_register_set, C_PPU_TEXT3, 0);  /* disable text3 */
    gplib_ppu_text_enable_set(ppu_register_set, C_PPU_TEXT4, 0);  /* disable text4 */	
    
    gplib_ppu_text_blend_set(ppu_register_set, C_PPU_TEXT1, 0, 1, 0);	// enable:0~1, mode:0(4 level) 1(64 level), value:0~63
    gplib_ppu_text_blend_set(ppu_register_set, C_PPU_TEXT2, 0, 1, 0);
    gplib_ppu_text_blend_set(ppu_register_set, C_PPU_TEXT3, 0, 1, 0);
    gplib_ppu_text_blend_set(ppu_register_set, C_PPU_TEXT4, 0, 1, 0);

	for (i=0; i<C_PPU_DRV_FRAME_NUM; i++) {
	    bptr = (INT32U *) TV_buffer[i];
	    for (j=0; j<FRAME_SIZE/2; j++) {
	        *bptr++ = 0x00800080;              // Black for YUYV color mode
		} 
	}//*/
	dma_memory_fill(Tx1_buffer,0,4096);
	dma_memory_fill(Tx2_buffer,0,4096);
	dma_memory_fill(Tx3_buffer,0,4096);		
	dma_memory_fill(Tx4_buffer,0,4096);	
    InitPaletteBuff();
    scaler_task_state=0;
}

void text_disable(INT32U text_index)
{
	gplib_ppu_text_enable_set(ppu_register_set, text_index, 0);  /* disable text1 */	
}



INT16U GetRandom(INT16U Max)
{
	return (*(INT16U *)0xD0500380>>3)%Max;
}
INT16U GetRandom_time(INT16U Max)
{
	return (game_time+(*(INT16U *)0xD0500380>>3))%Max;
}


void clearRandom(int Max,INT16S* randBuff)
{
	int i,m;
	INT16S buff[100];
	for(i=0;i<Max;i++) buff[i]=i;
	for(i=0;i<Max;i++){
		m=GetRandom(Max);
		while(buff[m]==-1){
			m=(m+1)%Max;
		}
		randBuff[i]=m;
		buff[m]=-1;
	}
}

INT16S getRandBuff(int Max,INT16S numIdx[] ,INT16S* randBuff)
{
	if( numIdx[0]>=Max || numIdx[0]<0){
		clearRandom(Max,randBuff);
		numIdx[0]=0;
	}
	return randBuff[numIdx[0]++];
}


void PlaySound(int i,int ch)
{
	SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[i], 64, 50, ch);
}

void PlaySpeech(int i)
{
	SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[i], 64, 110, 14);
}

void stopSpu()
{
	int i;
	for(i=0;i<16;i++) SPU_StopChannel(i);
//	SPU_Init();
}

INT8S musicStop;
void PlayMusic(int i,int vol )
{
	if(musicStop==0){
		SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[i], 64, vol, 15);
	}
}

void stopMusic()
{
	if(Key_States&KEY_MUSIC){
		if(musicStop){
			musicStop=0;
		}else{
			musicStop=1;
			SPU_StopChannel(15);
		}
	}
}


INT16S SpeechBuff[15]={-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
INT16S playdelay=0;
void popSpeech()
{
	int i;
	for(i=1;i<13;i+=2){
		SpeechBuff[i]=SpeechBuff[i+2];
		SpeechBuff[i+1]=SpeechBuff[i+3];
	}
	SpeechBuff[i]=-1;
	SpeechBuff[i+1]=0;
	SpeechBuff[0]--;
}

void PlaySpeechBuff()
{
	if(SPU_Get_SingleChannel_Status(SPEECHCHANL)==0){
		if(playdelay) playdelay--;
		else if( (SpeechBuff[0]>0) && (SpeechBuff[1]!=-1) ){
			PlaySpeech(SpeechBuff[1]);
			playdelay=SpeechBuff[2];
			popSpeech();
		}else SpeechBuff[0]=-1;
	}
}


void StopSpeech()
{
	SPU_StopChannel(SPEECHCHANL);
	SpeechBuff[0]=-1;
	playdelay=0;
}

void Play_Speech(INT16S num,
				INT16S sh0,INT16S st0,
				INT16S sh1,INT16S st1,
				INT16S sh2,INT16S st2,
				INT16S sh3,INT16S st3,
				INT16S sh4,INT16S st4,
				INT16S sh5,INT16S st5)
{
	INT16S i,h,t;
	StopSpeech();
	SpeechBuff[0]=num;
	for(i=0;i<num;i++){
		if(i==0) h=sh0,t=st0;
		else if(i==1) h=sh1,t=st1;
		else if(i==2) h=sh2,t=st2;
		else if(i==3) h=sh3,t=st3;
		else if(i==4) h=sh4,t=st4;
		else h=sh5,t=st5;
		SpeechBuff[1+2*i]=h;
		SpeechBuff[2+2*i]=t;
	}
}
INT16S PLAY_Flag=1;
void Stop_Wav()
{
	INT16S i;
	if(PLAY_Flag){
		SetWaveIo(1);
	}else if(volum==0){
		SetWaveIo(0);
	}else{
		for(i=0;i<16;i++){
			if(SPU_Get_SingleChannel_Status(i)) break;
		}
		if(i<16) SetWaveIo(1);	//open 喇叭
		else SetWaveIo(0);
	}
}
//==========================================================
void textMalloc()
{
	int i;
	int *addr;
	TV_buffer[0] = (INT32U) gp_malloc_align((FRAME_SIZE*2), 64);
	addr=(int*)TV_buffer[0];
	for(i=0;i<FRAME_SIZE/2;i++){
		*addr++=0;
	}
	TV_buffer[1] = (INT32U) gp_malloc_align((FRAME_SIZE*2), 64);
	addr=(int*)TV_buffer[1];
	for(i=0;i<FRAME_SIZE/2;i++){
		*addr++=0;
	}
	  
	Tx1_buffer = (INT32U) gp_malloc_align(1024*4, 4);
	dma_memory_fill(Tx1_buffer,0,4096);
	
	Tx2_buffer = (INT32U) gp_malloc_align(1024*4, 4);
	dma_memory_fill(Tx2_buffer,0,4096);
	
	Tx3_buffer = (INT32U) gp_malloc_align(1024*4, 4);
	dma_memory_fill(Tx3_buffer,0,4096);
	
	Tx4_buffer = (INT32U) gp_malloc_align(1024*4, 4);
	dma_memory_fill(Tx4_buffer,0,4096);
	
	pFileList=(INT32U)gp_malloc_align(40000, 4);

	TVOutBuff=(INT32U) gp_malloc_align(FRAME_SIZE*2*2, 64);
	ScalerBuff=(INT32U) gp_malloc_align(FRAME_SIZE*2*2, 64);
//	ScalerBuff=TVOutBuff+(INT32U) gp_malloc_align(FRAME_SIZE*2, 64);
	
	IconBuff=(INT32U) gp_malloc_align(FRAME_SIZE*2*2, 64);	
	
	VideoBuff=(INT32U) gp_malloc_align(FRAME_SIZE*2, 64);
}
void spFree()
{
	if(SpriteCellData)gp_free((void*)SpriteCellData);
	SpriteCellData=0;
}
void spMalloc(INT32U size_k)
{
	spFree();
	SpriteCellData= (INT32U) gp_malloc_align(size_k*_1K,4);
}

void Copy8(INT32U TagData,INT32U SrcData,INT32U num)
{
	INT8U *Tag=(INT8U*)TagData;
	INT8U *Src=(INT8U*)SrcData;
	while(num)
	{
		*Tag++=*Src++;
		num--;
	}
}

void Copy16(INT32U TagData,INT32U SrcData,INT32U num)
{
	INT16U *Tag=(INT16U*)TagData;
	INT16U *Src=(INT16U*)SrcData;
	while(num)
	{
		*Tag++=*Src++;
		num--;
	}
}

void Copy32(INT32U TagData,INT32U SrcData,INT32U num)
{
	INT32U *Tag=(INT32U*)TagData;
	INT32U *Src=(INT32U*)SrcData;

	while(num)
	{
		*Tag++=*Src++;
		num--;
	}
}
void fill_mem(INT32U TagData,INT32U num,INT8U data)
{
	INT8U *Tag=(INT8U*)TagData;

	while(num)
	{
		*Tag++=data;
		num--;
	}
}

//,TX_HSIZE32 ,TX_VSIZE16
void InitText(INT32U text_index,INT32U TextCelldata,INT32U Tx_buffer,INT32U textdepth,INT32U Bank,INT32U Paletteidx,INT8U TxColor,INT16S Textx,INT16S Texty,INT8U Hsize,INT8U Vsize)
{
	gplib_ppu_text_init(ppu_register_set, text_index);
	gplib_ppu_text_enable_set(ppu_register_set, text_index, 1);	                // Enable TEXT
	gplib_ppu_text_compress_disable_set(ppu_register_set, 1);	                    // Disable TEXT1/TEXT2 horizontal/vertical compress function
    gplib_ppu_text_direct_mode_set(ppu_register_set, 0);		               		// Disable TEXT direct address mode
	gplib_ppu_text_mode_set(ppu_register_set, text_index,0);
    gplib_ppu_text_attribute_source_select(ppu_register_set, text_index, 1);	    // Get TEXT attribute from register
	gplib_ppu_text_size_set(ppu_register_set, text_index, C_TXN_SIZE_1024X512);	// Set TEXT size to 1024x512
	gplib_ppu_text_character_size_set(ppu_register_set, text_index,Hsize,Vsize);
	gplib_ppu_text_segment_set(ppu_register_set, text_index,	TextCelldata);				    // Set TEXT segment address	
	gplib_ppu_text_depth_set(ppu_register_set, text_index, textdepth);	
	gplib_ppu_text_number_array_ptr_set(ppu_register_set, text_index,Tx_buffer);	 // Set TEXT number array address

  	//gplib_ppu_yuv_type_set(ppu_register_set, 3);								     // Set 32-bit color format to Y1UY0V
	gplib_ppu_text_bitmap_mode_set(ppu_register_set, text_index, 0);			     // Enable bitmap mode
	gplib_ppu_text_color_set(ppu_register_set, text_index, 0, TxColor);			// rgb_mode:0~1, color=0~3
	gplib_ppu_text_palette_set(ppu_register_set, text_index,Bank,Paletteidx);			// bank:0~3, palette_idx:0~15
	gplib_ppu_text_position_set(ppu_register_set, text_index,	Textx, Texty);                // Set TEXT position
}


void InitTextBmp(INT32U text_index,INT32U Tx_buffer,INT32U CellData,INT32U textdepth,INT16S Textx,INT16S Texty,INT16S colormode)
{
	gplib_ppu_text_init(ppu_register_set, text_index);
	gplib_ppu_text_enable_set(ppu_register_set, text_index, 1);	                // Enable TEXT
	gplib_ppu_text_compress_disable_set(ppu_register_set, 1);	                    // Disable TEXT1/TEXT2 horizontal/vertical compress function
    gplib_ppu_text_direct_mode_set(ppu_register_set, 0);		               		// Disable TEXT direct address mode
	gplib_ppu_text_mode_set(ppu_register_set, text_index,0);
    gplib_ppu_text_attribute_source_select(ppu_register_set, text_index, 1);	    // Get TEXT attribute from register
	gplib_ppu_text_size_set(ppu_register_set, text_index, C_TXN_SIZE_1024X512);	// Set TEXT size to 1024x512
	gplib_ppu_text_segment_set(ppu_register_set, text_index,0);				    // Set TEXT segment address	
	gplib_ppu_text_depth_set(ppu_register_set, text_index, textdepth);	
	
	gplib_ppu_text_number_array_ptr_set(ppu_register_set, text_index,Tx_buffer);	 // Set TEXT number array address		
    if(colormode==BMP_YUYV){
		gplib_ppu_yuv_type_set(ppu_register_set, 3);								     // Set 32-bit color format to Y1UY0V
	}
	gplib_ppu_text_bitmap_mode_set(ppu_register_set, text_index, 1);			     // Enable bitmap mode
	gplib_ppu_text_color_set(ppu_register_set, text_index, 1, colormode);
	gplib_ppu_text_position_set(ppu_register_set, text_index,	Textx, Texty);                // Set TEXT position

	gplib_ppu_text_calculate_number_array(ppu_register_set, text_index, PPU_TEXT_SIZE_HPIXEL4, PPU_TEXT_SIZE_VPIXEL4, (INT32U)CellData);	// Calculate Number array
}

void InitText_Scaler(INT32U text_index,INT32U CellData,INT16S Weight,INT16S Height,INT32U textdepth,INT16S colormode)
{
	INT32U Tx_buffer;

	gplib_ppu_text_init(ppu_register_set, text_index);
	gplib_ppu_text_enable_set(ppu_register_set, text_index, 1);	                // Enable TEXT
	gplib_ppu_text_compress_disable_set(ppu_register_set, 1);	                    // Disable TEXT1/TEXT2 horizontal/vertical compress function
    gplib_ppu_text_direct_mode_set(ppu_register_set, 0);		               		// Disable TEXT direct address mode
	gplib_ppu_text_mode_set(ppu_register_set, text_index,0);
    gplib_ppu_text_attribute_source_select(ppu_register_set, text_index, 1);	    // Get TEXT attribute from register
	gplib_ppu_text_size_set(ppu_register_set, text_index, C_TXN_SIZE_1024X1024);	// Set TEXT size to 1024x512
	gplib_ppu_text_segment_set(ppu_register_set, text_index,0);				    // Set TEXT segment address	
	gplib_ppu_text_depth_set(ppu_register_set, text_index, textdepth);	
	
	if(text_index==C_PPU_TEXT1) Tx_buffer=Tx1_buffer;
	else if(text_index==C_PPU_TEXT2) Tx_buffer=Tx2_buffer;
	else if(text_index==C_PPU_TEXT3) Tx_buffer=Tx3_buffer;
	else if(text_index==C_PPU_TEXT4) Tx_buffer=Tx4_buffer;
	
	gplib_ppu_text_number_array_ptr_set(ppu_register_set, text_index,Tx_buffer);	 // Set TEXT number array address		

    if(colormode==BMP_YUYV){
		gplib_ppu_yuv_type_set(ppu_register_set, 3);								     // Set 32-bit color format to Y1UY0V
	}
	gplib_ppu_text_bitmap_mode_set(ppu_register_set, text_index, 1);			     // Enable bitmap mode
	gplib_ppu_text_color_set(ppu_register_set, text_index, 1, colormode);
	gplib_ppu_text_position_set(ppu_register_set, text_index,	0, 0);                // Set TEXT position

	gplib_ppu_text_calculate_number_array(ppu_register_set, text_index, Weight, Height, (INT32U)CellData);	// Calculate Number array
	
}
//==================================================================================
// 图片的路径是相对路径

INT16S LoadNoteBookIconRel(INT16U width, INT16U height, char *path)
{
	INT16S eng=0;

	INT16S* icon;
	INT16S* bmp;
	INT16S* tv;
	int i,j,n,nn,k=0;	
	
	
	if(gUnicodePage != UNI_ENGLISH){
		ChangeCodePage(UNI_ENGLISH);
		eng=1;
	}
//	AddRootPath(path);		//加盘符
	if(Photo_decode_to_buff(path,width,height,IconBuff)>=0){
//	if(image_decode_to_buff(rootPath,TVOutBuff)>=0){
		;
	}else fill_mem(IconBuff,320*240*2*2,0);
//	Copy32(TVOutBuff,ScalerBuff,FRAME_SIZE/2);

//	if(text==C_PPU_TEXT1)
//		InitText_Scaler(C_PPU_TEXT1,IconBuff,320,240,TX_DEPTH0,BMP_RGB565);
//	else if(text==C_PPU_TEXT2)
//		InitText_Scaler(C_PPU_TEXT2,IconBuff,320,240,TX_DEPTH0,BMP_RGB565);
		

	
	icon=(INT16S*)IconBuff;	
	tv=(INT16S*)TVOutBuff;	



	for(i=0;i<180;i++){
		n=i*320;
		nn=(i+30)*320;
		for(j=0;j<120;j++){
			*(tv+nn+j+100)=*(icon+n+j);
//			if(*(bmp+nn+j)!=0x001F) *(sen+n+j)=*(bmp+n+j);
		}
	}

	if(eng) ChangeCodePage(UNI_UNICODE);
}


INT16S LoadNoteBookIcon(INT16U width, INT16U height, char *path)
{
	INT16S eng=0;

	INT16S* icon;
	INT16S* bmp;
	INT16S* tv;
	int i,j,n,nn,k=0;	
	
	
	if(gUnicodePage != UNI_ENGLISH){
		ChangeCodePage(UNI_ENGLISH);
		eng=1;
	}
	AddRootPath(path);		//加盘符
	if(Photo_decode_to_buff(rootPath,width,height,IconBuff)>=0){
//	if(image_decode_to_buff(rootPath,TVOutBuff)>=0){
		;
	}else fill_mem(IconBuff,320*240*2*2,0);
//	Copy32(TVOutBuff,ScalerBuff,FRAME_SIZE/2);

//	if(text==C_PPU_TEXT1)
//		InitText_Scaler(C_PPU_TEXT1,IconBuff,320,240,TX_DEPTH0,BMP_RGB565);
//	else if(text==C_PPU_TEXT2)
//		InitText_Scaler(C_PPU_TEXT2,IconBuff,320,240,TX_DEPTH0,BMP_RGB565);
		

	
	icon=(INT16S*)IconBuff;	
	tv=(INT16S*)TVOutBuff;	



	for(i=0;i<180;i++){
		n=i*320;
		nn=(i+30)*320;
		for(j=0;j<120;j++){
			*(tv+nn+j+100)=*(icon+n+j);
//			if(*(bmp+nn+j)!=0x001F) *(sen+n+j)=*(bmp+n+j);
		}
	}

	if(eng) ChangeCodePage(UNI_UNICODE);
}
//==================================================================================
// 图片的路径是相对路径
INT16S LoadNoteBookTextBmp_Rel(INT16S text, INT16U width, INT16U height, char *path)
{
	INT16S eng=0;

	INT16S* icon;
	INT16S* bmp;
	INT16S* tv;
	int i,j,n,nn,k=0;	
	
	
	if(gUnicodePage != UNI_ENGLISH){
		ChangeCodePage(UNI_ENGLISH);
		eng=1;
	}
//	AddRootPath(path);
	if(Photo_decode_to_buff(path,width,height,TVOutBuff)>=0){
//	if(image_decode_to_buff(rootPath,TVOutBuff)>=0){
		;
	}else fill_mem(TVOutBuff,320*240*2*2,0);
	Copy32(IconBuff,TVOutBuff,FRAME_SIZE);

	if(text==C_PPU_TEXT1)
		InitText_Scaler(C_PPU_TEXT1,TVOutBuff,width,height,TX_DEPTH0,BMP_RGB565);
	else if(text==C_PPU_TEXT2)
		InitText_Scaler(C_PPU_TEXT2,TVOutBuff,width,height,TX_DEPTH0,BMP_RGB565);
		
	if(eng) ChangeCodePage(UNI_UNICODE);
}




INT16S LoadNoteBookTextBmp(INT16S text, INT16U width, INT16U height, char *path)
{
	INT16S eng=0;

	INT16S* icon;
	INT16S* bmp;
	INT16S* tv;
	int i,j,n,nn,k=0;	
	
	
	if(gUnicodePage != UNI_ENGLISH){
		ChangeCodePage(UNI_ENGLISH);
		eng=1;
	}
	AddRootPath(path);
	if(Photo_decode_to_buff(rootPath,width,height,TVOutBuff)>=0){
//	if(image_decode_to_buff(rootPath,TVOutBuff)>=0){
		;
	}else fill_mem(TVOutBuff,320*240*2*2,0);
	Copy32(IconBuff,TVOutBuff,FRAME_SIZE);

	if(text==C_PPU_TEXT1)
		InitText_Scaler(C_PPU_TEXT1,TVOutBuff,width,height,TX_DEPTH0,BMP_RGB565);
	else if(text==C_PPU_TEXT2)
		InitText_Scaler(C_PPU_TEXT2,TVOutBuff,width,height,TX_DEPTH0,BMP_RGB565);
		
	if(eng) ChangeCodePage(UNI_UNICODE);
}


INT16S LoadTextBmp(INT16S text, char *path)
{
	INT16S eng=0;
	if(gUnicodePage != UNI_ENGLISH){
		ChangeCodePage(UNI_ENGLISH);
		eng=1;
	}
	AddRootPath(path);
	if(image_decode_to_buff(rootPath,TVOutBuff)>=0){
		;
	}else fill_mem(TVOutBuff,FRAME_SIZE*2,0);
	//else fill_mem(ScalerBuff,FRAME_SIZE*2,0);
	//Copy32(TVOutBuff,ScalerBuff,FRAME_SIZE/2);

	if(text==C_PPU_TEXT1)
		InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);
	else if(text==C_PPU_TEXT2)
		InitTextBmp(C_PPU_TEXT2,Tx2_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);

	if(eng) ChangeCodePage(UNI_UNICODE);
}

void show_color_RGB565_bg(INT16U R,INT16U G,INT16U B)
{
	INT16U col;
	col=(R&0xF8)<<8|(G&0xFC)<<3|B>>3;	
	PPU_MemSet((INT32U *)TVOutBuff,col,FRAME_SIZE*2,16);
	InitTextBmp(C_PPU_TEXT1,Tx1_buffer,(INT32U)TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);	
}


void setTextInit(int text,int col,int bank,int pal,int depth,INT32U cell_addr)
{
	gplib_ppu_text_init(ppu_register_set, text);
	// Now configure TEXT relative elements
	gplib_ppu_text_enable_set(ppu_register_set, text, 1);	                // Enable TEXT
	gplib_ppu_text_compress_disable_set(ppu_register_set, 1);	                    // Disable TEXT1/TEXT2 horizontal/vertical compress function
    gplib_ppu_text_direct_mode_set(ppu_register_set, 0);			                // Disable TEXT direct address mode
	gplib_ppu_text_mode_set(ppu_register_set, text,0);
    gplib_ppu_text_attribute_source_select(ppu_register_set, text, 1);	    // Get TEXT attribute from register
	gplib_ppu_text_size_set(ppu_register_set, text, C_TXN_SIZE_1024X512);	// Set TEXT size to 1024x512
	gplib_ppu_text_character_size_set(ppu_register_set, text,1,1);
	gplib_ppu_text_segment_set(ppu_register_set, text,cell_addr);				    // Set TEXT segment address	

	gplib_ppu_text_position_set(ppu_register_set, text, 0, 0);                // Set TEXT position
  	//gplib_ppu_yuv_type_set(ppu_register_set, 3);								     // Set 32-bit color format to Y1UY0V
	gplib_ppu_text_bitmap_mode_set(ppu_register_set, text, 0);			     // Enable bitmap mode
	gplib_ppu_text_color_set(ppu_register_set, text, 0, col);				     // Set TEXT color to RGB565
	gplib_ppu_text_palette_set(ppu_register_set, text,bank,pal);
	gplib_ppu_text_depth_set(ppu_register_set, text,depth);
}

void text_Update(int text,int tWidth,int tHeight,int iWidth,int iHeight,INT32U index_addr)
{
	text_info.nTxAddr_mode=0;
	text_info.nChar_width=16;
	text_info.nChar_height=16;
	text_info.ntext_width=tWidth;
	text_info.ntext_height=tHeight;
	text_info.nImage_width=iWidth;
	text_info.nImage_height=iHeight;
	if(text==0){
		text_info.Num_arr_ptr=(INT32U)Tx1_buffer;
	}else if(text==1){
		text_info.Num_arr_ptr=(INT32U)Tx2_buffer;
	}else if(text==2){
		text_info.Num_arr_ptr=(INT32U)Tx3_buffer;
	}else{
		text_info.Num_arr_ptr=(INT32U)Tx4_buffer;
	}
	gplib_ppu_text_number_array_ptr_set(ppu_register_set, text,text_info.Num_arr_ptr);
	text_info.index_ptr=index_addr;
	text_info.data_ptr=0;
	Update_Number_Array((PnTX_Num_Arr)&text_info);
}
void set_index_ptr(INT32U index_addr)
{
	text_info.index_ptr=index_addr;
	Update_Number_Array((PnTX_Num_Arr)&text_info);
}

int counter=0;
void Paintdelay()  //delay到一秒25帧
{
	int dt;

	do
	{
		dt=((tiny_counter_get() | 0x10000) - counter) & (0xFFFF);
		
	}while(dt<40*375);	// 375=1ms

	counter=tiny_counter_get();
}

void PaintTFT_cell4()
{
	if(Desk_flag) ;//Test_Player();
	{
		paint_ppu_spriteram_cell4(ppu_register_set,Sprite_Coordinate_480X272,Left_Top_coordinate_mode,Spritebuf_number);
		gplib_ppu_go_and_wait_done(ppu_register_set);
//*
		Paintdelay();

		game_time++;
//		TouchPanelService();
		GetKeyBoardCode();
		//PowerOff();
		//ShowPower();
		//Sleep();
		//Alarm_Test();
	   	//usb_TestIo();
	   	//SD_Server();
	   	//Mep003_Server();
		//Stop_Wav();
    }
}



void PaintTFT_in()
{	
		paint_ppu_spriteram_cell4(ppu_register_set,Sprite_Coordinate_480X272,Left_Top_coordinate_mode,Spritebuf_number);
		gplib_ppu_go_and_wait_done(ppu_register_set);
}

void PaintTFT_go()
{	
	if(Desk_flag) Test_Player();
	//paint_ppu_spriteram_cell4(ppu_register_set,Sprite_Coordinate_320X240,Left_Top_coordinate_mode,Spritebuf_number);
	{
		Paintdelay();

		game_time++;
	
		GetKeyBoardCode();
	   	usb_TestIo();
		PowerOff();
	    Sleep();
	    SD_Server();

		ShowPower();
		//show_mem_space();
//		show_alarmFlag();

	    Alarm_Test();
	    //Stop_Wav();
    }
}
void PaintTFT_Power()
{
	if(Desk_flag) Test_Player();
	paint_ppu_spriteram_cell4(ppu_register_set,Sprite_Coordinate_480X272,Left_Top_coordinate_mode,Spritebuf_number);
	gplib_ppu_go_and_wait_done(ppu_register_set);
	
	game_time++;
	Paintdelay();
	GetKeyBoardCode();
}


void Palette_ram_cpy(int bank,int off,INT16U *addr,int num)
{
	int i;
	INT16U *p_pal;

	if(bank==0) p_pal= (INT16U*)ppu_register_set->ppu_palette0_ptr+off;
	else if(bank==1) p_pal= (INT16U*)ppu_register_set->ppu_palette1_ptr+off;
	else if(bank==2) p_pal= (INT16U*)ppu_register_set->ppu_palette2_ptr+off;
	else p_pal= (INT16U*)ppu_register_set->ppu_palette3_ptr+off;
	
	for(i=num;i>0;i--){
		*p_pal++=*addr++;
	}
	ppu_register_set->update_register_flag|=C_UPDATE_REG_SET_PALETTE0<<bank;
}

void Palette_ram_Fill(int bank,int off,INT16U addr,int num)
{
	int i;
	INT16U *p_pal;

	if(bank==0) p_pal= (INT16U*)ppu_register_set->ppu_palette0_ptr;
	else if(bank==1) p_pal= (INT16U*)ppu_register_set->ppu_palette1_ptr;
	else if(bank==2) p_pal= (INT16U*)ppu_register_set->ppu_palette2_ptr;
	else p_pal= (INT16U*)ppu_register_set->ppu_palette3_ptr;
	for(i=0;i<num;i++){
		p_pal[off+i]=addr;
	}
	ppu_register_set->update_register_flag|=C_UPDATE_REG_SET_PALETTE0<<bank;
}

//-----------------------------------------------------

INT16S InitFileSystem(INT32S Disk)
{
	Disk_Type=Disk;
	Chdir_Root(0);
	
	return 0;
}
INT16S mount_Disk(INT32S Disk)
{

	INT16S  ret,i=0;
	
	while(1)
	{
		if(_devicemount(Disk))					// Mount device nand
		{
			DBG_PRINT("Mount DISK Fail....\r\n");
			if(++i>=2) {
				ret=-1;
				break;
			}
		}
		else
		{
			DBG_PRINT("Mount DISK success....\r\n");
			ret=0;
			break;
		}
		OSTimeDly(5);
	}
	return ret;
}

INT32S mount_and_mk_ch_dir(void)
{

	_deviceunmount(DISKUSED);
	
	DBG_PRINT("Mount DISK start\r\n");
	mount_Disk(DISKUSED);
	//mount_Disk(FS_NAND2);

    if(DISKUSED==FS_NAND1){
		InitCardin();
		//*
		OSTimeDly(5);
		if(TestCardin()){
			if(mount_Disk(FS_SD)==0) SD_Mount=1;
			else SD_Mount=0;
		}else SD_Mount=-1;		//*/
    }else SD_Mount=1;

	Disk_Type=DISKUSED;
	Chdir_Root(0);
	return DISKUSED;
}

//=================================================================================================
//	INT16S Get_Name_List(char *pPath,char *pExp)  寻找当前文件夹下所有指定扩展名的文件
//	pPath==路径；   pExp==扩展名；pFileList 	ENGLISH
//	return: 文件个数
// list 格式 2*16U: 	entry + no(文件特征)
//=================================================================================================
INT16U Get_Name_List(char *pPath,char *pExp,INT32U listAddr)
{
	INT16S ret,num;
	struct f_info fin;
	INT16U* list=(INT16U*)listAddr;

	num=0;
	if(pPath==NULL || pExp==NULL) return 0;
	Chdir_Root(pPath);
	ret = _findfirst(pExp,&fin,D_ALL);
	if(ret<0) return 0;
	while(1){
		*(list+2*num)=fin.entry;
		*(list+2*num+1)=0;
		num++;
		ret =_findnext(&fin);
		if(ret<0) break;
	}
	return num;
}

//信息存在 file_info 
INT16S Get_file_info(INT16U picnum,INT32U listAddr,INT16U Max)
{
	INT16S i,ret;
	INT16U* list=(INT16U*)listAddr;

	if(picnum>=Max) return -1;
	i=Max-picnum-1;
	seekdir(*(list+2*i));
	ret = _findnext(&file_info);
	if(ret==0) return *(list+2*i+1);
	return ret;
}

INT16S Del_One_file(INT16S picnum,INT32U listAddr,INT16U Max)
{
	INT16S i,Num;
	INT16U* list=(INT16U*)listAddr;
	
	for(i=Max-picnum-1;i<Max-1;i++){
		*(list+2*i)=*(list+2*i+2);
		*(list+2*i+1)=*(list+2*i+3);
	}
	return Max-1;
}

//=========================================================================
void Screen_init(void)
{
	int i,j;

    INT32U *bptr;

    // Initiate display device
	//tft_init();
//	tv_init();
    //tft_init();
		//TFT_TD025THED1_Init();
		//tft_start(AUO_A024CN02V9);
		//TFT_AU024_Init();
	//tft_HV430_init();
	//HV430V3B_HX8257_init();

// Initiate display device
    if(DISPLAY_DEVICE==DISPLAY_TFT){
		tft_init();
    }else{
	    tv_init();
    }

	SPU_Init();
    SPU_Enable_MultiChannel(0xffff);
	SPU_Set_MainVolume(VOLUME_MAX*volum/3);
	SPU_MIDI_Set_SPU_Channel_Mask(0x003f);

	//mount_and_mk_ch_dir();
	
   /* initial ppu register parameter set structure */
    ppu_register_set = (PPU_REGISTER_SETS *) &ppu_register_structure;

    // Initiate PPU hardware engine and PPU register set structure
	gplib_ppu_init(ppu_register_set);
	

	// Now configure PPU software structure
	gplib_ppu_enable_set(ppu_register_set, 1);					    // Enable PPU
	gplib_ppu_char0_transparent_set(ppu_register_set, 1);		    // Enable character 0 transparent mode
	gplib_ppu_bottom_up_mode_set(ppu_register_set, 1);

    // TV frame mode
  	gplib_ppu_non_interlace_set(ppu_register_set, 0);			// Set interlace mode
  	gplib_ppu_frame_buffer_mode_set(ppu_register_set, 1, 1);		// Enable TV/TFT frame buffer mode
  	gplib_ppu_rgb565_transparent_color_set(ppu_register_set,0,0);

	gplib_ppu_fb_format_set(ppu_register_set, 0, 0);			      // Set PPU output frame buffer format to RGB565


    gplib_ppu_vga_mode_set(ppu_register_set, 0);							// disable VGA mode
    gplib_ppu_resolution_set(ppu_register_set, 0);
    gplib_ppu_free_size_set(ppu_register_set,0,480, 272);	//set display resolution  128x128
//      gplib_ppu_window_set(ppu_register_set, 0, 480, 272);

   	gplib_ppu_palette_type_set(ppu_register_set, 1, 0);
   	
	gplib_ppu_text_compress_disable_set(ppu_register_set, 1);	                    // Disable TEXT1/TEXT2 horizontal/vertical compress function
    gplib_ppu_text_direct_mode_set(ppu_register_set, 0);			                // Disable TEXT direct address mode

    gplib_ppu_deflicker_mode_set(0);			            // De-flicker is only valid under VGA + Interlace + Frame mode
   
    Curren_Frame=0;
	for (i=0; i<C_PPU_DRV_FRAME_NUM; i++) {
		gplib_ppu_frame_buffer_add(ppu_register_set, TV_buffer[i]);
	}
	
#if DISPLAYUSED==TFT_DISPLAY
	//video_codec_show_image(DISPLAYUSED, (INT32U)T80_buffer,VGA_MODE , IMAGE_OUTPUT_FORMAT_RGB565);//VGA_MODE //IMAGE_OUTPUT_FORMAT_YUYV
#endif
	
	//bmp
	//gplib_ppu_text_number_array_ptr_set(ppu_register_set, C_PPU_TEXT1, Tx1_buffer);	 // Set TEXT number array address
	//gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0, 0);                // Set TEXT position
  	//gplib_ppu_yuv_type_set(ppu_register_set, 3);								     // Set 32-bit color format to Y1UY0V
	//gplib_ppu_text_bitmap_mode_set(ppu_register_set, C_PPU_TEXT1, 1);			     // Enable bitmap mode
	//gplib_ppu_text_color_set(ppu_register_set, C_PPU_TEXT1, 1, 1);				     // Set TEXT color to RGB565
	//gplib_ppu_text_calculate_number_array(ppu_register_set, C_PPU_TEXT1, 640, 480, (INT32U)TVOutBuff);	// Calculate Number array


	//load spritecelldata
//	SDCARD_init();
    	// Now configure Sprite
	gplib_ppu_sprite_init(ppu_register_set);
	gplib_ppu_sprite_enable_set(ppu_register_set, 1);			// Enable Sprite
	gplib_ppu_sprite_coordinate_set(ppu_register_set, 0);
	gplib_ppu_sprite_direct_mode_set(ppu_register_set, 0);		// Set sprite direct address mode
	gplib_ppu_sprite_number_set(ppu_register_set, Spritebuf_number);           //
	gplib_ppu_sprite_attribute_ram_ptr_set(ppu_register_set, (INT32U)SpriteRAM);//set sprite ram buffer
	gplib_ppu_exsprite_enable_set(ppu_register_set,0);

	//gplib_ppu_sprite_zoom_enable_set(ppu_register_set, 1);		// value:0=disable 1=enable
	//gplib_ppu_sprite_rotate_enable_set(ppu_register_set, 1);		// value:0=disable 1=enable
	gpio_write_io(T430_RESET_PIN, DATA_LOW);

	//HSM240T004_36A_7789S_I80_16();
	FT043S480272NC07_init();

    buf_ck=0;
	gplib_ppu_go_and_wait_done(ppu_register_set);
	gplib_ppu_fade_effect_set(ppu_register_set, 0xff);
	gplib_ppu_sprite_rotate_enable_set(ppu_register_set,1);
	gplib_ppu_sprite_blend_mode_set(ppu_register_set,1);

	gpio_write_io(T430_DSP_PIN, DATA_LOW);

}


void Fade_in(void)
{
	int i=0xff;
	while(1)
	{
		i-=16;
		if(i<0){
			i=0;
			break;
		}
		gplib_ppu_fade_effect_set(ppu_register_set, i);
		PaintTFT_in();
	}
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	PaintTFT_in();
}

void Fade_out(void)
{
	int i=0x0;
	while(1)
	{
		i+=16;
		if(i>0xff){
			i=0xff;
			break;
		}
		gplib_ppu_fade_effect_set(ppu_register_set, i);
		PaintTFT_cell4();
	}
	gplib_ppu_fade_effect_set(ppu_register_set, 0xff);
}


int PointMove(INT16S x[],INT16S y[],int X0,int Y0,int stp)
{
	int i,j,m,n;
	
	if(((x[0]!=X0) || (y[0]!=Y0)) ){
		i=X0-x[0];
		j=Y0-y[0];
		if(i<0) i=-i,m=-1;
		else m=1;
		if(j<0) j=-j,n=-1;
		else n=1;
		if((i>=j)&&(i>stp)){
			x[0]+=stp*m;
			y[0]+=stp*j/i*n;
		}else if((i<j)&&(j>stp)){
			y[0]+=stp*n;
			x[0]+=stp*i/j*m;
		}else{
			x[0]=X0;
			y[0]=Y0;
		}
	}
	if( (x[0]==X0) && (y[0]==Y0) ) return 1;
	else return 0;
}

void setSpeech3(int m,int p0,int p1,int p2)
{
	SPU_StopChannel(14);
	SpeechBuff[0]=m;
	SpeechBuff[1]=p0;
	SpeechBuff[3]=p1;
	SpeechBuff[5]=p2;
}
//================================================================================
//====================================================================================================
// Text palette from original palette data fadeout to while
// Fade_Out_White(void);
// Input parameters: none 	
// Return:     none
//====================================================================================================
#define P_PPU_PALETTE_RAM0_N	0xD0501000

void Fade_Out_White(void)
{
	int i,j;
	INT16U data,RedColor,GreenColor,BlueColor;
	int check;
	INT32U *Palette;
	
	do{
		check = 0;
		
		Palette=(INT32U *)P_PPU_PALETTE_RAM0_N;
		   
		for( i = 0 ; i < 256*4 ; i++ )
		{
			data 			= *Palette;
			BlueColor 		= data&0x1f;
			if( BlueColor!= 0x1f ){
				BlueColor++;
				check = 1;
			}
			GreenColor 		= ( data>>5 )& 0x1f;
			if( GreenColor!= 0x1f ){
				GreenColor++;
				check = 1;
			}
			RedColor 		= ( data>>10 )& 0x1f;
			if( RedColor!= 0x1f ){
				RedColor++;
				check = 1;
			}
			*Palette++ 	= (data&0x8000) + BlueColor + (GreenColor<<5) + (RedColor<<10);
		}
		gplib_ppu_go_and_wait_done(ppu_register_set);
		
		
	}while (check == 1);
}
//====================================================================================================
// Text palette from while fadein to select palette data
// Fade_In_White();

// Return:     none
//====================================================================================================
void Fade_In_White(void)
{
	int i,j;
	INT16U data,DiData;
	INT16U RedColor,GreenColor,BlueColor;
	INT16U DiRedColor,DiGreenColor,DiBlueColor;
	int check;
	INT16U *PaletteBuff;
	INT32U *Palette;
	
	
	do{
		check = 0;
		Palette=(INT32U *)P_PPU_PALETTE_RAM0_N;
		
		for(j=0;j<4;j++){
		    if(j==0){
		    	PaletteBuff=PaletteBuff0;
		    }else if(j==1){
		    	PaletteBuff=PaletteBuff1;
		    }else if(j==2){
		    	PaletteBuff=PaletteBuff2;
		    }else{
		    	PaletteBuff=PaletteBuff3;
		    }
		    		
			for( i = 0 ; i < 256 ; i++ )
			{
				DiData 			= PaletteBuff[i];				
				DiBlueColor 	= DiData & 0x1f;
				DiGreenColor 	= ( DiData>>5 )& 0x1f;
				DiRedColor 		= ( DiData>>10 )& 0x1f;
				
				data 			= *Palette;
				BlueColor 		= data&0x1f;
				if( BlueColor!= DiBlueColor )
				{
					BlueColor--;
					check = 1;
				}
				GreenColor 		= ( data>>5 )& 0x1f;
				if( GreenColor!= DiGreenColor )
				{
					GreenColor--;
					check = 1;
				}
				RedColor 		= ( data>>10 )& 0x1f;
				if( RedColor!= DiRedColor )
				{
					RedColor--;
					check = 1;
				}
				*Palette++ 	= (DiData&0x8000) + BlueColor + (GreenColor<<5) + (RedColor<<10);
			}		
		}
		gplib_ppu_go_and_wait_done(ppu_register_set);
	}while (check == 1);
}

//=====================================================================
#define Char_buff	0x1000
int LoadData(int game,int* DataAddr,char* DataName[])
{
	INT16S test_handle_1;
	INT32S nRet;
	struct stat_t   file_stat;
	INT8U *cha,*addr;
	int i,j=0,flag=0;
	int len=0;

    AddRootPath(DataName[game]);

	test_handle_1 = open((char *)rootPath, O_RDONLY);
	
	if(test_handle_1 ==-1){
		while(1);
	}
	
	nRet = fstat(test_handle_1, &file_stat); //get move.bin size
	if(DataAddr[2*game+1]==0){
		nRet = read(test_handle_1,DataAddr[2*game],file_stat.st_size);	
		nRet = close(test_handle_1);
	}else{
		cha=gp_malloc(Char_buff);
		addr=(INT8U *)DataAddr[2*game];

		while(len<file_stat.st_size){
			nRet = read(test_handle_1,(INT32U)cha,Char_buff);
			for(i=0;i<Char_buff;i++){
				if(flag==0){
					if(cha[i]==0x80){
						nRet = close(test_handle_1);
						gp_free(cha);
						return 0;
					}else if(cha[i]>0x80){
						flag=2;
						j=cha[i]-0x80;
					}else{
						flag=1;
						j=cha[i];
					}
				}else if(flag==1){
					*addr=cha[i];
					addr++;
					j--;
					if(j==0) flag=0;
				}else if(flag==2){
					while(j){
						*addr=cha[i];
						addr++;
						j--;
					}
					flag=0;
				}
			}
			len+=Char_buff;
		}
	}
	return 0;
	
}

int LoadData_1(int compress,INT32U DataAddr,char* DataName)
{
	INT16S test_handle_1;
	INT32S nRet;
	struct stat_t   file_stat;
	INT8U *cha,*addr;
	int i,j=0,flag=0;
	int len=0,size;
	INT16U *data,_data,*addr16;

	AddRootPath(DataName);
	
	test_handle_1 = open(rootPath, O_RDONLY);
	
	if(test_handle_1 ==-1){
		while(1);
	}
	nRet = fstat(test_handle_1, &file_stat); //get move.bin size
	switch(compress)
	{
		case 0:
			nRet = read(test_handle_1,DataAddr,file_stat.st_size);	
			nRet = close(test_handle_1);
		break;
		case 1:
			cha=gp_malloc(Char_buff);
			addr=(INT8U *)DataAddr;

			while(len<file_stat.st_size){
				nRet = read(test_handle_1,(INT32U)cha,Char_buff);
				for(i=0;i<Char_buff;i++){
					if(flag==0){
						if(cha[i]==0x80){
							nRet = close(test_handle_1);
							gp_free(cha);
							return 0;
						}else if(cha[i]>0x80){
							flag=2;
							j=cha[i]-0x80;
						}else{
							flag=1;
							j=cha[i];
						}
					}else if(flag==1){
						*addr=cha[i];
						addr++;
						j--;
						if(j==0) flag=0;
					}else if(flag==2){
						while(j){
							*addr=cha[i];
							addr++;
							j--;
						}
						flag=0;
					}
				}
				len+=Char_buff;
			}
		break;
		case 2: //BMP文件解压 RGB1555
			data=gp_malloc(Char_buff);
			addr16=(INT16U *)DataAddr;
			len=0;
			
			size= file_stat.st_size/2;
			while(1)
			{
				nRet = read(test_handle_1,(INT32U)data,Char_buff);

				for(i=0;i<Char_buff;i++,len++){
					
					if(len==size){
						nRet = close(test_handle_1);
						gp_free(data);
						return 0;
					}else if(data[i]>0x8000){
						
						j=data[i]-0x8000-1;
						while(j)
						{
							*addr16++=_data;
							j--;
						}
						
					}else{
						_data=data[i];
						*addr16++=_data;
					}
					
				}

			}
		break;
	}

	return 0;
}


void LoadData_N(int str,int num,int* DataAddr,char* DataName[])
{
	int i;

//	for(i=0;i<num;i++) LoadData(str+i,DataAddr,DataName);
	for(i=0;i<num;i++) LoadData_1(DataAddr[(str+i)*2+1],DataAddr[(str+i)*2],DataName[str+i]);
}

int LoadDRM(int game,int num,INT32U addr,int* DataAddr,char* DataName[])
{
	INT16S test_handle_1;
	INT32S nRet;
	struct stat_t   file_stat;
	int i;

	for(i=game;i<game+num;i++){
		AddRootPath(DataName[i]);
		
		test_handle_1 = open((char *)rootPath, O_RDONLY);
		if(test_handle_1 ==-1){
			while(1);
		}
		nRet = fstat(test_handle_1, &file_stat); //get move.bin size
		if(addr!=0){
			DataAddr[i]=addr-file_stat.st_size;
			addr=0;
		}else{
			DataAddr[i]=DataAddr[i-1]-file_stat.st_size;
		}
		nRet = read(test_handle_1,DataAddr[i],file_stat.st_size);	
		nRet = close(test_handle_1);
	}
//	SPU_resume_channel(MUSIC_CHCHANL);
	return 0;
}

int LoadDRM_1(char* path,INT32U addr,INT16S drm_n)
{
	INT16S test_handle_1;
	INT32S nRet;
	struct stat_t   file_stat;
	
	AddRootPath(path);
	{		
		test_handle_1 = open((char *)rootPath, O_RDONLY);
		if(test_handle_1 ==-1){
			while(1);
		}
		nRet = fstat(test_handle_1, &file_stat); //get move.bin size
		DrmAddr[drm_n]=addr-file_stat.st_size;
		nRet = read(test_handle_1,DrmAddr[drm_n],file_stat.st_size);	
		nRet = close(test_handle_1);
	}
	return 0;
}
//=============================================================
void Act_LoadOs()
{
	INT16S nRet,fd;
	stopSpu();
	gplib_ppu_fade_effect_set(ppu_register_set, 0xff);
	gpio_write_io(T430_RESET_PIN, DATA_LOW);
	/*
	saveLog();
	AddRootPath("log.log");
	fd = open(rootPath, O_RDWR);
	if(fd>=0){
		lseek(fd, 0,SEEK_END);
		
		nRet = write(fd,(INT32U)"a\r\n",3);
		nRet = close(fd);
		if(DISKUSED==FS_NAND1){
		   DrvNand_flush_allblk();
	    }	
	}//*/
	
	AddRootPath("iwatch\\mov.rom");
	nRet = _findfirst(rootPath, &file_info, D_ALL); //get move.bin size
	fd = open(rootPath,O_RDONLY);
	if(fd!=-1){
		nRet = read(fd,0xF00000,file_info.f_size);
		nRet = close(fd);
		
		*(INT16U *)0xF8000100=0xA5;
		*(INT8U *)0xF8000102=2;  
		*(INT8U *)0xF8000103=volum;	//Vol	
		*(INT8U *)0xF8000104=0;	
				
		if(DISKUSED==FS_NAND1) DrvNand_flush_allblk();
		_deviceunmount(DISKUSED);
		ReMap(0xF00000);//jump to SDRAM addres 0xF80000
	}
}

void Act_LoadGameEbook(INT16S type,INT16S no)
{
	INT16S nRet,fd;

	stopSpu();
	gplib_ppu_fade_effect_set(ppu_register_set, 0xff);
	gpio_write_io(T430_RESET_PIN, DATA_LOW);

	AddRootPath("iwatch\\mov.rom");
	nRet = _findfirst(rootPath, &file_info, D_ALL); //get move.bin size
	fd = open(rootPath, O_RDONLY);
	if(fd!=-1){
		nRet = read(fd,0xF00000,file_info.f_size);
		nRet = close(fd);
		*(INT16U *)0xF8000100=no;
		*(INT8U *)0xF8000102=type;	//0ebook,1-game
		*(INT8U *)0xF8000103=volum;	//Vol
		*(INT8U *)0xF8000104=0xa5;	//sleepKey

		if(DISKUSED==FS_NAND1) DrvNand_flush_allblk();
		_deviceunmount(DISKUSED);
		ReMap(0xF00000); //jump to SDRAM addres 0xF80000
	}
}

//*(INT8U *)0xF8000104=2;

