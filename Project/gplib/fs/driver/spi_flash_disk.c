/*
* Purpose: SPI flash interface of File system 
*
* Author:
*
* Date: 2009/6/15
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
*
* History:
        0  V1.00 :
*/

#define CREAT_DRIVERLAYER_STRUCT

#include "fsystem.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if NOR_EN == 1                                                   //
//================================================================//

extern INT32S SPI_Flash_write_nByte(INT32U addr, INT8U *buf, INT16U BufCnt);
extern INT32S spi_disk_memcpy(INT8S* destBuffer, INT8S* srcBuffer, INT32U byteLen);
extern INT32S spi_flash_backup_write_4kPage(INT32U byteAddr, INT8U* pBuffer);


INT32U spi_flash_part0_offset = 0;
INT32U spi_flash_size = 0x8000; //Should be set by USER, unit: sector   32Mbit = 0x2000 sector

#if _OPERATING_SYSTEM != _OS_NONE
OS_EVENT *gSPIF_sem = 0;
#endif

static void SPIF_SEM_INIT(void)
{
#if _OPERATING_SYSTEM != _OS_NONE
	if(gSPIF_sem == 0)
	{
		gSPIF_sem = OSSemCreate(1);
	}
#endif
}

static void SPIF_LOCK(void)
{
#if _OPERATING_SYSTEM != _OS_NONE
	INT8U err = NULL;

	OSSemPend(gSPIF_sem, 0, &err);
#endif
}

static void SPIF_UNLOCK(void)
{
#if _OPERATING_SYSTEM != _OS_NONE
    OSSemPost(gSPIF_sem);
#endif
}

#define SPI_FLASH_TAG       	"PGpssiipps"
#define SPI_HDR_TAG_ID			(0)
#define SPI_HDR_TAG_SIZE		(10)  //Bytes
#define SPI_HDR_TAG_ID			(0)
#define SPI_BTLDR_SIZE_ID		(16)
#define SPI_APP_HDR_TAG			"GP"
#define SPI_APP_HDR_TAG_ID		(0)
#define SPI_APP_HDR_TAG_SIZE	(2) //Bytes
#define SPI_MAX_FILENAME_LEN	(19)
#define SPI_FILE_OFFSET_LEN		(4)
#define SPI_FILE_INFO_LEN		(SPI_MAX_FILENAME_LEN + SPI_FILE_OFFSET_LEN)
#define SPI_APP_HDR_SIZE		(512) //Bytes
#define SPI_RESERVE_ZONE_NAME	"RESERVEBLOCKBIN"
#define SPI_PAIRPAGE_TAG		"***** SPI BackUp PairPage *****"
#define SPI_PAGE_BIT_NUM		(12)//4KBytes
#define SPI_PAGE_MASK			(0xFFFUL)
#define SPI_SECTPR_BIT_NUM		(9)//512
#define SPI_BLOCK_BIT_NUM       (16)//64KBytes
#define SPI_PAGE_SIZE			(4096)

//////////////////////////////////////////////////////////////////////////////////////////
//Restore original block if power-down during updating
//First  4K: Tag | Target PageAddress
//Second 4K: The data needed be restored

INT32U spi_backup_start_page = 0;
INT32U spi_backup_pairpage_num = 0;
INT32U spi_backup_pairpage_index = 0;
INT8U* spi_backup_buffer = NULL;
INT8U  spi_backup_tag_len = 0;
INT32U spi_addr_backedup = 0;

#define SPI_BUFFER_EMPTY	(5)
#define SPI_BUFFER_CLEAN	(6)
#define SPI_BUFFER_DIRTY	(7)
#define SPI_INVALID_ADDR	(0xFFFFFFFFUL)

typedef struct 
{
    INT32U Flag;             
    INT32U targetByteAddr;   //the block number
    INT8U* pBuffer;
}SPI_DISK_BUFFER_STRUCT;

SPI_DISK_BUFFER_STRUCT spi_disk_buffer = {SPI_BUFFER_EMPTY, SPI_INVALID_ADDR, NULL};

