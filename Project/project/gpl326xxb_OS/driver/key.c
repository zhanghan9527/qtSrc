
#include "key.h"
#include "project.h"
#include "touch_panel.h"
//==========================================================
// variable
//==========================================================

int Key_State;		//连续按
int Key_old;		
int Key_States;		//单按
int Key_Release;	//释放按键



int KeyBoardCode;
int KeyBoardBuff[20];
int BoardShift,BoardCapsLock,BoardCtrl;
unsigned int KeyCodeLast[10],KeyCodeLost[10];
unsigned int KeyCode[10];
int RepeatNum,BoardIndex0;
int BoardIndex,BoardRepeat;
int	RepeatKeyCode;

int old_key1 = 0;
int old_key2 = 0;

void ReadHandle(void);
unsigned short _getKey1();
unsigned short GetJoystickKeyCode(void);
void GetKeyBoardCode(void);


#define KEY_TURN 0x20

/***************************************************************************
* Function:		MiMi Notebook 键盘驱动程序  
				KEY_IOInit
				KeyService
* Description:	
* Syntax:
* parameters:   
* returns:
* Notes:
*	有3种调用方法
*	1。loop 直接调用GetKeyBoardCode，立即获得KeyBoardCode.
*		应用于键盘练习,	 GetKeyBoardCode();
*	2. IRQ调用GetKeyBoardCode，loop 中 GetKeyBoardBuff 返回值,
*		应用于 输入法 打字练习  Code=GetKeyBoardBuff();
*	3.	IRQ调用 GetKeyBoardJoy ，像 Joystick 一样返回值。
*		应用于游戏中,类似手柄.可以按多健,但仅限下面的健有作用。
*		up-down-left-right------KEY_UP KEY_DOWN KEY_LEFT KEY_RIGHT
*		enter--KEY_START  space--KEY_SELECT
*		z--x--c--	KEY_A B C
***************************************************************************/
const INT16S KeyScanCode0[]={		//Shift Not Pressed ,Caps Is  UnLocked
K_Language	,K_s		,K_x		,K_w		,0x00			,K_Up			,K_Album		,K_N2	,
K_VideoCall	,K_a		,K_z		,K_q		,0x00			,K_Left			,K_Decoration	,K_N1	,
0x00		,K_k		,K_Comma	,K_i		,K_Heart		,K_Div			,K_Minus		,K_N8	,
K_LastVideo	,K_g		,K_b		,K_t		,0x00			,K_Ok			,K_MimiCard		,K_N5	,
K_NextVideo	,K_j		,K_m		,K_u		,K_Music			,K_SQuotes		,K_Equal	,K_N7	,
K_Diary		,K_Home		,0x00		,K_Setting	,0x00			,K_Return		,K_Camera		,K_Back,
K_Video		,K_f		,K_v		,K_r		,0x00			,K_Right		,K_EMail		,K_N4	,
K_Play		,K_h		,K_n		,K_y		,K_Enter		,K_Shift		,K_Del			,K_N6	,
K_Space		,K_d		,K_c		,K_e		,0x00			,K_Down			,K_Game			,K_N3	,
0x00		,K_l		,K_Point	,K_o		,K_p			,K_SemiColon	,K_N0			,K_N9	,
};
/*
K_SQuotes	,K_Shift	,K_Minus	,K_N0		,K_LeftSqBra	,K_Equal		,K_RightSqBra	,K_Ins	,
K_Up		,K_Left		,K_End		,K_Home		,K_BackSlash	,K_BackSpace	,K_Enter		,K_Del	,
K_v			,K_Del		,K_w		,K_q		,K_x			,K_s			,K_c			,K_N1	,
K_Comma		,K_Ctrl		,K_N6		,K_N5		,K_h			,K_y			,K_j			,K_F4	,
K_f			,K_b		,K_N2		,K_F1		,K_e			,K_N3			,K_d			,K_Esc	,
K_Right		,K_Down		,K_Div		,K_PageDown	,K_Minus		,K_S_8			,K_Plus			,K_PageUp,
K_l			,K_Point	,K_N7		,K_F6		,K_i			,K_u			,K_k			,K_F5	,
K_n			,K_m		,K_N4		,K_F3		,K_t			,K_r			,K_g			,K_F2	,
K_SemiColon	,K_Div		,K_N9		,K_F8		,K_o			,K_N8			,K_p			,K_F7	,
K_Alt		,K_Space	,K_a		,K_CapsLock	,K_Ctrl			,K_Shift		,K_z			,K_TAB	,
*/

