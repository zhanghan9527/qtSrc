#include "driver_l1.h"
#include "drv_l1_sfr.h"

#define		EP0_Setup_PKT	    BIT0
#define		EP0_OUT_PKT		    BIT1
#define		EP0_OUT_NACK		BIT2
#define		EP0_IN_PKT_Clear	BIT3
#define		EP0_IN_NACK			BIT4
#define		EP0_Status_Clear	BIT5
#define		EP0_Status_NACK		BIT6
#define		BULK_IN_PKT_Clear	BIT7
#define		BULK_IN_NACK		BIT8
#define		BULK_OUT_PKT_Set	BIT9
#define		BULK_OUT_NACK		BIT10
#define		INT_IN_PKT_Clear	BIT11
#define		INT_IN_NACK			BIT12
#define		Suspend				BIT13
#define		Resume				BIT14
#define		USB_RESET			BIT15
//
#define		EP0_Setup_PKT_EN	BIT0
#define		EP0_OUT_PKT_EN		BIT1
#define     EP0_Out_PKT_READY   BIT2
//
#if  MCU_VERSION  >= GPL326XX
#define     EP0_MAX_SIZE             64 //usb2.0
#else
#define     EP0_MAX_SIZE             8  //usb1.1
#endif

#define		EP0_SETUP_Stage		        0x0001
#define		EP0_Data_Stage		        0x0002
#define		EP0_Status_Stage	        0x0003
#define		EP0_OUT_NACK_Status_Stage	0x0004
#define		EP0_IN_NACK_Status_Stage	0x0005
//MSDC
#define		SCSI_CBW_Stage		0x0004
#define		SCSI_Data_Stage		0x0005
#define		SCSI_CSW_Stage		0x0006
#define		Bulk_IN_Stall		0x0001
#define		Bulk_OUT_Stall		0x0002

#define		ISO_Data			0x0008

#define     DIR_H_To_D          0x0000
#define     DIR_D_To_H          0x0001   
#define		C_Device_Descriptor_Table		1
#define		C_Config_Descriptor_Table		2
#define		C_String0_Descriptor_Table		3
#define		C_String1_Descriptor_Table		4

#define		C_String2_Descriptor_Table		5
#define		C_String3_Descriptor_Table		6

#define		C_String4_Descriptor_Table		7
#define		C_String5_Descriptor_Table		8

#define		C_Interface_Descriptor_Table	8
#define		C_Endpoint_Descriptor_Table		9

//usb2.0
#define		C_Qualifier_Descriptor_Table	11
#define		C_OtherSpeed_Descriptor_Table	12	

#define		C_MaxLUN_Table					0x000A

#define     DEVDST_LENGTH  0


#define		BIT0		0x0001
#define		BIT1		0x0002
#define		BIT2		0x0004
#define		BIT3		0x0008
#define		BIT4		0x0010
#define		BIT5		0x0020
#define		BIT6		0x0040
#define		BIT7		0x0080
#define		BIT8		0x0100
#define		BIT9		0x0200
#define		BIT10		0x0400
#define		BIT11		0x0800
#define		BIT12		0x1000
#define		BIT13		0x2000
#define		BIT14		0x4000
#define		BIT15		0x8000
#define     INTERFACE0_MASK  0x3
#define     INTERFACE1_MASK  0xC
#define     INTERFACE2_MASK  0x30
#define     INTERFACE3_MASK  0xC0
#define     INTERFACE4_MASK  0x300


#define R_Timeout_count 0xf888;

//USB STD
extern unsigned int  R_USB_State_Machine ; 
extern unsigned int   USB_Status          ;
extern unsigned int   R_USB_Suspend       ;   
extern INT32U   s_usbd_user_string  ;
extern char     *user_string_pt;
extern INT32U   user_string_size;
//extern unsigned int   USBWriteProtect     ; 

