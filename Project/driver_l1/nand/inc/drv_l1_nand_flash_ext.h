#ifndef _DRV_L1_NAND_FLASH_EXT_H_
#define _DRV_L1_NAND_FLASH_EXT_H_
#include "project.h"
#ifndef GPL32_GP10
#define GPL32_GP10	0x10
#endif

#define APP_PART_TAG		0x5a5a5a5a
#define NAND_FLASH_GP6_TAG  "PGandnandn"
#define NAND_FLASH_GP10_TAG "GPNandTag2"
#define NAND_ROM_GP10_TAG   "GPNandTag2R"
#define MAX_GOOD_HEADER_BLKS            20
#define NAND_HEADER_MAX_BLOCKS          MAX_GOOD_HEADER_BLKS
#define HEADER_OFFSET_NAND_ROM          10
#define HEADER_OFFSET_BCH_ENABLE        36
#define HEADER_OFFSET_BCH_BIT           37
#define HEADER_OFFSET_ND_DEFINE         18
#define HEADER_OFFSET_ND_PAGE           20
#define HEADER_OFFSET_ND_ADDR_CYC       21
#define HEADER_OFFSET_ND_BLOCK          22
#define HEADER_DENSIZTY_CODE_PAGES      23
#define HEADER_GP6_DENSIZTY_CODE_PAGES  10
#define HEADER_OFFSET_ND_HASH_KEY       26  /*2 Bytes*/

#define HEADER_OFFSET_NEXT_GOOD_BLK_CNT  	24
#define HEADER_OFFSET_BOOTLOADSEC_CNT       16
#define HEADER_OFFSET_BCHPAGE_CNT      		18
#define HEADER_OFFSET_BOOT_BACKUP_NUM  		38
#define HEADER_OFFSET_NF_START_RUN_ADDR     40

//#define HEADER_OFFSET_SEC_CNT       16
#define HEADER_OFFSET_SEC_CNT       8


#define NAND_READ_ALLOW	    0x1
#define NAND_WRITE_ALLOW	0x2

#define HEADER_OFFSET_APP_FLAG 					0x1c0	/*4 bytes*/
#define HEADER_OFFSET_APP_START 				0x1c4	/*1 bytes*/
#define HEADER_OFFSET_APP_SIZE  				0x1c5	/*2 bytes*/
#define HEADER_OFFSET_APP_PRENT 				0x1c7	/*1 bytes*/


#define HEADER_OFFSET_USER_SETTING_SIZE_BLK		0x1d0	/*4 bytes*/
#define HEADER_OFFSET_PARTITION_NUM				0x1d7	/*1 bytes*/ 	//0,1,2
#define HEADER_OFFSET_NAND_PART0_SIZE_BLK   	0x1d8   /*4 bytes*/		//size = 0:auto	
#define HEADER_OFFSET_NAND_PART1_SIZE_BLK		0x1dc   /*4 bytes*/		//size = 0:auto
#define HEADER_OFFSET_NAND_PART0_ATTRIBUTE   	0x1e3   /*1 bytes*/		//1,2,3
#define HEADER_OFFSET_NAND_PART1_ATTRIBUTE   	0x1e7   /*1 bytes*/		//1,2,3

#define HEADER_BANK_BLOCKS   					0x1e8	/*4 bytes*/	
#define HEADER_RECYCLE_BLOCKS					0x1ec	/*4 bytes*/	

#define HEADER_NAND_MB_SIZE                     0x1f8   /*4 bytes*/
#define HEADER_MAX_MCLK_MHz                     0x1fe   /*1 byte*/
#define HEADER_OFFSET_WP_PIN_ID                 0x1ff   /*1 byte*/

#define GPL6_BOOTAREA_BCH_SET_PAGE_NUMS		4	
#define GPL6_BOOTAREA_BCH_CHG_CYCLE		    (GPL6_BOOTAREA_BCH_SET_PAGE_NUMS*2)
// This is pseudo header for extern to driver_l1.h 

#ifndef PHYSICAL_NANDINFP_STRUCT
#define PHYSICAL_NANDINFP_STRUCT
typedef struct NandInfo_Str
{
INT32U    wNandID;
INT16U    wNandType;
INT16U    wAddrCycle;
INT32U    wNandBlockNum;
INT16U    wPageSize;
INT16U    wPageSectorSize;
INT16U    wPage_with_spare_size;
INT16U    wBlkPageNum;
}Physical_NandInfo;
#endif


#ifndef BCH_MODE_ENUM_DEF
#define BCH_MODE_ENUM_DEF
typedef enum
{
    BCH_4_BITS_MODE=0x0,
    BCH_8_BITS_MODE=0x2,
    BCH_12_BITS_MODE=0x800,
    BCH_OFF=0xFF,
    BCH_AUTO_MODE=0x1000,
    BCH_MODE_MAX
} BCH_MODE_ENUM;
#endif

#ifndef NAND_WP_ENUM_DEF
#define NAND_WP_ENUM_DEF
typedef enum {
    NAND_WP_ENABLE=0,
    NAND_WP_DISABLE=1,
    NAND_WP_NON_DEFINE
}NAND_WP_ENUM;
#endif


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

