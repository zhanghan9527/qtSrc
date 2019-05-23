#include "application.h"

#define MainTaskStackSize 1024

//define the demo code
#define AUDIO_DECODE		      0
#define AUDIO_ENCODE		      0
#define IMAGE_CODEC				  0
#define VIDEO_DECODE		      0
#define	VIDEO_ENCODE		      0
#define MULTIMEDIA_DEMO		      0
#define	SACM_DECODE			      0
#define	SACM_ENCODE			      0
#define	USB_DEVICE_WEBCAME_DEMO	  0
#define IMAGE_ENCODE_BLOCK_RW	  0
#define IMAGE_ENCODE_BLOCK_READ	  0
#define GP326XXX_PPU_DEMO		  0
#define DYNAMICALLY_CLOCK_DEMO    0
#define ID3_TAG_DEMO              0
#define USBH_ISO_DEMO             0
#define FACE_DETECT_DEMO          0
#define COMAIR_TX_DEMO			  0	
#define	COMAIR_RX_DEMO            0
#define QRCODE_BARCODE_DEMO		  0
#define VR_DEMO					  0

#define PRINT_OUTPUT_NONE	0x00 
#define PRINT_OUTPUT_UART	0x01
#define PRINT_OUTPUT_USB	0x02

extern void Audio_Decode_Demo(void);
extern void Audio_Encode_Demo(void);
extern void Image_Codec_Demo(void);
extern void Video_Decode_Demo(void);
extern void Video_Encode_Demo(void);
extern void Platform_Demo_Code(void);
extern void Sacm_Decode_Demo(void);
extern void Sacm_Encode_Demo(void);
extern void Usb_Dvice_WebCam_Demo(void);
extern void encode_block_rw_demo(void);
extern void encode_block_demo(void);
extern void GPL32XXXX_PPU_Hblank_Demo(void);
extern void GPL326XXX_PPU_Demo(void);
extern void gpl32_dynamic_mclk_demo(void);
extern void id3_tag_demo(void);
extern void USBH_ISO_Demo(void);
extern void set_print_output_type(INT32U type);
extern void Object_Detect_demo(void);
extern void COMAIR_SendCmd_Demo(void);
extern void Comair_RX_Demo(void);
extern void BarCode_QRCode_Demo(void);
extern void VR_Demo(void);

INT32U free_memory_start, free_memory_end;
INT32U MainTaskStack[MainTaskStackSize];

void Main_task_entry(void *para)
{
	gpio_init();

	Main_OS();

	while(1);
}

void Debug_UART_Port_Enable(void)
{
#if _DRV_L1_UART==1	
	uart0_buad_rate_set(UART0_BAUD_RATE);
	uart0_tx_enable();
	uart0_rx_enable();	
#endif
}

void Main(void *free_memory)
{
	PowerOn();
	// Initialize Operating System
	os_init();			
			
	// Initialize drvier layer 1 modules
	drv_l1_init();
	SetFreeIOB5_6();
	testIOHigh();

	// Initiate Component Layer modules
	free_memory_start = ((INT32U) free_memory + 3) & (~0x3);	// Align to 4-bytes boundry
	free_memory_end = (INT32U) SDRAM_END_ADDR & ~(0x3);
	gplib_init(free_memory_start, free_memory_end);
	
	// Initialize driver Layer 2 modules
	drv_l2_init();
	
	// Enable UART port for debug
	Debug_UART_Port_Enable();
	
	//Configure the output type of debug message, NONE, UART, USB or both
	set_print_output_type(PRINT_OUTPUT_UART | PRINT_OUTPUT_USB);
	
	// Initialize platform
	platform_entrance(free_memory);
	
	// Create a user-defined task
	OSTaskCreate(Main_task_entry, (void *) 0, &MainTaskStack[MainTaskStackSize - 1], 32); 
	
	// Start running
	OSStart();
}
