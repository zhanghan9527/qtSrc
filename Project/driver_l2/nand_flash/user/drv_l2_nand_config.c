#include "drv_l2_nand_config.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L2_NAND) && (_DRV_L2_NAND == 1)                 //
//================================================================//


static INT32U nand_fs_blk_number = 0;
static INT32U nand_fs_sector_number = 0;

void GetNandInfo(NF_USER_INFO *user_info);
void DrvNand_WP_Initial(void);
void DrvNand_WP_Enable(void);
void DrvNand_WP_Disable(void);
void SetNandInfo(INT16U app_start,INT16U app_size,INT16U app_Percent,  INT16U data_BankSize, INT16U data_BankRecSize);
INT32S NandInfoAutoGet(void);
INT32U combin_reg_data2(INT8U *data,INT32S len);
INT16U Nand_iData_Start_Addr_get(void);
static INT32U nand_block0_AppTag_get(INT8U * header_data);
INT32U NandBootAreaByteSizeGet(void);
INT32U NandAppByteSizeGet(void);
INT32U Nand_part0_Offset_Get(void);
INT32U Nand_part0_mode_Get(void);
TAG_ENUM nand_header_read(INT8U* data_buf);
INT32U nand_block_checkbad(INT32U block_id, INT8U* data_buf);
void bch_polling_exchange(void);
void Nand_part0_Offset_Set(INT32U nand_fs_sector_offset);
void Nand_part0_mode_Set(INT16U mode);
void Nand_part0_size_Set(INT32U nand_fs_sector_size);
void Nand_part1_Offset_Set(INT32U nand_fs_sector_offset);
void Nand_part1_mode_Set(INT16U mode);
void Nand_part1_size_Set(INT32U nand_fs_sector_size);
INT32S NandHeaderInfoSet(INT8U* header_data);
INT8U nand_header_max_sys_clk_get(void);
extern INT32U ib10_smart_para_get(INT16U *smart_page_size, INT16U *smart_blk_size);
/*######################	Warning		############################
#																	#
#		Don`t modify the content below,otherwise it will			#
# 	be cause driver can`t work normally or can`t work optimumly!	#	
#																	#
###################################################################*/

ALIGN8 TT				tt_basic;	// Must be defined
ALIGN8 TT				tt_extend;	// Option
ALIGN16 TT      		APP_tt;
ALIGN16 BANK_INFO  		gBankInfo[BANK_NUM];

ALIGN16 L2P   			gL2PTable[0x200];		// Ч秶妏蚚湮size


INT32U Nand_Part0_Offset=0;
INT32U Nand_Part0_Size=0;
INT16U Nand_Part0_Mode=0;

INT32U Nand_Part1_Offset=0;
INT32U Nand_Part1_Size=0;
INT16U Nand_Part1_Mode=0;

INT32U Nand_Part2_Offset=0;
INT32U Nand_Part2_Size=0;
INT16U Nand_Part2_Mode=0;

INT16U Nand_Partition_NUM=0;

//INT32U Nand_Reserve_Offset = NAND_RESERVE_OFFSET;

#if (defined _PROJ_BOOTLOADER)&&(_PROJ_TYPE ==_PROJ_BOOTLOADER)
//for nandboot loader Null 
#else
	OS_EVENT * gNand_sem = NULL;
	OS_EVENT * gNand_PHY_sem = NULL;
#endif

INT16U     		gMallocBCHTable = 0;
INT8U 			dwNFConfig_flag = 0;

INT32U nand_l2_app_start_page_id=0;
INT32U nand_l2_app_size = 0;
INT32U nand_l2_bootarea_size = 0;
INT32U HeaderMaxMCLK=0;

void 	DrvNand_bchtable_free(void);
INT32U 	DrvNand_bchtable_alloc(void);
void nand_part0_para_set(INT32U offset, INT32U size, INT16U mode);
void nand_part1_para_set(INT32U offset, INT32U size, INT16U mode);
void nand_part2_para_set(INT32U offset, INT32U size, INT16U mode);
INT8U nand_tag_match_querry(INT8U *page_buf);
INT8S nand_option_table_detect(INT8U* head_data);

void   Nand_Partition_Num_Set(INT16U number);
INT16U Nand_Partition_Num_Get(void);

INT32U NandAppByteSizeGet(void);
void nf_null_printf(CHAR *fmt, ...);
extern INT8U gpl32_version_get(void);

INT32S Nand_OS_Init(void)
{
	#if (defined _PROJ_BOOTLOADER)&&(_PROJ_TYPE ==_PROJ_BOOTLOADER)
	//for nandboot loader Null 
	#else
	if (OSRunning==TRUE)
	{
		if(NULL == gNand_sem)
		{
	    	gNand_sem = OSSemCreate(1);	
		}

	}
	#endif	
    return (0);
}
INT32S Nand_OS_Exit(void)
{
	return 0;
}

void Nand_OS_LOCK(void)
{
	#if (defined _PROJ_BOOTLOADER)&&(_PROJ_TYPE ==_PROJ_BOOTLOADER)
	//for nandboot loader Null 
	#else
	if (OSRunning==TRUE)
	{
		INT8U err = NULL;	
		OSSemPend(gNand_sem , 0 , &err);
	}
	#endif
}
void Nand_OS_UNLOCK(void)
{
	#if (defined _PROJ_BOOTLOADER)&&(_PROJ_TYPE ==_PROJ_BOOTLOADER)
	//for nandboot loader Null 
	#else
	if (OSRunning==TRUE)
	{
		OSSemPost(gNand_sem);
	}
	#endif
}
/////////////////////////////////
void Sys_Exception(INT32S errorcode)
{
	switch(errorcode)
	{
		default:
			errorcode++;
		break;
	}
	return;				
}	

