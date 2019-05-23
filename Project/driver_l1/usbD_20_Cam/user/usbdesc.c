/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    USBDESC.C
 *      Purpose: USB Descriptors
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#include "type.h"
#include "usb.h"
//#include "usbcfg.h"
#include "usbdesc.h"
#include "uvc.h"
#include "audio.h"
#include "project.h"
#include "drv_l1_usbd.h"
//#include "drv_l1_usbd_tbl.h"

/*----------------------------------------------------------------------------
 * based on
 * USB Device Class Definition for Video Devices: Video Device Examples
 *---------------------------------------------------------------------------*/
 
extern char     *EP0dataPt;
extern char Max_LUN[];

#define STILL_IMAGE_SUPPORT 0
#define FRAME_RATE_SETTING	15

#define NUMBER_OF_FRAME_DES		0x01
#define FRAME_DES_TOTAL_LEN		(0x26*NUMBER_OF_FRAME_DES)

#define FRAME_INDEX2_WIDTH		800
#define FRAME_INDEX2_HEIGHT		600

#define FRAME_INDEX3_WIDTH		1280
#define FRAME_INDEX3_HEIGHT		720

#define FRAME_INDEX4_WIDTH		1920
#define FRAME_INDEX4_HEIGHT		1080

/* Setting for image processing control */
#define	UVC_PU_CONTROL_BM	(PU_BM_BRIGHTNESS)
#if 0
#define	UVC_PU_CONTROL_BM	(PU_BM_BRIGHTNESS | PU_BM_HUE | PU_BM_CONTRAST | PU_BM_SATURATION | PU_BM_SHARPNESS \
								| PU_BM_BLCOM | PU_BM_GAMMA | PU_BM_GAIN | PU_BM_WBTEMP)								
#endif

/* USB Standard Device Descriptor */
BYTE USB_DeviceDescriptor[] = {
  USB_DEVICE_DESC_SIZE,                      // bLength                 18
  USB_DEVICE_DESCRIPTOR_TYPE,                // bDescriptorType          1
 // WBVAL(0x0110),                             // bcdUSB                2.00
  WBVAL(0x0200),
  0xEF,                                      // bDeviceClass           239 Miscellaneous Device
  0x02,                                      // bDeviceSubClass          2 Common Class
  0x01,                                      // bDeviceProtocol          1 Interface Association
  0x40,                           			// bMaxPacketSize0
  //WBVAL(0xC251),                             // idVendor
  //WBVAL(0x1706),                             // idProduct                  Keil MCB2300, Video Device
  WBVAL(0x1b3f),                             // idVendor
  WBVAL(0x2002),                             // idProduct                  Keil MCB2300, Video Device
  WBVAL(0x0100),                             // bcdDevice             1.00
  0x01,                                      // iManufacturer            1 Keil Software
  0x02,                                      // iProduct                 2 Keil MCB2300 UVC
//#if MCU_VERSION >= GPL327XX
#if (MCU_VERSION >= GPL327XX) || (MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)
  0x00,                                      // iSerialNumber            3 Demo 1.00
#else
  0x03,                                      // iSerialNumber            3 Demo 1.00
#endif  
  0x01                                       // bNumConfigurations       1
};

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
BYTE USB_ConfigDescriptor[] = {
/* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,               // bLength                  9
  USB_CONFIGURATION_DESCRIPTOR_TYPE,         // bDescriptorType          2
#if C_USB_AUDIO == CUSTOM_ON
  WBVAL(                                     // wTotalLength 
    USB_CONFIGUARTION_DESC_SIZE + UVC_INTERFACE_ASSOCIATION_DESC_SIZE + USB_INTERFACE_DESC_SIZE +          //
    UVC_VC_INTERFACE_HEADER_DESC_SIZE(1) +   UVC_CAMERA_TERMINAL_DESC_SIZE(2) + UVC_OUTPUT_TERMINAL_DESC_SIZE(0) + //                          header + 1 interface
    UVC_SELECTOR_UNIT_DESC_SIZE(1) + UVC_PROCESSING_UNIT_DESC_SIZE(2) + USB_ENDPOINT_DESC_SIZE +
  	UVC_VC_ENDPOINT_DESC_SIZE + USB_INTERFACE_DESC_SIZE + UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1) +
    0x0B + FRAME_DES_TOTAL_LEN +        //                          VS Format Descriptor                                 //                          VS Frame Descriptor
    USB_INTERFACE_DESC_SIZE + USB_ENDPOINT_DESC_SIZE + 0x0000 +// der ganze Rest 
    /*audio */ 8 + /* IAD size */ +USB_INTERFACE_DESC_SIZE+AUDIO_CONTROL_INTERFACE_DESC_SZ(1)+AUDIO_INPUT_TERMINAL_DESC_SIZE+AUDIO_FEATURE_UNIT_DESC_SZ(0,1)+
    AUDIO_OUTPUT_TERMINAL_DESC_SIZE+USB_INTERFACE_DESC_SIZE+USB_INTERFACE_DESC_SIZE+AUDIO_STREAMING_INTERFACE_DESC_SIZE+AUDIO_FORMAT_TYPE_I_DESC_SZ(1)+AUDIO_STANDARD_ENDPOINT_DESC_SIZE+AUDIO_STREAMING_ENDPOINT_DESC_SIZE
    ),
    0x04,                                      // bNumInterfaces           4
#else
#if (MCU_VERSION >= GPL327XX) || (MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)
  WBVAL(                                     // wTotalLength 
    USB_CONFIGUARTION_DESC_SIZE + UVC_INTERFACE_ASSOCIATION_DESC_SIZE + USB_INTERFACE_DESC_SIZE +          //
    UVC_VC_INTERFACE_HEADER_DESC_SIZE(1) +   UVC_CAMERA_TERMINAL_DESC_SIZE(2) + UVC_OUTPUT_TERMINAL_DESC_SIZE(0) + //                          header + 1 interface
    UVC_SELECTOR_UNIT_DESC_SIZE(1) + UVC_PROCESSING_UNIT_DESC_SIZE(2) + USB_ENDPOINT_DESC_SIZE +
  	UVC_VC_ENDPOINT_DESC_SIZE + USB_INTERFACE_DESC_SIZE + UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1) +
    0x0B + FRAME_DES_TOTAL_LEN +        //                          VS Format Descriptor                                 //                          VS Frame Descriptor
    USB_INTERFACE_DESC_SIZE + USB_ENDPOINT_DESC_SIZE + 0x0000// der ganze Rest 
    ),
    0x02,                                      // bNumInterfaces           2
