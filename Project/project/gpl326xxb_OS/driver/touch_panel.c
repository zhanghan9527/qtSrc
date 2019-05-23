//*****************************************************************************************
//	Projet:			TouchPanelDriver
//	Body:			GPL32600
//	Description:
//	Copyright:		Copyright(c)2010
//	Company:		Shen Zhen Multicontent Software
//	Author:			Ye
//	Modification History:
//					1.
//					2.
//*****************************************************************************************
//*****************************************************************************************
// Header File Included Area
//*****************************************************************************************


#include "drv_l1_gpio.h"
#include "touch_panel.h"
#include "drv_l1_tv.H"
#include "ir.H"



//*****************************************************************************************
// Global Variable Defintion Area
//*****************************************************************************************


int Coordinate_X,Coordinate_Y;
int TouchTest;

INT16S touch;
unsigned int TouchFlag,TouchFlags,ResponseFlag,PressDown;
unsigned int TouchRelease,Touchdelay;
INT16S DragOld_X,DragOld_Y;
int LastX,LastY;
unsigned int _X_FIFO[FIFOSize];
unsigned int *p_X_FIFO;
unsigned int _X_FullFlag;

unsigned int _Y_FIFO[FIFOSize];
unsigned int *p_Y_FIFO;
unsigned int _Y_FullFlag;

INT16U I2C_Flag=0;	//0--right call ,1---write,2---read,0xff---error
INT8U I2C_Data[8];

INT16U E2_Flag=0;

INT16S LVLevel=3;
INT16U Encflag=0;
INT16S lv_chk_num;
extern INT16S old_power;

//*****************************************************************************************
// Function Call Definition Area
//*****************************************************************************************
void  Clear_X_FIFO(void);
void  Clear_Y_FIFO(void);

void delay(void)
{
	int i;
	for(i=0;i<20;i++) i=i;
}
void AD_Init(void)
{
 //USB  IOF 5
	*(int*)IOF_ATTRIB_ADDR |=BIT5;
	*(int*)IOF_DIR_ADDR    &=~BIT5;
	*(int*)IOF_DATA_ADDR   &=~BIT5;
	*(int*)IOF_BUFFER_ADDR &=~BIT5;	
	*(int*)IOF_DRV0 &=~BIT5;
	*(int*)IOF_DRV1 &=~BIT5;

 //AD  input  IOF 9
	*(int*)LOW_POWER_Attrib |=LV_LINE;
	*(int*)LOW_POWER_Dir    &=~ LV_LINE;
	*(int*)LOW_POWER_Data   &=~LV_LINE;
	*(int*)LOW_POWER_Buffer &=~LV_LINE;	
	*(int*)LOW_POWER_DRV |=LV_LINE;
	
	*(int*)PWR_DETECT_Attrib|= PWR_DETECT;
	*(int*)PWR_DETECT_Dir   &=~ PWR_DETECT;
	*(int*)PWR_DETECT_Buffer&=~ PWR_DETECT;
	*(int*)PWR_DETECT_Data  &=~ PWR_DETECT;
	*(int*)PWR_DETECT_DRV |=PWR_DETECT;	

	R_ADC_SETUP = 0x8000;////init AD Reference Voltage enable, use 512 SYSCLK  MANUAL mode
  	R_ADC_MADC_CTRL =  0x8000;   //clear ready interrupt flag
	R_ADC_MADC_CTRL |= 0x0030;  //clear error flag1,2
//	R_ADC_MADC_CTRL |= 0x0000;  //ADC	line 0 enable
//	R_ADC_MADC_CTRL |= 0x4000;  //ready interrupt enable
    
//    R_NF_CTRL&=~BIT13;//释放IOF6 
//    R_ADC_USELINEIN&=~(BIT1|BIT0); //释放IOF6, IOF7
//    gpio_init_io(IO_F6, GPIO_OUTPUT);   
//    gpio_set_port_attribute(IO_F6, ATTRIBUTE_HIGH);   
//    gpio_write_io(IO_F6, DATA_HIGH); 

/*
	IR_TX_Attrib 		|=IR_TX_IO;
	IR_TX_Dir 			|=IR_TX_IO;	
	IR_TX_Buffer 		&=~IR_TX_IO;   
    IR_TX_Data 			&=~IR_TX_IO; 
//*/   
	delay();
//初始化fifo参数	
	Clear_Y_FIFO();
	Clear_X_FIFO();
	TouchTest=0;
	TouchRelease=0;
	Touchdelay=1;
	TouchFlag=0;
	Coordinate_X=WIDTH/2;
	Coordinate_Y=HIGHT/2;
	
	lv_chk_num=0;
}

        
void TP_IOInit(void)
{
#if TMP_MODE==0
//AD  input  IOF 6,7
	*(int*)IOF_ATTRIB_ADDR &=~TPIO_LINE;
	*(int*)IOF_DIR_ADDR    &=~TPIO_LINE;
	*(int*)IOF_BUFFER_ADDR |= TPIO_LINE;
	*(int*)IOF_DATA_ADDR   |= TPIO_LINE;
//IO output  IoB  10,11,12,13,
	*(int*)IOB_DRV1|= TPIO_XY;
	*(int*)IOB_DRV0|= TPIO_XY;
	*(int*)IOB_ATTRIB_ADDR|= TPIO_XY;
	*(int*)IOB_DIR_ADDR   |= TPIO_XY;
	*(int*)IOB_BUFFER_ADDR&= ~TPIO_XY;
	*(int*)IOB_DATA_ADDR  &= ~TPIO_XY;
#else
//AD  input  IOH 6,7
	*(int*)IOH_ATTRIB_ADDR &=~TPIO_LINE;
	*(int*)IOH_DIR_ADDR    &=~TPIO_LINE;
	*(int*)IOH_BUFFER_ADDR |= TPIO_LINE;
	*(int*)IOH_DATA_ADDR   |= TPIO_LINE;

//IO output  IoI 8-9  IOG 14-15
	*(int*)IOG_DRV1|= TPIO_X2;
	*(int*)IOG_DRV0|= TPIO_X2;
	*(int*)IOI_DRV1|= TPIO_Y2;
	*(int*)IOI_DRV0|= TPIO_Y2;
	*(int*)IOG_ATTRIB_ADDR|= TPIO_X2;
	*(int*)IOG_DIR_ADDR   |= TPIO_X2;
	*(int*)IOG_BUFFER_ADDR&= ~TPIO_X2;
	*(int*)IOG_DATA_ADDR  &= ~TPIO_X2;
	*(int*)IOI_ATTRIB_ADDR|= TPIO_Y2;
	*(int*)IOI_DIR_ADDR   |= TPIO_Y2;
	*(int*)IOI_BUFFER_ADDR&= ~TPIO_Y2;
	*(int*)IOI_DATA_ADDR  &= ~TPIO_Y2;
	
	
#endif

    
	AD_Init();
}

