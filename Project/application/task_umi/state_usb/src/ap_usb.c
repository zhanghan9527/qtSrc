#include "ap_usb.h"

#define DEVICE_READ_ALLOW	0x1
#define DEVICE_WRITE_ALLOW	0x2

#define RWSECTOR_512B     1
#define RWSECTOR_1024B    2
#define RWSECTOR_2048B    4
#define RWSECTOR_4096B    8
#define RWSECTOR_8192B    16
#define RWSECTOR_16384B   32

#define READWRITE_SECTOR   RWSECTOR_16384B

//#define MASK_CMD7		            (0x00000007|0x7000|0x0080)	// SELECT/DESELECT_CARD ?
#define LUNTYPE_NF_APP		FS_NAND_APP
#define LUNTYPE_NF 	        FS_NAND1
#define LUNTYPE_MS   		FS_MS
#define LUNTYPE_SDCMMC	 	FS_SD
#define LUNTYPE_CF	        FS_CF
#define LUNTYPE_XD	        FS_XD
#define LUNTYPE_NF1			FS_NAND2
#define LUNTYPE_CDROM		FS_CDROM
#define LUNTYPE_NOR			FS_NOR
#define LUNTYPE_RAMDISK		FS_RAMDISK
////////////////////////str_USB_Lun_Info->unStatus//////////////////////////////////
#define LUNSTS_NORMAL			0x0000
#define LUNSTS_NOMEDIA			0x0001
#define LUNSTS_WRITEPROTECT		0x0002
#define LUNSTS_MEDIACHANGE		0x0004

struct DrvInfo2 {
	INT32U nSectors;
	INT32U nBytesPerSector;
};
struct Drv_FileSystem2 {
	INT8U 	Name[8];
	INT8U 	Status;
    INT32S	(*Drv_Initial)(void);
    INT32S	(*Drv_Uninitial)(void);
    void 	(*Drv_GetDrvInfo)(struct DrvInfo2* info);
    INT32S	(*Drv_ReadSector)(INT32U, INT32U, INT32U);
    INT32S	(*Drv_WriteSector)(INT32U,INT32U, INT32U);
};

INT32U usbd_tftblight_on = 0;

#if (MCU_VERSION == GPL32_B)
INT16U USBRWError_Flag = 0;             // B IC
extern INT32U usbd_check_bi_dma();
#endif
int   R_Write_protect;
//LUN
INT16U usbd_current_lun;

t_egui_image usb_connect_sprite1;
SPRITE usb_connect_sprite2;
INT16U usb_storage_curent[8];
INT8U *p_usbd_user_string_buffer=NULL;
INT16U *BUF=NULL; //A+B
/////////////////////////////////////////////
//add by crf
BOOLEAN usb_plug_states_flag =0;
BOOLEAN Usb_Dvice_WebCam_Demo_turn_on_flag = 0;
INT8U   s_usbmode_check_pin;
////////////////////////////////////////////

#if CDROM_EN
extern INT32S CDROM_ReadSector(INT32U blkno, INT32U blkcnt, INT32U buf);
#endif
extern str_USB_Lun_Info 	USB_Lun_Define[];
extern struct Drv_FileSystem2   *FileSysDrv[];
extern struct Usb_Storage_Access *UsbReadWrite[];

INT32S Read_10(str_USB_Lun_Info* lUSB_LUN_Read);
INT32S Write_10(str_USB_Lun_Info* lUSB_LUN_Read);
void usbdsuspend_callback(void);
void usbdresume_callback(void);
void usbdreset_callback(void);
void usbset_configuration(void);
INT32U usbdvendor_callback(void);
INT32U ep0_datapointer_callback(INT32U nTableType);
//INT32U usbd_pin_detection(void);
INT32U usbd_wpb_detection(void);
void ap_usb_pc_connect_show(void);
void usb_ppu_display(void);
///INT32U usbd_storage_detection(void);
extern void GetLcdResoultion(void);
extern void GetLcdpanel(void);
extern void GetSendFileFromHost(void);
extern INT32S SetCDROMISOName(char* name); //Eugene
extern INT32S usbd_setvendorcmdval(INT8U val);
extern INT32U get_usb_logger_data(void);	//for USB logger
extern  INT32U   CSW_Residue;
extern  INT32U   CSW_Status;

extern INT32U   R_USB_State_Machine ; 
extern INT32U   USB_Status          ;  
extern INT32U   R_USB_Suspend       ;  
extern INT16U USB_UDisk_Eject;
INT16S Test_USB(void);  //judge USB Device
INT32U usbd_msdc_lun_change(str_USB_Lun_Info* pNEW_LUN);


INT32S Test_Write_10(str_USB_Lun_Info* lUSB_LUN_Write);
INT32S Test_Read_10(str_USB_Lun_Info* lUSB_LUN_Write);
INT32S FlushAppArea_10(str_USB_Lun_Info* lUSB_LUN_Write);

void usbd_ftf_blight_power_on()
{
  usbd_tftblight_on = 1;
}

void usb_nothing()
{
}

#if 1	//for USB IF back voltage test	
INT16S usbd_dected_io(void)
{
	if (gpio_read_io(IO_F5))
	{
		*P_USBD_CONFIG1 &= ~(0x02) ;
		//DBG_PRINT("Plug in\r\n");
		return 1;
	}
	else
	{
		*P_USBD_CONFIG1 |= 0x02 ;
		//DBG_PRINT("Plug out\r\n");
		return 0;
	}	
}	
#endif

void ap_usb_pc_connect_show(void)
{
	
}

void usb_ppu_display(void)
{}

void usb_msdc_state_exit(void)
{
	usb_uninitial();
	if (BUF)
	{
	    gp_free((void *) BUF);
	}

	USB_Host_Enable();

}

