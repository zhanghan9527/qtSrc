#ifndef __MOUTH_DETECT_H__
#define __MOUTH_DETECT_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "define.h"
#include "fixed.h"

#define FACTOR_SCALE	33554432	// FACTOR_BITS	25

#define UPRIGHT_FACE_IMG_WIDTH		RESIZE_FACE_IMG_WIDTH
#define	UPRIGHT_FACE_IMG_HEIGHT		(int)((RESIZE_FACE_IMG_HEIGHT) * 1.2)


int MouthDetect_get_work_mem_size(void);

void MouthDetect_init(unsigned char *workmem);


////////////////////////////////////////////////////////////////////////////////////
// @Mouth Detect function
// @Argument information
//
// @workmem: working memory for Mouth Detect
// @img: face image
// @nose_pt: the nose point (left & right)
// @eye_distance: the distance between left & right eye
// @mouth_pt: after detection, it has left, right, top, and bottom points of Mouth
//
// @return: 1 -> success
// @return: 0 -> fail
////////////////////////////////////////////////////////////////////////////////////
int MouthDetect(unsigned char *workmem, gpImage img, gpPoint *leye, gpPoint *reye, gpFeaturePoint *nose_pt, int eye_distance, gpFeaturePoint *mouth_pt);



////////////////////////////////////////////////////////////////////////////////////
// Get Mouth information
////////////////////////////////////////////////////////////////////////////////////

// Get the average width of mouth
int MouthDetect_get_mouth_width(unsigned char *workmem);

// return: 1 -> smile
int MouthDetect_is_smile(unsigned char *workmem);

// return the factor for mouth open
/*double*/FIXED_TYPE MouthDetect_get_mouth_open_factor(unsigned char *workmem);

// return the factor for mouth close
/*double*/FIXED_TYPE MouthDetect_get_mouth_close_factor(unsigned char *workmem);


const char *MouthDetect_GetVersion(void);


#ifdef __cplusplus
}
#endif

#endif