INT32S spi_flash_write_4kPage(INT32U byteAddr, INT8U* pBuffer)
{
	INT32U index;
	
	SPI_Flash_erase_sector(byteAddr);
	
	for(index = 0; index < 16; index++)
	{
		SPI_Flash_write_page(byteAddr, pBuffer);
		byteAddr += 256;
		pBuffer += 256;
	}
	
	return 0;
}

INT32S erase_backup_pairpage(void)
{
	INT32U destAddr = (spi_backup_start_page + spi_backup_pairpage_index * 2) << SPI_PAGE_BIT_NUM;
	
	SPI_Flash_erase_sector(destAddr);
	SPI_Flash_erase_sector(destAddr + SPI_PAGE_SIZE);
	
	spi_backup_pairpage_index++;
	
	if(spi_backup_pairpage_index >= spi_backup_pairpage_num)
	{
		spi_backup_pairpage_index = 0;
	}
	
	return 0;
}

INT32S write_data_to_backup_pairpage(INT32U relatedPageByteAddr)
{
	INT32S retval = 0;
	INT32U destAddr = 0;
	INT32U* pWord = 0;
	
	destAddr = (spi_backup_start_page + spi_backup_pairpage_index * 2) << SPI_PAGE_BIT_NUM;
	
	//Backup First, then write the tag
	SPI_Flash_read(relatedPageByteAddr, spi_backup_buffer, SPI_PAGE_SIZE);
	spi_flash_write_4kPage(destAddr + SPI_PAGE_SIZE, spi_backup_buffer);
	
	
	//Prepare Tag and address info
	gp_memcpy((INT8S *)spi_backup_buffer, (INT8S *)SPI_PAIRPAGE_TAG, spi_backup_tag_len);
	pWord = (INT32U*)(spi_backup_buffer + spi_backup_tag_len);
	*pWord = relatedPageByteAddr;
	
	SPI_Flash_write_nByte(destAddr, spi_backup_buffer, spi_backup_tag_len + 4);
	
	return retval;
}

INT32S restore_data_from_backup_page(void)
{
	INT32S retval = 0;
	INT32U srcAddr = (spi_backup_start_page + spi_backup_pairpage_index * 2) << SPI_PAGE_BIT_NUM;
	
	SPI_Flash_erase_sector(spi_addr_backedup);
	
	SPI_Flash_read(srcAddr + SPI_PAGE_SIZE, spi_backup_buffer, SPI_PAGE_SIZE);
	
	spi_flash_write_4kPage(spi_addr_backedup, spi_backup_buffer);
	
	SPI_Flash_erase_sector(srcAddr);
	SPI_Flash_erase_sector(srcAddr + SPI_PAGE_SIZE);
	
	return retval;
}

INT32S init_spi_backup_pairpages(void)
{
	INT32S retval = 0;
	INT32U pairpageIndex = 0;
	INT32U byteAddr = 0;
	
	spi_backup_tag_len = sizeof(SPI_PAIRPAGE_TAG);
	
	for(pairpageIndex = 0; pairpageIndex < spi_backup_pairpage_num; pairpageIndex++)
	{
		byteAddr = (spi_backup_start_page + pairpageIndex * 2) << SPI_PAGE_BIT_NUM;
		
		retval = SPI_Flash_read(byteAddr, spi_backup_buffer, spi_backup_tag_len + 4);
		if(retval != 0)
		{
			return -1;
		}
		
		if(gp_strncmp((INT8S *)SPI_PAIRPAGE_TAG, (INT8S *)spi_backup_buffer, spi_backup_tag_len) == 0)
		{//founded backup page, restore it
			spi_addr_backedup = combin_reg_data(&spi_backup_buffer[spi_backup_tag_len], 4);
			spi_backup_pairpage_index = pairpageIndex;
			
			retval = restore_data_from_backup_page();
			if(retval != 0)
			{
				return -2;
			}
		}
		else
		{
			SPI_Flash_erase_sector(byteAddr);
			SPI_Flash_erase_sector(byteAddr + SPI_PAGE_SIZE);
		}
	}
	
	spi_backup_pairpage_index = 0;
	
	return retval;
}
///////////////////////////////////////////////////////////////////////////////////////////

