
#ifndef		SETTING_H
#define		SETTING_H

typedef struct {

	INT16S 	HomeIconIdx[4];
	INT16S 	HomeScreenBGIdx;

	INT16U  PlayerOriName[4];	
	INT16S  PlayerAge;		
	INT16U  PlayerBirthMonth;			
	INT16U  PlayerBirthDay;
//12x2=24
	INT16U RecorderName1[3];
	INT16U RecorderName2[3];
	INT16U RecorderName3[3];
	INT16U RecordScore[3];
//12*2=24	48
	INT16U GetGiftFlag;
	
	INT8U InBox_Day;
	INT8U InBox_Mail;
	INT8U Miss_Mimi;
	INT8U Miss_User;

	INT8U PlayGameTime;
	INT8U TalkPhotoTime;
	INT8U SendMailTime;
	INT8U RecodeTime;
	INT8U NoMailMonth;
	INT8U NoMailDay;
//60
	INT8U Room_Buff[6];

	INT16U	tmp4;		//
	INT16U	tmp5;		//
	INT16U	tmp6;		//
	INT16U	tmp7;		//
	INT16U	tmp8;		//
	INT16U	tmp12;		//
	INT16U	tmp13;		//
	INT16U	tmp14;		//
	INT16U	tmp15;		//
				
} PLAYER, *PPLAY;		//
#define PLAYER_MAX	70



#endif