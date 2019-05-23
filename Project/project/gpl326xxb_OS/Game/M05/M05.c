#include "application.h"
#include "data.h"
#include "math.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "m5_Uisp.h"

extern const INT16U M05_SPRITE_Palette1[];

INT16S WhoCall,CallAct,CallSpeech;
INT16S MimiCall=0,Call_Time=0,Call_Times=0;
INT16S Place_old=-1;
INT16S getRoom()
{
	int i,m;
	while(1){
		m=GetRandom(10)%8;
		for(i=0;i<6;i++){
			if(m==player.Room_Buff[i]) break;
		}
		if(i==6) break;
	}
	for(i=5;i>0;i--){
		player.Room_Buff[i]=player.Room_Buff[i-1];
	}
	player.Room_Buff[0]=m;
	return m;
}
//====================================================
#define  M5_RoomVoice	50
const char* m5_VoiceNames[]={
	"Pororo\\m05\\voice\\H1.drm",
	"Pororo\\m05\\voice\\H2.drm",
	"Pororo\\m05\\voice\\H3.drm",
	"Pororo\\m05\\voice\\H4.drm",
	"Pororo\\m05\\voice\\H5.drm",
	"Pororo\\m05\\voice\\H6.drm",
	"Pororo\\m05\\voice\\H7.drm",
	"Pororo\\m05\\voice\\H8.drm",
//8
	"Pororo\\m05\\voice\\DA1.drm",
	"Pororo\\m05\\voice\\DA2.drm",
	"Pororo\\m05\\voice\\DA3.drm",
	"Pororo\\m05\\voice\\DA4.drm",
	"Pororo\\m05\\voice\\DA5.drm",
	"Pororo\\m05\\voice\\DA6.drm",
//14
	"Pororo\\m05\\voice\\DB1.drm",
	"Pororo\\m05\\voice\\DB2.drm",
	"Pororo\\m05\\voice\\DB3.drm",
	"Pororo\\m05\\voice\\DB4.drm",
	"Pororo\\m05\\voice\\DB5.drm",
//19
	"Pororo\\m05\\voice\\B1.drm",
	"Pororo\\m05\\voice\\B2.drm",
	"Pororo\\m05\\voice\\B3.drm",
	"Pororo\\m05\\voice\\B4.drm",
	"Pororo\\m05\\voice\\B5.drm",
//24
	"Pororo\\m05\\voice\\Explosion_EFX.drm",
	"Pororo\\m05\\voice\\VC_absence.drm",
	"Pororo\\m05\\voice\\VC_bell.drm",
	"Pororo\\m05\\voice\\Wave_EFX.drm",
	
};

const char* m5_VoiceROOM1[]={
	"Pororo\\m05\\voice\\ROOM1_N1.drm",
	"Pororo\\m05\\voice\\ROOM1_N2.drm",
	"Pororo\\m05\\voice\\ROOM1_N3.drm",
	"Pororo\\m05\\voice\\ROOM1_N4.drm",
	"Pororo\\m05\\voice\\ROOM1_N5.drm",
	"Pororo\\m05\\voice\\ROOM1_N6.drm",
	"Pororo\\m05\\voice\\ROOM1_N7.drm",
	"Pororo\\m05\\voice\\ROOM1_N8.drm",
	"Pororo\\m05\\voice\\ROOM1_N9.drm",
	"Pororo\\m05\\voice\\ROOM1_N10.drm",
	"Pororo\\m05\\voice\\ROOM1_N11.drm",

};

const char* m5_VoiceROOM2[]={
	"Pororo\\m05\\voice\\ROOM_N1.drm",
	"Pororo\\m05\\voice\\ROOM2_N2.drm",
	"Pororo\\m05\\voice\\ROOM2_N3.drm",
	"Pororo\\m05\\voice\\ROOM2_N4.drm",
	"Pororo\\m05\\voice\\ROOM2_N5.drm",
	"Pororo\\m05\\voice\\ROOM2_N6.drm",
	"Pororo\\m05\\voice\\ROOM2_N7.drm",
	"Pororo\\m05\\voice\\ROOM2_N8.drm",
	"Pororo\\m05\\voice\\ROOM2_N9.drm",
	"Pororo\\m05\\voice\\ROOM2_N10.drm",
	"Pororo\\m05\\voice\\ROOM2_N11.drm",
};
const char* m5_VoiceCAFE[]={
	"Pororo\\m05\\voice\\CAFE_N1.drm",
	"Pororo\\m05\\voice\\CAFE_N2.drm",
	"Pororo\\m05\\voice\\CAFE_N3.drm",
	"Pororo\\m05\\voice\\CAFE_N4.drm",
	"Pororo\\m05\\voice\\CAFE_N5.drm",
	"Pororo\\m05\\voice\\CAFE_N6.drm",
	"Pororo\\m05\\voice\\CAFE_N7.drm",
};
const char* m5_VoiceVC_BEACH[]={
	"Pororo\\m05\\voice\\VC_BEACH_N1.drm",
	"Pororo\\m05\\voice\\VC_BEACH_N2.drm",
	"Pororo\\m05\\voice\\VC_BEACH_N3.drm",
	"Pororo\\m05\\voice\\VC_BEACH_N4.drm",
	"Pororo\\m05\\voice\\VC_BEACH_N5.drm",

	"Pororo\\m05\\voice\\VIOLIN_N2_bach.drm",
};

