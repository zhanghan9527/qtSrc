#include "application.h"
#include "data.h"
#include "setting.h"
#include "math.h"
#include "photoedit.h"
#include "drv_l1_ppu.h"

PLAYER player;

const INT8U pwm_duty_cycle_percent[]=
{
	1,10,40,100
//	10,30,60,100
};
void led_pwm_Disable()
{
	timer_ccp_ctrl(TIMER_C, 0, 0, 0, 0);
}
void led_pwm_init(INT8U level) 
{ 
	if(level<0 && level>3) level = 3; 
	timer_pwm_setup(TIMER_C, 20000, pwm_duty_cycle_percent[level], 0); 
}

//=============================================
void Init_player()
{
	int i;
	INT8U *ck;
	
	ck=(INT8U*)&player;
	for(i=0;i<PLAYER_MAX;i++) *(ck+i)=0;

}

void Save_player()
{
	INT16S fd,nRet;

	AddRootPath("Pororo\\user\\player.ini");
	fd = open(rootPath, O_CREAT|O_RDWR);
	if(fd !=-1){
		nRet = write(fd,(INT32U)&player,PLAYER_MAX);
		nRet = close(fd);
	}
    if(DISKUSED==FS_NAND1){
	   DrvNand_flush_allblk();
    }
}
void Load_player()
{
	INT16S fd,nRet;

	AddRootPath("Pororo\\user\\player.ini");
	fd = open(rootPath, O_RDONLY);
	if(fd ==-1){
	}else{
		nRet = read(fd,(INT32U)&player,PLAYER_MAX);
		nRet = close(fd);
	}
}

extern INT16U Date_Start,auto_screen_off;
INT8S LCD_light=0;
void Act_SaveSetting()
{
	INT16S fd,nRet;

	AddRootPath("Pororo\\user\\setting.ini");
	fd = open(rootPath, O_CREAT|O_RDWR);
	if(fd !=-1){
		nRet = write(fd,(INT32U)&Date_Start,2);
		nRet = write(fd,(INT32U)&volum,1);
		nRet = write(fd,(INT32U)&LCD_light,1);
		nRet = write(fd,(INT32U)&alarmFlag,1);
		nRet = write(fd,(INT32U)&alarm_hour,1);
		nRet = write(fd,(INT32U)&alarm_minute,1);

		nRet = close(fd);
	}
    if(DISKUSED==FS_NAND1){
	   DrvNand_flush_allblk();
    }
}
extern INT8S InBoxMail[5];
void Init_Setting()
{
	int i;
	
	Date_Start=5000;
	volum=3;
	SPU_Set_MainVolume(VOLUME_MAX*volum/VOL_MAX_FAME);
	LCD_light=0;
	set_led_light(LCD_light);
	alarm_hour=8;
	alarm_minute=0;
	alarmFlag=0;
	
//	CallSound=TalkSound=alarm_music=0;
	PIC_Nums=PicSd_Nums=AUDIO_Nums=VID_Nums=1;
	Hour_24=MM_DD=0;
//	auto_screen_off=30;

	Init_player();
	
	player.HomeIconIdx[0]=player.HomeIconIdx[1]=player.HomeIconIdx[2]=player.HomeIconIdx[3]=-1;
	InBoxMail[0]=InBoxMail[1]=InBoxMail[2]=InBoxMail[3]=InBoxMail[4]=-1;
	player.PlayerAge =-1;
	for(i=0;i<6;i++) player.Room_Buff[i]=-1;
}

void Act_LoadSetting()
{
	INT16S i,fd,nRet;

	AddRootPath("Pororo\\user\\setting.ini");
	fd = open(rootPath, O_RDONLY);
	if(fd ==-1){
		OS_game=2;
	}else{
		nRet = read(fd,(INT32U)&Date_Start,2);
		nRet = read(fd,(INT32U)&volum,1);
		nRet = read(fd,(INT32U)&LCD_light,1);
		nRet = read(fd,(INT32U)&alarmFlag,1);
		nRet = read(fd,(INT32U)&alarm_hour,1);
		nRet = read(fd,(INT32U)&alarm_minute,1);

		nRet = close(fd);

		for(i=0;i<4;i++){
			if(player.HomeIconIdx[i]>=0 && player.HomeIconIdx[i]<=9) ;
			else player.HomeIconIdx[i]=-1;
		}
	}
}
const INT16S talk_times[]={
	60*60,	2*60*60,	(3*60+0)*60,	(3*60+30)*60,	(3*60+10)*60,	(3*60+20)*60
};
const INT16S talk_says[]={
	16,20,	1,10,	1,10,	1,10,	1,10,	1,8
};
//================================================================
INT32U	Play_Time=0;
void Test_Player()
{
	Play_Time++;
	if(Play_Time%25==0){
		SaveInBoxMail();
		GetMimiCall();
		if(Play_Time>50) GetEventFlag();
	}
	Show_mailRev();
}
