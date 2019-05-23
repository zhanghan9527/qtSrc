#include "application.h"
#include "data.h"
#include "math.h"
#include "icon.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "ir.h"
#include "key.h"
#include "testbg.h"

INT32U downcount=0,startclock;
int PrintfCounter=0;
INT16S TestMode_Flag=0;

const char* checksum[]={
	"Checksum: 0x151813/",
};

extern const INT32U  test_button_Frame[];
extern const INT16U _test_button_Sp_16x16_CellData[];
extern const INT16U ALIGN4 _test_Palette1[];
extern const INT16U ALIGN4 Test_SPRITE_Palette1[];
extern const char* SfxName_Pub[];

extern const INT32U  test_ASCII_Frame[];

extern const INT16S KeyScanCode0[];

/*
const KeyScanCode0[]={		//Shift Not Pressed ,Caps Is  UnLocked
K_Language	,K_s		,K_x		,K_w		,0x00			,K_Up			,K_Album		,K_N2	,
K_VideoCall	,K_a		,K_z		,K_q		,0x00			,K_Left			,K_Decoration	,K_N1	,
0x00		,K_k		,K_Comma	,K_i		,K_Heart		,K_Div			,K_Minus		,K_N8	,
K_LastVideo	,K_g		,K_b		,K_t		,0x00			,K_Ok			,K_MimiCard		,K_N5	,
K_NextVideo	,K_j		,K_m		,K_u		,K_Star			,K_SQuotes		,K_Plus			,K_N7	,
K_Diary		,K_Home		,0x00		,K_Setting	,0x00			,K_Return		,K_Camera		,K_Back,
K_Video		,K_f		,K_v		,K_r		,0x00			,K_Right		,K_EMail		,K_N4	,
K_Play		,K_h		,K_n		,K_y		,K_Enter		,K_Shift		,K_Del			,K_N6	,
K_Space		,K_d		,K_c		,K_e		,0x00			,K_Down			,K_Game			,K_N3	,
0x00		,K_l		,K_Point	,K_o		,K_p			,K_SemiColon	,K_N0			,K_N9	,
};
*/

const INT16S KeyPos[]={		//按键位置, x,y
79,150,		66,94,	77,123,		56,67,		00,00,			153,189,		72,8,			56,39,
38,150,		44,94,	56,123,		34,67,		00,00,			129,204,		44,8,			34,39,
00,00,		195,94,	207,123,	185,67,		250,67,			250,123,		250,39,			185,39,
250,150,	130,94,	142,123,	120,67,		00,00,			205,207,		155,8,			120,39,
293,150,	173,94,	185,123,	163,67,		271,67,			260,95,			271,39,			163,39,
15,150,		17,94,	00,00,		12,67,		00,00,			100,207,		16,8,			12,39,
229,150,	109,94,	120,123,	99,67,		00,00,			176,204,		127,8,			99,39,
271,150,	152,94,	163,123,	142,67,		287,95,			271,123,		293,39,			142,39,
144,150,	87,94,	99,123,		77,67,		00,00,			153,220,		100,8,			77,39,
00,00,		217,94,	228,123,	207,67,		228,67,			238,95,			228,39,			207,39,
};


INT16S KeyStatus[80]={

0x01	,0x01		,0x01		,0x01		,0x00			,0x01			,0x01		,0x01	,
0x01	,0x01		,0x01		,0x01		,0x00			,0x01			,0x01		,0x01	,
0x00	,0x01		,0x01		,0x01		,0x01			,0x01			,0x01		,0x01	,
0x01	,0x01		,0x01		,0x01		,0x00			,0x01			,0x01		,0x01	,
0x01	,0x01		,0x01		,0x01		,0x01			,0x01			,0x01		,0x01	,
0x01	,0x01		,0x00		,0x01		,0x00			,0x01			,0x01		,0x01,
0x01	,0x01		,0x01		,0x01		,0x00			,0x01			,0x01		,0x01	,
0x01	,0x01		,0x01		,0x01		,0x01			,0x01			,0x01		,0x01	,
0x01	,0x01		,0x01		,0x01		,0x00			,0x01			,0x01		,0x01	,
0x00	,0x01		,0x01		,0x01		,0x01			,0x01			,0x01		,0x01	,


};

