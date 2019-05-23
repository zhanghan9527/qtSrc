/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    UVCUSER.c
 *      Purpose: USB Video Device Class User module 
 *      Version: V1.00
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#include <string.h>
//#include "LPC23xx.H"                                  /* LPC23xx definitions */

#include "type.h"
#include "usb.h"
//#include "usbhw.h"
//#include "usbcfg.h"
#include "usbcore.h"
#include "uvc.h"
#include "uvcuser.h"
#define SINT16	INT16S
typedef short	INT16;

extern void print_string(CHAR *fmt, ...);

static BYTE UVC_PowerMode          = 0xA0;           /* Power Mode */
static BYTE UVC_ErrorCode          = NO_ERROR_ERR;   /* Error Code */

static BYTE UVC_suSelectedInputPin = 0x01;   /* SelectorUnit   selcted input pin */

static SINT16 UVC_puBrightnessCur    = 0x0010; /* ProcessingUnit Brightness Current */
static SINT16 UVC_puBrightnessDef    = 0x0010; /* ProcessingUnit Brightness Default */
static SINT16 UVC_puBrightnessMin    = 0x0001; /* ProcessingUnit Brightness Minimum */
static SINT16 UVC_puBrightnessMax    = 0x00FF; /* ProcessingUnit Brightness Maximum */
static SINT16 UVC_puBrightnessRes    = 0x0001; /* ProcessingUnit Brightness Resolution */

static SINT16 UVC_puHueCur    = 16; 		/* ProcessingUnit Hue Current */
static SINT16 UVC_puHueDef    = 16; 		/* ProcessingUnit Hue Default */
static SINT16 UVC_puHueMin    = -128;		/* ProcessingUnit Hue Minimum */
static SINT16 UVC_puHueMax    = 127; 		/* ProcessingUnit Hue Maximum */
static SINT16 UVC_puHueRes    = 0x0001; 	/* ProcessingUnit Hue Resolution */

static INT16 UVC_puContrastCur    = 16; 	/* ProcessingUnit Contrast Current */
static INT16 UVC_puContrastDef    = 16; 	/* ProcessingUnit Contrast Default */
static INT16 UVC_puContrastMin    = 1; 		/* ProcessingUnit Contrast Minimum */
static INT16 UVC_puContrastMax    = 255; 	/* ProcessingUnit Contrast Maximum */
static INT16 UVC_puContrastRes    = 0x0001; /* ProcessingUnit Contrast Resolution */

static INT16 UVC_puSaturationCur    = 16; 		/* ProcessingUnit Saturation Current */
static INT16 UVC_puSaturationDef    = 16; 		/* ProcessingUnit Saturation Default */
static INT16 UVC_puSaturationMin    = 1; 		/* ProcessingUnit Saturation Minimum */
static INT16 UVC_puSaturationMax    = 255; 		/* ProcessingUnit Saturation Maximum */
static INT16 UVC_puSaturationRes    = 0x0001; 	/* ProcessingUnit Saturation Resolution */

static INT16 UVC_puSharpnessCur    = 3; 		/* ProcessingUnit Sharpness Current */
static INT16 UVC_puSharpnessDef    = 2; 		/* ProcessingUnit Sharpness Default */
static INT16 UVC_puSharpnessMin    = 0; 		/* ProcessingUnit Sharpness Minimum */
static INT16 UVC_puSharpnessMax    = 3; 		/* ProcessingUnit Sharpness Maximum */
static INT16 UVC_puSharpnessRes    = 0x0001; 	/* ProcessingUnit Sharpness Resolution */

static INT16 UVC_puBacklightCur    = 3; 		/* ProcessingUnit Backlight Current */
static INT16 UVC_puBacklightDef    = 3; 		/* ProcessingUnit Backlight Default */
static INT16 UVC_puBacklightMin    = 1; 		/* ProcessingUnit Backlight Minimum */
static INT16 UVC_puBacklightMax    = 6; 		/* ProcessingUnit Backlight Maximum */
static INT16 UVC_puBacklightRes    = 0x0001; 	/* ProcessingUnit Backlight Resolution */

