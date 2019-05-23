#include "application.h"
#include "task_code_decoder.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it =====================//
#if (APP_QRCODE_BARCODE_EN == 1)
//==================================================================================//
#include "QRCode.h"
#include "gpbarcode_api.h"

/****************************************************
*		define										*
****************************************************/
#define C_ACK_SUCCESS			0x00000001
#define C_ACK_FAIL				0x80000000
#define C_TASK_STACK_SIZE		256
#define C_QUEUE_MAX				1

/****************************************************
*		data type									*
****************************************************/
typedef struct
{
	gpImage ScaleIn;
	gpImage ScaleOut;
} ObjFrame_t;

typedef struct 
{
	INT32U CodeDecTaskStack[C_TASK_STACK_SIZE];
	void *CodeDec_q_stack[C_QUEUE_MAX];
	void *CodeDec_finish_q_stack[C_QUEUE_MAX];
	OS_EVENT *CodeDec_q; 
	OS_EVENT *CodeDec_finish_q;  
	OS_EVENT *CodeDec_ack_m;
	
	INT32S (*process)(void *Workmem, ObjFrame_t *pInput);
	void *WorkMem;
	INT32U mem_size;
	INT8U status_flag;
	INT8U decode_type;
	INT8U image_type;
	INT8U idx;
	ObjFrame_t Obj[C_QUEUE_MAX];
	
	// result
	INT8U QRCodeStream[1024];
	INT32U BarCodeNumber;
	BarCodeResult_t BarCodestream;
} TaskCodeDec_t;

/****************************************************
*		extnal func									*
****************************************************/
extern INT32S scaler_once(INT8U wait_done, gpImage *src, gpImage *dst);
extern INT32S scaler_wait_done(void);
static void code_decoder_task_entry(void *para);

/****************************************************
*		varaible									*
****************************************************/
static const char bar_code_name[][16] = 
{
	"ID Err",
	"CODE128"
};

static TaskCodeDec_t *pTaskCode, gTaskCode;


static INT32S code_decoder_malloc(TaskCodeDec_t *pTCode)
{
	INT32U i, size;
	
	for(i=0; i<C_QUEUE_MAX; i++) {
		size = pTCode->Obj[i].ScaleOut.width * pTCode->Obj[i].ScaleOut.height;
		if(pTCode->Obj[i].ScaleOut.format != IMG_FMT_GRAY) {
			size <<= 1;
		}
		
		pTCode->Obj[i].ScaleOut.ptr = (INT8U *)gp_malloc_align(size, 32);
		if(pTCode->Obj[i].ScaleOut.ptr == 0) {
			return STATUS_FAIL;
		}
		DEBUG_MSG("ScaleOutBuf[%d] = 0x%x\r\n", i, pTCode->Obj[i].ScaleOut.ptr);
	}
	
	return STATUS_OK;
}

static void code_decoder_free(TaskCodeDec_t *pTCode)
{
	INT32U i;
	
	for(i=0; i<C_QUEUE_MAX; i++) {
		if(pTCode->Obj[i].ScaleOut.ptr) {
			gp_free((void *)pTCode->Obj[i].ScaleOut.ptr);
			pTCode->Obj[i].ScaleOut.ptr = 0;
		}
	}
	
	if(pTCode->WorkMem) {
		gp_free((void *)pTCode->WorkMem);
		pTCode->WorkMem = 0;
	}
}

static INT32S QR_Code_Proc(void *Workmem, ObjFrame_t *pInput) 
{
	INT8U *res;
	INT32S nRet;
	INT32U t1, t2;
	
	nRet = QR_Init(Workmem,
				pInput->ScaleOut.width,
				pInput->ScaleOut.height,
				pInput->ScaleOut.ptr, 
				pTaskCode->image_type, 
				pTaskCode->QRCodeStream);

	t1 = OSTimeGet();
	res =(INT8U*) QR_Decode(Workmem);
	t2 = OSTimeGet();
	DBG_PRINT("QRCode Decoding Time: %d ms \r\n", (t2-t1)*10);
	if(res == NULL) {
		DBG_PRINT("QRCode Decode NG! \r\n");
		DBG_PRINT("ErrorN0 = %d\r\n", QR_GetErrorNum(Workmem));
		return -1;
	} else {
		DEBUG_MSG("QRCode Decode OK! \r\n");
		DEBUG_MSG("QRCode Result: %s\r\n", pTaskCode->QRCodeStream);
	}
	
	return 0;
}