const char* m5_VoiceKITCHEN[]={
	"Pororo\\m05\\voice\\KITCHEN_N1.drm",
	"Pororo\\m05\\voice\\KITCHEN_N2.drm",
	"Pororo\\m05\\voice\\KITCHEN_N3.drm",
	"Pororo\\m05\\voice\\KITCHEN_N4.drm",
	"Pororo\\m05\\voice\\KITCHEN_N5.drm",
	"Pororo\\m05\\voice\\KITCHEN_N6.drm",

	"Pororo\\m05\\voice\\KITCHEN_N3_oven.drm",
	"Pororo\\m05\\voice\\KITCHEN_N4_oven.drm",
	"Pororo\\m05\\voice\\KITCHEN_N4_oven_explosion.drm",
	"Pororo\\m05\\voice\\KITCHEN_oven.drm",
};

const char* m5_VoicePARTY[]={
	"Pororo\\m05\\voice\\PARTY_N1.drm",
	"Pororo\\m05\\voice\\PARTY_N2.drm",
	"Pororo\\m05\\voice\\PARTY_N3.drm",
	"Pororo\\m05\\voice\\PARTY_N4.drm",
	"Pororo\\m05\\voice\\PARTY_N5.drm",

	//"Pororo\\m05\\voice\\PARTY_SONG.drm",
	"Pororo\\m05\\voice\\PARTY_N2_SONG.drm",
};

const char* m5_VoiceSHOP[]={
	"Pororo\\m05\\voice\\SHOP_N1.drm",
	"Pororo\\m05\\voice\\SHOP_N2.drm",
	"Pororo\\m05\\voice\\SHOP_N3.drm",
	"Pororo\\m05\\voice\\SHOP_N4.drm",
	"Pororo\\m05\\voice\\SHOP_N5.drm",
	"Pororo\\m05\\voice\\SHOP_N6.drm",
};

const char* m5_VoiceVIOLIN[]={
	"Pororo\\m05\\voice\\VIOLIN_N1.drm",
	"Pororo\\m05\\voice\\VIOLIN_N2.drm",
	"Pororo\\m05\\voice\\VIOLIN_N3.drm",
	"Pororo\\m05\\voice\\VIOLIN_N4.drm",
	"Pororo\\m05\\voice\\VIOLIN_N5.drm",

	"Pororo\\m05\\voice\\VIOLIN_N2_bach.drm",
};
const INT16S m5VoiceNum[]={
	11,11,7,6,10,6,6,6
};
void LoadM5Voice(INT16S page)
{
	if(page==0) LoadDRM(0,m5VoiceNum[page],SpriteCellData+1200*_1K,(int*)&DrmAddr[M5_RoomVoice],m5_VoiceROOM1);
	else if(page==1) LoadDRM(0,m5VoiceNum[page],SpriteCellData+1200*_1K,(int*)&DrmAddr[M5_RoomVoice],m5_VoiceROOM2);
	else if(page==2) LoadDRM(0,m5VoiceNum[page],SpriteCellData+1200*_1K,(int*)&DrmAddr[M5_RoomVoice],m5_VoiceCAFE);
	else if(page==3) LoadDRM(0,m5VoiceNum[page],SpriteCellData+1200*_1K,(int*)&DrmAddr[M5_RoomVoice],m5_VoiceVC_BEACH);
	else if(page==4) LoadDRM(0,m5VoiceNum[page],SpriteCellData+1200*_1K,(int*)&DrmAddr[M5_RoomVoice],m5_VoiceKITCHEN);
	else if(page==5) LoadDRM(0,m5VoiceNum[page],SpriteCellData+1200*_1K,(int*)&DrmAddr[M5_RoomVoice],m5_VoicePARTY);
	else if(page==6) LoadDRM(0,m5VoiceNum[page],SpriteCellData+1200*_1K,(int*)&DrmAddr[M5_RoomVoice],m5_VoiceSHOP);
	else LoadDRM(0,m5VoiceNum[page],SpriteCellData+1200*_1K,(int*)&DrmAddr[M5_RoomVoice],m5_VoiceVIOLIN);
}
void set_Miss(INT16S flag)
{
	if(flag==0){
		Call_Time=60*3;
		player.Miss_User++;
		player.Miss_Mimi=0;
	}else{
		player.Miss_Mimi++;
		player.Miss_User=0;
	}
}
INT16S TestMimiCall()
{
	if(OS_game==15) return 0;
	if(MimiCall){
		if(OS_game==11 && step>=10 && step<30){
			MimiCall=0;
			set_Miss(0);
			return 0;
		}else if(OS_game==12 && step>=10){
			MimiCall=0;
			set_Miss(0);
			return 0;
		}else if(OS_game==16 && step>=10){
			MimiCall=0;
			set_Miss(0);
			return 0;
		}else if(OS_game==13 && ((step>=20 && step<30) || step==250 || (step>=60 && step<70)  || step==660) ){
			MimiCall=0;
			set_Miss(0);
			return 0;
		}else if(OS_game==17 && Game_State>=52){
			MimiCall=0;
			set_Miss(0);
			return 0;
		}else if(OS_game==18 && Game_State>=10 && Game_State<50){
			MimiCall=0;
			set_Miss(0);
			return 0;
		}else if(OS_game==19 && Game_State>=12 && Game_State<50){
			MimiCall=0;
			set_Miss(0);
			return 0;
		}else return 1;
	}
}

