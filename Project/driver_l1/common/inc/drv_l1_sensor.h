#ifndef __drv_l1_SENSOR_H__
#define __drv_l1_SENSOR_H__

#include "driver_l1.h"
#include "drv_l1_sfr.h"

/* sensor configure setting */
#if BOARD_TYPE == BOARD_EMU_V2_0
	#define	 SCCB_SCL			IO_F5	
	#define	 SCCB_SDA   		IO_C1	
	#define	 SENSOR_ENABLE		IO_G11
	#define	 SENSOR_RESET	 	0xFF
#elif BOARD_TYPE == BOARD_DEMO_GPL32XXX
	#define	 SCCB_SCL   		IO_G11	
	#define	 SCCB_SDA   		IO_G10
	#define	 SENSOR_ENABLE		IO_G13
	#define	 SENSOR_RESET	 	0xFF		
#elif BOARD_TYPE == BOARD_DEMO_GPL326XX
	#define	 SCCB_SCL   		IO_B9	
	#define	 SCCB_SDA   		IO_B8
	#define	 SENSOR_ENABLE		IO_H7
	#define	 SENSOR_RESET	 	0xFF	
#elif (BOARD_TYPE == BOARD_DEMO_GPL327XX) || (BOARD_TYPE == BOARD_DEMO_GP327XXXA)
	#define	 SCCB_SCL   		IO_C2	
	#define	 SCCB_SDA   		IO_C1
	#define	 SENSOR_ENABLE		IO_C3
	#define	 SENSOR_RESET	 	0xFF
#elif (BOARD_TYPE == BOARD_DEMO_GPL326XXB) || (BOARD_TYPE == BOARD_DEMO_GP326XXXA)
	#define	 SCCB_SCL   		IO_C2	
	#define	 SCCB_SDA   		IO_C1
	#define	 SENSOR_ENABLE		IO_C3
	#define	 SENSOR_RESET	 	0xFF
#else
	#define	 SCCB_SCL   		IO_F5	
	#define	 SCCB_SDA   		IO_C1
	#define	 SENSOR_ENABLE		IO_G11
	#define	 SENSOR_RESET	 	0xFF	
#endif

#define CSI_CLOCK_SYS_CLK_DIV2		0
#define CSI_CLOCK_27MHZ				1
#define CSI_CLOCK_SYS_CLK_DIV4		2
#define CSI_CLOCK_13_5MHZ			3
#if (defined __HSB0357_DRV_C__)
	#define CSI_CLOCK				CSI_CLOCK_SYS_CLK_DIV4
#elif (defined __OV3640_MIPI_DRV_C__) || (defined __GC0307_DRV_C_)
	#define CSI_CLOCK				CSI_CLOCK_13_5MHZ
#else
	#define CSI_CLOCK				CSI_CLOCK_13_5MHZ//CSI_CLOCK_SYS_CLK_DIV4
#endif

// For OID sensor
#define OID_SENSOR_CHIP_VERSION_A600	0x012A
#define OID_SENSOR_CHIP_VERSION_A601	0x023E
#define OID_SENSOR_CHIP_VERSION_UNKNOWN	0xFFFF

#define	OV6680_ID				0xC0
#define OV7680_ID				0x42
#define OV7670_ID				0x42
#define OV9655_ID				0x60
#define OV9660_ID				0x60
#define OV2655_ID				0x60
#define OV5642_ID				0x60
#define OV3640_ID				0x78
#define OV7675_ID				0x42
#define OV2643_ID				0x60
#define OV2659_ID				0x60
#define OID_SENSOR_ID			0x92
#define BF3710_ID				0xDC
#define HSB0357_ID				0x60
#define GC0307_ID				0x42
#define GC0308_ID				0x42
#define GC0329_ID				0x62
#define GC2155_ID				0x78
#define GC2145_ID				0x78