/*
* Function Name :  GetNandInfo
*
* Syntax : void GetNandInfo(NF_USER_INFO *user_info);
*
* Purpose :  to get the configuration of Nand flash
*
* Parameters : <IN> none
*
*              <OUT> user_info:Nand flash configuration information.
* Return : None
*
* Note :
*
*/
void GetNandInfo(NF_USER_INFO *user_info)
{	
	//user_info->ulSelectID		= CHIP_ID;
	//user_info->uiNandType		= NF_TYPE;
	//user_info->uiBlkNum		= BLK_NUM;
	//user_info->uiPageNum		= BLK_PAGE_NUM;
	user_info->uiPageSize		= PAGE_SIZE;
	user_info->uiPageSpareSize	= SPARE_SIZE;	
	user_info->uiBankNum		= BANK_NUM;
	user_info->uiMM_Num			= 1;
	
	if(dwNFConfig_flag != 0x01)
	{
		user_info->uiAppStart		 = APP_AREA_START;
		user_info->uiAppSize		 = APP_AREA_SIZE;
		user_info->uiAppSpareStart	 = APP_AREA_SPARE_START;
		user_info->uiAppSparePercent = APP_AREA_SPARE_PERCENT;
		user_info->uiAppSpareSize	 = APP_AREA_SPARE_SIZE;
		user_info->uiDataStart		 = DATA_AREA_START;
    	user_info->uiBankSize		 = BANK_LOGIC_SIZE;		//200;
    	user_info->uiBankRecSize	 = BANK_RECYCLE_SIZE;	//20;
	}
	//user_info->uiDirReadSupport	= DIRECT_READ;

	SetWorkBuffer( &tt_basic, &tt_extend);
}
/*
* Function Name :  SetNandInfo
*
* Syntax : void SetNandInfo(INT16U app_start,INT16U app_size,INT16U app_Percent,  INT16U data_BankSize, INT16U data_BankRecSize);
*
* Purpose :  to set the configuration of App area(App start/App size/App recovery area size, etc.
*
* Parameters : <IN> app_startㄩStart address of App area
*                   app_sizeㄩSize of App area.
*                   app_PercentㄩSize of recovery area compared to app_size
*                                4 means size of recovery area will be 1/4 * app_size
*
*              <OUT> none
* Return : None
*
* Note :
*
*/
void SetNandInfo(INT16U app_start,INT16U app_size,INT16U app_Percent,  INT16U data_BankSize, INT16U data_BankRecSize)
{

    nand_protect_block_nums_set(app_start);

	gNandUsrInfo.uiAppStart 	   = app_start;
	
	gNandUsrInfo.uiAppSize  	   = app_size;

    if (app_Percent==0) {app_Percent=APP_AREA_SPARE_PERCENT;} // default value, to avoid div zero

	gNandUsrInfo.uiAppSparePercent = app_Percent;
	
	gNandUsrInfo.uiAppSpareStart   = app_start + app_size;
	
	gNandUsrInfo.uiAppSpareSize    = (app_size/app_Percent);
    if (((app_size%app_Percent)!=0) || (gNandUsrInfo.uiAppSpareSize < 2)) {
        gNandUsrInfo.uiAppSpareSize += 1;
    }
    
	gNandUsrInfo.uiDataStart       = gNandUsrInfo.uiAppSpareStart + gNandUsrInfo.uiAppSpareSize;
	
	gNandUsrInfo.uiBankSize        = data_BankSize;

	gNandUsrInfo.uiBankRecSize     = data_BankRecSize;
	
	dwNFConfig_flag = 0x01;
}
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
INT32S NandInfoAutoGet(void)
{
    INT32S ret=STATUS_FAIL;
    BCH_MODE_ENUM  backup_bch_mode;
    INT16U ib6_page_size;
    INT16U ib6_blk_pages;
    INT16U backup_page_size;
	INT8U  backup_nf_8k_flag;	//add 2010-12-08
	INT8U  backup_nf_ecc_nand_flag;	//add 2011-01-27
	
	backup_nf_8k_flag = nf_8k_get();
	backup_nf_ecc_nand_flag = nf_ecc_nand_get();
    backup_bch_mode = bch_mode_get(); 
    backup_page_size = nand_page_size_get();
	
    if(gpl32_version_get() < GPL32_GP10) {
        // gp6 header 只認得 BCH4
        ib6_smart_para_get(&ib6_page_size, &ib6_blk_pages);
        nand_page_size_set(ib6_page_size);
        if(0 == nf_rom_flag_get())
        {
        	bch_mode_set(BCH_4_BITS_MODE);//luowl   
        }
        else
        {
        	bch_mode_set(BCH_OFF);
        } 
        nf_8k_en(0);
        //nf_ecc_nand_flag = 0;
        nf_ecc_nand_en(0);
    }
    else	//GP10 2011-01-27 add for ECC and 8k page nand flash
    {
    	if((nf_8k_get()) || (nf_ecc_nand_get()))
    	{
    		//nand_page_size_set(4096);
    		ib10_smart_para_get(&ib6_page_size, &ib6_blk_pages);
        	nand_page_size_set(ib6_page_size);
    		//bch_mode_set(BCH_12_BITS_MODE);
        	//nf_8k_flag = 0;				//yunxiangan 2010-12-08
        	nf_8k_en(0);
        	//nf_ecc_nand_flag = 0;
        	nf_ecc_nand_en(0);
    	}
    	
    }

    nand_header_read((INT8U *) &tt_basic.WorkBuffer[0]);
 

    if(gpl32_version_get() < GPL32_GP10) {
        // gp6 header 只認得 BCH4
        nand_page_size_set(backup_page_size);
        bch_mode_set(backup_bch_mode);    
        //nf_8k_flag = backup_nf_8k_flag; 
        nf_8k_en(backup_nf_8k_flag);
        //nf_ecc_nand_flag = backup_nf_ecc_nand_flag;
        nf_ecc_nand_en(backup_nf_ecc_nand_flag);
    }
    else	//GP10 2011-01-27 add for ECC and 8k page nand flash
    {
    	if((backup_nf_8k_flag) || (backup_nf_ecc_nand_flag))
    	{
	        nand_page_size_set(backup_page_size);
	        bch_mode_set(backup_bch_mode); 
	        //nf_8k_flag = backup_nf_8k_flag;
	        nf_8k_en(backup_nf_8k_flag);
	        //nf_ecc_nand_flag = backup_nf_ecc_nand_flag;
	        nf_ecc_nand_en(backup_nf_ecc_nand_flag);
	    }
    }


    ret = NandHeaderInfoSet((INT8U *) &tt_basic.WorkBuffer[0]);
    
    return ret;
}