INT32S calc_spi_reserve_zone_byte_offset(INT8U* pAppHeader, INT32U* p_reserve_offset, INT32U* p_reserve_size)
{
	INT32U fileIndex = 0;
	INT32U fileOffsetIndex = 0;
	INT32U len = 0;
	
	INT32U reserve_offset = 0;
	INT32U reserve_size = 0;
	
	len = gp_strlen((INT8S*)(SPI_RESERVE_ZONE_NAME));
	fileIndex = SPI_FILE_OFFSET_LEN;
	fileOffsetIndex = fileIndex + SPI_MAX_FILENAME_LEN;
	
	while(1)
	{
		if((fileOffsetIndex + SPI_FILE_INFO_LEN) > SPI_APP_HDR_SIZE)
		{
			DBG_PRINT("Parse SPI App Header Failed\r\n");
			return -1;
		}
		
		if(gp_strncmp((INT8S *)SPI_RESERVE_ZONE_NAME, (INT8S*)(&pAppHeader[fileIndex]), len) == 0)
		{
			reserve_offset = combin_reg_data(&pAppHeader[fileOffsetIndex], SPI_FILE_OFFSET_LEN) * 512;
			
			if(pAppHeader[fileIndex + SPI_FILE_INFO_LEN] == 0xFF) //The reserved zone is the last partition in APP
			{
				reserve_size = combin_reg_data(&pAppHeader[fileIndex + SPI_FILE_INFO_LEN + SPI_MAX_FILENAME_LEN], SPI_FILE_OFFSET_LEN)* 512;
			
				if(reserve_size & 0xffffUL){
					reserve_size = ((reserve_size >> SPI_BLOCK_BIT_NUM) + 1) << SPI_BLOCK_BIT_NUM;
				}
				
				reserve_size -= reserve_offset;
			}
			else
			{
				reserve_size  = (combin_reg_data(&pAppHeader[fileOffsetIndex + SPI_FILE_INFO_LEN], SPI_FILE_OFFSET_LEN)*512) - (*p_reserve_offset);
			}
			
			*p_reserve_offset = reserve_offset;
			*p_reserve_size   = reserve_size;
			
			return 0;
		}
			
		fileIndex += SPI_FILE_INFO_LEN;
		fileOffsetIndex += SPI_FILE_INFO_LEN;
	}
	
	return -2;
}

INT32S calc_spi_data_zone_byte_offset(INT8U* pAppHeader, INT32U* p_data_start_offset)
{
	INT32U fileIndex = 0;
	INT32U fileOffsetIndex = 0;
	INT32U data_start = 0;
	
	fileIndex = SPI_FILE_OFFSET_LEN;
	
	while(1)
	{
		if((fileOffsetIndex + SPI_FILE_INFO_LEN) > SPI_APP_HDR_SIZE)
		{
			DBG_PRINT("Parse SPI App Header Failed\r\n");
			return -1;
		}
		
		
		if(pAppHeader[fileIndex] == 0xFF) //End file mark
		{
			data_start = combin_reg_data(&pAppHeader[fileIndex + SPI_MAX_FILENAME_LEN], SPI_FILE_OFFSET_LEN)* 512;
			
			if(data_start & 0xffffUL){
				data_start = ((data_start >> SPI_BLOCK_BIT_NUM) + 1) << SPI_BLOCK_BIT_NUM;
			}
			
			*p_data_start_offset = data_start;
			
			return 0;
		}
			
		fileIndex += SPI_FILE_INFO_LEN;
	}
	
	return -2;
}