void usb_msdc_state_initial(void)
{

  INT16U i,PinID,Luns ;
  INT16S ret;
  struct DrvInfo2 inf;
//str_USB_Lun_Info lUSB_LUN_Write;

  BUF= (INT16U *) gp_malloc_align(1024* READWRITE_SECTOR, 16);
  
  usbd_current_lun =0;
  for(i =0 ; i<6 ; i++)
    usb_storage_curent[i] = storage_detection(i);
  //polling storages........
  for(i =0 ; i<6 ; i++)
	USB_Lun_Define[i].unStatus = LUNSTS_NOMEDIA;
 //Set Luns
 	Luns =2;
	SetLuntpye(0,LUNTYPE_NF1);
	SetLuntpye(1,LUNTYPE_SDCMMC);

#if AP_USBD_SDMS_LUN_IN_ONE_LUN == CUSTOM_ON
  usbd_setlun(Luns-2);
#else
  usbd_setlun(Luns-1);
#endif
#if AP_USBD_SDMS_LUN_IN_ONE_LUN == CUSTOM_ON
	for (i = 0 ; i<Luns-1; i++)  //x Luns
#else
	for (i = 0 ; i<Luns; i++)  //x Luns
#endif
  {
     switch (USB_Lun_Define[i].unLunType)
     {
     #if MCU_VERSION <= GP326XXXA // GPL326XX_C // william
	  case LUNTYPE_NF_APP:
	 #endif
	  case LUNTYPE_NF1:
      case LUNTYPE_NF:
            PinID =5;
	        break;
      case LUNTYPE_MS:
            PinID =2;
    	    break;
      case  LUNTYPE_SDCMMC:
            PinID =1;
       		break;
      case  LUNTYPE_CF:
            PinID =0;
       		break;
      case  LUNTYPE_XD:
            PinID =6;
       		break;
      case LUNTYPE_NOR:
      		PinID = 9;
      		break;
      case LUNTYPE_CDROM:
			PinID = 8;
			break;
#if (defined RAMDISK_EN) && (RAMDISK_EN == 1)			
	  case LUNTYPE_RAMDISK:
	  		PinID = 12;
	  		break;
#endif	  		
      default:
       		break;
     }
     if (storage_detection(PinID) || PinID==8|| PinID==9 || PinID == 12)
     {
      	ret = FileSysDrv[USB_Lun_Define[i].unLunType]->Drv_Initial();
  		FileSysDrv[USB_Lun_Define[i].unLunType]->Drv_GetDrvInfo(&inf);
   		FileSysDrv[USB_Lun_Define[i].unLunType]->Drv_Uninitial();
 		if (!ret)
 		{
  			USB_Lun_Define[i].ulSecSize = (INT32U)inf.nSectors;
			USB_Lun_Define[i].unStatus = LUNSTS_NORMAL;
		}
		else
		{
  			USB_Lun_Define[i].ulSecSize =0;
			USB_Lun_Define[i].unStatus = LUNSTS_NOMEDIA;
			if(PinID==8)
			{
				USB_Lun_Define[i].ulSecSize = (INT32U)inf.nSectors;
				USB_Lun_Define[i].unStatus = LUNSTS_NORMAL;
			}
		}
     	}
	 #if AP_USBD_SDMS_LUN_IN_ONE_LUN == CUSTOM_ON
	else// sd invalid then set lun ms
	if(i == 0)
	{
		//SetLuntpye(i,LUNTYPE_MS);
			
		SetLuntpye(i,LUNTYPE_SDCMMC);//Modify QDH 120710
		ret = FileSysDrv[USB_Lun_Define[i].unLunType]->Drv_Initial();
		FileSysDrv[USB_Lun_Define[i].unLunType]->Drv_GetDrvInfo(&inf);
		FileSysDrv[USB_Lun_Define[i].unLunType]->Drv_Uninitial();
 		if (!ret)
 		{
  			USB_Lun_Define[i].ulSecSize = (INT32U)inf.nSectors;
			USB_Lun_Define[i].unStatus = LUNSTS_NORMAL;			
		}
		else
		{
  			USB_Lun_Define[i].ulSecSize =0;
			USB_Lun_Define[i].unStatus = LUNSTS_NOMEDIA;
		}
	}
	#endif
	 
  }

  //initial current Luns
  if (USB_Lun_Define[usbd_current_lun].unStatus == LUNSTS_NOMEDIA)
  {
  for (i =0 ;i <6 ; i++)
  {
    if (USB_Lun_Define[i].unStatus == LUNSTS_NORMAL)
    {
        FileSysDrv[USB_Lun_Define[i].unLunType]->Drv_Initial();
		usbd_current_lun =i ;
        break;
    }
  }
  }
  else
  {
       FileSysDrv[USB_Lun_Define[usbd_current_lun].unLunType]->Drv_Initial();
  }
  //callback function registers;
  usbd_write10_register(&Write_10);
  usbd_read10_register(&Read_10);
  usbd_suspend_register(&usbdsuspend_callback);
  usbd_resume_register(&usbdresume_callback);
  usbd_reset_register(&usbdreset_callback);
#if (MCU_VERSION >= GPL326XX)  
  usbd_set_configuration_register(&usbset_configuration);
#endif  
  usbd_ep0descriptor_register(&ep0_datapointer_callback);
  usbd_writeprotected_register(&usbd_wpb_detection);
  usbd_vendorcallback_register(&usbdvendor_callback);  
  usbd_setvendorcmdval(0xC0);	

  /*
  lUSB_LUN_Write.unLunNum = 0;
  lUSB_LUN_Write.unLunType = 9;
  lUSB_LUN_Write.ulSecSize = 0x4000;
  lUSB_LUN_Write.unStatus = 0;
  
  ret = Test_Write_10(&lUSB_LUN_Write);
  DBG_PRINT("WriteRet:%d\r\n", ret);
  gp_memset((INT8S *)BUF, 0, 1024);
  ret = Test_Read_10(&lUSB_LUN_Write);
  DBG_PRINT("ReadRet:%d\r\n", ret);*/
#if CDROM_EN  
  usbd_CDROM_ReadSector_register(&CDROM_ReadSector);//huanghe for cdrom 090605  
#endif
  
#if (MCU_VERSION == GPL32_B)
   USBRWError_Flag = 0;            // for B IC
#endif
  //IOG[6] WPB initial

#if (defined MCU_VERSION) && (MCU_VERSION <= GPL32_C)
  //uninitail uhost
  USB_Host_Disable();
  //usbd initial
  usb_reset();
  usb_initial();
#else  
  usb_initial_udisk();
#endif  
  
  usbd_interrupt_register(usb_nothing);
  usbd_ep0descriptor_register(ep0_datapointer_callback);
  usbd_suspend_register(usbdsuspend_callback);

  //user string


}

void usb_msdc_state()
{	
	while(1)
	{
	//cmd loop
#if 1	//for USB IF back voltage test	
	if(usbd_dected_io()==0) break;
#endif			
	usb_isr();
	
	#if MCU_VERSION <= GPL32_C
		usb_std_service(0);
	#else
		usb_std_service_udisk(0);
	#endif
	
		usb_msdc_service(0);

	#if (MCU_VERSION == GPL32_B)
		if (USBRWError_Flag == 0xfefe)      // B IC
		{
			break;
		}
	#endif
	   //pollingExit:
		if (storage_detection(4) == 0)
		{
		//DBG_PRINT("PollingExit U Disk!!\r\n");
			//break;
		}
	
	#if MCU_VERSION >= GPL326XX
		////modify dehaiqin for mac eject u disk 20130802
		if (USB_UDisk_Eject&&(R_USB_State_Machine==0))
		{
		//DBG_PRINT("USB_UDisk_Eject U Disk!!\r\n");
			break;
		}	
	#endif   	
		
		if(Usb_Dvice_WebCam_Demo_turn_on_flag){
			if (storage_detection(4) == 0){
				DBG_PRINT("PollingExit U Disk!!\r\n");
				break;
			}
			if(s_usbmode_check_pin==0){
				usb_plug_states_flag = 0;
				break;
			}
	   }	
	}
}

void usb_webcam_state(void)
{
	while(1)
 	{
 	#if MCU_VERSION >= GPL326XX	
   		//cmd loop
   		usb_std_service(0);
 	#endif
 		//pollingExit:
   		if (storage_detection(4) == 0)
    		break;
    	if(Usb_Dvice_WebCam_Demo_turn_on_flag){
    		if(s_usbmode_check_pin==1){
				usb_plug_states_flag = 0;
		   		break;
			}
    	}
  	}
}