void GetMimiCall()
{	
	if(MimiCall || OS_game==15 || Call_Times>=2) return;
	if(Call_Times==0){
		if(Call_Time==0) Call_Time=60*5+GetRandom(60*5);
		Call_Time--;
		if(Call_Time==0){
			Call_Time=60*15;
			MimiCall=1;
			Call_Times++;
			TestMimiCall();
		}
	}else if(player.Miss_User){
		Call_Time--;
		if(Call_Time==0){
			MimiCall=1;
			Call_Times++;
			TestMimiCall();
		}
	}else if(Call_Times==1){
		Call_Time--;
		if(Call_Time==0){
			Call_Time=60*3;
			MimiCall=1;
			Call_Times++;
			TestMimiCall();
		}
	}
}
//===================================================================
const char* m5_Names[]={
	"Pororo\\m05\\M5_open_guide_p1.gif",
	"Pororo\\m05\\M5_open_guide_p2.gif",
	"Pororo\\m05\\M5_open_guide_p3.gif",
	"Pororo\\m05\\M5_open_guide_p4.gif",
	"Pororo\\m05\\M5_open_guide_p5.gif",
	"Pororo\\m05\\M5_open_guide_p6.gif",
//6
	"Pororo\\m05\\M5_absence-1.gif",
	"Pororo\\m05\\M5_absence-2.gif",
//8	
	"Pororo\\m05\\bell\\M5_bell-1.gif",
	"Pororo\\m05\\bell\\M5_bell-2.gif",
	"Pororo\\m05\\bell\\M5_bell-3.gif",
	"Pororo\\m05\\bell\\M5_bell-4.gif",
	"Pororo\\m05\\bell\\M5_bell-5.gif",
	"Pororo\\m05\\bell\\M5_bell-6.gif",
//14	
};
const char* m5_PathDir[]={
	"Pororo\\m05\\3-1.ROOM1\\",
	"Pororo\\m05\\3-2.ROOM2\\",
	"Pororo\\m05\\3-3.CAFE\\",
	"Pororo\\m05\\3-4.BEACH\\",
	"Pororo\\m05\\3-5.KITCHEN\\",
	"Pororo\\m05\\3-6.PARTY\\",
	"Pororo\\m05\\3-7.SHOP\\",
	"Pororo\\m05\\3-8.VIOLIN\\",
};
const INT8S m5_ROOM1Num[]={
	3,5,7,3,3,5,7,9,7,
	5,6,7,5,5,9,7,5,5,3
};
//H0  	DA8-1 		DB14-1 		B19-1
const INT8S m5_ROOM1Call[]={
//L1 L2 
	9,-1,	10,-1,
//Miss 2
	12,7,	11,6,
//start		4
	0,0,	0,1,	2,2,	0,3,	0,0,
	0,0,	0,1,	1,4,	1,5,	0,0,
//DA	14
	5,8,	18,9,	4,10,	18,11,	18,12,	3,13,	18,9,	//7
//DB	21
	4,14,	18,15,	4,16,	18,17,	3,18,	4,16,		//6
//END	27
	6,19,	6,20,	8,21,	6,22,	7,23,		//5
//ABC	32	
	11,50,	13,51,	5,52,	15,53,	5,54,	
	16,55,	17,56,	4,57,	18,58,	4,59,	14,60,		//11
};
const char* m5_ROOM1[]={
	"VC_ROOM1_H1",
	"VC_ROOM1_H2",
	"VC_ROOM1_H3",
	"VC_ROOM1_D1",
	"VC_ROOM1_D2",
	"VC_ROOM1_D3",
	"VC_ROOM1_B1",
	"VC_ROOM1_B2",
	"VC_ROOM1_B3",
//9
	"VC_ROOM1_L1",
	"VC_ROOM1_L2",
//11	
	"VC_ROOM1_N1",
	"VC_ROOM1_N2",
	"VC_ROOM1_N3",
	"VC_ROOM1_N4",
	"VC_ROOM1_N5",
	"VC_ROOM1_N6",
	"VC_ROOM1_N7",
	"VC_ROOM1_N8",
};
const INT8S m5_ROOM2Num[]={
	3,5,7,3,3,5,7,9,7,
	5,6,7,5,7,9,7,5,5,3
};
//H0  	DA8-1 		DB14-1 		B19-1
const INT8S m5_ROOM2Call[]={
//L1 L2 
	9,-1,	10,-1,
//Miss 2
	12,7,	11,6,
//start		4
	0,0,	0,1,	2,2,	0,3,	0,0,
	0,0,	0,1,	1,4,	1,5,	0,0,
//DA	14
	5,8,	18,9,	4,10,	18,11,	18,12,	3,13,	18,9,	//7
//DB	21
	4,14,	18,15,	4,16,	18,17,	3,18,	4,16,		//6
//END	27
	6,19,	6,20,	8,21,	6,22,	7,23,		//5
//ABC	32	
	12,50,	13,51,	5,52,	15,53,	5,54,	
	16,55,	17,56,	4,57,	18,58,	4,59,	14,60,		//11
};
const char* m5_ROOM2[]={
	"VC_ROOM2_H1",
	"VC_ROOM2_H2",
	"VC_ROOM2_H3",
	"VC_ROOM2_D1",
	"VC_ROOM2_D2",
	"VC_ROOM2_D3",
	"VC_ROOM2_B1",
	"VC_ROOM2_B2",
	"VC_ROOM2_B3",
//9
	"VC_ROOM2_L1",
	"VC_ROOM2_L2",
//11	
	"VC_ROOM2_N1",
	"VC_ROOM2_N2",
	"VC_ROOM2_N3",
	"VC_ROOM2_N4",
	"VC_ROOM2_N5",
	"VC_ROOM2_N6",
	"VC_ROOM2_N7",
	"VC_ROOM2_N8",
};