static INT16 UVC_puGammaCur    = 10; 			/* ProcessingUnit Gamma Current */
static INT16 UVC_puGammaDef    = 0; 			/* ProcessingUnit Gamma Default */
static INT16 UVC_puGammaMin    = 0; 			/* ProcessingUnit Gamma Minimum */
static INT16 UVC_puGammaMax    = 20; 			/* ProcessingUnit Gamma Maximum */
static INT16 UVC_puGammaRes    = 0x0001; 		/* ProcessingUnit Gamma Resolution */

static INT16 UVC_puWBTempCur    = 10; 			/* ProcessingUnit WB Temperature Current */
static INT16 UVC_puWBTempDef    = 0; 		    /* ProcessingUnit WB Temperature Default */
static INT16 UVC_puWBTempMin    = 0; 			/* ProcessingUnit WB Temperature Minimum */
static INT16 UVC_puWBTempMax    = 20; 			/* ProcessingUnit WB Temperature Maximum */
static INT16 UVC_puWBTempRes    = 0x0001; 		/* ProcessingUnit WB Temperature Resolution */

static INT16 UVC_puWBComCur    = 10; 			/* ProcessingUnit WB Component Current */
static INT16 UVC_puWBComDef    = 0; 		    /* ProcessingUnit WB Component Default */
static INT16 UVC_puWBComMin    = 0; 			/* ProcessingUnit WB Component Minimum */
static INT16 UVC_puWBComMax    = 20; 			/* ProcessingUnit WB Component Maximum */
static INT16 UVC_puWBComRes    = 0x0001; 		/* ProcessingUnit WB Component Resolution */

static INT16 UVC_puGainCur    = 10; 			/* ProcessingUnit Gain Current */
static INT16 UVC_puGainDef    = 0; 				/* ProcessingUnit Gain Default */
static INT16 UVC_puGainMin    = 0; 				/* ProcessingUnit Gain Minimum */
static INT16 UVC_puGainMax    = 20; 			/* ProcessingUnit Gain Maximum */
static INT16 UVC_puGainRes    = 0x0001; 		/* ProcessingUnit Gain Resolution */

#define DEFAULT_FORMAT_INDEX	0x01
#define DEFAULT_FRAME_INDEX		0x01

