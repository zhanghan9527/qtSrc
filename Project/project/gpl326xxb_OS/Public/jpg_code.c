#include "application.h"
#include "data.h"
#include "math.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "ap_jpeg.h"
#include "photoedit.h"


struct f_info	file_info;

extern INT32U AUDIO_Nums;
extern INT16S File_Max,File_No,File_Page;

extern INT16U MailNumOutbox;

INT16S	Voice_Magic=0;
INT16S Data_Tmp[10];	//[0x100000];
INT32S Data_TmpDx=0;
INT32S fd_SaveAS;

extern INT16S D_year,D_mon,D_date;
//=====================================================================================

const INT8U _jpeg_422_q50_header_block_rw[624] = {
	0xFF, 0xD8, 0xFF, 0xFE, 0x00, 0x0B, 0x47, 0x50, 0x45, 0x6E, 0x63, 0x6F, 0x64, 0x65, 0x72, 0xFF, 
#if 1		// Quality 50
	0xDB, 0x00, 0x43, 0x00, 0x10, 0x0B, 0x0C, 0x0E, 0x0C, 0x0A,	0x10, 0x0E, 0x0D, 0x0E, 0x12, 0x11, 
	0x10, 0x13, 0x18, 0x28, 0x1A, 0x18, 0x16, 0x16, 0x18, 0x31, 0x23, 0x25, 0x1D, 0x28, 0x3A, 0x33, 
	0x3D, 0x3C, 0x39, 0x33, 0x38, 0x37, 0x40, 0x48, 0x5C, 0x4E, 0x40, 0x44, 0x57, 0x45, 0x37, 0x38, 
	0x50, 0x6D, 0x51, 0x57, 0x5F, 0x62, 0x67, 0x68, 0x67, 0x3E, 0x4D, 0x71, 0x79, 0x70, 0x64, 0x78, 
	0x5C, 0x65, 0x67, 0x63, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x11,	0x12, 0x12, 0x18, 0x15, 0x18, 0x2F, 
	0x1A, 0x1A, 0x2F, 0x63, 0x42, 0x38, 0x42, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x00, 0xF0, 
#else		// Quality 100
	0xDB, 0x00, 0x43, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x00, 0xF0, 
#endif

	0x01, 0x40, 0x03, 0x01, 0x21, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xFF, 0xC4, 0x00, 0x1F, 
	0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 
	0xB5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 
	0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 
	0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 
	0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 
	0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 
	0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 
	0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 
	0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 
	0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 
	0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 
	0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 
	0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xC4, 0x00, 0x1F, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
	0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 
	0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 
	0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 
	0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 
	0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 
	0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 
	0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 
	0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 
	0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 
	0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 
	0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 
	0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xDD, 0x00, 0x04, 
	0x00, 0x00, 0xFF, 0xDA, 0x00, 0x0C, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00
};

//==================================================================================================

void Act_SaveNums()
{
	INT16S fd,nRet;

	AddRootPath("Pororo\\user\\Nums.ini");
	fd = open(rootPath, O_CREAT|O_RDWR);
	nRet = write(fd,(INT32U)&PIC_Nums,4);
	nRet = write(fd,(INT32U)&AUDIO_Nums,4);
	nRet = write(fd,(INT32U)&VID_Nums,4);
	nRet = write(fd,(INT32U)&PicSd_Nums,4);
	nRet = write(fd,(INT32U)&VidSd_Nums,4);
	nRet = close(fd);
    //if(DISKUSED==FS_NAND1){
	   DrvNand_flush_allblk();
    //}
}
void Act_LoadNums()
{
	INT16S fd,nRet;

	AddRootPath("Pororo\\user\\Nums.ini");
	fd = open(rootPath, O_RDONLY);
	if(fd ==-1){
		Act_SaveNums();
	}else{
		nRet = read(fd,(INT32U)&PIC_Nums,4);
		nRet = read(fd,(INT32U)&AUDIO_Nums,4);
		nRet = read(fd,(INT32U)&VID_Nums,4);
		nRet = read(fd,(INT32U)&PicSd_Nums,4);
		nRet = read(fd,(INT32U)&VidSd_Nums,4);
		nRet = close(fd);
	}
}