const INT8S m5_CAFENum[]={
	3,5,7,3,3,5,7,9,
	5,6,7,8,5,6,7,7,3
};
//H0  	DA8-1 		DB14-1 		B19-1
const INT8S m5_CAFECall[]={
//L1 L2 
	8,-1,	9,-1,
//Miss 2
	2,7,	1,6,
//start		4
	0,0,	0,1,	2,2,	0,3,	0,0,
	0,0,	0,1,	1,4,	1,5,	0,0,
//DA	14
	5,8,	3,9,	5,10,	4,11,	4,12,	3,13,	3,9,	//7
//DB	21
	4,14,	4,15,	5,16,	3,17,	3,18,	5,16,		//6
//END	27
	6,19,	6,20,	6,21,	6,22,	7,23,		//5
//ABC	32	
	10,50,	3,51,	11,52,		//3
	13,53,	14,54,				//2
	15,55,	12,56,				//2
};
const char* m5_CAFE[]={
	"VC_CAFE_H1",
	"VC_CAFE_H2",
	"VC_CAFE_H3",
	"VC_CAFE_D1",
	"VC_CAFE_D2",
	"VC_CAFE_D3",
	"VC_CAFE_B1",
	"VC_CAFE_B2",
//8
	"VC_CAFE_L1",
	"VC_CAFE_L2",
//10	
	"VC_CAFE_N1",
	"VC_CAFE_N2",
	"VC_CAFE_N3",
	"VC_CAFE_N4",
	"VC_CAFE_N5",
	"VC_CAFE_N6",
};
const INT8S m5_BEACHNum[]={
	3,5,7,3,3,5,7,9,
	4,6,3,6,5,5
};
//H0  	DA8-1 		DB14-1 		B19-1
const INT8S m5_BEACHCall[]={
//L1 L2 
	8,-1,	9,-1,
//Miss 2
	2,7,	1,6,
//start		4
	0,0,	0,1,	2,2,	0,3,	0,0,
	0,0,	0,1,	1,4,	1,5,	0,0,
//DA	14
	13,8,	3,9,	12,10,	4,11,	4,12,	3,13,	3,9,	//7
//DB	21
	12,14,	4,15,	13,16,	3,17,	3,18,	13,16,		//6
//END	27
	6,19,	6,20,	6,21,	6,22,	7,23,		//5
//ABC	32	
	13,50,	5,51,	//3
	10,52,	11,53,	12,54,				//2
};
const char* m5_BEACH[]={
	"VC_BEACH_H1",
	"VC_BEACH_H2",
	"VC_BEACH_H3",
	"VC_BEACH_D1",
	"VC_BEACH_D2",
	"VC_BEACH_D3",
	"VC_BEACH_B1",
	"VC_BEACH_B2",
//8
	"VC_BEACH_L1",
	"VC_BEACH_L2",
	"VC_BEACH_N1",
	"VC_BEACH_N2",
	"VC_BEACH_N3",
	"VC_BEACH_N4",
};
const INT8S m5_KITCHENNum[]={
	3,5,7,3,3,5,7,9,
	4,6,13,13,6
};
//H0  	DA8-1 		DB14-1 		B19-1
const INT8S m5_KITCHENCall[]={
//L1 L2 
	8,-1,	9,-1,
//Miss 2
	2,7,	1,6,
//start		4
	0,0,	0,1,	2,2,	0,3,	0,0,
	0,0,	0,1,	1,4,	1,5,	0,0,
//DA	14
	12,8,	3,9,	3,10,	3,11,	4,12,	4,13,	3,9,	//7
//DB	21
	3,14,	3,15,	3,16,	4,17,	4,18,	3,16,		//6
//END	27
	6,19,	6,20,	6,21,	6,22,	7,23,		//5
//ABC	32	
	5,50,	3,51,		//2
	10,52,	11,53,		//3
	12,54,	9,55		//2
};
const char* m5_KITCHEN[]={
	"VC_KITCHEN_H1",
	"VC_KITCHEN_H2",
	"VC_KITCHEN_H3",
	"VC_KITCHEN_D1",
	"VC_KITCHEN_D2",
	"VC_KITCHEN_D3",
	"VC_KITCHEN_B1",
	"VC_KITCHEN_B2",
//8
	"VC_KITCHEN_L1",
	"VC_KITCHEN_L2",
	"VC_KITCHEN_N1",
	"VC_KITCHEN_N2",
	"VC_KITCHEN_N3",
};
const INT8S m5_PARTYNum[]={
	3,5,7,3,3,5,7,9,
	4,6,35,3,3
};
//H0  	DA8-1 		DB14-1 		B19-1
const INT8S m5_PARTYCall[]={
//L1 L2 
	8,-1,	9,-1,
//Miss 2
	2,7,	1,6,
//start		4
	0,0,	0,1,	2,2,	0,3,	0,0,
	0,0,	0,1,	1,4,	1,5,	0,0,
//DA	14
	5,8,	3,9,	11,10,	12,11,	3,12,	3,13,	3,9,	//7
//DB	21
	12,14,	3,15,	12,16,	3,17,	4,18,	12,16,		//6
//END	27
	6,19,	6,20,	6,21,	6,22,	7,23,		//5
//ABC	32	
	4,50,						//1
	10,51,	5,52,	11,53,	12,54,	//4
};