void set_led_light(INT8S m)
{
	INT8S light;

    
    if(m==0){
    	light=3;	
    }else if(m==1){
    	light=1;
    }else{
    	light=0;
    }
    
	gpio_init_io(TFT_LIGHT0, GPIO_OUTPUT);   
    gpio_set_port_attribute(TFT_LIGHT0, ATTRIBUTE_HIGH);

	if(light&BIT0){
	 
	    gpio_write_io(TFT_LIGHT0, DATA_HIGH);
	}else{
//		R_IOF_I_DATA&=~BIT15;  
//		R_IOF_O_DATA&=~BIT15;
		gpio_write_io(TFT_LIGHT0, DATA_LOW);
	}
	
	gpio_init_io(TFT_LIGHT1, GPIO_OUTPUT);   
    gpio_set_port_attribute(TFT_LIGHT1, ATTRIBUTE_HIGH);  
    	
	if(light&BIT1){
	    gpio_write_io(TFT_LIGHT1, DATA_HIGH);		
	}else{
		gpio_write_io(TFT_LIGHT1, DATA_LOW);
	}	
	
}


void ENCService1(void);
//int EncNum,EncNum1;
//int EncDateold=1;

void ENC_IOInit(void)
{ 
	*(int*)IOE_ATTRIB_ADDR &=~ENC_INPUT;
	*(int*)IOE_ATTRIB_ADDR |=ENC_OUTPUT;
	*(int*)IOE_DIR_ADDR    &=~ENC_INPUT;
	*(int*)IOE_DIR_ADDR    |=ENC_OUTPUT;
	*(int*)IOE_BUFFER_ADDR &=~(ENC_INPUT|ENC_OUTPUT);
	*(int*)IOE_DATA_ADDR   &=~(ENC_INPUT|ENC_OUTPUT);
	
   //time_base_setup(TIME_BASE_C,TMBCS_1024HZ,0,test);
   //time_base_stop(TIME_BASE_C);
	//timer_freq_setup(2,4096, 0, ENCService1);
	//触发	
	//*(int*)IOE_BUFFER_ADDR |= ENC_OUTPUT;
	//timer_stop(2);
	Encflag=0;
	//EncNum=0;
	//EncNum1=0;
	//EncDateold=1;
}

void initI2c()
{
     P_I2C_Attrib 	|= P_I2C_SDA|P_I2C_SCL;
     P_I2C_Dir 		|= P_I2C_SDA|P_I2C_SCL;
     P_I2C_Data		|=( P_I2C_SDA|P_I2C_SCL);
     P_I2C_BUFF		|=( P_I2C_SDA|P_I2C_SCL);
}

//==============================================================================
#define DATABUFF 25