const INT16S KeyScanCode1[]={		//Shift  Pressed  ,Caps Is Locked
K_Language	,K_S_s		,K_S_x		,K_S_w		,0x00			,K_Up			,K_Album		,K_S_2	,
K_VideoCall	,K_S_a		,K_S_z		,K_S_q		,0x00			,K_Left			,K_Decoration	,K_S_1	,
0x00		,K_S_k		,K_Less		,K_S_i		,K_Heart2		,K_Question		,K_S_Minus		,K_S_8	,
K_NextVideo	,K_S_g		,K_S_b		,K_S_t		,0x00			,K_Ok			,K_MimiCard		,K_S_5	,
K_LastVideo	,K_S_j		,K_S_m		,K_S_u		,K_Star			,K_DQuotes		,K_Plus			,K_S_7	,
K_Diary		,K_Home		,0x00		,K_Setting	,0x00			,K_Return		,K_Camera		,K_Back,
K_Video		,K_S_f		,K_S_v		,K_S_r		,0x00			,K_Right		,K_EMail		,K_S_4	,
K_Play		,K_S_h		,K_S_n		,K_S_y		,K_Enter		,K_Shift		,K_Del			,K_S_6	,
K_Space		,K_S_d		,K_S_c		,K_S_e		,0x00			,K_Down			,K_Game			,K_S_3	,
0x00		,K_S_l		,K_More		,K_S_o		,K_S_p			,K_Colon		,K_S_0			,K_S_9	,
};
/*
K_DQuotes	,K_Shift	,K_S_Minus	,K_S_0		,K_LeftBrace	,K_Plus			,K_RightBrace	,K_Ins	,
K_Up		,K_Left		,K_End		,K_Home		,K_OrMark		,K_BackSpace	,K_Enter		,K_Del	,
K_S_v		,K_Del		,K_S_w		,K_S_q		,K_S_x			,K_S_s			,K_S_c			,K_S_1	,
K_Less		,K_Ctrl		,K_S_6		,K_S_5		,K_S_h			,K_S_y			,K_S_j			,K_F4	,
K_S_f		,K_S_b		,K_S_2		,K_F1		,K_S_e			,K_S_3			,K_S_d			,K_Esc	,
K_Right		,K_Down		,K_Div		,K_PageDown	,K_Minus		,K_S_8			,K_Plus			,K_PageUp,
K_S_l		,K_More		,K_S_7		,K_F6		,K_S_i			,K_S_u			,K_S_k			,K_F5	,
K_S_n		,K_S_m		,K_S_4		,K_F3		,K_S_t			,K_S_r			,K_S_g			,K_F2	,
K_Colon		,K_Question	,K_S_9		,K_Tilde	,K_S_o			,K_S_8			,K_S_p			,K_F7	,
K_Alt		,K_Space	,K_S_a		,K_CapsLock	,K_Ctrl			,K_Shift		,K_S_z			,K_TAB	,
*/



//====================================================================================================
// 初始化
//====================================================================================================

