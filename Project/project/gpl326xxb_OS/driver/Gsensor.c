#include "Gsensor.h"
#include "project.h"
#include "touch_panel.h"
//==========================================================
// variable
//==========================================================
#define GBUFF_SIZE		100
INT16S G_Idx;
INT16S GBuff_X[GBUFF_SIZE],GBuff_Y[GBUFF_SIZE],GBuff_Z[GBUFF_SIZE];


//======================================================
INT16S Gsensor_init()
{
	INT8U	who_ami;
	
	//init i2c
	drv_msec_wait(100);
	sccb_init();
	sccb_delay(200);
	
	sccb_read_Reg8Data8(Gsensor_ID,0x0f, &who_ami);
	if(who_ami!=0x09){
		return -1;
	}

	//init Gsensor
	sccb_write_Reg8Data8(Gsensor_ID,0x1B, 0x00);	
	sccb_delay(200);
	//set Gsensor  READ_MODE
	sccb_write_Reg8Data8(Gsensor_ID,0x1B, 0x40);
	sccb_write_Reg8Data8(Gsensor_ID,0x21, 0x02);	
	sccb_write_Reg8Data8(Gsensor_ID,0x1B, 0xC0);	
	sccb_delay(200);
	return 0;
}
//Gsensor -2g/+2g  data=-2048/+2048
INT16S Gsensor_ReadX()
{
	INT8U	rData1,rData2;
	INT16S tmp;
	sccb_read_Reg8Data8(Gsensor_ID,0x06, &rData1);
	sccb_read_Reg8Data8(Gsensor_ID,0x07, &rData2);
	tmp=(rData2<<4)+(rData1>>4);
	if(tmp>2048) return tmp-4096;
	else return tmp;	
}
INT16S Gsensor_ReadY()
{
	INT8U	rData1,rData2;
	INT16S tmp;
	sccb_read_Reg8Data8(Gsensor_ID,0x08, &rData1);
	sccb_read_Reg8Data8(Gsensor_ID,0x09, &rData2);
	tmp=(rData2<<4)+(rData1>>4);
	if(tmp>2048) return tmp-4096;
	else return tmp;	
}
INT16S Gsensor_ReadZ()
{
	INT8U	rData1,rData2;
	INT16S tmp;
	sccb_read_Reg8Data8(Gsensor_ID,0x0A, &rData1);
	sccb_read_Reg8Data8(Gsensor_ID,0x0B, &rData2);
	tmp=(rData2<<4)+(rData1>>4);
	if(tmp>2048) return tmp-4096;
	else return tmp;	
}

void Test_Gsensor_I2C()
{
	INT16S ret;
	INT8U	rData1,rData2;
	INT16S tmp;
	
	Gsensor_init();

	game_time=0;
	G_Idx=0;
	while(1){
		sccb_read_Reg8Data8(Gsensor_ID,0x06, &rData1);
		sccb_read_Reg8Data8(Gsensor_ID,0x07, &rData2);
		tmp=(rData2<<4)+(rData1>>4);
		if(tmp>2048) GBuff_X[G_Idx]=tmp-4096;
		else GBuff_X[G_Idx]=tmp;
		sccb_read_Reg8Data8(Gsensor_ID,0x08, &rData1);
		sccb_read_Reg8Data8(Gsensor_ID,0x09, &rData2);
		tmp=(rData2<<4)+(rData1>>4);
		if(tmp>2048) GBuff_Y[G_Idx]=tmp-4096;
		else GBuff_Y[G_Idx]=tmp;
		sccb_read_Reg8Data8(Gsensor_ID,0x0a, &rData1);
		sccb_read_Reg8Data8(Gsensor_ID,0x0b, &rData2);
		tmp=(rData2<<4)+(rData1>>4);
		if(tmp>2048) GBuff_Z[G_Idx]=tmp-4096;
		else GBuff_Z[G_Idx]=tmp;

		G_Idx=(G_Idx+1)%GBUFF_SIZE;
		game_time++;
	}
	
	while(1);

}
//=======================================================
#define CTPM_MAX	5
INT16S CTPM_Nums;
INT16S CTPM_States[CTPM_MAX],CTPM_PosX[CTPM_MAX],CTPM_PosY[CTPM_MAX];
INT16S CTPM_init()
{
	INT16S ret;
	
	//init i2c
	drv_msec_wait(100);
	sccb_init();
	sccb_delay(200);
	
	//init CTPM
	ret=sccb_write_Reg8Data8(CTPM_ID,0, 0);

	return ret;
}

void CTPM_Sever()
{
	INT16S ret;
	INT16U i,n,tmp;
	INT8U	rData1,rData2,rData3,rData4;

	ret=sccb_read_Reg8Data8(CTPM_ID,0x02, &rData1);
	if(ret<0) return;
	CTPM_Nums=rData1&0x0F;
	if(CTPM_Nums==0 || CTPM_Nums>5) return;
	for(i=0;i<5;i++){
		sccb_read_Reg8Data8(CTPM_ID,4*i+3, &rData1);
		sccb_read_Reg8Data8(CTPM_ID,4*i+4, &rData2);
		sccb_read_Reg8Data8(CTPM_ID,4*i+5, &rData3);
		sccb_read_Reg8Data8(CTPM_ID,4*i+6, &rData4);
		n=(rData3&0xf0)>>4;
		if(n>5) continue;
		CTPM_States[n]=(rData1&0xC0)>>6;
		CTPM_PosX[n]=((rData1&0x0f)<<8)+rData2;
		CTPM_PosY[n]=((rData3&0x0f)<<8)+rData4;
	}
}

void Test_CTPM_I2C()
{
	CTPM_init();
	
	while(1){
		Paintdelay();
		CTPM_Sever();		
	}
	
}
