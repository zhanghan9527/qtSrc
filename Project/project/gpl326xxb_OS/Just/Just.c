#include "application.h"
#include "data.h"
#include "math.h"
//#include "os_res.h"
#include "icon.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"


extern const INT16U _icon_Sp_16x16_CellData[];
extern const INT16U _menuX3_CellData[];
extern const INT16U _menuX3_CellIdx[];
extern const INT16U _logo_Palette1[];
extern const INT16U _just_line_CellIdx[];
extern const INT16U _just_line_CellData[];

//===========================================================

INT16S just(INT16S Mode)
{
	int i;
	INT32U key_flag = 0;
	INT16S powerflag=0;
	INT32S tx[5],ty[5],x,y;
	
//	KeyService();
	
	if((Mode==0) && testJust()){
		return;
	}
	
	TouchTest=1;
	clear_sprite(Spritebuf_number);
	spMalloc(500);
	Palette_ram_cpy( 1,0, (INT32U)_logo_Palette1+512,256);
	Copy16(SpriteCellData,_icon_Sp_16x16_CellData,57344/2);
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	
	Curren_Frame=0;
	game_time=0;
	LoadTextBmp(C_PPU_TEXT1,"Pororo\\just\\M26_5.gif");
	
    gplib_ppu_fade_effect_set(ppu_register_set, 0x0);
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);
	//powerflag=Os_Flag;
	//Os_Flag=0;
	while(1){
		PaintTFT_cell4();
		
		if(Mode==1){
			//if(goto_desktop()){
				//Os_Flag=powerflag;
				//TouchTest=0;
				//return -1;
			//}					
		}
		
		//if(game_time%120==5) GetLV();
		
		if(TouchTest){
			if(game_time%8<4){
				if(TouchTest==5) set_sprite_display_init(1,WIDTH/2-8,HIGHT/2-8,(INT32U)justSp_Frame[0]);
				else if(TouchTest==1) set_sprite_display_init(1,16-8,16-8,(INT32U)justSp_Frame[0]);
				else if(TouchTest==2) set_sprite_display_init(1,WIDTH-16-8,16-8,(INT32U)justSp_Frame[0]);
				else if(TouchTest==3) set_sprite_display_init(1,16-8,HIGHT-16-8,(INT32U)justSp_Frame[0]);
				else if(TouchTest==4) set_sprite_display_init(1,WIDTH-16-8,HIGHT-16-8,(INT32U)justSp_Frame[0]);
			}else set_sprite_disable(1);
			if(TouchFlag){
				if(key_flag==0) key_flag=0x108;
				i=key_flag&0xff;
				if(key_flag&0x100){
					if(i) key_flag--;
					if(i==1){
						tx[TouchTest-1]=Average_X_FIFO();
						ty[TouchTest-1]=Average_Y_FIFO();
						if(TouchTest>1){
							x=tx[TouchTest-1]-tx[TouchTest-2];
							y=ty[TouchTest-1]-ty[TouchTest-2];
							if( (x<-300 || x>300 ) || (y<-300 || y>300 ) ) TouchTest++;
						}else TouchTest++;
						if(TouchTest>5){

							XCenter=tx[4];
							i=(tx[1]+tx[3]-tx[0]-tx[2])/2;
							DivValueX=i*100/(WIDTH-32);

							YCenter=ty[4];
							i=(ty[2]+ty[3]-ty[0]-ty[1])/2;
							DivValueY=i*100/(HIGHT-32);

							TouchTest=0;
							set_sprite_disable(1);
						}
					}
				}
			}else {
				key_flag=0;
			}
		}
		
		
		if(!TouchTest){
			if(DivValueX<441-120 || DivValueX>441+120 || DivValueY<513-160 || DivValueY>513+160){
				TouchTest=1;
				key_flag=0;
				//XCenter=3375;	YCenter=3027;
				//DivValueX=441;	DivValueY=513;
			}else{
			   //Os_Flag=powerflag;
				SaveJust();
				WaitNoTouch();
				return 0;
			}
//		 	InitText(C_PPU_TEXT1,(INT32U)_just_line_CellData,Tx1_buffer,TX_DEPTH0,TX_BANK0,TX_PALETTE0,TX_COLOR16,0,0,TX_HSIZE32,TX_VSIZE16);
//			UpdateText((INT32U)_just_line_CellIdx,Tx1_buffer);
			while(1)
			{
				PaintTFT_cell4();
				set_sprite_display_init(1,Coordinate_X-8,Coordinate_Y-8,(INT32U)justSp_Frame[0]);
			}

		}
	}
}




int testJust()
{
	INT16S test_handle_1;
	INT32S nRet=0;
	int data[5]={0};

	Chdir_Root("Pororo\\user");
	test_handle_1 = open("just.ini", O_RDONLY);
	if(test_handle_1==0){
		nRet = read(test_handle_1,(INT32U)&data,16);		
		nRet = close(test_handle_1);
		if( (data[0]==0)|| (data[1]==0)|| (data[2]==0)|| (data[3]==0)){
			nRet=0;
		}else{
			XCenter=data[0];
			YCenter=data[1];
			DivValueX=data[2];
			DivValueY=data[3];
			nRet=1;
		}
	}

	return nRet;
}

void SaveJust()
{
	INT16S test_handle_1;
	INT32S nRet;
	INT64U disk_size;
	int data[5]={0};

	//disk_size=vfsFreeSpace(DISKUSED);
	//if(disk_size < 50)return;
	
	
	Chdir_Root("Pororo\\user");
	
	
	test_handle_1 = open("just.ini", O_RDWR|O_CREAT|O_TRUNC);
	if(test_handle_1==0){
		data[0]=XCenter;
		data[1]=YCenter;
		data[2]=DivValueX;
		data[3]=DivValueY;
		nRet = write(test_handle_1,(INT32U)&data,16);
		nRet = close(test_handle_1);
	}
	
    if(DISKUSED==FS_NAND1){
	   DrvNand_flush_allblk();
    }
}


//==========================================================
/*
int testCard()
{
	int key_flag;
	clear_sprite(Spritebuf_number);
	
	// Now configure TEXT relative elements
	InitText(C_PPU_TEXT1,(INT32U)_insert_cartridge_CellData,Tx1_buffer,TX_DEPTH0,TX_BANK0,TX_PALETTE2,TX_COLOR16,0,0,TX_HSIZE32,TX_VSIZE16);
	UpdateText((INT32U)_insert_cartridge_CellIdx,Tx1_buffer);
	
	set_sprite_display_init(11,430,10,(INT32U)power_Frame[0]);
	PaintTFT_cell4();
	key_flag=0;
	game_time=0;
	gplib_ppu_fade_effect_set(ppu_register_set, 0x0);
	while(1)
	{
	    PaintTFT_cell4();
	
  		UpdateText((INT32U)_insert_cartridge_CellIdx+key_flag*510,Tx1_buffer);
  		
	    if(game_time%20==0){
	   		key_flag++;
	   		if(key_flag>3) key_flag=0;
	    }
	    
		Showpower(11);

	    if(Key_States==KEY_TOOL) return 1;	//
	    if(TestCardin()){
	    	drv_msec_wait(1000);
	    	return 0;
	    }
	}
}
//*/
//==========================================================