//Got configure value for spi_flash_part0_offset, spi_backup_start_page, spi_backup_pairpage_num
INT32S parse_spi_header(void)
{
	INT32S retval = 0;
	INT8U* pBuffer = NULL;
	INT32U app_start_offset = 0;
	
	pBuffer = (INT8U*)gp_malloc_align(SPI_APP_HDR_SIZE, 16);
	if(pBuffer == (INT8U*)NULL)
	{
		DBG_PRINT("spi malloc buffer failed\r\n");
		return -1;
	}
	
	SPI_Flash_read(0, pBuffer, SPI_APP_HDR_SIZE >> 2);
	if(gp_strncmp((INT8S *)SPI_FLASH_TAG, (INT8S *)&pBuffer[SPI_HDR_TAG_ID], SPI_HDR_TAG_SIZE) != 0)
	{
		DBG_PRINT("Read SPI Header Failed\r\n");
		retval = -2;
    	goto _end_parse_spi_header;
	}
	
	app_start_offset = combin_reg_data(&pBuffer[SPI_BTLDR_SIZE_ID], 2) << 9;
	
	retval = SPI_Flash_read(app_start_offset, pBuffer, SPI_APP_HDR_SIZE);
    if(retval != STATUS_OK)
    {
    	retval = -3;
    	goto _end_parse_spi_header;
    }
     
    if(gp_strncmp((INT8S *)SPI_APP_HDR_TAG, (INT8S*)(&pBuffer[SPI_APP_HDR_TAG_ID]), SPI_APP_HDR_TAG_SIZE) != 0)
	{
		DBG_PRINT("Read SPI App Header Failed\r\n");
		retval = -4;
    	goto _end_parse_spi_header;
	}
	
	retval = calc_spi_data_zone_byte_offset(pBuffer, &spi_flash_part0_offset);
	if(retval != 0)
	{
		retval = -5;
    	goto _end_parse_spi_header;
	}
	
	spi_flash_part0_offset += app_start_offset;
	spi_flash_part0_offset >>= 9; //Covert to sector unit
	
	retval = calc_spi_reserve_zone_byte_offset(pBuffer, &spi_backup_start_page, &spi_backup_pairpage_num);
	if(retval != 0)
	{
		retval = -6;
    	goto _end_parse_spi_header;
	}
	
	spi_backup_start_page += app_start_offset;
	spi_backup_pairpage_num += spi_backup_start_page;
	
	if(spi_backup_start_page & 0xFFFUL) //Not 4KBytes Aligned
	{
		spi_backup_start_page = (spi_backup_start_page >> SPI_PAGE_BIT_NUM) + 1;
	}
	
	spi_backup_pairpage_num = spi_backup_pairpage_num >> SPI_PAGE_BIT_NUM;
	spi_backup_pairpage_num = (spi_backup_pairpage_num - spi_backup_start_page) >> 1;
	
	if(spi_backup_pairpage_num < 1)
	{
		DBG_PRINT("RESERVEBLOCK.BIN in SPI APP is too small (%d)\r\n", spi_backup_pairpage_num);
		retval = -7;
		goto _end_parse_spi_header;
	} 
	
_end_parse_spi_header:
	
	if(pBuffer != NULL)
	{
		gp_free(pBuffer);
		pBuffer = NULL;
	}
	
	DBG_PRINT("SPI %d Data Zone Start Sector: 0x%x\r\n", retval, spi_flash_part0_offset);
	DBG_PRINT("SPI %d Reserve Zone Start Page: 0x%x, Size %d pairpages\r\n", retval, spi_backup_start_page, spi_backup_pairpage_num);
	return retval;
}