#else
#if STILL_IMAGE_SUPPORT == 0
  WBVAL(                                     // wTotalLength 
    USB_CONFIGUARTION_DESC_SIZE + UVC_INTERFACE_ASSOCIATION_DESC_SIZE + USB_INTERFACE_DESC_SIZE +
    UVC_VC_INTERFACE_HEADER_DESC_SIZE(1) +  UVC_CAMERA_TERMINAL_DESC_SIZE(2) + //camera sensor + 2 controls
    UVC_OUTPUT_TERMINAL_DESC_SIZE(0) + UVC_SELECTOR_UNIT_DESC_SIZE(1) +  //selector + 1 input pins
    UVC_PROCESSING_UNIT_DESC_SIZE(2) +       //                          processing + 2 control bytes
    USB_ENDPOINT_DESC_SIZE /*standard interrupt EP*/ + UVC_VC_ENDPOINT_DESC_SIZE /*class-specific interrupt EP*/ +
    USB_INTERFACE_DESC_SIZE /*VS interface alternate setting 0*/ + UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1) /*VS input header*/ +
    0x0B + /*VS Format Descriptor*/ FRAME_DES_TOTAL_LEN + /* VS Frame Descriptor */ +USB_INTERFACE_DESC_SIZE /*VS interface alternate setting 1*/ +
    USB_ENDPOINT_DESC_SIZE + 0x0000  // der ganze Rest 
      ),
#else
	WBVAL(                                     // wTotalLength 
    USB_CONFIGUARTION_DESC_SIZE + UVC_INTERFACE_ASSOCIATION_DESC_SIZE + USB_INTERFACE_DESC_SIZE +
    UVC_VC_INTERFACE_HEADER_DESC_SIZE(1) +  UVC_CAMERA_TERMINAL_DESC_SIZE(2) + //camera sensor + 2 controls
    UVC_OUTPUT_TERMINAL_DESC_SIZE(0) + UVC_SELECTOR_UNIT_DESC_SIZE(1) +  //selector + 1 input pins
    UVC_PROCESSING_UNIT_DESC_SIZE(2) +       //                          processing + 2 control bytes
    USB_ENDPOINT_DESC_SIZE /*standard interrupt EP*/ + UVC_VC_ENDPOINT_DESC_SIZE /*class-specific interrupt EP*/ +
    USB_INTERFACE_DESC_SIZE /*VS interface alternate setting 0*/ + UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1) /*VS input header*/ +
    0x0B + /*VS Format Descriptor*/ FRAME_DES_TOTAL_LEN + /* VS Frame Descriptor */ +USB_INTERFACE_DESC_SIZE /*VS interface alternate setting 1*/ + 0xa +
    USB_ENDPOINT_DESC_SIZE + 0x0000  // der ganze Rest 
      ),
#endif //STILL_IMAGE_SUPPORT
    0x02,                                      // bNumInterfaces           2
#endif //MCU_VERSION
#endif //C_USB_AUDIO
  0x01,                                      // bConfigurationValue      1 ID of this configuration
  0x00,                                      // iConfiguration           0 no description available
  USB_CONFIG_BUS_POWERED ,                   // bmAttributes          0x80 Bus Powered
  USB_CONFIG_POWER_MA(100),                  // bMaxPower              100 mA

/* Interface Association Descriptor */
  UVC_INTERFACE_ASSOCIATION_DESC_SIZE,       // bLength                  8
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, // bDescriptorType         11
//#if MCU_VERSION >= GPL327XX
#if (MCU_VERSION >= GPL327XX) || (MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)
  0x00,                                      // bFirstInterface          0
#else
  0x01,                                      // bFirstInterface          0
#endif
  0x02,                                      // bInterfaceCount          2
  CC_VIDEO,                                  // bFunctionClass          14 Video
  SC_VIDEO_INTERFACE_COLLECTION,             // bFunctionSubClass        3 Video Interface Collection
  PC_PROTOCOL_UNDEFINED,                     // bInterfaceProtocol       0 (protocol undefined)
//#if MCU_VERSION >= GPL327XX
#if (MCU_VERSION >= GPL327XX) || (MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)
  0x02,                                      // iFunction                4 LPC23xx UVC Device
#else
  0x04,                                      // iFunction                4 LPC23xx UVC Device
#endif

/* VideoControl Interface Descriptor */

/* Standard VC Interface Descriptor  = interface 0 */
  USB_INTERFACE_DESC_SIZE,                   // bLength                  9
  USB_INTERFACE_DESCRIPTOR_TYPE,             // bDescriptorType          4    
  USB_UVC_VCIF_NUM,                          // bInterfaceNumber         0 index of this interface
  0x00,                                      // bAlternateSetting        0 index of this setting
  0x01,                                      // bNumEndpoints            1 one interrupt endpoint
  CC_VIDEO,                                  // bInterfaceClass         14 Video
  SC_VIDEOCONTROL,                           // bInterfaceSubClass       1 Video Control
  PC_PROTOCOL_UNDEFINED,                     // bInterfaceProtocol       0 (protocol undefined)
//#if MCU_VERSION >= GPL327XX
#if (MCU_VERSION >= GPL327XX) || (MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)
  0x02,                                      // iFunction                4 LPC23xx UVC Device
#else
  0x04,                                      // iFunction                4 LPC23xx UVC Device
#endif

/* Class-specific VC Interface Descriptor */
  UVC_VC_INTERFACE_HEADER_DESC_SIZE(1),      // bLength                 13 12 + 1 (header + 1*interface
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_HEADER,                                 // bDescriptorSubtype       1 (HEADER)
  WBVAL(UVC_VERSION),                        // bcdUVC                1.10 or 1.00
  WBVAL(                                     // wTotalLength               header+units+terminals (no Endpoints)
    UVC_VC_INTERFACE_HEADER_DESC_SIZE(1) +   //                          header + 1 interface
    UVC_CAMERA_TERMINAL_DESC_SIZE(2) +       //                          camera sensor + 2 controls
    UVC_OUTPUT_TERMINAL_DESC_SIZE(0) +       //
    UVC_SELECTOR_UNIT_DESC_SIZE(1) +         //                          selector + 1 input pins
    UVC_PROCESSING_UNIT_DESC_SIZE(2)         //                          processing + 2 control bytes
      ),
  DBVAL(0x005B8D80),                         // dwClockFrequency  6.000000 MHz
  0x01,                                      // bInCollection            1 one streaming interface
