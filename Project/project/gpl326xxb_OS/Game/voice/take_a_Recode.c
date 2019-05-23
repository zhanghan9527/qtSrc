#include <stdio.h>
#include <string.h>

#include "application.h"				//G+ 
#include "drv_l2_ad_key_scan.h"			//G+ 
#include "gplib.h"						//G+ 

#include "Sprite_demo.h"				//公用
#include "data.h"						//公用
#include "photo_buttton.h"
#include "photoedit.h"

extern INT32U AUDIO_Nums;
extern struct f_info	file_info;
extern const INT16U video_SPRITE_Palette1[];
extern INT16U Voice_Max;
INT16U Voice_No;
extern INT16S	Voice_Magic;
extern INT32S fd_SaveAS,Data_TmpDx;
//======================================================================
//======================================================================
void audio_decode_playback(char* filename, AUDIO_ARGUMENT* dec_arg)
{
	AUDIO_ARGUMENT 	temp;
	MEDIA_SOURCE	media_source_0;
	AUDIO_CODEC_STATUS 	audio_status_0;
	
	INT32U  dec_type = 0;
	
	INT32U  filename_len = strlen(filename);
		
	//init varaible
	gp_memcpy((INT8S*)&temp, (INT8S*)dec_arg, sizeof(AUDIO_ARGUMENT));
	media_source_0.type = SOURCE_TYPE_FS;
	audio_status_0 = AUDIO_CODEC_PROCESS_END;
	
	
	if ((filename[filename_len-4] == '.') && 
		(filename[filename_len-3] == 'a') && 
		(filename[filename_len-2] == '1') && 
		(filename[filename_len-1] == '8'))
	{
		dec_type = 0;
	}
	else if ((filename[filename_len-4] == '.') && 
		(filename[filename_len-3] == 'w') && 
		(filename[filename_len-2] == 'a') && 
		(filename[filename_len-1] == 'v'))
	{
		dec_type = 1;
	}
	else if ((filename[filename_len-4] == '.') && 
		(filename[filename_len-3] == 'm') && 
		(filename[filename_len-2] == 'p') && 
		(filename[filename_len-1] == '3'))
	{
		dec_type = 2;
	}
		
	switch(dec_type)
	{
		case 0:	// a18
			media_source_0.type_ID.FileHandle = open(filename, O_RDONLY);
			media_source_0.type = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE
			media_source_0.Format.AudioFormat = A1800;

			G_snd_info.Speed = 12;     //for speed control(golbal var)
			G_snd_info.Pitch = 12;     //for pitch control(golbal var)

			break;
		case 1:	// pcm, ima_adpcm, ms_adpcm
			media_source_0.type_ID.FileHandle = open(filename, O_RDONLY);
			media_source_0.type = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE
			media_source_0.Format.AudioFormat = MICROSOFT_ADPCM;//WAV;

			G_snd_info.Speed = 12;     //for speed control(golbal var)
			G_snd_info.Pitch = 12;     //for pitch control(golbal var)
			G_snd_info.VoiceChangerEn=1;
			G_snd_info.ConstPitchEn = 0;
			G_snd_info.EchoEn = 0;
			Voice_Magic=1;

			break;
			
		case 2:	// mp3
			media_source_0.type_ID.FileHandle = open(filename, O_RDONLY);
			media_source_0.type = SOURCE_TYPE_FS;
			media_source_0.Format.AudioFormat = MP3;
			
			temp.midi_index = 1;		// midi use

			G_snd_info.Speed = 12;					// for speed control(golbal var)
			G_snd_info.Pitch = 12;					// for pitch control(golbal var)

			break;
		default:
			break;
	}
	
	DBG_PRINT("now decoding %s ... \r\n", filename);
	audio_status_0 = audio_decode_start(temp, media_source_0);	
	if (audio_status_0 != 0)
		DBG_PRINT("audio_decode_error!!!\r\n");			
}

