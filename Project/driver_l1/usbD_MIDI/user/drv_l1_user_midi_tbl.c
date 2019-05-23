/**********************************************************
* Purpose: USB descriptor definition for MIDI class
*
* Author: Eugenehsu
*
* Date: 2012/07/13
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.01
* History :
              
**********************************************************/
#include "driver_l1.h"

char MIDI_Device_Descriptor_TBL[] = 
{
	 // Device Descriptor
     0x12,
 	 0x01,				        //bDescriptorType	: Device
	 0x10, 0x01,				//bcdUSB			: version 1.10
	 0x00, 						//bDeviceClass
	 0x00, 						//bDeviceSubClass
	 0x00,						//bDeviceProtocol
     0x08,                      //bMaxPacketSize0	
	 0xF3, 0x21,				//idVendor	0x21F3
	 0x01, 0x10,				//idProduct 0x1001
	 0x00, 0x01,				//bcdDevice
	 0x00,						//iManufacturer
	 0x01,						//iProduct
	 0x00,						//iSerialNumber
	 0x01,						//bNumConfigurations
};
	
char MIDI_Config_Descriptor_TBL[] =
{
	//Configuration (0x09 byte)
    0x09,                   //bLength: 0x09 byte
    0x02,                   //bDescriptorType: CONFIGURATION
    0x65,                   //wTotalLength:		//65 = 122 bytes
    0x00,
    0x02,                   //bNumInterfaces: 1 interfaces
    0x01,                   //bConfigurationValue: configuration 1
    0x00,                   //iConfiguration: index of string
    0x80,                    //bmAttributes: self powered, Not Support Remote-Wakeup
    0x32,                   //MaxPower: 100 mA
// Interface_Descriptor
    0x09,                   //bLength: 0x09 byte
    0x04,                   //bDescriptorType: INTERFACE
    0x00,                   //bInterfaceNumber: interface 0
    0x00,                   //bAlternateSetting: alternate setting 0
    0x00,                   //bNumEndpoints: 0 endpoints
    0x01,                   //bInterfaceClass: Audio Class
    0x01,                   //bInterfaceSubClass:
    0x00,                   //bInterfaceProtocol
    0x00,                   //iInterface: index of string
//AC Interface Header Descriptor:     
	0x09,					//bLength: 0x09 bytes
	0x24,					//bDescriptorType: CS_INTERFACE type
	0x01,
	0x00, 0x01, 			// Audio class 1.00  
	0x09, 0x00,				// wTotalLength
	0x01,
	0x01,
//InterfaceMS Descriptor
	0x09,                   //bLength: 0x09 byte
	0x04,                   //bDescriptorType: INTERFACE
	0x01,                   //bInterfaceNumber: interface 0
	0x00,                   //bAlternateSetting: alternate setting 0
	0x02,                   //bNumEndpoints: 2 endpoints(EP1,EP2)
	0x01,                   //bInterfaceClass: Audio class
	0x03,                   //bInterfaceSubClass:MIDIStream
	0x00,                   //bInterfaceProtocol
	0x00,                   //iInterface: index of string
//InterfaceAD Descriptor
	0x07,                   //bLength: 0x07 byte
	0x24,                   //bDescriptorType: CS_INTERFACE descriptor.
	0x01,                   //bDescriptorSubtype: MS_HEADER subtype.
	0x00, 0x01,             //BcdADC: Audio class 1.0
	0x41, 0x00,             //Total size of class-specific descriptors
//InterfaceJKEI Descriptor  (Embedded)
 	0x06,                   //bLength: 0x06 byte
    0x24,                   //bDescriptorType: CS_INTERFACE descriptor.
    0x02,                   //bDescriptorSubtype: MIDI_IN_JACK subtype.
    0x01,                   //bJackType : EMBEDDED
    0x01,                   //bJackID   :1
    0x00,                   //X 
//InterfaceJKXI Descriptor	(External)
 	0x06,                   //bLength: 0x06 byte
    0x24,                   //bDescriptorType: CS_INTERFACE descriptor.
    0x02,                   //bDescriptorSubtype: MIDI_IN_JACK subtype.
    0x02,                   //bJackType :EXTERNAL.
    0x02,                   //bJackID   : 2
    0x00,                   //X
//InterfaceJKEO Descriptor	(Embedded)
	0x09,                   //bLength: 0x09 byte
	0x24,                   //bDescriptorType: CS_INTERFACE descriptor.
	0x03,                   //bDescriptorSubtype: MIDI_OUT_JACK subtype.
	0x01,                   //bJackType :EMBEDDED.
	0x03,                   //bJackID   : 3
	0x01,                   //Number of Input Pins of this Jack.
	0x02,                   //ID of the Entity connected.
	0x01,                   //Output Pin number of the Entity this Input Pin is connected. 
	0x00,                   //X
//InterfaceJKXO_Descriptor	(External)
	0x09,                   //bLength: 0x09 byte
	0x24,                   //bDescriptorType: CS_INTERFACE descriptor.
	0x03,                   //bDescriptorSubtype: MIDI_OUT_JACK subtype.
	0x02,                   //bJackType :EXTERNAL.
	0x04,                   //bJackID   : 4
	0x01,                   //Number of Input Pins of this Jack.
	0x01,                   //ID of the Entity connected.
	0x01,                   //Output Pin number of the Entity this Input Pin is connected. 
	0x00,                   //X
//MIDI Adapter Standard Bulk OUT Endpoint Descriptor ,MSO Endpoit Descriptor
	0x09,					//bLength
	0x05,					//ENDPOINT type
	0x01,					//bEndpointAddress
	0x02,        	        //bmAttributes: BULK
	0x10, 0x00,    	        //wMaxPacketSize :0x10
	0x00,          		    //bInterval : 0
	0x00,                   //X
	0x00,                   //X
//MIDI Adapter Class-specific Bulk OUT Endpoint Descriptor ,MSOC Endpoit Descriptor
    0x05,					//bLength
	0x25,					//CS_ENDPOINT descriptor
	0x01,                	//MS_GENERAL subtype.
    0x01,                	//Number of embedded MIDI IN Jacks.
    0x01,                	//ID of the Embedded MIDI IN Jack.
//MIDI Adapter Standard Bulk IN Endpoint Descriptor ,MSI Endpoit Descriptor
	0x09,					//bLength
	0x05,					//ENDPOINT type
	0x82,					//bEndpointAddress
	0x02,	                //bmAttributes: BULK
	0x08, 0x00,             //wMaxPacketSize :0x08
	0x00,	                //bInterval : 0
	0x00,	                //X
	0x00,	                //X
//MIDI Adapter Class-specific Bulk IN Endpoint Descriptor ,MSIC Endpoit Descriptor
    0x05,					//bLength
	0x25,					//CS_ENDPOINT descriptor
	0x01,       	        //MS_GENERAL subtype.
    0x01,           	    //Number of embedded MIDI OUT Jacks.
    0x03,              	    //ID of the Embedded MIDIOUT Jack.
#if 0    	
//  Endpoint1
	0x07,                   //bLength: 0x07 byte
	0x05,                   //bDescriptorType: ENDPOINT
	0x81,                   //bEndpointAddress: IN endpoint 1 --Chamber For PA8591
	0x02,                   //bmAttributes: Bulk
	0x40, 0x00,             //wMaxPacketSize: 64 byte
	0x00,                   //bInterval: ignored
//Endpoint2                
	0x07,                   //bLength: 0x07 byte
	0x05,                   //bDescriptorType: ENDPOINT
	0x02,                   //bEndpointAddress: OUT endpoint 2 -- Chamber For PA8591
	0x02,                   //bmAttributes: Bulk
	0x40, 0x00,             //wMaxPacketSize: 64 byte
	0x00,                   //bInterval: ignored
//Endpoint 3
	0x07,                   //bLength: 0x07 byte
	0x05,                   //bDescriptorType: ENDPOINT
	0x83,                   //bEndpointAddress: IN endpoint 3 -- Chamber For PA8591
	0x03,                   //bmAttributes: Interrupt
	0x02, 0x00,             //wMaxPacketSize: 2 byte
	0x01,                   //bInterval: polling interval is 1 ms
#endif	
};
 