INT32S CreatSaveAs()
{
	INT32S fd,ret;
	char    enc_path[64];
	INT8U	buff_tmp[0x2c];

	Get_file_info(File_No);
	fd=open((CHAR *)file_info.f_name,O_RDONLY);
	if(fd<0) return -1;
	read(fd,(INT32U)buff_tmp,0x2c);
	close(fd);

	while(1){
		sprintf(enc_path,(const char *)"%03d.%s",  AUDIO_Nums, "wav");
		ret = _findfirst(enc_path,&file_info,D_ALL);
		if(ret<0){
			break;	
		}else{
			AUDIO_Nums++;
			if(AUDIO_Nums>999){
				AUDIO_Nums=1;														
			}
		}
	}
	AUDIO_Nums++;
	Act_SaveNums();
	fd=open((CHAR *) enc_path, O_CREAT|O_RDWR);	
	write(fd,(INT32U)buff_tmp,0x2c);
	
	return fd;
}
INT32S SaveSaveAS(INT32S pcm_point, INT32U tmp_fp)
{
	int ret=0;
	int size;

	if(Voice_Magic&0x100){
		if(pcm_point<=0) size=WAV_DEC_FRAMESIZE*2;
		else size=pcm_point*2;
		ret=write(fd_SaveAS,(INT32U)tmp_fp,size);
		Data_TmpDx+=size;
		if(pcm_point<=0){
			lseek(fd_SaveAS,0x28,SEEK_SET);
			write(fd_SaveAS,(INT32U)&Data_TmpDx,4);
			ret=close(fd_SaveAS);
		}
	}
	return ret;
}
INT32S SaveSaveAS2(INT32S pcm_point, INT32U addr)
{
	int i;
	INT16S* tmp_fp;

	if(Voice_Magic){
		tmp_fp=(INT16S*)addr;
		if(pcm_point<=0) pcm_point=WAV_DEC_FRAMESIZE;
		for(i=0;i<pcm_point;i++){
			Data_Tmp[Data_TmpDx+i]=*(tmp_fp+i);
		}
		Data_TmpDx+=pcm_point;
	}
}


