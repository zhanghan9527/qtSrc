/*
* Purpose: Nand Flash Read/Write modulers of USBD(MSDC)
*
* Author: wsf
*
* Date: 2008/10/28
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
*/

#include "storages.h"
#include "drv_l2_nand_ext.h"
#include "drv_l1_cache.h"

#if (defined NAND_APP_EN) && (NAND_APP_EN == 1)
//================================================================//

#define USE_NAND_NUM	1
static INT8U real_part_id_map[USE_NAND_NUM]={1};

static INT32U nand_lba[USE_NAND_NUM];
static INT32U nand_seccount[USE_NAND_NUM];
static INT16U nandAppPartTotal;
static INT32U nandAppPartTotalSector;
static NF_APP_PART_INFO nandAppPartInfo[USE_NAND_NUM];
static INT8U nandAppMsdcInited = 0;
static INT8U part_id = 0;
static INT32U BufferPtr;

static INT32S Check_APP_Init_Status(void);
INT32S unand_app_read_cmd_phase(INT32U lba,INT32U seccount);
INT32S unand_app_read_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount);
INT32S unand_app_read_cmdend_phase(void);
INT32S unand_app_write_cmd_phase(INT32U lba,INT32U seccount);
INT32S unand_app_write_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount);
INT32S unand_app_write_cmdend_phase(void);

INT32S unand_app_read_cmd_phase(INT32U lba,INT32U seccount)
{
  	INT32S sts;
 
 	//DBG_PRINT("unand_app_read_cmd_phase:lba%d seccount%d\r\n", lba, seccount);
 	
	nand_lba[part_id] = lba;
	nand_seccount[part_id] = seccount;
	
	sts = Check_APP_Init_Status();
	
	if ((nand_lba[part_id] + seccount) > nandAppPartInfo[part_id].part_size)
	{
		DBG_PRINT(" ~0 read start LBA[%d] len[%d] over. left[%d] \r\n",nand_lba[part_id], seccount, nand_seccount[part_id]);
		sts = 2;
	}
			
	return sts;
}

INT32S unand_app_read_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount)
{
 	INT32S sts =1;

	//DBG_PRINT("unand_app_read_dma_phase:buf%08x ifwait%d seccount%d\r\n", (INT32U)buf, ifwait, seccount);
	
	if (ifwait == 0)
	{
		BufferPtr = (INT32U)buf;
		sts = 0;
	}
	if (ifwait == 1 || ifwait == 2)
	{		
		if (ifwait == 2)
			buf = (void *)BufferPtr;
			
		if((seccount != 0) && (nand_seccount[part_id] >= seccount))
		{
			if ((nand_lba[part_id] + seccount) > nandAppPartInfo[part_id].part_size)
			{
				DBG_PRINT(" ~1 read start LBA[%d] len[%d] over. left[%d] \r\n",nand_lba[part_id], seccount, nand_seccount[part_id]);
				sts = 2;
				return sts;
			}
					
			sts = NandBootReadSector(nand_lba[part_id] + nandAppPartInfo[part_id].startSector, seccount , (INT32U)buf);
	    	if(sts)
	    	{
				DBG_PRINT(" ~2 read start LBA[%d] len[%d] failed \r\n",nand_lba[part_id] + nandAppPartInfo[part_id].startSector,seccount);
			}
			#if _DRV_L1_CACHE        
        	cache_drain_range((INT32U) buf, (INT32U) (seccount<<9)); // drain things nand driver ever touch by cpu
        	cache_invalid_range((INT32U)buf, (INT32U)(seccount<<9)); // inval so that usb always get from dram
			#endif
			nand_lba[part_id]  += seccount;
	    	nand_seccount[part_id] -= seccount;	
		}
		else
		{
			DBG_PRINT(" ~3 read start LBA[%d] len[%d] over. left[%d] \r\n",nand_lba[part_id] + nandAppPartInfo[part_id].startSector, seccount, nand_seccount[part_id]);
		}		
	}
	
 	return sts;
}

INT32S unand_app_read_cmdend_phase(void)
{
	INT32S sts =0;

	//DBG_PRINT("unand_app_read_cmdend_phase\r\n");
	
 	return sts;
}