void audio_decode_Magic(char* filename, AUDIO_ARGUMENT* dec_arg)
{
	AUDIO_ARGUMENT 	temp;
	MEDIA_SOURCE	media_source_0;
	AUDIO_CODEC_STATUS 	audio_status_0;
	
	INT32U  dec_type = 0;
	INT16S  Vm=Voice_Magic&0xff;
	
	INT32U  filename_len = strlen(filename);
		
	//init varaible
	gp_memcpy((INT8S*)&temp, (INT8S*)dec_arg, sizeof(AUDIO_ARGUMENT));
	media_source_0.type = SOURCE_TYPE_FS;
	audio_status_0 = AUDIO_CODEC_PROCESS_END;
	
	dec_type = 1;
		
	media_source_0.type_ID.FileHandle = open(filename, O_RDONLY);
	media_source_0.type = SOURCE_TYPE_FS;//SOURCE_TYPE_USER_DEFINE
	media_source_0.Format.AudioFormat = WAV;

	G_snd_info.Speed = 12;     //for speed control(golbal var)
	G_snd_info.Pitch = 12;     //for pitch control(golbal var)
	G_snd_info.VoiceChangerEn=0;
	G_snd_info.ConstPitchEn = 0;
	G_snd_info.EchoEn = 0;
	Data_TmpDx=0;

	if(Vm==1){		//slow
		G_snd_info.Speed = 6;
		G_snd_info.Pitch = 12;
		G_snd_info.VoiceChangerEn=1;
	}else if(Vm==2){		//回音
		G_snd_info.delay_len = 5000;
		G_snd_info.weight_idx = 2;
		G_snd_info.EchoEn = 1;
	}else if(Vm==3){		//Low
		G_snd_info.Speed = 12;
		G_snd_info.Pitch = 16;
		G_snd_info.VoiceChangerEn=1;
	}else if(Vm==4){		//High
		G_snd_info.Speed = 12;
		G_snd_info.Pitch = 8;
		G_snd_info.VoiceChangerEn=1;
	}else if(Vm==5){		//robot 音
		G_snd_info.Pitch_idx = 3;
		G_snd_info.ConstPitchEn = 1;
	}
	
	DBG_PRINT("now decoding %s ... \r\n", filename);
	audio_status_0 = audio_decode_start(temp, media_source_0);	
	if (audio_status_0 != 0)
		DBG_PRINT("audio_decode_error!!!\r\n");
				
}


void audio_stop_processing2()
{

	if(audio_encode_status() == AUDIO_CODEC_PROCESSING)
	{
//		DBG_PRINT("audio_encode_stop..");
		audio_encode_stop();
	}
}

void audio_stop_processing(AUDIO_ARGUMENT* dec_arg)
{

	AUDIO_ARGUMENT temp;
	gp_memcpy((INT8S*)&temp, (INT8S*)dec_arg, sizeof(AUDIO_ARGUMENT));

	if (dec_arg->Main_Channel != 255)
	{
		if(audio_decode_status(temp) == AUDIO_CODEC_PROCESSING)
		{
			DBG_PRINT("audio_decode_stop..");
			audio_decode_stop(temp);
			DBG_PRINT("stop decoding!!!\r\n");
		}
		
		dec_arg->Main_Channel = 255;
	}

	if(audio_encode_status() == AUDIO_CODEC_PROCESSING)
	{
		DBG_PRINT("audio_encode_stop..");
		audio_encode_stop();
		if(Disk_Type == FS_NAND1){
			DrvNand_flush_allblk();
		}
		DBG_PRINT("stop encoding!!!\r\n");
	}
}