INT32S drv_spiflashdisk_read_sector(INT32U sectorAddr, INT32U sectorNum, INT32U buf)
{
	INT32U startByteAddr = 0;
	INT32U readByteLen = 0;
	INT8U* pBuffer = (INT8U*)buf;
	INT32U curReadLen = 0;
	INT32S retval = 0;
	INT32U curPageByteStartAddr = 0;
	
	SPIF_LOCK();
	
	if( (sectorAddr >= spi_flash_size) || ((sectorAddr + sectorNum) > spi_flash_size) )
	{
		DBG_PRINT("SPI Read Overrage (0x%x + 0x%x > 0x%x)\r\n", sectorAddr, sectorNum, spi_flash_size);
		retval = -1;
		goto _end_spi_disk_read;
	}
	
	startByteAddr = sectorAddr << 9;
	readByteLen = sectorNum << 9;
	
	while(readByteLen)
	{
		curPageByteStartAddr = (startByteAddr >> SPI_PAGE_BIT_NUM) << SPI_PAGE_BIT_NUM;
		curReadLen = (curPageByteStartAddr + SPI_PAGE_SIZE) - startByteAddr;
		curReadLen = (readByteLen > curReadLen)? curReadLen : readByteLen;
		
		if((spi_disk_buffer.Flag != SPI_BUFFER_EMPTY) && (spi_disk_buffer.targetByteAddr == curPageByteStartAddr))
		{
			spi_disk_memcpy((INT8S*)pBuffer, (INT8S*)(spi_disk_buffer.pBuffer + (startByteAddr&SPI_PAGE_MASK)), curReadLen);
		}
		else
		{
			SPI_Flash_read(startByteAddr, pBuffer, curReadLen);
		}
		
		startByteAddr += curReadLen;
		pBuffer += curReadLen;
		readByteLen -= curReadLen;
	}
	
#if 0	
	{
		INT32U* pWord = (INT32U*)buf;
		INT32U i;
		
		DBG_PRINT("Read Sector 0x%x, 0x%x\r\n", sectorAddr, sectorNum);
		DBG_PRINT("(0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x)\r\n",
			pWord[0],pWord[1],pWord[2],pWord[3],pWord[4],pWord[5],pWord[6],pWord[7],pWord[8],pWord[9]);
		
	}
#endif

_end_spi_disk_read:

	SPIF_UNLOCK();
	
	return retval;
}

INT32S spi_flash_disk_read_sector(INT32U sectorAddr, INT32U sectorNum, INT32U buf)
{
	INT32S retval = 0;
	
	retval = drv_spiflashdisk_read_sector(sectorAddr + spi_flash_part0_offset, sectorNum, buf);
	
	return retval;
}

INT32S spi_disk_memcpy(INT8S* destBuffer, INT8S* srcBuffer, INT32U byteLen)
{
	DMA_STRUCT str_dma;
	INT32S retval = 0;
	
	str_dma.s_addr = (INT32U)srcBuffer;
	str_dma.t_addr = (INT32U)destBuffer;
	str_dma.count =  byteLen;
	str_dma.width = 1;
	str_dma.timeout = 0;
	str_dma.notify = NULL;
	retval = dma_transfer_wait_ready(&str_dma);
	if(retval != 0)
	{
		gp_memcpy(destBuffer, srcBuffer, byteLen);
	}
		
	return 0;
}
	 
INT32S spi_flash_disk_flush_no_sem(void)
{
	INT32S retval = 0;
	
	if(spi_disk_buffer.Flag == SPI_BUFFER_DIRTY)
	{//flush
		retval = spi_flash_backup_write_4kPage(spi_disk_buffer.targetByteAddr, spi_disk_buffer.pBuffer);
		spi_disk_buffer.Flag = SPI_BUFFER_CLEAN;
	}
	
	return retval;
}
 
INT32S write_data_into_spidisk_buffer(INT32U relatedPageAddr, INT8U* pBuffer, INT32U offsetInPage, INT32U byteLen)
{
	INT32U relatedByteAddr = relatedPageAddr << SPI_PAGE_BIT_NUM;
	
	if(spi_disk_buffer.Flag != SPI_BUFFER_EMPTY)
	{//need flush the spidisk buffer
		if(spi_disk_buffer.targetByteAddr != relatedByteAddr)
		{
			spi_flash_disk_flush_no_sem();
			SPI_Flash_read(relatedByteAddr, spi_disk_buffer.pBuffer, SPI_PAGE_SIZE);
			spi_disk_buffer.targetByteAddr = relatedByteAddr;
		}
	}
	else
	{
		SPI_Flash_read(relatedByteAddr, spi_disk_buffer.pBuffer, SPI_PAGE_SIZE);
		spi_disk_buffer.targetByteAddr = relatedByteAddr;
	}
	
	spi_disk_memcpy((INT8S*)(spi_disk_buffer.pBuffer + offsetInPage), (INT8S*)pBuffer, byteLen);
	spi_disk_buffer.Flag = SPI_BUFFER_DIRTY;
	
	return 0;
}

