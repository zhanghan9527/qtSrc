/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    USBUSER.H
 *      Purpose: USB Custom User Definitions
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#ifndef __USBUSER_H__
#define __USBUSER_H__

/*
* Function Name :  USB_Power_Event
*
* Syntax : void USB_Power_Event     (BOOL  power);
*
* Purpose :  Called automatically on USB Power Event
*
* Parameters : <IN>  power: On(TRUE)/Off(FALSE)                     
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_Power_Event     (BOOL  power);
/*
* Function Name :  USB_Reset_Event
*
* Syntax : USB_Reset_Event     (void);
*
* Purpose :  Called automatically on USB Reset Event
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
extern void USB_Reset_Event     (void);

/*
* Function Name :  USB_Suspend_Event
*
* Syntax : void USB_Suspend_Event   (void);
*
* Purpose :  Called automatically on USB Suspend Event
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
extern void USB_Suspend_Event   (void);

/*
* Function Name :  USB_Resume_Event
*
* Syntax : void USB_Resume_Event    (void);
*
* Purpose :  Called automatically on USB Resume Event
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
extern void USB_Resume_Event    (void);

/*
* Function Name :  USB_WakeUp_Event
*
* Syntax : void USB_WakeUp_Event    (void);
*
* Purpose :  Called automatically on USB Remote Wakeup Event
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
extern void USB_WakeUp_Event    (void);

/*
* Function Name :  USB_SOF_Event
*
* Syntax : BOOL USB_SOF_Event       (INT32U framesize,INT32U startaddr );
*
* Purpose :  Called automatically on USB Start of Frame Event
*
* Parameters : <IN>  framesize :       frame size         
*                              startaddr  :       frame start addr
*                    <OUT> none
* Return : TRUE - Success 
*             FALSE - Error              
*
* Note :
*
*/
extern BOOL USB_SOF_Event       (INT32U framesize,INT32U startaddr );

/*
* Function Name :  USB_Error_Event
*
* Syntax : void USB_Error_Event     (DWORD error);
*
* Purpose :  Called automatically on USB Error Event
*
* Parameters : <IN>  error: Error Code     
*                            
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_Error_Event     (DWORD error);

/* USB Endpoint Callback Events */
#define USB_EVT_SETUP       1   /* Setup Packet */
#define USB_EVT_OUT         2   /* OUT Packet */
#define USB_EVT_IN          3   /*  IN Packet */
#define USB_EVT_OUT_NAK     4   /* OUT Packet - Not Acknowledged */
#define USB_EVT_IN_NAK      5   /*  IN Packet - Not Acknowledged */
#define USB_EVT_OUT_STALL   6   /* OUT Packet - Stalled */
#define USB_EVT_IN_STALL    7   /*  IN Packet - Stalled */
#define USB_EVT_OUT_DMA_EOT 8   /* DMA OUT EP - End of Transfer */
#define USB_EVT_IN_DMA_EOT  9   /* DMA  IN EP - End of Transfer */
#define USB_EVT_OUT_DMA_NDR 10  /* DMA OUT EP - New Descriptor Request */
#define USB_EVT_IN_DMA_NDR  11  /* DMA  IN EP - New Descriptor Request */
#define USB_EVT_OUT_DMA_ERR 12  /* DMA OUT EP - Error */
#define USB_EVT_IN_DMA_ERR  13  /* DMA  IN EP - Error */

//#if MCU_VERSION >= GPL327XX
#if (MCU_VERSION >= GPL327XX) || (MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)
#define EP7_MAX_PACKET      1024//96//192//96//0x1FE
#else
#define EP7_MAX_PACKET      512//96//192//96//0x1FE
#endif
#define PAYLOAD_HEADER_SIZE 0x0C
#define EP6_MAX_PACKET      192

/* USB Endpoint Events Callback Pointers */
extern void (* const USB_P_EP[16])(DWORD event);

/* USB Endpoint Events Callback Functions */

/*
* Function Name :  USB_EndPoint0
*
* Syntax : void USB_EndPoint0  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 1 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
void USB_EndPoint0(DWORD event);
/*
* Function Name :  USB_EndPoint1
*
* Syntax : void USB_EndPoint1  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 1 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint1  (DWORD event);

/*
* Function Name :  USB_EndPoint2
*
* Syntax : void USB_EndPoint2  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 2 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint2  (DWORD event);

/*
* Function Name :  USB_EndPoint3
*
* Syntax : void USB_EndPoint3  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 3 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint3  (DWORD event);

/*
* Function Name :  USB_EndPoint4
*
* Syntax : void USB_EndPoint4  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 4 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint4  (DWORD event);

/*
* Function Name :  USB_EndPoint5
*
* Syntax : void USB_EndPoint5  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 5 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint5  (DWORD event);

/*
* Function Name :  USB_EndPoint6
*
* Syntax : void USB_EndPoint6  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 6 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint6  (DWORD event);

/*
* Function Name :  USB_EndPoint7
*
* Syntax : void USB_EndPoint7  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 7 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint7  (DWORD event);

/*
* Function Name :  USB_EndPoint8
*
* Syntax : void USB_EndPoint8  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 8 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint8  (DWORD event);

/*
* Function Name :  USB_EndPoint9
*
* Syntax : void USB_EndPoint9  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 9 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint9  (DWORD event);

/*
* Function Name :  USB_EndPoint10
*
* Syntax : void USB_EndPoint10  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 10 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint10 (DWORD event);

/*
* Function Name :  USB_EndPoint12
*
* Syntax : void USB_EndPoint12  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 12 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint11 (DWORD event);
/*
* Function Name :  USB_EndPoint12
*
* Syntax : void USB_EndPoint12  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 12 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint12 (DWORD event);

/*
* Function Name :  USB_EndPoint13
*
* Syntax : void USB_EndPoint13  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 13 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint13 (DWORD event);

/*
* Function Name :  USB_EndPoint14
*
* Syntax : void USB_EndPoint14 (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 14 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint14 (DWORD event);

/*
* Function Name :  USB_EndPoint15
*
* Syntax : void USB_EndPoint15  (DWORD event);
*
* Purpose :  Called automatically on USB Endpoint 15 Event
*
* Parameters : <IN>  event                        
*                             
*                    <OUT> none
* Return : none
*             
*
* Note :
*
*/
extern void USB_EndPoint15 (DWORD event);