//#if MCU_VERSION >= GPL327XX
#if (MCU_VERSION >= GPL327XX) || (MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)
  0x01,                                      // baInterfaceNr( 0)        1 VS interface 1 belongs to this VC interface
#else
  0x02,                                      // baInterfaceNr( 0)        1 VS interface 1 belongs to this VC interface
#endif

/* Input Terminal Descriptor (Camera) */
  UVC_CAMERA_TERMINAL_DESC_SIZE(2),          // bLength                 17 15 + 2 controls
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_INPUT_TERMINAL,                         // bDescriptorSubtype       2 (INPUT_TERMINAL)
  0x01,                                      // bTerminalID              1 ID of this Terminal
  WBVAL(ITT_CAMERA),                         // wTerminalType       0x0201 Camera Sensor
  0x00,                                      // bAssocTerminal           0 no Terminal assiciated
  0x00,                                      // iTerminal                0 no description available                                      
  WBVAL(0x0000),                             // wObjectiveFocalLengthMin 0
  WBVAL(0x0000),                             // wObjectiveFocalLengthMax 0
  WBVAL(0x0000),                             // wOcularFocalLength       0
  0x02,                                      // bControlSize             2
  0x00, 0x00,                                // bmControls          0x0000 no controls supported

#ifdef COMPOSITE_VIDEO
/* Input Terminal Descriptor (Composite) */   
  UVC_INPUT_TERMINAL_DESC_SIZE(0),           // bLength                  8
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_INPUT_TERMINAL,                         // bDescriptorSubtype       2 (INPUT_TERMINAL)
  0x02,                                      // bTerminalID              2 ID of this Terminal
  WBVAL(COMPOSITE_CONNECTOR),                // wTerminalType       0x0401 Composite video input
  0x00,                                      // bAssocTerminal           0 no Terminal assiciated
  0x00,                                      // iTerminal                0 no description available                                      
#endif
 
/* Output Terminal Descriptor */ 
  UVC_OUTPUT_TERMINAL_DESC_SIZE(0),          // bLength                  9
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_OUTPUT_TERMINAL,                        // bDescriptorSubtype       3 (OUTPUT_TERMINAL)
  0x03,                                      // bTerminalID              3 ID of this Terminal
  WBVAL(TT_STREAMING),                       // wTerminalType       0x0101 USB streaming terminal
  0x00,                                      // bAssocTerminal           0 no Terminal assiciated
  0x05,                                      // bSourceID                5 input pin connected to output pin unit 5
//#if MCU_VERSION >= GPL327XX
#if (MCU_VERSION >= GPL327XX) || (MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)
  0x04,                                      // iTerminal                0 no description available                                      
#else
  0x00,                                      // iTerminal                0 no description available                                      
#endif

#ifdef COMPOSITE_VIDEO
/* Selector Unit Descriptor */
  UVC_SELECTOR_UNIT_DESC_SIZE(2),            // bLength                  8 6 + 2 input pins
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_SELECTOR_UNIT,                          // bDescriptorSubtype       4 (SELECTOR UNIT)
  0x04,                                      // bUnitID                  4
  0x02,                                      // bNrInPins                2
  0x01,                                      // baSourceID(0)            1 see Input Terminal Descriptor (Camera)
  0x02,                                      // baSourceID(1)            2 see Input Terminal Descriptor (Composite)
  0x00,                                      // iSelector                0 no description available
#else
/* Selector Unit Descriptor */
  UVC_SELECTOR_UNIT_DESC_SIZE(1),            // bLength                  8 6 + 1 input pins
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_SELECTOR_UNIT,                          // bDescriptorSubtype       4 (SELECTOR UNIT)
  0x04,                                      // bUnitID                  4
  0x01,                                      // bNrInPins                1
  0x01,                                      // baSourceID(0)            1 see Input Terminal Descriptor (Camera)
  0x00,                                      // iSelector                0 no description available
#endif

/* Processing Unit Descriptor */
  UVC_PROCESSING_UNIT_DESC_SIZE(2),          // bLength                 12 10 + 2 control bytes
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VC_PROCESSING_UNIT,                        // bDescriptorSubtype       5 (PROCESSING_UNIT)
  0x05,                                      // bUnitID                  5
  0x04,                                      // bSourceID                4 input pin connected to output pin unit 4
  WBVAL(0x0000),                             // wMaxMultiplier           0 not used
  0x02,                                      // bControlSize             2 two control bytes
  WBVAL(UVC_PU_CONTROL_BM),                  // bmControls
  0x00,                                      // iProcessing              0 no description available
#if (UVC_VERSION == 0x0110)
  0x00,                                      // bmVideoStandards         0 none
#endif

/* Standard Interrupt Endpoint Descriptor */
// we use an interrupt endpoint for notification
  USB_ENDPOINT_DESC_SIZE,                   // bLength                  7 
  USB_ENDPOINT_DESCRIPTOR_TYPE,             // bDescriptorType          5 (ENDPOINT)
  USB_ENDPOINT_IN(1),                       // bEndpointAddress      0x81 EP 1 IN
  USB_ENDPOINT_TYPE_INTERRUPT,              // bmAttributes             3 interrupt transfer type
  //WBVAL(0x0008),                            // wMaxPacketSize      0x0008 1x 8 bytes
//#if MCU_VERSION >= GPL327XX
#if (MCU_VERSION >= GPL327XX) || (MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)
  WBVAL(0x0040),                            // wMaxPacketSize      0x0008 1x 8 bytes
  0x20,                                     // bInterval               32 ms polling interval
#else
  WBVAL(0x0010),                            // wMaxPacketSize      0x0008 1x 8 bytes
  0xa,                                     // bInterval               32 ms polling interval
#endif
         
/* Class-Specific Interrupt Endpoint Descriptor */
// mandatory if Standard Interrupt Endpoint is used
  UVC_VC_ENDPOINT_DESC_SIZE,                // bLength                  5
  CS_ENDPOINT,                              // bDescriptorType       0x25 (CS_ENDPOINT)
  EP_INTERRUPT,                             // bDescriptorSubtype       3 (EP_INTERRUPT)
//#if MCU_VERSION >= GPL327XX
#if (MCU_VERSION >= GPL327XX) || (MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)
  WBVAL(0x0008),                            // wMaxTransferSize         8 8-Byte status packet