//============================M7 Mail===================================================
int Act_Save_jpeg_Mail(IMAGE_ENCODE_ARGUMENT encode_info,INT32U jpeg_encode_output_prt,INT32U jpeg_encode_size,INT32U DEVICE)
{
	  char path[100];
	  char pan[50];
	  INT16S fd;
	  struct f_info fin;
	  INT32S disk_size;
	  INT16S ret=-1;
	  INT8U jpeg_422_q50_header_block_rw[624]; 
	  int num;


	  DEVICE=Disk_Type;

      disk_size=vfsFreeSpace(DEVICE);
      
      if(DEVICE==FS_SD) sprintf((char *)pan,"C:\\mail\\");
      else if(DEVICE==FS_NAND1) sprintf((char *)pan,"A:\\mail\\");
      else if(DEVICE==FS_NAND2) sprintf((char *)pan,"B:\\mail\\");
      
	  if(disk_size > jpeg_encode_size){
		gp_memcpy((INT8S *)jpeg_422_q50_header_block_rw, (INT8S *)_jpeg_422_q50_header_block_rw, sizeof(jpeg_422_q50_header_block_rw));	  	  

    	while(1){
       		sprintf((char *)path, (const char *)"%soumail%0d.jpg",pan,MailNumOutbox+1);
			ret = _findfirst(path,&fin,D_ALL);
			if(ret<0){
				break;
			}else{
				MailNumOutbox++;
			}
    	}

		// Save encoded data to file	     
    	 fd = open((CHAR *) path, O_CREAT|O_RDWR);
	     if (fd >= 0) {
		    jpeg_422_q50_header_block_rw[0x9E] = encode_info.InputHeight >> 8;
			jpeg_422_q50_header_block_rw[0x9F] = encode_info.InputHeight & 0xFF;
			jpeg_422_q50_header_block_rw[0xA0] = encode_info.InputWidth >> 8;
			jpeg_422_q50_header_block_rw[0xA1] = encode_info.InputWidth & 0xFF;
			if (encode_info.OutputFormat == IMAGE_ENCODE_OUTPUT_FORMAT_YUV422) {
				jpeg_422_q50_header_block_rw[0xA4] =  0x21;
			} else {
				jpeg_422_q50_header_block_rw[0xA4] =  0x22;
			}
			write(fd, (INT32U) jpeg_422_q50_header_block_rw, sizeof(jpeg_422_q50_header_block_rw));
			write(fd, jpeg_encode_output_prt, jpeg_encode_size);
			close(fd);
			ret=0;
	     }
	     Act_SaveNums();
	  }
	return ret;
}
//==================================================================================================
int Act_Save_jpeg_file(IMAGE_ENCODE_ARGUMENT encode_info,INT32U jpeg_encode_output_prt,INT32U jpeg_encode_size,INT32U DEVICE)
{
	  char path[30];
	  char pan[20];
	  INT16S fd;
	  struct f_info fin;
	  INT32S disk_size;
	  INT16S ret=-1;
	  INT8U jpeg_422_q50_header_block_rw[624]; 
	  int num;


		DEVICE=Disk_Type;

      disk_size=vfsFreeSpace(DEVICE);
      
      if(DEVICE==FS_SD) sprintf((char *)pan,"C:\\mimicard_photo\\");
      else if(DEVICE==FS_NAND1) sprintf((char *)pan,"A:\\M2_photo\\");
      else if(DEVICE==FS_NAND2) {
      		if(OS_game==17) sprintf((char *)pan,"B:\\Mimimail\\");
      		else sprintf((char *)pan,"B:\\M2_photo\\");
      }
      
	  if(disk_size > jpeg_encode_size){
		gp_memcpy((INT8S *)jpeg_422_q50_header_block_rw, (INT8S *)_jpeg_422_q50_header_block_rw, sizeof(jpeg_422_q50_header_block_rw));	  	  

    	while(1){
    		if(DEVICE==FS_SD){
	       		sprintf((char *)path, (const char *)"%sphoto_%04d.jpg",pan,PicSd_Nums);
	       	}else if(OS_game==17){
	       		sprintf((char *)path, (const char *)"%sMM_%04d.jpg",pan,PIC_Nums);
	       	}else{
	       		sprintf((char *)path, (const char *)"%sM2_%04d.jpg",pan,PIC_Nums);
	       	}
			ret = _findfirst(path,&fin,D_ALL);
			if(ret<0){
				break;
			}else{
				if(DEVICE==FS_SD) PicSd_Nums++;
				else PIC_Nums++;
			}
    	}

		// Save encoded data to file	     
    	 fd = open((CHAR *) path, O_CREAT|O_RDWR);
	     if (fd >= 0) {
		    jpeg_422_q50_header_block_rw[0x9E] = encode_info.InputHeight >> 8;
			jpeg_422_q50_header_block_rw[0x9F] = encode_info.InputHeight & 0xFF;
			jpeg_422_q50_header_block_rw[0xA0] = encode_info.InputWidth >> 8;
			jpeg_422_q50_header_block_rw[0xA1] = encode_info.InputWidth & 0xFF;
			if (encode_info.OutputFormat == IMAGE_ENCODE_OUTPUT_FORMAT_YUV422) {
				jpeg_422_q50_header_block_rw[0xA4] =  0x21;
			} else {
				jpeg_422_q50_header_block_rw[0xA4] =  0x22;
			}
			write(fd, (INT32U) jpeg_422_q50_header_block_rw, sizeof(jpeg_422_q50_header_block_rw));
			write(fd, jpeg_encode_output_prt, jpeg_encode_size);
			close(fd);
			ret=0;
	     }
		 if(DEVICE==FS_SD) PicSd_Nums++;
		 else PIC_Nums++;
		 //File_SaveMax++;
	     Act_SaveNums();
	  }
	return ret;
}



IMAGE_DECODE_STRUCT img_decode;
extern INT8U *pjpeg_input_buf;
extern INT8U *pjpeg_working_memory;

void Init_jpeg_decode(INT32U OutPutbuff)
{
	img_decode.cmd_id=0;
	
	img_decode.source_size=0x100000;		// File size or buffer size
	img_decode.source_type=TK_IMAGE_SOURCE_TYPE_FILE;
	img_decode.decode_status=NULL;
	img_decode.output_ratio=1;
	img_decode.output_format=SCALER_CTRL_OUT_RGB565;
	img_decode.output_buffer_width=PPU_TEXT_SIZE_HPIXEL4;
	img_decode.output_buffer_height=PPU_TEXT_SIZE_VPIXEL4;
	img_decode.output_image_width=PPU_TEXT_SIZE_HPIXEL4;
	img_decode.output_image_height=PPU_TEXT_SIZE_VPIXEL4;
	img_decode.out_of_boundary_color=0;
	img_decode.output_buffer_pointer=OutPutbuff;

}

