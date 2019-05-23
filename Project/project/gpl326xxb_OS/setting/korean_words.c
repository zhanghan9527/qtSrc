#include "application.h"
#include "data.h"
#include "math.h"
#include "Sprite_demo.h"
#include "drv_l1_ppu.h"
#include "words_sp.h"
#include "words32_sp.h"

extern const INT16U menu_SPRITE_Palette1[];
extern const INT16U words_SPRITE_Palette1[];

extern INT32U Word_Kb_Num;

INT16S Keyboard=0;


#define Wirte_Max	200

INT16S writeing_max,writeing_Words[Wirte_Max];
INT16S Words_max=0,Words_pos=0;
INT8U ABC[6],ABC_idx=0;
INT16S abc=0;

//======================================================
const INT16U ALIGN4 words_SPRITE_Palette1[]={
	0xFFFF, 0x0000, 0x8018, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318,
};

extern INT8U Korean_Data[];
#define KOREA_MAX	2354
void Words_B_20()
{
	int i,j;

	for(j=0;j<KOREA_MAX;j++){
		if(Korean_Data[5*j+1]) Korean_Data[5*j+1]+=20;
		if(Korean_Data[5*j+2]) Korean_Data[5*j+2]+=20;
	}
}
INT16S getWords(INT8U* abc)
{
	int i,j,n=0;
	INT8U m,k;
	
	if(*(abc+0)>20) return 0;
	for(j=0;j<KOREA_MAX;j++){
		for(i=0,k=0;i<5;i++){
			m=Korean_Data[5*j+i];
			if(m==0);
			else if(*(abc+k)>m) break;
			else if(*(abc+k)<m){
				if(i<3) return 0;
				else break;
			}else k++;
		}
		if((k==5) || (i==5 && *(abc+k)==0) ){
			n=1+j;
			break;
		}
	}
	if(n) return 1000+n;
	else return 0;
}

void show_word(INT16S sp,INT16S x,INT16S y,INT16S no)
{
	if(no>=1000 && no<1001+KOREA_MAX){		//韩文 1000=space 1001begin
		set_sprite_display_init(sp,x,y,words_sp_Frame[0]);
		set_sprite_nCharNum(sp,Word_Kb_Num*16+no-1000);
	}else if(no>=221 && no<221+16){				//korea char  yuan
		set_sprite_display_init(sp,x,y,Korea_char_Frame[no-202]);
		set_sprite_nCharNum(sp,Word_Kb_Num*16);
	}else if(no>=200 && no<221){				//korea char	fu
		set_sprite_display_init(sp,x,y,Korea_char_Frame[no-201]);
		set_sprite_nCharNum(sp,Word_Kb_Num*16);
	}else if(no>0x20 && no<0x80){				//ascii 0x20--0x7f
		set_sprite_display_init(sp,x,y,chars_sp_Frame[0]);
		set_sprite_nCharNum(sp,Word_Kb_Num*16+no-0x20);
	}else set_sprite_image_number(sp,0);
}
INT16S show_pointline(INT16S sp,INT16S x,INT16S y)
{
	INT16S m=0;
	if(abc){
		show_word(sp,x,y,abc);
		sp++;
		m++;
	}
	if(game_time/10%2){
		show_word(sp,x,y+1,'_');
	}else set_sprite_image_number(sp,0);

	return m;
}
void show_word32(INT16S sp,INT16S x,INT16S y,INT16S no)
{
	if(no>=1000 && no<1001+KOREA_MAX){		//韩文 1000=space 1001begin
		set_sprite_display_init(sp,x,y,words32_sp_Frame[0]);
		set_sprite_nCharNum(sp,Word_Kb_Num*4+no-1000);
	}else if(no>=221 && no<221+16){				//korea char  yuan
		set_sprite_display_init(sp,x,y,Korea32_char_Frame[no-202]);
		set_sprite_nCharNum(sp,Word_Kb_Num*4);
	}else if(no>=200 && no<221){				//korea char	fu
		set_sprite_display_init(sp,x,y,Korea32_char_Frame[no-201]);
		set_sprite_nCharNum(sp,Word_Kb_Num*4);
	}else if(no>0x20 && no<0x80){				//ascii 0x20--0x7f
		set_sprite_display_init(sp,x,y,chars32_sp_Frame[0]);
		set_sprite_nCharNum(sp,Word_Kb_Num*4+no-0x20);
	}else set_sprite_image_number(sp,0);
}
INT16S show_pointline32(INT16S sp,INT16S x,INT16S y)
{
	INT16S m=0;
	if(abc){
		show_word32(sp,x,y,abc);
		sp++;
		m++;
	}
	if(game_time/10%2){
		show_word32(sp,x,y+3,'_');
	}else set_sprite_image_number(sp,0);

	return m;
}

