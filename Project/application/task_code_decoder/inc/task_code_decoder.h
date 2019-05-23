#ifndef __TASK_CODE_DECODER__
#define __TASK_CODE_DECODER__

#include "application.h"
#include "define.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it =====================//
#if (APP_QRCODE_BARCODE_EN == 1)
//==================================================================================//
/****************************************************
*		define										*
****************************************************/
#define CODE_DEC_WIDTH			640
#define CODE_DEC_HEIGHT			480

#define QRCODE_DEC_FMT 			IMG_FMT_GRAY 	//IMG_FMT_GRAY, IMG_FMT_UYVY
#define QRCODE_Y_SWAP			0				//0:disable, 1:enable
#define QRCODE_CLR_INVRESE_EN	0 				//0:disable, 1:enable

#define BARCODE_DEC_FMT			IMG_FMT_UYVY

/****************************************************
*		macro										*
****************************************************/
#ifndef DEBUG_MSG
#if DBG_MESSAGE
	#define DEBUG_MSG	DBG_PRINT
#else
	#define DEBUG_MSG(...)
#endif
#endif

#ifndef RETURN
#define RETURN(x)	{nRet = x; goto Return;}
#endif

#ifndef POST_MESSAGE
#define POST_MESSAGE(msg_queue, message, ack_mbox, msc_time, msg, err)\
{\
	msg = (INT32S) OSMboxAccept(ack_mbox);\
	err = OSQPost(msg_queue, (void *)message);\
	if(err != OS_NO_ERR)\
	{\
		DEBUG_MSG("OSQPost Fail!!!\r\n");\
		RETURN(STATUS_FAIL);\
	}\
	msg = (INT32S) OSMboxPend(ack_mbox, msc_time/10, &err);\
	if(err != OS_NO_ERR || msg == C_ACK_FAIL)\
	{\
		DEBUG_MSG("OSMbox ack Fail!!!\r\n");\
		RETURN(STATUS_FAIL);\
	}\
}
#endif

#define C_CODE_DEC_DISABLE	0x00
#define C_CODE_DEC_ENABLE	(1 << 0)
#define C_CODE_DEC_BUSY		(1 << 1)
#define C_CODE_DEC_ALL		(C_CODE_DEC_ENABLE | C_CODE_DEC_BUSY)

/****************************************************
*		data type									*
****************************************************/
typedef enum
{
	MSG_QRCODE_DEC_INIT = 0x1000,
	MSG_BARCODE_DEC_INIT,
	MSG_CODE_DEC_UNINIT,
	MSG_CODE_DEC_EXIT 
} MSG_CODE_DEC_ENUM;

typedef enum
{
	QR_CODE = 0,
	BAR_CODE,
	CODE_MAX
} CODE_TYPE;

typedef struct
{
	INT32U id[32];
	INT8U  name[32];
	INT8U  stream[32];
}BarCodeResult_t;

/****************************************************
*		function									*
****************************************************/
//========================================================
// Function Name :  code_decoder_task_create
// Syntax : 
// Purpose :  code decoder task create
// Parameters : INT32U prio: priority
// Return : >=0: success, <0: fail
//========================================================
INT32S code_decoder_task_create(INT32U prio);

//========================================================
// Function Name :  code_decoder_task_del
// Syntax : 
// Purpose :  code decoder task delete
// Parameters : none
// Return : >=0: success, <0: fail
//========================================================
INT32S code_decoder_task_del(void);

//========================================================
// Function Name :  code_decoder_init
// Syntax : 
// Purpose :  code decoder initialize
// Parameters : CODE_TYPE type: code decode type
// Return : >=0: success, <0: fail
//========================================================
INT32S code_decoder_init(CODE_TYPE type);

//========================================================
// Function Name :  code_decoder_uninit
// Syntax : 
// Purpose :  code decoder un-initialize
// Parameters : none
// Return : >=0: success, <0: fail
//========================================================
INT32S code_decoder_uninit(void);

//========================================================
// Function Name :  code_decoder_set_enable
// Syntax : 
// Purpose :  code decoder set enable, it will clear when decode finish
// Parameters : none
// Return : none
//========================================================
void code_decoder_set_enable(void);

//========================================================
// Function Name :  code_decoder_get_status
// Syntax : 
// Purpose :  code decoder get status
// Parameters : none
// Return : status
//========================================================
INT32U code_decoder_get_status(void);

//========================================================
// Function Name :  code_decoder_set_frame
// Syntax : 
// Purpose :  code decoder post decode buffer 
// Parameters : INT32U frame_buffer: input buffer
//				INT16U w: buffer width
//				INT16U h: buffer height
//				INT32U infmt: buffer format
// Return : >=0: success, <0: fail
//========================================================
INT32S code_decoder_set_frame(INT32U frame_buffer, INT16U w, INT16U h, INT32U infmt);

//========================================================
// Function Name :  code_decoder_wait_frame_finish
// Syntax : 
// Purpose :  code decoder wait buffer ready
// Parameters : INT32U wait_done: 1 is wait done, 0 polling
// Return : >0: success, <0: fail, =0: not ready
//========================================================
INT32S code_decoder_wait_finish(INT32U wait_done);

//========================================================
// Function Name :  QRCode_get_Result
// Syntax : 
// Purpose :  get qr code decode result 
// Parameters : nont
// Return : result
//========================================================
INT8U *QRCode_get_Result(void);

//========================================================
// Function Name :  BarCode_get_Result
// Syntax : 
// Purpose :  get bar code decode result 
// Parameters : INT32S *Number: resukt number
// Return : result 
//========================================================
BarCodeResult_t *BarCode_get_Result(INT32S *Number);
#endif
#endif