INT32S NandHeaderInfoSet(INT8U* header_data) 
{
	INT8U  bApp_Percent;
    INT8U  wp_pin_io=0;
    INT16U bApp_Start;
	INT16U wApp_Size;
    INT16U fs_blk_offset;
    INT32S ret=STATUS_FAIL;
    INT16U data_area_bank_blocks=512;
    INT16U bank_recycle_blocks=32;
    INT32U Used_BLK_Number = 0;	
	INT16U Part0_Mode = 0;
	INT32U part0_blk_number = 0;
	INT16U Part1_Mode = 0;
	INT32U part1_blk_number = 0;
	INT32U Partition_number = 0;
    INT32U nf_total_MB_size = 0;
    INT32U app_page_nums;
    INT16U ib6_page_size;
    INT16U ib6_blk_pages;
    INT16U ib10_page_size_extend;
    INT16U ib10_blk_pages_extend;
	INT32U Code_Size;
	INT32U Code_Size_Block_Num = 0;
	HeaderMaxMCLK=96;

	if(nf_rom_flag_get() || nf_otp_bch4_flag_get())
	{
		if(nf_rom_flag_get())
		{
			if ((nand_tag_match_querry((INT8U *) header_data) == 6) || (nand_tag_match_querry((INT8U *) header_data) == 10) || (nand_tag_match_querry((INT8U *) header_data) == 'R'))
			{
				Code_Size  = combin_reg_data2((INT8U *)&header_data[16],2);//HEADER_BOOT_OFFSET_SEC_CNT = 16
				Code_Size_Block_Num = (Code_Size + 32 - 1) / 32;
				SetNandRomAppAreaStartAddr(nand_page_nums_per_block_get() * 10);
				SetNandRomDataAreaStartAddr(nand_page_nums_per_block_get() * (11 + Code_Size_Block_Num));
				//Nand_part0_Offset_Set(0);
				//Nand_part0_mode_Set(NAND_READ_ALLOW|NAND_WRITE_ALLOW);
				nand_part0_para_set(0, DrvNand_get_Size(), NAND_READ_ALLOW);
			}
			else
			{
				NF_DBG_PRINT("nf_rom_flag_get nand_tag_match_querry Fail!\r\n"); 
				ret = -1;
				return ret;
			}
			//ret = STATUS_OK;
		}
		else
		{
			//nand otp 2012-04-09
			if ((nand_tag_match_querry((INT8U *) header_data) == 6) || (nand_tag_match_querry((INT8U *) header_data) == 10) || (nand_tag_match_querry((INT8U *) header_data) == 'R'))
			{
				Code_Size  = combin_reg_data2((INT8U *)&header_data[16],2);//HEADER_BOOT_OFFSET_SEC_CNT = 16
				Code_Size_Block_Num = (Code_Size + 32 - 1) / 32;
				SetNandRomAppAreaStartAddr(nand_page_nums_per_block_get() * 30);//5+5
				SetNandRomDataAreaStartAddr(nand_page_nums_per_block_get() * (31 + Code_Size_Block_Num));
				//Nand_part0_Offset_Set(0);
				//Nand_part0_mode_Set(NAND_READ_ALLOW|NAND_WRITE_ALLOW);
				nand_part0_para_set(0, DrvNand_get_Size(), NAND_READ_ALLOW);
			}
			else
			{
				NF_DBG_PRINT("nf_otp_bch4_flag_get nand_tag_match_querry Fail!\r\n"); 
				ret = -1;
				return ret;
			}
			//ret = STATUS_OK;
		}
		ret = STATUS_OK;
	}
	else
	{
	if (nand_block0_AppTag_get((INT8U *) header_data) == APP_PART_TAG)
	{
        NF_DBG_PRINT("Auto Config Teg exist.\r\n");  
        HeaderMaxMCLK = (INT8U)combin_reg_data2(&header_data[HEADER_MAX_MCLK_MHz],1);        
		bApp_Start = (INT8U)combin_reg_data2(&header_data[HEADER_OFFSET_APP_START],1);
        nand_l2_bootarea_size = bApp_Start*Nand_blk_size_get();
        wApp_Size  = (INT16U)combin_reg_data2(&header_data[HEADER_OFFSET_APP_SIZE],2);
        
        if(gpl32_version_get() < GPL32_GP10) {
            ib6_smart_para_get(&ib6_page_size, &ib6_blk_pages);
           	nand_l2_app_start_page_id = bApp_Start*ib6_blk_pages;
            /*換算成為 SVN driver 之 Block 單位*/
            bApp_Start = nand_l2_app_start_page_id/nand_page_nums_per_block_get();
            if (nand_l2_app_start_page_id%nand_page_nums_per_block_get()) {
                bApp_Start++;  // Block alignment
            }
            app_page_nums = wApp_Size*ib6_blk_pages;  
            /*換算成為 SVN driver 之 Block 單位*/
            wApp_Size = app_page_nums/nand_page_nums_per_block_get();
            if (app_page_nums%nand_page_nums_per_block_get()) {
                wApp_Size++;  // Block alignment
            }
        }
        else
        {
        	if(nf_8k_get()) //2012-11-30 correct app size
        	{
	        	ib10_smart_para_get(&ib10_page_size_extend, &ib10_blk_pages_extend);
	           	nand_l2_app_start_page_id = bApp_Start*ib10_blk_pages_extend;
	            /*換算成為 SVN driver 之 Block 單位*/
	            bApp_Start = nand_l2_app_start_page_id/nand_page_nums_per_block_get();
	            if (nand_l2_app_start_page_id%nand_page_nums_per_block_get()) {
	                bApp_Start++;  // Block alignment
	            }
	            app_page_nums = wApp_Size*ib10_blk_pages_extend;  
	            /*換算成為 SVN driver 之 Block 單位*/
	           // wApp_Size = app_page_nums/nand_page_nums_per_block_get();
	           // if (app_page_nums%nand_page_nums_per_block_get()) {
	           //     wApp_Size++;  // Block alignment
	           // }
	           wApp_Size = (app_page_nums * ib10_page_size_extend + Nand_blk_size_get() - 1) / Nand_blk_size_get();
	        }
        }

        nand_l2_app_size = wApp_Size*Nand_blk_size_get();
        nf_total_MB_size = (INT16U)combin_reg_data2(&header_data[HEADER_NAND_MB_SIZE],2);
        if (nf_total_MB_size!=0)   /*header assign is always high priority*/
        {
            nand_total_MByte_size_set(nf_total_MB_size);
        }
        
        wp_pin_io = (INT8U)combin_reg_data2(&header_data[HEADER_OFFSET_WP_PIN_ID],1); 
        if (wp_pin_io!=0)   /*header assign is always high priority*/
        {
            nand_wp_pin_reg(wp_pin_io);
        } 

       #if 1  //(defined BOARD_TYPE) && (BOARD_TYPE == BOARD_TK_32600_COMBO)                       //
        if (wp_pin_io==0)   /*header assign is always high priority*/
        {
            nand_wp_pin_reg(0x23);  // TURNKEY
        }
       #endif

        bApp_Percent = (INT8U)combin_reg_data2(&header_data[HEADER_OFFSET_APP_PRENT],1);
        if(bApp_Percent== 0) 
        {
       		bApp_Percent = 8;
        }      
        //nand_protect_block_nums_set(bApp_Start);  // move into set nand info
		
        data_area_bank_blocks=(INT16U)combin_reg_data2(&header_data[HEADER_BANK_BLOCKS],4);
        if (data_area_bank_blocks==0) {
            if(nand_page_size_get() > 512) {
                data_area_bank_blocks = 512;
            } else {
                data_area_bank_blocks = 200;
            }
        }

        bank_recycle_blocks=(INT16U)combin_reg_data2(&header_data[HEADER_RECYCLE_BLOCKS],4);
        if (bank_recycle_blocks==0) {
            if(nand_page_size_get() > 512) {
                bank_recycle_blocks = 64;
            } else {
                bank_recycle_blocks = 20;
            }
        }
        SetNandInfo(bApp_Start, wApp_Size, bApp_Percent, data_area_bank_blocks,bank_recycle_blocks);
       
/// FAT Area config////

		Partition_number = (INT32U)combin_reg_data2(&header_data[HEADER_OFFSET_PARTITION_NUM],1);	
		switch(Partition_number)
		{
			case 0:  
				fs_blk_offset = (INT16U)combin_reg_data2(&header_data[HEADER_OFFSET_USER_SETTING_SIZE_BLK],4);
				Nand_part0_Offset_Set(fs_blk_offset*Nand_blk_size_get()/512);
				
				fs_blk_offset = (INT16U)combin_reg_data2(&header_data[HEADER_OFFSET_USER_SETTING_SIZE_BLK],4);				
				Used_BLK_Number = bApp_Start + wApp_Size + (wApp_Size/bApp_Percent) + fs_blk_offset;
				nand_fs_blk_number = Nand_blk_number_get() - Used_BLK_Number;
								
				Part0_Mode = 0x03;				
				part0_blk_number = nand_fs_blk_number;
				
				Nand_part0_Offset_Set(fs_blk_offset*(Nand_blk_size_get()/512));
				Nand_part0_size_Set(part0_blk_number*(Nand_blk_size_get()/512));
				Nand_part0_mode_Set(Part0_Mode);
				
				Nand_part1_Offset_Set(0);
				Nand_part1_size_Set(0);	
				Nand_part1_mode_Set(0);					
				Nand_Partition_Num_Set(1);
				break;
			case 1:
				fs_blk_offset = (INT16U)combin_reg_data2(&header_data[HEADER_OFFSET_USER_SETTING_SIZE_BLK],4);				
				Used_BLK_Number = bApp_Start + wApp_Size + (wApp_Size/bApp_Percent) + fs_blk_offset;
				nand_fs_blk_number = Nand_blk_number_get() - Used_BLK_Number;
								
				part0_blk_number = (INT32U)combin_reg_data2(&header_data[HEADER_OFFSET_NAND_PART0_SIZE_BLK],4);
				Part0_Mode = (INT16U)combin_reg_data2(&header_data[HEADER_OFFSET_NAND_PART0_ATTRIBUTE],1);        
				
				if((part0_blk_number == 0)||(part0_blk_number > nand_fs_blk_number))
				{
					part0_blk_number = nand_fs_blk_number;
				}
				
				Nand_part0_Offset_Set(fs_blk_offset*(Nand_blk_size_get()/512));
				Nand_part0_size_Set(part0_blk_number*(Nand_blk_size_get()/512));
				Nand_part0_mode_Set(Part0_Mode);

				Nand_part1_Offset_Set(0);
				Nand_part1_size_Set(0);	
				Nand_part1_mode_Set(0);
				Nand_Partition_Num_Set(1);		
				break;
			case 2:
				fs_blk_offset = (INT16U)combin_reg_data2(&header_data[HEADER_OFFSET_USER_SETTING_SIZE_BLK],4);
				Used_BLK_Number = bApp_Start + wApp_Size + (wApp_Size/bApp_Percent) + fs_blk_offset;
				nand_fs_blk_number = Nand_blk_number_get() - Used_BLK_Number;
				
				part0_blk_number = (INT32U)combin_reg_data2(&header_data[HEADER_OFFSET_NAND_PART0_SIZE_BLK],4);
				Part0_Mode = (INT16U)combin_reg_data2(&header_data[HEADER_OFFSET_NAND_PART0_ATTRIBUTE],1);        
								
				if((part0_blk_number > nand_fs_blk_number)||(part0_blk_number == 0))
                {            
                    part0_blk_number = nand_fs_blk_number;
                }
				Nand_part0_Offset_Set(fs_blk_offset*(Nand_blk_size_get()/512));
				Nand_part0_size_Set(part0_blk_number*(Nand_blk_size_get()/512));
				Nand_part0_mode_Set(Part0_Mode);		
						
				Part1_Mode = (INT16U)combin_reg_data2(&header_data[HEADER_OFFSET_NAND_PART1_ATTRIBUTE],1);        
				part1_blk_number = nand_fs_blk_number - part0_blk_number;
				
				Nand_part1_Offset_Set(Nand_part0_size_Get() + Nand_part0_Offset_Get());
				Nand_part1_size_Set(part1_blk_number*(Nand_blk_size_get()/512));			
				Nand_part1_mode_Set(Part1_Mode);
				Nand_Partition_Num_Set(2);
				break;
			default:
                nand_part0_para_set(0,0,0);
                nand_part1_para_set(0,0,0);
                Nand_Partition_Num_Set(0); 
				break;			    				
		}    
/// FAT Area config////		        
              	
        ret = STATUS_OK;
	}
    else {
          NF_DBG_PRINT("Auto Config Teg not find.\r\n");
        #if (defined _PROJ_TYPE) && ((_PROJ_TYPE == _PROJ_TURNKEY)||(_PROJ_TYPE == _PROJ_TURNKEY))
            fs_blk_offset = 8;//resvered 8 blocks
        #else
            fs_blk_offset = 0;
        #endif
        
        Nand_Part0_Offset = fs_blk_offset*Nand_blk_size_get()/512;
        nand_l2_app_size = APP_AREA_SIZE*Nand_blk_size_get();
        nand_l2_bootarea_size = APP_AREA_START*Nand_blk_size_get();
        
        bApp_Start = APP_AREA_START;
        wApp_Size = APP_AREA_SIZE;
        bApp_Percent = APP_AREA_SPARE_PERCENT;
        if(bApp_Percent == 0) {bApp_Percent = 8;}
        //nand_protect_block_nums_set(bApp_Start);  // move into set nand info

		    if(nand_page_size_get() > 512) {
		        data_area_bank_blocks = 512;
		        bank_recycle_blocks = 64;
		    } else {
		        data_area_bank_blocks = 200;
		        bank_recycle_blocks = 20;
		    }
		    
		    SetNandInfo(bApp_Start, wApp_Size, bApp_Percent, data_area_bank_blocks,bank_recycle_blocks );
			
			
		    Used_BLK_Number = bApp_Start + wApp_Size + (wApp_Size/bApp_Percent) + fs_blk_offset;
			nand_fs_blk_number = Nand_blk_number_get() - Used_BLK_Number;
			nand_fs_sector_number = nand_fs_blk_number * nand_page_nums_per_block_get() * nand_sector_nums_per_page_get();	
		         
		    nand_part0_para_set(Nand_part0_Offset_Get(), nand_fs_sector_number, (NAND_READ_ALLOW|NAND_WRITE_ALLOW));
		    nand_part1_para_set(0, 0, (NAND_READ_ALLOW|NAND_WRITE_ALLOW));
		    nand_part2_para_set(0, 0, (NAND_READ_ALLOW|NAND_WRITE_ALLOW));
		   	
		    /* do nothing, auto use default value*/
		}
	}
    
   switch(bch_mode_get())	// zurong add for auto adjust bit error count control
   {
   	case BCH_4_BITS_MODE:
   		SetBitErrCntAsBadBlk(3);
   		break;
   		
   	case BCH_8_BITS_MODE:
   		SetBitErrCntAsBadBlk(6);
	   	break;
	   	
   	case BCH_12_BITS_MODE:
   		SetBitErrCntAsBadBlk(9);
   		break;  
   	default:
   		break;   	
   }
 
 	NF_DBG_PRINT("Nand Total Size= %d MB, \r\n", nand_total_MByte_size_get());
	NF_DBG_PRINT("Nand_blk_size_get= 0x%x B, \r\n", Nand_blk_size_get());
	NF_DBG_PRINT("bApp_Start= %x\r\n", bApp_Start);
	NF_DBG_PRINT("wApp_Size= %x\r\n", wApp_Size);
	NF_DBG_PRINT("bApp_Percent= %x,spare_size:0x%x\r\n", bApp_Percent, wApp_Size/bApp_Percent);
	NF_DBG_PRINT("fs_blk_offset= %x\r\n", fs_blk_offset);
	NF_DBG_PRINT("Used_BLK_Number= %x\r\n", Used_BLK_Number);
	NF_DBG_PRINT("Nand_BLK_Number= %x\r\n", Nand_blk_number_get());
	NF_DBG_PRINT("Remain_BLK_Number= %x\r\n", nand_fs_blk_number);
	NF_DBG_PRINT("part0_blk_number= %x\r\n", part0_blk_number);
	NF_DBG_PRINT("data_area_bank_blocks= %d\r\n", data_area_bank_blocks);
	NF_DBG_PRINT("bank_recycle_blocks= %d\r\n", bank_recycle_blocks);
    return ret;

}