INT32U usbd_check_lun(DRV_PLUG_STATUS_ST * pStorage)
{
 INT8U change_lun_type =0xff;
 INT8U change_lun_id     =0xff;
 INT16S i,ret;
 //INT32U test;
 struct DrvInfo2 inf;
 if (pStorage->storage_id == STORAGE_DEV_MAX)
 {
   if (pStorage->plug_status == DRV_PLGU_OUT)
   {
      FileSysDrv[USB_Lun_Define[usbd_current_lun].unLunType]->Drv_Uninitial();
      return 1;
   }
 }
 else
 {
  	 if (pStorage->storage_id == STORAGE_DEV_SD)
  	 {
  	   storage_sdms_detection();
  	   if (usb_storage_curent[1] != storage_detection(1))
  	   {
  	     usb_storage_curent[1] = storage_detection(1);
  	     change_lun_type =LUNTYPE_SDCMMC;
  	   }
  	 }  	  
#if (MSC_EN==1)
   	 else if (pStorage->storage_id == STORAGE_DEV_MS)
   	 {
  	   if (usb_storage_curent[2] != storage_detection(2))
  	   {
   	     usb_storage_curent[2] = storage_detection(2);
 	      change_lun_type =LUNTYPE_MS;
  	   }
  	  }
#endif
#if (XD_EN==1)
   	 else if (pStorage->storage_id == STORAGE_DEV_XD)
   	 {
  	   if (usb_storage_curent[6] != storage_detection(6))
  	   {
   	      usb_storage_curent[6] = storage_detection(6);
 	      change_lun_type =LUNTYPE_XD;
  	   }
  	  }
#endif

#if (CFC_EN==1)
  	 else if (pStorage->storage_id == DRV_FS_DEV_CF)
  	 {
  	   storage_cfc_detection();
  	   change_lun_type =LUNTYPE_CF;
  	 }
#endif
#if USB_NUM >= 1
  	 else if (pStorage->storage_id == DRV_FS_DEV_USBH)
  	 {
  	   //When UHOST plug in then STOP usb state aand exit
 	   if (pStorage->plug_status == DRV_PLGU_IN)
 	   {
 	       FileSysDrv[USB_Lun_Define[usbd_current_lun].unLunType]->Drv_Uninitial();
           return 1;
 	   }
  	 }
#endif
     else
  	 {
	   return 0;
  	 }
#if AP_USBD_SDMS_LUN_IN_ONE_LUN == CUSTOM_ON
    if((change_lun_type == LUNTYPE_SDCMMC) || (change_lun_type == LUNTYPE_MS))
    {
         USB_Lun_Define[0].unLunType =change_lun_type;
	 	 change_lun_id =0;
    }
    else
#endif		
    {
     for (i=0 ; i<6; i++)
     {
     	if (USB_Lun_Define[i].unLunType == change_lun_type )
       		change_lun_id = i;
       }
   	 }
     if (pStorage->plug_status == DRV_PLGU_OUT)
     {
   //unmount
     	USB_Lun_Define[change_lun_id].unStatus =LUNSTS_NOMEDIA | LUNSTS_MEDIACHANGE;
     	USB_Lun_Define[change_lun_id].ulSecSize =0;
     	FileSysDrv[USB_Lun_Define[change_lun_id].unLunType]->Drv_Uninitial();
  	 }
   	 else
     {
   //1 unmount current
        FileSysDrv[USB_Lun_Define[usbd_current_lun].unLunType]->Drv_Uninitial();
   //2 mount and check plugin storage
        #if (CFC_EN == 1)
        if (change_lun_type ==LUNTYPE_CF)
        {storages_cfc_reset();}
        #endif
     	ret =FileSysDrv[USB_Lun_Define[change_lun_id].unLunType]->Drv_Initial();
     	if (!ret)
     	{
       		FileSysDrv[USB_Lun_Define[change_lun_id].unLunType]->Drv_GetDrvInfo(&inf);
    		USB_Lun_Define[change_lun_id].ulSecSize =(INT32U)inf.nSectors;
	    	//DBG_PRINT("plug in %x \r\n", inf->nSectors);
       		//USB_Lun_Define[change_lun_id].unStatus =LUNSTS_NORMAL | LUNSTS_MEDIACHANGE;
     	}
     	else
     	{
     	   //DBG_PRINT("plug X %x \r\n",change_lun_id);
       		//USB_Lun_Define[change_lun_id].unStatus =LUNSTS_NOMEDIA ;
       		USB_Lun_Define[change_lun_id].ulSecSize =0;
     	}
     //3 unmount check plugin storage
     	FileSysDrv[USB_Lun_Define[change_lun_id].unLunType]->Drv_Uninitial();
     //4 mount current
        FileSysDrv[USB_Lun_Define[usbd_current_lun].unLunType]->Drv_Initial();
       	if (!ret)
     	{
       		USB_Lun_Define[change_lun_id].unStatus =LUNSTS_NORMAL | LUNSTS_MEDIACHANGE;
     	}
     	else
     	{
       		USB_Lun_Define[change_lun_id].unStatus =LUNSTS_NOMEDIA ;
     	}

   	 }
 }
 return 0;
}

#if 0
//IOF[3] as usb pin
INT32U usbd_pin_detection(void)
{
  return 0;
}
#endif

INT8U         LUN1_OPEN_FLAG = 0;
INT8U         OLD_LUN1__FLAG = 1;
//*
INT32U usbd_wpb_detection(void)
{
	//USB_Lun_Define[usbd_current_lun].unStatus &= ~LUNSTS_WRITEPROTECT;
	USB_Lun_Define[0].unStatus &= ~LUNSTS_WRITEPROTECT;
	//DBG_PRINT("==%d\r\n",usbd_current_lun);
	if(LUN1_OPEN_FLAG == 0){
		USB_Lun_Define[0].unStatus |=  LUNSTS_WRITEPROTECT;
//		USB_Lun_Define[2].unStatus |=  LUNSTS_WRITEPROTECT;
	 }
	 else if(LUN1_OPEN_FLAG == 1)
	 {
		USB_Lun_Define[0].unStatus &=  ~LUNSTS_WRITEPROTECT;
//		USB_Lun_Define[2].unStatus &=  ~LUNSTS_WRITEPROTECT;
	 }
	 return 1;
 //if ( gpio_read_io(C_USB_WP_D_PIN) )
#if _PROJ_TYPE == _PROJ_TURNKEY	
 //Nand flash/ XD/ CFC are by pass WPB
if ( USB_Lun_Define[usbd_current_lun].unLunType != LUNTYPE_SDCMMC)
   return 0;

 if (!storage_wpb_detection())
#else
 if (storage_wpb_detection())
#endif
 {
    USB_Lun_Define[usbd_current_lun].unStatus &= ~LUNSTS_WRITEPROTECT;
	return 0;
 }
 else
 {
	USB_Lun_Define[usbd_current_lun].unStatus |=  LUNSTS_WRITEPROTECT;
    return 1;
  }
}	//*/


