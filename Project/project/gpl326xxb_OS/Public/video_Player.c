//================================================================================================
//	demo code function:
//	1. use sd and nand 
//  2. copy avi file from sd to nand 
//  3. play avi file from nand 
//================================================================================================
#include "application.h"
#include "drv_l2_key_scan.h"
#include "drv_l2_ad_key_scan.h"
#include "video_codec_callback.h"
#include "data.h"


#define C_AVI_NODE_SIZE			200
INT16U	avi_nand_node_buf[C_AVI_NODE_SIZE];

INT16S File_Max,File_No=0,File_Page=0;

struct sfn_info g_nand_fs_info;
struct STFileNodeInfo g_nand_node_info;
VIDEO_INFO video_information;


INT16S VIDEO_PROCESSING()
{
	INT16S i;
	if(video_decode_status() == VIDEO_CODEC_PROCESS_PAUSE){
		video_decode_resume();
		Paintdelay();
	}
	if(video_decode_status() == VIDEO_CODEC_PROCESSING) return 1;
	else return 0;
}


//===========================================================================
INT16S Get_video_Info_byname(char *name)
{
	INT16S ret=-1;
	VIDEO_CODEC_STATUS status;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;	

	arg.bScaler = 0x01;	//scaler output size or not
	arg.bUseDefBuf = FALSE;	//use user define buffer 
	//arg.AviDecodeBuf1 = avi_dec_buffer[0];
	//arg.AviDecodeBuf2 = avi_dec_buffer[1];

	arg.DisplayWidth = 240;		//display width
	arg.DisplayHeight = 320;	//display height
	arg.DisplayBufferWidth = 240;	//display buffer width
	arg.DisplayBufferHeight = 320;	//display buffer height

	arg.OutputFormat = DISPLAY_OUTPUT_FORMAT;	//set output format

	src.type_ID.FileHandle = open(name,O_RDONLY); //open file handle
	if(src.type_ID.FileHandle < 0)
	{
		close(src.type_ID.FileHandle);
		return -1;
	}	
	src.type = SOURCE_TYPE_FS;	//play file by file system
	src.Format.VideoFormat =MJPEG;//MJPEG;// MPEG4;

	if(1){
		status = video_decode_paser_header(&video_information, arg, src);
		if(status != VIDEO_CODEC_STATUS_OK){
			close(src.type_ID.FileHandle);
			return -1;
		}
		status = video_decode_start(arg, src);
		if(status != START_OK) return -1;
		ret=0;
	}else{
		status = video_decode_get_nth_video_frame(&video_information,arg, src,1);
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


INT16S VideoDecode_byname(char *name)
{ 
	int codeflag=0;
	INT8S end=0;
    INT8S pause=0;
    
	INT8S	reverse_play_flag;
	INT32S  ret, i;
	FP32    play_speed;
	VIDEO_CODEC_STATUS status;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;	
	
	INT16S vd_time,vd_cur;
	INT16S touch_no;
//---------------------------
	gplib_ppu_fade_effect_set(ppu_register_set, 0xff);
	
	reverse_play_flag = 0;
	play_speed = 1;
	user_defined_video_codec_entrance();
	video_decode_entrance();
	spMalloc(100);
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,3);//3=BMP_YUV
	dma_memory_fill(TVOutBuff,0,320*240*2);
	clear_sprite(Spritebuf_number);
	PaintTFT_in();
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	
	step=0;
	while(1)
	{
		Paintdelay();
		//KeyService();
		//if(Key_States&KEY_HOME){
			//step=100;
			//ret=-1;
		//}
		switch(step)
		{
//-------------------------------------
			case 0:			//init & play
				codeflag=1;
				step=1;
				pause=0;
				end=0;
			break;
			case 1:
			break;
		}
		if(step==100){
			if(video_decode_status() == VIDEO_CODEC_PROCESSING) video_decode_stop();
			break;
		}
//---------------------------------------------------------------------------
		if(codeflag==1)                // Start to decode the specified file 
		{
			if(video_decode_status() == VIDEO_CODEC_PROCESSING)
				video_decode_stop();

			ret=Get_video_Info_byname(name);
			if(ret==-1){
				codeflag=0;
				continue;
			}
			vd_time=video_information.TotalDuration;

			audio_decode_volume(63);
			reverse_play_flag = 0;
			play_speed = 1;
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
		}else if(codeflag==4){         // reserve play
			if(reverse_play_flag)
			{
				reverse_play_flag = 0;
				video_decode_set_reserve_play(0);
			}else{
				reverse_play_flag = 1;
				video_decode_set_reserve_play(1);
			}
		}else if(codeflag==5){         // set_play_time +5
			if(VIDEO_PROCESSING())
			{
				ret = video_decode_get_current_time();
				ret += 5;
				video_decode_set_play_time(ret);
				DBG_PRINT("Set Play Time = %dsec\r\n", ret);
			}
		}else if(codeflag==6){         // set_play_time -5
			if(VIDEO_PROCESSING())
			{			
				ret = video_decode_get_current_time();
				ret -= 5;
				video_decode_set_play_time(ret);
				DBG_PRINT("Set Play Time = %dsec\r\n", ret);
			}	
		}else if(codeflag==7){         // play_speed*10
			play_speed += 0.5;
			if(play_speed > 4)	play_speed = 0.5;
			ret = play_speed*10;
			DBG_PRINT("speed = %d.%d\r\n", ret/10, ret%10);	
			video_decode_set_play_speed(play_speed);
		}else{        				 // END
			if(video_decode_status()==VIDEO_CODEC_PROCESS_END){
				break;
			}
		}
		if(codeflag) codeflag=0;
	}
	video_decode_stop();
	video_decode_exit();
	ChangeCodePage(UNI_ENGLISH);
	InitFileSystem(DISKUSED);
	return 	ret;
}
//video_decode_paser_header