#define Writing_X		70
#define Writing_Y		100
#define Writing_Line	10
void show_writing(INT16S sp,INT16S num)
{
	INT16S i,x,y,m,n,k;

	x=Writing_X;
	y=Writing_Y;
	ClearSomeSp(sp,sp+num+2);
	m=writeing_max;

	for(i=0;i<=m;i++){
		if(Words_pos==i){
			k=show_pointline(sp,x,y);
			sp+=k+1;
			x+=16*k;
			if(x>=Writing_X+16*Writing_Line){
				x=Writing_X;
				y+=16;
			}
		}
		if(i==m) break;
		n=writeing_Words[i];
		show_word(sp,x,y,n);
		x+=16;
		if(x>=Writing_X+16*Writing_Line){
			x=Writing_X;
			y+=16;
		}
		sp++;
	}
}

#define Writing32_X		70
#define Writing32_Y		100
#define Writing32_Line	6
void show_writing32(INT16S sp,INT16S num)
{
	INT16S i,x,y,m,n,k;

	x=Writing32_X;
	y=Writing32_Y;
	ClearSomeSp(sp,sp+num+2);
	m=writeing_max;

	for(i=0;i<=m;i++){
		if(Words_pos==i){
			k=show_pointline32(sp,x,y);
			sp+=k+1;
			x+=32*k;
			if(x>=Writing32_X+32*Writing32_Line){
				x=Writing32_X;
				y+=32;
			}
		}
		if(i==m) break;
		n=writeing_Words[i];
		show_word32(sp,x,y,n);
		x+=32;
		if(x>=Writing32_X+32*Writing32_Line){
			x=Writing32_X;
			y+=32;
		}
		sp++;
	}
}

void InitABC()
{
	int i;

	for(i=0;i<6;i++) ABC[i]=0;
	ABC_idx=0;
	abc=0;
}

//flag==0 In	==1 del		==2 repeat
INT16S Input_word(INT16S flag,INT16S word)
{
	INT16S i,m,k;
	INT16S Num;
	
	if(flag==0){
		if((Words_max&0x1000) && (word<1000)) return;
		else if((Words_max&0x2000) && ((word<'0')||(word>'9')) ) return;
	}
	
	m=writeing_max;
	Num=Words_max&0xfff;
	if(flag==0){
		if(OS_game==19 || OS_game==18|| OS_game==17){
			if(Words_pos>=126) return 1;
			
			for(k=Words_pos;k<Num;k++){
				if(writeing_Words[k]==0) break;
			}
			for(i=0;i<k-Words_pos;i++){
				writeing_Words[k-i]=writeing_Words[k-1-i];
			}
			writeing_Words[Words_pos]=word;
			Words_pos++;
		}else{
			if(m>=Num) return 1;
			
			for(i=0;i<m-Words_pos;i++){
				writeing_Words[m-i]=writeing_Words[m-1-i];
			}
			writeing_Words[Words_pos]=word;
			Words_pos++;
			writeing_max++;
		}
	}else if(flag==1){
		if(Words_pos==0) return 0;
		if(OS_game==19 || OS_game==18|| OS_game==17){
			for(k=Words_pos;k<Num;k++){
				if(writeing_Words[k]==0) break;
			}
			for(i=0;i<k-Words_pos;i++){
				writeing_Words[Words_pos-1+i]=writeing_Words[Words_pos+i];
			}
			writeing_Words[k-1]=0;			
			Words_pos--;
			/*while(1){
				if(Words_pos>0 && writeing_Words[Words_pos-1]==0) Words_pos--;
				else break;
			}//*/
		}else{
			for(i=0;i<m-Words_pos;i++){
				writeing_Words[Words_pos-1+i]=writeing_Words[Words_pos+i];
			}
			writeing_Words[m-1]=0;
			writeing_max--;
			Words_pos--;
		}
	}else if(flag==2){
		writeing_Words[Words_pos-1]=word;
	}
	return 0;
}
void write_abc()
{	
	if(abc) Input_word(0,abc);
	InitABC();
}