INT32U usbd_msdc_lun_change(str_USB_Lun_Info* pNEW_LUN)
{
  INT32U ret ;
  if (pNEW_LUN->unStatus ==LUNSTS_NOMEDIA)
   return 1;
  DBG_PRINT("Lun %x ->%x \r\n", usbd_current_lun, pNEW_LUN->unLunNum);
  //umount current
  FileSysDrv[USB_Lun_Define[usbd_current_lun].unLunType]->Drv_Uninitial();
  //mount new storage
  ret =FileSysDrv[pNEW_LUN->unLunType]->Drv_Initial();
  if(ret)
  {
	   USB_Lun_Define[pNEW_LUN->unLunType].unStatus =LUNSTS_NOMEDIA ;
	   FileSysDrv[pNEW_LUN->unLunType]->Drv_Uninitial();
  }
  return ret;
}


void SetOpenFlag(void)//add by crf
{
	LUN1_OPEN_FLAG = 1;
	CSW_Status = 0;
	CSW_Residue = 0;
	usbd_wpb_detection();

}
void SetCloseFlag(void)//add by crf
{
	LUN1_OPEN_FLAG = 0;
	CSW_Status = 0;
	CSW_Residue = 0;
	usbd_wpb_detection();

}	//*/
//====================================================================================================
//	Description:	This function will be called if PC run to be suspend or device be plug out.
//	Function:		usbdvendor_callback()
//	Syntax:			void usbdvendor_callback(void)
//	Input Paramter:	None
//	Return: 		None
//===================================================================================================
INT32U usbdvendor_callback(void)
{

 	unsigned int   Command_Value;
	INT32U ret = 0;
	if (GetCMDValueEx(29)!='G' || GetCMDValueEx(30)!='P')
	{
		//CommandFail(0x27);
		CommandFail(3);
		return -1;
	}

	Command_Value = (GetCMDValueEx(15) << 8  | GetCMDValueEx(16) );
	DBG_PRINT("\r\nCmd:0x%x ", Command_Value);
	switch (Command_Value )
	{
		case 0xF301:				//Read IC info
			GetLcdResoultion();
			break;
		case 0xF302:
			GetLcdpanel();
			break;
        case 0xF0F0: //easy
        case 0xFFF0: //standard
             SetVenderID();
             break; 
        //case 0xF0FF:
        //	SetSystime();
        //	break;
        case 0xF010:         
             GetICVersion(); 
             break; 			
        case 0xF500:    //Eugene added, for demo SCSI command, 20111222
            DBG_PRINT("usbdvendor_callback , cmd ID 0x%x\r\n", Command_Value);
            GetSendFileFromHost();
            break;
        case 0xFEAA:
        	{
        		INT32U ptr;
        		
        		ptr = get_usb_logger_data();
        		if(ptr)
        		{
					/* Get 512 bytes data then send to host */
        			ret = Send_To_USB_DMA_USB((INT32U*)ptr,1,0);
		    		ret = Send_To_USB_DMA_USB((INT32U*)ptr,1,1);
		    		CSW_Residue = 0;
            		CSW_Status = 0;
        		}	
        	}	
        	break;
 	#if MCU_VERSION <= GPL326XX_C
 	#if (NAND_APP_EN == 1)
        case 0xFF2A:
        case 0xFF29:
        case 0xFF28:
          	 Cmd_GPNandAppUpgrade();   
          	 break;
    #endif
    #endif
		default:
			ret  = 1;
			break;
    }
    return ret;
}
//====================================================================================================
//	Description:	This function will be called if PC run to be suspend or device be plug out.
//	Function:		USBDSuspend_CallBack()
//	Syntax:			void USBSuspend_CallBack(void)
//	Input Paramter:	None
//	Return: 		None
//===================================================================================================
void usbdsuspend_callback(void)
{
	//set R_USB_Suspend=1 will break out usb service loop.
	R_USB_Suspend=1;
	//DBG_PRINT("Debug: got a suspend command.\r\n");
}

//====================================================================================================
//	Description:	This function will be called if PC send resume command to device.
//	Function:		usbdresume_callback()
//	Syntax:			void usbdresume_callback(void)
//	Input Paramter:	None
//	Return: 		None
//===================================================================================================
void usbdresume_callback(void)
{
	//DBG_PRINT("Debug: got a resume command.\r\n");
}

//====================================================================================================
//	Description:	This function will be called if PC send reset command to deivce.
//	Function:		usbdreset_callback()
//	Syntax:			void usbdreset_callback(void)
//	Input Paramter:	None
//	Return: 		None
//===================================================================================================
void usbdreset_callback(void)
{
	//DBG_PRINT("Debug: got a reset command.\r\n");
}

//====================================================================================================
//	Description:	This function will be called if PC send reset command to deivce.
//	Function:		usbset_configuration()
//	Syntax:			void usbset_configuration(void)
//	Input Paramter:	None
//	Return: 		None
//===================================================================================================
void usbset_configuration(void)
{
	//DBG_PRINT("Debug: got a set configuration command.\r\n");
}

//====================================================================================================
//	Description:	This function will be called if PC run to be suspend or device be plug out.
//	Function:		EP0_DataPointer_CallBack()
//	Syntax:			int EP0_DataPointer_CallBack(int nTableType)
//	Input Paramter:	Descriptor address
//	Return: 		1: if you want to redirect R_Descriptor_High and R_Descriptor_Low address.
//					0: use default reply info.
//====================================================================================================
INT32U ep0_datapointer_callback(INT32U nTableType)
{
	return 0;
}