void audio_Decode_stop(AUDIO_ARGUMENT arg)
{
	if(audio_decode_status(arg) == AUDIO_CODEC_PROCESS_PAUSED)
	{
		audio_decode_resume(arg);
//			DBG_PRINT("resume decoding!!!\r\n");
	}
	if(audio_decode_status(arg) == AUDIO_CODEC_PROCESSING)
	{
//			DBG_PRINT("audio_decode_stop..");
		audio_decode_stop(arg);
//			DBG_PRINT("stop decoding!!!\r\n");
	}

}
void Disp_Vol_Name(INT16S sp,INT16S time,INT16S music)
{
	INT8U ch;
	/*
	set_sprite_display_init(sp+1,36,110,Rec_Sp_Frame[4]);
	if(music){
		ch=(Voice_No+1)/100%10;
		set_sprite_display_init(sp+2,64,111,Rec_num_Frame[ch]);
		ch=(Voice_No+1)%100/10;
		set_sprite_display_init(sp+3,74,111,Rec_num_Frame[ch]);
		ch=(Voice_No+1)%10;
		set_sprite_display_init(sp+4,84,111,Rec_num_Frame[ch]);
	}else{
		ch=file_info.f_name[0]-0x30;
		if(ch>9) ch=0;
		set_sprite_display_init(sp+2,64,111,Rec_num_Frame[ch]);
		ch=file_info.f_name[1]-0x30;
		if(ch>9) ch=0;
		set_sprite_display_init(sp+3,74,111,Rec_num_Frame[ch]);
		ch=file_info.f_name[2]-0x30;
		if(ch>9) ch=0;
		set_sprite_display_init(sp+4,84,111,Rec_num_Frame[ch]);
	}
	set_sprite_display_init(sp+5,40,8,Rec_num_Frame[time/60/10]);
	set_sprite_display_init(sp+6,50,8,Rec_num_Frame[time/60%10]);
	set_sprite_display_init(sp+7,60,8,Rec_num_Frame[10]);
	set_sprite_display_init(sp+8,70,8,Rec_num_Frame[time%60/10]);
	set_sprite_display_init(sp+9,80,8,Rec_num_Frame[time%60%10]);	
	//*/
}
void disp_RecTime(INT16S sp,INT16S time)
{
	INT8U ch;
	/*
	set_sprite_display_init(sp+1,18+50,110,Rec_Sp_Frame[4]);
	ch=(AUDIO_Nums-1)/100%10;
	set_sprite_display_init(sp+2,42+50,111,Rec_num_Frame[ch]);
	ch=(AUDIO_Nums-1)%100/10;
	set_sprite_display_init(sp+3,52+50,111,Rec_num_Frame[ch]);
	ch=(AUDIO_Nums-1)%10;
	set_sprite_display_init(sp+4,62+50,111,Rec_num_Frame[ch]);

	if(time>=6000) set_sprite_display_init(sp+10,28,8,Rec_num_Frame[time/6000%10]);
	else set_sprite_image_number(sp+10,0);
	set_sprite_display_init(sp+5,38,8,Rec_num_Frame[time%6000/60/10]);
	set_sprite_display_init(sp+6,48,8,Rec_num_Frame[time%6000/60%10]);
	set_sprite_display_init(sp+7,58,8,Rec_num_Frame[10]);
	set_sprite_display_init(sp+8,68,8,Rec_num_Frame[time%60/10]);
	set_sprite_display_init(sp+9,78,8,Rec_num_Frame[time%60%10]);	
	if(game_time/6%2==0) set_sprite_display_init(sp+0,0,0,0);
	else set_sprite_display_init(sp+0,88,10,Rec_num_Frame[11]);//*/
}

extern INT16S PLAY_Flag;
//======================================================================
INT16S ACT_Audio_Encode(INT16S act)
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

	spMalloc(1000);
//-----------------------------------------------------
	if(DISKUSED==FS_SD) Disk_Type=FS_SD;
	else Disk_Type=FS_NAND2;
	ret=Chdir_Root("voice");

	Palette_ram_cpy( 3,0, video_SPRITE_Palette1+256*0	,256);
	LoadData_1(1,SpriteCellData,"Pororo\\video\\SpriteCell_cmp.BIN");
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	gplib_ppu_fade_effect_set(ppu_register_set, 0);
	LoadTextBmp(C_PPU_TEXT1,"Pororo\\menu\\bg\\bg1.gif");
	clear_sprite(Spritebuf_number);

	audio_encode_entrance();
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

	gpio_write_io(T430_RESET_PIN, DATA_HIGH);
	//if(act) step=10;
	//else 
	step=10;
	while(1)
	{
	
		if( (Key_States&KEY_HOME) && (act==0) && (step>10) ){
			step=0;
		}else if( (Goto_desktop()) ){
			ret=-1;
			step=100;
		}
		PaintTFT_cell4();
		switch(step)
		{
			case 0:			//init
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,130,172,photo_buttton_Frame[7]);

			    step++;
			break;
			case 1:
				if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, 12);
					step++;
				}
			break;
			case 2:
				set_sprite_display_init(0,130,172,photo_buttton_Frame[6]);
				codeflag=2;
				codeT=codetime=0;
				//SetMICIo(1);
				step++;
			break;
			case 3:
				//set_sprite_display_init(0,0,0,Rec_wav_Frame[game_time/2%8]);
				//set_sprite_nCharNum(0,BUTTON_IDX*4);
				codeT++;
				if(codeT>=25) {
					codeT=0;
					codetime++;
					sleeptime=100;
				}
				disp_RecTime(10,codetime);
				if(KeyBoardCode==K_Ok){	//stop
					codeflag=3;
					step++;
				}
			break;
			case 4:
				//SetMICIo(0);
				WaitNoTouch();
				if(Disk_Type==FS_NAND1){
					DrvNand_flush_allblk();
				}
				step=10;
			break;