void Clock10Msecond();

#define ASCII_Small	ASCII_IDX

#define TEXT_SIZE_H   240
#define TEXT_SIZE_V   320
const INT16S ascII_xsize[]={
4	,	//0  ' '
4	,	//1		'!'
8	,	//2
9	,	//3
8	,	//4
11	,	//5
11	,	//6
5	,	//7
5	,	//8
5	,	//9
7	,	//10
8	,	//11
5	,	//12
5	,	//13
4	,	//14
8	,	//15
9	,	//16
6	,	//17
8	,	//18
8	,	//19
10	,	//20
8	,	//21
9	,	//22
8	,	//23
9	,	//24
9	,	//25
4	,	//26
5	,	//27
10	,	//28
8	,	//29
10	,	//30
7	,	//31
13	,	//32
11	,	//33
9	,	//34
10	,	//35
11	,	//36
8	,	//37
8	,	//38
10	,	//39
10	,	//40
5	,	//41
7	,	//42
9	,	//43
8	,	//44
13	,	//45
10	,	//46
11	,	//47
9	,	//48
11	,	//49
9	,	//50
8	,	//51
10	,	//52
10	,	//53
11	,	//54
15	,	//55
10	,	//56
10	,	//57
10	,	//58
5	,	//59
8	,	//60
5	,	//61
9	,	//62
9	,	//63
5	,	//64
8	,	//65
9	,	//66
8	,	//67
9	,	//68
8	,	//69
7	,	//70
9	,	//71
9	,	//72
4	,	//73
6	,	//74
8	,	//75
4	,	//76
12	,	//77
9	,	//78
9	,	//79
9	,	//80
9	,	//81
6	,	//82
7	,	//83
7	,	//84
9	,	//85
9	,	//86
13	,	//87
9	,	//88
9	,	//89
8	,	//90
6	,	//91
4	,	//92
6	,	//93
8	,	//94
11	,	//95
11	,	//96
10	,	//97
8	,	//98
8	,	//99
8	,	//100
8	,	//101
5	,	//102
6	,	//103
5	,	//104
11	,	//105
11	,	//106
14	,	//107
10	,	//108
10	,	//109
9	,	//110
10	,	//111
8	,	//112
8	,	//113
8	,	//114
8	,	//115
8	,	//116
8	,	//117
8	,	//118
5	,	//119
6	,	//120
6	,	//121
9	,	//122
9	,	//123
13	,	//124
9	,	//125
9	,	//126
9	,	//127
9	,	//128
8	,	//129
11	,	//130
10	,	//131
9	,	//132
9	,	//133
9	,	//134
5	,	//135
10	,	//136

};


void ClearAscII(int spIdx)
{
	while( PrintfCounter > 0 )
	{
		PrintfCounter--;
		set_sprite_disable(spIdx+PrintfCounter);
	}
}  

#define MAX_ROW		10

//x:为中心点坐标
void PrintfAscii_explain_center(int spIdx,int x,int y,const char *date_str,const INT32U *abc_Frame,const INT16S Xsize[],INT16S idx,int offsety)
{
	int row=0,column_size=0,ascii_value=0;
	int i=0,space_flag=0;
	int realsize=0;
	int num=0;
	const char *str;
	INT16S ch;
	INT16S each_row_x[MAX_ROW];

	str=date_str;
	ascii_value=*str++;
	column_size=0;
//	ClearAscII(spIdx);
	while(ascii_value !=0x00)
	{
		if(ascii_value=='/'){
			
			each_row_x[row]=x-column_size/2;
			row++;
			column_size=0;
			ch=*(str);
			if(ch==' '){
				str++;
			}
		}else{
			if(ascii_value>=127){
				str++;
			}
			column_size+=Xsize[ascii_value-' '];
		}
		
		ascii_value=*str++;
			
	}
	
//	x=x-column_size/2;

	str=date_str;
	column_size=0;
	ascii_value=*str++;
	i=0;
	row=0;
    x=each_row_x[row++];
	while(ascii_value !=0x00)
	{
		if(ascii_value==' '){		//	' '
			x+=Xsize[ascii_value-' '];
		}else if(ascii_value=='/'){
			x=each_row_x[row++];
			y+=offsety;
			ch=*(str);
			if(ch==' '){
				str++;
			}
		}else
		{
			if(ascii_value>=127){
				str++;
			}
			set_sprite_display_init(spIdx+PrintfCounter,x,y,abc_Frame[0]);
			set_sprite_nCharNum(spIdx+PrintfCounter,idx+ascii_value-' ');
			SetSpDepth(spIdx+PrintfCounter,SP_DEPTH1);	 //SP_DEPTH1
			x+=Xsize[ascii_value-' '];
			
			PrintfCounter++;
		}
		ascii_value=*str++;
	}
}

