#include "application.h"
#include "data.h"
#include "math.h"
//#include "os_res.h"
//#include "icon.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"

INT16S SD_Server();
extern BOOLEAN Usb_Dvice_WebCam_Demo_turn_on_flag;
INT16S saveType=0,SD_Mount=-1;
INT16S Usb_delay=0;

//===========================================================
#define C_USBDEVICE_PIN   IO_F5 

void usb_TestIo()
{
	if(s_usbd_pin){
		//gpio_write_io(C_USBDEVICE_PIN,0);
	}else{
		Usb_delay=0;
		//gpio_write_io(C_USBDEVICE_PIN,1);
	}
	s_usbd_pin=gpio_read_io(C_USBDEVICE_PIN);
	
	//s_usbd_pin=0;
	if(JTAG==1)	s_usbd_pin=0;

}

void usb_server(void)
{	
	s_usbd_pin=gpio_read_io(C_USBDEVICE_PIN);
	if (s_usbd_pin && Usb_delay==0){
		//testmode();
		//*
		ChangeCodePage(UNI_ENGLISH);
		InitState();
		//Close_Sensor();
		gplib_ppu_fade_effect_set(ppu_register_set, 0xff);
		tft_clk_set(TFT_CLK_DIVIDE_8);
		clear_sprite(Spritebuf_number);
		PaintTFT_in();
		LoadTextBmp(C_PPU_TEXT1,"Pororo\\menu\\bg\\M_usb.gif");
		gplib_ppu_fade_effect_set(ppu_register_set, 0x00);
		PaintTFT_in();
		PaintTFT_in();	//*/

		//Usb_Dvice_WebCam_Demo();
		system_clock_set(48);

		_deviceunmount(FS_SD);
		//if(*(int*)IOH_DATA_ADDR & 0x40) SetCloseFlag();
		SetOpenFlag();
		storages_init(0x32);//Nand
		//usb_ver_set(0);
		state_usb_entry((void *)1);

		//stop detect 
		storage_polling_stop();
		system_clock_set(96);
		tft_clk_set(TFT_CLK_DIVIDE_2);

		_deviceunmount(FS_SD);
		SD_Mount=-1;
		Usb_delay=1;
	}
}

void usb_server2(void)
{

	storages_init(0x30);//Nand

	state_usb_entry((void *)1);
	
	//stop detect 
	storage_polling_stop();
	while(1);


	while (1){

		//SetOpenFlag();
//		Usb_Dvice_WebCam_Demo_turn_on_flag =1;
		storages_init(0x32);//Nand
		while(1)
		{	
			
			if (storage_detection(4)){
				state_usb_entry((void *)1);
			}else break;
		}
		//stop detect 
		storage_polling_stop();		
	}
}

//==========================================================
INT16S SD_Server()
{
	if(TestCardin()){
		if(SD_Mount==-1){
			if(mount_Disk(FS_SD)==0) {
				SD_Mount=1;
				//GetSD_Nums();
			}else{
				SD_Mount=0;
			}
		}else if(SD_Mount==1){
			Save_SD=saveType;
		}else Save_SD=0;
	}else{
		SD_Mount=-1;
		Save_SD=0;
	}
}


INT16S direct_enter_game()
{


	return 0;		
}
INT16S enter_game()
{
	return -1;
}
//==========================================================
extern INT16S OS_game,PowerOff_Video,MimiCall;
extern t_rtc Rtc_time,Rtc_alarm;
//INT8S alarm;
INT16S Goto_desktop(void)
{	
	if(KeyBoardCode==K_Home && OS_game!=0){		//press home
		OS_game=0;
		homeFlag=-1;
	}else if(s_usbd_pin==1 && Usb_delay==0){		//usb
		OS_game=8;
		homeFlag=-2;
	}else if(alarm==1){				//闹铃发生
		OS_game=9;
		homeFlag=-3;
	}else if(MimiCall){		//Call 发生
		OS_game=15;
		homeFlag=-4;
	}else if(TestMenu_Key()){		//press menu_key
		homeFlag=-5;
	}else homeFlag=0;

	if(homeFlag){
		stopSpu();
	}
	return homeFlag;
}


INT16S Goto_desktop_Home(void)
{
	homeFlag=0;
	if(alarm==1){
		OS_game=9;
		homeFlag=-3;
	}else if(s_usbd_pin==1 && Usb_delay==0){				//usb
		OS_game=8;
		homeFlag=-2;
	}else if(TestMenu_Key()){		//press menu_key
		homeFlag=-5;
	}

	return homeFlag;
}
INT16S Goto_desktop_watch(void)
{
	homeFlag=0;
	if(Key_States&KEY_HOME){
		homeFlag=-1;
	}else if(s_usbd_pin && Usb_delay==0){
		homeFlag=-4;
	}else if(PowerOff_Video){
		homeFlag=-5;
	}

	return homeFlag;
}

//当OS_game==原游戏时,调到gameto
void GotoGame(INT16S gameno,INT16S gameto)
{
	SPU_StopChannel(SPEECHCHANL);
	SPU_StopChannel(MUSIC_CHCHANL);
	sleeptime=WAITINGTIME;
	homeFlag=0;
	game_time=0;
	if(alarm==3) alarm=0;
	clear_sprite(Spritebuf_number);
	if(OS_game==gameno) OS_game=gameto;
}