INT16S image_decode_to_buff1(char *SD_ID_JPG)
{
	INT16S ret;
	INT16S fd;

	fd=(INT32U)open(SD_ID_JPG, O_RDONLY);       //open jpg file
	if(fd==-1){		
		return -1;
	}
	
    img_decode.image_source=fd;  // File handle/resource handle/pointer
    
    if(image_decode_and_scale(&img_decode)==STATUS_OK){
    	ret=1;	
    }else ret=-1;
	
	close(fd);
 	
	 		
	return ret; 
}

extern INT32U BLOCK_ENCODE,decode_mode_ck;

INT16S image_decode_to_buff(char *path,INT32U OutPutBuff)
{
	INT32S i,j;
	IMAGE_ARGUMENT image_decode;
	MEDIA_SOURCE image_source;
	INT16S ret,decode_state;	
	IMAGE_INFO image_file_info;

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
	//image output infomation
	image_decode.ScalerOutputRatio=FIT_OUTPUT_SIZE;//FIT_OUTPUT_SIZE;                  //Fit to output_buffer_width and output_buffer_height for image output size
	image_decode.OutputFormat=IMAGE_OUTPUT_FORMAT_RGB565;//IMAGE_OUTPUT_FORMAT_YUYV; //                 //scaler out format
	image_decode.OutputBufPtr=(INT8U *)(OutPutBuff);            //decode output buffer
	image_decode.OutputBufWidth=PPU_TEXT_SIZE_HPIXEL4;                  //width of output buffer 
	image_decode.OutputBufHeight=PPU_TEXT_SIZE_VPIXEL4;                 //Heigh of output buffer
	image_decode.OutputWidth=PPU_TEXT_SIZE_HPIXEL4;                     //scaler width of output image
	image_decode.OutputHeight=PPU_TEXT_SIZE_VPIXEL4;                    //scaler Heigh of output image
	
	//image decode function
	image_decode_entrance();//global variable initial for image decode
	image_decode_start(image_decode,image_source);//image decode start	

	
 	while(1)
 	{
		decode_state=image_decode_status();
		if (decode_state==IMAGE_CODEC_DECODE_END){
			 ret=0;
			 break;
		}else if(decode_state==IMAGE_CODEC_DECODE_FAIL){
			 DBG_PRINT("image decode failed\r\n");			 
			 ret=-1;
			 break;
		}
	}
	close(image_source.type_ID.FileHandle);
	image_decode_stop();//image decode stop			 	
	 		
	return ret;
}
INT16S height_Two=0;
INT16S Photo_decode_info(char *SD_ID_JPG,INT16S width, INT16S height)
{
	MEDIA_SOURCE image_source;
	INT16S ret;	
	IMAGE_INFO image_file_info;

   //image decoder to used image callback function
    BLOCK_ENCODE=0;
    decode_mode_ck=0;
    
    disk_safe_exit(DISKUSED);
	//image source infomation
	image_source.type=SOURCE_TYPE_FS;                                    //image file infomation form file system
	{
		image_source.type_ID.FileHandle=(INT32U)open(SD_ID_JPG, O_RDONLY);       //open jpg file
		if(image_source.type_ID.FileHandle==-1){
			return -1;
		}
	}
	image_decode_Info(&image_file_info,image_source);
	if(image_file_info.Height==0 || image_file_info.Width==0) return -1;
	if(image_file_info.Width*height*2==image_file_info.Height*width){
		height_Two=height*2;
	}else{
		height_Two=height;
	}
	
	close(image_source.type_ID.FileHandle);
	return height_Two;
}
INT16S Photo_decode_to_buff(char *SD_ID_JPG, INT16S width, INT16S height,INT32U TmpBuff)
{
	INT32S i,j,n,nn,k;
	IMAGE_ARGUMENT image_decode;
	MEDIA_SOURCE image_source;
	INT16S ret,decode_state;	
	IMAGE_INFO image_file_info;
	INT16S *buff;

   //image decoder to used image callback function
    BLOCK_ENCODE=0;
    decode_mode_ck=0;
    
    disk_safe_exit(DISKUSED);
	//image source infomation
	image_source.type=SOURCE_TYPE_FS;                                    //image file infomation form file system
	{
		image_source.type_ID.FileHandle=(INT32U)open(SD_ID_JPG, O_RDONLY);       //open jpg file
		if(image_source.type_ID.FileHandle==-1){
			return -1;
		}		
	}
	image_decode_Info(&image_file_info,image_source);
	if(image_file_info.Height==0 || image_file_info.Width==0) return -1;
	if(image_file_info.Width*height*2==image_file_info.Height*width){
		height_Two=height*2;
	}else{
		height_Two=height;
	}
	//k=j*width*2+i*2/4*4;
	k=0;
	//for(i=0;i<j/2;i++) *((INT16U *)TmpBuff+i)=0x0;
	//image output infomation
	image_decode.ScalerOutputRatio=MAINTAIN_IMAGE_RATIO_2;//FIT_OUTPUT_SIZE;                  //Fit to output_buffer_width and output_buffer_height for image output size
	image_decode.OutputFormat=IMAGE_OUTPUT_FORMAT_RGB565;//IMAGE_OUTPUT_FORMAT_YUYV; //                 //scaler out format
	image_decode.OutputBufPtr=(INT8U *)(TmpBuff+k);            //decode output buffer
	image_decode.OutputBufWidth=320;                  //width of output buffer 
	image_decode.OutputBufHeight=480;                 //Heigh of output buffer
	image_decode.OutputWidth=width;                     //scaler width of output image
	image_decode.OutputHeight=height_Two;                    //scaler Heigh of output image
	
	//image decode function
	image_decode_entrance();//global variable initial for image decode
	image_decode_start(image_decode,image_source);//image decode start	

 	while(1)
 	{
		decode_state=image_decode_status();
		if (decode_state==IMAGE_CODEC_DECODE_END){
			 ret=0;
			 break;
		}else{ //if(decode_state==IMAGE_CODEC_DECODE_FAIL){
			 DBG_PRINT("image decode failed\r\n");			 
			 ret=-1;
			 break;
		}
	}
/*
	buff=(INT16S*)TmpBuff;
	if(i==0 && j){
		k=j+2;
		for(i=0;i<k;i++){
			n=i*width;
			nn=(height-1-i)*width;
			for(j=0;j<width;j++){
				*(buff+n+j)=0;
				*(buff+nn+j)=0;
			}
		}
		for(i=0;i<width;i++){
			*(buff+i)=0x000;
			*(buff+width*height-1+i)=0x000;
		}
	}else{
		k=i/4*4+2;
		for(i=0;i<k;i++){
			for(j=0;j<height;j++){
				*(buff+j*width+i)=0;
				*(buff+j*width+width-1-i)=0;
			}
		}
		for(i=0;i<height;i++){
			n=i*width;
			*(buff+n)=0x000;
			*(buff+n+width-1)=0x000;
		}
	}
//*/
	close(image_source.type_ID.FileHandle);
	image_decode_stop();//image decode stop			 	
	return ret;
}