//-----------------------------------------------------------------	play voice
			case 10:
				clear_sprite(Spritebuf_number);
				//LoadTextBmp(C_PPU_TEXT1,"iWatch\\spy_menu\\recode.gif");
				get_photo_list(2,4);
				Voice_No=0;
			    if(Voice_Max==0) step=0;
			    else{
			    	Get_file_info(Voice_No,pFileList,Voice_Max);
			    	step=11;
			    }
			break;
			case 11:				
				if(Voice_Max){
					if(pause==0) {
						sleeptime=100;
							set_sprite_display_init(0,130,172,photo_buttton_Frame[6]);
						codeT++;
						if(codeT>=25) {
							codeT=0;
							codetime++;
						}
					}else{
						set_sprite_display_init(0,130,172,photo_buttton_Frame[3]);
					}
					if(end) codetime=0;
					Disp_Vol_Name(15,codetime,0);
				}
				//audio_decode_volume(volum*63/10);
				if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					codeflag=12;
					step=50;
				}else if(KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, 12);
					if(end==1){
						step=12;
					}else{
						if(pause==0){
							pause=1;
							codeflag=11;
						}else{
							pause=0;
							codeflag=11;
						}
					}
				}
				PLAY_Flag=pause^1;
			break;
			case 12:		//play
				codeflag=10;
				pause=end=0;
				codeT=codetime=0;
				step=11;
			break;
			case 13:		//stop
				Get_file_info(File_No,pFileList,File_Max);
				codeflag=12;
				pause=end=1;
				codeT=codetime=0;
				step=11;
			break;
//------------------------------------------no voice
			case 20:		//no file
				clear_sprite(Spritebuf_number);
			    //set_sprite_display_init(1,0,0,Common_Frame[6]);
			    set_sprite_nCharNum(1,BUTTON_IDX*4);
				step++;
			break;
			case 21:
				if(TouchRelease){
					ret=0;
					if(act) step=100;
					else step=0;
				}
			break;
//------------------------------------------del
 			case 50:
 				clear_sprite(Spritebuf_number);
 				step++;
			break;
			case 51:
				if(media_source.type_ID.FileHandle>=0) close(media_source.type_ID.FileHandle);				
				ret=Chdir_Root("voice");
				for(i=0;i<Voice_Max;i++){
					Get_file_info(i,pFileList,Voice_Max);
					ret=unlink((char*)file_info.f_name);
				}
			    if(Disk_Type==FS_NAND1){
				   DrvNand_flush_allblk();
			    }
			    step=0;
			    break;
				/*
				//Voice_Max=Del_One_file(Voice_No,pFileList,Voice_Max);

			    if(Disk_Type==FS_NAND1){
				   DrvNand_flush_allblk();
			    }
				if(Voice_Max==0){
					AUDIO_Nums=0;
					step=0;
				}else{
					Voice_No=Voice_No%Voice_Max;
					step=101;
				}//*/
			break;