void KEY_IOInit(void)
{
	int i;
	INT32U* ptr;

	if(KEY_FLAG==0) return;
	R_UART_CTRL = 0;


	P_KB_CTRL_ATT |= KEYBOARD_OUT0|KEYBOARD_OUT1;	
	P_KB_CTRL_DIR |= KEYBOARD_OUT0|KEYBOARD_OUT1;
	P_KB_CTRL_O &= ~(KEYBOARD_OUT0|KEYBOARD_OUT1);	
	P_KB_CTRL_DRV |= KEYBOARD_OUT0|KEYBOARD_OUT1;	

	P_KB_DATA0_ATT  &= ~(KEYBOARD_PID0);
	P_KB_DATA0_DIR  &= ~(KEYBOARD_PID0);
	P_KB_DATA0_O 	&= ~(KEYBOARD_PID0);	
	P_KB_DATA0_DRV  |=  (KEYBOARD_PID0);	
	
	P_KB_DATA1_ATT  &= ~(KEYBOARD_PID1);
	P_KB_DATA1_DIR  &= ~(KEYBOARD_PID1);
	P_KB_DATA1_O 	&= ~(KEYBOARD_PID1);	
	P_KB_DATA1_DRV  |=  (KEYBOARD_PID1);	

	P_KB_DATA2_ATT  &= ~(KEYBOARD_PID2|KEYBOARD_PID3);
	P_KB_DATA2_DIR  &= ~(KEYBOARD_PID2|KEYBOARD_PID3);
	P_KB_DATA2_O 	&= ~(KEYBOARD_PID2|KEYBOARD_PID3);	
	P_KB_DATA2_DRV  |=  (KEYBOARD_PID2|KEYBOARD_PID3);	
	

	for(i=0;i<20;i++){
		KeyBoardBuff[i]=0;
	}
	for(i=0;i<10;i++){
		KeyCodeLast[i]=0;
		KeyCodeLost[i]=0;
		KeyCode[i]=0;
	}
	KeyBoardCode=0;
	BoardIndex=0;
	BoardIndex0=0;
	BoardRepeat=0;
	RepeatNum=0;
	BoardShift=0;
	BoardCapsLock=0;
	BoardCtrl=0;	
	

	Key_State=0;
	Key_States=0;
	Key_old=0;

}

unsigned short GetKeyBoardBuff()
{
	unsigned short m=0;
	if(BoardIndex!=BoardIndex0){
		m=KeyBoardBuff[BoardIndex];
		BoardIndex++;
		if(BoardIndex>=20) BoardIndex=0;
	}
	return m;
}
void SetKeyBoardBuff(unsigned short m)
{
	KeyBoardBuff[BoardIndex0]=m;
	BoardIndex0++;
	if(BoardIndex0>=20) BoardIndex0=0;
}