int ReadXData(void)
{
	unsigned int temp;
	int i;
	unsigned int X[DATABUFF]={0};
	unsigned int MIN,MAX;
	READ_X_IO;
	READ_X_AD;
	delay();delay();delay(); 

	while((R_ADC_MADC_CTRL&0x0080)==0){;}// wait convert  over
	
	//the data is ready
	i=DATABUFF-1;
	do//  gain 7 datas
	{
		delay();
		R_ADC_MADC_CTRL|=0x8000;
		delay();

		//start AD conversion
		R_ADC_MADC_CTRL|=0x0040;
		delay();

		while((R_ADC_MADC_CTRL&0x0080)==0){
			;
		}// wait convert  over
		delay();

		temp = R_ADC_MADC_DATA;  //get AD data
		
		X[i]	=	temp>>4;       // gain  high 10 bit
		i--;
	}while(i>=0);
	
	
	//去掉最大，最小值
	MIN=X[DATABUFF-2];
	MAX=MIN;
	temp=MIN;
	i=DATABUFF-2-1;
	do
	{
	  if(MAX<X[i]){
	  	MAX=X[i];	
	  }
	  if(MIN>X[i]){
	  	MIN=X[i];
	  }
	  temp+= X[i];
	  i--;
	}while(i>=0);
	
	/*
	temp=0;
	i=DATABUFF-1-1;
	do//get the average of last 4 datas
	{
		temp+= X[i];
		i--;
	}while(i>=0); */
	
	R_ADC_MADC_CTRL |= 0x8000;	//set ready status
	return (temp-MIN-MAX)/(DATABUFF-3);
	
}

int ReadYData(void)
{
	unsigned int temp;
	int i;
	unsigned int Y[DATABUFF]={0};
	unsigned int MIN,MAX;
	
	READ_Y_IO;
	READ_Y_AD;

	delay();delay();delay();
	while((R_ADC_MADC_CTRL&0x0080)==0){;}// wait convert  over
	
	//the data is ready
	i=DATABUFF-1;
	do//  gain 7 datas
	{
		delay();
		R_ADC_MADC_CTRL|=0x8000;
		delay();

		//start AD conversion
		R_ADC_MADC_CTRL|=0x0040;
		delay(); 
		
		while((R_ADC_MADC_CTRL&0x0080)==0){
			;
		}// wait convert  over
		delay();

		temp = R_ADC_MADC_DATA;  //get AD data
		
		Y[i] = temp>>4;// gain  high 10 bit
		i--;
	}while(i>=0);
	//去掉最大，最小值
	MIN=Y[DATABUFF-2];
	MAX=MIN;
	temp=MIN;
	i=DATABUFF-2-1;
	do
	{
	  if(MAX<Y[i]){
	  	MAX=Y[i];	
	  }
	  if(MIN>Y[i]){
	  	MIN=Y[i];
	  }
	  temp+= Y[i];
	  i--;
	}while(i>=0);
	/*
	temp=0;
	i=DATABUFF-1-1;
	do//get the average of last 4 datas
	{
		temp+= Y[i];
		i--;
	}while(i>=0);  */

	R_ADC_MADC_CTRL |= 0x8000;	//set ready status
	
	return (temp-MIN-MAX)/(DATABUFF-3);
	
}

//---------------------------------------------------------------
void  In_X_FIFO(unsigned int x)
{
	*p_X_FIFO	=	x;

	p_X_FIFO++;

	if((p_X_FIFO - _X_FIFO) >= FIFOSize)
	{
		p_X_FIFO	=	_X_FIFO;
		_X_FullFlag	=	1;// show FIFO is full
	}
}
void  In_Y_FIFO(unsigned int x)
{
	*p_Y_FIFO	=	x;

	p_Y_FIFO++;

	if((p_Y_FIFO - _Y_FIFO) >= FIFOSize)
	{
		p_Y_FIFO	=	_Y_FIFO;
		_Y_FullFlag	=	1;// show FIFO is full
	}
}
//---------------------------------------------------------------

unsigned int   Average_X_FIFO(void)
{
	unsigned int sum;
	int i;
    unsigned int MAX,MIN;
	sum	= 0;

#if FIFOSize > 2
    //去掉最大，最小值
	MIN=_X_FIFO[FIFOSize-1];
	MAX=MIN;
	
	i=FIFOSize-2;
	do
	{
	  if(MAX<_X_FIFO[i]){
	  	MAX=_X_FIFO[i];	
	  }
	  if(MIN>_X_FIFO[i]){
	  	MIN=_X_FIFO[i];
	  }
	  i--;
	}while(i>=FIFOREAD);
#endif
	
	for(i = FIFOREAD; i < FIFOSize;	i++)
	{
		sum	+= _X_FIFO[i];
	}
	
	//去掉一个最大值
    //去掉一个最小值
#if FIFOSize > 2  
    
    
	return (sum-MAX-MIN)/(FIFOSize-FIFOREAD-2);
#else 
	return (sum)/(FIFOSize-FIFOREAD);
#endif
}

