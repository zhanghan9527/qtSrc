#ifndef	__KEY_H__
#define	__KEY_H__
//	write your header here

#include "drv_l1_sfr.h"
#include "drv_l1_tv.h"

extern int Key_State;	//连续按
extern int Key_old;
extern int Key_States;	//单按
extern int Key_Release;//释放按键

extern int KeyBoardCode,RepeatKeyCode,BoardRepeat;

void ReadHandle(void);
unsigned short _getKey1();
unsigned short GetKeyBoardJoy();

//----------------KEY IO口设置-------------------------
#define 		P_KB_CTRL_I 		R_IOE_I_DATA
#define 		P_KB_CTRL_O 		R_IOE_O_DATA
#define 		P_KB_CTRL_ATT 		R_IOE_ATT
#define 		P_KB_CTRL_DIR		R_IOE_DIR
#define 		P_KB_CTRL_DRV 		R_IOE_DRV

#define 		P_KB_DATA0_I 		R_IOC_I_DATA
#define 		P_KB_DATA0_O 		R_IOC_O_DATA
#define 		P_KB_DATA0_ATT 		R_IOC_ATT
#define 		P_KB_DATA0_DIR 		R_IOC_DIR
#define 		P_KB_DATA0_DRV 		R_IOC_DRV

#define 		P_KB_DATA1_I 		R_IOG_I_DATA
#define 		P_KB_DATA1_O 		R_IOG_O_DATA
#define 		P_KB_DATA1_ATT 		R_IOG_ATT
#define 		P_KB_DATA1_DIR 		R_IOG_DIR
#define 		P_KB_DATA1_DRV 		R_IOG_DRV

#define 		P_KB_DATA2_I 		R_IOI_I_DATA
#define 		P_KB_DATA2_O 		R_IOI_O_DATA
#define 		P_KB_DATA2_ATT 		R_IOI_ATT
#define 		P_KB_DATA2_DIR 		R_IOI_DIR
#define 		P_KB_DATA2_DRV 		R_IOI_DRV

//#define 		KEYBOARD_OUT2		BIT3
#define 		KEYBOARD_OUT1		BIT3
#define 		KEYBOARD_OUT0		BIT2

#define 		KEYBOARD_PID0		BIT10
#define 		KEYBOARD_PID1		BIT15
#define 		KEYBOARD_PID2		BIT8
#define 		KEYBOARD_PID3		BIT9


//*/
//------------------------------------------
/* define GPIO bit */
#define 		P_JOY_Attrib 		R_IOE_ATT
#define 		P_JOY_Dir 			R_IOE_DIR
#define 		P_JOY_Data 			R_IOE_I_DATA
#define 		P_JOY_Buffer 		R_IOE_O_DATA
#define 		P_JOY_DRV			R_IOE_DRV
#define 		P_JOY_IO			BIT2


#define 		JOYSTICK_STB		BIT4
#define 		JOYSTICK_OUT1		BIT5
#define 		JOYSTICK_CLK		BIT6
#define 		JOYSTICK_OUT2		BIT7


#define 		JOY_RIGHT1			( 0x0100 )
#define 		JOY_LEFT1			( 0x0200 )
#define 		JOY_DOWN1			( 0x0400 )
#define 		JOY_UP1				( 0x0800 )
#define 		JOY_START1			( 0x1000 )
#define 		JOY_SELECT1			( 0x2000 )
#define 		JOY_A1				( 0x8000 )
#define 		JOY_B1				( 0x4000 )

#define 		JOY_RIGHT2			( 0x0001 )
#define 		JOY_LEFT2			( 0x0002 )
#define 		JOY_DOWN2			( 0x0004 )
#define 		JOY_UP2				( 0x0008 )
#define 		JOY_START2			( 0x0010 )
#define 		JOY_SELECT2			( 0x0020 )
#define 		JOY_A2				( 0x0080 )
#define 		JOY_B2				( 0x0040 )

#define			JOY_NONE			( 0x0000 )