/*
* Function Name :  Read_10
*
* Syntax :INT32S Read_10(str_USB_Lun_Info* lUSB_LUN_Read);
*
* Purpose : Read data from device
*
* Parameters : <IN>  str_USB_Lun_Info is the struct of USB 
*			             
*                             
*                    <OUT> none
* Return : 0: read success
*          	-1: read fail
*
* Note :
*
*/
INT32S Read_10(str_USB_Lun_Info* lUSB_LUN_Read)
{
    INT16U  i=0,j=0,stage =0;
    INT32S  ret;
    INT32U  SCSI_LBA,SCSI_Transfer_Length;
    INT32U  AB_Counts,Ren_Sectors;
    INT16U* USB_RW_Buffer_PTR_A;
    INT16U* USB_RW_Buffer_PTR_B;
    INT16U* USB_RW_Buffer_PTR_Temp;
    INT32U LUN_DMA_SIZE = RWSECTOR_512B;

    if (!BUF) {
   			Sense_Code = 0x1B;
			return -1;
    }
    //Checking curent LUN    
    if (usbd_current_lun != lUSB_LUN_Read->unLunNum)
    {
      if (usbd_msdc_lun_change(lUSB_LUN_Read))
      {
            //DBG_PRINT("LUN FAIL\r\n");
   			Sense_Code = 0x1B;
			return -1;
      }
      usbd_current_lun = lUSB_LUN_Read->unLunNum;
    }
    
    if(lUSB_LUN_Read->unLunType == LUNTYPE_NF || lUSB_LUN_Read->unLunType == LUNTYPE_NF_APP)
    {
    	LUN_DMA_SIZE = READWRITE_SECTOR;
    }
    
    stage =0;
	SCSI_LBA=GetLBA();
	SCSI_Transfer_Length=GetTransLeng();
	USB_RW_Buffer_PTR_A=(INT16U*) BUF ;
	USB_RW_Buffer_PTR_B=USB_RW_Buffer_PTR_A+ (256*LUN_DMA_SIZE);
	if(SCSI_LBA == 0x3ffd)
	SCSI_LBA= 0x3ffd;

	//DBG_PRINT("READ LBA = %x ,len = %x\r\n",SCSI_LBA,SCSI_Transfer_Length);
	if ((SCSI_LBA + SCSI_Transfer_Length)>lUSB_LUN_Read->ulSecSize)
	{
			Sense_Code = 0x1B;		
			return -1;
	}

#if (CDROM_EN == 1)
	if(lUSB_LUN_Read->unLunType == LUNTYPE_CDROM)	//CDROM, do not modify below CDROM code
	{
		SCSI_LBA ++;
		SCSI_LBA = SCSI_LBA << 2;						//1 sector = 4 *512
		//SCSI_LBA += ptr_USB_LUN_Read->ulNandStartAddr;
		SCSI_Transfer_Length = SCSI_Transfer_Length << 2; //length *4
	}

	if(lUSB_LUN_Read->unLunType == LUNTYPE_CDROM)
	{
		for(i=0 ; i < SCSI_Transfer_Length ; i++)
	{
	//	Temp_buf = CDROM_BUF+(SCSI_LBA*512+i*512)/2;
	//	memcpy(USB_RW_Buffer_PTR_A,(INT16U*)Temp_buf,512);
	//	USB_RW_Buffer_PTR_A = CDROM_BUF + (SCSI_LBA*512+i*512)/2;
	//	memcpy(USB_RW_Buffer_PTR_A,(INT16U*)(CDROM_BUF+SCSI_LBA*512+i*512),512);
	
		ret = CDROM_ReadSector(SCSI_LBA+i,1,(INT32U)USB_RW_Buffer_PTR_A);
		ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,LUN_DMA_SIZE,0);
		ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,LUN_DMA_SIZE,1);
	}
	
	}
	else//LUNTYPE_CDROM cdrom
#endif	
	{
	AB_Counts   = SCSI_Transfer_Length / LUN_DMA_SIZE;
	Ren_Sectors = SCSI_Transfer_Length % LUN_DMA_SIZE;	
			
    ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_cmd_phase(SCSI_LBA,SCSI_Transfer_Length);
	if(ret != 0)
	{
  	    //DBG_PRINT("e1\r\n");
    	CSW_Residue=SCSI_Transfer_Length;
		CSW_Residue=CSW_Residue << 9;
		Sense_Code=0x12;
		return -1;
	}
	for(i=0 ; i < AB_Counts ; i++)
	{
            if (i == 0)
              ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_dma_phase((INT32U*)USB_RW_Buffer_PTR_A,1,LUN_DMA_SIZE);
            else
              ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_dma_phase((INT32U*)USB_RW_Buffer_PTR_A,0,LUN_DMA_SIZE);
            //if(ret != 0)  {	DBG_PRINT("e5 %x \r\n",i);break; }
			if (ret != 0)	 break;

            if (i != 0)
              ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_dma_phase(NULL,2,LUN_DMA_SIZE); //wait and check storage DMA
            if (stage)
			{
				ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,LUN_DMA_SIZE,1);
				//DBG_PRINT("Read:0x%08x\r\n", *(INT32U*)USB_RW_Buffer_PTR_A);
				//Check if timeout
				//if(ret != 0)  {	DBG_PRINT("e1\r\n");break; }
				if(ret != 0)  break;
			}
        	//if(ret != 0)  {	DBG_PRINT("e2\r\n");break; }
			if(ret != 0)  break;
  			//if (i == AB_Counts-1)
            //   UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_cmdend_phase();//Storage end

			ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,LUN_DMA_SIZE,0);
		    //if(ret != 0)  {	DBG_PRINT("e3\r\n");break; }
			if(ret != 0)  break;
			stage=1;
			//for last data
			if (i == AB_Counts-1)
			{
				ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,LUN_DMA_SIZE,1);
			    //if(ret != 0)  {	DBG_PRINT("e4\r\n");break; }
				if(ret != 0)  break;
			}
			USB_RW_Buffer_PTR_Temp = USB_RW_Buffer_PTR_B;
			USB_RW_Buffer_PTR_B = USB_RW_Buffer_PTR_A;
			USB_RW_Buffer_PTR_A = USB_RW_Buffer_PTR_Temp;

	}//end for
	
	if( (ret == 0x00) && (Ren_Sectors> 0))
	{
    	  for(j=0 ; j < Ren_Sectors ; j++)
     	  {
            if (j == 0)
              ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_dma_phase((INT32U*)USB_RW_Buffer_PTR_A,1,1);
            else
              ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_dma_phase((INT32U*)USB_RW_Buffer_PTR_A,0,1);
            //if(ret != 0)  {	DBG_PRINT("e5 %x \r\n",i);break; }
			if (ret != 0)	 break;
            if (j != 0)
              ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_dma_phase(NULL,2,1); //wait and check storage DMA
            if (stage)
			{
				ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,1,1);
				//Check if timeout
				//if(ret != 0)  {	DBG_PRINT("e1\r\n");break; }
				if(ret != 0)  break;
			}
        	//if(ret != 0)  {	DBG_PRINT("e2\r\n");break; }
			if(ret != 0)  break;
  			ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,1,0);
		    //if(ret != 0)  {	DBG_PRINT("e3\r\n");break; }
			if(ret != 0)  break;
			stage=1;
			//for last data
			if (j == Ren_Sectors-1)
			{
				ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,1,1);
			    //if(ret != 0)  {	DBG_PRINT("e4\r\n");break; }
				if(ret != 0)  break;
			}
			USB_RW_Buffer_PTR_Temp = USB_RW_Buffer_PTR_B;
			USB_RW_Buffer_PTR_B = USB_RW_Buffer_PTR_A;
			USB_RW_Buffer_PTR_A = USB_RW_Buffer_PTR_Temp;
		 }//end for	
	}
    UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_cmdend_phase();//Storage end
	}//else cdrom

	if(ret != 0x00)
	{
       	    //DBG_PRINT("READ10 %x %x Fail!\r\n",i,SCSI_Transfer_Length);
			CSW_Residue=SCSI_Transfer_Length-(i*LUN_DMA_SIZE)-j;
			CSW_Residue=CSW_Residue << 9;
			Sense_Code = 0x12;
#if (MCU_VERSION == GPL32_B)
            if usbd_check_bi_dma()
            {
                 USBRWError_Flag = 0xfefe;
                 return 1;
            }
#endif
	}
	else
			CSW_Residue=0;
			    		
	return 0;
}

