#ifndef __TASK_OBJECT_DETECT__
#define __TASK_OBJECT_DETECT__

#include "application.h"
#include "task_object_detect_cfg.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (APP_FACE_DETECTION_EN == 1) 
//================================================================//
#include "define.h"

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

/****************************************************
*		data type									*
****************************************************/
typedef struct {
	INT32U FrameBuf;
	INT8S ResultCnt;
	INT8U EyeFlag;
	INT8U NoseFlag;
	INT8U MouthFlag;
	
	INT8U SmileFlag;
	INT8U FaceTrainingFlag;
	INT8U FaceIdentifyFlag;
	INT8U HandFlag;
	INT8U FistFlag;
	
	gpRect Result[64];
	gpRect Eye[2];
	gpRect Eyebrow[2];
	gpRect Nose[2];
	gpRect Mouth[4];

	INT8U ObjIdentifyFlag;
	INT8U MatchingCardNum;
	INT8U reserved0;
	INT8U reserved1;
} ObjDetResult_t;

/****************************************************
*		function									*
****************************************************/
// object detect task
//========================================================
// Function Name :  object_detect_task_create
// Syntax : 
// Purpose :  create object detect task
// Parameters : DETECT_TYPE type: detect type
//				INT32U prio: task priority
// Return : >=0: success, <0: fail
//========================================================
INT32S object_detect_task_create(INT32U type, INT32U prio);

//========================================================
// Function Name :  object_detect_task_del
// Syntax : 
// Purpose :  delect object detect task
// Parameters : none
// Return : >=0: success, <0: fail
//========================================================
INT32S object_detect_task_del(void);

//========================================================
// Function Name :  object_detect_init
// Syntax : 
// Purpose :  initial object detect
// Parameters : none
// Return : >=0: success, <0: fail
//========================================================
INT32S object_detect_init(void);

//========================================================
// Function Name :  object_detect_set_post_proc
// Syntax : 
// Purpose :  set post process enable or disable
// Parameters : INT32U enable: 0 is disable, 1 is enable
// Return : >=0: success, <0: fail
//========================================================
INT32S object_detect_set_post_proc(INT32U enable);

//========================================================
// Function Name :  object_detect_set_frame
// Syntax : 
// Purpose :  set detect frame buffer 
// Parameters : INT32U frame_buffer: frame buffer addres
//				INT16U bufw: frame buffer width
//				INT16U bufh: frame buffer height
//				INT32U infmt: frame buffer format
// Return : >=0: success, <0: fail
//========================================================
INT32S object_detect_set_frame(INT32U frame_buffer, INT16U bufw, INT16U bufh, INT32U infmt);

//========================================================
// Function Name :  object_detect_wait_frame_finish
// Syntax : 
// Purpose :  polling or wait object done 
// Parameters : INT32U wait_done: 0 is not wait done, 1 is wait done.
// Return : object detect finish result
//========================================================
ObjDetResult_t *object_detect_wait_frame_finish(INT32U wait_done);

//========================================================
// Function Name :  obj_detect_adject_position
// Syntax : 
// Purpose :  adject the result postion 
// Parameters : ObjDetResult_t *result: result.
//				gpImage *pSrcImg: input image
//				gpImage *pDstImg: output image
// Return : none
//========================================================
void obj_detect_adject_position(ObjDetResult_t *result, gpImage *pSrcImg, gpImage *pDstImg);

//========================================================
// Function Name :  obj_recognize_set_mode
// Syntax : 
// Purpose :  object recognize set runing mode 
// Parameters : INT32U mode: 0:idle, 1: image save, 2:object training, 3: object recognize
// Return : 
//========================================================
void obj_recognize_set_mode(INT32U mode);

//========================================================
// Function Name :  obj_recognize_set_matchingThre_level
// Syntax : 
// Purpose :  object recognize set matching level
// Parameters : INT8U value: matching level 
// Return : none
//========================================================
void obj_recognize_set_matchingThre_level(INT8U value);

//========================================================
// Function Name :  face_recognize_set_security_level
// Syntax : 
// Purpose :  face recognize set security level
// Parameters : INT8U value: security level 
// Return : none
//========================================================
void face_recognize_set_security_level(INT8U value);

//========================================================
// Function Name :  face_recognize_get_buffer
// Syntax : 
// Purpose :  face recognize get ULBP buffer address 
// Parameters : 
// Return : buffer address
//========================================================
INT32U face_recognize_get_buffer(void);

//========================================================
// Function Name :  face_recognize_get_traning_cnt
// Syntax : 
// Purpose :  face recognize get training count 
// Parameters : 
// Return : count
//========================================================
INT32U face_recognize_get_training_cnt(void);

//========================================================
// Function Name :  face_recognize_set_mode
// Syntax : 
// Purpose :  face recognize set runing mode 
// Parameters : INT32U mode: 0:idle, 1: training, 2: identify
// Return : 
//========================================================
void face_recognize_set_mode(INT32U mode);

//========================================================
// Function Name :  face_recognize_get_mode
// Syntax : 
// Purpose :  face recognize get runing mode 
// Parameters : INT32U mode: 0:idle, 1: training, 2: identify
// Return : 
//========================================================
INT32U face_recognize_get_mode(INT32U mode);

//========================================================
// Function Name :  face_recognize_register_store_func
// Syntax : 
// Purpose :  face recognize register training data store function
// Parameters : INT32S (*pFuncStore)(INT32U, INT32U): store function
// Return : 
//========================================================
void face_recognize_register_store_func(INT32S (*pFuncStore)(INT32U, INT32U));

// draw line
//========================================================
// Function Name :  obj_detect_show_result
// Syntax : 
// Purpose :  draw the result to frame buffer
// Parameters : INT32U img_adr: frame buffer address
//				INT32U bufw: frame buffer width
//				INT32U bufh: frame buffer height
//				ObjDetResult_t *result: draw rectangle data
// Return : None
//========================================================
extern void obj_detect_show_result(INT32U img_adr, INT32U bufw, INT32U bufh, ObjDetResult_t *result);
#endif
#endif