typedef enum {
    NF_PROGRAM_DENY =0x00,  /*Write Protect*/
    NF_PROGRAM_ERR7 =0x01,  /*Write Protect*/
    NF_PROGRAM_ERR8 =0x02,  /*Write Protect*/
    NF_PROGRAM_ERR9 =0x03,  /*Write Protect*/  
    NF_PROGRAM_ERR10=0x04,  /*Write Protect*/ 
    NF_PROGRAM_ERR11=0x05,  /*Write Protect*/
    NF_PROGRAM_ERR12=0x06,  /*Write Protect*/  
    NF_PROGRAM_ERR13=0x07,  /*Write Protect*/
    NF_PROGRAM_WELL =0x80,  /*Program Well*/
    NF_PROGRAM_ERR0 =0x81,  /*Program Error*/
    NF_PROGRAM_ERR1 =0x82,  /*Program Error1*/
    NF_PROGRAM_ERR2 =0x83,  /*Program Error2*/
    NF_PROGRAM_ERR3 =0x84,  /*Program Error3*/  
    NF_PROGRAM_ERR4 =0x85,  /*Program Error4*/
    NF_PROGRAM_ERR5 =0x86,  /*Program Error5*/
    NF_PROGRAM_ERR6 =0x87   /*Program Error6*/
} NFCMD_STATUS;

#ifndef NAND_CS_ENUM_DEF
#define NAND_CS_ENUM_DEF
typedef enum
{
    NAND_CS1=0x2,   /* IOF1 */
    NAND_CS2=0x4,   /* IOF2 */
    NAND_CS3=0x8    /* IOF3 */
} NAND_CS_ENUM;
#endif


#ifndef NAND_SHARE_MODE_DEF
#define NAND_SHARE_MODE_DEF
typedef enum
{
    NF_SHARE_PIN_MODE,
    NF_NON_SHARE_MODE
} NAND_SAHRE_MODE_ENUM;
#endif

#ifndef _TAG_ENUM
#define _TAG_ENUM
typedef enum {
    TAG_GPL325XX=6,
    TAG_GPL326XX=10
} TAG_ENUM;
#endif
//extern INT8U nf_8k_flag;
//extern INT8U nf_ecc_nand_flag;
//extern INT16U gNand_spare_size;
extern Physical_NandInfo gPhysical_NandInfo;
/*
* Function Name :  NandReset
*
* Syntax : INT32U NandReset(void);
*
* Purpose :  to reset nand flash
*
* Parameters : <IN> void
*                  
*              <OUT> none
* Return : NAND_OK if successful; 
*          NAND_TIMEOUT if reset fail
*		   
*          
* Note :   
*
*/
extern INT32U NandReset(void);
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
extern   void nand_flash_bchtable_alloc(INT16S *alpha_to ,INT16U *index_of);
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
extern void nand_wp_pin_reg(INT8U gpio_pin_number);
extern void DrvNand_WP_Enable(void);
extern void DrvNand_WP_Disable(void);
extern NAND_WP_ENUM DrvNand_WP_Status_get(void);
extern INT32U nand_total_MByte_size_get(void);
extern INT32U Nand_blk_size_get(void);
extern INT32U nand_sector_nums_per_page_get(void);
extern INT32U nand_page_nums_per_block_get(void);
extern void nand_protect_block_nums_set(INT32U nand_protect_nums);
extern void nand_small_page_force_set(void);
extern INT16U nand_page_size_get(void);
extern INT32U nand_total_MByte_size_get(void);
extern void nand_total_MByte_size_set(INT32U nand_MB_size);
extern BCH_MODE_ENUM bch_mode_get(void);
extern void bch_mode_set(BCH_MODE_ENUM bch_set);
extern void nand_addr_cycle_set(INT16U addr_cycle);
extern void Nand_blk_number_set(INT16U u16BlkNum); 
extern void nand_page_nums_per_block_set(INT16U blk_page_size);
extern void bch_mode_set(BCH_MODE_ENUM bch_set);
extern void DrvNand_WP_Initial(void);
extern void nand_set_hw_bch(BCH_MODE_ENUM type);
extern void nand_page_size_set(INT16U page_size);
extern INT32U Nand_blk_number_get(void); 

extern void nf_8k_en(INT8U nand_8k_en);
extern INT8U nf_8k_get(void);
extern void nf_ecc_nand_en(INT8U ecc_nand_en);
extern INT8U nf_ecc_nand_get(void);
extern void nf_spare_size_set(INT16U Nand_spare_size);
extern INT16U nf_spare_size_get(void);

extern INT16U nand_bch_err_bits_get(void);
extern NFCMD_STATUS nand_write_status_get(void);
/*
* Function Name :  gpl32_version_get
*
* Syntax : INT8U gpl32_version_get(void);
*
* Purpose :  to get IC version  
*
* Parameters : <IN> void
*              <OUT> none
* Return : IC version if successful; 
*			   0x01: indicates GPL325XX Ver.A
*			   0x02: indicates GPL325XX Ver.B
*			   0x03: indicates GPL325XX Ver.C
*			   0x10: indicates GPL32600 Ver.S
*			   0x11: indicates GPL32600 Ver.B
*              0xFF if it fails to match the ic version.
*              
*
* Note :
*
*/
extern INT8U gpl32_version_get(void);
#endif

