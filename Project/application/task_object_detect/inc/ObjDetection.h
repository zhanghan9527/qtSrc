#ifndef __OBJDETECTION_H__
#define __OBJDETECTION_H__

#include "application.h"
#include "task_object_detect_cfg.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (APP_FACE_DETECTION_EN == 1) 
//================================================================//
#include "define.h"
#include "FaceDetectAP.h"
#include "FaceDetectKernel.h"
#include "gp_track.h"
#include "MouthDetect.h"
#include "NoseDetect.h"
#include "ObjDetection.h"
#include "FaceIdentifyAP.h"

/****************************************************
*		define										*
****************************************************/
#define PI		3.1415926535897932384626433832795

// post proc
#define EYE_DETECT_ENABLE 		1
#define EYE_DETECT_DISABLE		0

#define MOUTH_DETECT_ENABLE 	1
#define MOUTH_DETECT_DISABLE	0

#define POST_PROC_ENABLE 		1
#define POST_PROC_DISABLE		0

// for eye detection
#define RESIZE_FACE_IMG_WIDTH	96/*128*/
#define RESIZE_FACE_IMG_HEIGHT	96/*128*/

#define HAAR_X_MARGIN		7
#define HAAR_Y_MARGIN		7

#define EYES_WIDTH			36	// RESIZE_IMG_WIDTH - RESIZE_IMG_WIDTH * 0.13 * 2
#define EYES_HEIGHT			36	// RESIZE_IMG_HEIGHT - RESIZE_IMG_WIDTH * 0.4 - 2 * EYES_WIDTH * 0.15

#define OBJDETECT_IMG_WIDTH			320
#define OBJDETECT_IMG_HEIGHT		240

#define OBJDETECT_MIN_RT_NBR        4
#define OBJDETECT_MIN_LT_NBR        5
#define OBJDETECT_MIN_fist_NBR      5
#define OBJDETECT_MIN_NBR			4
#define OBJDETECT_MAX_RESULT		64
#define OBJDETECT_MAX_CONDIDATE		512

#define GESTURE_CTRL_FLIP		0x01
#define GESTURE_CTRL_VOL		0x02

#define FD_TRAINING_STATE_GET		1
#define FD_IDENTIFY_STATE_GET		2
#define FD_SECURITY_LEVEL_GET       3
#define FD_TRAINING_CNT_GET			4
#define FD_IDENTIFY_SCORE_GET		5

// object recognize
#define RFMSDETECT_IMG_WIDTH	256
#define RFMSDETECT_IMG_HEIGHT	192

/****************************************************
*		data type									*
****************************************************/
typedef struct {
	INT32S flag;
	void *workmem;
	void (*reset)(void *para);
	int (*alloc)(void);
	void (*free)(void *para);
	void (*run)(void *para, gpImage *org_img, void *pre_result);
	void (*get_result)(void *para, void *result);	
} PostProc_t;

typedef struct {
	void *obj_track_WorkMem;
	void *obj_detect_WorkMem;
	void *rfms_detect_WorkMem;
	gpImage	image;
	INT32U WorkMemSize;
	
	//classify
	ClassifyData clf;		
	INT32S xstep;
	INT32S ystep;
	INT32S scale;
	PostProc_t post_proc;
	
	//result
	INT8U no_face_cnt;
	INT8U gesture_mode;	//gesture detect use 
	INT8U reserved0;
	INT8U reserved1;
	gpRect result[OBJDETECT_MAX_RESULT];
	INT32S count[OBJDETECT_MAX_RESULT];
} ObjDetect_t;

typedef struct {
	gpPoint left_eye;
	gpPoint right_eye;
	gpRect left_eyebrow;
	gpRect right_eyebrow;
	gpRect face;
	
	double eye_angle;
} FaceEyeResult_t;

typedef struct {
	void *eye_detect_WorkMem;
	gpImage	face_img;
	gpImage	eq_face_img;
	
	ClassifyData clf;
	INT32S xstep;
	INT32S scale;	
	PostProc_t post_proc;

	FaceEyeResult_t fe_result;
	INT8U is_get_eye;
	INT8U no_eye_cnt;
	INT8U reserved0;
	INT8U reserved1;
	
	gpRect eye_rect[OBJDETECT_MAX_RESULT];
	INT32S eye_cnt[OBJDETECT_MAX_RESULT];
} EyeDetect_t;

typedef struct {
	void *mouth_detect_WorkMem;
	gpImage	upright_face_img;
	gpFeaturePoint mouth_pt;
	gpFeaturePoint nose_pt;
	INT8U is_get_nose;
	INT8U is_get_mouth;
	INT8U reserved0;
	INT8U reserved1;
} MouthNoseDetect_t;

