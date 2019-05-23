#include "drv_l1_nand_flash_ext.h"

// nand configuration table
#define _MB 

 /*Total Block Nums*/
#define _1024_BLOCKS  (0x0) /*000b*/                                                                                                                                
#define _2048_BLOCKS  (0x4) /*001b*/                                                                                                                                
#define _4096_BLOCKS  (0x8) /*010b*/                                                                                                                                
#define _8192_BLOCKS  (0xC) /*011b*/ 
#define _16384_BLOCKS (0x10) /*100b*/ 

#define BLOCK_NUM_1024_BLOCKS  1024                                                                                                                              
#define BLOCK_NUM_2048_BLOCKS  2048
#define BLOCK_NUM_4096_BLOCKS  4096                                                                                                                              
#define BLOCK_NUM_8192_BLOCKS  8192

#define _32_PAGES    (0x0) /*00b*/                                                                                                                                
#define _64_PAGES    (0x20) /*01b*/                                                                                                                                
#define _128_PAGES   (0x40) /*10b*/                                                                                                                                
#define _256_PAGES   (0x60) /*11b*/  

 /*Page Size*/
#define _512_BYTES   0x0 /*00b*/                                                                                                                                     
#define _1024_BYTES  0x1 /*01b*/                                                                                                                                     
#define _2048_BYTES  0x2 /*10b*/                                                                                                                                     
#define _4096_BYTES  0x3 /*11b*/ 

 /*Erase Cyccle*/                                                                                                                                                             
#define _ERASE_CYC2  (0x0) /*0*/                                                                                                                               
#define _ERASE_CYC3  (0x80) /*1*/ 

#define BLOCK_SIZE_64_PAGE	   64
#define BLOCK_SIZE_128_PAGE	   128
#define BLCOK_SIZE_256_PAGE	   256

#define PAGE_SIZE_2K	2048
#define PAGE_SIZE_4K	4096
#define PAGE_SIZE_8K	8192

#define PAGE_SPARE_SIZE_32BYTE	32
#define PAGE_SPARE_SIZE_64BYTE	64
#define PAGE_SPARE_SIZE_128BYTE	128
#define PAGE_SPARE_SIZE_192BYTE	192
#define PAGE_SPARE_SIZE_256BYTE	256
#define PAGE_SPARE_SIZE_384BYTE	384

#define ADDRESS_CYCLE_4CYCLE	4
#define ADDRESS_CYCLE_5CYCLE	5
#define ERASE_CYCLE_3CYCLE	3
#define ERASE_CYCLE_2CYCLE	2

typedef struct flash_list_tag
{
   INT8U      id;
   INT16U     total_MB_size;
   INT8U  BlockNums_BlockSize_PageSize;
}  flash_list;

typedef struct flash_8k_list_tag
{
	INT16U	MainID;
	INT32U 	VenDorID;
	INT32U	TotalMB;
	INT32U  BlockNum;
	INT16U	BlockSize;
	INT32U 	PageSize;
	INT16U  PageSpareSize;
	INT8U   AddressCycle;
	INT8U 	EraseCycle;
	BCH_MODE_ENUM	BCHMode; 
} flash_8k_list;			//yunxiangan add for 8k page nand flash list 2010-12-07

