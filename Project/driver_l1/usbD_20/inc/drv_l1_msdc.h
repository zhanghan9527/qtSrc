//#include "boot.h"
#include "driver_l1.h"
#include "drv_l1_sfr.h"

//MSDC
extern INT32U   Sense_Code;

extern INT32U   Bulk_Stall;



#define R_Timeout_count 0xf888;

typedef struct {
	unsigned int ScsiCmdCode;
	void	(*function)(void);
} SCSI_CMD_MAP;



////////////////////////str_USB_Lun_Info->unStatus//////////////////////////////////
#define LUNSTS_NORMAL			0x0000
#define LUNSTS_NOMEDIA			0x0001
#define LUNSTS_WRITEPROTECT		0x0002
#define LUNSTS_MEDIACHANGE		0x0004


/*
* Function Name :  GetLBA
*
* Syntax : INT32U GetLBA(void);
*
* Purpose :  User can call this function to get the LBA from CBW package
*
* Parameters : <IN>  none                        
*                             
*              <OUT> none
* Return : LBA of CSW package
*           
*
* Note :
*
*/
extern INT32U GetLBA(void);

/*
* Function Name :  GetTransLeng
*
* Syntax : INT32U GetTransLeng(void);
*
* Purpose :  User can call this function to get the transfer length defined in CBW package
*
* Parameters : <IN>  none                        
*                             
*              <OUT> none
* Return : Length of CBW package
*           
*
* Note :
*
*/
extern  INT32U GetTransLeng(void);


/*
* Function Name :  GetICVersion
*
* Syntax : INT32U GetICVersion(void);
*
* Purpose :  Internal use only,Get IC Version
*
* Parameters : <IN>    none                        
*                             
*              <OUT> none
* Return : none
*           
*
* Note :
*
*/
extern void GetICVersion(void);

/*
* Function Name :  GetDriveStatus
*
* Syntax : void GetDriveStatus(void);
*
* Purpose :  Internal use only,Get Drive Status
*
* Parameters : <IN>  none                        
*                             
*              <OUT> none
* Return : none
*           
*
* Note :
*
*/
extern void GetDriveStatus(void);

/*
* Function Name :  SetVenderID
*
* Syntax : void SetVenderID(void);
*
* Purpose :  Internal use only,Set Vender ID
*
* Parameters : <IN>  none                        
*                             
*              <OUT> none
* Return : none
*           
*
* Note :
*
*/
extern void SetVenderID(void);

/*
* Function Name :  CommandFail
*
* Syntax : void CommandFail(INT32U nSenseIndex);
*
* Purpose :  User can call this function to handle when a SCSI command is not supported or fail
*
* Parameters : <IN>  nSenseIndex :sense code index                   
*                             
*              <OUT> none
* Return : none
*           
*
* Note :
*
*/
extern void CommandFail(INT32U nSenseIndex);

/*
* Function Name :  GetCMDValueEx
*
* Syntax :INT32U GetCMDValueEx(INT32U index );
*
* Purpose :  User can call this function to get the CMD value from CBW package
*
* Parameters : <IN>  index:get which one the CMD value from CBW package(index:0~30)                
*                             
*              <OUT> none
* Return : none
*           
*
* Note :
*
*/
extern INT32U GetCMDValueEx(INT32U index );

/*
* Function Name :  GotoExRam
*
* Syntax :void GotoExRam(INT32U LBA);
*
* Purpose :  User can call this function to go to ExRam address running code
*
* Parameters : <IN>  LBA:ExRam address             
*                             
*              <OUT> none
* Return : none
*           
*
* Note :
*
*/
extern void GotoExRam(INT32U LBA);


/*
* Function Name :  CallExRom
*
* Syntax : void CallExRom (INT32U LBA);
*
* Purpose :  User can call this function to call to ExRom address running code
*
* Parameters : <IN>  LBA:ExRom address            
*                             
*              <OUT> none
* Return : none
*           
*
* Note :
*
*/
extern void CallExRom (INT32U LBA);

/*
* Function Name :  RegRead
*
* Syntax : void RegRead(INT32U REG,INT32U Count);
*
* Purpose :  User can call this function to Read  Reg data to host
*
* Parameters : <IN>  REG:register address,Count:lenth of register data    
*                             
*              <OUT> none
* Return : none
*           
*
* Note :
*
*/
extern void RegRead(INT32U REG,INT32U Count);

/*
* Function Name :  RegWrite
*
* Syntax : void RegWrite(INT32U REG ,INT32U Count);
*
* Purpose :  User can call this function to Write  Reg data to device
*
* Parameters : <IN>     REG:register address,Count:lenth of register data
*                             
*              <OUT> none
* Return : none
*           
*
* Note :
*
*/
extern void RegWrite(INT32U REG ,INT32U Count);

extern  INT32U   CSW_Residue;
extern  INT32U   CSW_Status;





