#ifndef __CSpotterSDKApi_H__
#define __CSpotterSDKApi_H__

#include "base_types.h"

// Error MSG
#define CSPOTTER_SUCCESS					(0)
#define CSPOTTER_ERR_SDKError				(-2000)
// Fatal error
#define CSPOTTER_ERR_IllegalHandle			(CSPOTTER_ERR_SDKError-1)
#define CSPOTTER_ERR_IllegalParam			(CSPOTTER_ERR_SDKError-2)
#define CSPOTTER_ERR_LeaveNoMemory			(CSPOTTER_ERR_SDKError-3)
#define CSPOTTER_ERR_InitializeFailed		(CSPOTTER_ERR_SDKError-4)
#define CSPOTTER_ERR_LoadDLLFailed			(CSPOTTER_ERR_SDKError-5)
#define CSPOTTER_ERR_LoadModelFailed		(CSPOTTER_ERR_SDKError-6)
#define CSPOTTER_ERR_LoadFuncFailed			(CSPOTTER_ERR_SDKError-7)
#define CSPOTTER_ERR_DecodeINIFailed		(CSPOTTER_ERR_SDKError-8)
#define CSPOTTER_ERR_SetNoParam				(CSPOTTER_ERR_SDKError-9)
#define CSPOTTER_ERR_ParseEINFailed			(CSPOTTER_ERR_SDKError-10)
#define CSPOTTER_ERR_OpenFileFailed			(CSPOTTER_ERR_SDKError-11)
#define CSPOTTER_ERR_CheckFileFailed		(CSPOTTER_ERR_SDKError-12)
#define CSPOTTER_ERR_OpenRecognizer			(CSPOTTER_ERR_SDKError-14)
// Normal error
#define CSPOTTER_ERR_NoUttrStart			(CSPOTTER_ERR_SDKError-50)
#define CSPOTTER_ERR_NoResult				(CSPOTTER_ERR_SDKError-51)
#define CSPOTTER_ERR_NeedMoreSample			(CSPOTTER_ERR_SDKError-52)
#define CSPOTTER_ERR_FeaBufEmpty			(CSPOTTER_ERR_SDKError-53)
#define CSPOTTER_ERR_UnknownError			(CSPOTTER_ERR_SDKError-99)
#define CSPOTTER_ERR_Expired				(CSPOTTER_ERR_SDKError-100)
#define CSPOTTER_ERR_AddToManySamples		(CSPOTTER_ERR_SDKError-104)
// after add VAD
#define CSPOTTER_Ninja_Rejected				(CSPOTTER_ERR_SDKError-200)
#define CSPOTTER_Ninja_TimeOut				(CSPOTTER_ERR_SDKError-201)
#define CSPOTTER_Ninja_NoVoiceDetect		(CSPOTTER_ERR_SDKError-202)

#if defined(__SYMBIAN32__)
	#ifdef CSpotterDll_EXPORTS
		#define CSPDLL_API EXPORT_C
	#endif
#elif defined(_WIN32)
	#ifdef CSpotterDll_EXPORTS
		#define CSPDLL_API __declspec(dllexport)
	#endif
#endif

#ifndef CSPDLL_API
#define CSPDLL_API
#endif


#ifdef __cplusplus
extern "C"{
#endif

// Purpose: Initiate a recognizer for recognition
// hCSpotter(IN): a handle of the recognizer
// hCModLoc(IN): model for recognize
// BYTE* hMemPool(IN/OUT): memory buffer for VR engine
// INT nMemSzie: memory buffer size
// Return: 0 if success, otherwise error code, if hMemPool is NULL return Memory usage
CSPDLL_API INT CSpotter_Initiallize(HANDLE* hCSpotter, BYTE* hCModLoc, BYTE* hMemPool, INT nMemSzie);

// Purpose: Destroy a recognizer (free resources)
// hCSpotter(IN): a handle of the recognizer
// Return: Success or error code
CSPDLL_API INT CSpotter_Release(HANDLE *hCSpotter);

// Purpose: Start recognizing.
// hCSpotter(IN): a handle of the recognizer
// Return: Success or error code
CSPDLL_API INT CSpotter_RecogStart(HANDLE hCSpotter);

// Purpose: Set VAD Enable.Default is Off.
//			Call this function after init CSpotter and before call CSpotter_AddSample()
// hCSpotter(IN): a handle of the recognizer
// Return: Success or error code
CSPDLL_API INT CSpotter_VAD_Enable(HANDLE hCSpotter, INT nEnable);

// Purpose: Transfer voice samples to the recognizer for recognizing.
//			Call this function after call CSpotter_RecogStart()
// hCSpotter(IN): a handle of the recognizer
// lpsSample(IN): the pointer of voice data buffer
// nNumSamples(IN): the number of voice data (a unit is a short, better add 512 samples in on call)
// Return: Commands ID (start with 0) if success. If scores of command is not good enough return CSPOTTER_Ninja_Rejected. If needs more wave sample to decide return CSPOTTER_ERR_NeedMoreSample.
// If VAD is on :
// Return CSPOTTER_Ninja_NoVoiceDetect while not detect any voice.
// After detect voice start to recognize and stop recognize if found no voice detect.
// Return commands ID (start with 0) if success. If scores of command is not good enough return CSPOTTER_Ninja_Rejected. If needs more wave sample to decide return CSPOTTER_ERR_NeedMoreSample.
// If Engine found no voice any more and not recognize any result return CSPOTTER_Ninja_TimeOut.
CSPDLL_API INT CSpotter_AddSample(HANDLE hCSpotter, SHORT *lpsSample, INT nNumSamples);

// Purpose: Set model rejection level
// hCSpotter(IN): a handle of the recognizer
// nRejectionLevel(IN): rejection Level
// Return: Success or error code
CSPDLL_API INT CSpotter_SetRejectionLevel(HANDLE hCSpotter, INT nRejectionLevel);

// Purpose: Set Cmd Add score
// hCSpotter(IN): a handle of the recognizer
// nWordIdx(IN): Cmd start by 0
// nAddScore(IN): Score
// Return: Success or error code
CSPDLL_API INT CSpotter_SetWordAddScore(HANDLE hCSpotter, INT nWordIdx , INT nAddScore );

#ifdef __cplusplus
}
#endif

#endif // __CSpotterSDKApi_H__