flash_list NAND_ID_Table[] =                                                                                                                     
{                                                                                                                                                             
    /*{  MainID,Defalut Size, BlockNums  | BlockSize |PageSize(Byte)| Erase Cycles }*/                                                                                                      
 /*00*/   {0x73,   16 _MB, (_1024_BLOCKS | _32_PAGES |  _512_BYTES  | _ERASE_CYC2)},   //16MB                                                                                                
 /*01*/   {0x58,   16 _MB, (_1024_BLOCKS | _32_PAGES |  _512_BYTES  | _ERASE_CYC2)},   //16MB  // Nand ROM ¤öªí                                                                                               
 /*02*/   {0x74,   16 _MB, (_1024_BLOCKS | _32_PAGES |  _512_BYTES  | _ERASE_CYC2)},   //16MB                                                                                                
 /*03*/   {0x34,   16 _MB, (_1024_BLOCKS | _32_PAGES |  _512_BYTES  | _ERASE_CYC2)},   //16MB                                                                                                
 /*04*/   {0x75,   32 _MB, (_2048_BLOCKS | _32_PAGES |  _512_BYTES  | _ERASE_CYC2)},   //32MB                                                                                                 
 /*05*/   {0x35,   32 _MB, (_2048_BLOCKS | _32_PAGES |  _512_BYTES  | _ERASE_CYC2)},   //32MB                                                                                                                                                                                                 	
 /*06*/   {0x76,   64 _MB, (_4096_BLOCKS | _32_PAGES |  _512_BYTES  | _ERASE_CYC3)},   //64MB                                                                                                
 /*07*/   {0x36,   64 _MB, (_4096_BLOCKS | _32_PAGES |  _512_BYTES  | _ERASE_CYC3)},   //64MB	                                                                                              
 /*08*/   {0x5A,   64 _MB, (_4096_BLOCKS | _32_PAGES |  _512_BYTES  | _ERASE_CYC3)},   //64MB	                                                                                              
 /*19*/   {0x79,  128 _MB, (_8192_BLOCKS | _32_PAGES |  _512_BYTES  | _ERASE_CYC3)},   //128MB                                                                                               
 /*%10*/  {0x39,  128 _MB, (_8192_BLOCKS | _32_PAGES |  _512_BYTES  | _ERASE_CYC3)},   //128MB
 /*%11*/  {0xF1,  128 _MB, (_1024_BLOCKS | _64_PAGES | _2048_BYTES  | _ERASE_CYC2)},   //128MB
 /*12*/   {0xD1,  128 _MB, (_1024_BLOCKS | _64_PAGES | _2048_BYTES  | _ERASE_CYC2)},   //128MB
 /*13*/   {0xD3, 1024 _MB, (_8192_BLOCKS | _64_PAGES | _2048_BYTES  | _ERASE_CYC3)},   //1GB 
 /*14*/   {0xD5, 2048 _MB, (_16384_BLOCKS| _64_PAGES | _2048_BYTES  | _ERASE_CYC3)},   //2GB   // default for toshiba , Old or New Spec                                                                                                   
 /*15*/   {0xCA,  256 _MB, (_2048_BLOCKS | _64_PAGES | _2048_BYTES  | _ERASE_CYC3)},   //256MB
 /*16*/   {0xDA,  256 _MB, (_2048_BLOCKS | _64_PAGES | _2048_BYTES  | _ERASE_CYC3)},   //256MB                                                                                               
 /*17*/   {0x71,  256 _MB, (_2048_BLOCKS | _64_PAGES | _2048_BYTES  | _ERASE_CYC3)},   //256MB                                                                                               
 /*18*/   {0xDC,  512 _MB, (_4096_BLOCKS | _64_PAGES | _2048_BYTES  | _ERASE_CYC3)},   //512MB                                                                                               
 /*19*/   {0xD7, 4096 _MB, (_8192_BLOCKS | _128_PAGES| _4096_BYTES  | _ERASE_CYC3)},   //4GB //Old or New Spec 
 /*20*/   {0xD9, 4096 _MB, (_16384_BLOCKS| _128_PAGES| _4096_BYTES  | _ERASE_CYC3)},   //4GB //Old or New Spec 
 /*%21*/  {0x48, 2048 _MB, (_2048_BLOCKS | _256_PAGES| _4096_BYTES  | _ERASE_CYC3)},   //2GB   //New Spec                                                                                             	
 /*22*/   {0x68, 4096 _MB, (_4096_BLOCKS | _256_PAGES| _4096_BYTES  | _ERASE_CYC3)},   //4GB   //New Spec 
 /*23*/   {0x88, 8192 _MB, (_8192_BLOCKS | _256_PAGES| _4096_BYTES  | _ERASE_CYC3)},   //8GB   //New Spec 
 /*24*/   {0xFF, 1024 _MB, (_2048_BLOCKS | _128_PAGES| _2048_BYTES  | _ERASE_CYC3)}   //NONE                                                                                              
};

