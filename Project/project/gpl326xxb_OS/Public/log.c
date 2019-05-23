#include "application.h"
#include "data.h"
#include <stdio.h>
#include <string.h> 

//===========================================================

typedef struct{
	INT8U app; //app ����
    INT8U game;
    INT8U part;
    INT8U round; //�ڼ������� 
    INT8U level;
    INT8U result;//1:true,0:false  
    INT32U time;

}LOG_TYPE;


LOG_TYPE *p_log;
INT32U logbuff; 


INT32U Playtime;//second
INT16S roundnum;
INT16S errornum;

//partnum: �м���part//  ���5��
//int finishpart;//finishpart:��ɵ�part����  //��0��ʼ
//int playloop;//loop:��ʾ��ڼ��飻��0��ʼ
//game:��1��ʼ
//level ��1��ʼ

void _Play(void)
{
	Playtime++;
}

//����Ϸֻ����һ��
void initLog(void)
{
	timer_stop(TIMER_B);
	Playtime=0;
	roundnum=0;
	p_log=(LOG_TYPE *)logbuff;  //Ҫ���Ȼ�ȡ600���ֽڳ����ڴ�

	timer_msec_setup(TIMER_B, 1000,0,_Play);
	errornum=0;	
}

void addlog(int app,int game,int part,int level,int answernum)
{
	
	roundnum++;
	p_log->app=app;
	p_log->game=game;
	p_log->part=part;
	p_log->round=roundnum;	
	p_log->level=level;
	p_log->result=checkError(answernum);
	p_log->time=Playtime;
	p_log++;
	errornum=0;
}

int checkError(int answernum)
{
	if(answernum<3){
		if(errornum){
			return 0;//error		
		}
	}else{
		if(errornum>1){
			return 0;//error		
		}
	}
	
	return 1;	//correct
}
//const char *gameName[]={
//"GAME",
//"Ebook",
//};
const char logtitle[]={
"APP	GAME	part	round	level	result	time",
};

int saveLog(void)
{
	INT16S i;
	INT16S fd;
	INT16S num;
	INT16S first;
	INT32S nRet=-1;
	INT32U time;
	
	char log[200];
		
	timer_stop(TIMER_B);	
	
	
	return 0;
	
	if(!roundnum){
		return nRet;	
	}
	
	CD_PAN("");
	
	fd = open("log.log", O_RDWR);	
	if(fd<0){
		fd = open("log.log", O_RDWR|O_CREAT);
		first=1;		
	}else first=0;
	time=0;
	if(fd==0){
		
		lseek(fd, 0,SEEK_END);
		p_log=(LOG_TYPE *)logbuff;
		
		while(roundnum)
		{
			roundnum--;
			if(!first){
				sprintf(log, (const char *)"%d	%d	%d	%d	%d	%d	%d\r\n",
					p_log->app,p_log->game,p_log->part,p_log->round,p_log->level,p_log->result,p_log->time-time);  
			}else{
				first=0;				
				sprintf(log, (const char *)"%s\r\n%d	%d	%d	%d	%d	%d	%d\r\n",logtitle,
					p_log->app,p_log->game,p_log->part,p_log->round,p_log->level,p_log->result,p_log->time); 				 			
			}
			time=p_log->time;
			p_log++;
			num=strlen(log);
			if(num<200){
				nRet = write(fd,(INT32U)log,num);				
			}						
		}
	
		nRet = close(fd);
		if(DISKUSED==FS_NAND1){
		   DrvNand_flush_allblk();
	    }
	    
	}

	return nRet;//0 :�ɹ�
	
}