/*
* Function Name :  USB_Configure_Event
*
* Syntax : void USB_Configure_Event (void);
*
* Purpose :  Called automatically on USB Set Configuration Request
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
extern void USB_Configure_Event (void);

/*
* Function Name :  USB_Interface_Event
*
* Syntax : void USB_Interface_Event (void);
*
* Purpose :  Called automatically on USB Set Interface Request
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
extern void USB_Interface_Event (void);

/*
* Function Name :  USB_Feature_Event
*
* Syntax : void USB_Feature_Event   (void);
*
* Purpose :  Called automatically on USB Set/Clear Feature Request
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
extern void USB_Feature_Event   (void);

/*
* Function Name :  Write_To_Buf
*
* Syntax : BOOL Write_To_Buf(void);
*
* Purpose :  Write data to buffer
*
* Parameters : <IN>  none                        
*                             
*                    <OUT> none
* Return : TRUE - Success
*             FALSE - Error           
*
* Note :
*
*/
extern BOOL Write_To_Buf(void);

extern volatile DWORD UVC_Delay;
extern volatile DWORD TestCnt;
extern volatile DWORD JPG_Cnt;
extern volatile DWORD Buf_Size;
extern volatile DWORD PTS_Value;
extern volatile DWORD SCR_Value;
extern volatile DWORD JPG_size;
//extern const unsigned char *JPG_data;
extern unsigned char  *JPG_data;
extern volatile BYTE  PicsToggle;
//extern          BYTE SOF_Event_Buf[EP3_MAX_PACKET];
extern          BYTE *SOF_Event_Buf;

/*
* Function Name :  UVC_SetCur
*
* Syntax : BOOL UVC_SetCur(void);
*
* Purpose :  Called automatically on UVC SET_CUR Request
*
* Parameters : <IN>  none                        
*                             
*                     <OUT> none
* Return : TRUE - Success
*             FALSE - Error
*
* Note :
*
*/
extern BOOL UVC_SetCur  (void);


/*
* Function Name :  UVC_GetCur
*
* Syntax : BOOL UVC_GetCur(void);
*
* Purpose :  Called automatically on UVC GET_CUR Request
*
* Parameters : <IN>  none                        
*                             
*                    <OUT> none
* Return : TRUE - Success
*             FALSE - Error
*
* Note :
*
*/
extern BOOL UVC_GetCur  (void);


/*
* Function Name :  UVC_GetMin
*
* Syntax : BOOL UVC_GetMin(void);
*
* Purpose :  Called automatically on UVC GET_MIN Request
*
* Parameters : <IN>  none                        
*                             
*                    <OUT> none
* Return : TRUE - Success
*             FALSE - Error
*
* Note :
*
*/
extern BOOL UVC_GetMin  (void);

/*
* Function Name :  UVC_GetMax
*
* Syntax : BOOL UVC_GetMax(void);
*
* Purpose :  Called automatically on UVC GET_MAX Request
*
* Parameters : <IN>  none                        
*                             
*                    <OUT> none
* Return : TRUE - Success
*             FALSE - Error
*
* Note :
*
*/
extern BOOL UVC_GetMax  (void);

/*
* Function Name :  UVC_GetRes
*
* Syntax : BOOL UVC_GetRes(void);
*
* Purpose :  Called automatically on UVC GET_RES Request
*
* Parameters : <IN>  none                        
*                             
*                    <OUT> none
* Return : TRUE - Success
*             FALSE - Error
*
* Note :
*
*/
extern BOOL UVC_GetRes  (void);

/*
* Function Name :  UVC_GetLen
*
* Syntax : BOOL UVC_GetLen(void);
*
* Purpose :  Called automatically on UVC GET_LEN Request
*
* Parameters : <IN>  none                        
*                             
*                    <OUT> none
* Return : TRUE - Success
*             FALSE - Error
*
* Note :
*
*/
extern BOOL UVC_GetLen  (void);

/*
* Function Name :  UVC_GetInfo
*
* Syntax : BOOL UVC_GetInfo(void);
*
* Purpose :  Called automatically on UVC GET_INFO Request
*
* Parameters : <IN>  none                        
*                             
*                    <OUT> none
* Return : TRUE - Success
*             FALSE - Error
*
* Note :
*
*/
extern BOOL UVC_GetInfo (void);

/*
* Function Name :  UVC_GetDef
*
* Syntax : BOOL UVC_GetDef(void);
*
* Purpose :  Called automatically on UVC GET_DEF Request
*
* Parameters : <IN>  none                        
*                             
*                    <OUT> none
* Return : TRUE - Success
*             FALSE - Error
*
* Note :
*
*/
extern BOOL UVC_GetDef  (void);



#endif  /* __USBUSER_H__ */