INT16S Photo_decode_wh(char *SD_ID_JPG, INT16S width, INT16S height,INT16S Outwidth, INT16S Outheight,INT32U TmpBuff)
{
	INT32S i,j,n,nn,k;
	IMAGE_ARGUMENT image_decode;
	MEDIA_SOURCE image_source;
	INT16S ret,decode_state;	
	IMAGE_INFO image_file_info;
	INT16S *buff;

   //image decoder to used image callback function
    BLOCK_ENCODE=0;
    decode_mode_ck=0;
    
    disk_safe_exit(DISKUSED);
	//image source infomation
	image_source.type=SOURCE_TYPE_FS;                                    //image file infomation form file system
	{
		image_source.type_ID.FileHandle=(INT32U)open(SD_ID_JPG, O_RDONLY);       //open jpg file
		if(image_source.type_ID.FileHandle==-1){
			return -1;
		}		
	}
	image_decode_Info(&image_file_info,image_source);
	if(image_file_info.Height==0 || image_file_info.Width==0) return -1;
	if(image_file_info.Width*height>image_file_info.Height*width){
		i=0;
		j=(height-width*image_file_info.Height/image_file_info.Width)/2;
	}else{
		i=(width-height*image_file_info.Width/image_file_info.Height)/2;
		j=0;
	}
	//k=j*width*2+i*2/4*4;
	k=0;
	//for(i=0;i<j/2;i++) *((INT16U *)TmpBuff+i)=0x0;
	//image output infomation
	image_decode.ScalerOutputRatio=MAINTAIN_IMAGE_RATIO_2;//FIT_OUTPUT_SIZE;                  //Fit to output_buffer_width and output_buffer_height for image output size
	image_decode.OutputFormat=IMAGE_OUTPUT_FORMAT_RGB565;//IMAGE_OUTPUT_FORMAT_YUYV; //                 //scaler out format
	image_decode.OutputBufPtr=(INT8U *)(TmpBuff+k);            //decode output buffer
	image_decode.OutputBufWidth=Outwidth;                  //width of output buffer 
	image_decode.OutputBufHeight=Outheight;                 //Heigh of output buffer
	image_decode.OutputWidth=width;                     //scaler width of output image
	image_decode.OutputHeight=height;                    //scaler Heigh of output image
	
	//image decode function
	image_decode_entrance();//global variable initial for image decode
	image_decode_start(image_decode,image_source);//image decode start	

 	while(1)
 	{
		decode_state=image_decode_status();
		if (decode_state==IMAGE_CODEC_DECODE_END){
			 ret=0;
			 break;
		}else{ //if(decode_state==IMAGE_CODEC_DECODE_FAIL){
			 DBG_PRINT("image decode failed\r\n");			 
			 ret=-1;
			 break;
		}
	}
	close(image_source.type_ID.FileHandle);
	image_decode_stop();//image decode stop			 	
	return ret;
}