unsigned int   Average_Y_FIFO(void)
{
	int i;
	unsigned int sum;
    unsigned int MAX,MIN;
    
	sum	= 0;
#if FIFOSize > 2	
    //去掉最大，最小值
	MIN=_Y_FIFO[FIFOSize-1];
	MAX=MIN;
	
	i=FIFOSize-2;
	do
	{
	  if(MAX<_Y_FIFO[i]){
	  	MAX=_Y_FIFO[i];	
	  }
	  if(MIN>_Y_FIFO[i]){
	  	MIN=_Y_FIFO[i];
	  }
	  i--;
	}while(i>=FIFOREAD);
#endif
	for(i = FIFOREAD; i < FIFOSize;	i++)
	{
		sum	+= _Y_FIFO[i];
	}
#if FIFOSize > 2
	return (sum-MAX-MIN)/(FIFOSize-FIFOREAD-2);
#else
    return (sum)/(FIFOSize-FIFOREAD);
#endif
}

//---------------------------------------------------------------
void  Clear_Y_FIFO(void)
{
	unsigned int i;

	p_Y_FIFO	=	_Y_FIFO;/// Init the point

	_Y_FullFlag	=	0;//

	for(i	=	0;	i < FIFOSize; i++)//  clear the data in the FIFO
	{
		_Y_FIFO[i]	=	0;
	}
}
void  Clear_X_FIFO(void)
{
	unsigned int i;

	p_X_FIFO	=	_X_FIFO;/// Init the point

	_X_FullFlag	=	0;//

	for(i	=	0;	i < FIFOSize; i++)//  clear the data in the FIFO
	{
		_X_FIFO[i]	=	0;
	}
}

void VariableInit(void)
{
	PressDown	=	0;
	ResponseFlag	=	0;
	TouchFlag = 0;
	TouchFlags = 0;
    touch=0;
	Clear_X_FIFO();
	Clear_Y_FIFO();
}

//---------------------------------------------------------------
#if TMP_MODE==0
	#define  NOTOUCH  2000
	int XCenter=3026,DivValueX=413;
	int YCenter=3569,DivValueY=298;	
#else
	#define  NOTOUCH  1000
	int XCenter=3375,DivValueX=441;
	int YCenter=3027,DivValueY=513;	

#endif


int Ymultiple=100;
int Xmultiple=100;



void TouchPanelService(void)
{
	int _X,_Y,t;
	int XDiff,YDiff;
	int i,m;

#if TMP_MODE==0
	//*
	*(int*)IOB_ATTRIB_ADDR&= ~ TPIO_X0;
	*(int*)IOB_DIR_ADDR   &= ~(TPIO_Y0 | TPIO_Y1 |TPIO_X0);
	*(int*)IOB_DIR_ADDR   |= (TPIO_X1 );

	*(int*)IOB_BUFFER_ADDR = (*(int*)IOB_BUFFER_ADDR&(~TPIO_XY))|TPIO_X0;
	if( (*(int*)IOB_DATA_ADDR & TPIO_X0) ){
		t++;
		t++;
	}
	*(int*)IOB_ATTRIB_ADDR|=  TPIO_X0;
	//*/

#else
	//*
	*(int*)IOG_ATTRIB_ADDR&= ~ TPIO_X0;
	*(int*)IOG_DIR_ADDR   &= ~(TPIO_X0);
	*(int*)IOI_DIR_ADDR   &= ~(TPIO_Y0 | TPIO_Y1);
	*(int*)IOG_DIR_ADDR   |= (TPIO_X1 );

	*(int*)IOI_BUFFER_ADDR = (*(int*)IOI_BUFFER_ADDR&(~TPIO_Y2));
	*(int*)IOG_BUFFER_ADDR = (*(int*)IOG_BUFFER_ADDR&(~TPIO_X2))|TPIO_X0;
	if( (*(int*)IOG_DATA_ADDR & TPIO_X0) ){
		t++;
		t++;
	}
	*(int*)IOG_ATTRIB_ADDR|=  TPIO_X0;
	//*/

#endif

 	while(1)
 	{
//	i=7;
	_Y=ReadYData();

	if(_Y< NOTOUCH){		//表示没有碰屏
		if(Touchdelay)Touchdelay--;
		else{		
			TouchRelease=TouchFlag;
			VariableInit();
		}
//		TouchFlag=0;
		TouchFlags=0;
		return;
	}
	
//	do{
//		_Y+=(ReadYData());
//		i--;
//	}while(i>0);
//	_Y=_Y>>3;

//	i=7;
	_X=ReadXData();
//	do{
//		_X+=ReadXData();
//		i--;
//	}while(i>0);
//	_X=_X>>3;

//------------------------------------------	
	if(TouchTest){ //just 时候有作用
		In_X_FIFO(_X);
		In_Y_FIFO(_Y);
		if(_X_FullFlag) TouchFlag = 1;
		return;
	}
	_Y=_Y-YCenter;
	_X=_X-XCenter;
	
	_X=_X*Xmultiple/DivValueX;
	_Y=_Y*Ymultiple/DivValueY;
//	if(_X<200) _X=_X-5*(200-_X)/200;
	_X=_X+2;
	_Y=_Y+2;

	_X+=WIDTH/2;
	_Y+=HIGHT/2;

	if(_X<0) _X=0;
	else if(_X>WIDTH) _X=WIDTH;
	if(_Y<0) _Y=0;
	else if(_Y>HIGHT) _Y=HIGHT;

	if(PressDown	==	0){			//status from not touch to touched
		PressDown++;
		LastX	=	_X;
		LastY	=	_Y;
		ResponseFlag =	1;
	}else {                  		//last status is touched
		XDiff	=	LastX	-	_X; 
		YDiff	=	LastY	-	_Y;
		if(XDiff <3 && XDiff > -3 && YDiff <3 && YDiff > -3)	//  if the difference is less the 2 ,response last coordinate
		{
			_X = LastX;
			_Y = LastY;
			ResponseFlag	=	1;
		}else {
			LastX	=	_X;
			LastY	=	_Y;
			ResponseFlag =	1;
		}
	}

	if(ResponseFlag)
	{
		In_X_FIFO(_X);
		In_Y_FIFO(_Y);
	}
	else
	{
		Clear_X_FIFO();
		Clear_Y_FIFO();
	}
	
	if(ResponseFlag&&_X_FullFlag) //the X Y value is right
	{
		if(TouchFlag==0) TouchFlags=1;
		else TouchFlags=0;
		
		Coordinate_X = Average_X_FIFO();
		Coordinate_Y = Average_Y_FIFO();
		TouchFlag = 1;  //set the TouchFlag
		TouchRelease=0;
		Touchdelay=0;
		touch=1;

		return;
	}
}
}