INT32S FlushAppArea_10(str_USB_Lun_Info* lUSB_LUN_Write)
{
	INT32S i, ret;
	i = usbd_current_lun;
	ret = FileSysDrv[USB_Lun_Define[i].unLunType]->Drv_Uninitial();
	
//_USB_ERROR:	
	if(ret != 0x00)
	{
			CSW_Residue=1;
		    Sense_Code=0x10;
            //DBG_PRINT("Write10 %x %x Fail!\r\n",j,SCSI_Transfer_Length);
	}
	else
 		CSW_Residue=0;
	return ret;
}
/*
* Function Name :  Write_10
*
* Syntax :INT32S Write_10(str_USB_Lun_Info* lUSB_LUN_Write);
*
* Purpose : Write data to device
*
* Parameters : <IN>  str_USB_Lun_Info is the struct of USB 
*			             
*                             
*                    <OUT> none
* Return : 0: read success
*          	-1: read fail
*
* Note :
*
*/
INT32S Write_10(str_USB_Lun_Info* lUSB_LUN_Write)
{
   INT32S  i=0,j=0,ret,ret1,stage;
   INT32U  SCSI_LBA,SCSI_Transfer_Length;
   INT32U  AB_Counts,Ren_Sectors;
   INT16U* USB_RW_Buffer_PTR_A;
   INT16U* USB_RW_Buffer_PTR_B;
   INT16U* USB_RW_Buffer_PTR_Temp;
   INT32U LUN_DMA_SIZE = RWSECTOR_512B;

   if (!BUF) {
   			Sense_Code = 0x1B;
			return -1;
    }
   if (usbd_current_lun != lUSB_LUN_Write->unLunNum)
   {
      if (usbd_msdc_lun_change(lUSB_LUN_Write))
      {
   			Sense_Code = 0x1B;
			return -1;
      }
      usbd_current_lun = lUSB_LUN_Write->unLunNum;
   }
   
    if(lUSB_LUN_Write->unLunNum == LUNTYPE_NF || lUSB_LUN_Write->unLunNum == LUNTYPE_NF_APP)
    {
    	LUN_DMA_SIZE = READWRITE_SECTOR;
    }
   
   //if (usbd_wpb_detection())   { }
   stage =0;
   SCSI_LBA=GetLBA();
   SCSI_Transfer_Length=GetTransLeng();
   USB_RW_Buffer_PTR_A=(INT16U*) BUF;
   USB_RW_Buffer_PTR_B=USB_RW_Buffer_PTR_A+ (256*LUN_DMA_SIZE);
   //DBG_PRINT("W10 %x %x\r\n",SCSI_LBA,SCSI_Transfer_Length);
   if ((SCSI_LBA + SCSI_Transfer_Length)>lUSB_LUN_Write->ulSecSize)
   {
            UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_cmdend_phase();
			Sense_Code = 0x1B; //LOGICAL BLOCK ADDRESS OUT OF RANGE		
			return 1;
   }
   
   AB_Counts   = SCSI_Transfer_Length / LUN_DMA_SIZE;
   Ren_Sectors = SCSI_Transfer_Length % LUN_DMA_SIZE;				   
   ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_cmd_phase(SCSI_LBA,SCSI_Transfer_Length);
   if(ret != 0)
   {
   //DBG_PRINT("E1\r\n");
        UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_cmdend_phase();
    	CSW_Residue=SCSI_Transfer_Length;
		CSW_Residue=CSW_Residue << 9;
		Sense_Code=0x10;
		return -1;
   }
   if(AB_Counts)
   {
	   ret=Receive_From_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_B,LUN_DMA_SIZE,0);
	   if(ret != 0){/* 2011-2-17 USB Bug Fix:USB plug out cause data Fail*/
	   		goto _USB_ERROR;
	   }
	   
	   for(i=0; i<AB_Counts; i++)
	   {
				//=================================================================================================
			    if (i != 0)
			       ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_dma_phase(NULL,2,LUN_DMA_SIZE);
	            //if(ret != 0)    DBG_PRINT("E3\r\n");
	            if(ret != 0) break;
			    ret=Receive_From_USB_DMA_USB(NULL,LUN_DMA_SIZE,1);
				if(ret != 0) break;
	            ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_dma_phase((INT32U*)USB_RW_Buffer_PTR_B,0,LUN_DMA_SIZE);
	            //if(ret != 0)    DBG_PRINT("E4\r\n");
	            if(ret != 0) break;
	
				//for last data
				if (i == AB_Counts-1)
				{
			       ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_dma_phase(NULL,2,LUN_DMA_SIZE);
	              //if(ret != 0)    DBG_PRINT("E5\r\n");
			       break;
				}
				USB_RW_Buffer_PTR_Temp = USB_RW_Buffer_PTR_B;
				USB_RW_Buffer_PTR_B = USB_RW_Buffer_PTR_A;
				USB_RW_Buffer_PTR_A = USB_RW_Buffer_PTR_Temp;
			    ret=Receive_From_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_B,LUN_DMA_SIZE,0);
			    if(ret != 0) break;/* 2011-2-17 USB Bug Fix:USB plug out cause data Fail*/
			    
			    //ret1 = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_cmdend_phase();
		}//j
	}
   	if( (ret == 0x00)  && (Ren_Sectors> 0))
	{   
			ret=Receive_From_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_B,1,0);
			 if(ret != 0){/* 2011-2-17 USB Bug Fix:USB plug out cause data Fail*/
	   			goto _USB_ERROR;
	  		 }
		   	for(j=0; j<Ren_Sectors; j++)
			{
			//=================================================================================================
			    if (j != 0)
			       ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_dma_phase(NULL,2,1);
	            //if(ret != 0)    DBG_PRINT("E3\r\n");
        	    if(ret != 0) break;
		    		ret=Receive_From_USB_DMA_USB(NULL,1,1);
				if(ret != 0) break;
			
    	        ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_dma_phase((INT32U*)USB_RW_Buffer_PTR_B,0,1);
        	    //if(ret != 0)    DBG_PRINT("E4\r\n");
            	if(ret != 0) break;

			//for last data
				if (j == Ren_Sectors-1)
				{
			       ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_dma_phase(NULL,2,1);
            		  //if(ret != 0)    DBG_PRINT("E5\r\n");
			       break;
				}
				USB_RW_Buffer_PTR_Temp = USB_RW_Buffer_PTR_B;
				USB_RW_Buffer_PTR_B = USB_RW_Buffer_PTR_A;
				USB_RW_Buffer_PTR_A = USB_RW_Buffer_PTR_Temp;
			    ret=Receive_From_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_B,1,0);
			    if(ret != 0) break;/* 2011-2-17 USB Bug Fix:USB plug out cause data Fail*/
	    	}//j
	        //ret1 = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_cmdend_phase();
	}	
	
_USB_ERROR:	
    ret1 = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_cmdend_phase();
   //if(ret != 0)    DBG_PRINT("E6\r\n");
	if( (ret != 0x00) || (ret1 != 0x00) )
	{
			CSW_Residue=SCSI_Transfer_Length-(i*LUN_DMA_SIZE)-j-1;
			CSW_Residue=CSW_Residue << 9;
		    Sense_Code=0x10;
            //DBG_PRINT("Write10 %x %x Fail!\r\n",j,SCSI_Transfer_Length);
	}
	else
 		CSW_Residue=0;
	return 0;
}