void Input_KoreaKey(INT16S key )
{	
	INT16S i,m,k,kk;

	m=writeing_max;
	k=Words_max&0xfff;
	if(OS_game==19 || OS_game==18  || OS_game==17){
		if(Words_pos>=126) return;
	}else if(m>=k) return;
	
	if(key==-1){
		ABC_idx--;
		ABC[ABC_idx]=0;
	}else{
		ABC[ABC_idx]=key;
		ABC_idx++;
	}
	if(ABC_idx==0){				//none
		abc=0;
	}else if(ABC_idx==1){			//one
		abc=200+ABC[ABC_idx-1];
	}else{
		k=ABC[ABC_idx-1];
		if(k>=20){		//yuan
			if(ABC_idx>3){
				m=getWords((INT8U*)&ABC[ABC_idx-2]);
				if(m){
					abc=m;
					kk=ABC[ABC_idx-2];
					ABC[ABC_idx-1]=0;
					ABC[ABC_idx-2]=0;
					m=getWords((INT8U*)&ABC);
					if(m) Input_word(0,m);
					ABC[0]=kk;
					ABC[1]=k;
					ABC[2]=ABC[3]=ABC[4]=ABC[5]=0;
					ABC_idx=2;
				}else{
					write_abc();
					ABC[ABC_idx]=k;
					ABC_idx++;
					abc=200+ABC[ABC_idx-1];
				}
			}else{
				m=getWords((INT8U*)&ABC);
				if(m){
					abc=m;
				}else{
					write_abc();
					ABC[ABC_idx]=k;
					ABC_idx++;
					abc=200+ABC[ABC_idx-1];
				}
			}
		}else{						//fu yin
			if(ABC_idx==6){
				write_abc();
				ABC[ABC_idx]=k;
				ABC_idx++;
				abc=200+ABC[ABC_idx-1];
			}else{
				m=getWords((INT8U*)&ABC);
				if(m){
					abc=m;
				}else{
					write_abc();
					ABC[ABC_idx]=k;
					ABC_idx++;
					abc=200+ABC[ABC_idx-1];
				}
			}
		}
	}
}
const INT16S Korean_Key[]={
	7 ,32,15,12, 4, 6,19,		//abcdefg
	29,23,25,21,34,33,31,		//hijklmn
	22,26, 8, 1, 3,10,			//opqrst
	27,18,13,17,30,16			//uvwxyz
};
const INT16S Korean_Key2[]={
	0 , 0, 0, 0, 5, 0, 0,		//ABCDEFG
	0 , 0, 0, 0, 0, 0, 0,		//HIJKLMN
	24,28, 9, 2, 0,11,			//OPQRST
	 0, 0,14, 0, 0, 0			//UVWXYZ
};
INT16S Keyboard_pos(INT16S m)
{
	if(m==K_Left){
		write_abc();
		if(Words_pos){
			Words_pos--;
			return 0;
		}else return 1;
	}else if(m==K_Right){
		write_abc();	
		if(Words_pos<writeing_max) {
			Words_pos++;
			return 0;
		}else return 1;
	}else if(m==K_Up){
		write_abc();	
		if(Words_pos>=Writing_Line) {
			Words_pos-=Writing_Line;
			return 0;
		}else return 1;
	}else if(m==K_Down){
		write_abc();	
		if(Words_pos<=writeing_max-Writing_Line) {
			Words_pos+=Writing_Line;
			return 0;
		}else return 1;
	}
	return -1;
}

