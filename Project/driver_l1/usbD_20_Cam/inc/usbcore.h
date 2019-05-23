/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    USBCORE.H
 *      Purpose: USB Core Definitions
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#ifndef __USBCORE_H__
#define __USBCORE_H__

#include "usb.h"
#include "driver_l2.h"

/* USB Endpoint Data Structure */
typedef struct _USB_EP_DATA {
  BYTE  *pData;
  WORD   Count;
} USB_EP_DATA;

/* USB Core Global Variables */
extern WORD  USB_DeviceStatus;
extern BYTE  USB_DeviceAddress;
extern BYTE  USB_Configuration;
extern DWORD USB_EndPointMask;
extern DWORD USB_EndPointHalt;
extern BYTE  USB_AltSetting[USB_IF_NUM];

/* USB Endpoint 0 Buffer */
extern BYTE  EP0Buf[64/*USB_MAX_PACKET0*/];

/* USB Endpoint 0 Data Info */
extern USB_EP_DATA EP0Data;

/* USB Setup Packet */
extern USB_SETUP_PACKET SetupPacket;
extern BYTE  USB_NumInterfaces;

/* USB Core Functions */
/*
* Function Name :  USB_ResetCore
*
* Syntax : void  USB_ResetCore (void);
*
* Purpose :  to  Reset USB Core
*
* Parameters : <IN> none
*                             
*                     <OUT> none
* Return : none
*              
*
* Note :
*
*/
extern void  USB_ResetCore (void);

extern DWORD USB_ReadEP(DWORD EPNum, BYTE *pData,DWORD len);
/*
* Function Name :  USB_GetStatus
*
* Syntax : BOOL USB_GetStatus (void);
*
* Purpose :  Get USB Status
*
* Parameters : <IN> none
*                             
*                     <OUT> none
* Return : TRUE - Success
*              FALSE - Error
*
* Note :
*
*/
extern BOOL USB_GetStatus (void);

/*
* Function Name :  USB_SetClrFeature
*
* Syntax : BOOL USB_SetClrFeature (DWORD sc);
*
* Purpose : Set/Clear Feature USB Request
*
* Parameters : <IN>  sc:    0 - Clear
*                             	      1 - Set
*                     <OUT> none
* Return : TRUE - Success
*              FALSE - Error
*
* Note :
*
*/
extern BOOL USB_SetClrFeature (DWORD sc);
/*
* Function Name :  USB_GetDescriptor
*
* Syntax :  BOOL USB_GetDescriptor (void);
*
* Purpose :  Get Descriptor USB Request
*
* Parameters : <IN>  none
*                             	  
*                     <OUT> none
* Return : TRUE - Success
*              FALSE - Error
*
* Note :
*
*/
extern BOOL USB_GetDescriptor (void);

/*
* Function Name :  USB_SetConfiguration
*
* Syntax : BOOL USB_SetConfiguration (void);
*
* Purpose :  Get Descriptor USB Request
*
* Parameters : <IN>  none
*                             	  
*                     <OUT> none
* Return : TRUE - Success
*              FALSE - Error
*
* Note :
*
*/
extern BOOL USB_SetConfiguration (void);

/*
* Function Name :  USB_SetInterface
*
* Syntax : BOOL USB_SetInterface (void);
*
* Purpose :   Set Interface USB Request
*
* Parameters : <IN>  none
*                             	  
*                     <OUT> none
* Return : TRUE - Success
*              FALSE - Error
*
* Note :
*
*/
extern BOOL USB_SetInterface (void);

/*
* Function Name :  USB_SetupStage
*
* Syntax : void USB_SetupStage (void);
*
* Purpose :    USB Request - Setup Stage
*
* Parameters : <IN>  none
*                             	  
*                     <OUT> none
* Return : none
*            
*
* Note :
*
*/
extern void USB_SetupStage (void);

/*
* Function Name :  USB_StatusInStage
*
* Syntax : void USB_StatusInStage (void);
*
* Purpose :    USB Request - Status In Stage
*
* Parameters : <IN>  none
*                             	  
*                     <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_StatusInStage (void);

/*
* Function Name :  USB_DataOutStage
*
* Syntax : void USB_DataOutStage (void);
*
* Purpose :   USB Request - Data Out Stage
*
* Parameters : <IN>  none
*                             	  
*                     <OUT> none
* Return : none
*              
*
* Note :
*
*/
extern void USB_DataOutStage (void);

/*
* Function Name :  USB_StatusOutStage
*
* Syntax : void USB_StatusOutStage (void);
*
* Purpose :   USB Request - Status Out Stage
*
* Parameters : <IN>  none
*                             	  
*                     <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_StatusOutStage (void);

#endif  /* __USBCORE_H__ */
