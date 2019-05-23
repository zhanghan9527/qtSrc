#include "application.h"
#include "task_object_detect.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it =====================//
#if (APP_FACE_DETECTION_EN == 1)
//==================================================================================//
#include "ObjDetection.h"

/****************************************************
*		define										*
****************************************************/
#define C_ACK_SUCCESS			0x00000001
#define C_ACK_FAIL				0x80000000
#define ObjDetTaskStackSize		2048 * 4
#define C_QUEUE_MAX				3

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
	INT32U ObjDetTaskStack[ObjDetTaskStackSize];
	void *ObjDet_q_stack[C_QUEUE_MAX];
	void *ObjDet_finish_q_stack[C_QUEUE_MAX];
	OS_EVENT *ObjDet_q; 
	OS_EVENT *ObjDet_finish_q;  
	OS_EVENT *ObjDet_ack_m;
	
	// workmem and function
	ObjDetect_t *odWorkMem;
	void (*process)(ObjDetect_t *);
	void (*get_result)(ObjDetect_t *, ObjDetResult_t *, gpImage *, gpImage *);
	void (*free)(ObjDetect_t *);
	
	// face detect input
	INT32U obj_type;
	INT32U obj_idx;
	ObjFrame_t Obj[C_QUEUE_MAX];
	
	// result
	INT32U result_idx;
	ObjDetResult_t result[C_QUEUE_MAX];
} TaskObjDect_t;

/****************************************************
*		extnal func									*
****************************************************/
extern INT32S scalerStart(gpImage *src, gpImage *dst);
extern INT32S scalerEnd(void);

static void object_detect_task_entry(void *para);
static INT32S get_queue_size(OS_EVENT *q);
			
/****************************************************
*		varaible									*
****************************************************/
TaskObjDect_t *pTaskObj = NULL;
TaskObjDect_t gTaskObj;

static INT32S object_detect_malloc(TaskObjDect_t *pTObj)
{
	INT32U i, size;
	
	for(i=0; i<C_QUEUE_MAX; i++) {
		size = pTObj->Obj[i].ScaleOut.width * pTObj->Obj[i].ScaleOut.height;
		pTObj->Obj[i].ScaleOut.ptr = (INT8U *)gp_malloc_align(size, 32);
		if(pTObj->Obj[i].ScaleOut.ptr == 0) {
			return STATUS_FAIL;
		}
		DEBUG_MSG("ScaleOutBuf[%d] = 0x%x\r\n", i, pTObj->Obj[i].ScaleOut.ptr);
	}
	
	return STATUS_OK;
}

static void object_detect_free(TaskObjDect_t *pTObj)
{
	INT32U i;
	
	for(i=0; i<C_QUEUE_MAX; i++) {
		if(pTObj->Obj[i].ScaleOut.ptr) {
			gp_free((void *)pTObj->Obj[i].ScaleOut.ptr);
			pTObj->Obj[i].ScaleOut.ptr = 0;
		}
	}
}

static INT32S get_queue_size(OS_EVENT *q)
{
	OS_Q *pq;
	pq = (OS_Q *)q->OSEventPtr;

	return (pq->OSQSize - pq->OSQEntries); 
}

static void obj_get_result(ObjDetect_t *odWorkMem, ObjDetResult_t *pResult, gpImage *pSrcImg, gpImage *pDstImg)
{
	obj_detect_get_result(odWorkMem, pResult);
	obj_detect_adject_position(pResult, pSrcImg, pDstImg);
}

#if OBJECT_RECOGNIZE_EN == 1
static void recog_get_result(ObjDetect_t *odWorkMem, ObjDetResult_t *pResult, gpImage *pSrcImg, gpImage *pDstImg)
{
	obj_recognize_get_result(odWorkMem, pResult);
}
#endif