extern INT8U	CDROMDelay;//huanghe 090604 for cdrom autorun 
extern INT64U	Start_CDROM_Tick;
/*
* Function Name :  usbd_interrupt_register
*
* Syntax : usbd_interrupt_register(void (*pt_function)(void));
*
* Purpose 	 :  User can call this function to register the function to handle ISR of USBD
*
* Parameters : <IN> none
*                             
*          	   <OUT> none  
* Return 	:	none
*             
*
* Note :
*/
extern void usbd_interrupt_register(void (*pt_function)(void));
/*
* Function Name :  usbd_ep0descriptor_register
*
* Syntax : void usbd_ep0descriptor_register(INT32U (*pt_function)(INT32U nTableType));
*
* Purpose 	 :  User can call this function to register the function to handle ep0descriptor
*
* Parameters :<IN>	 *pt_function: function pointer for ep0descriptor
*                             
*          				nTableType: type of table
*                             
*          		<OUT> none
* Return 	:	none
*             
*
* Note :
*
*/
extern void usbd_ep0descriptor_register(INT32U (*pt_function)(INT32U nTableType));
/*
* Function Name :  usbd_suspend_register
*
* Syntax : void usbd_suspend_register(void (*pt_function)(void));
*
* Purpose 	 :  User can call this function to register the function to handle SUSPEND
*
* Parameters : <IN>	*pt_function: function pointer for SUSPEND
*                             
*          	   <OUT> none  
* Return 	:	none
*             
*
* Note :
*
*/
extern void usbd_suspend_register(void (*pt_function)(void));
/*
* Function Name :  usbd_resume_register
*
* Syntax : void usbd_resume_register(void (*pt_function)(void));
*
* Purpose 	 :  User can call this function to register the function to handle RESUME
*
* Parameters : <IN>	*pt_function: function pointer for RESUME
*                             
*          		<OUT> none
* Return 	:	none
*             
*
* Note :
*
*/
extern void usbd_resume_register(void (*pt_function)(void));
/*
* Function Name :  usbd_set_configuration_register
*
* Syntax : usbd_set_configuration_register(void (*pt_function)(void));
*
* Purpose 	 :  User can call this function to register the function to SET CONFIGURATION
*
* Parameters : <IN>	*pt_function: function pointer for SET CONFIGURATION
*                             
*          		<OUT> none
* Return 	:	none
*             
*
* Note :
*/
extern void usbd_set_configuration_register(void (*pt_function)(void));
/*
* Function Name :  usbd_reset_register
*
* Syntax : usbd_reset_register(void (*pt_function)(void));
*
* Purpose 	 :  User can call this function to register the function to USB RESET
*
* Parameters : <IN>*pt_function: function pointer for USB RESET
*                             
*          		<OUT> none
* Return 	:	none
*             
*
* Note :
*/
extern void usbd_reset_register(void (*pt_function)(void));
/*
* Function Name :  usb_initial
*
* Syntax : void usb_initial(void);
*
* Purpose 	 :  User can call this function to initialize the variables and the USB device hardware
*                            
* Parameters : <IN> none
*                             
*          		<OUT> none
* Return 	:	none
*             
*
* Note :
*/
extern void usb_initial(void);
/*
* Function Name :  usb_uninitial
*
* Syntax : void usb_uninitial(void);
*
* Purpose 	 :  User can call this function to uninitialize the variables and the USB device hardware
*
* Parameters : <IN>none
*                     
*          	   <OUT> none   
* Return 	:None
*             
*
* Note :
*
*/
extern void usb_uninitial(void);
/*
* Function Name :  usb_reset
*
* Syntax : void usb_reset(void);
*
* Purpose 	 :  User can call this function to reset the USB device hardware and also set the state machine to initial state
*
* Parameters : <IN>none
*                     
*          	   <OUT> none  
* Return 	:None
*             
*
* Note :
*
*/
extern void usb_reset(void);
/*
* Function Name :  usb_isr
*
* Syntax : void usb_isr(void);
*
* Purpose 	 :  User can call this function to handle to ISR of USB device
*
* Parameters : <IN>none
*                     
*          	   <OUT> none  
* Return 	:None
*             
*
* Note :
*
*/
extern void usb_isr(void);
/*
* Function Name :  usb_std_service
*
* Syntax : void usb_std_service(INT32U unUseLoop);
*
* Purpose 	 :  User can call this function to handle the EP0 SETUP command
*
* Parameters : <IN>	unUseLoop: 0: call once then return;
*            				     		    1: call will not return until receiving SUSPEND                          
*          		
*          		<OUT> none
* Return 	:	none
*             
*
* Note :
*
*/
extern void usb_std_service(INT32U unUseLoop);
/*
* Function Name :  Read_EP0_FIFO
*
* Syntax : void Read_EP0_FIFO(void);
*
* Purpose :  User can call this function to read data from EP0, this function will receive EP0 data
*
* Parameters : <IN> path: file path
*                              open_flag: file properties
*          	   <OUT> none  
* Return : file handle if successful; 
*              -1 if it fails to open or create a file
*
* Note :
*
*/
extern void Read_EP0_FIFO(void);
/*
* Function Name :  usbd_set_trigger
*
* Syntax : void usbd_set_trigger(void);
*
* Purpose 	 :  User can call this function to set USB device trigger
*
* Parameters : <IN> none
*                             
*          	   <OUT> none  
* Return 	:	none
*             
*
* Note :
*
*/
extern void usbd_set_trigger(void);
/*
* Function Name :  USB_CheckRegister_Complete
*
* Syntax : INT32U USB_CheckRegister_Complete(volatile INT32U *RegisterAddr,INT32U CheckValue,INT32U CheckResult);
*
* Purpose :  User can call this function to check register value. To check if the value of the RegisterAddr is the same as CheckValue, 
*		     also the timeout is implemented in this function.imeout count about 10ms
* Parameters : <IN>	RegisterAddr: address of register
*                     		CheckValue: value to check
*                     		CheckResult: result of check
*          	   <OUT> none  
* Return : 0: check ok
*              1: check fail
*
* Note :
*
*/
extern unsigned int   USB_CheckRegister_Complete(volatile unsigned int *RegisterAddr,unsigned int CheckValue,unsigned int CheckResult);
/*
* Function Name :  USB_CheckRegister_Complete_Long
*
* Syntax : INT32U USB_CheckRegister_Complete_Long(volatile INT32U *RegisterAddr,INT32U CheckValue,INT32U CheckResult);
*
* Purpose :  User can call this function to check register value. To check if the value of the RegisterAddr is the same as CheckValue, 
*		     also the timeout is implemented in this function. timeout count about 2S
* Parameters : <IN>	RegisterAddr: address of register
*                     		CheckValue: value to check
*                    		CheckResult: result of check
*          	   <OUT> none  
* Return : 0: check ok
*              1: check fail
*
* Note :
*
*/
extern unsigned int   USB_CheckRegister_Complete_Long(volatile unsigned int *RegisterAddr,unsigned int CheckValue,unsigned int CheckResult);
/*
* Function Name :  usb_os_event_init
*
* Syntax : INT32S usb_os_event_init(void);
*
* Purpose :  to create OSMbox
*
* Parameters : <IN>none
*                          
*          	   <OUT> none  
* Return :  0   STATUS OK; 
*              -1 STATUS FAIL
*
* Note :
*
*/
extern INT32S usb_os_event_init(void);
/*
* Function Name :  usb_os_event_uninit
*
* Syntax : void usb_os_event_uninit(void);
*
* Purpose :  to Del OSMbox
*
* Parameters : <IN>none
*                          
*          	   <OUT> none  
* Return :  None 
*              
*
* Note :
*
*/
extern void usb_os_event_uninit(void);
/*
* Function Name :  Drv_USBD_AUDIO_ISOIN_DMA_START
*
* Syntax : INT32S Drv_USBD_AUDIO_ISOIN_DMA_START(INT8U *buf, INT32U len);
*
* Purpose :  for set USB AUDIO ISOIN data start DMA
*
* Parameters :<IN>	 *buf : buffer pointer of DMA start address
*                      		len  :data length
*          	   <OUT> none   
* Return : 1 : had set USB AUDIO ISOIN data start DMA
*              
*
* Note :
*
*/
extern INT32S Drv_USBD_AUDIO_ISOIN_DMA_START(INT8U *buf, INT32U len);
/*
* Function Name :  Drv_USBD_AudioISOI_DMA_Finish
*
* Syntax : INT32S Drv_USBD_AudioISOI_DMA_Finish(void);
*
* Purpose :   os pend finish USB AUDIO ISOIN data start DMA message
*
* Parameters : <IN>	*buf : buffer pointer of DMA start address
*                     		 len  :data length
*          	   <OUT> none  
* Return : 1 : finish DMA 
*              
*
* Note :
*
*/
extern INT32S Drv_USBD_AudioISOI_DMA_Finish(void);


/*
* Function Name :  usbd_phy_clock_on
*
* Syntax : void usbd_phy_clock_on(void);
*
* Purpose :    to open USB PHY clock
*
* Parameters : <IN>	none
*                     		
*          	   <OUT> none  
* Return :none
*              
*
* Note :
*
*/
extern void usbd_phy_clock_on(void);

/*
* Function Name :  usb_isr_cb_register
*
* Syntax : void usb_isr_cb_register(void);
*
* Purpose :    register call back function to usb_isr,user can use it to quickly check 
* device plug in ,if you do not need quickly detect then do not care it.
*
* Parameters : <IN>	none
*                     		
*          	   <OUT> none  
* Return :none
*              
*
* Note :
*
*/
extern void usb_isr_cb_register(INT32S (*pt_function)());