flash_8k_list NAND_8k_ID_Table[] =
{
//	  		{MainID,VenDorID,Defalut Size,BlockNums,		 BlockSize,			 PageSize,		PageSpareSize,			AddressCycle,		EraseCycle,			BchMode}
/*SAMSUMG*/	{0xd3ec,0x507284,1024 _MB,BLOCK_NUM_1024_BLOCKS,BLOCK_SIZE_128_PAGE,PAGE_SIZE_8K,PAGE_SPARE_SIZE_384BYTE,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_12_BITS_MODE},	//sansum
	  		{0xd5ec,0x507284,2048 _MB,BLOCK_NUM_2048_BLOCKS,BLOCK_SIZE_128_PAGE,PAGE_SIZE_8K,PAGE_SPARE_SIZE_384BYTE,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_12_BITS_MODE},
	  		{0xd5ec,0x547694,2048 _MB,BLOCK_NUM_2048_BLOCKS,BLOCK_SIZE_128_PAGE,PAGE_SIZE_8K,PAGE_SPARE_SIZE_384BYTE,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_12_BITS_MODE},
/*HYNIX*/	{0xd5ad,0x749a94,2048 _MB,BLOCK_NUM_1024_BLOCKS,BLCOK_SIZE_256_PAGE,PAGE_SIZE_8K,PAGE_SPARE_SIZE_384BYTE,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_12_BITS_MODE},	//H27UAG8T2B
			{0xd7ad,0x749a94,4096 _MB,BLOCK_NUM_2048_BLOCKS,BLCOK_SIZE_256_PAGE,PAGE_SIZE_8K,PAGE_SPARE_SIZE_384BYTE,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_12_BITS_MODE},	//H27UBG8T2A
			{0xDC2C,0x54a690, 512 _MB,BLOCK_NUM_2048_BLOCKS,BLOCK_SIZE_64_PAGE ,PAGE_SIZE_4K,PAGE_SPARE_SIZE_192BYTE,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_12_BITS_MODE},	//
///*MICRON*/	{0x882C,0xA94B04,1024 _MB,BLOCK_NUM_1024_BLOCKS,BLOCK_SIZE_128_PAGE,PAGE_SIZE_8K,PAGE_SPARE_SIZE_384BYTE,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_12_BITS_MODE},	//MT29F128G08CFAAC
/*MICRON*/	{0x882C,0xA94B04,8192 _MB,BLOCK_NUM_4096_BLOCKS,BLCOK_SIZE_256_PAGE,PAGE_SIZE_8K,PAGE_SPARE_SIZE_384BYTE,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_12_BITS_MODE},	//MT29F64G08CBAAA
/*TOSHIBA*/	{0xD398,0xA94B04,1024 _MB,BLOCK_NUM_1024_BLOCKS,BLOCK_SIZE_128_PAGE,PAGE_SIZE_8K,PAGE_SPARE_SIZE_256BYTE,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_8_BITS_MODE},	//TOSHIBA TC58NVG3D2ETA00
			{0xD198,0x761590, 128 _MB,BLOCK_NUM_1024_BLOCKS,BLOCK_SIZE_64_PAGE ,PAGE_SIZE_2K,PAGE_SPARE_SIZE_64BYTE ,ADDRESS_CYCLE_4CYCLE,ERASE_CYCLE_2CYCLE,BCH_8_BITS_MODE},	//TC58NVG0S3ETA00
			{0xDC98,0x762690, 512 _MB,BLOCK_NUM_2048_BLOCKS,BLOCK_SIZE_64_PAGE ,PAGE_SIZE_4K,PAGE_SPARE_SIZE_192BYTE,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_12_BITS_MODE},	//TC58NVG2S0FTA00
			{0xD398,0x762690,1024 _MB,BLOCK_NUM_4096_BLOCKS,BLOCK_SIZE_64_PAGE ,PAGE_SIZE_4K,PAGE_SPARE_SIZE_192BYTE,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_12_BITS_MODE},	//TC58NVG3S0FTA00
//			{0xD598,0x763294,2048 _MB,BLOCK_NUM_2048_BLOCKS,BLOCK_SIZE_128_PAGE,PAGE_SIZE_8K,PAGE_SPARE_SIZE_384BYTE,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_12_BITS_MODE},	//TC58NVG4D2FTA00
/*PHISON*/	{0xd750,0xf43294,4096 _MB,BLOCK_NUM_4096_BLOCKS,BLOCK_SIZE_128_PAGE,PAGE_SIZE_8K,PAGE_SPARE_SIZE_32BYTE ,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_OFF},			//PHISON ECC NAND FLASH
/*End*/    	{0xffff,0xffffff,4096 _MB,BLOCK_NUM_4096_BLOCKS,BLOCK_SIZE_128_PAGE,PAGE_SIZE_8K,PAGE_SPARE_SIZE_32BYTE ,ADDRESS_CYCLE_5CYCLE,ERASE_CYCLE_3CYCLE,BCH_OFF}			//PHISON ECC NAND FLASH			
};

INT8U get_new_spec_ntype_id_start(void)
{
	return 21;
}

INT8U get_tbl_fix_ntype_id_end(void)
{
	return 10;
}

INT8U get_old_spec_ntype_id_start(void)
{
	return 11;
}
//