//==========================================================================
INT16S TestTouch(int x0,int x1,int y0,int y1)
{
	if(Coordinate_X>x0 && Coordinate_X<x1 && Coordinate_Y>y0 && Coordinate_Y<y1) return 1;
	else return 0;
}

INT16S TestTouchs(int x0,int x1,int y0,int y1)
{
	if(TouchFlags==0) return 0;
	if(Coordinate_X>x0 && Coordinate_X<x1 && Coordinate_Y>y0 && Coordinate_Y<y1) return 1;
	else return 0;
}

void WaitNoTouch()
{
	while(1){
		PaintTFT_cell4();
		if(TouchFlag==0 && TouchRelease==0) break;
	}
}
void WaitNoTouch_go()
{
	while(1){
		PaintTFT_go();
		if(TouchFlag==0 && TouchRelease==0 && KeyBoardCode==0) break;
	}
}
INT16S TestDragRel(INT16S dx[],INT16S dy[])
{
	if(TouchFlag){
		if(TouchFlags || DragOld_Y==-1) {
			DragOld_X=Coordinate_X;
			DragOld_Y=Coordinate_Y;
			dx[0]=dy[0]=0;
		}
		dx[0]+=Coordinate_X-DragOld_X;
		dy[0]+=Coordinate_Y-DragOld_Y;
		DragOld_X=Coordinate_X;
		DragOld_Y=Coordinate_Y;
		return 0;
	}else if(TouchRelease){
		return 1;
	}else{
		DragOld_Y=-1;
	}
	return -1;
}

//==========================================================================
#define TOUCH_TIME		1
#define	Drag_DXDY		2
INT16S Touch_TempX,Touch_TempY;
INT16S Touch_Time;
//0-None  1-Release  2--Drag left(++),	3--Drag right(--) 4==UP   5==DOWN
INT16S TestReleaseState()
{
	INT16S m;
	static INT16S dx,dy;
	INT16S DX,DY;
	
	m=TestDragRel(&dx,&dy);
	if(m==1){
		if(dx<0) DX=-dx;
		else DX=dx;
		if(dy<0) DY=-dy;
		else DY=dy;

		Coordinate_X=Touch_TempX;
		Coordinate_Y=Touch_TempY;
		Touch_TempX=0;
		Touch_TempY=0;
		
		if(DX>=DY && DX>=Drag_DXDY){
			if(dx<0) return 2;
			else return 3;
		}else if(DY>DX && DY>=Drag_DXDY){
			if(dy<0) return 4;
			else return 5;
		}else if(Touch_Time>=TOUCH_TIME){
			return 1;
		}
		sleeptime=WAITINGTIME;
		return 0;
	}else if(m==0){
		Touch_Time++;
		if(Touch_Time==1){
			Touch_TempX=Coordinate_X;
			Touch_TempY=Coordinate_Y;
		}
		return 0;
	}else{
		Touch_TempX=0;
		Touch_TempY=0;
		Touch_Time=0;
		return -1;
	}
}

INT16S mov_len;
INT16S mov_dxy;
INT16S DragDelay;