#if MCU_VERSION <= GPL326XX_C
#if (NAND_APP_EN == 1)
//-----------------------Test --------------------------------
INT32S Test_Read_10(str_USB_Lun_Info* lUSB_LUN_Read)
{
    INT16U  i=0,j=0,stage =0;
    INT32S  ret;
    INT32U  SCSI_LBA,SCSI_Transfer_Length;
    INT32U  AB_Counts,Ren_Sectors;
    INT16U* USB_RW_Buffer_PTR_A;
    INT16U* USB_RW_Buffer_PTR_B;
    INT16U* USB_RW_Buffer_PTR_Temp;
    INT32U LUN_DMA_SIZE = RWSECTOR_512B;

    if (!BUF) {
   			Sense_Code = 0x1B;
			return -1;
    }
    //Checking curent LUN    
    if (usbd_current_lun != lUSB_LUN_Read->unLunNum)
    {
      if (usbd_msdc_lun_change(lUSB_LUN_Read))
      {
            //DBG_PRINT("LUN FAIL\r\n");
   			Sense_Code = 0x1B;
			return -1;
      }
      usbd_current_lun = lUSB_LUN_Read->unLunNum;
    }
    
    if(lUSB_LUN_Read->unLunType == LUNTYPE_NF || lUSB_LUN_Read->unLunType == LUNTYPE_NF_APP)
    {
    	LUN_DMA_SIZE = READWRITE_SECTOR;
    }
    
    stage =0;
	SCSI_LBA=0;
	SCSI_Transfer_Length=8;
	USB_RW_Buffer_PTR_A=(INT16U*) BUF ;
	USB_RW_Buffer_PTR_B=USB_RW_Buffer_PTR_A+ (256*LUN_DMA_SIZE);
	if(SCSI_LBA == 0x3ffd)
	SCSI_LBA= 0x3ffd;

	//DBG_PRINT("READ LBA = %x ,len = %x\r\n",SCSI_LBA,SCSI_Transfer_Length);
	if ((SCSI_LBA + SCSI_Transfer_Length)>lUSB_LUN_Read->ulSecSize)
	{
			Sense_Code = 0x1B;		
			return -1;
	}
	
	{
	AB_Counts   = SCSI_Transfer_Length / LUN_DMA_SIZE;
	Ren_Sectors = SCSI_Transfer_Length % LUN_DMA_SIZE;	
			
    ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_cmd_phase(SCSI_LBA,SCSI_Transfer_Length);
	if(ret != 0)
	{
  	    //DBG_PRINT("e1\r\n");
    	CSW_Residue=SCSI_Transfer_Length;
		CSW_Residue=CSW_Residue << 9;
		Sense_Code=0x12;
		return -1;
	}
	for(i=0 ; i < AB_Counts ; i++)
	{
            if (i == 0)
              ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_dma_phase((INT32U*)USB_RW_Buffer_PTR_A,1,LUN_DMA_SIZE);
            else
              ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_dma_phase((INT32U*)USB_RW_Buffer_PTR_A,0,LUN_DMA_SIZE);
            //if(ret != 0)  {	DBG_PRINT("e5 %x \r\n",i);break; }
			if (ret != 0)	 break;

            if (i != 0)
              ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_dma_phase(NULL,2,LUN_DMA_SIZE); //wait and check storage DMA
            if (stage)
			{
				ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,LUN_DMA_SIZE,1);
				DBG_PRINT("Read:0x%08x\r\n", *(INT32U*)USB_RW_Buffer_PTR_A);
				//Check if timeout
				//if(ret != 0)  {	DBG_PRINT("e1\r\n");break; }
				if(ret != 0)  break;
			}
        	//if(ret != 0)  {	DBG_PRINT("e2\r\n");break; }
			if(ret != 0)  break;
  			//if (i == AB_Counts-1)
            //   UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_cmdend_phase();//Storage end

			ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,LUN_DMA_SIZE,0);
		    //if(ret != 0)  {	DBG_PRINT("e3\r\n");break; }
			if(ret != 0)  break;
			stage=1;
			//for last data
			if (i == AB_Counts-1)
			{
				ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,LUN_DMA_SIZE,1);
			    //if(ret != 0)  {	DBG_PRINT("e4\r\n");break; }
				if(ret != 0)  break;
			}
			USB_RW_Buffer_PTR_Temp = USB_RW_Buffer_PTR_B;
			USB_RW_Buffer_PTR_B = USB_RW_Buffer_PTR_A;
			USB_RW_Buffer_PTR_A = USB_RW_Buffer_PTR_Temp;

	}//end for
	
	if( (ret == 0x00) && (Ren_Sectors> 0))
	{
    	  for(j=0 ; j < Ren_Sectors ; j++)
     	  {
            if (j == 0)
              ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_dma_phase((INT32U*)USB_RW_Buffer_PTR_A,1,1);
            else
              ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_dma_phase((INT32U*)USB_RW_Buffer_PTR_A,0,1);
            //if(ret != 0)  {	DBG_PRINT("e5 %x \r\n",i);break; }
			if (ret != 0)	 break;
            if (j != 0)
              ret = UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_dma_phase(NULL,2,1); //wait and check storage DMA
            if (stage)
			{
				ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,1,1);
				//Check if timeout
				//if(ret != 0)  {	DBG_PRINT("e1\r\n");break; }
				if(ret != 0)  break;
			}
        	//if(ret != 0)  {	DBG_PRINT("e2\r\n");break; }
			if(ret != 0)  break;
  			ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,1,0);
		    //if(ret != 0)  {	DBG_PRINT("e3\r\n");break; }
			if(ret != 0)  break;
			stage=1;
			//for last data
			if (j == Ren_Sectors-1)
			{
				ret = Send_To_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_A,1,1);
			    //if(ret != 0)  {	DBG_PRINT("e4\r\n");break; }
				if(ret != 0)  break;
			}
			USB_RW_Buffer_PTR_Temp = USB_RW_Buffer_PTR_B;
			USB_RW_Buffer_PTR_B = USB_RW_Buffer_PTR_A;
			USB_RW_Buffer_PTR_A = USB_RW_Buffer_PTR_Temp;
		 }//end for	
	}
    UsbReadWrite[lUSB_LUN_Read->unLunType]->usdc_read_cmdend_phase();//Storage end
	}//else cdrom

	if(ret != 0x00)
	{
       	    //DBG_PRINT("READ10 %x %x Fail!\r\n",i,SCSI_Transfer_Length);
			CSW_Residue=SCSI_Transfer_Length-(i*LUN_DMA_SIZE)-j;
			CSW_Residue=CSW_Residue << 9;
			Sense_Code = 0x12;
#if (MCU_VERSION == GPL32_B)
            if usbd_check_bi_dma()
            {
                 USBRWError_Flag = 0xfefe;
                 return 1;
            }
#endif
	}
	else
			CSW_Residue=0;
			    		
	return 0;
}