const char* m5_PARTY[]={
	"VC_PARTY_H1",
	"VC_PARTY_H2",
	"VC_PARTY_H3",
	"VC_PARTY_D1",
	"VC_PARTY_D2",
	"VC_PARTY_D3",
	"VC_PARTY_B1",
	"VC_PARTY_B2",
//8
	"VC_PARTY_L1",
	"VC_PARTY_L2",
	"VC_PARTY_N1",
	"VC_PARTY_N2",
	"VC_PARTY_N3",
};
const INT8S m5_SHOPNum[]={
	3,5,7,3,3,5,7,8,
	4,6,3,5,5,3
};
//H0  	DA8-1 		DB14-1 		B19-1
const INT8S m5_SHOPCall[]={
//L1 L2 
	8,-1,	9,-1,
//Miss 2
	2,7,	1,6,
//start		4
	0,0,	0,1,	2,2,	0,3,	0,0,
	0,0,	0,1,	1,4,	1,5,	0,0,
//DA	14
	5,8,	3,9,	13,10,	4,11,	4,12,	4,13,	3,9,	//7
//DB	21
	13,14,	3,15,	13,16,	3,17,	4,18,	13,16,		//6
//END	27
	6,19,	6,20,	6,21,	6,22,	7,23,		//5
//ABC	32	
	13,50,						//1
	10,51,	11,52,	12,53,	13,54,	3,55,//5
};