INT16S Test_Keyboard()
{
	INT16S i,m;

	m=GetKeyBoardBuff();
/*
	if(m==K_Enter){				//M7 加入此判断
		write_abc();
		Input_word(0,m);
		return 0;
	}	
*/	
	if(m==0) return -1;
	if(m==K_Language){
		Keyboard=(Keyboard+1)%2;
		return 0;
	}
	
	if(Keyboard==0){				//english
		write_abc();
		if(m==K_Del) {
			Input_word(1,0);
			return 0;
		}else{
			i=Keyboard_pos(m);
			if(i!=-1) return i;
		}
		if(m<' ' || m>0x7f) return 1;
		Input_word(0,m);
	}else if(Keyboard==1){				//Korean
		if(m==K_Del){
			if(ABC_idx) Input_KoreaKey(-1);
			else Input_word(1,0);
			return 0;
		}else if(m==' '){
			//if(ABC_idx) write_abc();
			//else Input_word(0,m);
			//return 0;
		}else{
			i=Keyboard_pos(m);
			if(i!=-1) return i;
		}
		if(m<' ' || m>0x7f) return 1;
		if(m>='a' && m<='z'){
			m=Korean_Key[m-'a'];
		}else if(m>='A' && m<='Z'){
			m=Korean_Key2[m-'A'];
			if(m==0) return 1;
		}else{
			write_abc();
			Input_word(0,m);
			return 0;
		}
		Input_KoreaKey(m);
	}
	return 0;
}
INT16S Test_Keyboard9()
{
	INT16S i,m;

	//m=GetKeyBoardBuff();
	m=KeyBoardCode;
/*
	if(m==K_Enter){				//M7 加入此判断
		write_abc();
		Input_word(0,m);
		return 0;
	}	
*/	
	if(m==0) return -1;
	if(m==K_Language){
		Keyboard=(Keyboard+1)%2;
		return 0;
	}
	
	if(Keyboard==0){				//english
		write_abc();
		if(m==K_Del) {
			Input_word(1,0);
			return 0;
		}else{
		}
		if(m<' ' || m>0x7f) return 1;
		Input_word(0,m);
	}else if(Keyboard==1){				//Korean
		if(m==K_Del){
			if(ABC_idx) Input_KoreaKey(-1);
			else Input_word(1,0);
			return 0;
		}else if(m==' '){
			//if(ABC_idx) write_abc();
			//else Input_word(0,m);
			//return 0;
		}else{
		}
		if(m<' ' || m>0x7f) return 1;
		if(m>='a' && m<='z'){
			m=Korean_Key[m-'a'];
		}else if(m>='A' && m<='Z'){
			m=Korean_Key2[m-'A'];
			if(m==0) return 1;
		}else{
			write_abc();
			Input_word(0,m);
			return 0;
		}
		Input_KoreaKey(m);
	}
	return 0;
}


void main_words()
{	
	INT16S i,m;
	
	spMalloc(500+700);
	Word_Kb_Num=500;
	Words_max=50;
	Palette_ram_cpy( 1,0, menu_SPRITE_Palette1+256*0	,256);	
	Palette_ram_cpy( 2,240, words_SPRITE_Palette1	,16);
	//LoadData_1(1,SpriteCellData,"Pororo\\menu\\SpriteCell_cmp.BIN");
	//LoadData_1(0,SpriteCellData+Word_Kb_Num*_1K,"Pororo\\words\\SpriteCell.bin");
	LoadData_1(1,SpriteCellData+Word_Kb_Num*_1K,"Pororo\\words\\SpriteCell_cmp.bin");
	
	gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)SpriteCellData);//sprite cell data
	LoadTextBmp(C_PPU_TEXT1,"Pororo\\words\\M0_1-4_sound.gif");
	clear_sprite(Spritebuf_number);
	gplib_ppu_fade_effect_set(ppu_register_set, 0);

	game_time=step=0;
	Keyboard=1;
	while(1)
	{
		switch(step){
//--------------------------------------------------view
			case 0:
				//show_writing(30,Words_max);
				show_writing32(30,Words_max);
				m=Test_Keyboard();
				if(m==0) SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
				else if(m==1) SPU_PlayPCM_NoEnv_FixCH((INT32U*)DrmAddr[TAP_SFX], 64, 80, 12);
			break;
		}

		PaintTFT_cell4();
		if(Goto_desktop()){
			step=100;
		}
		if(step==100) break;
	}
}