INT32S unand_app_write_cmd_phase(INT32U lba,INT32U seccount)
{
  	INT32S sts;
 
 	DBG_PRINT("unand_app_write_cmd_phase:lba%d seccount%d\r\n", lba, seccount);
 	
	nand_lba[part_id] = lba;
	nand_seccount[part_id] = seccount;
	
	sts = Check_APP_Init_Status();

	if ((nand_lba[part_id] + seccount) > nandAppPartInfo[part_id].part_size)
	{
		DBG_PRINT(" ~0 write start LBA[%d] len[%d] over. left[%d] \r\n",nand_lba[part_id], seccount, nand_seccount[part_id]);
		sts = 2;
	}		
  	return sts;
}

INT32S unand_app_write_dma_phase(INT32U *buf,INT8U ifwait,INT32U seccount)
{
	INT32S sts =1 ;

	DBG_PRINT("unand_app_write_dma_phase:buf%08x ifwait%d seccount%d\r\n", (INT32U)buf, ifwait, seccount);
	
	if (ifwait == 0)
	{
		BufferPtr = (INT32U)buf;
		sts = 0;
	}
	if (ifwait == 1 || ifwait == 2)
	{
		if (ifwait == 2)
			buf = (void *)BufferPtr;
			
		if((seccount!= 0) && (nand_seccount[part_id]>=seccount))
		{
			if ((nand_lba[part_id] + seccount) > nandAppPartInfo[part_id].part_size)
			{
				DBG_PRINT(" ~1 write start LBA[%d] len[%d] over. left[%d] \r\n",nand_lba[part_id], seccount, nand_seccount[part_id]);
				sts = 2;
				return sts;
			}
			
			#if _DRV_L1_CACHE        
        	cache_drain_range((INT32U) buf, (INT32U) (seccount<<9)); // if the buffer being touch by cpu, drain it
        	cache_invalid_range((INT32U)buf, (INT32U)(seccount<<9)); // inval so that nand driver always get from dram,
        														 // when usbd BOT_DMA_RAM_BUF_ORDER is 0, this inval must apply
			#endif
			NandBootEnableWrite();
			sts = NandBootWriteSector(nand_lba[part_id] + nandAppPartInfo[part_id].startSector, seccount , (INT32U)buf);
			NandBootDisableWrite();
			if(sts)
			{
				DBG_PRINT(" ~2 write start LBA[%d] len[%d] failed \r\n",nand_lba[part_id] + nandAppPartInfo[part_id].startSector,seccount);
			}	
	    	nand_lba[part_id]  += seccount;
	    	nand_seccount[part_id] -= seccount;		
		}	
		else
		{
			DBG_PRINT(" ~3 write start LBA[%d] len[%d] over. left[%d] \r\n",nand_lba[part_id] + nandAppPartInfo[part_id].startSector,seccount, nand_seccount[part_id]);
		}	
	}

 	return sts;
}

INT32S unand_app_write_cmdend_phase(void)
{
	INT32S sts =0;
 
 	DBG_PRINT("unand_app_write_cmdend_phase\r\n");
 	
 	return sts;
}

static INT32S drv_l2_msc_nand_init(	void* priv)
{
	INT32S ret;
	
	DBG_PRINT(" ~init~ \r\n");
	ret = NandBootInit();
	if (ret != 0)
		return ret;
	ret = NandAppInitParts(&nandAppPartTotal, &nandAppPartTotalSector);
	
	return ret;
}

static INT32S Check_APP_Init_Status(void)
{
 	INT32S ret = 0;
 	
 	if (!nandAppMsdcInited)
 	{
 		ret = drv_l2_msc_nand_init(NULL);
 		if (ret == 0)
 		{
 			ret = NandAppGetPartInfo(real_part_id_map[part_id], &nandAppPartInfo[part_id]);
 			if (ret == 0)
 				nandAppMsdcInited = 1;
 		}
 	}
  	
  	if (ret != 0)
  		DBG_PRINT("Check_APP_Init_Status fail %d\r\n", ret);
  		
  return ret;
}

struct Usb_Storage_Access USBD_NF_APP_ACCESS = {
	unand_app_read_cmd_phase,
	unand_app_read_dma_phase,
	unand_app_read_cmdend_phase,
	#if (defined NAND_APP_WRITE_EN) && (NAND_APP_WRITE_EN == 1)
	unand_app_write_cmd_phase,
	unand_app_write_dma_phase,
	unand_app_write_cmdend_phase,
	#else
	NULL, NULL, NULL,
	#endif
};

#endif //(defined NAND1_EN) && (NAND1_EN == 1)