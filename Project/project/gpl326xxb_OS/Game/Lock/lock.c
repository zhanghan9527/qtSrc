#include "application.h"
#include "data.h"
#include "math.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "lock_star.h"

#define STAR_SP			1
#define LINE_SP			STAR_SP+9

extern const INT16U lock_SPRITE_Palette1[];
INT16S Locked=0;
extern const INT16U  StarRelXYPos[];
extern const INT16U  LineKindTable[];
extern const INT16S  LineRelXYPos[];

extern INT16S  nowPattern,lastPattern;
extern INT16S	PatternNo;
extern INT8S	SecurityPattern1[9],SecurityCode1[];
extern INT16S	LineSPKind;

extern INT8U	ArrayPtr1,ArrayPtr2;


void show_locktime(INT16S sp)
{
	INT16S h,m;
	
	if(Time_hour>=12) set_sprite_display_init(sp,60,30,Lock_time_Frame[12]);
	else set_sprite_display_init(sp,60,30,Lock_time_Frame[10]);

	h=Time_hour%12;
	m=Time_min;
	if(h==0) h=12;
	set_sprite_display_init(sp+1,96,18,Lock_time_Frame[h/10]);
	set_sprite_display_init(sp+2,96+20,18,Lock_time_Frame[h%10]);
	if(game_time/12%2) set_sprite_display_init(sp+3,96+20*2+2,18+3,Lock_time_Frame[11]);
	else set_sprite_image_number(sp+3,0);
	set_sprite_display_init(sp+4,96+20*3-8,18,Lock_time_Frame[m/10]);
	set_sprite_display_init(sp+5,96+20*4-8,18,Lock_time_Frame[m%10]);
	
	set_sprite_display_init(sp+6,60,50,Lock_date_Frame[Year/1000]);
	set_sprite_display_init(sp+7,60+9,50,Lock_date_Frame[Year/100%10]);
	set_sprite_display_init(sp+8,60+9*2,50,Lock_date_Frame[Year/10%10]);
	set_sprite_display_init(sp+9,60+9*3,50,Lock_date_Frame[Year%10]);
	set_sprite_display_init(sp+10,60+9*4,50-1,Lock_date_Frame[12]);

	set_sprite_display_init(sp+11,118,50,Lock_date_Frame[Month/10]);
	set_sprite_display_init(sp+12,118+9,50,Lock_date_Frame[Month%10]);
	set_sprite_display_init(sp+13,118+9*2,50-1,Lock_date_Frame[11]);

	set_sprite_display_init(sp+14,155,50,Lock_date_Frame[Day/10]);
	set_sprite_display_init(sp+15,155+9,50,Lock_date_Frame[Day%10]);
	set_sprite_display_init(sp+16,155+9*2,50-1,Lock_date_Frame[10]);
	
}
INT16S lock_3times=0;

INT16S Test_Pattern()
{
	int i;
	
	if(SecurityCode1[0]==4 && SecurityCode1[1]==-1){
		lock_3times++;
		if(lock_3times>=3) return 1;
	}
		
	for(i=0;i<9;i++){
		if(player.Lock_Pattern[i]!=SecurityCode1[i]) return 0;
	}
	return 1;
}
void main_lock()
{	
	INT16S i,page=0,page2;
	INT16S m,drag,Dx;
	INT16S tmp,tmpx,tmpy;
	
	spMalloc(500);
	Palette_ram_cpy( 1,0, lock_SPRITE_Palette1	,256);	
	LoadData_1(1,SpriteCellData,"NoteBook\\lock\\SpriteCell_cmp.BIN");
	
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	Display_Background(1);
	clear_sprite(Spritebuf_number);
	gplib_ppu_fade_effect_set(ppu_register_set, 0);

	if(player.Lock_flag) step=20;
	else step=0;
	while(1)
	{
		PaintTFT_cell4();
		if(Goto_desktop_Home()){
			step=100;
		}
		switch(step){
//--------------------------------------------------unlock
			case 0:
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,0,0,Lock_un_Frame[0]);
				set_sprite_display_init(1,24,280,Lock_un_Frame[1]);	//176
				game_time=0;
				drag=Dx=0;
				step++;
			break;
			case 1:
				if(game_time<50);
				else if(game_time<50+8*42) set_sprite_display_init(2,0,0,Lock_angel_Frame[(game_time-50)/8%42]);
				else if(game_time>150+8*42) game_time=0;
				else set_sprite_image_number(2,0);
				show_locktime(30);
				
				set_sprite_x(1,24+Dx);
				m=TestDragRel_2(0,20);
				if(m==0){
					if(drag){
						Dx=mov_len;
						if(Dx<0) Dx=0;
						else if(Dx>152) Dx=152;
					}else if(mov_len==0 && TestTouch(24,24+44,278,278+30) ){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 127, SFX_CHANL);
						drag=1;
					}
				}else if(m==1){
					if(Dx>120){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[STAR_SFX], 64, 127, 12);
						step=100;						
					}else drag=Dx=0;
				}
			break;