static CHAR *Bar_Code_get_dec_name(GPBARCODE_DEC_ID id)
{
	switch (id)
	{
	case GPBARCODE_NONE:
		return (CHAR *)(bar_code_name[0]);
	case GPBARCODE_CODE128:
		return (CHAR *)(bar_code_name[1]);
	default:
		return (CHAR *)(bar_code_name[0]);
	}

	return (CHAR *)(bar_code_name[0]);
}

static INT32S Bar_Code_Proc(void *Workmem, ObjFrame_t *pInput) 
{
	INT32S i;
	INT32U t1, t2;
	
	t1 = OSTimeGet();
	pTaskCode->BarCodeNumber = gpbarcode_dec(Workmem, &pInput->ScaleOut);
	t2 = OSTimeGet();
	DBG_PRINT("BARCode Decoding Time: %d ms \r\n", (t2-t1)*10);
	
	for(i=0; i<pTaskCode->BarCodeNumber; i++) {
		GPBARCODE_DEC_ID id = gpbarcode_get_decoder_id(Workmem, i);
		
		pTaskCode->BarCodestream.id[i] = id;
		gp_strcpy((INT8S *)pTaskCode->BarCodestream.name[i], (INT8S *)Bar_Code_get_dec_name(id));
		gp_strcpy((INT8S *)pTaskCode->BarCodestream.stream[i], (INT8S *)gpbarcode_get_data(Workmem, i));
		DEBUG_MSG("id = %d, [%s]: \" %s \" \r\n", id, Bar_Code_get_dec_name(id), gpbarcode_get_data(Workmem, i));				
	}
	
	return 0;
}

static INT32S code_initial(TaskCodeDec_t *pTCode)
{
	INT32S i, nRet;
	
	switch(pTCode->decode_type)
	{
	case QR_CODE:
		pTCode->mem_size = QR_GetWorkingMemorySize();
		pTCode->WorkMem = gp_malloc_align(pTCode->mem_size, 16);
		if(pTCode->WorkMem == 0) {
			RETURN(STATUS_FAIL);
		}
		
		DEBUG_MSG("QR_CODE_Workmem = 0x%x\r\n", pTCode->WorkMem);
		gp_memset(pTCode->WorkMem, 0x00, pTCode->mem_size);
		pTCode->process = QR_Code_Proc;
		
		for(i=0; i<C_QUEUE_MAX; i++) {
			pTCode->Obj[i].ScaleOut.width = CODE_DEC_WIDTH;
			pTCode->Obj[i].ScaleOut.height = CODE_DEC_HEIGHT;
		#if QRCODE_DEC_FMT == IMG_FMT_GRAY
			pTCode->Obj[i].ScaleOut.widthStep = CODE_DEC_WIDTH;
			pTCode->Obj[i].ScaleOut.ch = 1;
			pTCode->Obj[i].ScaleOut.format = IMG_FMT_GRAY;
		#else
			pTCode->Obj[i].ScaleOut.widthStep = CODE_DEC_WIDTH * 2;
			pTCode->Obj[i].ScaleOut.ch = 2;
			pTCode->Obj[i].ScaleOut.format = IMG_FMT_UYVY;
		#endif
		}
		
	#if QRCODE_DEC_FMT != IMG_FMT_GRAY 
		//YUYV
		pTCode->image_type = IMAGETYPE_YUYV; 
	#else
		//Y only
		#if QRCODE_Y_SWAP == 1 
		pTCode->image_type = IMAGETYPE_YY_SWAP; 
		#else
		pTCode->image_type = IMAGETYPE_YY_SEQUENTIAL;
		#endif
	#endif
	
	#if QRCODE_CLR_INVRESE_EN == 1
		pTCode->image_type |= IMAGETYPE_COLORINVERSE;
	#endif
		break;
		
	case BAR_CODE:
		pTCode->mem_size = gpbarcode_dec_get_mem_size();
		pTCode->WorkMem = gp_malloc_align(pTCode->mem_size, 16);
		if(pTCode->WorkMem == 0) {
			RETURN(STATUS_FAIL);
		}
		
		DEBUG_MSG("QR_CODE_Workmem = 0x%x\r\n", pTCode->WorkMem);
		gp_memset(pTCode->WorkMem, 0x00, pTCode->mem_size);
		gpbarcode_dec_init(pTCode->WorkMem);
		gpbarcode_dec_set_step(pTCode->WorkMem, 1, 2);
		pTCode->process = Bar_Code_Proc;
		
		for(i=0; i<C_QUEUE_MAX; i++) {
			pTCode->Obj[i].ScaleOut.width = CODE_DEC_WIDTH;
			pTCode->Obj[i].ScaleOut.height = CODE_DEC_HEIGHT;
			pTCode->Obj[i].ScaleOut.widthStep = CODE_DEC_WIDTH * 2;
			pTCode->Obj[i].ScaleOut.ch = 2;
			pTCode->Obj[i].ScaleOut.format = BARCODE_DEC_FMT;
		}
		break;
		
	default:
		RETURN(STATUS_FAIL);
	}
	
	if(code_decoder_malloc(pTCode) < 0) {
		RETURN(STATUS_FAIL);
	}
	
	nRet = STATUS_OK;
Return:
	if(nRet < 0) {
		code_decoder_free(pTCode);
	}
	
	return nRet;
}