#else
  WBVAL(0x0010),                            // wMaxTransferSize         8 8-Byte status packet
#endif

/* Video Streaming Interface Descriptor */

/* Standard VS Interface Descriptor  = interface 1 */
// alternate setting 0 = Zero Bandwidth
  USB_INTERFACE_DESC_SIZE,                   // bLength                  9
  USB_INTERFACE_DESCRIPTOR_TYPE,             // bDescriptorType          4    
  USB_UVC_VSIF_NUM,                          // bInterfaceNumber         1 index of this interface
  0x00,                                      // bAlternateSetting        0 index of this setting
  0x00,                                      // bNumEndpoints            0 no EP used
  CC_VIDEO,                                  // bInterfaceClass         14 Video
  SC_VIDEOSTREAMING,                         // bInterfaceSubClass       2 Video Streaming
  PC_PROTOCOL_UNDEFINED,                     // bInterfaceProtocol       0 (protocol undefined)
  0x00,                                      // iInterface               0 no description available

/* Class-specific VS Header Descriptor (Input) */
  UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1),// bLength               14 13 + (1*1) (no specific controls used)
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VS_INPUT_HEADER,                           // bDescriptorSubtype       5 (INPUT_HEADER)
  0x01,                                      // bNumFormats              1 one format descriptor follows
#if STILL_IMAGE_SUPPORT == 0
  WBVAL(                                     // wTotalLength               header+frame/format descriptors
    UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1) +//                     VS input header
    0x0B +                                   //                          VS Format Descriptor
    FRAME_DES_TOTAL_LEN                      //                          VS Frame Descriptor
      ),
#else
  WBVAL(                                     // wTotalLength               header+frame/format descriptors
    UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1) +//                     VS input header
    0x0B +                                   //                          VS Format Descriptor
    FRAME_DES_TOTAL_LEN + 0xa                //                          VS Frame Descriptor
      ),
#endif
 // USB_ENDPOINT_IN(6),                        // bEndPointAddress      0x86 EP 6 IN
 USB_ENDPOINT_IN(7),                        // bEndPointAddress      0x87 EP 6 IN
  0x00,                                      // bmInfo                   0 no dynamic format change supported
  0x03,                                      // bTerminalLink            3 supplies terminal ID 3 (Output terminal)
#if STILL_IMAGE_SUPPORT == 0
  0x01,                                      // bStillCaptureMethod      1 supports still image capture method1
#else
  0x02,
#endif
  0x01,                                      // bTriggerSupport          1 HW trigger supported for still image capture
  0x00,                                      // bTriggerUsage            0 HW trigger initiate a still image capture
  0x01,                                      // bControlSize             1 one byte bmaControls field size
  0x00,                                      // bmaControls(0)           0 no VS specific controls

/* Class-specific VS Format Descriptor */
  0x0B,                                      // bLength                 11
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VS_FORMAT_MJPEG,                           // bDescriptorSubtype       6 (VS_FORMAT_MJPEG)
  0x01,                                      // bFormatIndex             1 first (and only) format descriptor
  NUMBER_OF_FRAME_DES,                       // bNumFrameDescriptors     1 one frame descriptor follows
  0x01,                                      // bmFlags                  1 uses fixed size samples
  0x01,                                      // bDefaultFrameIndex       1 default frame index is 1
  0x00,                                      // bAspectRatioX            0 non-interlaced stream - not required
  0x00,                                      // bAspectRatioY            0 non-interlaced stream - not required
  0x00,                                      // bmInterlaceFlags         0 non-interlaced stream
  0x00,                                      // bCopyProtect             0 no restrictions

/* Class specific VS Frame Descriptor index 1 */
  0x26,                                      // bLength                 38
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VS_FRAME_MJPEG,                            // bDescriptorSubtype       7 (VS_FRAME_MJPEG)
  0x01,                                      // bFrameIndex              1 first (and only) Frame Descripot
#if (UVC_VERSION == 0x0110)
  0x03,                                      // bmCapabilities        0x03 Still images using capture method 1, fixed frame rate
#else
  0x01,                                      // bmCapabilities        0x01 Still images using capture method 1
#endif
  WBVAL(JPG_WIDTH),                          // wWidth                 176 width of frame is 176 pixels
  WBVAL(JPG_HEIGHT),                         // wHeight                144 hight of frame is 144 pixels
  DBVAL(0x000DEC00),                         // dwMinBitRate        912384 min bit rate in bits/s
  DBVAL(0x000DEC00),                         // dwMaxBitRate        912384 max bit rate in bits/s
 // DBVAL(0x00009480),                         // dwMaxVideoFrameBufferSize   38016 max video/still frame size in bytes
  DBVAL(0x00096000),                         // dwMaxVideoFrameBufferSize   38016 max video/still frame size in bytes 640*480*2
#if (FRAME_RATE_SETTING == 15) 
 //15 frame set
  DBVAL(0x000A2C2A),                         // dwDefaultFrameInterval     666666 default frame interval is 666666ns (15fps)
  0x00,                                      // bFrameIntervalType              0 continuous frame interval
  DBVAL(0x000A2C2A),                         // dwMinFrameInterval         666666 min frame interval is 666666ns (15fps) 
  DBVAL(0x000A2C2A),                         // dwMaxFrameInterval         666666 max frame interval is 666666ns (15fps)
#elif(FRAME_RATE_SETTING == 30)   
 //30 frame set 
  DBVAL(0x00051615),                         // dwDefaultFrameInterval     666666 default frame interval is 333333ns (30fps)
  0x00,                                      // bFrameIntervalType              0 continuous frame interval
  DBVAL(0x00051615),                         // dwMinFrameInterval         666666 min frame interval is 333333ns (30fps) 
  DBVAL(0x00051615),                         // dwMaxFrameInterval         666666 max frame interval is 333333ns (30fps)
#endif  
  DBVAL(0x00000000),                         // dwFrameIntervalStep             0 no frame interval step supported

#if (NUMBER_OF_FRAME_DES >= 2)
/* Class specific VS Frame Descriptor */
  0x26,                                      // bLength                 38
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VS_FRAME_MJPEG,                            // bDescriptorSubtype       7 (VS_FRAME_MJPEG)
  0x02,                                      // bFrameIndex              1 first (and only) Frame Descripot
#if (UVC_VERSION == 0x0110)
  0x03,                                      // bmCapabilities        0x03 Still images using capture method 1, fixed frame rate
