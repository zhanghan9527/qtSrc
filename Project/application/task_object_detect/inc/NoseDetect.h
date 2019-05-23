#ifndef __NOSE_DETECT_H__
#define __NOSE_DETECT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "define.h"

int NoseDetect(unsigned char *workmem, gpImage img, gpPoint left_eye, gpPoint right_eye, gpFeaturePoint *nose_pt);

#ifdef __cplusplus
}
#endif

#endif