typedef struct {
	void *identify_WorkMem;
	void *eye_detect_WorkMem;
	void *ownerULBP;
	gpImage	image;
	INT8U id_mode;
	INT8U training_cnt;
	INT8U identify_cnt;
	INT8U security_level;
	INT32U training_state;
	INT32U identify_state;
	INT32U security_value;
	INT32S recognize_score;
	INT32S (*pFuncStore)(INT32U buf, INT32U size);
} FaceIdentify_t;

typedef struct {
	INT32S *rfms_WorkMem;
	CHAR   *obj_data_buf;
	gpImage	image;
	INT8U id_mode;
	INT8U object_training_cnt;
	INT8U object_match_cnt;	
	INT32S cardsN;
	INT32U matchingThre;
	INT32U matchingThreRangeV;
	INT32U minExtractionThre;
	INT32U incExtractionThre;
	INT32U decExtractionThre;
	INT32U startMatchingPointN;
	INT32U matchingCardNum;
} RfmsIdentify_t;

typedef struct {
	INT32S l[3];
	INT32S level[4];
	INT32S cnt[3];
} LumStr;

typedef struct {
	INT8S result_cnt;
	INT8U is_best_face;
	INT8U is_get_eye;
	INT8U is_get_nose;
	
	INT8U is_get_mouth;
	INT8U is_smile;
	INT8U training_ok;
	INT8U identify_ok;
	
	INT8U is_hand;
	INT8U is_fist;
	INT8U Reserved0;
	INT8U Reserved1;
	
	gpRect rect[OBJDETECT_MAX_RESULT];
	gpRect best_face;
	gpRect eye[2];
	gpRect eyebrow[2];
	gpRect nose[2];
	gpRect mouth[4];
} ObjResult_t;

/****************************************************
*		function									*
****************************************************/
// api
//========================================================
// Function Name :  scalerStart
// Syntax : 
// Purpose :  start scale
// Parameters : gpImage *src: source image
//				gpImage *dst: destination image
// Return : >=0: success, <0: fail
//========================================================
INT32S scalerStart(gpImage *src, gpImage *dst);

//========================================================
// Function Name :  scalerEnd
// Syntax : 
// Purpose :  wait scale finish
// Parameters : None
// Return : >=0: success, <0: fail
//========================================================
INT32S scalerEnd(void);

//========================================================
// Function Name :  scalerClip
// Syntax : 
// Purpose :  scale clip 
// Parameters : gpImage *src: source image
//				gpImage *dst: destination image
//				gpRect *clip: clip region
// Return : >=0: success, <0: fail
//========================================================
INT32S scalerClip(gpImage *src, gpImage *dst, gpRect *clip);

// tracking
//========================================================
// Function Name :  obj_track_init
// Syntax : 
// Purpose :  tracking init
// Parameters : ObjDetect_t *odWorkMem: object detect work memory
// Return : >=0: success, <0: fail
//========================================================
INT32S obj_track_init(ObjDetect_t *odWorkMem);

// mouth
//========================================================
// Function Name :  mouth_detect_init
// Syntax : 
// Purpose :  mouth detect init
// Parameters : MouthNoseDetect_t *mdWorkMem: mouth detect work memory
// Return : none
//========================================================
void mouth_detect_init(MouthNoseDetect_t *mdWorkMem);

//========================================================
// Function Name :  mouth_detect_reset
// Syntax : 
// Purpose :  mouth detect reset
// Parameters : void *para: mouth detect work memory
// Return : none
//========================================================
void mouth_detect_reset(void *para);

//========================================================
// Function Name :  mouth_detect_alloc
// Syntax : 
// Purpose :  mouth detect memory alloc
// Parameters : 
// Return : !=0 is success, ==0 is fail
//========================================================
INT32S mouth_detect_alloc(void);

//========================================================
// Function Name :  mouth_detect_free
// Syntax : 
// Purpose :  mouth detect free memory
// Parameters : void *md_work_mem: mouth detect work memory
// Return : none
//========================================================
void mouth_detect_free(void *md_work_mem);

//========================================================
// Function Name :  mouth_detect_proc
// Syntax : 
// Purpose :  mouth detect process
// Parameters : void *md_work_mem: mouth detect work memory
//				gpImage *face_img: face detect image
//				void *pre_result: previous result
// Return : none
//========================================================
void mouth_detect_proc(void *md_work_mem, gpImage *face_img, void *pre_result);

//========================================================
// Function Name :  mouth_detect_get_result
// Syntax : 
// Purpose :  mouth detect get result
// Parameters : void *md_work_mem: mouth detect work memory
//				void *pre_result: result
// Return : none
//========================================================
void mouth_detect_get_result(void *md_work_mem, void *result);