static UVC_VIDEO_PROBE_AND_COMMIT_CONTROL UVC_vsVideoProbe = 
{
	0x001F,                 // bitfield indicating what fields shall be kept fixed
    DEFAULT_FORMAT_INDEX,   // Video format index from a format descriptor
    DEFAULT_FRAME_INDEX,    // Video frame index from a frame descriptor
    //0x000A2C2A,             // Frame intervall in 100 ns units
    0x51615,                 //Frame intervall in 333333 ns units
    0x0000,                 // Key frame rate in key-frame per video-frame units
    0x0000,                 // PFrame rate i PFrame/key frame units
    0x0000,                 // Compression quality control in abstarct units (0..10000)
    0x0000,                 // Window size for average bit rate control
    0x00FF,                 // int. VS interface latency in ms from capture to presentation
    0x00096000,             // maximum video frame or codec specific segemet size in bytes 640*480*2
    //0x00009480,             // maximum video frame or codec specific segemet size in bytes
//#if MCU_VERSION >= GPL327XX
#if (MCU_VERSION >= GPL327XX) || (MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)  
    0x0000400,             // max. bytes the device can transmit/receive in single payload transfer
#else
    0x0000200,             // max. bytes the device can transmit/receive in single payload transfer  WBVAL(0x0200),                            // wMaxPacketSize      0x01FE max packet 512 bytes usb2.0
#endif
#if (UVC_VERSION ==  0x0110)
    0x00000000,             // device clock frequency in Hz for sepcified format
    0x00,                   // bitfield control
    0x00,                   // preferred payload format version for specifide bFormatIndex
    0x00,                   // minimum payload format version for specifide bFormatIndex    
    0x00                    // maximum payload format version for specifide bFormatIndex
#endif   
  };


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
BOOL UVC_SetCur (void) {
  WORD wTemp = 0;

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 0:                                            // interface control requests
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case VC_VIDEO_POWER_MODE_CONTROL:
              UVC_PowerMode = EP0Buf[0];
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 1:                                            // InputTerminal (camera)         see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite)      see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal                 see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit                   see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              UVC_suSelectedInputPin = EP0Buf[0];
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit                 see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:                    // only Brightness is supported   see usbdesc.c
              wTemp = *((PACKED WORD *)EP0Buf);
              if ((wTemp >= UVC_puBrightnessMin) &&        // check the value to set
                  (wTemp <= UVC_puBrightnessMax)) {
                   
                UVC_puBrightnessCur = wTemp; 
                UVC_ErrorCode = NO_ERROR_ERR;
              }
              else {
                UVC_ErrorCode = OUT_OF_RANGE_ERR;
              }
              break;
             case PU_HUE_CONTROL:                    // Hue is supported   see usbdesc.c
              wTemp = *((PACKED WORD *)EP0Buf);
              if ((wTemp >= UVC_puHueMin) &&        // check the value to set
                  (wTemp <= UVC_puHueMax)) {
                   
                UVC_puHueCur = wTemp; 
                UVC_ErrorCode = NO_ERROR_ERR;
              }
              else {
                UVC_ErrorCode = OUT_OF_RANGE_ERR;
              }
              break;
             case PU_CONTRAST_CONTROL:              // Contrast is supported   see usbdesc.c
              wTemp = *((PACKED WORD *)EP0Buf);
              if ((wTemp >= UVC_puContrastMin) &&        // check the value to set
                  (wTemp <= UVC_puContrastMax)) {
                   
                UVC_puContrastCur = wTemp; 
                UVC_ErrorCode = NO_ERROR_ERR;
              }
              else {
                UVC_ErrorCode = OUT_OF_RANGE_ERR;
              }
              break;
            case PU_SATURATION_CONTROL:             // Saturation is supported   see usbdesc.c
              wTemp = *((PACKED WORD *)EP0Buf);
              if ((wTemp >= UVC_puSaturationMin) &&        // check the value to set
                  (wTemp <= UVC_puSaturationMax)) {
                   
                UVC_puSaturationCur = wTemp; 
                UVC_ErrorCode = NO_ERROR_ERR;
              }
              else {
                UVC_ErrorCode = OUT_OF_RANGE_ERR;
              }
              break;
            case PU_SHARPNESS_CONTROL:             // Sharpness is supported   see usbdesc.c
              wTemp = *((PACKED WORD *)EP0Buf);
              if ((wTemp >= UVC_puSharpnessMin) &&        // check the value to set
                  (wTemp <= UVC_puSharpnessMax)) {
                   
                UVC_puSharpnessCur = wTemp; 
                UVC_ErrorCode = NO_ERROR_ERR;
              }
              else {
                UVC_ErrorCode = OUT_OF_RANGE_ERR;
              }
              break;
            case PU_BACKLIGHT_COMPENSATION_CONTROL:  // Backlight is supported   see usbdesc.c
              wTemp = *((PACKED WORD *)EP0Buf);
              if ((wTemp >= UVC_puBacklightMin) &&        // check the value to set
                  (wTemp <= UVC_puBacklightMax)) {
                   
                UVC_puBacklightCur = wTemp; 
                UVC_ErrorCode = NO_ERROR_ERR;
              }
              else {
                UVC_ErrorCode = OUT_OF_RANGE_ERR;
              }
              break;  
            case PU_GAMMA_CONTROL:                 // Gamma is supported   see usbdesc.c
              wTemp = *((PACKED WORD *)EP0Buf);
              if ((wTemp >= UVC_puGammaMin) &&        // check the value to set
                  (wTemp <= UVC_puGammaMax)) {
                   
                UVC_puGammaCur = wTemp; 
                UVC_ErrorCode = NO_ERROR_ERR;
              }
              else {
                UVC_ErrorCode = OUT_OF_RANGE_ERR;
              }
              break;
            case PU_WHITE_BALANCE_TEMPERATURE_CONTROL: // WB temperature is supported   see usbdesc.c
              wTemp = *((PACKED WORD *)EP0Buf);
              if ((wTemp >= UVC_puWBTempMin) &&        // check the value to set
                  (wTemp <= UVC_puWBTempMax)) {
                   
                UVC_puWBTempCur = wTemp; 
                UVC_ErrorCode = NO_ERROR_ERR;
              }
              else {
                UVC_ErrorCode = OUT_OF_RANGE_ERR;
              }
              break;            
            case PU_WHITE_BALANCE_COMPONENT_CONTROL:   // WB Component is supported   see usbdesc.c
              wTemp = *((PACKED WORD *)EP0Buf);
              if ((wTemp >= UVC_puWBComMin) &&        // check the value to set
                  (wTemp <= UVC_puWBComMax)) {
                   
                UVC_puWBComCur = wTemp; 
                UVC_ErrorCode = NO_ERROR_ERR;
              }
              else {
                UVC_ErrorCode = OUT_OF_RANGE_ERR;
              }
              break;
            case PU_GAIN_CONTROL:                 // Gain is supported   see usbdesc.c
              wTemp = *((PACKED WORD *)EP0Buf);
              if ((wTemp >= UVC_puGainMin) &&        // check the value to set
                  (wTemp <= UVC_puGainMax)) {
                   
                UVC_puGainCur = wTemp; 
                UVC_ErrorCode = NO_ERROR_ERR;
              }
              else {
                UVC_ErrorCode = OUT_OF_RANGE_ERR;
              }
              break;    
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case VS_PROBE_CONTROL :
            case VS_COMMIT_CONTROL:
              memcpy((void *)&UVC_vsVideoProbe, (const void *)EP0Buf, sizeof(UVC_VIDEO_PROBE_AND_COMMIT_CONTROL));
              UVC_ErrorCode = NO_ERROR_ERR;
#if 0              
              if( UVC_vsVideoProbe.bFormatIndex != 1 || UVC_vsVideoProbe.bFrameIndex != 1 || UVC_vsVideoProbe.dwFrameInterval != 0x51615 )
                	UVC_ErrorCode = INVALID_CONTROL_ERR;
#endif
				DBG_PRINT("Frame index = %d\r\n", UVC_vsVideoProbe.bFrameIndex);                
                UVC_vsVideoProbe.bmHint = 0x1F;
                UVC_vsVideoProbe.wDelay = 0xFF;
            //#if MCU_VERSION >= GPL327XX
            #if (MCU_VERSION >= GPL327XX) || (MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)
                UVC_vsVideoProbe.dwMaxPayloadTransferSize = 0x400;
            #else
                UVC_vsVideoProbe.dwMaxPayloadTransferSize = 0x200;
            #endif
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_SetCur


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
BOOL UVC_GetCur (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 0:                                            // interface control requests
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case VC_VIDEO_POWER_MODE_CONTROL:
              EP0Buf[0] = UVC_PowerMode;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case VC_REQUEST_ERROR_CODE_CONTROL:
              EP0Buf[0] = UVC_ErrorCode;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 1:                                            // InputTerminal (camera)    see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite) see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
            break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit              see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              EP0Buf[0] = UVC_suSelectedInputPin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puBrightnessCur;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_HUE_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puHueCur;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_CONTRAST_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puContrastCur;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_SATURATION_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puSaturationCur;
              UVC_ErrorCode = NO_ERROR_ERR;
              break; 
            case PU_SHARPNESS_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puSharpnessCur;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_BACKLIGHT_COMPENSATION_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puBacklightCur;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_GAMMA_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puGammaCur;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puWBTempCur;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_WHITE_BALANCE_COMPONENT_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puWBComCur;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_GAIN_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puGainCur;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;                    
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case VS_PROBE_CONTROL :
            case VS_COMMIT_CONTROL:
              memcpy((void *)EP0Buf, (const void *)&UVC_vsVideoProbe, sizeof(UVC_VIDEO_PROBE_AND_COMMIT_CONTROL));
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetCur


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
BOOL UVC_GetMin (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 1:                                            // InputTerminal (camera)    see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite) see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit              see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              EP0Buf[0] = UVC_suSelectedInputPin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puBrightnessMin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_HUE_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puHueMin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_CONTRAST_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puContrastMin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_SATURATION_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puSaturationMin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_SHARPNESS_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puSharpnessMin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_BACKLIGHT_COMPENSATION_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puBacklightMin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_GAMMA_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puGammaMin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puWBTempMin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_WHITE_BALANCE_COMPONENT_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puWBComMin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_GAIN_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puGainMin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;                            
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetMin

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
BOOL UVC_GetMax (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 1:                                            // InputTerminal (camera)    see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:                            
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite) see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit              see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              EP0Buf[0] = UVC_suSelectedInputPin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puBrightnessMax;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_HUE_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puHueMax;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_CONTRAST_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puContrastMax;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_SATURATION_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puSaturationMax;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_SHARPNESS_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puSharpnessMax;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_BACKLIGHT_COMPENSATION_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puBacklightMax;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_GAMMA_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puGammaMax;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puWBTempMax;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_WHITE_BALANCE_COMPONENT_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puWBComMax;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_GAIN_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puGainMax;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;                       
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetMax

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
BOOL UVC_GetRes (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 1:                                            // InputTerminal (camera)    see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite) see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit              see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              EP0Buf[0] = UVC_suSelectedInputPin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puBrightnessRes;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_HUE_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puHueRes;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_CONTRAST_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puContrastRes;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_SATURATION_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puSaturationRes;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_SHARPNESS_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puSharpnessRes;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_BACKLIGHT_COMPENSATION_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puBacklightRes;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_GAMMA_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puGammaRes;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puWBTempRes;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_WHITE_BALANCE_COMPONENT_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puWBComRes;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_GAIN_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puGainRes;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;                         
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetRes

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
BOOL UVC_GetLen (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
              UVC_ErrorCode = INVALID_REQUEST_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetLen

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
BOOL UVC_GetInfo (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 0:                                            // interface control requests
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case VC_VIDEO_POWER_MODE_CONTROL:
              EP0Buf[0] = (SUPPORTS_GET | SUPPORTS_SET);
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case VC_REQUEST_ERROR_CODE_CONTROL:
              EP0Buf[0] = (SUPPORTS_GET);
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 1:                                            // InputTerminal (camera)    see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite) see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit              see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              EP0Buf[0] = (SUPPORTS_GET | SUPPORTS_SET);
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
            case PU_HUE_CONTROL:
            case PU_CONTRAST_CONTROL:
            case PU_SATURATION_CONTROL:
            case PU_SHARPNESS_CONTROL:
            case PU_BACKLIGHT_COMPENSATION_CONTROL:
            case PU_GAMMA_CONTROL:
            case PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
            case PU_GAIN_CONTROL:
              EP0Buf[0] = (SUPPORTS_GET | SUPPORTS_SET);
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetInfo

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
BOOL UVC_GetDef (void) {

  switch (SetupPacket.wIndex.WB.L) {                       // select the Interface
    case USB_UVC_VCIF_NUM:                                 // Video Control Interface
      switch (SetupPacket.wIndex.WB.H) {                   // select the Entity
        case 1:                                            // InputTerminal (camera)    see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 2:                                            // InputTerminal (composite) see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 3:                                            // OutputTerminal            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 4:                                            // SelectorUnit              see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case SU_INPUT_SELECT_CONTROL:
              EP0Buf[0] = UVC_suSelectedInputPin;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
          break;
        case 5:                                            // ProcessingUnit            see usbdesc.c
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            case PU_BRIGHTNESS_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puBrightnessDef;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_HUE_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puHueDef;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_CONTRAST_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puContrastDef;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_SATURATION_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puSaturationDef;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_SHARPNESS_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puSharpnessDef;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_BACKLIGHT_COMPENSATION_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puBacklightDef;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_GAMMA_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puGammaDef;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puWBTempDef;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_WHITE_BALANCE_COMPONENT_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puWBComDef;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            case PU_GAIN_CONTROL:
              *((PACKED WORD *)EP0Buf) = UVC_puGainDef;
              UVC_ErrorCode = NO_ERROR_ERR;
              break;
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;                                
          } // end SetupPacket.wValue.WB.H
          break;
        default:
          UVC_ErrorCode = INVALID_UNIT_ERR;
          break;
      } // end SetupPacket.wIndex.WB.H
      break;

    case USB_UVC_VSIF_NUM:                                 // Video Streaming Interface
          switch (SetupPacket.wValue.WB.H) {               // select the  selector control
            default:
              UVC_ErrorCode = INVALID_CONTROL_ERR;
              break;
          } // end SetupPacket.wValue.WB.H
      break;

    default:
      UVC_ErrorCode = UNKNOWN_ERR;
      break;
  }  // end SetupPacket.wIndex.WB.L

  if (UVC_ErrorCode == NO_ERROR_ERR)
    return (TRUE);                                         // we handled the request
  else
    return (FALSE);                                        // if not handled we stall it

} // end UVC_GetDef