INT32S spi_flash_backup_write_4kPage(INT32U byteAddr, INT8U* pBuffer)
{
	INT32S retval = 0;
	
	retval = write_data_to_backup_pairpage(byteAddr);
	if(retval != 0)
	{
		return -1;
	}
	
	retval = spi_flash_write_4kPage(byteAddr, pBuffer);
	if(retval != 0)
	{
		return -2;
	}
	
	retval = erase_backup_pairpage();
	if(retval != 0)
	{
		return -3;
	}
			
	return 0;
}

INT32S drv_spiflashdisk_write_sector(INT32U sectorAddr, INT32U sectorNum, INT32U buf)
{
	INT32U startByteAddr = 0;
	INT32U writeByteLen = 0;
	INT32U offsetInPage = 0;
	INT8U* pBuffer = (INT8U*)buf;
	INT32U curWriteLen = 0;
	INT32S retval = 0;
	
	SPIF_LOCK();
	
	if( (sectorAddr >= spi_flash_size) || ((sectorAddr + sectorNum) > spi_flash_size) )
	{
		DBG_PRINT("SPI Write Overrage (0x%x + 0x%x > 0x%x)\r\n", sectorAddr, sectorNum, spi_flash_size);
		retval = -1;
		goto _end_spi_disk_write;
	}
	
	#if 0
	{
		INT32U* pWord = (INT32U*)buf;
		INT32U i;
		
		DBG_PRINT("Write Sector 0x%x, 0x%x\r\n", sectorAddr, sectorNum);
		DBG_PRINT("(0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x)\r\n",
			pWord[0],pWord[1],pWord[2],pWord[3],pWord[4],pWord[5],pWord[6],pWord[7],pWord[8],pWord[9]);
		
	}
	#endif
	
	startByteAddr = sectorAddr << 9;
	writeByteLen = sectorNum << 9;
	
	if(startByteAddr & SPI_PAGE_MASK)
	{//Start Address not page aligned
	    offsetInPage = startByteAddr & SPI_PAGE_MASK;
	    curWriteLen = SPI_PAGE_SIZE - offsetInPage;
	    curWriteLen = (writeByteLen > curWriteLen)? curWriteLen : writeByteLen;
	    
		retval = write_data_into_spidisk_buffer(startByteAddr >> SPI_PAGE_BIT_NUM, pBuffer, offsetInPage, curWriteLen);
		if(retval != 0)
		{
			goto _end_spi_disk_write;
		}
		
		writeByteLen -= curWriteLen;
		pBuffer += curWriteLen;
		startByteAddr += curWriteLen;
	}
	
	while(writeByteLen)
	{
		curWriteLen = (writeByteLen > SPI_PAGE_SIZE)? SPI_PAGE_SIZE : writeByteLen;
		
		if(curWriteLen != SPI_PAGE_SIZE) //last data transfer
		{
			retval = write_data_into_spidisk_buffer(startByteAddr >> SPI_PAGE_BIT_NUM, pBuffer, 0, curWriteLen);
			if(retval != 0)
			{
				goto _end_spi_disk_write;
			}
		}
		else
		{
			retval = spi_flash_backup_write_4kPage(startByteAddr, pBuffer);
			if(retval != 0)
			{
				goto _end_spi_disk_write;
			}
		}
		
		writeByteLen -= curWriteLen;
		pBuffer += curWriteLen;
		startByteAddr += curWriteLen;
	}
	
_end_spi_disk_write:

	SPIF_UNLOCK();
	
	return retval;
}

INT32S spi_flash_disk_write_sector(INT32U sectorAddr, INT32U sectorNum, INT32U buf)
{
	INT32S retval = 0;
	
	retval = drv_spiflashdisk_write_sector(sectorAddr + spi_flash_part0_offset, sectorNum, buf);
	
	return retval;
}

INT32S spi_flash_disk_flush(void)
{
	INT32S retval = 0;
	
	SPIF_LOCK();
	
	retval = spi_flash_disk_flush_no_sem();
	
	SPIF_UNLOCK();
	
	return retval;
}
   
