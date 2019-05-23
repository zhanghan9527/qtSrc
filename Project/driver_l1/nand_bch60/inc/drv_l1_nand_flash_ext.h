#ifndef DRV_L1_NAND_FLASH_EXT_H
#define DRV_L1_NAND_FLASH_EXT_H

/*******************  For Both GPL326XXA and GPL326XXB *************************/
#include "project.h"

/* *****************   Specific for GPL326XXB  *************************************/


#define NAND_ORG_BAD_TAG					0x00
#define NAND_GOOD_TAG						0xff

#define UINT32 INT32U
#define UINT16 INT16U
#define UINT8  INT8U
#define SINT32 INT32S
#define SINT16 INT16S
#define SINT8  INT8S

#define NAND_MLC									0xf1
#define NAND_SLC_LARGE_BLK							0xf2
#define NAND_SLC_SMALL_BLK							0xf3

#ifndef _BCH_MODE_ENUM_
#define _BCH_MODE_ENUM_
typedef enum
{
    BCH1K60_BITS_MODE=0x00, 
    BCH1K40_BITS_MODE=0x01, 
    BCH1K24_BITS_MODE=0x02,  
    BCH1K16_BITS_MODE=0x03,  
    BCH512B8_BITS_MODE=0x04, 
    BCH512B4_BITS_MODE=0x05,
    ECC_1_BIT_MODE=0xFE,
    BCH_OFF=0xFF       
} BCH_MODE_ENUM;
#endif //_BCH_MODE_ENUM_

#ifndef _NAND_STATUS_ENUM_
#define _NAND_STATUS_ENUM_
typedef enum 
{
	NAND_TRUE=0,
	NAND_OK=0,
	NAND_ROM_FIND=1,
	NAND_UNKNOWN=2,
	NAND_FAIL=3,
	NAND_PAD_FAIL=4,
	NAND_DMA_ALIGN_ADDRESS_NEED,
	NAND_DMA_LENGTH_ERROR,
	NAND_BCH_LENGTH_ERROR,
	NAND_PAGE_USED,
	NAND_BLOCK_BREAK,
    NAND_BLOCK_VALID_PAGE_FULL,
	NAND_BIT_ERROR_CORRECTED,
	NAND_PROGRAM_FAIL,
	NAND_ERASE_FAIL,
	NAND_INTER_MOVE_NOT_SAME_PLANE,
	NAND_INTER_MOVE_ADDR_MOD_2_DIFF,
	NAND_NOT_SUPPORT_OVER_BLOCK_PAGE_PROG,
	NAND_TIMEOUT,
	NAND_BCH_ERROR,
	NAND_STATUS_MAX
} NAND_STATUS_ENUM; 
#endif

#ifndef NAND_SHARE_MODE_DEF
#define NAND_SHARE_MODE_DEF
typedef enum
{
    NF_SHARE_PIN_MODE,
    NF_NON_SHARE_MODE
} NAND_SAHRE_MODE_ENUM;
#endif