INT32U NandAppByteSizeGet(void)
{
    return nand_l2_app_size;
}

INT32U NandBootAreaByteSizeGet(void)
{
    return nand_l2_bootarea_size;
}

INT32U nand_block0_AppTag_get(INT8U* header_data)
{
    INT32U bApp_Tag;

    bApp_Tag = (INT32U)combin_reg_data2(&header_data[HEADER_OFFSET_APP_FLAG],4);
     
    //Nand_BCH_force_mode_set(BCH_AUTO_MODE);
    if (bApp_Tag==APP_PART_TAG) {  	
       	
        return APP_PART_TAG;
    } else {
        
        return 0xFFFFFFFF;  
    }
}




TAG_ENUM nand_header_read(INT8U* data_buf) 
{
    INT8S header_scan_blks;
    INT8S nand_option_well;
    INT8U  nand_tag = 0xFF;
    INT32U i;
    INT16U blk_page_offset;
    INT32U header_page;
    INT32U ret;

    nand_option_well = 0; /*User Option Mode 需要成功讀到才能設起來*/
    header_scan_blks = MAX_GOOD_HEADER_BLKS; /*最多承認前20個 "good" block 之page 可能含有 Header*/
	if(nf_rom_flag_get() || nf_otp_bch4_flag_get())
	{
		if(nf_rom_flag_get())
		{
			ret = Nand_ReadPhyPage((32*10),(INT32U) data_buf);		//modify read APP HEADER bootheader(5copys)+bootloader(1copys)=10blocks
			nand_tag = nand_tag_match_querry(data_buf);
		}
		else
		{
			//nand Otp 2012-04-09
			ret = Nand_ReadPhyPage((32*30),(INT32U) data_buf);		//modify read APP HEADER bootheader(5copys)+bootloader(5copys)=30blocks
			nand_tag = nand_tag_match_querry(data_buf);	
		}
	}
	else
	{ 
	    for (i=0;i<nand_page_nums_per_block_get()*header_scan_blks;i++) 
	    {
	      header_page = i;
	      if(gpl32_version_get() < GPL32_GP10) {
	          if (nand_page_size_get()>512) {
	              blk_page_offset = i%nand_page_nums_per_block_get();
	              if ((blk_page_offset%GPL6_BOOTAREA_BCH_CHG_CYCLE)==0) {  //wwj change, in GP6, each 8 pages, turn to BCH4 encode Header
	                  bch_mode_set(BCH_4_BITS_MODE);
	              } else {
	                  bch_mode_set(BCH_8_BITS_MODE);
	              }
	          }
	      }
	      
	      ret = Nand_ReadPhyPage(i,(INT32U) data_buf);
	      if((nand_option_well==0) && (nand_option_table_detect(data_buf)==STATUS_OK))  // 看有沒有Nand Para of User Define
	      {
	          nand_option_well = 1;
	          ret = Nand_ReadPhyPage(i,(INT32U) data_buf);  // 以 User Option value 鄭重再讀一次
	      }
	  
	      if (i==0) {
	          nand_tag = nand_tag_match_querry(data_buf);
	          if(nand_tag == 'R') {
	              nand_small_page_force_set();                                                                                                                     
	              break;  /*Nand ROM 出現, 直接 Boot*/
	          } 
	      } else {
	          if ((i%nand_page_nums_per_block_get())==0) { /*scan 到非 blk0 之 blk*/
	              if (nand_block_checkbad(i/nand_page_nums_per_block_get(),data_buf)!=0) {
	                  if (nand_page_size_get() == 4096) {   /*避免壞塊誤判, 可能是 BCH mode 設錯而已*/
	                      bch_polling_exchange();
	                      if (nand_block_checkbad(i/nand_page_nums_per_block_get(),data_buf)!=0) {
	                          i+=nand_page_nums_per_block_get()-1; 
	                          header_scan_blks++; /*因為躲掉一個壞塊, 所以總塊數+1*/
	                          bch_polling_exchange();  //確定是壞塊, 與 BCH 無關, 返回原bch值
	                          continue;
	                      } 
	                  } else {                        
	                      i+=nand_page_nums_per_block_get()-1; 
	                      header_scan_blks++; /*因為躲掉一個壞塊, 所以總塊數+1*/
	                      continue;
	                  }
	              }
	          }
	      }
	      if (ret == STATUS_OK) {
	          /*成功沒那麼簡單, TAG 還要對才行*/
	          nand_tag = nand_tag_match_querry(data_buf);
	          if (nand_tag != 0xFF) {
	              break;  // page 對了, header 也認到了
	          } else {
	              continue;  // page 對了, 但 header 認不到
	          }
	      } else {  /* BCH Read 失敗, 在4K page 中有兩個可能原因, 1.BCH mode錯, 2.page本來就無法BCH解 */
	          if (nand_page_size_get()==4096) 
	          {
	              /*不用灰心, 改用 BCH12/8 看看, 再重 Try*/
	              bch_polling_exchange();
	              if(Nand_ReadPhyPage(i,(INT32U)data_buf)==STATUS_OK) 
	              {
	                  /*成功沒那麼簡單, TAG 還要對才行*/
	                  /*猜中該 Nand 為 BCH12 4K nand*/
	                  nand_tag = nand_tag_match_querry(data_buf);
	                  if (nand_tag!=0xFF) {
	                      break;  // page 對了, header 也認到了
	                  } else {
	                    continue;  // page 對了, 但 header 認不到
	                  }
	              } 
	              else  /*如果又錯了, 再把 BCH mode 換回來, 有需要再 polling exchange*/ 
	              {  
	                  bch_polling_exchange();
	              }                     
	          }
	      }//if (ret == STATUS_OK) 
	    }//for (i=0;i<nand_page_nums_per_block_get()*header_scan_blks;i++) 
	}

    return nand_tag;
}