//-------------------------------------------------------------------------
		}
		if(codeflag==1){      
			// encode type(A18/ WAV/ MS_ADPCM/ IMA_ADPCM/ MP3)
			enc_type++;
			if(enc_type > 4)
				enc_type = 0;
			
			switch(enc_type)
			{
				case 0:
					DBG_PRINT("encode type:A18\r\n");
					break;
				case 1:
					DBG_PRINT("encode type:WAV\r\n");
					break;
				case 2:
					DBG_PRINT("encode type:MS_ADPCM\r\n");
					break;
				case 3:
					DBG_PRINT("encode type:IMA_ADPCM\r\n");
					break;
				case 4:
					DBG_PRINT("encode type:MP3\r\n");
					break;
			}				
		}else if(codeflag==2){
			// start encoding (file_num ++)		
			if(audio_encode_status() == AUDIO_CODEC_PROCESS_END)
			{
				media_source.type = SOURCE_TYPE_FS; 
				//media_source.type = SOURCE_TYPE_USER_DEFINE;
				if(media_source.type_ID.FileHandle>=0) close(media_source.type_ID.FileHandle);

				disk_free=vfsFreeSpace(Disk_Type);
				if(disk_free>0x500000){
					while(1){
						sprintf(enc_path,(const char *)"10_%05d.%s",  AUDIO_Nums, "wav");
						ret = _findfirst(enc_path,&file_info,D_ALL);
						if(ret<0){
							break;	
						}else {
							AUDIO_Nums++;
							if(AUDIO_Nums>999){
								AUDIO_Nums=1;														
							}
						}
					}
					AUDIO_Nums++;
					Act_SaveNums();
					Chdir_Root("voice");
					
					media_source.type_ID.FileHandle = open(enc_path, O_WRONLY|O_CREAT|O_TRUNC);
					media_source.Format.AudioFormat = MICROSOFT_ADPCM;	//WAV;
					//audio_encode_set_downsample(TRUE, 2);
					ret = audio_encode_start(media_source, 16000, 0);	//44100

					mic_set_pga_gain(1,8);
					//booster gain: 00: 0dB; 01: 6dB; 10: 12dB; 11: 18dB
					//pga gain : 00000: 37.5dB ~ 11111: - infinite ( mute ) 

				}
				
				if (ret != 0)
				{
					DBG_PRINT("encode %s start error... \r\n", enc_path);
					continue;
				}
				
				cur_state = 0;
				DBG_PRINT("now encoding %s ... \r\n", enc_path);
					
			}			
		}else if(codeflag==3){
			// stop encoding/decoding
			//audio_stop_processing2();
			audio_stop_processing(&audio_argument);
		}else if(codeflag==4){		//pause & resume
//=================================================================================
		}else if(codeflag==10){		//play audio
			// play previous encoded file
			audio_Decode_stop(audio_argument);
			if(media_source.type_ID.FileHandle>=0) close(media_source.type_ID.FileHandle);
						
			audio_argument.Main_Channel = 1;	// Use DAC Channel A+B
			audio_argument.L_R_Channel = 3;		// Left + Right Channel
			audio_argument.mute = 0;
			audio_argument.volume = 63;			// volume level = 0~63

			cur_state = 1;
			DBG_PRINT("now playing ... %s\r\n", file_info.f_name);
			audio_decode_playback((char*)(file_info.f_name), &audio_argument);
		}else if(codeflag==13 || codeflag==14){		//play audio  use Magic
			// play previous encoded file
			audio_Decode_stop(audio_argument);
			if(media_source.type_ID.FileHandle>=0) close(media_source.type_ID.FileHandle);

			audio_argument.Main_Channel = 1;	// Use DAC Channel A+B
			audio_argument.L_R_Channel = 3;		// Left + Right Channel
			audio_argument.mute = 0;
			if(codeflag==14) audio_argument.volume = 0;
			else audio_argument.volume = 63;			// volume level = 0~63
				
			cur_state = 1;
			DBG_PRINT("now playing ... %s\r\n", file_info.f_name);
			audio_decode_Magic((char*)(file_info.f_name), &audio_argument);

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
			if(media_source.type_ID.FileHandle>=0) close(media_source.type_ID.FileHandle);
			pause=end=1;
		}else{
			if(audio_decode_status(audio_argument)==AUDIO_CODEC_PROCESS_END){
				end=1;
				pause=1;
			}
		}

		if(codeflag) codeflag=0;
		if(step==100){
			//audio_stop_processing2();
			audio_stop_processing(&audio_argument);
			audio_Decode_stop(audio_argument);
			if(media_source.type_ID.FileHandle>=0) close(media_source.type_ID.FileHandle);
			break;
		}
	}

	audio_encode_exit();
	audio_decode_exit();	// Release the resources needed by audio decoder	
	InitFileSystem(DISKUSED);
	
	DBG_PRINT("exit audio encode demo !!!\r\n");
	//led_pwm_init();
	PLAY_Flag=0;
	return ret;
}