#ifndef _NFIO_CFG
#define _NFIO_CFG
typedef enum {
    NAND_IO_IOI_IOF   =0x00,  
    NAND_IO_IOB_IOC   =0x01,  
    NAND_IO_IOA_IOC   =0x02,  
    NAND_IO_IOB_IOG   =0x03,   
    NAND_IO_IOA_IOG   =0x04, 
    NAND_IO_IOI_IOG   =0x05,
    NAND_IO_AUTO_SCAN =0xFF
} NFIO_CFG;
#endif
/*
* Function Name :  nand_iopad_sel
*
* Syntax : void nand_iopad_sel(NFIO_CFG nand_io_sel);
*
* Purpose :  to config a group of io pin mux for nand flash controller.
*
* Parameters : <IN> nand_io_sel: it's enumeration type, the values are as following;
*                          	NF_SHARE_PIN_MODE -->nand will be configrated as share mode.
*    			      	NF_NON_SHARE_MODE-->nand will be configrated as non-share mode.
*              <OUT> none
* Return : -None
*        
*		   
*          
* Note :   
*
*/
extern void nand_iopad_sel(NFIO_CFG nand_io_sel);
/*
* Function Name :  nand_share_mode_reg
*
* Syntax : void nand_share_mode_reg(NAND_SAHRE_MODE_ENUM share_or_nonshare);
*
* Purpose :  to config nand flash share mode.
*
* Parameters : <IN> share_or_nonshare: it indicates share mode, the values are as following;
*                          	NAND_IO_IOI_IOF   =0x00,  
*    			      	NAND_IO_IOB_IOC   =0x01,  
*    				NAND_IO_IOA_IOC   =0x02,  
*    				NAND_IO_IOB_IOG   =0x03,   
*				NAND_IO_IOA_IOG   =0x04, 
*				NAND_IO_IOI_IOG   =0x05,
*				NAND_IO_AUTO_SCAN =0xFF
*              <OUT> none
* Return : -None
*        
*		   
*          
* Note :   
*
*/
extern void nand_share_mode_reg(NAND_SAHRE_MODE_ENUM share_or_nonshare);
extern void nf_otp_bch4_flag_set(INT8U nand_otp_bch4_en);
/*
* Function Name :  nand_block_byte_size_get
*
* Syntax : INT32U nand_block_byte_size_get(void);
*
* Purpose :  to get nand flash total bytes.
*
* Parameters : <IN>void
*              	 <OUT> none
* Return : - INT32U type variable, it indicates nand total bytes.
*        
*		   
*          
* Note :   
*
*/
extern INT32U nand_block_byte_size_get(void);
/*
* Function Name :  Nand_Getinfo
*
* Syntax : void Nand_Getinfo(INT16U* PageSize,INT16U* BlkPageNum,INT32U* NandBlockNum,INT32U* NandID);
*
* Purpose :  to get nand flash information, include page size, block size, block numbers, nand id etc.
*
* Parameters : <IN>void
*              	 <OUT> PageSize: nand page size, unit: byte. eg. 2048/4096/8192Byte
*					BlkPageNum: a block contains page numbers, eg. 1Block = 128page, BlkPageNum is 128.
*					NandBlockNum: a nand contains block numbers. eg. 1nand = 4096Block, NandBlockNum is 4096.
*					NandID: indicates nand unique id.
* Return : - None.
*        
*		   
*          
* Note :   
*
*/
extern void Nand_Getinfo(INT16U* PageSize,INT16U* BlkPageNum,INT32U* NandBlockNum,INT32U* NandID);
/*
* Function Name :  good_block_check
*
* Syntax : INT8U good_block_check(INT16U blk_id, INT32U WorkBuffer);
*
* Purpose :  to check nand flash block whether good or not..
*
* Parameters : <IN>blk_id: indicates block numbers to check
*				WorkBuffer: used to store page data that read back.
*              	 <OUT> none
*
* Return : -  0xFF if current block number is good block.
*                 0x00 indicates current block number is original bad block
*		     0x44 indicates current block number is user bad block.
*                 other value indicates read fail.
* Note :   
*
*/
extern INT8U good_block_check(INT16U blk_id, INT32U WorkBuffer);
/*
* Function Name :  NandSetBootHeader
*
* Syntax : SINT32 NandSetBootHeader(UINT8 *Buffer);
*
* Purpose :  to set boot header to nand flash boot area.
*
* Parameters : <IN>Buffer: to store boot header data.
*              	 <OUT> none
*
* Return : -  0x00 no matter boot header set  successfully or not.

* Note :   
*
*/
extern SINT32 NandSetBootHeader(UINT8 *Buffer);
extern INT16U GetNandType(void); // obsolete
extern UINT16 GetAppStartBlkFromBth(void);
extern UINT16 GetAppPercentFromBth(void);
extern UINT16 GetDataBankLogicSizeFromBth(void);
extern UINT16 GetDataBankRecycleSizeFromBth(void);
extern UINT32 GetNoFSAreaSectorSizeFromBth(void);
/*
* Function Name :  GetPartNumFromBth
*
* Syntax : UINT8 GetPartNumFromBth(void);
*
* Purpose :  to get data area partition numbers.
*
* Parameters : <IN>void.
*              	 <OUT> none
*
* Return : -  return data area partition numbers..

* Note :   
*
*/
extern UINT8 GetPartNumFromBth(void);
/*
* Function Name :  GetPartInfoFromBth
*
* Syntax : void GetPartInfoFromBth(UINT8 part,UINT16 *MB_size,UINT8 *attr);
*
* Purpose :  to get data area partition informations from nand boot header..
*
* Parameters : <IN>part: indicates specific partition number to get
*              	 <OUT> MB_size:  to store specific data partition number size. Unit; MB
*					attr: indicates current partition attribute, eg. Readonly, Read/Write, etc.
*
* Return : -  None.

* Note :   
*
*/
extern void GetPartInfoFromBth(UINT8 part,UINT16 *MB_size,UINT8 *attr);
extern INT8U nf_otp_bch4_flag_get(void);
/*
* Function Name :  DrvL1_Nand_Init
*
* Syntax : INT32U DrvL1_Nand_Init(void);
*
* Purpose :  Read ID and initial Nand flash
*
* Parameters : <IN> void
*              <OUT> none
* Return : 
*             - NAND_OK  if successful; 
*             
*
* Note :
*
*/
extern INT32U DrvL1_Nand_Init(void);
/*
* Function Name :  Nand_ReadPhyPage
*
* Syntax : INT32U Nand_ReadPhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer);
*
* Purpose :  read data from nand flash physical page   
*
* Parameters : <IN> wPhysicPageNum: nand pbysical page number to read
*                   WorkBuffer: buffer to store read back data
*              <OUT> none
* Return : NAND_OK if successful; 
*          NAND_BCH_ERROR if bch decode fail
*		   NAND_BIT_ERROR_CORRECTED if it has some bit error
*          NAND_DMA_ALIGN_ADDRESS_NEED if dma address is not aligned.
* Note :
*
*/
extern INT32U Nand_ReadPhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer);
/*
* Function Name :  Nand_WritePhyPage
*
* Syntax : INT32U Nand_WritePhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer);
*
* Purpose :  write data to nand flash physical page   
*
* Parameters : <IN> wPhysicPageNum: nand pbysical page number to write
*                   WorkBuffer: buffer to store data that will be written.
*              <OUT> none
* Return : 0x00 if successful; 
*          0x01 if write data to page fail
*		   
*          
* Note :   
*
*/
extern INT32U Nand_WritePhyPage(INT32U wPhysicPageNum, INT32U WorkBuffer);
/*
* Function Name :  Nand_ErasePhyBlock
*
* Syntax : INT32U Nand_ErasePhyBlock(INT32U wPhysicBlkNum);
*
* Purpose :  erase nand flash physical blk    
*
* Parameters : <IN> wPhysicPageNum: nand pbysical page number to erase
*                  
*              <OUT> none
* Return : 0x00 if successful; 
*          0x01 if erase physical block fail
*		   
*          
* Note :   
*
*/ 
extern INT32U Nand_ErasePhyBlock(INT32U wPhysicBlkNum);

extern void nand_set_external_wp_enable(INT8U enable, void ExNand_WP_Initial(void), void (*ExNand_WP_Disable)(void), void ExNand_WP_Enable(void));

extern INT8U nand_get_external_wp_enable(void);

#endif //DRV_L1_NAND_FLASH_EXT_H