static INT32U object_initial(TaskObjDect_t *pTObj)
{
	INT16U width, height;
	INT32S i;
	
	switch(pTObj->obj_type)
	{
	case C_FACE_DETECT: // face only
		width = OBJDETECT_IMG_WIDTH;
		height = OBJDETECT_IMG_HEIGHT;
		pTObj->process = obj_detect_proc;
		pTObj->get_result = obj_get_result;
		pTObj->free = obj_detect_free;	
		pTObj->odWorkMem = (ObjDetect_t *)obj_detect_alloc(OBJ_FACE, 
														POST_PROC_DISABLE, 
														eye_detect_reset, 
														eye_detect_alloc, 
														eye_detect_free,
														eye_detect_proc, 
														eye_detect_get_result);	
		break;
	
	#if EYE_DETECT_EN == 1
	case C_FACE_EYE_DETECT: // face, eye, nose, mouth
		width = OBJDETECT_IMG_WIDTH;
		height = OBJDETECT_IMG_HEIGHT;
		pTObj->process = obj_detect_proc;
		pTObj->get_result = obj_get_result;
		pTObj->free = obj_detect_free;
		pTObj->odWorkMem = (ObjDetect_t *)obj_detect_alloc(OBJ_FACE, 
														POST_PROC_ENABLE, 
														eye_detect_reset, 
														eye_detect_alloc, 
														eye_detect_free,
														eye_detect_proc, 
														eye_detect_get_result);
		break;
	#endif
	
	#if FACE_RECOGNIZE_EN == 1
	case C_FACE_RECOGNIZE: // face identify
		width = OBJDETECT_IMG_WIDTH;
		height = OBJDETECT_IMG_HEIGHT;
		pTObj->process = face_detect_proc;
		pTObj->get_result = obj_get_result;
		pTObj->free = face_detect_free;
		pTObj->odWorkMem = (ObjDetect_t *)face_detect_alloc(OBJ_FACE, 
														POST_PROC_ENABLE, 
														face_recognize_reset, 
														face_recognize_alloc, 
														face_recognize_free,
														face_recognize_proc, 
														face_recognize_get_result);
		break;
	#endif
	
#if GESTURE_DETECT_EN == 1	
	case C_GESTURE_DETECT: // hand, fist 
		width = OBJDETECT_IMG_WIDTH;
		height = OBJDETECT_IMG_HEIGHT;
		pTObj->process = gesture_detect_proc;
		pTObj->get_result = obj_get_result;
		pTObj->free = obj_detect_free;
		pTObj->odWorkMem = (ObjDetect_t *)obj_detect_alloc(OBJ_HAND, 
														POST_PROC_DISABLE, 
														NULL, 
														NULL, 
														NULL,
														NULL, 
														NULL);
		break;
#endif
	
#if OBJECT_RECOGNIZE_EN == 1	
	case C_OBJECT_RECOGNIZE:
		width = RFMSDETECT_IMG_WIDTH;
		height = RFMSDETECT_IMG_HEIGHT;
		pTObj->process = obj_recognize_proc;
		pTObj->get_result = recog_get_result;
		pTObj->free = obj_recognize_free;
		pTObj->odWorkMem = (ObjDetect_t *)obj_recognize_alloc();
		break;
#endif
	
	case C_USER_MODE_DETECT:
	
		break;
		
	default:
		return 0;;
	} 
	
	if(pTObj->odWorkMem == 0) {
		return 0;
	}
	
	// memory alloc
	for(i=0; i<C_QUEUE_MAX; i++) {
		pTObj->Obj[i].ScaleOut.width = width;
		pTObj->Obj[i].ScaleOut.height = height;
		pTObj->Obj[i].ScaleOut.ch = 1;
		pTObj->Obj[i].ScaleOut.widthStep = width;
		pTObj->Obj[i].ScaleOut.format = IMG_FMT_GRAY;
	}
	
	if(object_detect_malloc(pTObj) < 0) {
		object_detect_free(pTObj);
		return 0;
	}
	
	return (INT32U)pTObj->odWorkMem;
}

