#ifndef _NAND_CONFIG_H_
#define _NAND_CONFIG_H_  
#include "project.h"
#include "driver_l2.h"
#include "drv_l2_nand_ext.h"



#define DRV_L2_NAND_DBG     0
#if DRV_L2_NAND_DBG==1
	#define NF_DBG_PRINT	DBG_PRINT
#else
	#define NF_DBG_PRINT     nf_null_printf
#endif



/*######################################################
#												 		#
#	   Nand Flash Blocks Assignation Configuration		#
#														#
 ######################################################*/
#define APP_AREA_START			11	/* Start Physical Block Address */
#define APP_AREA_SIZE			0x40 	/* APP Area Size|Block numbers. */
#define APP_AREA_SPARE_START	(APP_AREA_START + APP_AREA_SIZE)
#define APP_AREA_SPARE_PERCENT  4
#define	APP_AREA_SPARE_SIZE		(APP_AREA_SIZE/APP_AREA_SPARE_PERCENT)	/* Spare area size */
#define DATA_AREA_START			(APP_AREA_SPARE_START + APP_AREA_SPARE_SIZE)

/*######################################################
#												 		#
#	    Nand Flash Spec/Type Configuration	        	#
#														#
 ######################################################*/
#define SUPPORT_ALL_PAGE
//#define SUPPORT_2KB_512B_PAGE
//#define SUPPORT_512B_PAGE

#if defined SUPPORT_ALL_PAGE
	#define PAGE_SIZE	 	8192
	#define SPARE_SIZE	 	436
#elif defined SUPPORT_2KB_512B_PAGE	
	#define PAGE_SIZE	 	2048
	#define SPARE_SIZE	 	64
#elif defined SUPPORT_512B_PAGE
	#define PAGE_SIZE	 	512
	#define SPARE_SIZE	 	16
#else
	" Error: Nandflash Config ERROR!! "
#endif

#define BANK_NUM	30

/*######################################################
#												 		#
#	   Nand Flash Driver Bank&Recycle Configuration	    #
#														#
 ######################################################*/ 
#define SP_BANK_LOGIC_SIZE	 200 	// Small Page
#define SP_BANK_RECYCLE_SIZE 20 	// Small Page
#define LP_BANK_LOGIC_SIZE	 512 	// Large Page
#define LP_BANK_RECYCLE_SIZE 64 	// Large Page

#if (PAGE_SIZE==512)
	#define BANK_LOGIC_SIZE		SP_BANK_LOGIC_SIZE
	#define BANK_RECYCLE_SIZE 	SP_BANK_RECYCLE_SIZE
#else  /*default condition is Large page*/
	#define BANK_LOGIC_SIZE		LP_BANK_LOGIC_SIZE
	#define BANK_RECYCLE_SIZE 	LP_BANK_RECYCLE_SIZE
#endif

/*######################	Warning		############################
#																	#
#		Don`t modify the content below,otherwise it will			#
# 	be cause driver can`t work normally or can`t work optimumly!	#
#																	#
###################################################################*/
#ifdef NAND_16BIT
	#define BYTE_WORD_SHIFT		1		// 16 platform
#else
	#define BYTE_WORD_SHIFT		0		// 32 platform
#endif

#ifndef _BANK_INFO
#define _BANK_INFO
typedef struct
{	
	INT16U	wLogicBlkNum;
	INT16U	wRecycleBlkNum;
	INT16U	wMapTableBlk;
	INT16U	wMapTablePage;
	INT16U  wOrgBlkNum;
	INT16U	wUserBadBlkNum;
	INT16U	wExchangeBlkNum;
	INT16U  wStatus;
}BANK_INFO;
#endif


typedef struct
{
	INT16U	Logic2Physic[BANK_LOGIC_SIZE];
	INT16U	RecycleBlk[BANK_RECYCLE_SIZE];
}IMM;

typedef union 
{
	IMM 	Maptable;	
	INT16U  pMaptable[BANK_LOGIC_SIZE+BANK_RECYCLE_SIZE];
}MM;

typedef struct
{
	INT16U	wPageData[PAGE_SIZE>>1]; // 为了兼容16bit complier
	INT8U	wBad_Flag_0;
	INT8U	wCount;
	INT16U	wLogicNum;
	INT8U	wExchangeType;
	INT8U	wBad_Flag_6;
	INT16U  wP2P;	
}WB;