void DrawPenPoint(int flag,INT32U addr,int x,int y,int dec,int color)
{
	int i,j;
	INT16U *ptr;
	if(flag!=3){
		if(dec==2) x-=1,y-=1;
		else if(dec==4) x-=2,y-=2;
		else if(dec==8) x-=4,y-=4;
	}

	ptr=(INT16U*)(addr+y*TEXT_SIZE_H*2+x*2);
	if(flag==3) dec*=2;
	for(i=0;i<dec;i++){
		if((y+i)>=TEXT_SIZE_V) break;
		//else if((y+i)<=71) continue;
		for(j=0;j<dec;j++)
		{
			if((x+j)>=TEXT_SIZE_H) break;
			//else if((x+j)<=64) continue;
			if(flag==3) *(ptr+i*TEXT_SIZE_H+j)=0xffff;
			else if(flag==5) *(ptr+i*TEXT_SIZE_H+j)=color;
			else{
				//*
				if(dec==4){
					if((i==0)&&((j==0)||(j==3))) continue;
					if((i==3)&&((j==0)||(j==3))) continue;
				}else if(dec==8){
					if((i==0)&&((j==0)||(j==1)||(j==6)||(j==7))) continue;
					if((i==1)&&((j==0)||(j==7))) continue;
					if((i==6)&&((j==0)||(j==7))) continue;
					if((i==7)&&((j==0)||(j==1)||(j==6)||(j==7))) continue;
				}	//*/
				*(ptr+i*TEXT_SIZE_H+j)=color;
			}
		}
	}
}

void clear_all_user_data()
{
	INT16S ret;
	
	AddRootPath("Pororo\\user");
	_deleteall(rootPath);
	AddRootPath("Pororo\\daily");
	_deleteall(rootPath);
	
	Disk_Type=FS_NAND2;
	Chdir_Root("m2_photo");
    _deleteall("");
	Chdir_Root("Mimimail");
    _deleteall("");
	Chdir_Root("M1-1_video");
    _deleteall("");
	Chdir_Root("M1-2_video");
    _deleteall("");
    
	Set_Rtc_reset();
    
	DrvNand_flush_allblk();
}

extern INT32U scaler_task_state;
extern const INT16U ALIGN4 Note_SPRITE_Palette1[];


void Clock10Msecond()
{
	downcount++;	
}

INT16S check_sd_RW()
{
	INT16S fd;
	INT32S nRet;
	INT32U data;

    
	fd = open("c:\\test_.ini", O_RDWR|O_CREAT|O_TRUNC);
	
	if(fd>=0){
		data=0x55AA;
		nRet = write(fd,(INT32U)&data,4);	
		nRet = close(fd);
	}else{
		return -1;
	}	
	
	PaintTFT_cell4();
	data=0;

	fd=open("c:\\test_.ini",O_RDONLY);
	if(fd>=0){
		nRet = read(fd,(INT32U)&data,4);
		nRet = close(fd);
		unlink((char *)"c:\\test_.ini");
		if(data==0x55AA){
			return 0;	
		}else return -1;
	}else return -1;
	
}