INT16S image_decode_to_RGB1555(char *path,INT32U OutPutBuff)
{
	IMAGE_ARGUMENT image_decode;
	MEDIA_SOURCE image_source;
	INT16S ret,decode_state;	
	IMAGE_INFO image_file_info;

   //image decoder to used image callback function
    BLOCK_ENCODE=0;
    decode_mode_ck=0;
    
	//image source infomation
	image_source.type=SOURCE_TYPE_FS;                                    //image file infomation form file system

	image_source.type_ID.FileHandle=(INT32U)open(path, O_RDONLY);       //open jpg file
	if(image_source.type_ID.FileHandle==-1){
		return -1;
	}

    
//	image_decode_Info(&image_file_info,image_source);

	//image output infomation
	image_decode.ScalerOutputRatio=MAINTAIN_IMAGE_RATIO_1;//FIT_OUTPUT_SIZE;//                  //Fit to output_buffer_width and output_buffer_height for image output size
	image_decode.OutputFormat=IMAGE_OUTPUT_FORMAT_RGB1555;//IMAGE_OUTPUT_FORMAT_YUYV; //                 //scaler out format
	image_decode.OutputBufPtr=(INT8U *)(OutPutBuff);            //decode output buffer
	image_decode.OutputBufWidth=PPU_TEXT_SIZE_HPIXEL4;                  //width of output buffer 
	image_decode.OutputBufHeight=PPU_TEXT_SIZE_VPIXEL4;                 //Heigh of output buffer
	image_decode.OutputWidth=PPU_TEXT_SIZE_HPIXEL4;                     //scaler width of output image
	image_decode.OutputHeight=PPU_TEXT_SIZE_VPIXEL4;                    //scaler Heigh of output image
    image_decode.OutBoundaryColor=(INT32U)0x008080; 				 //set the black for out of boundary color 
	
	//image decode function
	image_decode_entrance();//global variable initial for image decode
	image_decode_start(image_decode,image_source);//image decode start	
	
 	while(1)
 	{
		decode_state=image_decode_status();
		if (decode_state==IMAGE_CODEC_DECODE_END){
			 ret=0;
			 break;
		}else if(decode_state==IMAGE_CODEC_DECODE_FAIL){
			 DBG_PRINT("image decode failed\r\n");			 
			 ret=-1;
			 break;
		}
	}
	close(image_source.type_ID.FileHandle);
	image_decode_stop();//image decode stop			 	
	 		
	return ret;
}

