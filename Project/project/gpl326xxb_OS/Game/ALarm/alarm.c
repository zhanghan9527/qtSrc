#include "application.h"
#include "Sprite_demo.h"
#include "Text_demo.h"
#include "data.h"

#include "M05Button.h"

//==============================================================================
// Constants
//==============================================================================
//====¿¨Í¨Ë÷Òý
#define ON_OFF			0
#define DOT				ON_OFF+1
#define SELECT			DOT+3
#define BAR				SELECT+1
#define AMPM			BAR+1
#define HOUR			AMPM+1
#define HOUR_NUM		HOUR+1
#define MINUTE			HOUR_NUM+2
#define MINUTE_NUM		MINUTE+1
#define MUSIC_NAME		MINUTE_NUM+2

//==============================================================================
// External Declarations
//=============================================================================
extern t_rtc Rtc_time,Rtc_alarm;

extern const 	INT16U ALIGN4 Alarm_SPRITE_Palette1[];
const char* M05Sfx_ResourceLocation[]={
	"Pororo\\M23\\sfx\\sound_alarm1.drm",
	"Pororo\\M23\\sfx\\sound_alarm2.drm",	
	"Pororo\\M23\\sfx\\sound_alarm3.drm",	
	"Pororo\\M23\\sfx\\sound_alarm4.drm",	
	"Pororo\\M23\\sfx\\sound_alarm5.drm",
	"Pororo\\M23\\sfx\\sound_alarm6.drm",
};

void Alarm_Test()
{
	//rtc_time_get(&Rtc_time);
	if(game_time%25==0) Get_Rtc_Data();	
	if(alarmFlag)
	{
		if(alarm_hour==Time_hour && alarm_minute==Time_min){
			if(alarm==0){
				alarm=1;
			}
		}else alarm=0;
	}
}


extern INT8S light;
void showAlarm()
{
	char m;
	int time,i;

 	spMalloc(100);

	LoadData_1(1,SpriteCellData,"Pororo\\M05\\SpriteCell_cmp.bin");
	LoadDRM(0,6,SFX_ADDR_GAME,(int*)&DrmAddr[0],M05Sfx_ResourceLocation);		
	Palette_ram_cpy(1,0, Alarm_SPRITE_Palette1+0,256);

	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
  	gplib_ppu_sprite_zoom_enable_set(ppu_register_set, 1);		// value:0=disable 1=enable
  	gplib_ppu_fade_effect_set(ppu_register_set, 0x0);	
	
 	clear_sprite(Spritebuf_number);
 	
	LoadTextBmp(C_PPU_TEXT1,"Pororo\\m05\\bg\\M5_2_layout.gif"); 	 	

	game_time=0;
	time=25*30;	
	m=alarm_hour%12;
	if(m==0) m=12;

	if(alarm_hour<12){
		set_sprite_display_init(AMPM, 99,146,M05BigNum_Frame[11]);
		set_sprite_display_init(HOUR_NUM+0, 81,176,M05BigNum_Frame[m/10]);		
		set_sprite_display_init(HOUR_NUM+1, 99,176,M05BigNum_Frame[m%10]);				
	}else{	
		set_sprite_display_init(AMPM, 99,146,M05BigNum_Frame[12]);
		set_sprite_display_init(HOUR_NUM+0, 81,176,M05BigNum_Frame[m/10]);		
		set_sprite_display_init(HOUR_NUM+1, 99,176,M05BigNum_Frame[m%10]);				
	}	

	set_sprite_display_init(MINUTE, 	114,176,M05BigNum_Frame[10]);		//:
	set_sprite_display_init(MINUTE_NUM+0, 128,176,M05BigNum_Frame[alarm_minute/10]);		
	set_sprite_display_init(MINUTE_NUM+1, 146,176,M05BigNum_Frame[alarm_minute%10]);	

	SetWaveIo(1);
//	set_led_light(LCD_light);
//	if(volum==0) SPU_Set_MainVolume(VOLUME_MAX*1/VOL_MAX_FAME);
	light=0;
	
	while(1)
	{
//		if(game_time%5==0){
//			m^=1;
//			set_sprite_display_init(ALARM_SP+1,21,37,alarm_Frame[m]);
//		}				
		if(SPU_Get_SingleChannel_Status(14)==0){
			PlaySpeech(alarm_music);
		}
		
		paint_ppu_spriteram_cell4(ppu_register_set,Sprite_Coordinate_240X320,Left_Top_coordinate_mode,Spritebuf_number);
		gplib_ppu_go_and_wait_done(ppu_register_set);		
		Paintdelay();
	
		game_time++;
		GetKeyBoardJoy();
//		TouchPanelService();
//		PowerOff();
		sleeptime=WAITINGTIME;
		
		if(TouchRelease && (TestTouch(76,76+89,263,263+37)||TestTouch(1,40,1,40) )){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);											
			break;
		}

		if(Key_States&KEY_HOME){
			break;
		}		
		
		time--;
		if(!time)break;
	}
	
	stopSpu();	
//	gp_free((void *)DrmAddr[ALARM_SFX]);
	alarmFlag=0;
	alarm=2;
	Act_SaveSetting();
//	if(volum==0) SPU_Set_MainVolume(VOLUME_MAX*0/VOL_MAX_FAME);
}