INT32U get_spi_disk_offset(void)
{
	return spi_flash_part0_offset;
}
 
INT32S spi_flash_disk_initial(void)
{
	INT32S retval = STATUS_OK;
	INT8U mainID;
	INT8U spiId[3];
	static INT32U spi_init_flag = 0;

	SPIF_SEM_INIT();
	SPIF_LOCK();

	if(spi_init_flag != 0)
	{
		retval = STATUS_OK;
		goto _end_spi_disk_initial;
	}

	retval = SPI_Flash_init();
	if(retval != STATUS_OK)
	{
		goto _end_spi_disk_initial;
	}

	spi_clk_set(0, SYSCLK_128); /* spi clock = 48M/8/128 = 46.8k */
	SPI_Flash_readID(spiId);

	DBG_PRINT("SPI ID: 0x%02x, 0x%02x, 0x%02x\r\n", spiId[0], spiId[1], spiId[2]);
	mainID = spiId[0];
	if (mainID == 0xd2) {
	}
	else if (mainID == 0xc2) {
		spi_clk_set(0,SYSCLK_4);
	}
	else{
		spi_clk_set(0,SYSCLK_4);
	}
     
	retval = parse_spi_header();
	if(retval != 0)
	{
		retval = STATUS_FAIL;
		goto _end_spi_disk_initial;
	}

	spi_backup_buffer = gp_malloc_align(SPI_PAGE_SIZE, 16);
	if(spi_backup_buffer == NULL)
	{
		retval = STATUS_FAIL;
		goto _end_spi_disk_initial;
	}

	retval = init_spi_backup_pairpages();
	if(retval != 0)
	{
		retval = STATUS_FAIL;
		goto _end_spi_disk_initial;
	}
     
	spi_disk_buffer.Flag = SPI_BUFFER_EMPTY;
	spi_disk_buffer.targetByteAddr = SPI_INVALID_ADDR;
	spi_disk_buffer.pBuffer = gp_malloc_align(SPI_PAGE_SIZE, 16);
	if(spi_disk_buffer.pBuffer == NULL)
	{
		retval = STATUS_FAIL;
		goto _end_spi_disk_initial;
	}

	spi_init_flag = 1;
    
_end_spi_disk_initial:

	if(retval != 0)
	{
		if(spi_backup_buffer != NULL)
		{
			gp_free(spi_backup_buffer);
			spi_backup_buffer = NULL;
		}

		if(spi_disk_buffer.pBuffer != NULL)
		{
			gp_free(spi_disk_buffer.pBuffer);
			spi_disk_buffer.pBuffer = NULL;
		}
	}

	SPIF_UNLOCK();

	return retval;
}

INT32S spi_flash_disk_uninitial(void)
{
	return 0;
}

void spi_flash_disk_get_drv_info(struct DrvInfo *info)
{
    info->nSectors = spi_flash_size - spi_flash_part0_offset;//C_SPIFDISK_SIZE / 512;//0x8000;
    info->nBytesPerSector = 512;
    
    DBG_PRINT("SPI Disk Size: %d sectors\r\n", info->nSectors);
}

////////////////////////////// Test SPI /////////////////////////////////////////
#if 0

#define  BUFFER_SIZE  (4096)
 
ALIGN32 INT8U writeBuffer[BUFFER_SIZE];
ALIGN32 INT8U readBuffer[BUFFER_SIZE];

INT32U testDumpBuffer(INT8U* pBuffer, INT32U size)
{
	INT32U i = 0;
	
	for(i = 0; i < size; i++)
	{
		DBG_PRINT("0x%02x ", pBuffer[i]);
		if((i+1) % 16 == 0)
		{
			DBG_PRINT("\r\n");
		}
	}
	
	if(i % 16 != 0)
	{
		DBG_PRINT("\r\n");
	}
	
	return 0;
}

INT32U testSetSerialData(INT32U* pWordBuffer, INT32U wordLen, INT32U startValue)
{
	INT32U i = 0;
	
	for(i = 0; i < wordLen; i++)
	{
		pWordBuffer[i] = startValue + i;
	}
	
	return 0;
}