INT16S image_decode_Sdram_to_buff(INT32U sdram_input_ptr,INT32U OutPutBuff)
{
	IMAGE_ARGUMENT image_decode;
	MEDIA_SOURCE image_source;
	INT16S ret,decode_state;	
	IMAGE_INFO image_file_info;
	INT8U *t_addr,*s_addr;
	
   	
	   //image source infomation
   image_source.type=SOURCE_TYPE_SDRAM;                             //image file infomation form sdram
   image_decode.OutputFormat=IMAGE_OUTPUT_FORMAT_RGB565;//IMAGE_OUTPUT_FORMAT_YUYV; //             //scaler out format
      

   //image decode function   
   image_source.type_ID.memptr=(INT8U *)sdram_input_ptr;            //open jpg file
   image_decode.OutputBufPtr=(INT8U *)OutPutBuff;             //decode output buffer
   image_decode.OutputBufWidth=PPU_TEXT_SIZE_HPIXEL4;                  //width of output buffer 
   image_decode.OutputBufHeight=PPU_TEXT_SIZE_VPIXEL4;                 //Heigh of output buffer
   image_decode.OutputWidth=PPU_TEXT_SIZE_HPIXEL4;                     //scaler width of output image
   image_decode.OutputHeight=PPU_TEXT_SIZE_VPIXEL4;                    //scaler Heigh of output image
   
   //image decode function
   image_decode_entrance();//global variable initial for image decode 
   image_decode_start(image_decode,image_source);//image decode start

   while (1) {
	  decode_state=image_decode_status();
      if (decode_state==IMAGE_CODEC_DECODE_END) {
         ret=0;
		 break;
	  }else if(decode_state==IMAGE_CODEC_DECODE_FAIL) {
	  	 ret=-1;
		 DBG_PRINT("image decode resource form SDRAM buffer failed\r\n");
		 break;
	  }	
   }		
   image_decode_stop();//image decode stop
	return ret;	
}

//image scale============================================================================
//image encode===========================================================================

IMAGE_SCALE_STRUCT img_scale;

void Init_image_scale(INT16U InputWidth,INT16U InputHeight,
					  INT16U OutputWidth,INT16U OutputHeight,
					  INT16U InputFormat,INT16U OutputFormat,
					  INT32U inputBuff1,INT32U outputBuff2					  
					  )
{
    img_scale.cmd_id=0;
    img_scale.status=0;
    img_scale.special_effect=0;
    img_scale.scaler_input_format=InputFormat;
    img_scale.scaler_output_format=OutputFormat;

    img_scale.input_width=InputWidth;
    img_scale.input_height=InputHeight;
    img_scale.input_visible_width=InputWidth;
    img_scale.input_visible_height=InputHeight;
    img_scale.input_offset_x=0;
    img_scale.input_offset_y=0;
    img_scale.input_buf1=NULL;
    
    img_scale.output_buffer_width=OutputWidth;
    img_scale.output_buffer_height=OutputHeight;
    img_scale.output_width_factor=65536*InputWidth/OutputWidth;
    img_scale.output_height_factor=65536*InputHeight/OutputHeight;
    img_scale.out_of_boundary_color=0;
    img_scale.output_buf1=(void*)outputBuff2;
    img_scale.input_buf1=(void*)inputBuff1;
}


void image_scale_to_buff()
{	
	image_scale(&img_scale);
}


void Init_image_scale_t(IMAGE_SCALE_STRUCT *img_scale_struct,
					  INT16U InputWidth,INT16U InputHeight,
					  INT16U OutputWidth,INT16U OutputHeight,
					  INT16U InputFormat,INT16U OutputFormat,
					  INT32U inputBuff1,INT32U outputBuff2					  
					  )
{
	
    img_scale_struct->cmd_id=0;
    img_scale_struct->status=0;
    img_scale_struct->special_effect=0;
    img_scale_struct->scaler_input_format=InputFormat;
    img_scale_struct->scaler_output_format=OutputFormat;

    img_scale_struct->input_width=InputWidth;
    img_scale_struct->input_height=InputHeight;
    img_scale_struct->input_visible_width=InputWidth;
    img_scale_struct->input_visible_height=InputHeight;
    img_scale_struct->input_offset_x=0;
    img_scale_struct->input_offset_y=0;
    img_scale_struct->input_buf1=NULL;
    
    img_scale_struct->output_buffer_width=OutputWidth;
    img_scale_struct->output_buffer_height=OutputHeight;
    img_scale_struct->output_width_factor=65536*InputWidth/OutputWidth;
    img_scale_struct->output_height_factor=65536*InputHeight/OutputHeight;
    img_scale_struct->out_of_boundary_color=0;
    img_scale_struct->output_buf1=(void*)outputBuff2;
    img_scale_struct->input_buf1=(void*)inputBuff1;
}