const char* m5_SHOP[]={
	"VC_SHOP_H1",
	"VC_SHOP_H2",
	"VC_SHOP_H3",
	"VC_SHOP_D1",
	"VC_SHOP_D2",
	"VC_SHOP_D3",
	"VC_SHOP_B1",
	"VC_SHOP_B2",
//8
	"VC_SHOP_L1",
	"VC_SHOP_L2",
	"VC_SHOP_N1",
	"VC_SHOP_N2",
	"VC_SHOP_N3",
	"VC_SHOP_N4",
};
const INT8S m5_VIOLINNum[]={
	3,5,7,3,3,5,7,9,
	4,6,63,5,3,4
};
//H0  	DA8-1 		DB14-1 		B19-1
const INT8S m5_VIOLINCall[]={
//L1 L2 
	8,-1,	9,-1,
//Miss 2
	2,7,	1,6,
//start		4
	0,0,	0,1,	2,2,	0,3,	0,0,
	0,0,	0,1,	1,4,	1,5,	0,0,
//DA	14
	5,8,	11,9,	3,10,	12,11,	12,12,	4,13,	11,9,	//7
//DB	21
	3,14,	12,15,	13,16,	12,17,	4,18,	13,16,		//6
//END	27
	6,19,	6,20,	6,21,	6,22,	7,23,		//5
//ABC	32	
	3,50,						//1
	10,51,	11,52,	12,53,	13,54,//4
};
const char* m5_VIOLIN[]={
	"VC_VIOLIN_H1",
	"VC_VIOLIN_H2",
	"VC_VIOLIN_H3",
	"VC_VIOLIN_D1",
	"VC_VIOLIN_D2",
	"VC_VIOLIN_D3",
	"VC_VIOLIN_B1",
	"VC_VIOLIN_B2",
//8	
	"VC_VIOLIN_L1",
	"VC_VIOLIN_L2",
//10	
	"VC_VIOLIN_N1",
	"VC_VIOLIN_N2",
	"VC_VIOLIN_N3",
	"VC_VIOLIN_N4",
};