INT32U nand_block_checkbad(INT32U block_id, INT8U* data_buf)
{
    INT32U ret;
    INT8U  Mark;
    INT16U wBadFlgPage1;
    INT16U wBadFlgPage2;
    INT32U wPhysicPage1;
    INT32U wPhysicPage2;
    
    if((nand_page_size_get() == 512) || (nand_page_nums_per_block_get() == 64))
    {
    	wBadFlgPage1 = 0;
    	wBadFlgPage2 = 1;
    }
    else
    {
	    wBadFlgPage1 = 0;
	    wBadFlgPage2 =  nand_page_nums_per_block_get() - 1;
    }
    
	wPhysicPage1 	= ((INT32U)block_id* nand_page_nums_per_block_get()) + wBadFlgPage1;
	ret = Nand_ReadPhyPage(wPhysicPage1,(INT32U) data_buf);

    if(ret != 0) {
        return 1;  
    }

	if (nand_page_size_get() == 512)
	{
		 Mark =  data_buf[511+ 6];
	}	 
	else //other page size
	{
		 Mark =  data_buf[nand_page_size_get()]; 
    }
	
	if( Mark != 0xFF ) 
    {
        return 2;
    }

    wPhysicPage2 	= ((INT32U)block_id* nand_page_nums_per_block_get()) + wBadFlgPage2;
	ret = Nand_ReadPhyPage(wPhysicPage2,(INT32U) data_buf);

    if(ret != 0) {
        return 1;  
    }

	if (nand_page_size_get() == 512)
	{
		 Mark =  data_buf[511+ 6];
	}	 
	else //other page size
	{
		 Mark =  data_buf[nand_page_size_get()]; 
    }
	
	if( Mark != 0xFF ) 
    {
        return 2;
    }
    
    return 0; /*good block return*/
}