ObjDetect_t *obj_detect_get_workmem(void)
{
	return pTaskObj->odWorkMem;
}

void obj_detect_adject_position(ObjDetResult_t *result, gpImage *pSrcImg, gpImage *pDstImg)
{
	INT32S i;
	float x_factor, y_factor;
	
	x_factor = (float)pSrcImg->width / (float)pDstImg->width;
	y_factor = (float)pSrcImg->height / (float)pDstImg->height;
	
	for(i=0; i<result->ResultCnt; i++) {
		result->Result[i].x = result->Result[i].x * x_factor;
		result->Result[i].y = result->Result[i].y * y_factor;
		result->Result[i].width = result->Result[i].width * x_factor;
		result->Result[i].height = result->Result[i].height * x_factor;
	}
	
	if(result->EyeFlag) {
		for(i=0; i<2; i++) {
			result->Eye[i].x = result->Eye[i].x * x_factor;
			result->Eye[i].y = result->Eye[i].y * y_factor;
			result->Eye[i].width = result->Eye[i].width * x_factor;
			result->Eye[i].height = result->Eye[i].height * x_factor;
			
			result->Eyebrow[i].x = result->Eyebrow[i].x * x_factor;
			result->Eyebrow[i].y = result->Eyebrow[i].y * y_factor;
			result->Eyebrow[i].width = result->Eyebrow[i].width * x_factor;
			result->Eyebrow[i].height = result->Eyebrow[i].height * x_factor;
		}
	}
	
	if(result->NoseFlag) {
		for(i=0; i<2; i++) {
			result->Nose[i].x = result->Nose[i].x * x_factor;
			result->Nose[i].y = result->Nose[i].y * y_factor;
			result->Nose[i].width = result->Nose[i].width * x_factor;
			result->Nose[i].height = result->Nose[i].height * x_factor;
		}
	}
	
	if(result->MouthFlag) {
		for(i=0; i<4; i++) {
			result->Mouth[i].x = result->Mouth[i].x * x_factor;
			result->Mouth[i].y = result->Mouth[i].y * y_factor;
			result->Mouth[i].width = result->Mouth[i].width * x_factor;
			result->Mouth[i].height = result->Mouth[i].height * x_factor;
		}
	}
}

INT32S object_detect_task_create(INT32U type, INT32U prio)
{
	INT8U err;
	
	pTaskObj = &gTaskObj;
	gp_memset((INT8S *)pTaskObj, 0x00, sizeof(TaskObjDect_t));
	pTaskObj->obj_type = type;
	
	pTaskObj->ObjDet_q = OSQCreate(pTaskObj->ObjDet_q_stack, C_QUEUE_MAX);
	if(pTaskObj->ObjDet_q == 0) {
		goto __error;
	}

	pTaskObj->ObjDet_finish_q = OSQCreate(pTaskObj->ObjDet_finish_q_stack, C_QUEUE_MAX);
	if(pTaskObj->ObjDet_finish_q == 0) {
		goto __error;
	}

	pTaskObj->ObjDet_ack_m = OSMboxCreate(NULL);
	if(pTaskObj->ObjDet_ack_m == 0) {
		goto __error;
	}
	
	err = OSTaskCreate(object_detect_task_entry, (void *)pTaskObj, &pTaskObj->ObjDetTaskStack[ObjDetTaskStackSize - 1], prio); 
	if(err != OS_NO_ERR) {
		goto __error;
	}
	
	if(object_detect_init() < 0) {
		goto __error;
	}
	
	return STATUS_OK;
	
__error:
	if(pTaskObj->ObjDet_q) {
		OSQDel(pTaskObj->ObjDet_q, OS_DEL_ALWAYS, &err);
	}

	if(pTaskObj->ObjDet_finish_q) {
		OSQDel(pTaskObj->ObjDet_finish_q, OS_DEL_ALWAYS, &err);
	}
	
	if(pTaskObj->ObjDet_ack_m) {
		OSMboxDel(pTaskObj->ObjDet_ack_m, 0, &err);
	}
	
	pTaskObj->ObjDet_q = NULL;
	pTaskObj->ObjDet_finish_q = NULL;
	pTaskObj->ObjDet_ack_m = NULL;
	pTaskObj = NULL;
	return STATUS_FAIL;
}