#else
  0x01,                                      // bmCapabilities        0x01 Still images using capture method 1
#endif
  WBVAL(FRAME_INDEX2_WIDTH),                 // wWidth                 800 width
  WBVAL(FRAME_INDEX2_HEIGHT),                // wHeight                600 hight
  DBVAL(0x000DEC00),                         // dwMinBitRate        912384 min bit rate in bits/s
  DBVAL(0x000DEC00),                         // dwMaxBitRate        912384 max bit rate in bits/s
  DBVAL(0x00096000),                         // dwMaxVideoFrameBufferSize   38016 max video/still frame size in bytes 640*480*2
#if (FRAME_RATE_SETTING == 15) 
 //15 frame set
  DBVAL(0x000A2C2A),                         // dwDefaultFrameInterval     666666 default frame interval is 666666ns (15fps)
  0x00,                                      // bFrameIntervalType              0 continuous frame interval
  DBVAL(0x000A2C2A),                         // dwMinFrameInterval         666666 min frame interval is 666666ns (15fps) 
  DBVAL(0x000A2C2A),                         // dwMaxFrameInterval         666666 max frame interval is 666666ns (15fps)
#elif(FRAME_RATE_SETTING == 30)   
 //30 frame set 
  DBVAL(0x00051615),                         // dwDefaultFrameInterval     666666 default frame interval is 333333ns (30fps)
  0x00,                                      // bFrameIntervalType              0 continuous frame interval
  DBVAL(0x00051615),                         // dwMinFrameInterval         666666 min frame interval is 333333ns (30fps) 
  DBVAL(0x00051615),                         // dwMaxFrameInterval         666666 max frame interval is 333333ns (30fps)
#endif  
  DBVAL(0x00000000),                         // dwFrameIntervalStep             0 no frame interval step supported
#endif

#if (NUMBER_OF_FRAME_DES >= 3)
/* Class specific VS Frame Descriptor */
  0x26,                                      // bLength                 38
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VS_FRAME_MJPEG,                            // bDescriptorSubtype       7 (VS_FRAME_MJPEG)
  0x03,                                      // bFrameIndex              1 first (and only) Frame Descripot
#if (UVC_VERSION == 0x0110)
  0x03,                                      // bmCapabilities        0x03 Still images using capture method 1, fixed frame rate
#else
  0x01,                                      // bmCapabilities        0x01 Still images using capture method 1
#endif
  WBVAL(FRAME_INDEX3_WIDTH),                 // wWidth                 800 width
  WBVAL(FRAME_INDEX3_HEIGHT),                // wHeight                600 hight
  DBVAL(0x000DEC00),                         // dwMinBitRate        912384 min bit rate in bits/s
  DBVAL(0x000DEC00),                         // dwMaxBitRate        912384 max bit rate in bits/s
  DBVAL(0x00096000),                         // dwMaxVideoFrameBufferSize   38016 max video/still frame size in bytes 640*480*2
#if (FRAME_RATE_SETTING == 15) 
 //15 frame set
  DBVAL(0x000A2C2A),                         // dwDefaultFrameInterval     666666 default frame interval is 666666ns (15fps)
  0x00,                                      // bFrameIntervalType              0 continuous frame interval
  DBVAL(0x000A2C2A),                         // dwMinFrameInterval         666666 min frame interval is 666666ns (15fps) 
  DBVAL(0x000A2C2A),                         // dwMaxFrameInterval         666666 max frame interval is 666666ns (15fps)
#elif(FRAME_RATE_SETTING == 30)   
 //30 frame set 
  DBVAL(0x00051615),                         // dwDefaultFrameInterval     666666 default frame interval is 333333ns (30fps)
  0x00,                                      // bFrameIntervalType              0 continuous frame interval
  DBVAL(0x00051615),                         // dwMinFrameInterval         666666 min frame interval is 333333ns (30fps) 
  DBVAL(0x00051615),                         // dwMaxFrameInterval         666666 max frame interval is 333333ns (30fps)
#endif  
  DBVAL(0x00000000),                         // dwFrameIntervalStep             0 no frame interval step supported
#endif

#if (NUMBER_OF_FRAME_DES >= 4)
/* Class specific VS Frame Descriptor */
  0x26,                                      // bLength                 38
  CS_INTERFACE,                              // bDescriptorType         36 (INTERFACE)
  VS_FRAME_MJPEG,                            // bDescriptorSubtype       7 (VS_FRAME_MJPEG)
  0x04,                                      // bFrameIndex              1 first (and only) Frame Descripot
#if (UVC_VERSION == 0x0110)
  0x03,                                      // bmCapabilities        0x03 Still images using capture method 1, fixed frame rate
#else
  0x01,                                      // bmCapabilities        0x01 Still images using capture method 1
#endif
  WBVAL(FRAME_INDEX4_WIDTH),                 // wWidth                 800 width
  WBVAL(FRAME_INDEX4_HEIGHT),                // wHeight                600 hight
  DBVAL(0x000DEC00),                         // dwMinBitRate        912384 min bit rate in bits/s
  DBVAL(0x000DEC00),                         // dwMaxBitRate        912384 max bit rate in bits/s
  DBVAL(0x00096000),                         // dwMaxVideoFrameBufferSize   38016 max video/still frame size in bytes 640*480*2
#if (FRAME_RATE_SETTING == 15) 
 //15 frame set
  DBVAL(0x000A2C2A),                         // dwDefaultFrameInterval     666666 default frame interval is 666666ns (15fps)
  0x00,                                      // bFrameIntervalType              0 continuous frame interval
  DBVAL(0x000A2C2A),                         // dwMinFrameInterval         666666 min frame interval is 666666ns (15fps) 
  DBVAL(0x000A2C2A),                         // dwMaxFrameInterval         666666 max frame interval is 666666ns (15fps)
#elif(FRAME_RATE_SETTING == 30)   
 //30 frame set 
  DBVAL(0x00051615),                         // dwDefaultFrameInterval     666666 default frame interval is 333333ns (30fps)
  0x00,                                      // bFrameIntervalType              0 continuous frame interval
  DBVAL(0x00051615),                         // dwMinFrameInterval         666666 min frame interval is 333333ns (30fps) 
  DBVAL(0x00051615),                         // dwMaxFrameInterval         666666 max frame interval is 333333ns (30fps)
#endif  
  DBVAL(0x00000000),                         // dwFrameIntervalStep             0 no frame interval step supported
