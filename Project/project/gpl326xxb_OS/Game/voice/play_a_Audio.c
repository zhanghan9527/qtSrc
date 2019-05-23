#include <stdio.h>
#include <string.h>

#include "application.h"				//G+ 
#include "drv_l2_ad_key_scan.h"			//G+ 
#include "gplib.h"						//G+ 

#include "Sprite_demo.h"				//公用
#include "data.h"						//公用
#include "photo_buttton.h"

extern const INT16U Vol_SPRITE_Palette1[];
extern INT32U AUDIO_Nums;
extern struct f_info	file_info;
extern INT16S Video_Max,Video_No,Video_Num;
//======================================================================

//======================================================================
INT16S ACT_Audio_Decode(void)
{
	char    enc_file[24];
	char    enc_path[64];
	
	INT32S  enc_num = 0;
	INT32S  file_nums = 0;
	INT32U  enc_type = 1;	
	INT32U	cur_state = 0; // 0:encoding 1:decoding
	INT64U  disk_free;	
	
	AUDIO_ARGUMENT 	audio_argument;
	MEDIA_SOURCE	media_source;

	INT32S i,ret,m;
	INT16S codeT,codetime,codeflag=0;
	INT16S pause,end;
	
	media_source.type_ID.FileHandle=0;
	audio_argument.Main_Channel = 255;

//-----------------------------------------------------
	//Disk_Type=FS_SD;
	ret=Chdir_Root("music");
	if(ret < 0) {
		mkdir("music");
		chdir("music");
	}

	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	Palette_ram_cpy(1,0,Vol_SPRITE_Palette1+256*0,256);
	LoadData_1(0,SpriteCellData,"iWatch\\voice\\SpriteCell.bin");
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	clear_sprite(Spritebuf_number);
	LoadTextBmp(C_PPU_TEXT1,"iWatch\\voice\\bg02.gif");

	//audio_encode_entrance();
	audio_decode_entrance();	// Initialize and allocate the resources needed by audio decoder
	// --------------- Description of Keys' Function --------------- //
	DBG_PRINT("GPL32XXX AUDIO EECODE DEMO\r\n");
	DBG_PRINT("Media : SDC/NAND/USBH (default: SDC)\r\n");
	DBG_PRINT("Key 1 : encode type(A18/ WAV/ MS_ADPCM/ IMA_ADPCM/ MP3)\r\n");
	DBG_PRINT("Key 2 : start encoding (file_num ++)\r\n");
	DBG_PRINT("Key 3 : stop encoding/decoding\r\n");
	DBG_PRINT("Key 4 : reserved\r\n");
	DBG_PRINT("Key 5 : reserved\r\n");
	DBG_PRINT("Key 6 : play encoded file\r\n");
	DBG_PRINT("Key 7 : play previous encoded file\r\n");
	DBG_PRINT("Key 8 : stop encoding/decoding and exit audio encode/decode tasks\r\n");
	// ------------------------------------------------------------- //

	step=10;
	while(1)
	{
	
		if( (Goto_desktop()) ){
			ret=-1;
			step=100;
		}
		PaintTFT_cell4();
		switch(step)
		{
//-----------------------------------------------------------------	play Audio
			case 10:
				clear_sprite(Spritebuf_number);
				Video_Max=Get_NameList_100("music","*.wav");
				Video_No=0;
				//set_sprite_display_init(14,112,38,Rec_Sp_Frame[3]);	//>>
			    step=13;
			break;
			case 11:
				if(Video_Max){
					if(pause==0) {
						sleeptime=100;
						//set_sprite_display_init(15,49,50,Rec_Button_Frame[1]);
						codeT++;
						if(codeT>=25) {
							codeT=0;
							codetime++;
						}
					}else ;//set_sprite_display_init(15,49,50,Rec_Button_Frame[0]);
					if(end) codetime=0;
					Disp_Vol_Name(15,codetime,1);
				}else ClearSomeSp(15,25);

				audio_decode_volume(volum*63/10);
				m=TestReleaseState();
				if(m==1){
					if(pause==0 && TestTouch(28,100,30,86)){			//play OR pause  & resume
						pause=1;
						codeflag=11;
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, 12);
					}else if(pause && TestTouch(28,100,30,86)){			//play OR pause  & resume
						if(end==1){
							step=12;
						}else{
							pause=0;
							codeflag=11;
						}
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, 12);
					}else if(TestTouch(0,28,34,64)){		//<<
						if(Video_No>0) Video_No--;
						else Video_No=Video_Max-1;
						if(end) step=13;
						else step=12;
						step=13;
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, 12);
					}else if(TestTouch(128-28,128,34,64)){		//>>
						if(Video_No<Video_Max-1) Video_No++;
						else Video_No=0;
						if(end) step=13;
						else step=12;
						step=13;
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, 12);
					}else if(TestTouch(85,128,86,128)){		//del video
						codeflag=12;
						step=20;
						game_time=0;
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, 12);
					}
				}else if(m==2){
					if(Video_No<Video_Max-1) Video_No++;
					else Video_No=0;
					if(end) step=13;
					else step=12;
					step=13;
				}else if(m==3){
					if(Video_No>0) Video_No--;
					else Video_No=Video_Max-1;
					if(end) step=13;
					else step=12;
					step=13;
				}
			break;
			case 12:		//play
				codeflag=10;
				pause=end=0;
				codeT=codetime=0;
				step=11;
			break;
			case 13:		//stop
				codeflag=12;
				pause=end=1;
				codeT=codetime=0;
				step=11;
			break;