//-----------------------------------------------------lock			
			case 20:
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,0,0,Lock_star_Frame[1]);
				Locked=1;
				nowPattern=0;lastPattern=0;PatternNo=0;
			  	for (i=0;i<9;i++)	SecurityPattern1[i]=-1;
			  	for (i=0;i<9;i++)	SecurityCode1[i]=-1;
			  	ArrayPtr1=0;
				step++;
			break;
			case 21:
				show_locktime(30);
				if(TouchFlag){
					for (i=0;i<9;i++){
						if(TestTouch(28-5+StarRelXYPos[2*i],28+46+5+StarRelXYPos[2*i],
								79-5+StarRelXYPos[2*i+1],79+46+5+StarRelXYPos[2*i+1])) break;
					}
					if(i!=9 && PatternNo<9 && SecurityPattern1[i]==-1 ){
						set_sprite_display_init(STAR_SP+i,14+StarRelXYPos[2*i],64+StarRelXYPos[2*i+1],Lock_star_Frame[0]);
						SecurityPattern1[i]=i;
						SecurityCode1[ArrayPtr1]=i;
						nowPattern=i;
						if(PatternNo){		//»­Ïß
							if(StarRelXYPos[nowPattern*2]>StarRelXYPos[lastPattern*2])
								tmpx=(StarRelXYPos[nowPattern*2]-StarRelXYPos[lastPattern*2])/70;
							else 
								tmpx=(StarRelXYPos[lastPattern*2]-StarRelXYPos[nowPattern*2])/70+3;							
								
							if(StarRelXYPos[nowPattern*2+1]>StarRelXYPos[lastPattern*2+1])
								tmpy=(StarRelXYPos[nowPattern*2+1]-StarRelXYPos[lastPattern*2+1])/70;
							else 
								tmpy=(StarRelXYPos[lastPattern*2+1]-StarRelXYPos[nowPattern*2+1])/70+3;
								
							tmp=tmpx+tmpy*6;
							
							LineSPKind=LineKindTable[tmp];	
							set_sprite_display_init(LINE_SP+i,14+28+StarRelXYPos[2*lastPattern]+LineRelXYPos[2*tmp],
							64+28+StarRelXYPos[2*lastPattern+1]+LineRelXYPos[2*tmp+1],Lock_line_Frame[LineSPKind]);														
							lastPattern=i;
							ArrayPtr1++;
							PatternNo++;							
						}else{
							lastPattern=i;						
							PatternNo++;
							ArrayPtr1++;
						}

					}else {
					
					}			
				}else if (TouchRelease){
					if(Test_Pattern()){
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[STAR_SFX], 64, 127, 12);
						step=100;
						Locked=0;
					}else{
						SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[FAIL_SFX], 64, 127, 12);
						step=20;
					}
				}   	    	
			break;
		}
		if(step==100) break;
	}
    if(DISKUSED==FS_NAND1){
	   DrvNand_flush_allblk();
    }
}