void bch_polling_exchange(void) 
{
    if (bch_mode_get()==BCH_8_BITS_MODE) {
        nand_set_hw_bch(BCH_12_BITS_MODE); 
    } else if (bch_mode_get()==BCH_12_BITS_MODE) {
        nand_set_hw_bch(BCH_8_BITS_MODE); 
    }	
}


INT32U combin_reg_data2(INT8U *data,INT32S len)
{
	INT32S i;
	INT32U value = 0;
	for (i=0;i<len;i++) {
		value |= data[i] << i*8;
	}
	return value;
}

INT8U nand_tag_match_querry(INT8U *page_buf) 
{
    INT8U ret=0xFF;
    if (gp_strncmp((INT8S *)page_buf, (INT8S *)NAND_FLASH_GP10_TAG,10) == 0) 
    {
        ret = page_buf[HEADER_OFFSET_NAND_ROM];
        if (page_buf[HEADER_OFFSET_NAND_ROM]<=10) {
            ret=10;
        }          
        return ret;
    } 
    else if (gp_strncmp((INT8S *)page_buf, (INT8S *)NAND_FLASH_GP6_TAG,10) == 0)
    {
        ret = page_buf[HEADER_OFFSET_NAND_ROM];
        if (page_buf[HEADER_OFFSET_NAND_ROM]<=10) {
            ret=6;
        }          
        return ret;
    }
    return ret;

}