//------------------Note Book 专用----------------------------

#define         K_Esc                     0x1B	
#define         K_CapsLock                0x1C
#define         K_Shift                   0x1D
#define         K_Alt                     0x1E
#define         K_Ctrl                    0x1F
#define         K_PRN                     0x16
#define         K_Pause                   0x03
#define         K_Break                   0x03
//----------------------------------------------
#define         K_Enter                   0x0D
#define         K_BackSpace               0x08
#define         K_Home                    0x87 //menu
#define         K_Left                    0x84
#define         K_End                     0x81
#define         K_Up                      0x88
#define         K_Down                    0x82
#define         K_PageUp                  0x89
#define         K_PageDown                0x83
#define         K_Right                   0x86
#define         K_Ins                     0x80
#define         K_Del                     0x8A
//----------------------------------------------
#define         K_Ctrl_A                  0x01
#define         K_Ctrl_B                  0x02
#define         K_Ctrl_C                  0x03
#define         K_Ctrl_D                  0x04
#define         K_Ctrl_E                  0x05
#define         K_Ctrl_F                  0x06
#define         K_Ctrl_G                  0x07
#define         K_Ctrl_H                  0x08
#define         K_Ctrl_I                  0x09
#define         K_Ctrl_J                  0x0A
#define         K_Ctrl_K                  0x0B
#define         K_Ctrl_L                  0x0C
#define         K_Ctrl_M                  0x0D
#define         K_Ctrl_N                  0x0E
#define         K_Ctrl_O                  0x0F
#define         K_Ctrl_P                  0x10
#define         K_Ctrl_Q                  0x11
#define         K_Ctrl_R                  0x12
#define         K_Ctrl_S                  0x13
#define         K_Ctrl_T                  0x14
#define         K_Ctrl_U                  0x15
#define         K_Ctrl_V                  0x16
#define         K_Ctrl_W                  0x17
#define         K_Ctrl_X                  0x18
#define         K_Ctrl_Y                  0x19
#define         K_Ctrl_Z                  0x1A
//----------------------------------------------
#define         K_TAB             0x09
#define         K_Ctrl_End        0x0A
#define         K_Ctrl_Home       0x0B

#define         K_Space           0x20
#define         K_S_1             0x21             //!  exclamination mark
#define         K_DQuotes         0x22             //"  double quotation
#define         K_S_3             0x23             //#  Number sign
#define         K_S_4             0x24             //0x  Dollar sign
#define         K_S_5             0x25             //%  Percent sign
#define         K_S_7             0x26             //&  and sign
#define         K_SQuotes         0x27             //'  single quotation
#define         K_S_9             0x28             //(  left bracket
#define         K_S_0             0x29             //)  right bracket
#define         K_S_8             0x2A             //*
#define         K_Times           0x2A             //*  times mark
#define         K_Plus            0x2B             //+  plus mark
#define         K_Comma           0x2C             //,  comma
#define         K_Minus           0x2D             //-  minus
#define         K_Point           0x2E             //.  point
#define         K_Div             0x2F             ///  div
#define         K_N0              0x30             //0
#define         K_N1              0x31             //1
#define         K_N2              0x32             //2
#define         K_N3              0x33             //3
#define         K_N4              0x34             //4
#define         K_N5              0x35             //5
#define         K_Ext5            0x35             //5
#define         K_N6              0x36             //6
#define         K_N7              0x37             //7
#define         K_N8              0x38             //8
#define         K_N9              0x39             //9
#define         K_Colon           0x3A             //:
#define         K_SemiColon       0x3B             //;
#define         K_Less            0x3C             //<