INT32S object_detect_task_del(void)
{
	INT8U err;
	INT32S msg, nRet;
	 
	nRet = STATUS_OK;
	POST_MESSAGE(pTaskObj->ObjDet_q, MSG_OBJ_DET_EXIT, pTaskObj->ObjDet_ack_m, 5000, msg, err);
Return:
	if(pTaskObj->ObjDet_q) {
		OSQFlush(pTaskObj->ObjDet_q);
		OSQDel(pTaskObj->ObjDet_q, OS_DEL_ALWAYS, &err);
	}

	if(pTaskObj->ObjDet_finish_q) {
		OSQFlush(pTaskObj->ObjDet_finish_q);
		OSQDel(pTaskObj->ObjDet_finish_q, OS_DEL_ALWAYS, &err);
	}
	
	if(pTaskObj->ObjDet_ack_m) {
		OSMboxDel(pTaskObj->ObjDet_ack_m, 0, &err);
	}
	
	pTaskObj->ObjDet_q = NULL;
	pTaskObj->ObjDet_finish_q = NULL;
	pTaskObj->ObjDet_ack_m = NULL;
	pTaskObj = NULL;
	return nRet;
}

INT32S object_detect_init(void)
{
	INT8U err;
	INT32S msg, nRet;
			
	nRet = STATUS_OK;
	POST_MESSAGE(pTaskObj->ObjDet_q, MSG_OBJ_DET_INIT, pTaskObj->ObjDet_ack_m, 5000, msg, err);
Return:
	return nRet;
}

INT32S object_detect_set_post_proc(INT32U enable)
{
	INT8U err;
	INT32S msg, nRet;	

	nRet = STATUS_OK;
	if(enable) {
		POST_MESSAGE(pTaskObj->ObjDet_q, MSG_OBJ_DET_POST_PROC_ENABLE, pTaskObj->ObjDet_ack_m, 5000, msg, err);
	} else {
		POST_MESSAGE(pTaskObj->ObjDet_q, MSG_OBJ_DET_POST_PROC_DISABLE, pTaskObj->ObjDet_ack_m, 5000, msg, err);
	}
Return:
	return nRet;	
}