// eyes
//========================================================
// Function Name :  eye_detect_init
// Syntax : 
// Purpose :  eye detect init
// Parameters : EyeDetect_t *edWorkMem: eye detect work memory
// Return : none
//========================================================
void eye_detect_init(EyeDetect_t *edWorkMem);

//========================================================
// Function Name :  eye_detect_reset
// Syntax : 
// Purpose :  eye detect reset
// Parameters : void *para: eye detect work memory
// Return : none
//========================================================
void eye_detect_reset(void *para);

//========================================================
// Function Name :  eye_detect_alloc
// Syntax : 
// Purpose :  eye detect memory alloc
// Parameters : 
// Return : !=0 is success, ==0 is fail
//========================================================
INT32S eye_detect_alloc(void);

//========================================================
// Function Name :  eye_detect_free
// Syntax : 
// Purpose :  eye detect free memory
// Parameters : void *ed_work_mem: eye detect work memory
// Return : none
//========================================================
void eye_detect_free(void *ed_work_mem);

//========================================================
// Function Name :  eye_detect_proc
// Syntax : 
// Purpose :  eye detect process
// Parameters : void *ed_work_mem: eye detect work memory
//				gpImage *org_imgimg: orignal image
//				void *pre_result: previous result
// Return : none
//========================================================
void eye_detect_proc(void *ed_work_mem, gpImage *org_img, void *pre_result);

//========================================================
// Function Name :  eye_detect_get_result
// Syntax : 
// Purpose :  eye detect get result
// Parameters : void *ed_work_mem: eye detect work memory
//				void *pre_result:  result
// Return : none
//========================================================
void eye_detect_get_result(void *ed_work_mem, void *result);

// identify
//========================================================
// Function Name :  face_recognize_init
// Syntax : 
// Purpose :  face recognize init
// Parameters : FaceIdentify_t *idWorkMem: identify detect work memory
// Return : success/fail
//========================================================
INT32S face_recognize_init(FaceIdentify_t *idWorkMem);

//========================================================
// Function Name :  face_recognize_reset
// Syntax : 
// Purpose :  face recognize reset
// Parameters : void *para: identify detect work memory
// Return : none
//========================================================
void face_recognize_reset(void *para);

//========================================================
// Function Name :  face_recognize_alloc
// Syntax : 
// Purpose :  face recognize memory alloc
// Parameters : 
// Return : !=0 is success, ==0 is fail
//========================================================
INT32S face_recognize_alloc(void);

//========================================================
// Function Name :  face_recognize_free
// Syntax : 
// Purpose :  face recognize free memory
// Parameters : void *id_work_mem: identify detect work memory
// Return : none
//========================================================
void face_recognize_free(void *id_work_mem);

//========================================================
// Function Name :  face_recognize_proc
// Syntax : 
// Purpose :  face recognize process
// Parameters : void *id_work_mem: object identify work memory
//				gpImage *org_imgimg: orignal image
//				void *pre_result: previous result
// Return : none
//========================================================
void face_recognize_proc(void *id_work_mem, gpImage *org_img, void *pre_result);

//========================================================
// Function Name :  face_recognize_get_result
// Syntax : 
// Purpose :  face recognize get result
// Parameters : void *id_work_mem: identify detect work memory
//				void *pre_result:  result
// Return : none
//========================================================
void face_recognize_get_result(void *id_work_mem, void *result);

// gesture
//========================================================
// Function Name :  gesture_detect_proc
// Syntax : 
// Purpose :  gesture detect process
// Parameters : ObjDetect_t *odWorkMem: object detect work memory
// Return : none
//========================================================
void gesture_detect_proc(ObjDetect_t *odWorkMem);

// object detect
//========================================================
// Function Name :  obj_detect_init
// Syntax : 
// Purpose :  object detect init
// Parameters : ObjDetect_t *odWorkMem: object detect work memory
// Return : none
//========================================================
INT32S obj_detect_init(ObjDetect_t *odWorkMem);

//========================================================
// Function Name :  obj_detect_alloc
// Syntax : 
// Purpose :  object detect memory alloc
// Parameters : INT32S obj_type: object detect type
//				INT32S post_flag: 0 is disable, 1 is enable
//				void(*post_proc_reset)(void*para): post process reset funtion
//				INT32S (*post_proc_alloc)(void): post process memory allocate funtion
//				void (*post_proc_free)(void *para): post process memory free funtion
//				void (*post_proc_run)(void *para, gpImage *org_img, void *pre_result): post process run funtion
//				void(*post_proc_get_result)(void *para, void *result)): post process get result funtion
// Return : !=0 is success, ==0 is fail
//========================================================
INT32S obj_detect_alloc(INT32S obj_type, 
					INT32S post_flag, 
					void(*post_proc_reset)(void*para),
					INT32S (*post_proc_alloc)(void), 
					void (*post_proc_free)(void *para), 
					void (*post_proc_run)(void *para, gpImage *org_img, void *pre_result), 
					void(*post_proc_get_result)(void *para, void *result));				

