/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    ADCUSER.H
 *      Purpose: Audio Device Class Custom User Definitions
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2006 Keil Software.
 *---------------------------------------------------------------------------*/

#ifndef __ADCUSER_H__
#define __ADCUSER_H__


/* Audio Device Class Requests Callback Functions */

/*
* Function Name :  ADC_IF_GetRequest
*
* Syntax : BOOL ADC_IF_GetRequest (void);
*
* Purpose :  Called automatically on ADC Interface Get Request
*
* Parameters : <IN>   none
*                              
*                     <OUT> none
* Return : TRUE  - Success
*              FALSE - Error
*
* Note :
*
*/
extern BOOL ADC_IF_GetRequest (void);

/*
* Function Name :  ADC_IF_SetRequest
*
* Syntax : BOOL ADC_IF_SetRequest (void);
*
* Purpose :  Called automatically on ADC Interface Set Request
*
* Parameters : <IN>   none
*                              
*                     <OUT> none
* Return : TRUE  - Success
*              FALSE - Error
*
* Note :
*
*/
extern BOOL ADC_IF_SetRequest (void);

/*
* Function Name :  ADC_EP_GetRequest
*
* Syntax :  BOOL ADC_EP_GetRequest (void);
*
* Purpose :  Called automatically on ADC EndPoint Get Request
*
* Parameters : <IN>   none
*                              
*                     <OUT> none
* Return : TRUE  - Success
*              FALSE - Error
*
* Note :
*
*/
extern BOOL ADC_EP_GetRequest (void);

/*
* Function Name :  ADC_EP_SetRequest
*
* Syntax : BOOL ADC_EP_SetRequest (void);
*
* Purpose :  Called automatically on ADC EndPoint Set Request
*
* Parameters : <IN>   none
*                              
*                     <OUT> none
* Return : TRUE  - Success
*              FALSE - Error
*
* Note :
*
*/
extern BOOL ADC_EP_SetRequest (void); /* Not Supported */


#endif  /* __ADCUSER_H__ */
