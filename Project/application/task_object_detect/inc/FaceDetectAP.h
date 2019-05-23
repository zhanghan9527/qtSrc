#ifndef __FACE_DETECT_V2_AP_H__
#define __FACE_DETECT_V2_AP_H__

//**************************************************************************
// Face-Detect API Header File
// v2201 (131028)
//**************************************************************************

#include "define.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FDAP_IMG_SIZE_OVERFLOW	0x80000201
#define FDAP_MEM_ERR			0x80000202
#define FDAP_INVALID_WORKMEM	0x80000203
#define FDAP_PARAM_ERR			0x80000204
#define FDAP_OS_ERR				0x80000205
#define FDAP_WORKMEM_SIZE_ERR	0x80000206
#define FDAP_IMG_FMT_ERR		0x80000207


typedef enum {
	SCALE_LINEAR = 0,
	SCALE_NONLINEAR
} SCALE_METHOD;


/*
#define CASCADE_FACE				0
#define CASCADE_LEYE				1
#define CASCADE_REYE				2
#define CASCADE_HAND				3
#define CASCADE_HANDSTONE		4
#define CASCADE_HAND_LT45		5
#define CASCADE_HAND_RT45		6
#define CASCADE_USER				*/

typedef enum {
	OBJ_FACE = 0,
	OBJ_LEYE,
	OBJ_REYE,
	OBJ_HAND,
	OBJ_FIST,
	OBJ_HAND_LT45,
	OBJ_HAND_RT45,
	OBJ_USER
} OBJ_TYPE;



typedef struct {
	char *classify_name;
	void *data;

	int obj_type;

	int stage_of_classify;
	int num_of_classify;
	int size_of_classify;
} ClassifyData;


//========================================================
// Function Name :  FaceDetect_GetVersion
// Syntax : const char *FaceDetect_GetVersion(void);
// Purpose :  get the version of FaceDetect library
// Parameters : none
// Return : version string 
//========================================================
const char *FaceDetect_GetVersion(void);


//========================================================
// Function Name :  FaceDetect_Config
// Syntax : 
// Purpose :  config setting of FaceDetect
// Parameters : void *WorkMem: pointer to working memory
//	            int WorkMemSize: working memory size      
//	            int maxWidth: image width              
//	            int maxHeight: image height             
//	            int maxChannel: Y only: 1,  YUYV or UYUV: 2
//	            int maxCandidate: Max Num of detected face
//	            int xStep: x step (horizontal)
//	            int yStep: y step (vertical)
// Return : if(WorkMemSize==0)return size of working memory
//          if(WorkMemSize==size of working memory) return 0
//          else return FDAP_WORKMEM_SIZE_ERR;
//========================================================
int FaceDetect_Config(
	void *WorkMem,        // pointer to working memory
	int WorkMemSize,      // working memory size      
	int maxWidth,         // image width              
	int maxHeight,        // image height             
	int maxChannel,       // Y only: 1,  YUYV or UYUV: 2
	int maxCandidate,     // Max Num of detected face
	int xStep,            // x step (horizontal)
	int yStep);           // y step (vertical)



//========================================================
// Function Name :  FaceDetect_SetScale
// Syntax : 
// Purpose :  set scale factor & max/min window size
// Parameters : void *WorkMem: pointer to working memory
//             	long Scale: scale factor for image resize
//            	int minWnd: min face size (default is 24)
//            	int maxWnd: MAX(image_width, image_height)
//             int scale_method: linear/non-linear scale
// Return : 0: success
//          FDAP_INVALID_WORKMEM: Fail
//========================================================
int FaceDetect_SetScale(
	void *WorkMem,        // pointer to working memory
	long Scale,           // scale factor for image resize
	int minWnd,           // min face size (default is 24)
	int maxWnd, 
	int scale_method);          // MAX(image_width, image_height)



//========================================================
// Function Name :  FaceDetect_SetX
// Syntax : 
// Purpose :  set x step
// Parameters : void *_WorkMem: pointer to working memory
//	            int xStep: x step (horizontal)
//	            int xShift: no working, could be removed
// Return : 0: success
//          FDAP_INVALID_WORKMEM: Fail
//========================================================
int FaceDetect_SetX(
	void *_WorkMem,       // pointer to working memory
	int xStep,            // x step (horizontal)
	int xShift);          // no working, could be removed



//========================================================
// Function Name :  FaceDetect
// Syntax : 
// Purpose :  
// Parameters : void *WorkMem: pointer to working memory
//	            const gpImage *Image: pointer to image structure
//	            const gpRect *Rect: region of Image
//	            int MaxResult: max num for faces
//	            gpRect *Result: face result
//  	          int *Count: the score of face
// Return : FDAP_INVALID_WORKMEM: invalid working memory
//							  -1: invalid scaling setting
//							  -2: invalid scaling memory
//							  -3: invalid roi
//                           < 0: Fail
//                           = 0: no face
//                           > 0: num of face
//========================================================
int FaceDetect(
	void *WorkMem,        // pointer to working memory
	const gpImage *Image, // pointer to image structure
	const gpRect *Rect,   // region of Image
	int MaxResult,        // max num for faces
	gpRect *Result,       // face result
	int *Count);          // the score of face




//========================================================
// Function Name :  FaceDetect_set_ScalerFn
// Syntax : 
// Purpose :  set scaler function to Face Detect
// Parameters : void *_WorkMem: pointer to working memory  
//             	int (*scaler_start)(gpImage *srcImg, gpImage *dstImg): function pointer to scaler start
// 	            int (*scaler_end)(void): function pointer to scaler end
// 	            int (*scaler_clip)(gpImage *srcImg, gpImage *dstImg, gpRect *clip): function pointer to scaler clip
// Return : none
//========================================================
extern void FaceDetect_set_ScalerFn(
	void *_WorkMem,                                                     // pointer to working memory  
	int (*scaler_start)(gpImage *srcImg, gpImage *dstImg),              // function pointer to scaler start
	int (*scaler_end)(void),                                            // function pointer to scaler end
	int (*scaler_clip)(gpImage *srcImg, gpImage *dstImg, gpRect *clip)  // function pointer to scaler clip
);



//========================================================
// Function Name :  FaceDetect_set_angle
// Syntax : 
// Purpose :  set the angle of detected object
// Parameters : void *_WorkMem: pointer to working memory
//            	int angle: image angle, angle is clockwise from 0 -> 90 -> 180 -> 270 -> 0 (360)
// Return : none
//========================================================
void FaceDetect_set_angle(
	void *_WorkMem,        // pointer to working memory
	int angle);            // image angle, angle is clockwise from 0 -> 90 -> 180 -> 270 -> 0 (360)



//========================================================
// Function Name :  FaceDetect_set_detect_obj
// Syntax : 
// Purpose :  set the object type for detection
// Parameters : void *_WorkMem: pointer to working memory
//            	ClassifyData *clfdata): classify data of detected object
// Return : none
//========================================================
void FaceDetect_set_detect_obj(
	void *_WorkMem,       // pointer to working memory
	ClassifyData *clfdata);    // classify data of detected object


#ifdef __cplusplus
}
#endif

#endif // __FACE_DETECT_V2_H__