#endif

#if STILL_IMAGE_SUPPORT == 1
  0x0a,
  CS_INTERFACE,
  VS_STILL_IMAGE_FRAME,
  0x0,
  0x01,
  WBVAL(JPG_WIDTH),                          // wWidth                 176 width of frame is 176 pixels
  WBVAL(JPG_HEIGHT),                         // wHeight                144 hight of frame is 144 pixels
  0x00,
#endif
/* Standard VS Interface Descriptor  = interface 1 */
// alternate setting 1 = operational setting
  USB_INTERFACE_DESC_SIZE,                   // bLength                  9
  USB_INTERFACE_DESCRIPTOR_TYPE,             // bDescriptorType          4    
  USB_UVC_VSIF_NUM,                          // bInterfaceNumber         1 index of this interface
  0x01,                                      // bAlternateSetting        1 index of this setting
  0x01,                                      // bNumEndpoints            1 one EP used
  CC_VIDEO,                                  // bInterfaceClass         14 Video
  SC_VIDEOSTREAMING,                         // bInterfaceSubClass       2 Video Streaming
  PC_PROTOCOL_UNDEFINED,                     // bInterfaceProtocol       0 (protocol undefined)
  0x00,                                      // iInterface               0 no description available

/* Standard VS Isochronous Video data Endpoint Descriptor */
  USB_ENDPOINT_DESC_SIZE,                   // bLength                  7 
  USB_ENDPOINT_DESCRIPTOR_TYPE,             // bDescriptorType          5 (ENDPOINT)
 // USB_ENDPOINT_IN(6),                       // bEndpointAddress      0x86 EP 6 IN usb1.1
  USB_ENDPOINT_IN(7),                       // bEndpointAddress      0x86 EP 7 IN usb2.0
  USB_ENDPOINT_TYPE_ISOCHRONOUS |           // bmAttributes             5 isochronous transfer type
  USB_ENDPOINT_SYNC_ASYNCHRONOUS,           //                            asynchronous synchronizationtype
  //WBVAL(0x00060),                            // wMaxPacketSize      0x01FE max packet 96 bytes usb1.1
//#if MCU_VERSION >= GPL327XX
#if (MCU_VERSION >= GPL327XX) || (MCU_VERSION == GPL326XXB) || (MCU_VERSION == GP326XXXA)
  WBVAL(0x0400),                            // wMaxPacketSize      0x01FE max packet 1024 bytes usb2.0
#else
  WBVAL(0x0200),                            // wMaxPacketSize      0x01FE max packet 512 bytes usb2.0
#endif
  0x01,                                     // bInterval                1 one frame interval
  
#if C_USB_AUDIO == CUSTOM_ON
  /* audio IAD descriptor */
  0x8,                                       // bLength                  8
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, // bDescriptorType         11
  0x02,                                      // bFirstInterface          2
  0x02,                                      // bInterfaceCount          2
  0x01,                                       // bFunctionClass          audio
  0x00,                                       // bFunctionSubClass       audio stream
  PC_PROTOCOL_UNDEFINED,                     // bInterfaceProtocol       0 (protocol undefined)
  0x04,                                      // iFunction                

  /* interface 2 ,audio interface descriptor */ 
  USB_INTERFACE_DESC_SIZE,                   // bLength                  9
  USB_INTERFACE_DESCRIPTOR_TYPE,             // bDescriptorType          4    
  0x02,                                      // bInterfaceNumber         2 index of this interface
  0x00,                                      // bAlternateSetting        0 index of this setting
  0x00,                                      // bNumEndpoints            
  0x01,                                      // bInterfaceClass          audio
  0x01,                                      // bInterfaceSubClass       audio Control interface
  PC_PROTOCOL_UNDEFINED,                     // bInterfaceProtocol       0 (protocol undefined)
  0x04,                                      // iFunction                
  
  /* Audio Control Header Interface */
  AUDIO_CONTROL_INTERFACE_DESC_SZ(1),   /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
  WBVAL(0x0100), /* 1.00 */             /* bcdADC */
  WBVAL(                                /* wTotalLength */
    AUDIO_CONTROL_INTERFACE_DESC_SZ(1) +  /* 9 */
    AUDIO_INPUT_TERMINAL_DESC_SIZE     +  /* 12 */
    AUDIO_FEATURE_UNIT_DESC_SZ(0,1)    +  /* 9 */
    AUDIO_OUTPUT_TERMINAL_DESC_SIZE       /* 9 */
  ),
  1,                                    /* bInCollection Number of streaming interface */
  3,                                    /* audiostreaming interface 1 belongs to this audiocontrol interface */  
  
  /* Audio Input Terminal */
  AUDIO_INPUT_TERMINAL_DESC_SIZE,       /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */
  0x03,                                 /* bTerminalID */
  WBVAL(AUDIO_TERMINAL_MICROPHONE),     /* wTerminalType */
  0x00,                                 /* bAssocTerminal */
  0x01,                                 /* bNrChannels */
  WBVAL(AUDIO_CHANNEL_M),               /* wChannelConfig,mono */
  0x00,                                 /* iChannelNames */
  0x00,                                 /* iTerminal */
  
  /* Audio Feature Unit */
  AUDIO_FEATURE_UNIT_DESC_SZ(0,1),      /* bLength, 8 */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_FEATURE_UNIT,           /* bDescriptorSubtype, 6 */
  0x05,                                 /* bUnitID */
  0x03,                                 /* bSourceID */
  0x01,                                 /* bControlSize */
  AUDIO_CONTROL_MUTE |
  AUDIO_CONTROL_VOLUME,                 /* bmaControls(0) */
  0x00,                                 /* iTerminal */
  
  /* Audio Output Terminal */
  AUDIO_OUTPUT_TERMINAL_DESC_SIZE,      /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */
  0x04,                                 /* bTerminalID */
  WBVAL(AUDIO_TERMINAL_USB_STREAMING),  /* wTerminalType */
  0x00,                                 /* bAssocTerminal */
  0x05,                                 /* bSourceID */
  0x00,                                 /* iTerminal */
  
  /* Interface 3, Alternate Setting 0, Audio Streaming - Zero Bandwith */
  USB_INTERFACE_DESC_SIZE,              /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x03,                                 /* bInterfaceNumber */
  0x00,                                 /* bAlternateSetting */
  0x00,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  
  /* Interface 3, Alternate Setting 1, Audio Streaming - Operational */
  USB_INTERFACE_DESC_SIZE,              /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x03,                                 /* bInterfaceNumber */
  0x01,                                 /* bAlternateSetting */
  0x01,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  
  /* Audio Streaming Interface */
  AUDIO_STREAMING_INTERFACE_DESC_SIZE,  /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */
  0x04,                                 /* bTerminalLink */
  0x01,                                 /* bDelay */
  WBVAL(AUDIO_FORMAT_PCM),              /* wFormatTag */
  
  /* Audio Type I Format */
  AUDIO_FORMAT_TYPE_I_DESC_SZ(1),       /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
  AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
  0x01,                                 /* bNrChannels */
  0x02,                                 /* bSubFrameSize */
  16,                                   /* bBitResolution */
  0x01,                                 /* bSamFreqType */
  B3VAL(8000),                         /* tSamFreq */
  //B3VAL(11025),                         /* tSamFreq */
  //B3VAL(16000),                         /* tSamFreq */
  //B3VAL(22050),                         /* tSamFreq */
  //B3VAL(32000),                         /* tSamFreq */
  //B3VAL(44100),                         /* tSamFreq */
  //B3VAL(48000),                         /* tSamFreq */
  //B3VAL(96000),
  
  /* Endpoint - Standard Descriptor */
  AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */
  USB_ENDPOINT_IN(6),                  /* bEndpointAddress */
  USB_ENDPOINT_TYPE_ISOCHRONOUS,        /* bmAttributes */
  WBVAL(192),                            /* wMaxPacketSize */
  0x4,                                 /* bInterval */
  0x00,                                 /* bRefresh */
  0x00,                                 /* bSynchAddress */
  
  /* Endpoint - Audio Streaming */
  AUDIO_STREAMING_ENDPOINT_DESC_SIZE,   /* bLength */
  AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
  AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */
  0x00,                                 /* bmAttributes */
  0x00,                                 /* bLockDelayUnits */
  WBVAL(0x0000),                        /* wLockDelay */
