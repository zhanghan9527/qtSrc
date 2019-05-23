#ifndef __COMWIRE_LITE_H__
#define __COMWIRE_LITE_H__

#include "gplib.h"

/**
 * @brief   get comwrie require pcm buffer size for encode.
 * @param   CmdNumber[in]: command number
 * @return 	size in byte
 */
extern INT32U ComWrieLite_Enc_Get_ReqSize(INT32U CmdNumber);

/**
 * @brief   comwrie encode as pcm data in sample rate as 48khz, mono.
 * @param   pPcmBuf[in]: pcm buffer
 * @param   pCmd[in]: command table
 * @param   CmdNumber[in]: command number
 * @return 	> 0 is encode size in short, < 0 is fail. 
 */
extern INT32S ComWrieLite_Enc_Cmd(INT16S *pPcmBuf, INT8U *pCmd, INT32U CmdNumber);

/**
 * @brief   comwrie transfer encode pcm data, use dac.
 * @param   pPcmBuf[in]: pcm buffer
 * @param   cwlen[in]: pcm length in short
 * @return 	>= 0 is success, < 0 is fail. 
 */
extern INT32S ComWrieLite_Enc_Trans(INT16S *pPcmBuf, INT32U cwlen);

/**
 * @brief   comwrie decode reset.
 * @param   none
 * @return 	none  
 */ 
extern void ComWrieLite_Dec_Reset(void);

/**
 * @brief   comwrie decode pcm data.
 * @param   pPcmBuf[in]: pcm buffer
 * @param   pCmd[in]: command table
 * @param   pCmd[in]: decode out command table
 * @return 	decode command number  
 */ 
extern INT32S ComWrieLite_Dec_Cmd(INT16S *pPcmBuf, INT32U cwlen, INT8U *pCmd);
#endif