INT32S object_detect_set_frame(INT32U frame_buffer, INT16U w, INT16U h, INT32U infmt)
{
	INT8U err;
	INT32S nRet;
	ObjFrame_t *pInput = &pTaskObj->Obj[pTaskObj->obj_idx];
	
	if(frame_buffer == 0) {
		RETURN(STATUS_FAIL);
	}

	if(pTaskObj == 0) {
		RETURN(STATUS_FAIL);
	}
	
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
	
	// check the queue can post or not
	if(get_queue_size(pTaskObj->ObjDet_q) <= 1) {
		//DEBUG_MSG("ObjDet_q is not enough\r\n");
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
	
	// scale 
	scalerStart(&pInput->ScaleIn, &pInput->ScaleOut);
	scalerEnd();
	
	// post buffer	
	err = OSQPost(pTaskObj->ObjDet_q, (void *)pInput);
	if(err != OS_NO_ERR) {
		DEBUG_MSG("Post detect Error\r\n");
		RETURN(STATUS_FAIL);
	}

	if(++pTaskObj->obj_idx >= C_QUEUE_MAX) {
		pTaskObj->obj_idx = 0;
	}

	nRet = STATUS_OK;
Return:	
	return nRet;
}

ObjDetResult_t *object_detect_wait_frame_finish(INT32U wait_done)
{
	INT8U err;
	ObjDetResult_t *msg;
	
	if(wait_done) {
	 	msg = (ObjDetResult_t *)OSQPend(pTaskObj->ObjDet_finish_q, 0, &err);
		if((err != OS_NO_ERR) || (msg == 0)) {
			return 0;
		}
	} else {
		msg = (ObjDetResult_t *)OSQAccept(pTaskObj->ObjDet_finish_q, &err);
	 	if((err != OS_NO_ERR) || msg == 0) {
	 		return 0;
	 	}
	}
	  
	return msg; 
}

static void object_detect_task_entry(void *para)
{
	INT8U err;
	INT32U msg_id;
	TaskObjDect_t *pTObj = (TaskObjDect_t *)para;
	ObjDetect_t *odWorkMem;
	ObjDetResult_t *pResult;
	ObjFrame_t Input;
	OS_EVENT *ObjDet_q = pTObj->ObjDet_q;
	OS_EVENT *ObjDet_finish_q = pTObj->ObjDet_finish_q;
	OS_EVENT *ObjDet_ack_m = pTObj->ObjDet_ack_m;
	
	while(1) {
        msg_id = (INT32U) OSQPend(ObjDet_q, 0, &err);
		if(err != OS_NO_ERR) {
			continue;
		}
		
		switch(msg_id)
		{
		case MSG_OBJ_DET_INIT:
			DEBUG_MSG("MSG_OBJ_DET_INIT\r\n");
			OSQFlush(ObjDet_finish_q);
			odWorkMem = (ObjDetect_t *)object_initial(pTObj);
			if(odWorkMem == 0) {
				OSMboxPost(ObjDet_ack_m, (void*)C_ACK_FAIL);
			} else {
				OSMboxPost(ObjDet_ack_m, (void*)C_ACK_SUCCESS);
			}
			break;
		
		case MSG_OBJ_DET_POST_PROC_ENABLE:
			DEBUG_MSG("MSG_OBJ_DET_POST_PROC_ENABLE\r\n");
			odWorkMem->post_proc.flag = POST_PROC_ENABLE;
			OSMboxPost(ObjDet_ack_m, (void*)C_ACK_SUCCESS);
			break;
			
		case MSG_OBJ_DET_POST_PROC_DISABLE:
			DEBUG_MSG("MSG_OBJ_DET_POST_PROC_DISABLE\r\n");
			odWorkMem->post_proc.flag = POST_PROC_DISABLE;
			OSMboxPost(ObjDet_ack_m, (void*)C_ACK_SUCCESS);
			break;	
		
		case MSG_OBJ_DET_EXIT:
			DEBUG_MSG("MSG_OBJ_DET_EXIT\r\n");
			pTObj->free(odWorkMem);
			OSMboxPost(ObjDet_ack_m, (void*)C_ACK_SUCCESS);
			OSTaskDel(OS_PRIO_SELF);
			break;
				
		default:
			// check the queue can post or not
			if(get_queue_size(ObjDet_finish_q) <= 0) {
				//DEBUG_MSG("ObjDet_finish_q is not enough\r\n");;
				continue;
			}
			
			// set detect buffer address
			gp_memcpy((INT8S *)&Input, (INT8S *)msg_id, sizeof(ObjFrame_t));
			odWorkMem->image.ptr = Input.ScaleOut.ptr;
			
			// set output buffer 
			pResult = (ObjDetResult_t *)&pTObj->result[pTObj->result_idx];
			pResult->FrameBuf = (INT32U)Input.ScaleIn.ptr;
			
			// detect run 
        	pTObj->process(odWorkMem);
		
			// get result
			pTObj->get_result(odWorkMem, pResult, &Input.ScaleIn, &Input.ScaleOut);
			
			// post finish frame
			err = OSQPost(ObjDet_finish_q, (void *)pResult);
			if(err != OS_NO_ERR) {
			    DEBUG_MSG("Post Finish Error\r\n");
			}
			
			if(++pTObj->result_idx >= (C_QUEUE_MAX)) {
				pTObj->result_idx = 0;
			}
			break;
		}
    }
}
#endif