#define         K_Equal           0x3D             //=
#define         K_More            0x3E             //>
#define         K_Question        0x3F             //?    //question mark
#define         K_S_2             0x40             //@    //at mark
#define         K_S_a             0x41             //A
#define         K_S_b             0x42             //B
#define         K_S_c             0x43             //C
#define         K_S_d             0x44             //D
#define         K_S_e             0x45             //E
#define         K_S_f             0x46             //F
#define         K_S_g             0x47             //G
#define         K_S_h             0x48             //H
#define         K_S_i             0x49             //I
#define         K_S_j             0x4A             //J
#define         K_S_k             0x4B             //K
#define         K_S_l             0x4C             //L
#define         K_S_m             0x4D             //M
#define         K_S_n             0x4E             //N
#define         K_S_o             0x4F             //O
#define         K_S_p             0x50             //P
#define         K_S_q             0x51             //Q
#define         K_S_r             0x52             //R
#define         K_S_s             0x53             //S
#define         K_S_t             0x54             //T
#define         K_S_u             0x55             //U
#define         K_S_v             0x56             //V
#define         K_S_w             0x57             //W
#define         K_S_x             0x58             //X
#define         K_S_y             0x59             //Y
#define         K_S_z             0x5A             //Z
#define         K_LeftSqBra       0x5B             //[    //Left Square Brackets
#define         K_BackSlash       0x5C             //"\"
#define         K_RightSqBra      0x5D             //]    //Right Square Brackets
#define         K_S_6             0x5E             //^    //circumflex

#define         K_S_Minus         0x5F             //_    //underline
#define         K_PauseMark       0x60             //`    //pause mark
#define         K_a			      0x61             //a
#define         K_b               0x62             //b
#define         K_c               0x63             //c
#define         K_d               0x64             //d
#define         K_e               0x65             //e
#define         K_f               0x66             //f
#define         K_g               0x67             //g
#define         K_h               0x68             //h
#define         K_i               0x69             //i
#define         K_j               0x6A             //j
#define         K_k               0x6B             //k
#define         K_l               0x6C             //l
#define         K_m               0x6D             //m
#define         K_n               0x6E             //n
#define         K_o               0x6F             //o
#define         K_p               0x70             //p
#define         K_q               0x71             //q
#define         K_r               0x72             //r
#define         K_s               0x73             //s
#define         K_t               0x74             //t
#define         K_u               0x75             //u
#define         K_v               0x76             //v
#define         K_w               0x77             //w
#define         K_x  		   	  0x78             //x
#define         K_y    			  0x79             //y
#define         K_z               0x7A             //z
#define         K_LeftBrace       0x7B             //{    Left Brace
#define         K_OrMark          0x7C             //|    Or Mark
#define         K_RightBrace      0x7D             //}    Right Brace
#define         K_Tilde           0x7E             //~    Tilde or wave

#define         K_F1              0x90		//mode1
#define         K_F2              0x91		//mode2
#define         K_F3              0x92		//mode3
#define         K_F4              0x93		//mode4
#define         K_F5              0x94		//mode5
#define         K_F6              0x95		//mode6
#define         K_F7              0x96		//vol+
#define         K_F8              0x97		//vol-
#define         K_F9              0x98
#define         K_F10             0x99
#define         K_F11             0x9A
#define         K_F12             0x9B

#define         K_Win1            0x9c
#define         K_Win2            0x9d

#define 		K_symbol		  0x9e

#define         K_NumLock         0xEA

//----------------下面是NoteBook 专用定义------------------------------


#define         K_Language			0x90
#define         K_Album				0x91
#define         K_VideoCall			0x92
#define         K_Decoration		0x93	
#define         K_Heart				K_LeftSqBra
#define         K_NextVideo			0x95	
#define         K_MimiCard			0x96
#define         K_LastVideo			0x97
#define         K_Diary 			0x98
#define         K_Setting			0x99
#define         K_Return			0x9a
#define         K_Camera			0x9b	
#define         K_Video				0x9c
#define         K_EMail				0x9d
#define         K_Game				0x9e
#define         K_Ok				0x9f
#define         K_Back				0xa0
//#define         K_Game				0xa1
#define         K_Play				0xa2
#define         K_Music				K_RightSqBra

#define         K_Heart2				K_LeftBrace
#define         K_Star				K_RightBrace

#endif