typedef union
{
	WB PageTag;
	INT16U WorkBuffer[(PAGE_SIZE+SPARE_SIZE)>>1]; // 为了兼容16bit complier	
}TT;

typedef struct 	
{
	INT16U	wLogicBlk;
	INT16U	wPhysicBlk;
}L2P;

#ifndef _NF_USER_INFO
#define _NF_USER_INFO
typedef struct 
{
	INT16U 	uiPageSize;
	INT16U	uiPageSpareSize;	
	INT16U	uiBankNum;
	INT16U	uiBankSize;
	INT16U	uiBankRecSize;	
	INT16U	uiMM_Num;
	INT16U	uiAppStart;
	INT16U 	uiAppSize;
	INT16U 	uiAppSpareStart;
	INT16U  uiAppSparePercent;
	INT16U 	uiAppSpareSize;
	INT16U 	uiDataStart;
	INT16U	uiDirReadSupport;
} NF_USER_INFO;
#endif

typedef struct
{
	INT32U BCH_BLK_NUM;
	INT32U BCH_BADBLK_NUM;
	INT32U BCH_BADPAGE_NUM;
	
	INT32U BOOTLOAD_NUM;
	INT32U BOOTLOAD_BAD_NUM;
}UsrBootArea;

extern 	TT   APP_tt;
extern 	ALIGN16 L2P  gL2PTable[0x200];//size=0x200, so ram size= 0x200*sizeof(L2P)=2048<=page size//gL2PTable[APP_AREA_SPARE_SIZE];

#if _OPERATING_SYSTEM != _OS_NONE
extern  OS_EVENT * gNand_sem;
extern  OS_EVENT * gNand_PHY_sem;
#endif
extern INT32U Nand_Part0_Offset;
extern INT32U Nand_Part0_Size;
extern INT16U Nand_Part0_Mode;
extern INT32U Nand_Part1_Offset;
extern INT32U Nand_Part1_Size;
extern INT16U Nand_Part1_Mode;
extern INT32U Nand_Part2_Offset;
extern INT32U Nand_Part2_Size;
extern INT16U Nand_Part2_Mode; 


extern void Nand_OS_LOCK(void);
extern void Nand_OS_UNLOCK(void);
/*
* Function Name :  NandInfoAutoGet
*
* Syntax : INT32S NandInfoAutoGet(void);
*
* Purpose :  to read nand info from bootheader and set nand parameters to nand.
*
* Parameters : <IN> void
*                   
*              <OUT> none
* Return :     STATUS_OK if successful; 
*              -1 if set parameters to nand fail.
*
* Note :
*
*/
extern INT32S NandInfoAutoGet(void);
extern void nand_part0_para_set(INT32U offset, INT32U size, INT16U mode);
extern void nand_part1_para_set(INT32U offset, INT32U size, INT16U mode);
extern void nand_part2_para_set(INT32U offset, INT32U size, INT16U mode);
extern INT32U NandAppByteSizeGet(void);
extern INT32U NandBootAreaByteSizeGet(void);
extern INT32U Nand_part0_Offset_Get(void);
extern INT32U Nand_part0_mode_Get(void);
extern INT32U Nand_part0_size_Get(void);
extern INT16U GetBadFlagFromNand(INT16U wPhysicBlkNum);
extern INT32U DrvNand_bchtable_alloc(void);
extern NF_USER_INFO gNandUsrInfo;
extern void SetWorkBuffer(TT* p_tt_basic, TT* p_tt_extend);
extern INT32U CalculateFATArea(void);
extern INT32U NandBootFormat_UP(INT16U format_type, INT16U format_step);
extern void nf_null_printf(CHAR *fmt, ...);
extern INT32U ib6_smart_para_get(INT16U *smart_page_size, INT16U *smart_blk_size);

extern void SetBitErrCntAsBadBlk(INT16U bit_cnt);
extern INT16U GetBitErrCntAsBadBlk(void);
extern INT8U nand_header_max_sys_clk_get(void);

extern void   Nand_Partition_Num_Set(INT16U number);
extern INT16U Nand_Partition_Num_Get(void);
extern INT32U NandBootAreaInfoGet(UsrBootArea *info);

extern INT32U combin_reg_data(INT8U *data, INT32S len);

#endif