/*
* Function Name :  sccb_init
*
* Syntax : void sccb_init(void);
*
* Purpose :  SCCB initialization
*
* Parameters : <IN>  none
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void sccb_init(void);
/*
* Function Name :  sccb_write_Reg8Data8
*
* Syntax : INT32S sccb_write_Reg8Data8(INT8U id, INT8U addr, INT8U data);
*
* Purpose :  SCCB write register process, register addr is 8 bits and register data is 8 bits
*
* Parameters : <IN>	id: 				Slave ID
*					addr:				Register Address
*					data:				Register Data
*              <OUT> none
* Return : 0: Success
*			-1: Fail
*
* Note :
*
*/
extern INT32S sccb_write_Reg8Data8(INT8U id, INT8U addr, INT8U data);
/*
* Function Name :  sccb_read_Reg8Data8
*
* Syntax : INT32S sccb_read_Reg8Data8(INT8U id, INT8U addr, INT8U data);
*
* Purpose :  SCCB read register process, register addr is 8 bits and register data is 8 bits
*
* Parameters : <IN>	id: 				Slave ID
*					*addr:				Register Address								
*              <OUT> data:			Register Data
* Return : 0: Success
*			-1: Fail
*
* Note :
*
*/
extern INT32S sccb_read_Reg8Data8(INT8U id, INT8U addr, INT8U *data);
/*
* Function Name :  sccb_write_Reg8Data16
*
* Syntax : INT32S sccb_write_Reg8Data16(INT8U id, INT8U addr, INT16U data) ;
*
* Purpose :  SCCB write register process, register addr is 8 bits and register data is 16 bits
*
* Parameters : <IN>	id: 				Slave ID
*					addr:				Register Address
*					data:				Register Data
*              <OUT> none
* Return : 0: Success
*			-1: Fail
*
* Note :
*/
extern INT32S sccb_write_Reg8Data16(INT8U id, INT8U addr, INT16U data);
/*
* Function Name :  sccb_read_Reg8Data16
*
* Syntax : INT32S sccb_read_Reg8Data16(INT8U id, INT8U addr, INT16U *data);
*
* Purpose :  SCCB read register process, register addr is 8 bits and register data is 16 bits
*
* Parameters : <IN>	id: 				Slave ID
*					addr:				Register Address								
*              <OUT> *data:			Register Data
* Return : 0: Success
*			-1: Fail
*
* Note :
*
*/
extern INT32S sccb_read_Reg8Data16(INT8U id, INT8U addr, INT16U *data);
/*
* Function Name :  sccb_write_Reg16Data8
*
* Syntax : INT32S sccb_write_Reg16Data8(INT8U id, INT16U addr, INT8U data);
*
* Purpose :  SCCB write register process, register addr is 16 bits and register data is 8 bits
*
* Parameters : <IN>	id: 				Slave ID
*					addr:				Register Address
*					data:				Register Data
*              <OUT> none
* Return : 0: Success
*			-1: Fail
*
* Note :
*/
extern INT32S sccb_write_Reg16Data8(INT8U id, INT16U addr, INT8U data); 
/*
* Function Name :  sccb_read_Reg16Data8
*
* Syntax : sccb_read_Reg16Data8(INT8U id, INT16U addr, INT8U *data) ;
*
* Purpose :  SCCB read register process, register addr is 16 bits and register data is 8 bits
*
* Parameters : <IN>	id: 				Slave ID
*					addr:				Register Address								
*              <OUT> *data:			Register Data
* Return : 0: Success
*			-1: Fail
*
* Note :
*
*/
extern INT32S sccb_read_Reg16Data8(INT8U id, INT16U addr, INT8U *data); 
/*
* Function Name :  sccb_write_Reg16Data16
*
* Syntax : INT32S sccb_write_Reg16Data16(INT8U id, INT16U addr, INT16U data);
*
* Purpose :  SCCB write register process, register addr is 16 bits and register data is 16 bits
*
* Parameters : <IN>	id: 				Slave ID
*					addr:				Register Address
*					data:				Register Data
*              <OUT> none
* Return : 0: Success
*			-1: Fail
*
* Note :
*/
extern INT32S sccb_write_Reg16Data16(INT8U id, INT16U addr, INT16U data);  
/*
* Function Name :  sccb_read_Reg16Data16
*
* Syntax : INT32S sccb_read_Reg16Data16(INT8U id,	INT16U addr, INT16U *data);
*
* Purpose :  SCCB read register process, register addr is 16 bits and register data is 16 bits
*
* Parameters : <IN>	id: 				Slave ID
*					addr:				Register Address								
*              <OUT> *data:			Register Data
* Return : 0: Success
*			-1: Fail
*
* Note :
*
*/
extern INT32S sccb_read_Reg16Data16(INT8U id,	INT16U addr, INT16U *data); 
 