void GetKeyBoardCode(void)
{

	unsigned int	*PortValue;
	unsigned int 	m,n,code;

	int i,j,k;

	for(i = 0; i < 10; i++) KeyCode[i]=0;

//---------------key scane-----------------------------------	
		
	P_KB_CTRL_O |= KEYBOARD_OUT0;		// Pin Hight rest
	for (i = 0; i < 100; i++) delay();
	P_KB_CTRL_O &= ~(KEYBOARD_OUT1);
	P_KB_CTRL_O &= ~(KEYBOARD_OUT0);
	
	for(i = 0; i < 10; i++)
	{

		P_KB_CTRL_O &= ~KEYBOARD_OUT1;				//
		for (j = 0; j < 100; j++) delay();
		KeyCode[i] =((P_KB_DATA0_I & KEYBOARD_PID0)>>10)|((P_KB_DATA1_I & KEYBOARD_PID1)>>14)
			|((P_KB_DATA2_I & (KEYBOARD_PID2|KEYBOARD_PID3))>>6);
		
		P_KB_CTRL_O |= KEYBOARD_OUT1;				//
		for (j = 0; j < 100; j++) delay();
		KeyCode[i] |=((P_KB_DATA0_I & KEYBOARD_PID0)>>6)|((P_KB_DATA1_I & KEYBOARD_PID1)>>10)
			|((P_KB_DATA2_I & (KEYBOARD_PID2|KEYBOARD_PID3))>>2);

	}

//========change==key====================================
	k=0;
	for(i = 0; i < 10; i++){
		if(KeyCode[i]!=KeyCodeLast[i]){
			k=1;
			break;
		}
	}
	if(k==0){		//repeat
		j=0;
		for(i = 0; i < 10; i++){
			if(KeyCode[i]!=0){
				j++;
				break;
			}
		}
		if(j==0){		//没有健按下
			BoardRepeat=0;
			RepeatKeyCode=KeyBoardCode=0;
			return;
		}else{
			if(BoardRepeat==0){
				BoardRepeat=1;
				RepeatNum=1*12;
				RepeatKeyCode=KeyBoardCode;
				KeyBoardCode=0;			
			}else if(BoardRepeat==1){
				if(RepeatNum==0) BoardRepeat=2;
				else RepeatNum--;
				KeyBoardCode=0;			
			}else{
				if(RepeatNum==0){
					KeyBoardCode=RepeatKeyCode;
					SetKeyBoardBuff(KeyBoardCode);
					RepeatNum=3;
				}else RepeatNum--;
			}
			return;
		}	
	}
	BoardRepeat=0;
	for(i = 0; i < 10; i++){
		KeyCodeLost[i]=KeyCodeLast[i];
		KeyCodeLast[i]=KeyCode[i];
	}
//========shift==Caps==ctrl===alt=======================
	if(KeyCode[7]&0x20){		//shift
		BoardShift=1;
	}else BoardShift=0;
//	if(KeyCode[5]&0x02){		//capslock
//		if(BoardCapsLock==1) BoardCapsLock=0;
//		else BoardCapsLock=1;
//	}
//	if(KeyCode[4]&0x80){		//ctrl
//		BoardCtrl=1;
//	}else BoardCtrl=0;
//========del lost key=================================
	for(i = 0; i < 10; i++){
		if(KeyCodeLost[i]){
			KeyCode[i]=KeyCode[i]&(KeyCodeLost[i]^0xff);
		}
	}
//========get keycode=================================
	KeyBoardCode=0;
	for(i = 0; i < 10; i++){
		if(KeyCode[i]){
			k=KeyCode[i];
			n=0;
			while(k!=1){
				k=k>>1;
				n++;
			}
			k=i*8+n;
			if(BoardShift){
				m=KeyScanCode1[k];
			}else{
				m=KeyScanCode0[k];
			}
/*			
			if((m>='A')&&(m<='Z')){
				if(BoardCapsLock) KeyBoardCode=m+0x20;
				else KeyBoardCode=m;
			}else if((m>='a')&&(m<='z')){
				if(BoardCapsLock) KeyBoardCode=m-0x20;
				else KeyBoardCode=m;
			}else{
				KeyBoardCode=m;
			}			
*/
			KeyBoardCode=m;						
			break;
		}
	}
	SetKeyBoardBuff(KeyBoardCode);
}
//====================================================================================================
unsigned short GetKeyBoardJoy()
{
	unsigned int     KeyCode[10]={0};
	unsigned int	*PortValue;
	unsigned int 	m,n,code;
	int i,j,k;

	for(i = 0; i < 10; i++) KeyCode[i]=0;
//---------------key scane-----------------------------------	
		
	P_KB_CTRL_O |= KEYBOARD_OUT0;		// Pin Hight rest
	for (i = 0; i < 100; i++) delay();
	P_KB_CTRL_O &= ~(KEYBOARD_OUT1);
	P_KB_CTRL_O &= ~(KEYBOARD_OUT0);
	
	for(i = 0; i < 10; i++)
	{

		P_KB_CTRL_O &= ~KEYBOARD_OUT1;				//
		for (j = 0; j < 100; j++) delay();
		KeyCode[i] =((P_KB_DATA0_I & KEYBOARD_PID0)>>10)|((P_KB_DATA1_I & KEYBOARD_PID1)>>14)
			|((P_KB_DATA2_I & (KEYBOARD_PID2|KEYBOARD_PID3))>>6);
		
		P_KB_CTRL_O |= KEYBOARD_OUT1;				//
		for (j = 0; j < 100; j++) delay();
		KeyCode[i] |=((P_KB_DATA0_I & KEYBOARD_PID0)>>6)|((P_KB_DATA1_I & KEYBOARD_PID1)>>10)
			|((P_KB_DATA2_I & (KEYBOARD_PID2|KEYBOARD_PID3))>>2);

	}

//========get=key==Joy===========================
//
	m=0;
	if(KeyCode[1]&0x01) m|=KEY_UP;
	if(KeyCode[1]&0x02) m|=KEY_LEFT;
	if(KeyCode[5]&0x01) m|=KEY_RIGHT;
	if(KeyCode[5]&0x02) m|=KEY_DOWN;

	if(KeyCode[1]&0x40) m|=KEY_START;  //K_Enter
	if(KeyCode[9]&0x02) m|=KEY_SELECT;  //K_Space

	if(KeyCode[9]&0x40) m|=KEY_A;	//K_z
	if(KeyCode[2]&0x10) m|=KEY_B;	//K_x

	return m;	
}