INT32S code_decoder_task_create(INT32U prio)
{
	INT8U err;
	
	pTaskCode =&gTaskCode;
	gp_memset((INT8S *)pTaskCode, 0x00, sizeof(TaskCodeDec_t));
	
	pTaskCode->CodeDec_q = OSQCreate(pTaskCode->CodeDec_q_stack, C_QUEUE_MAX);
	if(pTaskCode->CodeDec_q == 0) {
		goto __error;
	}
	
	pTaskCode->CodeDec_finish_q = OSQCreate(pTaskCode->CodeDec_finish_q_stack, C_QUEUE_MAX);
	if(pTaskCode->CodeDec_finish_q == 0) {
		goto __error;
	}

	pTaskCode->CodeDec_ack_m = OSMboxCreate(NULL);
	if(pTaskCode->CodeDec_ack_m == 0) {
		goto __error;
	}
	
	err = OSTaskCreate(code_decoder_task_entry, (void *)pTaskCode, &pTaskCode->CodeDecTaskStack[C_TASK_STACK_SIZE - 1], prio); 
	if(err != OS_NO_ERR) {
		goto __error;
	}
	
	return STATUS_OK;
__error:
	if(pTaskCode->CodeDec_q) {
		OSQDel(pTaskCode->CodeDec_q, OS_DEL_ALWAYS, &err);
	}

	if(pTaskCode->CodeDec_finish_q) {
		OSQDel(pTaskCode->CodeDec_finish_q, OS_DEL_ALWAYS, &err);
	}
	
	if(pTaskCode->CodeDec_ack_m) {
		OSMboxDel(pTaskCode->CodeDec_ack_m, 0, &err);
	}
	
	pTaskCode->CodeDec_q = NULL;
	pTaskCode->CodeDec_finish_q = NULL;
	pTaskCode->CodeDec_ack_m = NULL;
	return STATUS_FAIL;	
}

INT32S code_decoder_task_del(void)
{
	INT8U err;
	INT32S msg, nRet;
	 
	nRet = STATUS_OK;
	POST_MESSAGE(pTaskCode->CodeDec_q, MSG_CODE_DEC_EXIT, pTaskCode->CodeDec_ack_m, 5000, msg, err);
Return:
	if(pTaskCode->CodeDec_q) {
		OSQDel(pTaskCode->CodeDec_q, OS_DEL_ALWAYS, &err);
	}

	if(pTaskCode->CodeDec_finish_q) {
		OSQDel(pTaskCode->CodeDec_finish_q, OS_DEL_ALWAYS, &err);
	}
	
	if(pTaskCode->CodeDec_ack_m) {
		OSMboxDel(pTaskCode->CodeDec_ack_m, 0, &err);
	}
	
	pTaskCode->CodeDec_q = NULL;
	pTaskCode->CodeDec_finish_q = NULL;
	pTaskCode->CodeDec_ack_m = NULL;
	gp_memset((INT8S *)pTaskCode, 0x00, sizeof(TaskCodeDec_t));
	return STATUS_FAIL;	
}