INT32S Test_Write_10(str_USB_Lun_Info* lUSB_LUN_Write)
{
   INT32S  i=0,j=0,ret,ret1,stage;
   INT32U  SCSI_LBA,SCSI_Transfer_Length;
   INT32U  AB_Counts,Ren_Sectors;
   INT16U* USB_RW_Buffer_PTR_A;
   INT16U* USB_RW_Buffer_PTR_B;
   INT16U* USB_RW_Buffer_PTR_Temp;
   INT32U  LUN_DMA_SIZE = RWSECTOR_512B;

   if (!BUF) {
   			Sense_Code = 0x1B;
			return -1;
    }
   if (usbd_current_lun != lUSB_LUN_Write->unLunNum)
   {
      if (usbd_msdc_lun_change(lUSB_LUN_Write))
      {
   			Sense_Code = 0x1B;
			return -1;
      }
      usbd_current_lun = lUSB_LUN_Write->unLunNum;
   }
   
    if(lUSB_LUN_Write->unLunNum == LUNTYPE_NF || lUSB_LUN_Write->unLunNum == LUNTYPE_NF_APP)
    {
    	LUN_DMA_SIZE = READWRITE_SECTOR;
    }
   
   //if (usbd_wpb_detection())   { }
   stage =0;
   SCSI_LBA=0;
   SCSI_Transfer_Length=16;
   USB_RW_Buffer_PTR_A=(INT16U*) BUF;
   USB_RW_Buffer_PTR_B=USB_RW_Buffer_PTR_A+ (256*LUN_DMA_SIZE);
   //DBG_PRINT("W10 %x %x\r\n",SCSI_LBA,SCSI_Transfer_Length);
   if ((SCSI_LBA + SCSI_Transfer_Length)>lUSB_LUN_Write->ulSecSize)
   {
            UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_cmdend_phase();
			Sense_Code = 0x1B; //LOGICAL BLOCK ADDRESS OUT OF RANGE		
			return 1;
   }
   
   AB_Counts   = SCSI_Transfer_Length / LUN_DMA_SIZE;
   Ren_Sectors = SCSI_Transfer_Length % LUN_DMA_SIZE;				   
   ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_cmd_phase(SCSI_LBA,SCSI_Transfer_Length);
   if(ret != 0)
   {
   //DBG_PRINT("E1\r\n");
        UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_cmdend_phase();
    	CSW_Residue=SCSI_Transfer_Length;
		CSW_Residue=CSW_Residue << 9;
		Sense_Code=0x10;
		return -1;
   }
   if(AB_Counts)
   {
	   //ret=Receive_From_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_B,LUN_DMA_SIZE,0);
	   
	 
	   		
	   if(ret != 0){/* 2011-2-17 USB Bug Fix:USB plug out cause data Fail*/
	   		goto _USB_ERROR;
	   }
	   
	   for(i=0; i<AB_Counts; i++)
	   {
				//=================================================================================================
			    if (i != 0)
			       ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_dma_phase(NULL,2,LUN_DMA_SIZE);
	            //if(ret != 0)    DBG_PRINT("E3\r\n");
	            if(ret != 0) break;
			    //ret=Receive_From_USB_DMA_USB(NULL,LUN_DMA_SIZE,1);
				//if(ret != 0) break;
				 for(j = 0; j < 256; j++)
	   				USB_RW_Buffer_PTR_B[j] = j;
	   				
				DBG_PRINT("Write:0x%08x\r\n", *(INT32U*)USB_RW_Buffer_PTR_B);
	            ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_dma_phase((INT32U*)USB_RW_Buffer_PTR_B,0,LUN_DMA_SIZE);
	            //if(ret != 0)    DBG_PRINT("E4\r\n");
	            if(ret != 0) break;
	
				//for last data
				if (i == AB_Counts-1)
				{
			       ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_dma_phase(NULL,2,LUN_DMA_SIZE);
	              //if(ret != 0)    DBG_PRINT("E5\r\n");
			       break;
				}
				USB_RW_Buffer_PTR_Temp = USB_RW_Buffer_PTR_B;
				USB_RW_Buffer_PTR_B = USB_RW_Buffer_PTR_A;
				USB_RW_Buffer_PTR_A = USB_RW_Buffer_PTR_Temp;
			    //ret=Receive_From_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_B,LUN_DMA_SIZE,0);
			    //if(ret != 0) break;/* 2011-2-17 USB Bug Fix:USB plug out cause data Fail*/
			    
			    //ret1 = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_cmdend_phase();
		}//j
	}
   	if( (ret == 0x00)  && (Ren_Sectors> 0))
	{   
			ret=Receive_From_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_B,1,0);
			 if(ret != 0){/* 2011-2-17 USB Bug Fix:USB plug out cause data Fail*/
	   			goto _USB_ERROR;
	  		 }
		   	for(j=0; j<Ren_Sectors; j++)
			{
			//=================================================================================================
			    if (j != 0)
			       ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_dma_phase(NULL,2,1);
	            //if(ret != 0)    DBG_PRINT("E3\r\n");
        	    if(ret != 0) break;
		    		ret=Receive_From_USB_DMA_USB(NULL,1,1);
				if(ret != 0) break;
			
    	        ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_dma_phase((INT32U*)USB_RW_Buffer_PTR_B,0,1);
        	    //if(ret != 0)    DBG_PRINT("E4\r\n");
            	if(ret != 0) break;

			//for last data
				if (j == Ren_Sectors-1)
				{
			       ret = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_dma_phase(NULL,2,1);
            		  //if(ret != 0)    DBG_PRINT("E5\r\n");
			       break;
				}
				USB_RW_Buffer_PTR_Temp = USB_RW_Buffer_PTR_B;
				USB_RW_Buffer_PTR_B = USB_RW_Buffer_PTR_A;
				USB_RW_Buffer_PTR_A = USB_RW_Buffer_PTR_Temp;
			    ret=Receive_From_USB_DMA_USB((INT32U*)USB_RW_Buffer_PTR_B,1,0);
			    if(ret != 0) break;/* 2011-2-17 USB Bug Fix:USB plug out cause data Fail*/
	    	}//j
	        //ret1 = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_cmdend_phase();
	}	
	
_USB_ERROR:	
    ret1 = UsbReadWrite[lUSB_LUN_Write->unLunType]->usdc_write_cmdend_phase();
   //if(ret != 0)    DBG_PRINT("E6\r\n");
	if( (ret != 0x00) || (ret1 != 0x00) )
	{
			CSW_Residue=SCSI_Transfer_Length-(i*LUN_DMA_SIZE)-j-1;
			CSW_Residue=CSW_Residue << 9;
		    Sense_Code=0x10;
            //DBG_PRINT("Write10 %x %x Fail!\r\n",j,SCSI_Transfer_Length);
	}
	else
 		CSW_Residue=0;
	return 0;
}

//-----------------------End Test ----------------------------

#endif  
#endif   



/*************************************************************/
//USB
//
//
//***********************************************************/
INT16S Test_USB(void) //judge USB Device
{
	unsigned long R_TimeOutCount=0;
	while(1)
	{	

	if(R_USB_State_Machine)
		R_USB_Suspend = 1;	
	
		if(R_TimeOutCount >0x500000)//not usb
		{		
			return -1;
		}

		if(usbd_dected_io()==0)//plug out
		{		
			return -1;
		}
		
		if(R_USB_Suspend == 1)//it is usb plug in
		{
			return 0;
		}
		
		R_TimeOutCount++;	

		
	}
}

