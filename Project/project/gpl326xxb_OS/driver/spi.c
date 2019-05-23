#include "Gsensor.h"
#include "project.h"
#include "touch_panel.h"
//==========================================================
// variable
//==========================================================
INT16S Mouse_X=240,Mouse_Y=136;

#define	 SPI_SCL   		IO_B15
#define	 SPI_SDA   		IO_B14

void spi_delay (INT32U i)
{
	INT32U j;

	for(j=0; j<8*i; j++);
}


void SPI_IO_init (
	void
){

	R_FUNPOS2 &=~0x10;
	*(int*)IOB_DRV1|= 0xc000;
	*(int*)IOB_DRV0|= 0xc000;

	//init IO
	gpio_set_port_attribute(SPI_SCL, ATTRIBUTE_HIGH);
	gpio_set_port_attribute(SPI_SDA, ATTRIBUTE_HIGH);
	gpio_init_io(SPI_SCL, GPIO_OUTPUT);				//set dir	 	
	gpio_init_io(SPI_SDA, GPIO_OUTPUT);				//set dir
	gpio_write_io(SPI_SCL, DATA_HIGH);					//SCL1		
	gpio_write_io(SPI_SDA, DATA_HIGH);					//SDA0	 
}

void Spi_w_phase (INT16U addr,INT16U value)
{
	INT32U i;
	INT32S ret = 0;

	gpio_write_io(SPI_SCL, DATA_HIGH);
	drv_msec_wait(10);

	for(i=0;i<8;i++){
		gpio_write_io(SPI_SCL, DATA_LOW);					//SCL0
		spi_delay (1);
		if(addr & 0x80) {
			gpio_write_io(SPI_SDA, DATA_HIGH);				//SDA1
		} else {
			gpio_write_io(SPI_SDA, DATA_LOW);				//SDA0
		}
		
		gpio_write_io(SPI_SCL, DATA_HIGH);					//SCL1
		spi_delay(1);
		addr <<= 1;
	}
	
	for(i=0;i<8;i++){
		gpio_write_io(SPI_SCL, DATA_LOW);					//SCL0
		spi_delay (1);
		if(value & 0x80) {
			gpio_write_io(SPI_SDA, DATA_HIGH);				//SDA1
		} else {
			gpio_write_io(SPI_SDA, DATA_LOW);				//SDA0
		}
		
		gpio_write_io(SPI_SCL, DATA_HIGH);					//SCL1
		spi_delay(1);
		value <<= 1;
	}
	gpio_write_io(SPI_SCL, DATA_HIGH);
}

INT16U Spi_r_phase (INT16U addr)
{
	INT32U i;
	INT16U value = 0x00;

	gpio_write_io(SPI_SCL, DATA_HIGH);
	drv_msec_wait(1);

	for(i=0;i<8;i++){
		gpio_write_io(SPI_SCL, DATA_LOW);					//SCL0
		spi_delay (1);
		if(addr & 0x80) {
			gpio_write_io(SPI_SDA, DATA_HIGH);				//SDA1
		} else {
			gpio_write_io(SPI_SDA, DATA_LOW);				//SDA0
		}
		
		gpio_write_io(SPI_SCL, DATA_HIGH);					//SCL1
		spi_delay(1);
		addr <<= 1;
	}

	gpio_init_io(SPI_SDA, GPIO_INPUT);						//SDA is Hi-Z mode
	spi_delay(1);
	
	for(i=0;i<8;i++) {
		gpio_write_io(SPI_SCL, DATA_LOW);					//SCL0
		spi_delay(1);
		gpio_write_io(SPI_SCL, DATA_HIGH);					//SCL1
		value <<= 1;
		value |=( gpio_read_io(SPI_SDA));
		spi_delay(1);
	}

	gpio_write_io(SPI_SCL, DATA_HIGH);
	gpio_init_io(SPI_SDA, GPIO_OUTPUT);
	return value;
}
void Spi_TR_phase()
{
	gpio_write_io(SPI_SCL, DATA_LOW);
	spi_delay(10);
	gpio_write_io(SPI_SCL, DATA_HIGH);
}

void Get_Mouse_State();
void Spi_TIMER()
{
	//time_base_stop(TIME_BASE_B);
	//time_base_setup(TIME_BASE_B, TMBBS_64HZ, 0, Get_Mouse_State);

	timer_freq_setup(TIMER_B, 120, 0, Get_Mouse_State);
}

//======================================================
INT16S Spi_init()
{
	INT16U	i=0,who_ami;
	
	//init i2c
	drv_msec_wait(100);
	SPI_IO_init();
	
	while(1){
		who_ami=Spi_r_phase(0x00);
		if(who_ami==0x30){
			who_ami=Spi_r_phase(0x01);
			Spi_w_phase(0x85,0xA0);
			Spi_w_phase(0x86,0x04);
			Spi_w_phase(0x89,0x5A);
			Spi_w_phase(0xDB,0x9A);
			Spi_w_phase(0xD1,0x09);
			Spi_w_phase(0xD3,0x48);
			Spi_w_phase(0x89,0x00);
			
			Spi_TIMER();
			return 0;
		}
		i++;
		if(i>3) return -1;
	}
}
INT8S X_old=0,Y_old=0;
void Get_Mouse_State()
{
	INT8S data;
	
	data=Spi_r_phase(0x02);
	if(data&0x80){
		data=Spi_r_phase(0x03);
		if(data>3 || data<-3){
			Mouse_X-=data/3;
		}
		data=Spi_r_phase(0x04);
		if(data>3 || data<-3){
			Mouse_Y+=data/3;
		}
	}
}
void Test_MouseXY()
{
	if(Mouse_X<0) Mouse_X=0;
	else if(Mouse_X>480) Mouse_X=480;
	if(Mouse_Y<0) Mouse_Y=0;
	else if(Mouse_Y>272) Mouse_Y=272;
}