INT16S TestDragRel_2(INT16S HV,INT16S Max_V)
{
	if(TouchFlag){
	
		if(DragOld_Y==-1){
			DragOld_X=Coordinate_X;
			DragOld_Y=Coordinate_Y;
			DragDelay=0;
			mov_dxy=0;
			mov_len=0;
		}			
		
		if(HV){
			mov_dxy=Coordinate_Y-DragOld_Y;
			DragOld_Y=Coordinate_Y;
		}else{
			mov_dxy=Coordinate_X-DragOld_X;
			DragOld_X=Coordinate_X;
		}
			
		DragDelay++;
		mov_len+=mov_dxy;
		
		return 0;
	}else{
		if(game_time%5==0){
			if(mov_dxy>0)mov_dxy--;		
			else if(mov_dxy<0)mov_dxy++;  
		}
	}
	
	if(TouchRelease){

		mov_dxy=0;
		
		if(DragDelay<10){
			mov_dxy=mov_len;	
			if(Max_V>0){
				if(mov_dxy>Max_V){
					mov_dxy=Max_V;
				}else if(mov_dxy<-Max_V){
					mov_dxy=-Max_V;
				}
			}
		}

		DragDelay=0;
		
		return 1;
	}
	
	DragOld_Y=-1;
	return -1;
}

//==============================================================================
void Enc(void)
{
	int i;
	for(i=0;i<5;i++){
		ENCService();
		if(Encflag==0){
			break;
		}
	}	
}

void ENCService(void)
{
	int n,m=1,k=0,kk=0;
	int num=10;
	*(int*)IOE_DIR_ADDR|= ENC_OUTPUT;
	*(int*)IOE_ATTRIB_ADDR |=ENC_OUTPUT;
	*(int*)IOE_BUFFER_ADDR |= ENC_OUTPUT;
	delay(); delay();delay();
	while(1)
	{
		n=*(int*)IOE_DATA_ADDR & ENC_INPUT;
		if(n) break;
		else{
			kk++;
			if(kk>10000){
				*(int*)IOE_BUFFER_ADDR &=~ ENC_OUTPUT;
				delay(); 
				*(int*)IOE_BUFFER_ADDR |= ENC_OUTPUT;
				
				delay(); 
				kk=0;
				if(num){
					num--;
				}else{
					Encflag=1;
					return;
				}
			}
		}
	}
	
	kk=0;
	while(1){
		n=*(int*)IOE_DATA_ADDR & ENC_INPUT;
		if(n!=m){
			k++;
			m=n;
		}else kk++;
		if(k>=10){
			kk/=k;
			if(kk>7000 && kk<16185 ){
				Encflag=0;			
				break;		//12450
			}else{
				Encflag=1;
				break;
				//tv_disable();
//				t80_umount();
//				while(1);
			}
		}else if(kk>16185*k){
			Encflag=1;	 //失败
			break;
		}
	}
	*(int*)IOE_BUFFER_ADDR &=~ ENC_OUTPUT;	 

}

//==============================================================================

void Hex2Bcd(int n)
{
	INT16U i;
	INT16U iData;
	for(i=0;i<n;i++){
		iData=(I2C_Data[i]/16)*10;
		I2C_Data[i]=iData+I2C_Data[i]%16;
	}
}
void Rt_RealseSda()
{
	P_I2C_Attrib &=(~P_I2C_SDA);
	P_I2C_Dir &=(~P_I2C_SDA);	//chang SDK user
}
void Rt_GetSda()
{
	P_I2C_Attrib |=P_I2C_SDA;
	P_I2C_Dir |=P_I2C_SDA;		//get SDK user
}
void I2C_sta( )/*启动I2C总线*/
{
	P_I2C_BUFF |=P_I2C_SDA|P_I2C_SCL;
	delay();
	P_I2C_BUFF &=P_I2C_SDA^0xffff;
	delay();
	P_I2C_BUFF &=P_I2C_SCL^0xffff;
	delay();
}
void I2C_stop()
{
	P_I2C_BUFF &=P_I2C_SDA^0xffff;
	delay();
	P_I2C_BUFF |=P_I2C_SCL;
	delay();
	P_I2C_BUFF |=P_I2C_SDA;
	delay();
}
void I2C_mack( )/*发送应答位*/
{
	P_I2C_BUFF &=P_I2C_SDA^0xffff;
	delay();
	P_I2C_BUFF |=P_I2C_SCL;
	delay();
	P_I2C_BUFF &=P_I2C_SCL^0xffff;
	delay();
}

void I2C_mnack( )/*发送非应答位*/
{
	P_I2C_BUFF |=P_I2C_SDA;
	delay();
	P_I2C_BUFF |=P_I2C_SCL;
	delay();
	P_I2C_BUFF &=P_I2C_SCL^0xffff;
	delay();
}
int I2C_cack()
{
	int ack=1;
	P_I2C_BUFF |=P_I2C_SDA;		//SDK=1
	
	Rt_RealseSda();
	
	P_I2C_BUFF |=P_I2C_SCL;
	delay();
	if(P_I2C_Data &P_I2C_SDA) ack=0;
	delay();
	P_I2C_BUFF &=(P_I2C_SCL^0xffff);

	Rt_GetSda();
	return ack;
}
void I2C_wrbyt(int shu)
{
	int i;
	for(i=0;i<8;i++){
		if(shu&0x80) P_I2C_BUFF |=P_I2C_SDA;
		else P_I2C_BUFF &=P_I2C_SDA^0xffff;
		delay();
		P_I2C_BUFF |=P_I2C_SCL;
		delay();
		P_I2C_BUFF &=P_I2C_SCL^0xffff;
		shu=shu<<1;
	}
}
int I2C_rdbyt()
{
	int data=0;
	int i;
	
	P_I2C_BUFF |=P_I2C_SDA;
	Rt_RealseSda();
	for(i=0;i<8;i++){
		P_I2C_BUFF |=P_I2C_SCL;		//SCL=1
		delay();
		if(P_I2C_Data &P_I2C_SDA) data=data*2+1;
		else data=data*2;
		P_I2C_BUFF &=(P_I2C_SCL^0xffff);
		delay();
	}
	Rt_GetSda();
	return data;
}