void View_Call()	//Í¼Ïñ Half 
{
	INT16S* sen;
	INT16S* bmp;
	INT16S* tv;
	int i,j,n,nn,k=0;
	
	sen=(INT16S*)ScalerBuff;		//320x240 RGB data
	tv=(INT16S*)TVOutBuff;		//240x320 TV buff

	for(i=0;i<240;i++){
		n=i*320;
		for(j=0;j<160;j++){
			*(tv+n+160+j)=*(sen+n+80+j);
		}
	}
}
INT16S CallRandom=0;
INT16S CallGetRandom(INT16S num)	//GetRandom
{
	return CallRandom%num;
}
INT16S CallAct,CallSpeech,CallNum,CallTime;
INT16S Call_ABC=0;
void GetCallAct(INT16S page,INT16S steps)
{
	INT16S m;
	if(steps==0){				//Start
		if(player.Miss_User){
			player.Miss_User=0;
			m=2;
		}else if(player.Miss_Mimi) {
			player.Miss_Mimi=0;
			m=3;
		}else if(WhoCall==0) m=4+GetRandom(5);
		else m=9+GetRandom(5);
		//m=2+GetRandom(12);
	}else if(steps==1 || steps==3 ||steps==5 ||steps==7 ||steps==9 ||steps==11){ //L1 L2
		m=GetRandom(2);
	}else if(steps==2){		//A
		if(page==0 || page==1 ) {
			Call_ABC=GetRandom(11);
			m=32+Call_ABC;
		}else if(page==2) m=32+GetRandom(3);
		else if(page==3) m=32+GetRandom(2);
		else if(page==4) m=32+GetRandom(2);
		else if(page==5) m=32;
		else if(page==6) m=32;
		else if(page==7) m=32;
	}else if(steps==4){		//DA
		m=14+GetRandom(7);
	}else if(steps==6){		//B
		if(page==0 || page==1 ) {
			Call_ABC+=1+GetRandom(4);
			m=32+Call_ABC%11;
		}else if(page==2) m=35+GetRandom(2);
		else if(page==3) {
			Call_ABC=GetRandom(3);
			m=34+Call_ABC;
		}else if(page==4) m=34+GetRandom(2);
		else if(page==5) {
			Call_ABC=GetRandom(4);
			m=33+Call_ABC;
		}else if(page==6) {
			Call_ABC=GetRandom(5);
			m=33+Call_ABC;
		}else if(page==7) {
			Call_ABC=GetRandom(4);
			m=33+Call_ABC;
		}
	}else if(steps==8){		//C
		if(page==0 || page==1 ) { 
			Call_ABC+=1+GetRandom(4);
			m=32+Call_ABC%11;
		}else if(page==2) m=37+GetRandom(2);
		else if(page==3) {
			Call_ABC+=1+GetRandom(2);
			m=34+Call_ABC%3;
		}else if(page==4) m=36+GetRandom(2);
		else if(page==5) {
			Call_ABC+=1+GetRandom(3);
			m=33+Call_ABC%4;
		}else if(page==6) {
			Call_ABC+=1+GetRandom(4);
			m=33+Call_ABC%5;
		}else if(page==7) {
			Call_ABC+=1+GetRandom(3);
			m=33+Call_ABC%4;
		}
	}else if(steps==10){	//DB
		m=21+GetRandom(6);
	}else if(steps==12){	//END
		m=27+GetRandom(5);
	}
	if(page==0){
		CallAct=m5_ROOM1Call[2*m];
		CallSpeech=m5_ROOM1Call[2*m+1];
		CallNum=m5_ROOM1Num[CallAct];
	}else if(page==1){
		CallAct=m5_ROOM2Call[2*m];
		CallSpeech=m5_ROOM2Call[2*m+1];
		CallNum=m5_ROOM2Num[CallAct];
	}else if(page==2){
		CallAct=m5_CAFECall[2*m];
		CallSpeech=m5_CAFECall[2*m+1];
		CallNum=m5_CAFENum[CallAct];
	}else if(page==3){
		CallAct=m5_BEACHCall[2*m];
		CallSpeech=m5_BEACHCall[2*m+1];
		CallNum=m5_BEACHNum[CallAct];
	}else if(page==4){
		CallAct=m5_KITCHENCall[2*m];
		CallSpeech=m5_KITCHENCall[2*m+1];
		CallNum=m5_KITCHENNum[CallAct];
	}else if(page==5){
		CallAct=m5_PARTYCall[2*m];
		CallSpeech=m5_PARTYCall[2*m+1];
		CallNum=m5_PARTYNum[CallAct];
	}else if(page==6){
		CallAct=m5_SHOPCall[2*m];
		CallSpeech=m5_SHOPCall[2*m+1];
		CallNum=m5_SHOPNum[CallAct];
	}else if(page==7){
		CallAct=m5_VIOLINCall[2*m];
		CallSpeech=m5_VIOLINCall[2*m+1];
		CallNum=m5_VIOLINNum[CallAct];
	}
	if(CallSpeech!=-1) {
		if(page==3 && CallSpeech==52){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[CallSpeech], 64, 127, SPEECHCHANL);
			//SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[55], 64, 127, MUSIC_CHCHANL);
		}else if(page==5 && CallSpeech==50){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[CallSpeech], 64, 127, SPEECHCHANL);
			//SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[55], 64, 127, MUSIC_CHCHANL);
		}else if(page==5 && CallSpeech==51){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[55], 64, 127, MUSIC_CHCHANL);
		}else if(page==7 && CallSpeech==51){
			SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[55], 64, 127, MUSIC_CHCHANL);
		}else SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[CallSpeech], 64, 127, SPEECHCHANL);
	}
	CallTime=0;
}
INT16S Show_M5_Gif(INT16S page)
{
	char path[100];
	INT16S* sen;
	INT16S* tv;
	int i,j,n,nn,k=0;

	sen=(INT16S*)VideoBuff;		//320x240 RGB data
	tv=(INT16S*)TVOutBuff;		//240x320 TV buff

	if(CallTime>=CallNum) return 1;
	k=CallTime;
	
	if(page==0){
		sprintf((char*)path,"%s%s\\%s-%d.gif",m5_PathDir[page],m5_ROOM1[CallAct],m5_ROOM1[CallAct],1+k);
	}else if(page==1){
		sprintf((char*)path,"%s%s\\%s-%d.gif",m5_PathDir[page],m5_ROOM2[CallAct],m5_ROOM2[CallAct],1+k);
	}else if(page==2){
		sprintf((char*)path,"%s%s\\%s-%d.gif",m5_PathDir[page],m5_CAFE[CallAct],m5_CAFE[CallAct],1+k);
	}else if(page==3){
		sprintf((char*)path,"%s%s\\%s-%d.gif",m5_PathDir[page],m5_BEACH[CallAct],m5_BEACH[CallAct],1+k);
	}else if(page==4){
		sprintf((char*)path,"%s%s\\%s-%d.gif",m5_PathDir[page],m5_KITCHEN[CallAct],m5_KITCHEN[CallAct],1+k);
	}else if(page==5){
		sprintf((char*)path,"%s%s\\%s-%d.gif",m5_PathDir[page],m5_PARTY[CallAct],m5_PARTY[CallAct],1+k);
	}else if(page==6){
		sprintf((char*)path,"%s%s\\%s-%d.gif",m5_PathDir[page],m5_SHOP[CallAct],m5_SHOP[CallAct],1+k);
	}else if(page==7){
		sprintf((char*)path,"%s%s\\%s-%d.gif",m5_PathDir[page],m5_VIOLIN[CallAct],m5_VIOLIN[CallAct],1+k);
	}
	AddRootPath(path);
	if(Photo_decode_wh(rootPath,160,240,160,240,VideoBuff)>=0){
		for(i=0;i<240;i++){
			n=i*320;
			nn=i*160;
			for(j=0;j<160;j++){
				*(tv+n+j)=*(sen+nn+j);
			}
		}
	}
	CallTime++;
	if(CallTime>=CallNum) return 1;
	return 0;
}