INT32U testSetSerialByte(INT8U* pBuffer, INT32U len, INT8U startValue)
{
	INT32U i = 0;
	
	for(i = 0; i < len; i++)
	{
		pBuffer[i] = (startValue + i)&0xFF;
	}
	
	return 0;
}
 
INT32U testMemcpy(INT8U* srcAddr, INT8U* destAddr, INT32U len)
{
	INT32U i = 0;
	
	for(i = 0; i < len; i++)
	{
		destAddr[i] = srcAddr[i];
	}
	
	return 0;
}

INT32U testMemset(INT8U* addr, INT8U value, INT32U len)
{
	INT32U i = 0;
	
	for(i = 0; i < len; i++)
	{
		addr[i] = value;
	}
	
	return 0;
}

INT32U testMemcmp(INT8U* addrA, INT8U* addrB, INT32U len)
{
	INT32U i = 0;
	
	for(i = 0; i < len; i++)
	{
		if(addrA[i] != addrB[i])
		{
			DBG_PRINT("Error %d, 0x%x != 0x%x\r\n", i, addrA[i], addrB[i]);
			return 1;
		}
	}
	
	return 0;
}

void temp_spi_test(void)
{	
	INT32S retval = 0;
	INT32U startSector = 0;
	INT32U curSecotrLen = 0;
	INT32U sectorLen= 0;
	INT32U index = 0;
	
	DBG_PRINT("Begin spi test\r\n");
	
	retval = spi_flash_disk_initial();
	if(retval != 0)
	{
		DBG_PRINT("Init SPI Failed %d\r\n", retval);
		while(1);
	}
	
	sectorLen = spi_flash_size - spi_flash_part0_offset;
	
	curSecotrLen = BUFFER_SIZE >> 9;
	sectorLen /= curSecotrLen;
	
	DBG_PRINT("Write SectorLen %d: 0x%x (0x%x - 0x%x)\r\n", curSecotrLen, sectorLen, spi_flash_size, spi_flash_part0_offset);
	for(index = 0; index < sectorLen; index++)
	{
		DBG_PRINT("w");
		testSetSerialData((INT32U*)writeBuffer, BUFFER_SIZE>>2, index);
		retval = spi_flash_disk_write_sector(startSector + index*curSecotrLen, curSecotrLen, (INT32U)writeBuffer);
		if(retval != 0)
		{
			DBG_PRINT("Write SPI Failed %d(0x%x)\r\n", retval, startSector + index);
			while(1);
		}
	} 
	
	for(index = 0; index < sectorLen; index++)
	{
		DBG_PRINT("r");
		
		testSetSerialData((INT32U*)writeBuffer, BUFFER_SIZE>>2, index);
		testMemset(readBuffer, 0x00, BUFFER_SIZE);
		
		retval = spi_flash_disk_read_sector(startSector + index*curSecotrLen, curSecotrLen, (INT32U)readBuffer);
		if(retval != 0)
		{
			DBG_PRINT("Read SPI Failed %d(0x%x)\r\n", retval, startSector + index);
			while(1);
		}
		
		retval = testMemcmp(writeBuffer, readBuffer, BUFFER_SIZE);
		if(retval != 0)
		{
			DBG_PRINT("Compare SPI Failed %d\r\n", retval);
			while(1);
		}
	}
	
	DBG_PRINT("Tset spi successful\r\n");

	while(1);
}

#endif
////////////////////////////// End Test SPI /////////////////////////////////////


struct Drv_FileSystem FS_SPI_FLASH_DISK_driver = {
    "SPINOR" ,
    DEVICE_READ_ALLOW|DEVICE_WRITE_ALLOW ,
    spi_flash_disk_initial ,
    spi_flash_disk_uninitial ,
    spi_flash_disk_get_drv_info ,
    spi_flash_disk_read_sector ,
    spi_flash_disk_write_sector ,
    spi_flash_disk_flush ,
};

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //((NOR_EN == 1)                                           //
//================================================================//