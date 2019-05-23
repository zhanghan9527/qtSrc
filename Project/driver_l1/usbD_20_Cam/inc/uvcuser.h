/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    UVCUSER.H
 *      Purpose: USB Video Device Class User module Definitions
 *      Version: V1.00
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#ifndef __UVCUSER_H__
#define __UVCUSER_H__

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
* Function Name :  UVC_GetMin
*
* Syntax : BOOL UVC_GetMin(void);
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


#endif  /* __UVCUSER_H__ */