//========================================================
// Function Name :  obj_detect_free
// Syntax : 
// Purpose :  object detect free memory
// Parameters : void *od_work_mem: object detect work memory
// Return : none
//========================================================
void obj_detect_free(ObjDetect_t *odWorkMem);

//========================================================
// Function Name :  obj_detect_proc
// Syntax : 
// Purpose :  object detect process
// Parameters : ObjDetect_t *odWorkMem: object detect work memory
// Return : none
//========================================================
void obj_detect_proc(ObjDetect_t *odWorkMem);

//========================================================
// Function Name :  obj_detect_get_result
// Syntax : 
// Purpose :  object detect get result
// Parameters : void *od_work_mem: object detect work memory
//				void *pre_result:  result
// Return : none
//========================================================
void obj_detect_get_result(void *od_work_mem, void *result);

//========================================================
// Function Name :  obj_detect_set_table
// Syntax : 
// Purpose :  object detect set detect table
// Parameters : void *data: table start point
//				INT32S stage:  parameter stage
//				INT32S num: table size
// Return : none
//========================================================
void obj_detect_set_table(void *data, INT32S stage, INT32S num);

// face detect
//========================================================
// Function Name :  face_detect_init
// Syntax : 
// Purpose :  face detect init
// Parameters : ObjDetect_t *odWorkMem: object detect work memory
// Return : none
//========================================================
INT32S face_detect_init(ObjDetect_t *odWorkMem);

//========================================================
// Function Name :  face_detect_alloc
// Syntax : 
// Purpose :  face detect memory alloc
// Parameters : INT32S obj_type: object detect type
//				INT32S post_flag: 0 is disable, 1 is enable
//				void(*post_proc_reset)(void*para): post process reset funtion
//				INT32S (*post_proc_alloc)(void): post process memory allocate funtion
//				void (*post_proc_free)(void *para): post process memory free funtion
//				void (*post_proc_run)(void *para, gpImage *org_img, void *pre_result): post process run funtion
//				void(*post_proc_get_result)(void *para, void *result)): post process get result funtion
// Return : !=0 is success, ==0 is fail
//========================================================
INT32S face_detect_alloc(INT32S obj_type, 
					INT32S post_flag, 
					void(*post_proc_reset)(void*para),
					INT32S (*post_proc_alloc)(void), 
					void (*post_proc_free)(void *para), 
					void (*post_proc_run)(void *para, gpImage *org_img, void *pre_result), 
					void(*post_proc_get_result)(void *para, void *result));				

//========================================================
// Function Name :  face_detect_free
// Syntax : 
// Purpose :  face detect free memory
// Parameters : void *od_work_mem: object detect work memory
// Return : none
//========================================================
void face_detect_free(ObjDetect_t *odWorkMem);

//========================================================
// Function Name :  face_detect_proc
// Syntax : 
// Purpose :  face detect process
// Parameters : ObjDetect_t *odWorkMem: object detect work memory
// Return : none
//========================================================
void face_detect_proc(ObjDetect_t *odWorkMem);

//object recognize
//========================================================
// Function Name :  obj_recognize_init
// Syntax : 
// Purpose :  object recognize init
// Parameters : ObjDetect_t *odWorkMem: object recognize work memory
// Return : success/fail
//========================================================
INT32S obj_recognize_init(ObjDetect_t *odWorkMem);

//========================================================
// Function Name :  obj_recognize_alloc
// Syntax : 
// Purpose :  object recognize memory alloc and init
// Parameters : none
// Return : =0, fail, > 0, success memory address
//========================================================
INT32S obj_recognize_alloc(void);

//========================================================
// Function Name :  obj_recognize_proc
// Syntax : 
// Purpose :  object recognize process
// Parameters : ObjDetect_t *odWorkMem: object recognize work memory
// Return : none
//========================================================
void obj_recognize_proc(ObjDetect_t *odWorkMem);

//========================================================
// Function Name :  obj_recognize_free
// Syntax : 
// Purpose :  object recognize memory free
// Parameters : ObjDetect_t *odWorkMem: object recognize work memory
// Return : none
//========================================================
void obj_recognize_free(ObjDetect_t *odWorkMem);

//========================================================
// Function Name :  obj_recognize_get_result
// Syntax : 
// Purpose :  object recognize get result
// Parameters : void *od_work_mem: object detect work memory
//				void *pre_result:  result
// Return : none
//========================================================
void obj_recognize_get_result(void *od_work_mem, void *objresult);
#endif
#endif