INT32S code_decoder_init(CODE_TYPE type)
{
	INT8U err;
	INT32S msg, nRet;
	
	if(code_decoder_uninit() < 0) {
		RETURN(STATUS_FAIL);
	}
	
	if(type == QR_CODE) {
		POST_MESSAGE(pTaskCode->CodeDec_q, MSG_QRCODE_DEC_INIT, pTaskCode->CodeDec_ack_m, 5000, msg, err);
	} else {
		POST_MESSAGE(pTaskCode->CodeDec_q, MSG_BARCODE_DEC_INIT, pTaskCode->CodeDec_ack_m, 5000, msg, err);
	}
Return:
	return nRet;
}

INT32S code_decoder_uninit(void)
{
	INT8U err;
	INT32S msg, nRet;
		
	nRet = STATUS_OK;
	POST_MESSAGE(pTaskCode->CodeDec_q, MSG_CODE_DEC_UNINIT, pTaskCode->CodeDec_ack_m, 5000, msg, err);
Return:
	return nRet;
}

void code_decoder_set_enable(void)
{
	pTaskCode->status_flag |= C_CODE_DEC_ENABLE;
}

INT32U code_decoder_get_status(void)
{
	return pTaskCode->status_flag;
}

INT32S code_decoder_set_frame(INT32U frame_buffer, INT16U w, INT16U h, INT32U infmt)
{
	INT8U err;
	INT32S nRet;
	ObjFrame_t *pInput = &pTaskCode->Obj[pTaskCode->idx];
	
	if(frame_buffer == 0) {
		RETURN(STATUS_FAIL);
	}
	
	// check enable
	if((pTaskCode->status_flag & C_CODE_DEC_ENABLE) == 0) {
		RETURN(STATUS_FAIL);
	}
	
	// check busy
	if(pTaskCode->status_flag & C_CODE_DEC_BUSY) {
		RETURN(STATUS_FAIL);
	}
	
	// scale 	
	switch(infmt)
	{
	case BITMAP_GRAY:
		infmt = IMG_FMT_GRAY;
		break;
		
	case BITMAP_YUYV: 
		infmt = IMG_FMT_UYVY;
		break;
			
	default:
		RETURN(STATUS_FAIL);
	}
	
	pInput->ScaleIn.width = w;
	pInput->ScaleIn.height = h;
	if(infmt == IMG_FMT_GRAY) {
		pInput->ScaleIn.ch  = 1;
		pInput->ScaleIn.widthStep = w;
	} else {
		pInput->ScaleIn.ch  = 2;
		pInput->ScaleIn.widthStep = w * 2;
	}
	pInput->ScaleIn.format = infmt;
	pInput->ScaleIn.ptr = (INT8U *)frame_buffer;
	
	scaler_once(0, &pInput->ScaleIn, &pInput->ScaleOut);
	scaler_wait_done();
	
	// post buffer	
	err = OSQPost(pTaskCode->CodeDec_q, (void *)pInput);
	if(err != OS_NO_ERR) {
		DEBUG_MSG("Post detect Error\r\n");
		RETURN(STATUS_FAIL);
	}	
	
	// set busying
	pTaskCode->status_flag |= C_CODE_DEC_BUSY;
	if(++pTaskCode->idx >= C_QUEUE_MAX) {
		pTaskCode->idx = 0;
	}
	
	nRet = STATUS_OK;
Return:	
	return nRet;
}

