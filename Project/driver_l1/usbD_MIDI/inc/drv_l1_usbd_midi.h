/**********************************************************
* Purpose: USB layer 1 header file for MIDI class
*
* Author: Eugenehsu
*
* Date: 2012/07/16
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.01
* History :
              
**********************************************************/
#ifndef _DRV_L1_USBD_MIDI_H_
#define _DRV_L1_USBD_MIDI_H_

#define		BIT0		0x0001
#define		BIT1		0x0002
#define		BIT2		0x0004
#define		BIT3		0x0008
#define		BIT4		0x0010
#define		BIT5		0x0020
#define		BIT6		0x0040
#define		BIT7		0x0008
#define		BIT8		0x0100
#define		BIT9		0x0200
#define		BIT10		0x0400
#define		BIT11		0x0800
#define		BIT12		0x1000
#define		BIT13		0x2000
#define		BIT14		0x4000
#define		BIT15		0x8000

#define		EP0_Setup_PKT		BIT0
#define		EP0_OUT_PKT			BIT1
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


#define		EP0_SETUP_Stage		0x0001
#define		EP0_Data_Stage		0x0002
#define		EP0_Status_Stage	0x0003
#define		Bulk_OUT_Stage		0x0004
#define		Bulk_IN_Stage		0x0005
#define		SCSI_CSW_Stage		0x0006

#define		Device_Descriptor_Table		0x0001
#define		Config_Descriptor_Table		0x0002
#define		String0_Descriptor_Table	0x0003
#define		String1_Descriptor_Table	0x0004
#define		String2_Descriptor_Table	0x0005
#define		String3_Descriptor_Table	0x0006
#define		String4_Descriptor_Table	0x0007
#define		Interface_Descriptor_Table	0x0008
#define		Endpoint_Descriptor_Table	0x0009
#define 	Device_Feature1_Table		0x000B
#define 	Device_Feature2_Table		0x000C

#define		Bulk_IN_Stall		0x0001
#define		Bulk_OUT_Stall		0x0002

#define R_Timeout_count 0xf888

/*
* Function Name :  MIDI_USB_Initial
*
* Syntax : void MIDI_USB_Initial(void);
*
* Purpose :  USB MIDI device initialize.
*
* Parameters : <IN>   none            
*                            			
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_USB_Initial(void);

/*
* Function Name :  MIDI_USB_Uninitial
*
* Syntax : void MIDI_USB_Uninitial(void);
*
* Purpose :  USB MIDI device un-initialize.
*
* Parameters : <IN>  none                 
*                            			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_USB_Uninitial(void);

/*
* Function Name :  MIDI_USB_Reset
*
* Syntax : void MIDI_USB_Reset(void);
*
* Purpose :  USB MIDI device Reset.
*
* Parameters : <IN>  none                 
*                            			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_USB_Reset(void);

/*
* Function Name :  MIDI_NotSupport
*
* Syntax : void MIDI_NotSupport(void);
*
* Purpose :  USB MIDI device Not Support.
*
* Parameters : <IN>  none                 
*                            			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_NotSupport(void);

/*
* Function Name :  MIDI_Send_8_Bytes
*
* Syntax : void MIDI_Send_8_Bytes(void);
*
* Purpose :  User can call this function to send 8 bytes data thru USB. Prepare all the 8 byte data
*
* Parameters : <IN>  none                 
*                            			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_Send_8_Bytes(void);

/*
* Function Name :  MIDI_Send_Remain_Bytes
*
* Syntax : void MIDI_Send_Remain_Bytes(void);
*
* Purpose :  Send remain number of bytes when received an IN token of setup command. 
*		    The total number of bytes which described in setup command will be transferredpackage(8 bytes)
*		    by package thru Send_8_bytes(), and send the remainder by Send_Remain_Bytes()
*
* Parameters : <IN>  none                 
*                            			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_Send_Remain_Bytes(void);

/*
* Function Name :  MIDI_Send_Null
*
* Syntax : void MIDI_Send_Null(void);
*
* Purpose :  User can call this function to send NULL byte.
*
* Parameters : <IN>  none                 
*                            			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_Send_Null(void);

/*
* Function Name :  MIDI_Read_EP0_FIFO
*
* Syntax : void MIDI_Read_EP0_FIFO(void);
*
* Purpose :  User can call this function to read data from EP0
*
* Parameters : <IN>  none                 
*                            			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_Read_EP0_FIFO(void);

/*
* Function Name :  MIDI_Write_EP0_FIFO
*
* Syntax : void MIDI_Write_EP0_FIFO(void);
*
* Purpose :  User can call this function to write data to EP0, this function will call Send_8_Bytes() and Send_Remain_Bytes()
*
* Parameters : <IN>  none                 
*                            			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_Write_EP0_FIFO(void);

/*
* Function Name :  MIDI_USB_ISR
*
* Syntax : void MIDI_USB_ISR(void);
*
* Purpose :  User can call this function to handle to ISR of USB device
*
* Parameters : <IN>  none                 
*                            			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_USB_ISR(void);

/*
* Function Name :  MIDI_Setup_Token
*
* Syntax : void MIDI_Setup_Token(void);
*
* Purpose :  User can call this function to parse the token of SETUP command
*
* Parameters : <IN>  none                 
*                            			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_Setup_Token(void);

/*
* Function Name :  MIDI_USB_ServiceLoop
*
* Syntax : void MIDI_USB_ServiceLoop(INT32U unUseLoop);
*
* Purpose :  User can call this function to handle the EP0 SETUP command,and to handle SCSI Command, including parsing CBW
*		     BULK IN/OUT data, and send CSW to host.
*
* Parameters : <IN>  unUseLoop: 0: call once then return;
*          						 1: call will not return until receiving SUSPEND                                			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_USB_ServiceLoop(INT32U unUseLoop);

/*
* Function Name :  MIDI_USB_Register_in_out_fn
*
* Syntax : void MIDI_USB_Register_in_out_fn(INT32S (*in)(void), INT32S (*out)(void));
*
* Purpose : pointer function to process usb midi out/in handler  
*
* Parameters : <IN>  (*in)(void)  function pointer of process usb midi in handler 
*          			   (*out)(void) function pointer of process usb midi out handler               			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_USB_Register_in_out_fn(INT32S (*in)(void), INT32S (*out)(void));

/*
* Function Name :  MIDI_USB_Send_In
*
* Syntax : void MIDI_USB_Send_In(void* ptr, INT32U len);
*
* Purpose :  User can call this function to send data to host
*
* Parameters : <IN>  * ptr: pointer of send data buffer	             
*                               len  : length of send data			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_USB_Send_In(void* ptr, INT32U len);

/*
* Function Name :  MIDI_USB_GET_Out
*
* Syntax : void MIDI_USB_GET_Out(void* ptr, INT32U *len);
*
* Purpose :  User can call this function to Receive data from host
*
* Parameters : <IN>  * ptr: pointer of Receive data buffer	             
*                               len  : length of Receive data			  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void MIDI_USB_GET_Out(void* ptr, INT32U *len);

/*
* Function Name :  MIDI_Get_Out_Pkt
*
* Syntax : INT8U MIDI_Get_Out_Pkt(INT32U index);
*
* Purpose :  User can call this function to get the value from MIDI Data package
*
* Parameters : <IN>   index:get which one the  value from  MIDI Data package            
*                           		  
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern INT8U MIDI_Get_Out_Pkt(INT32U index);
#endif	//_DRV_L1_USBD_MIDI_H_