#endif
/* Terminator */
  0x00                                      // bLength                  0
};


#define MAX_STRDESC_SIZE  0x30             // see IAD string
const BYTE USB_StringDescriptor[][MAX_STRDESC_SIZE] = { // zweidimensionales feld
  {
    /* Index 0x00: LANGID Codes */
    0x04,                                   // bLength                  4
    USB_STRING_DESCRIPTOR_TYPE,             // bDescriptorType          3 (STRING)
    WBVAL(0x0409)                          // wLANGID             0x0409 US English
  },
  {
    /* Index 0x01: Manufacturer */
     0x10,                                   // bLength                 28
    USB_STRING_DESCRIPTOR_TYPE,             // bDescriptorType          3 (STRING)
    'G',0,
    'E',0,
    'N',0,
    'E',0,
    'R',0,
    'A',0,
    'L',0
    },
  {
    /* Index 0x02: Product */
    0x1E,                                   // bLength                 36
    USB_STRING_DESCRIPTOR_TYPE,             // bDescriptorType          3 (STRING)
    'G',0,
    'E',0,
    'N',0,
    'E',0,
    'R',0,
    'A',0,
    'L',0,
    ' ',0,
    '-',0,
    ' ',0,
    'U',0,
    'V',0,
    'C',0,
    ' ',0
  },
  {
    /* Index 0x03: Serial Number */
    0x14,                                   // bLength                  20
    USB_STRING_DESCRIPTOR_TYPE,             // bDescriptorType          3 (STRING)
    'D',0,
    'e',0,
    'm',0,
    'o',0,
    ' ',0,
    '1',0,
    '.',0,
    '0',0,
    '0',0
  },
  {
    /* Index 0x04:  */
    0x20,                                   // bLength                 
    USB_STRING_DESCRIPTOR_TYPE,             // bDescriptorType          3 (STRING)
    'G',0,
    'E',0,
    'N',0,
    'E',0,
    'R',0,
    'A',0,
    'L',0,
    ' ',0,
    '-',0,
    ' ',0,
    'A',0,
    'U',0,
    'D',0,
    'I',0,
    'O',0
  },
  {
    /* Index 0x05: endof string  */
    0x04,                                   // bLength                  4
    USB_STRING_DESCRIPTOR_TYPE,             // bDescriptorType          3 (STRING)
    0,0
  }
};

void GetConfigLen(void)
{
	int len;

	len = (                                     // wTotalLength 
    USB_CONFIGUARTION_DESC_SIZE +            //
    9+
    UVC_INTERFACE_ASSOCIATION_DESC_SIZE +    //
    USB_INTERFACE_DESC_SIZE +                //                          VC interface
    UVC_VC_INTERFACE_HEADER_DESC_SIZE(1) +   //                          header + 1 interface
    UVC_CAMERA_TERMINAL_DESC_SIZE(2) +       //                          camera sensor + 2 controls
//#ifdef COMPOSITE_VIDEO
//    UVC_INPUT_TERMINAL_DESC_SIZE(0) +        //                          composite video input
//#endif
    UVC_OUTPUT_TERMINAL_DESC_SIZE(0) +       //
//#ifdef COMPOSITE_VIDEO
//    UVC_SELECTOR_UNIT_DESC_SIZE(2) +         //                          selector + 2 input pins
//#else
    UVC_SELECTOR_UNIT_DESC_SIZE(1) +         //                          selector + 1 input pins
//#endif
    UVC_PROCESSING_UNIT_DESC_SIZE(2) +       //                          processing + 2 control bytes
    USB_ENDPOINT_DESC_SIZE +                 //                          standard interrupt EP
    UVC_VC_ENDPOINT_DESC_SIZE +              //                          class-specific interrupt EP
    7+
    USB_INTERFACE_DESC_SIZE +                //                          VS interface alternate setting 0
    UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1) +//                     VS input header
    0x0B +                                   //                          VS Format Descriptor
    0x26 +                                   //                          VS Frame Descriptor
    USB_INTERFACE_DESC_SIZE +                //                          VS interface alternate setting 1
    USB_ENDPOINT_DESC_SIZE +                 //                          Video data EP
    9+
//    7+
    0x0000                                   // der ganze Rest 
      );
//	DBG_PRINT("GetConfigLen --111222--> len = %0x\r\n", len);	
	

}