//------------------------------------------del
			case 20:
				ret=Del_One_All();
				if(ret==0) step=11;
				else if(ret==1){			//del one video
					if(media_source.type_ID.FileHandle>=0) close(media_source.type_ID.FileHandle);
					ret=Chdir_Root("music");
					Get_file_info(Video_No);
					ret=unlink((char*)file_info.f_name);
					Video_Max=Del_One_file(Video_No,Video_Max);
				    if(Disk_Type==FS_NAND1){
					   DrvNand_flush_allblk();
				    }
					if(Video_Max==0){
						step=11;
					}else{
						Video_No=Video_No%Video_Max;
						step=13;
					}
				}else if(ret==2){				//del all video
					for(i=0;i<Video_Max;i++){
						Get_file_info(i);
						unlink((char*)file_info.f_name);
					}
				    if(Disk_Type==FS_NAND1){
					   DrvNand_flush_allblk();
				    }
				    Video_Max=0;
					step=11;
				}else step=100;
			break;
//--------------------------------------------------------------------
		}

//=================================================================================
		if(codeflag==10){		//play audio
			// play previous encoded file
			audio_Decode_stop(audio_argument);
			Get_file_info(Video_No);
						
			audio_argument.Main_Channel = 1;	// Use DAC Channel A+B
			audio_argument.L_R_Channel = 3;		// Left + Right Channel
			audio_argument.mute = 0;
			audio_argument.volume = 63;			// volume level = 0~63
			
			cur_state = 1;
			DBG_PRINT("now playing ... %s\r\n", file_info.f_name);
			audio_decode_playback((char*)(file_info.f_name), &audio_argument);
			
		}else if(codeflag==11){		//pause & resume
			if(audio_decode_status(audio_argument)==AUDIO_CODEC_PROCESSING)
			{
				audio_decode_pause(audio_argument);
			}
			else if(audio_decode_status(audio_argument)==AUDIO_CODEC_PROCESS_PAUSED)
			{
				audio_decode_resume(audio_argument);
			}
		}else if(codeflag==12){		//stop
			// stop encoding/decoding
			audio_Decode_stop(audio_argument);
			pause=end=1;
		}else{
			if(audio_decode_status(audio_argument)==AUDIO_CODEC_PROCESS_END){
				end=1;
				pause=1;
			}
		}

		if(codeflag) codeflag=0;
		if(step==100){
			audio_Decode_stop(audio_argument);
			if(media_source.type_ID.FileHandle>=0) close(media_source.type_ID.FileHandle);
			break;
		}
	}

	//audio_encode_exit();
	audio_decode_exit();	// Release the resources needed by audio decoder	
	
	DBG_PRINT("exit audio encode demo !!!\r\n");
	return ret;
}