//========================================================IR
extern unsigned int KeyCode[];
#define KEY_TURN 0x20
//==================================================================
void testmode()
{
    int step,num,i,m;
    char filename[100];
    char timebuff[20];
    INT32S timeUp=0;
    int t,sd_check;

	TestMode_Flag=0;
//*
	GetKeyBoardCode();
	if( (KeyCode[6]&0x08) && (KeyCode[9]&0x80) ){
		;
	}else return;
	//*/
	
    InitState();

	TestMode_Flag=1;    
// 	just(1);
  	
#if IR_TEST==1	
	ir_task_demo();	
//	OSQPost(IRTaskQ,(void *)MSG_IR_SEND_API_INIT);
//	OSTimeDly(1);	
//	OSQPost(IRTaskQ,(void *)MSG_IR_DECODE_API_INIT);
    ir_read=0;
#endif	
//  Copy16(SpriteCellData,_test_button_Sp_16x16_CellData,6144/2);
//	LoadData_1(1,SpriteCellData,"15002\\os\\testmode\\SpriteCell_cmp.BIN");
//	LoadData_1(1,buttonCellData,"15002\\os\\spdata\\button\\SpriteCell.bin");
//	LoadDRM(0,18,SFX_ADDR,(int*)&DrmAddr[SFX_PUBLIC],SfxName_Pub);
	spMalloc(200);
	clear_sprite(Spritebuf_number);
	LoadData_1(1,SpriteCellData,"Pororo\\testmode\\SpriteCell_cmp.BIN");
	Palette_ram_cpy(1,0, (INT32U)_test_Palette1+0,16);
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	
    gpio_write_io(T430_RESET_PIN, DATA_HIGH);
   
    gplib_ppu_fade_effect_set(ppu_register_set, 0x0);
    step=0;
    num=1;
    Desk_flag=0;
//	set_alarm();
//	idp_rtc_alm_pin_set(1);	
    while(1)
    {
    	PaintTFT_cell4();
    	sleeptime=WAITINGTIME;
    	
   	    switch(step)
   	    {
   	    	case 0:
   	    		sprintf(filename,"Pororo\\testmode\\%d.gif",num);
   	    		LoadTextBmp(C_PPU_TEXT1,filename);
   	    		num++;
   	    		step=1;
   	    	break;
   	    	case 1:
   	    		ClearAscII(20);
				PrintfAscii_explain_center(20,120,300/2,checksum[0],test_ASCII_Frame,ascII_xsize,0,32);
   	    		if(KeyBoardCode==K_Ok){
   	    			if(num<7)step=0;
   	    			else step++;
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);												
   	    		}else if(KeyBoardCode==K_Left){
   	    			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
   	    			if(LCD_light>0) LCD_light--;
	   	    		set_led_light(LCD_light);
	   	    	}else if(KeyBoardCode==K_Right){
	   	    		SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												
   	    			if(LCD_light<2) LCD_light++;
	   	    		set_led_light(LCD_light);
	   	    	}
   	    	break;
   	    	case 2:
   	    		show_color_RGB565_bg(0,0,0);
   	    		//timer_msec_setup(TIMER_B, 10,0,Clock10Msecond);

   	    		downcount=0;
   	    		startclock=1;
   	    		ClearAscII(20);
//				PrintfAscii_explain_center(20,120,300/2,"please mov pen !/",test_ASCII_Frame,ascII_xsize,0,32);

				sd_check=check_sd_RW();
				
   	    		step++;
   	    	break;
   	    	case 3:
   	    		/*
   	    		if(Key_States&KEY_VOLUP){
   	    			SPU_PlayPCM_NoEnv_FixCH((INT32U*)OpenData16, 64, 127, 15);	
   	    		}else if(Key_States&KEY_VOLDOWN){
   	    			SPU_PlayPCM_NoEnv_FixCH((INT32U*)OpenData16, 64, 127, 15);
   	    		}
   	    		
   	    		for(i=0;i<8;i++){
   	    			set_sprite_spcell(i,0);
   	    		}
   	    		
   	    		if(Key_State&KEY_VOLUP){
   	    			set_sprite_display_init(1,373,32,test_button_Frame[2]);		
   	    		}
   	    		
   	    		if(Key_State&KEY_VOLDOWN){
    	    		set_sprite_display_init(0,350,33,test_button_Frame[1]);		
  	    		}
   	    		
   	    		if(Key_State&KEY_UP){
   	    			set_sprite_display_init(2,56,112,test_button_Frame[0]);	
   	    		}
   	    		
   	    		if(Key_State&KEY_DOWN){
   	    			set_sprite_display_init(3,56,149,test_button_Frame[0]);	
   	    		}
   	    		
   	    		if(Key_State&KEY_LEFT){
   	    			set_sprite_display_init(4,40,131,test_button_Frame[0]);	
   	    		}
   	    		
   	    		if(Key_State&KEY_RIGHT){
   	    			set_sprite_display_init(5,71,131,test_button_Frame[0]);	
   	    		}
   	    		
   	    		if(Key_State&KEY_HOME){
   	    			set_sprite_display_init(6,371,217,test_button_Frame[3]);
   	    		
   	    		}
   	    		
   	    		if(Key_State&KEY_OK){
   	    			set_sprite_display_init(7,403,118,test_button_Frame[4]);
   	    		}
				*/
				//direct_enter_game();
    			//sprintf(timebuff,"%02d:%02d:%02d  %d/",downcount/6000%60,downcount%6000/100,downcount%100,directgame);
    			downcount+=4;
    			sprintf(timebuff,"%02d:%02d:%02d /",downcount/6000%60,downcount/100%60,downcount%100);

				ClearAscII(20);
				PrintfAscii_explain_center(20,120,80,timebuff,test_ASCII_Frame,ascII_xsize,0,32);
//				PrintfAscii_explain_center(20,120,300/2,"please mov pen !/",test_ASCII_Frame,ascII_xsize,0,32);
				if(sd_check){
					sprintf(filename,"SD Failed/");	
				}else{
					sprintf(filename,"SD OK/");
				}
				PrintfAscii_explain_center(20,120,300/2+32,filename,test_ASCII_Frame,ascII_xsize,0,32);
				//set_sprite_display_init(5,Coordinate_X,Coordinate_Y,test_button_Frame[0]);
				
/*   	    		
   	    		if(TouchFlag){
   	    			DrawPenPoint(5,(INT32U)TVOutBuff,Coordinate_X,Coordinate_Y,2,0x7C00);
   	    		}
   	    		
   	    		
   	    		if(TouchRelease){
   	    			PlaySound(STAR_SFX,127,12);
	   	    		//else SPU_PlayPCM_NoEnv_FixCH((INT32U*)CloseData16, 64, 127, 15);
   	    			//step=10;
   	    		}
*/   	    			
   	    			
   	    		if(KeyBoardCode==K_Ok){
	   	    		ClearAscII(20); 
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 127, SFX_CHANL);												
   	    			step++;	
   	    		}
   	    		
   	    		/*
   	    		if(Key_States&KEY_OK){
   	    			dma_memory_fill(TVOutBuff,0,480*272*2);
   	    			InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);
   	    			Change_SensorMode(0);
   	    			Wait_Sensor(3);
   	    			step++;
   	    		}	//*/
   	    		
   	    	break;

//========================================================test sensor===============
   	    	case 4:
				//Main_Sensor2();
				Main_VideoEncode();
				SetWaveIo(1);
				main_VideoPlay();
				clear_sprite(Spritebuf_number);
				LoadData_1(1,SpriteCellData,"Pororo\\testmode\\SpriteCell_cmp.BIN");
				Palette_ram_cpy(1,0, (INT32U)_test_Palette1+0,16);
				gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
   	    		LoadTextBmp(C_PPU_TEXT1,"Pororo\\testmode\\keyboard.gif");   	    		
    			step++;	
   	    	break;
//========================================================test keyboard=============
   	    	case 5:
   	    		m=KeyBoardCode;
   	    		if(m!=0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);												

	   	    		for(i=0;i<80;i++){
	   	    			if(KeyBoardCode==KeyScanCode0[i]) break;
	   	    		}
	   	    		if(i!=80){
	   	    			KeyStatus[i] =10;
	   	    		}

	   	    		if(KeyBoardCode==K_Return) step=20;		   	    		
   	    		}
   	    		for(i=0;i<80;i++){
   	    			if(KeyStatus[i]>1){
	   	    			KeyStatus[i]--;
	   	    			if(KeyStatus[i]/2%2==0) set_sprite_display_init(1+i,KeyPos[2*i]-9,KeyPos[2*i+1]-7,RedDot_Frame[0]);
	   	    			else set_sprite_display_init(1+i,KeyPos[2*i]-9,KeyPos[2*i+1]-7,0);
   	    			}
   	    		}
   	    	break;   	    	