/*
* Function Name :  CSI_WaitCaptureComplete
*
* Syntax : void CSI_WaitCaptureComplete(void);
*
* Purpose :  Wait capture delay complete
*
* Parameters : <IN>	none
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void CSI_WaitCaptureComplete (void);
/*
* Function Name :  Sensor_Bluescreen_Enable
*
* Syntax : void Sensor_Bluescreen_Enable(void);
*
* Purpose :  Enable bluescreen function
*
* Parameters : <IN>	none
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void Sensor_Bluescreen_Enable(void);
/*
* Function Name :  Sensor_Bluescreen_Disable
*
* Syntax : void Sensor_Bluescreen_Disable(void);
*
* Purpose :  Disable bluescreen function
*
* Parameters : <IN>	none
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void Sensor_Bluescreen_Disable(void); 
/*
* Function Name :  Sensor_Cut_Enable
*
* Syntax : void Sensor_Cut_Enable(void);
*
* Purpose :  Enable cut function
*
* Parameters : <IN>	none
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void Sensor_Cut_Enable(void);
/*
* Function Name :  Sensor_Cut_Disable
*
* Syntax : void Sensor_Cut_Disable(void);
*
* Purpose :  Disable cut function
*
* Parameters : <IN>	none
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void Sensor_Cut_Disable(void);
/*
* Function Name :  Sensor_Black_Enable
*
* Syntax : void Sensor_Black_Enable(void);
*
* Purpose :  Enable shown area selection
*
* Parameters : <IN>	none
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void Sensor_Black_Enable(void);
/*
* Function Name :  Sensor_Black_Disable
*
* Syntax : void Sensor_Black_Disable(void);
*
* Purpose :  Disable shown area selection
*
* Parameters : <IN>	none
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void Sensor_Black_Disable(void); 
/*
* Function Name :  Sensor_EnableFrameEndInterrupt
*
* Syntax : void Sensor_EnableFrameEndInterrupt(BOOLEAN en);
*
* Purpose :  Enable/Disable sensor frame end interrupt
*
* Parameters : <IN>	en: 1:enable 0:disable
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void Sensor_EnableFrameEndInterrupt(BOOLEAN en);

/*
* Function Name :  OID_Sensor_Write_Register
*
* Syntax : void OID_Sensor_Write_Register(INT16U addr,	INT16U data);
*
* Purpose :  OID sensor write register process
*
* Parameters : <IN>	addr: Sensor register address
*					data: Sensro register value
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void OID_Sensor_Write_Register(INT16U addr,	INT16U data);
/*
* Function Name :  OID_Sensor_Read_Register
*
* Syntax : void OID_Sensor_Read_Register(INT16U addr);
*
* Purpose :  OID sensor write register process
*
* Parameters : <IN>	addr: Sensor register address
*              <OUT> none
* Return : INT16U: Sensro register value
*
* Note :
*
*/
extern INT16U OID_Sensor_Read_Register(INT16U addr);
/*
* Function Name :  OID_Sensor_GetChipVersion
*
* Syntax : INT16U OID_Sensor_GetChipVersion(void);
*
* Purpose :  get OID sensor Chip Version
*
* Parameters : <IN>	none
*              <OUT> none
* Return : INT16U: Chip Version
*
* Note :
*
*/
extern INT16U OID_Sensor_GetChipVersion(void);
/*
* Function Name :  OID_Sensor_SetEnablePin
*
* Syntax : void OID_Sensor_SetEnablePin(void);
*
* Purpose :  init OID Sensor enable pin
*
* Parameters : <IN>	none
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void OID_Sensor_SetEnablePin(void);
/*
* Function Name :  OID_Sensor_Enable
*
* Syntax : void OID_Sensor_Enable(BOOLEAN en);
*
* Purpose :  Enable/disable sensor itself
*
* Parameters : <IN>	en 1:enable 0:disable
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void OID_Sensor_Enable(BOOLEAN en);
/*
* Function Name :  OID_Sensor_CSI_Enable
*
* Syntax : void OID_Sensor_CSI_Enable(BOOLEAN en);
*
* Purpose :  Enable/disable CSI interface
*
* Parameters : <IN>	en 0:disable, 1:enable
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void OID_Sensor_CSI_Enable(BOOLEAN en);
/*
* Function Name :  OID_Sensor_CSI_Enable
*
* Syntax : void OID_Sensor_And_CSI_Enable(BOOLEAN en);
*
* Purpose :  Enable/disable both sensor and CSI interface
*
* Parameters : <IN>	en 0:disable, 1:enable
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void OID_Sensor_And_CSI_Enable(BOOLEAN en);
/*
* Function Name :  OID_Sensor_SetResetPin
*
* Syntax : void OID_Sensor_SetResetPin(void);
*
* Purpose :  init OID Sensor reset pin
*
* Parameters : <IN>	none
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void OID_Sensor_SetResetPin(void);
/*
* Function Name :  OID_Sensor_Reset
*
* Syntax : void OID_Sensor_Reset(BOOLEAN is_back_to_normal_state);
*
* Purpose :  Send sensor into reset state and then back to normal state
*
* Parameters : <IN>	is_back_to_normal_state 0:reset pin low, 1:reset pin high
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void OID_Sensor_Reset(BOOLEAN is_back_to_normal_state);
/*
* Function Name :  OID_Sensor_SetFrameBuffB
*
* Syntax : void OID_Sensor_SetFrameBuffB(INT32U buff);
*
* Purpose :  set sensor buff to gFrameBuffB
*
* Parameters : <IN>	buff: sensor buffer
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void OID_Sensor_SetFrameBuffB(INT32U buff);
/*
* Function Name :  OID_Sensor_ChangeFrameBuff
*
* Syntax : void OID_Sensor_ChangeFrameBuff(BOOLEAN is_valid);
*
* Purpose :  set gFrameBuffA/gFrameBuffB to P_CSI_TG_FBSADDR
*
* Parameters : <IN>	is_valid: 1:gFrameBuffA 0:gFrameBuffB
*              <OUT> none
* Return : none
*
* Note :
*
*/
extern void OID_Sensor_ChangeFrameBuff(BOOLEAN is_valid);

#endif	/*__drv_l1_SENSOR_H__*/