INT8S nand_option_table_detect(INT8U* head_data)
{
    INT16U page_size_idx[4]={512,2048,4096,8192};  
    INT16U page_nums_per_block_idx[5]={32,64,128,256,512};
//    INT8U optin_en;
    INT16U hash_key;
    INT16U hash_val;
    BCH_MODE_ENUM bch_set;

//    INT16U op_page_size,op_address_cycle,op_page_nums_per_block,op_ECC_check,op_ECC_bit;
 
    hash_val =  0xA8D8 +   // Hash Seed
                head_data[HEADER_OFFSET_ND_DEFINE] + 
                page_size_idx[head_data[HEADER_OFFSET_ND_PAGE]] + 
                head_data[HEADER_OFFSET_ND_ADDR_CYC] +
                page_nums_per_block_idx[head_data[HEADER_OFFSET_ND_BLOCK]] +
                head_data[HEADER_OFFSET_BCH_ENABLE] +
                head_data[HEADER_OFFSET_BCH_BIT];
                
    hash_key=combin_reg_data2(&head_data[HEADER_OFFSET_ND_HASH_KEY],2);	

    if ((hash_val==hash_key) && (head_data[HEADER_OFFSET_ND_DEFINE]==1))
    {
       nand_page_size_set(page_size_idx[head_data[HEADER_OFFSET_ND_PAGE]]);
       nand_addr_cycle_set(head_data[HEADER_OFFSET_ND_ADDR_CYC]);
       nand_page_nums_per_block_set(page_nums_per_block_idx[head_data[HEADER_OFFSET_ND_BLOCK]]); 
       //ECC_bit = combin_reg_data2(&head_data[HEADER_OFFSET_BCH_BIT],1);	
       if (head_data[HEADER_OFFSET_BCH_ENABLE]==0) {
            bch_mode_set(BCH_OFF);
       }
       
       if (head_data[HEADER_OFFSET_BCH_BIT]==0) {
            bch_set=BCH_4_BITS_MODE;
       } else if (head_data[HEADER_OFFSET_BCH_BIT]==1) {
            bch_set=BCH_8_BITS_MODE;
       } else if (head_data[HEADER_OFFSET_BCH_BIT]==2) {
            bch_set=BCH_12_BITS_MODE;
       } else {
            bch_set=BCH_8_BITS_MODE;
       }
       
       nand_set_hw_bch(bch_set);
       return STATUS_OK;  // Option Mode define Well
    } 
    
    return STATUS_FAIL;  // Option Mode define FAIL
}