unsigned int Get_Descriptor_Length(unsigned int DescriptorType)
{
	int bbk;

  switch(DescriptorType)
  {
    case C_Device_Descriptor_Table:
       return USB_DeviceDescriptor[0];  //Device_Descriptor_TBL[0];
    break;	
    case C_Config_Descriptor_Table:
//	return ((USB_CONFIGURATION_DESCRIPTOR *)&USB_ConfigDescriptor[0])->wTotalLength;
	bbk= ((USB_CONFIGURATION_DESCRIPTOR *)&USB_ConfigDescriptor[0])->wTotalLength;
//	DBG_PRINT("GetConfigLen --1112hhhhhhhhhhhjjjjjjjjjjjjj22--> bbk = %0x\r\n", bbk);	
	return bbk;
    break;	

    case C_String0_Descriptor_Table:
        return USB_StringDescriptor[0][0];//String0_Descriptor[0];
    break;	
    case C_String1_Descriptor_Table:    
	  if (s_usbd_user_string)  
	    return user_string_size;    
	  else  
        return USB_StringDescriptor[1][0];//String1_Descriptor[0];
    break;	
    
	case C_String2_Descriptor_Table:	
	  if (s_usbd_user_string)  
		return user_string_size;	
	  else	
		return USB_StringDescriptor[2][0];//String1_Descriptor[0];
	break;	

	case C_String3_Descriptor_Table:	
//	  if (s_usbd_user_string)  
//		return user_string_size;	
//	  else	
		return USB_StringDescriptor[3][0];//String1_Descriptor[0];
	break;	

  case C_String4_Descriptor_Table:	  
//	if (s_usbd_user_string)  
//	  return user_string_size;	  
//	else  
		//DBG_PRINT("DataPointer ===  C_Config_Descriptor_Table, len = %d\r\n", sizeof(USB_ConfigDescriptor)); 
	  return USB_StringDescriptor[4][0];//String1_Descriptor[0];
  break;  

  case C_String5_Descriptor_Table:	  
//	if (s_usbd_user_string)  
//	  return user_string_size;	  
//	else  
	  return USB_StringDescriptor[5][0];//String1_Descriptor[0];
  break;  

  }
   return 0;
}

void DataPointer(unsigned int DescriptorType)
{
  switch(DescriptorType)
  {
    case C_Device_Descriptor_Table:
//		DBG_PRINT("DataPointer ===  C_Device_Descriptor_Table\r\n"); 
	    EP0dataPt = &USB_DeviceDescriptor[0];//&Device_Descriptor_TBL[0];
    break;	
    case C_Config_Descriptor_Table:
//		DBG_PRINT("DataPointer ===  C_Config_Descriptor_Table\r\n");
    	EP0dataPt = &USB_ConfigDescriptor[0];//&Config_Descriptor_TBL[0];
    break;	
    case C_String0_Descriptor_Table:
//		DBG_PRINT("DataPointer ===  C_String0_Descriptor_Table\r\n"); 
	    EP0dataPt = (BYTE *)&USB_StringDescriptor[0];//&String0_Descriptor[0];
    break;	
    case C_String1_Descriptor_Table:
//		DBG_PRINT("DataPointer ===  C_String1_Descriptor_Table\r\n"); 
//   	    if (s_usbd_user_string)  
//	   		 EP0dataPt = user_string_pt;    
//	    else  
    		 EP0dataPt = (BYTE *)&USB_StringDescriptor[1];//&String1_Descriptor[0];
		break;	

	case C_String2_Descriptor_Table:
//		DBG_PRINT("DataPointer ===	C_String2_Descriptor_Table\r\n"); 
//		if (s_usbd_user_string)  
//			 EP0dataPt = user_string_pt;	
//		else  
			 EP0dataPt = (BYTE *)&USB_StringDescriptor[2];//&String1_Descriptor[0];
    	break;	

	case C_String3_Descriptor_Table:
//		DBG_PRINT("DataPointer ===	C_String3_Descriptor_Table\r\n"); 
//		if (s_usbd_user_string)  
//			 EP0dataPt = user_string_pt;	
//		else
			 EP0dataPt = (BYTE *)&USB_StringDescriptor[3];//&String1_Descriptor[0];
    	break;	

	   case C_String4_Descriptor_Table:
//		   DBG_PRINT("DataPointer ===  C_String4_Descriptor_Table\r\n"); 
//		   if (s_usbd_user_string)	
//				EP0dataPt = user_string_pt;    
//		   else
				EP0dataPt = (BYTE *)&USB_StringDescriptor[4];//&String1_Descriptor[0];
		   break;  

	   case C_String5_Descriptor_Table:
//		   DBG_PRINT("DataPointer ===  C_String5_Descriptor_Table\r\n"); 
//		   if (s_usbd_user_string)	
//				EP0dataPt = user_string_pt;    
//		   else
				EP0dataPt = (BYTE *)&USB_StringDescriptor[5];//&String1_Descriptor[0];
		   break;  

   /* 
    case C_String2_Descriptor_Table:
    EP0dataPt = &String2_Descriptor[0];
    break;	
    case C_String3_Descriptor_Table:
    EP0dataPt = &String3_Descriptor[0];
    break;	
   */
    //MSDC
//    case C_MaxLUN_Table:
//    EP0dataPt = (char *)& Max_LUN;
//    break;	

  }  
}

void usbd_desc_jpg_size_set(INT16U width,INT16U height)
{
	INT32U offset;
	
	/* Set first frame(index 1) resolution */
	offset = USB_CONFIGUARTION_DESC_SIZE + UVC_INTERFACE_ASSOCIATION_DESC_SIZE + USB_INTERFACE_DESC_SIZE + UVC_VC_INTERFACE_HEADER_DESC_SIZE(1) +
	         UVC_CAMERA_TERMINAL_DESC_SIZE(2) + UVC_OUTPUT_TERMINAL_DESC_SIZE(0) + UVC_SELECTOR_UNIT_DESC_SIZE(1) + UVC_PROCESSING_UNIT_DESC_SIZE(2) +
	         USB_ENDPOINT_DESC_SIZE + UVC_VC_ENDPOINT_DESC_SIZE + USB_INTERFACE_DESC_SIZE + UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1) + 0x0B +
	         + 5;
	USB_ConfigDescriptor[offset] = width & 0xFF;
	USB_ConfigDescriptor[offset+1] =(width>>8) & 0xFF;
	USB_ConfigDescriptor[offset+2] = height & 0xFF;
	USB_ConfigDescriptor[offset+3] = (height>>8) & 0xFF;
}