void image_change_To_format(INT32U Input_buff,INT32U Output_buff)
{
	IMAGE_SCALE_STRUCT	_img_scale;
	Init_image_scale_t(&_img_scale, 
					PPU_TEXT_SIZE_HPIXEL4,PPU_TEXT_SIZE_VPIXEL4,
					PPU_TEXT_SIZE_HPIXEL4,PPU_TEXT_SIZE_VPIXEL4,
					SCALER_CTRL_IN_RGB1555,SCALER_CTRL_OUT_YUYV, 
					Input_buff,Output_buff); 
					    					
	image_scale(&_img_scale);
}

int buff_to_save_jpgfile(INT32U InPutbuff,INT32U OutPutbuff,INT16U InputWidth,INT16U InputHeight)
{
	INT16S ret;
	IMAGE_ENCODE_ARGUMENT encode_info;
	INT32U encode_size;
	INT16S encode_state,scale_state;	
	    //image encode infomation
    encode_info.OutputBufPtr=(INT8U *)OutPutbuff;             //encode output buffer
    encode_info.BlockLength=0;                                       //encode block read length
    encode_info.EncodeMode=IMAGE_ENCODE_ONCE_READ;                   //encode mode setting
    encode_info.InputWidth=InputWidth;                        //width of input image
    encode_info.InputHeight=InputHeight;                       //Heigh of input image
    encode_info.quantizationquality=50;                              //encode quality
    encode_info.InputBufPtr.yaddr=(INT8U *)InPutbuff;         //encode input buffer
    encode_info.InputFormat=IMAGE_ENCODE_INPUT_FORMAT_YUYV;                 //encode input format
    encode_info.OutputFormat=IMAGE_ENCODE_OUTPUT_FORMAT_YUV422;             //encode input format
    //image encode function
    image_encode_entrance();//global variable initial for image encoder
    encode_size=image_encode_start(encode_info);//image encode start
    while (1) 
    {
         encode_state=image_encode_status();
         if (encode_state==IMAGE_CODEC_DECODE_END) {
             ret=Act_Save_jpeg_file(encode_info,OutPutbuff,encode_size,DISKUSED);	
             break;
         }else if(encode_state==IMAGE_CODEC_DECODE_FAIL) {
             DBG_PRINT("image decode failed\r\n");
			 ret=-2; //save lost             
             break;
         }	
    }
    image_encode_stop();//image encode stop 
   	return ret; 
    
}

//=======================================================================
void image_scale_YUV_320x480(INT32U Input_buff,INT32U Output_buff,INT16S inW,INT16S inH)
{
	IMAGE_SCALE_STRUCT	_img_scale;
	Init_image_scale_t(&_img_scale, 
					inW,inH,
					320,480,
					SCALER_CTRL_IN_RGB565,SCALER_CTRL_OUT_YUYV,		//SCALER_CTRL_OUT_YUYV
					Input_buff,Output_buff); 

	image_scale(&_img_scale);
}

void image_scale_YUV(INT32U Input_buff,INT32U Output_buff,INT16S inW,INT16S inH)
{
	IMAGE_SCALE_STRUCT	_img_scale;
	Init_image_scale_t(&_img_scale, 
					inW,inH,
					inW,inH,
					SCALER_CTRL_IN_RGB565,SCALER_CTRL_OUT_YUYV,		//SCALER_CTRL_OUT_YUYV
					Input_buff,Output_buff); 

	image_scale(&_img_scale);
}

void image_scale_RGB(INT32U Input_buff,INT32U Output_buff,INT16S inW,INT16S inH)
{
	IMAGE_SCALE_STRUCT	_img_scale;
	Init_image_scale_t(&_img_scale, 
					inW,inH,
					640,480,
					SCALER_CTRL_IN_YUYV,SCALER_CTRL_OUT_RGB565,		//SCALER_CTRL_OUT_YUYV
					Input_buff,Output_buff);
	image_scale(&_img_scale);
}

//=================================================================