char MIDI_String0_Descriptor[]={
	 0x04,		//bLength
	 0x03,		//bDescriptorType
	 0x09, 0x04,//bString
};

char MIDI_String1_Descriptor[] ={
	0x34,		//bLength
	0x03,		//bDescriptorType
	'i', 0x00,	//bString
	'i', 0x00,
	'i', 0x00,
	' ', 0x00,
	'Y', 0x00,
	'o', 0x00,
	'u', 0x00,
	' ', 0x00,
	'R', 0x00,
	'o', 0x00,
	'c', 0x00,
	'k', 0x00,
	' ', 0x00,
	'G', 0x00,
	'u', 0x00,
	'i', 0x00,
	't', 0x00,
	'a', 0x00,
	'r', 0x00,
	' ', 0x00,
	'V', 0x00,
	'0', 0x00,
	'.', 0x00,
	'0', 0x00,
	'0', 0x00
 };

char MIDI_String2_Descriptor[]=
{
     0x1E,		//bLength
	 0x03,		//bDescriptorType
	 'Y', 0x00,	//bString
	 'o', 0x00,
	 'u', 0x00,
	 'R', 0x00,
	 'o', 0x00,
	 'c', 0x00,
	 'k', 0x00,
	 'G', 0x00,
	 'u', 0x00,
	 'i', 0x00,
	 't', 0x00,
	 'a', 0x00,
	 'r', 0x00,
	 ' ', 0x00
};

char MIDI_String3_Descriptor[]=
{
	0x22,		//bLength
	0x03,		//bDescriptorType
	'1', 0x00,	//bString
	'2', 0x00,
	'3', 0x00,
	'4', 0x00,
	'5', 0x00,
	'6', 0x00,
	'7', 0x00,
	'8', 0x00,
	'9', 0x00,
	'A', 0x00,
	'B', 0x00,
	'C', 0x00,
	'D', 0x00,
	'E', 0x00,
	'F', 0x00,
	'G', 0x00,	
};

char MIDI_String4_Descriptor[]=
{
	0x0C,		//bLength
	0x03,		//bDescriptorType
	'1', 0x00,	//bString
	'.', 0x00,
	'0', 0x00,
	'.', 0x00,
	'0', 0x00,
};

char MIDI_mode1_TBL[] =
{
	0xC0,
	0xFF
};

char MIDI_mode2_TBL[] =
{
	0xA0,
	0xE3
};

char MIDI_mode3_TBL[] =
{
	0xF0,
	0xFF
};

char MIDI_mode4_TBL[] =
{
	0x30,
	0x00
};