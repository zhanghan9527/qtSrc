#ifndef __FACE_IDENTIFY_AP_H__
#define __FACE_IDENTIFY_AP_H__

/**************************************************************************/
// Face ID Head File
// v3008 (for sensor 0308)
/**************************************************************************/

#include "define.h"

#ifndef SZ_ULBP
#define SZ_ULBP        7168 // (IMAGE_SIZE<<1)
#endif

/**
* @brief Get Face ID AP version
* @return source version
*/
const char *FaceIdentify_GetVersion(void);

/**
* @brief Calculate needed memory
* @return needed memory size for face identification
*/
int FaceIdentify_MemCalc();

/**
* @brief Train user's feature
* @param[in] gray: captured image
* @param[in] ownerULBP: ulbp of the owner
* @param[in] train_i: index of the training data
* @param[in] fiMem: the allocated memory and the assigned memory address for this func
* @param[in] userROI[0], faceROI: detected face region
* @param[in] userROI[1], lEyeROI: detected left eye region
* @param[in] userROI[2], rEyeROI: detected right eye region
*/
void FaceIdentify_Train(const gpImage* gray, const gpRect* userROI, void* _ownerULBP, const int train_i, void* fiMem);

/**
* @brief Verify user's identity
* @param[in] gray: captured image
* @param[in] ownerULBP: ulbp of the owner
* @param[in] scoreThreshold: threshold of confidence score
* @param[in] fiMem: the allocated memory and the assigned memory address for this func
* @param[in] templ_num: template number
* @param[in] userROI[0], faceROI: detected face region
* @param[in] userROI[1], lEyeROI: detected left eye region
* @param[in] userROI[2], rEyeROI: detected right eye region
* @param[out] templ_index: template index
* @param[out] score: confidence score
* @return 1 if the user is classified as a genuine user, 0 if classified as imposter
*/
int FaceIdentify_Verify(gpImage* gray, const gpRect* userROI, void* ownerULBP, int scoreThreshold, void* fiMem, int templ_num, int* templ_index, int* score);

/**
* @brief Verify user's identity, for debug usage
* @param[in] gray: captured image
* @param[in] ownerULBP: ulbp of the owner
* @param[in] scoreThreshold: threshold of confidence score
* @param[in] fiMem: the allocated memory and the assigned memory address for this func
* @param[in] templ_num: template number
* @param[in] userROI[0], faceROI: detected face region
* @param[in] userROI[1], lEyeROI: detected left eye region
* @param[in] userROI[2], rEyeROI: detected right eye region
* @param[out] templ_index: template index
* @param[out] score: confidence score, an integer array
* @return 1 if the user is classified as a genuine user, 0 if classified as imposter
*/
int FaceIdentify_Verify_Debug(gpImage* gray, const gpRect* userROI, void* ownerULBP, int scoreThreshold, void* fiMem, int templ_num, int* templ_index, int* score);

/**
* @brief Update user's data
* @param[in, out] ownerULBP: ulbp of the owner
* @param[in] templ_num: template number
*/
void FaceIdentify_Update(void* ownerULBP, int templ_num);

#endif