void I2c_Write(INT16U SA,INT16U SubAddr,INT16U n)
{
	int ack;
	int i,m=0;
	do{
		I2C_sta();
		I2C_wrbyt(SA);
		ack=I2C_cack();
		m++;
		if(m==10) break;
	}while(ack==0);
	if(m==10) I2C_Flag=0xff;
	else{
		I2C_wrbyt(SubAddr);
		I2C_cack();
		for(i=0;i<n;i++){
			I2C_wrbyt(I2C_Data[i]);
			I2C_cack();
		}
		I2C_Flag=0;
	}
	I2C_stop();
}

void I2c_Read(INT16U SA,INT16U SubAddr,INT16U n)
{
	int ack;
	int i,m=0;
	do{
		I2C_sta();
		I2C_wrbyt(SA);
		ack=I2C_cack();
		m++;
		if(m==10) break;
	}while(ack==0);
	I2C_wrbyt(SubAddr);
	I2C_cack();
	do{
		I2C_sta();
		I2C_wrbyt(SA+1);
		ack=I2C_cack();
		m++;
		if(m>=10) break;
	}while(ack==0);
	if(m>=10) I2C_Flag=0xff;
	else{
		for(i=0;i<n;i++){
			I2C_Data[i]=I2C_rdbyt();
			if(i<n-1) I2C_mack();
			else I2C_mnack();
		}
		I2C_Flag=0;
	}
	I2C_stop();
}

void TestE2Flash()
{
	int i;
	if(E2_Flag<64){
		if(E2_Flag%2==0){
			for(i=0;i<8;i++){
				I2C_Data[i]=78+i;
			}
			I2c_Write(I2c_ID,(E2_Flag/2)*8,8);
		}else{
			I2c_Read(I2c_ID,(E2_Flag/2)*8,8);
			for(i=0;i<8;i++){
				if(I2C_Data[i]!=78+i) I2C_Flag=0xff;
			}
		}
		if(I2C_Flag) E2_Flag=100;
		else E2_Flag++;
	}
}
void TestE2Flash1()
{
	int i;
	if(E2_Flag<750){
		if(E2_Flag<125){
			for(i=0;i<8;i++){
				I2C_Data[i]=0x55;
			}
			I2c_Write(I2c_ID,E2_Flag*8,8);
		}else if(E2_Flag<250){
			I2c_Read(I2c_ID,(E2_Flag-125)*8,8);
			for(i=0;i<8;i++){
				if(I2C_Data[i]!=0x55) I2C_Flag=1;
			}
		}else if(E2_Flag<375){
			for(i=0;i<8;i++){
				I2C_Data[i]=0xAA;
			}
			I2c_Write(I2c_ID,(E2_Flag-250)*8,8);
		}else if(E2_Flag<500){
			I2c_Read(I2c_ID,(E2_Flag-375)*8,8);
			for(i=0;i<8;i++){
				if(I2C_Data[i]!=0xAA) I2C_Flag=1;
			}
		}else if(E2_Flag<625){
			for(i=0;i<8;i++){
				I2C_Data[i]=0;
			}
			I2c_Write(I2c_ID,(E2_Flag-500)*8,8);		
		}else{
			I2c_Read(I2c_ID,(E2_Flag-625)*8,8);
			for(i=0;i<8;i++){
				if(I2C_Data[i]!=0) I2C_Flag=1;
			}		
		}
		

		if(I2C_Flag) E2_Flag=1000;
		else E2_Flag++;
	}
}
//============================================================================
int ReadLvData(void)
{
	unsigned int temp;
	int i;
	unsigned int X[8]={0};
	
	READ_LV_AD;
	delay();delay();delay();
	while((R_ADC_MADC_CTRL&0x0080)==0){;}// wait convert  over
	
	//the data is ready
	i=0;
	do//  gain 7 datas
	{
		delay();
		R_ADC_MADC_CTRL|=0x8000;
		delay();

		//start AD conversion
		R_ADC_MADC_CTRL|=0x0040;
		delay();

		while((R_ADC_MADC_CTRL&0x0080)==0){
			;
		}// wait convert  over
		delay();

		temp = R_ADC_MADC_DATA;  //get AD data
		
		X[i]	=	temp>>4;       // gain  high 10 bit
		i++;
	}while(i<7);
	
	temp=0;
	i=3;
	do//get the average of last 4 datas
	{
		temp+= X[i];
		i++;
	}while(i<7);
	R_ADC_MADC_CTRL |= 0x8000;	//set ready status
	return temp>>2;
}
INT8U Check_battery=0;
void InitCheckPower()
{	
	if((*(int*)IOB_DATA_ADDR)&BIT6){
		Check_battery=0;
	}else Check_battery=1;
}

