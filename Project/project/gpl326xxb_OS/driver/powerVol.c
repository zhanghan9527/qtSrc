#include "application.h"
#include "Sprite_demo.h"
#include "Text_demo.h"
#include "powerVol.h"
#include "touch_panel.h"
//#include "os_res.h"



//======================================virtual vkey=============end
INT16S powtime=0;
INT16S volum=3,volumTime=0;
INT16S PowSp=0,VolSp=0;
int	PowVol_Idx=0;
void InitPowVol(INT16S powsp,INT16S volsp,int pv_idx)
{
	PowSp=powsp;
	VolSp=volsp;	
	if(pv_idx) PowVol_Idx=pv_idx*24;
	Palette_ram_cpy( 0, 192,vKey_SPRITE_Palette1+192,64);
	Set_Sprite_Group_bank(VolumSp_Frame,0,4);
	Set_Sprite_Group_bank(PowerSp_Frame,0,3);
	Set_Sprite_Group_bank(Key_black_Frame,0,1);
}
//======================================Power  &   Volum=============

void ShowPower()
{
	int i;

	if(game_time%100==5){

		GetLV();
		if(LVLevel<2)
			powtime=99;
	}
	if(powtime){
//	if(1){
		if(powtime==100) GetLV();
		if(LVLevel>2){
			set_sprite_display_init(PowSp+1,430+2,10,(INT32U)PowerSp_Frame[1]);
			SetSpDepth(PowSp+1, SP_DEPTH7);
			set_sprite_nCharNum(PowSp+1,PowVol_Idx);
		}else set_sprite_disable(PowSp+1);
		if(LVLevel>1){
			set_sprite_display_init(PowSp+2,430+10,10,(INT32U)PowerSp_Frame[2]);
			set_sprite_nCharNum(PowSp+2,PowVol_Idx);
			SetSpDepth(PowSp+2, SP_DEPTH7);
		}

		else set_sprite_disable(PowSp+2);
		if(LVLevel>0){
			set_sprite_display_init(PowSp+0,430,10,(INT32U)PowerSp_Frame[0]);
			set_sprite_display_init(PowSp+3,430+18,10,(INT32U)PowerSp_Frame[2]);
			set_sprite_nCharNum(PowSp+3,PowVol_Idx);
			set_sprite_nCharNum(PowSp+0,PowVol_Idx);
			SetSpDepth(PowSp+0, SP_DEPTH7);
			SetSpDepth(PowSp+3, SP_DEPTH7);
		}else{
			set_sprite_disable(PowSp+3);
			if(game_time%16<8){
				set_sprite_display_init(PowSp+0,430,10,(INT32U)PowerSp_Frame[0]);
				set_sprite_nCharNum(PowSp+0,PowVol_Idx);
				SetSpDepth(PowSp+0, SP_DEPTH7);
			}else set_sprite_disable(PowSp+0);
		}
		powtime--;
	}else{
		for(i=0;i<4;i++) set_sprite_disable(PowSp+i);
	}
}

void ShowVolum()
{
	int i;

	if(Key_States&KEY_VOLUP){
		if(volum<6){
			volum++;
			SPU_Set_MainVolume(VOLUME_MAX*volum/6);
		}
		volumTime=50;
	}else if(Key_States&KEY_VOLDOWN){
		if(volum>0){
			volum--;
			SPU_Set_MainVolume(VOLUME_MAX*volum/6);
		}
		volumTime=50;
	}
	if(volumTime){
		for(i=0;i<6;i++){
			if(i==0){
				if(volum>0) set_sprite_display_init(VolSp+0,130,212,(INT32U)VolumSp_Frame[0]);
				else set_sprite_display_init(VolSp+0,130,212,(INT32U)VolumSp_Frame[1]);

			}else{
				if(volum>i) set_sprite_display_init(VolSp+i,130+32+32*i,212+12,(INT32U)VolumSp_Frame[2]);
				else set_sprite_display_init(VolSp+i,130+32+32*i,212+12,(INT32U)VolumSp_Frame[3]);

			}
			set_sprite_nCharNum(VolSp+i,PowVol_Idx);
			SetSpDepth(VolSp+i, SP_DEPTH7);
		}
		volumTime--;
	}else {
		for(i=0;i<6;i++) set_sprite_disable(VolSp+i);
	}
}


//======================================Power  &   Volum=============end