INT32S code_decoder_wait_finish(INT32U wait_done)
{
	INT8U err;
	INT32S msg;
	
	if(wait_done) {
	 	msg = (INT32S)OSQPend(pTaskCode->CodeDec_finish_q, 0, &err);
	} else {
		msg = (INT32S)OSQAccept(pTaskCode->CodeDec_finish_q, &err); 	
	}
	
	if(err != OS_NO_ERR) {
	 	return -1;
	}
	  
	if(msg == C_ACK_FAIL) {
		return -1;	//fail
	} else if(msg == C_ACK_SUCCESS) {
		return 1;   //success
	} 
	
	return 0;   	//waiting
}

INT8U *QRCode_get_Result(void)
{
	return pTaskCode->QRCodeStream;
}

BarCodeResult_t *BarCode_get_Result(INT32S *Number)
{
	*Number = pTaskCode->BarCodeNumber;
	return &pTaskCode->BarCodestream;
}

static void code_decoder_task_entry(void *para)
{
	INT8U err;
	INT32U msg_id, ret;
	TaskCodeDec_t *pTCode = (TaskCodeDec_t *)para;
	ObjFrame_t Input;
	OS_EVENT *CodeDec_q = pTCode->CodeDec_q;
	OS_EVENT *CodeDec_finish_q = pTCode->CodeDec_finish_q;
	OS_EVENT *CodeDec_ack_m = pTCode->CodeDec_ack_m;
	
	while(1) {
        msg_id = (INT32U) OSQPend(CodeDec_q, 0, &err);
		if(err != OS_NO_ERR) {
			continue;
		}
		
		switch(msg_id)
		{
		case MSG_QRCODE_DEC_INIT:
			DEBUG_MSG("MSG_QRCODE_DEC_INIT\r\n");
			pTCode->decode_type = QR_CODE;
			if(code_initial(pTCode) < 0) {
				OSMboxPost(CodeDec_ack_m, (void*)C_ACK_FAIL);
			} else {
				OSMboxPost(CodeDec_ack_m, (void*)C_ACK_SUCCESS);
			}
			break;
		
		case MSG_BARCODE_DEC_INIT:
			DEBUG_MSG("MSG_BARCODE_DEC_INIT\r\n");
			pTCode->decode_type = BAR_CODE;
			if(code_initial(pTCode) < 0) {
				OSMboxPost(CodeDec_ack_m, (void*)C_ACK_FAIL);
			} else {
				OSMboxPost(CodeDec_ack_m, (void*)C_ACK_SUCCESS);
			}
			break;
		
		case MSG_CODE_DEC_UNINIT:
			DEBUG_MSG("MSG_CODE_DEC_UNINIT\r\n");
			OSQFlush(CodeDec_finish_q);
			code_decoder_free(pTCode);
			OSMboxPost(CodeDec_ack_m, (void*)C_ACK_SUCCESS);
			break;
			
		case MSG_CODE_DEC_EXIT:
			DEBUG_MSG("MSG_OBJ_DET_EXIT\r\n");
			code_decoder_free(pTCode);
			OSMboxPost(CodeDec_ack_m, (void*)C_ACK_SUCCESS);
			OSTaskDel(OS_PRIO_SELF);
			break;
			
		default:
			// check status
			if((pTaskCode->status_flag & C_CODE_DEC_ENABLE) == 0) {
				continue;
			}
			
			// set status 
			pTaskCode->status_flag |= C_CODE_DEC_BUSY;
		
			// set decode buffer address
			gp_memcpy((INT8S *)&Input, (INT8S *)msg_id, sizeof(ObjFrame_t));
			
			// flush cache
			ret = Input.ScaleOut.width * Input.ScaleOut.height;
			cache_invalid_range((INT32U)Input.ScaleOut.ptr, ret); 
			
			// decode run
			if(pTCode->process(pTCode->WorkMem, &Input) < 0) {
				ret = C_ACK_FAIL;
			} else {
				ret = C_ACK_SUCCESS;
			}
			
			// post finish frame
			err = OSQPost(CodeDec_finish_q, (void *)ret);
			if(err != OS_NO_ERR) {
			    DEBUG_MSG("Post Finish Error\r\n");
			}
			
			// set status 
			pTaskCode->status_flag &= ~C_CODE_DEC_BUSY;
			pTaskCode->status_flag &= ~C_CODE_DEC_ENABLE;
		}
	}
}
#endif