#if 0   /* jandy mark*/
INT32U DrvNand_bchtable_alloc(void)
{
	#if (defined _PROJ_BOOTLOADER)&&(_PROJ_TYPE ==_PROJ_BOOTLOADER)
		if(gMallocBCHTable != 0x5aa5)
		{
	// enable CS
			bch_alpha_to = (INT16S*) 0x500000+0x8000;
			if (bch_alpha_to == NULL) 
			{
				return 1;
			}	
			bch_index_of = (INT16U*)0x500000+0x8000*2;
			if (bch_index_of == NULL) 
			{	
				return 2;
			}	
			nand_flash_bchtable_alloc(bch_alpha_to,bch_index_of);
			gMallocBCHTable = 0x5aa5;
		}
	 	return 0;
	 	
	#else	
	   	if(gMallocBCHTable != 0x5aa5)
		{
			bch_alpha_to = (INT16S*) &bch_table1;//gp_malloc(8192*2);
			if (bch_alpha_to == NULL) 
			{
				return 1;
			}	
			bch_index_of = (INT16U*) &bch_table2;//gp_malloc(8192*2);
			if (bch_index_of == NULL) 
			{	
				return 2;
			}	
			nand_flash_bchtable_alloc(bch_alpha_to,bch_index_of);
			gMallocBCHTable = 0x5aa5;
		}
	 	return 0;
	 	
	#endif
	
}
void DrvNand_bchtable_free(void)
{
	#if (defined _PROJ_BOOTLOADER)&&(_PROJ_TYPE ==_PROJ_BOOTLOADER)
	//for nandboot loader Null 
	#else
	   if(gMallocBCHTable == 0x5aa5)
	   {
		 /* if(bch_index_of != NULL)
				gp_free(bch_alpha_to);
	      if (bch_index_of!=NULL)	    		
				gp_free(bch_index_of);*/
	   }
	 #endif  
   gMallocBCHTable = 0;
}	
#else
INT32U DrvNand_bchtable_alloc(void){return STATUS_OK;}
void DrvNand_bchtable_free(void) {;;}
#endif




INT16U Nand_iData_Start_Addr_get(void)
{
    return gNandUsrInfo.uiDataStart;
}


void Nand_part0_Offset_Set(INT32U nand_fs_sector_offset)
{
    Nand_Part0_Offset = nand_fs_sector_offset;
}

void Nand_part0_mode_Set(INT16U mode)
{
	Nand_Part0_Mode = mode;
}

void Nand_part0_size_Set(INT32U nand_fs_sector_size)
{
	Nand_Part0_Size = nand_fs_sector_size;
}

void Nand_part1_Offset_Set(INT32U nand_fs_sector_offset)
{
    Nand_Part1_Offset = nand_fs_sector_offset;
}

void Nand_part1_mode_Set(INT16U mode)
{
	Nand_Part1_Mode = mode;
}

void Nand_Partition_Num_Set(INT16U number)
{
	Nand_Partition_NUM = number;
}

INT16U Nand_Partition_Num_Get(void)
{
	 return Nand_Partition_NUM;
}


void Nand_part1_size_Set(INT32U nand_fs_sector_size)
{
	Nand_Part1_Size = nand_fs_sector_size;
}

INT32U Nand_part0_Offset_Get(void)
{
  return Nand_Part0_Offset;
}

INT32U Nand_part0_mode_Get(void)
{
  return Nand_Part0_Mode;
}

INT32U Nand_part0_size_Get(void)
{
  return Nand_Part0_Size;
}

INT32U Nand_part1_Offset_Get(void)
{
  return Nand_Part1_Offset;
}

INT32U Nand_part1_mode_Get(void)
{
  return Nand_Part1_Mode;
}

INT32U Nand_part1_size_Get(void)
{
  return Nand_Part1_Size;
}
void nand_part0_para_set(INT32U offset, INT32U size, INT16U mode)
{
    Nand_Part0_Offset = offset;
    Nand_Part0_Size = size;
    Nand_Part0_Mode = mode;
}

void nand_part1_para_set(INT32U offset, INT32U size, INT16U mode)
{
    Nand_Part1_Offset = offset;
    Nand_Part1_Size = size;
    Nand_Part1_Mode = mode;
}

void nand_part2_para_set(INT32U offset, INT32U size, INT16U mode)
{
    Nand_Part2_Offset = offset;
    Nand_Part2_Size = size;
    Nand_Part2_Mode = mode;
}

INT32U nand_fs_sector_number_get(void)
{
    return nand_fs_sector_number;
}

INT8U nand_header_max_sys_clk_get(void)
{
    return HeaderMaxMCLK;
}

#ifndef NAND_16BIT
INT32U DMAmmCopy(INT32U wSourceAddr, INT32U wTargeAddr, INT16U Count) //count is byte num
{
	DMA_STRUCT dma_struct;
	INT8S done;
	INT32S result;
	#if (defined _PROJ_BOOTLOADER)&&(_PROJ_TYPE ==_PROJ_BOOTLOADER)
		if (0)
	#else
		if (OSRunning==TRUE)//luowl
	#endif
    {
		done = C_DMA_STATUS_WAITING;
		dma_struct.s_addr = (INT32U) wSourceAddr;
		dma_struct.t_addr = (INT32U) wTargeAddr;
		dma_struct.width = 4;		// DMA_DATA_WIDTH_1BYTE or DMA_DATA_WIDTH_2BYTE or DMA_DATA_WIDTH_4BYTE
		dma_struct.count = (INT32U)(Count/4); 
		dma_struct.notify = &done;
		dma_struct.timeout = 0;
		result = dma_transfer(&dma_struct);
		if (result) {
            gp_memcpy((INT8S *)wTargeAddr,(INT8S *)wSourceAddr,(INT32U)Count);
			return 0;
		}
		
		while (1)
		 {
			if (done != C_DMA_STATUS_WAITING)
			{
				break;
			}
		}

        cache_invalid_range((INT32U) wTargeAddr, Count);
		return 0;
	}
	else
	{
            #if _DRV_L1_CACHE==1
              cache_invalid_range((INT32U) wTargeAddr, Count);
              cache_invalid_range((INT32U) wSourceAddr, Count);
            #endif 	

             mem_transfer_dma((INT32U) wSourceAddr, (INT32U) wTargeAddr, (INT32U)Count);
             //gp_memcpy((INT8S *)wTargeAddr,(INT8S *)wSourceAddr,(INT32U)Count);
	}	
	return 0;
}
#endif

void nf_null_printf(CHAR *fmt, ...) {;;}


//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L2_NAND) && (_DRV_L2_NAND == 1)            //
//================================================================//