//================================================================
   	    	case 20:
				show_color_RGB565_bg(0,0,0);
				ClearAscII(20);
				PrintfAscii_explain_center(20,120,120,"please waitting !/",test_ASCII_Frame,ascII_xsize,0,32);
				step=30;
   	    	break;

    	    case 30:
				ChangeCodePage(UNI_ENGLISH);
			    Disk_Type=FS_NAND1;
			    clear_all_user_data();
			    
			    clear_sprite(Spritebuf_number);
			    PrintfAscii_explain_center(40,120,120,"Test OK !/ user data deleted !/",test_ASCII_Frame,ascII_xsize,0,32);
				//PrintfAscii_explain_center(40," user data deleted !",240,150);
				step++;
   	    	break;
    	    case 31:
				while(1)
				{
					gplib_ppu_go_and_wait_done(ppu_register_set);
					Paintdelay();
					PowerOff();
				}	
   	    	break;
     	    case 32:
     	    /*
				Disk_Type=FS_NAND1;
			    Chdir_Root("photo");
			    _deleteall("");
			    _copy("A:\\OS\\menu\\DefaultPhoto_01.jpg","B:\\photo\\DefaultPhoto_01.jpg");
			    _copy("A:\\OS\\menu\\DefaultPhoto_02.jpg","B:\\photo\\DefaultPhoto_02.jpg");
			    _copy("A:\\OS\\menu\\DefaultPhoto_03.jpg","B:\\photo\\DefaultPhoto_03.jpg");
			    Chdir_Root("voice");
			    _deleteall("");
			    Chdir_Root("video");
			    _deleteall("");
			    if(DISKUSED==FS_NAND1){
				   DrvNand_flush_allblk();
			    }
					  */
   	    	break;
	    }
    	
    }

}
			 /*

INT16S Act_Main_Sensor1()
{
    INT32S ret,i,j;
	VIDEO_ARGUMENT arg;
	MEDIA_SOURCE   src;

//--------------------------
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
    //set_sprite_display_init(MESSAGE_SP+2,320-32,400,menu_icon_Frame[3]);//ok
    clear_sprite(Spritebuf_number);
    
	Palette_ram_cpy(1,0,XK_SPRITE_Palette1+256*0,256);
	Palette_ram_cpy(3,0,XK_SPRITE_Palette1+256*2,256);
	dma_memory_fill(TVOutBuff,0,480*272*2);
	InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);
    gplib_ppu_fade_effect_set(ppu_register_set, 0);	
	step=0;
	while (1)
	{
		if(Key_States&KEY_HOME){
			ret=-1;
			break;
		}
		PaintTFT_cell4();
		switch(step)
		{
//--------------------------camara
			case 0:
				Change_SensorMode(0);
				set_sprite_display_init(0,480-60,15,Camera_sp_Frame[1]);
				set_sprite_display_init(1,480-60,80,Camera_sp_Frame[5]);
				show_Buttons(2,BUT_Ok_X2,BUT_Ok_Y2,18);
				show_Buttons(3,BUT_Del_X2,BUT_Del_Y2,7);
				show_Buttons(4,BUT_Cat_X,BUT_Cat_Y,22);
				Wait_Sensor(3);
				//show_Buttons(5,0,0,40);
				//set_sprite_display_init(6,0,100,Camera_sp_Frame[4]);
				step++;
			break;
			case 1:
				if(scaler_task_state==0){
					Sensor_scale_to_buff(0);
					scaler_task_state=2;
				}
				
				
		}
	}			 */
	
	
	