void main_m05()
{	
	INT16S i,m;
	INT16S Calltime;
	INT16S place=0,steps,wait_music;
	INT16S place_sel=0;
	INT16S Wait_L1L2=0;
	
	spMalloc(300+500+400+900);
	sensorMalloc2();
	
	INIT_VideoPub(1200);
	Palette_ram_cpy( 1,0, M05_SPRITE_Palette1+256*0	,256);	
	LoadData_1(1,SpriteCellData,"Pororo\\m05\\SpriteCell_cmp.BIN");
	LoadDRM(0,28,SpriteCellData+800*_1K,(int*)&DrmAddr[0],m5_VoiceNames);
	
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	LoadTextBmp(C_PPU_TEXT1,"Pororo\\menu\\bg\\bg1.gif");
	//Display_Background(0);
	clear_sprite(Spritebuf_number);
	gplib_ppu_fade_effect_set(ppu_register_set, 0);

	if(MimiCall==0){
		step=0;
		WhoCall=0;
		Calltime=(75+GetRandom(4*25));
		if(player.Miss_User && Call_Times==1) Call_Times++;
	}else{
		MimiCall=0;
		WhoCall=1;
		step=10;
		Calltime=250;
	}
	game_time=0;
	while(1)
	{
		switch(step){
//--------------------------------------------------menu
			case 0:
				clear_sprite(Spritebuf_number);
				set_sprite_display_init(0,0,0,M5_Open_Frame[0]);
				SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[MENU_OPEN_SFX], 64, 127, SFX_CHANL);
				game_time=0;
				step++;
			break;
			case 1:
				#define Tile_Time	3
				if(game_time%Tile_Time==0 && game_time<6*Tile_Time){
					set_sprite_display_init(0,0,0,M5_Open_Frame[game_time/Tile_Time]);
				}else if(game_time>=Tile_Time*6+10){
					clear_sprite(Spritebuf_number);
					LoadTextBmp(C_PPU_TEXT1,m5_Names[8+0]);
					game_time=0;
					step=10;
				}
			break;

			case 10:
				if(SPU_Get_SingleChannel_Status(SPEECHCHANL)==0){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[26], 64, 60, SPEECHCHANL);
				}
				if(game_time%Tile_Time==0){
					LoadTextBmp(C_PPU_TEXT1,m5_Names[8+game_time/Tile_Time%6]);
				}
				if(WhoCall && KeyBoardCode==K_Ok){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					step=20;
				}else if(KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(WhoCall) set_Miss(0);
					step=100;
				}else if(game_time>=Calltime){
					game_time=0;
					if(WhoCall==0){
						if(Place_old>0 && GetRandom(6)==3){	//3
							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[25], 64, 80, SPEECHCHANL);
							Place_old=-1;
							set_Miss(1);
							step++;
						}else{
							SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
							step=20;
						}
					}else{
						set_Miss(0);
						step=100;
					}
				}/*else if(KeyBoardCode>=K_N1 && KeyBoardCode<=K_N8){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
					if(place_sel==0){
						place=KeyBoardCode-K_N1;
						CallRandom=0;
					}else CallRandom=KeyBoardCode-K_N1;
					place_sel=(place_sel+1)%2;
				}//*/
			break;
			case 11:
				if(game_time%Tile_Time==0){
					LoadTextBmp(C_PPU_TEXT1,m5_Names[6+game_time/Tile_Time%2]);
				}else if(game_time>=50){
					step=100;
					CallRandom=0;
				}
			break;

//========================================================
			case 20:
				SPU_StopChannel(SPEECHCHANL);
				clear_sprite(Spritebuf_number);
				InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,BMP_RGB565);
				set_sprite_display_init(0,0,0,M5_UiSp_Frame[0]);
				Change_SensorMode(0);
				place=getRoom();
				Place_old=place;
				LoadM5Voice(place);
				//CallRandom=0;
				steps=0;
				step++;
			break;
			case 21:
				sensor_view(1000);
				GetCallAct(place,steps);
				Show_M5_Gif(place);
				Wait_L1L2=0;
				wait_music=0;
				step++;
			break;
			case 22:
				sensor_view(1000);
				if(Wait_L1L2) Wait_L1L2--;
				else{
					m=Show_M5_Gif(place);
					wait_music+=m;
					if(steps%2==1) Wait_L1L2=6;
					else Wait_L1L2=2;
				}
				if(Wait_L1L2==0 && wait_music && SPU_Get_SingleChannel_Status(SPEECHCHANL)==0){
					if(SPU_Get_SingleChannel_Status(MUSIC_CHCHANL)==0 ){
						steps++;
						if(steps>12) {
							Close_Sensor();
							clear_sprite(Spritebuf_number);
							game_time=0;
							step=100;
							CallRandom++;
							place_sel=0;
							steps=0;
						}else step--;
						break;
					}else if(m){
						CallTime=5;
					}
				}
				set_sprite_display_init(1,0,0,M5_UiSp_Frame[1+game_time/5%3]);
				if(KeyBoardCode==K_Ok || KeyBoardCode==K_Return || KeyBoardCode==K_Back){
					SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[OK_SFX], 64, 80, 12);
					step=100;
				}
			break;
		}
		if(Goto_desktop()) {
			if(WhoCall && step==10) set_Miss(0);
			step=100;
		}
		PaintTFT_cell4();
		if(step==100) break;
	}
	SPU_StopChannel(SPEECHCHANL);
	sensorFree();
	Close_Sensor();
}