INT16S old_power=50;
INT16S power_pop=0;

void GetLV()
{
	int m,k; 
	if(LV_FLAG){
	
		if(lv_chk_num){
			lv_chk_num--;						
		}	
		
		m=ReadLvData()*330/4095;
		if(m>=145){			//4.8 145
			LVLevel=3;
		}else if(m>=133){	//4.4V 133
			LVLevel=2;
		}else if(m>=122){	//4,1 122
			LVLevel=1;
			
		}else if(m<108){    //3,6v	108   //107 3.5
			if(LVLevel==-1) LVLevel--;
			else LVLevel=-1;
		}else{			
			LVLevel=0;
			//if(m<123) power_pop=1;
		}
		if(m<115){			//3.8  115
			if(old_power<=0){
				old_power=25*240;
				power_pop=1;
			}
		}
	}else   LVLevel=3;
}

//----------------------------------------------------
void InitCardin()
{
	*(int*)IOC_ATTRIB_ADDR&=~ P_CARDIN;
	*(int*)IOC_DIR_ADDR   &=~ P_CARDIN;
	*(int*)IOC_BUFFER_ADDR|= P_CARDIN;
	*(int*)IOC_DATA_ADDR  |= P_CARDIN;
	
	//*
	R_ADC_USELINEIN&=~(BIT4); //释放IOH6,
	*(int*)IOH_ATTRIB_ADDR|= 0x40;
	*(int*)IOH_DIR_ADDR   &=~ 0x40;
	*(int*)IOH_BUFFER_ADDR|= 0x40;
	*(int*)IOH_DATA_ADDR  |= 0x40;
	//*/
}
int TestCardin()
{
	*(int*)IOC_BUFFER_ADDR|= P_CARDIN;
	if(*(int*)IOC_DATA_ADDR & P_CARDIN) 
		return 0;
	else return 1;
}

//BIT5 液晶屏：1，亮0，关，
//BIT4：液晶屏需要拉高，
//BIT6，电源检测初始化拉高。没接电源时拉低：接了悬空;
void testIOHigh()
{
	INT32U i,ptr;
	
	*(int*)IOB_ATTRIB_ADDR|= BIT5;
	*(int*)IOB_DIR_ADDR   |= BIT5;
	*(int*)IOB_ATTRIB_ADDR|= BIT6;
	*(int*)IOB_DIR_ADDR   |= BIT6;

	//ptr=*(INT32U*)IOB_DATA_ADDR;
	
	//ptr&= ~BIT5;
	//ptr|=BIT8|BIT9;
    gpio_write_io(T430_RESET_PIN, DATA_LOW);
     gpio_write_io(T430_DSP_PIN, DATA_HIGH);
   
	//*(INT32U*)IOB_BUFFER_ADDR=ptr;

	R_NF_CTRL&=~BIT13;//释放IOF6 IOF7
    R_ADC_USELINEIN&=~(BIT1|BIT0); //释放IOF6, IOF7
	*(int*)IOF_ATTRIB_ADDR|= BIT6;
	*(int*)IOF_DIR_ADDR   |= BIT6;
	*(int*)IOF_BUFFER_ADDR&= ~BIT6;
	*(int*)IOF_DATA_ADDR  &= ~BIT6;
	*(int*)IOF_DRV0		|= BIT6;
	*(int*)IOF_DRV1  	|= BIT6;//*/
}

void TFT_DSP_ON()
{
	gpio_write_io(T430_RESET_PIN, DATA_HIGH);
//	gpio_write_io(T430_DSP_PIN, DATA_HIGH);	
}

void TFT_DSP_Off()
{
	tft_init();
	gpio_write_io(T430_RESET_PIN, DATA_LOW);
//	gpio_write_io(T430_DSP_PIN, DATA_LOW);	
}

void SetFreeIOB5_6()
{
	*(int*)0xC0000174|=BIT6;
	*(int*)0xC0000174&=~(BIT1|BIT5) ;
	*(int*)0xc0000108&= ~BIT13 ;
}

void SetWaveIo(INT16S val)
{
	//out put high
	*(int*)IOF_ATTRIB_ADDR|= BIT2;
	*(int*)IOF_DIR_ADDR   |= BIT2;
	if(!val){
		*(int*)IOF_BUFFER_ADDR|= BIT2;
		*(int*)IOF_DATA_ADDR  |= BIT2;
	}else{
		*(int*)IOF_BUFFER_ADDR&= ~BIT2;
		*(int*)IOF_DATA_ADDR  &= ~BIT2;
	}
}
