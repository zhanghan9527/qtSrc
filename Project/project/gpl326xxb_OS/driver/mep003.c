#include "Gsensor.h"
#include "project.h"
#include "touch_panel.h"
#include "drv_l1_sensor.h"
//==========================================================
// variable
//==========================================================
#define MEP003_ID		0x30
#define MEP003_LED		0x4000		//IOE4  IOG14
#define MEP003_DATA		(*(int*)IOG_DATA_ADDR)&MEP003_LED
//======================================================
INT16S Mep003_init()
{
	INT8U	who_ami;
	
	//init i2c
	sccb_init();
	sccb_delay(200);

	*(int*)IOG_DIR_ADDR   &=~ (MEP003_LED);
	*(int*)IOG_ATTRIB_ADDR|= (MEP003_LED);
	*(int*)IOG_BUFFER_ADDR&=~(MEP003_LED);
	*(int*)IOG_DATA_ADDR  &=~(MEP003_LED);
	*(int*)IOG_DRV1|= MEP003_LED;
	*(int*)IOG_DRV |= MEP003_LED;
	
	*(int*)IOI_DRV1|= 2;
	*(int*)IOI_DRV |= 2;
	return 0;
}

INT32S sccb_write_Mep(
	INT8U id, 
	INT32U addr_bits, 
	INT32U addr
)
{
	INT8U temp, ack = 1;
	INT32S ret;

	sccb_lock();

	// 3-Phase write transmission
	// Transmission start 
	sccb_start();			
	
	// Phase 1, SLAVE_ID
	ret = sccb_w_phase(id, 1);	
	if(ret < 0) {
		goto __exit;
	}	
	
	// Phase 2, Register address
	while(addr_bits >= 8) {
		addr_bits -= 8;
		temp = addr >> addr_bits;
		ret = sccb_w_phase(temp, ack);
		if(ret < 0) {
			goto __exit;
		}
	}

__exit:
	// Transmission stop	
	sccb_stop();	
	sccb_unlock();
	return ret;
}


INT32S 
sccb_read_Mep(
	INT8U id, 
	INT32U bytes, 
	INT8U *data
)
{
	INT8U temp, ack = 1;
	INT32S ret;

	sccb_lock();
	
	// 2-Phase read transmission cycle
	// Transmission start
	sccb_start();	
	
	// Phase 1 (read)
	ret = sccb_w_phase(id | 0x01, 1);
	if(ret < 0) {
		goto __exit;
	}
	
	// Phase 2 (read)
	while(bytes >= 1) {
		bytes --;
		if(bytes) {
			ack = 1; //ACK
		} else {	
			ack = 0; //NACK
		}
		
		temp = sccb_r_phase(ack);
		(*data++)=temp;
	}
	
__exit:
	// Transmission stop
	sccb_stop();	
	sccb_unlock();

	return ret;
}

void Test_Mep003_I2C()
{
	INT16S ret;
	INT8U wData,rData[10];
	int i,no,data;
	
	Mep003_init();
	
	wData=8;
	while(1){
		gpio_write_io(SCCB_SCL, DATA_HIGH);					//SCL1		
		gpio_write_io(SCCB_SDA, DATA_HIGH);					//SDA0	 
		for(i=0;i<50;i++) Paintdelay();

		if(wData<8){
			ret=sccb_write_Mep(MEP003_ID,8,wData);
			gpio_write_io(SCCB_SCL, DATA_LOW);
		}else{
			for(i=0;i<10;i++) rData[i]=0;
			ret=sccb_write_Mep(MEP003_ID,8,wData);
			gpio_write_io(SCCB_SCL, DATA_LOW);
			for(i=0;i<3;i++) Paintdelay();
			ret=sccb_read_Mep(MEP003_ID,9,(INT8U*)&rData);			
		}
		for(i=0;i<100;i++) Paintdelay();
	}
}
INT16S Mep003_step=0,Mep003_time=0,Mep003_ok=0;
void Mep003_Server()
{
	if(Mep003_ok==1){
		if(Mep003_time) Mep003_time--;
		else Mep003_ok=0;
	}
}
void Mep003_Test()
{
	INT16S i,ret;
	INT8U wData,rData[10];
	
	if(ENC_FLAG==0) return;
	while(Mep003_ok!=1)
	{
		Paintdelay();
		switch(Mep003_step){
			case 0:
				//if(Mep003_time) Mep003_time--;
				//else 
				Mep003_step++;
			break;
			case 1:
				wData=8+GetRandom(4);
				for(i=0;i<10;i++) rData[i]=0;
				ret=sccb_write_Mep(MEP003_ID,8,wData);
				gpio_write_io(SCCB_SCL, DATA_LOW);
				Mep003_time=3;		//3
				Mep003_step++;
			break;
			case 2:
				if(Mep003_time) Mep003_time--;
				else{
					ret=sccb_read_Mep(MEP003_ID,3,(INT8U*)&rData);
					if(	(wData==9 && rData[0]=='D' && rData[1]=='O' && rData[2]=='Y') ||
						(wData==10 && rData[0]=='M' && rData[1]=='A' && rData[2]=='I') ||
						(wData==11 && rData[0]=='P' && rData[1]=='L' && rData[2]=='A') ||
						(rData[0]=='M' && rData[1]=='C' && rData[2]=='S') ){
						Mep003_ok=1;
						Mep003_step++;
						Mep003_time=1000;
					}else{
						Mep003_ok--;
						if(Mep003_ok<=-2){							
							ShowOffAnima();
						}else{
							Mep003_step=0;
							Mep003_time=100;
						}
					}
				}
			break;
			case 3:
				if(Mep003_ok==0){
					Mep003_step=0;
					Mep003_time=10;
				}
			break;
		}
	}
}

